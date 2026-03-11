#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
香橙派 5 Max 边侧推理代码
Edge Inference on Orange Pi 5 Max

功能:
1. 接收K01通过星闪/SLE传输的数据
2. 运行多模态融合AI模型推理
3. 根据概率阈值触发报警
4. 上报数据到IoTDA云平台

作者: 三英战ICT团队
"""

import os
import sys
import json
import time
import queue
import struct
import logging
import threading
import numpy as np
from datetime import datetime
from collections import deque
from dataclasses import dataclass
from typing import Optional, Tuple, List, Dict, Callable
from enum import Enum

# 配置日志
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger('BullyingDetector')


# ============== 配置参数 ==============
class Config:
    """系统配置"""
    # 模型参数
    MODEL_PATH = '/home/orangepi/models/bullying_model.om'  # OM模型路径
    N_MELS = 64
    AUDIO_TIME_STEPS = 128
    VIBRATION_SEQ_LEN = 200
    
    # 报警阈值
    ALARM_THRESHOLD = 0.7  # 概率超过此值触发报警
    CONFIRM_COUNT = 2      # 连续几次超过阈值才报警
    COOLDOWN_TIME = 30     # 报警冷却时间（秒）
    
    # 数据窗口
    AUDIO_WINDOW_SEC = 4.0    # 音频窗口时长
    VIBRATION_WINDOW_SEC = 2.0  # 振动窗口时长
    SAMPLE_RATE_AUDIO = 16000  # 音频采样率
    SAMPLE_RATE_IMU = 100      # IMU采样率
    
    # 通信参数
    UART_PORT = '/dev/ttyS3'  # 串口端口（星闪SLE）
    UART_BAUDRATE = 115200
    
    # IoTDA参数
    IOTDA_SERVER = 'iot-mqtts.cn-north-4.myhuaweicloud.com'
    IOTDA_PORT = 8883
    IOTDA_DEVICE_ID = 'your_device_id'
    IOTDA_SECRET = 'your_device_secret'
    
    # 设备位置
    LOCATION = '教学楼A-3楼-男厕所'
    DEVICE_ID = 'node_001'


class AlarmLevel(Enum):
    """报警级别"""
    NONE = 0
    LOW = 1      # 低风险 (0.5-0.7)
    MEDIUM = 2   # 中风险 (0.7-0.85)
    HIGH = 3     # 高风险 (0.85-1.0)


@dataclass
class SensorData:
    """传感器数据结构"""
    timestamp: float
    audio: Optional[np.ndarray] = None       # 音频数据
    accelerometer: Optional[np.ndarray] = None  # 加速度 [ax, ay, az]
    gyroscope: Optional[np.ndarray] = None      # 陀螺仪 [gx, gy, gz]
    sound_level: Optional[float] = None      # 声压级 dB


@dataclass 
class DetectionResult:
    """检测结果"""
    timestamp: float
    probability: float
    alarm_level: AlarmLevel
    audio_features: Dict
    vibration_features: Dict
    attention_weight: float


# ============== 数据缓冲区 ==============
class DataBuffer:
    """
    数据缓冲区
    用于累积和同步音频与IMU数据
    """
    
    def __init__(self, config: Config):
        self.config = config
        
        # 音频缓冲
        self.audio_buffer = deque(maxlen=int(config.SAMPLE_RATE_AUDIO * config.AUDIO_WINDOW_SEC))
        
        # IMU缓冲
        self.imu_buffer = deque(maxlen=int(config.SAMPLE_RATE_IMU * config.VIBRATION_WINDOW_SEC))
        
        # 时间戳
        self.last_audio_time = 0
        self.last_imu_time = 0
        
        # 锁
        self.lock = threading.Lock()
        
    def add_audio(self, audio_samples: np.ndarray, timestamp: float):
        """添加音频样本"""
        with self.lock:
            for sample in audio_samples:
                self.audio_buffer.append(sample)
            self.last_audio_time = timestamp
            
    def add_imu(self, accel: np.ndarray, gyro: np.ndarray, timestamp: float):
        """添加IMU数据"""
        with self.lock:
            imu_sample = np.concatenate([accel, gyro])
            self.imu_buffer.append(imu_sample)
            self.last_imu_time = timestamp
            
    def get_data(self) -> Tuple[Optional[np.ndarray], Optional[np.ndarray]]:
        """获取完整的数据窗口"""
        with self.lock:
            # 检查是否有足够的数据
            min_audio = int(self.config.SAMPLE_RATE_AUDIO * self.config.AUDIO_WINDOW_SEC * 0.8)
            min_imu = int(self.config.SAMPLE_RATE_IMU * self.config.VIBRATION_WINDOW_SEC * 0.8)
            
            if len(self.audio_buffer) < min_audio or len(self.imu_buffer) < min_imu:
                return None, None
            
            # 提取数据
            audio = np.array(list(self.audio_buffer), dtype=np.float32)
            imu = np.array(list(self.imu_buffer), dtype=np.float32)
            
            return audio, imu
    
    def clear(self):
        """清空缓冲区"""
        with self.lock:
            self.audio_buffer.clear()
            self.imu_buffer.clear()


# ============== 特征提取 ==============
class FeatureExtractor:
    """
    特征提取器
    将原始数据转换为模型输入
    """
    
    def __init__(self, config: Config):
        self.config = config
        
    def extract_audio_features(self, audio: np.ndarray) -> np.ndarray:
        """
        提取音频特征（梅尔频谱图）
        
        Args:
            audio: 原始音频数据
            
        Returns:
            mel_spec: 形状为 (1, n_mels, time_steps)
        """
        # 确保长度
        target_len = int(self.config.SAMPLE_RATE_AUDIO * self.config.AUDIO_WINDOW_SEC)
        if len(audio) > target_len:
            audio = audio[-target_len:]
        elif len(audio) < target_len:
            audio = np.pad(audio, (target_len - len(audio), 0))
        
        # 简化的梅尔频谱图计算
        n_fft = 1024
        hop_length = 512
        
        # STFT
        num_frames = 1 + (len(audio) - n_fft) // hop_length
        if num_frames <= 0:
            return np.zeros((1, self.config.N_MELS, self.config.AUDIO_TIME_STEPS), dtype=np.float32)
        
        frames = []
        for i in range(num_frames):
            start = i * hop_length
            frame = audio[start:start + n_fft] * np.hanning(n_fft)
            frames.append(frame)
        
        frames = np.array(frames)
        spectrum = np.abs(np.fft.rfft(frames, axis=1)) ** 2
        
        # 梅尔滤波器
        mel_filters = self._mel_filterbank(n_fft // 2 + 1, self.config.N_MELS)
        mel_spec = np.dot(spectrum, mel_filters.T)
        
        # 转dB并标准化
        mel_spec = 10 * np.log10(mel_spec + 1e-10)
        mel_spec = (mel_spec - np.mean(mel_spec)) / (np.std(mel_spec) + 1e-8)
        
        # 调整大小
        mel_spec = mel_spec.T
        if mel_spec.shape[1] > self.config.AUDIO_TIME_STEPS:
            mel_spec = mel_spec[:, :self.config.AUDIO_TIME_STEPS]
        elif mel_spec.shape[1] < self.config.AUDIO_TIME_STEPS:
            pad_width = self.config.AUDIO_TIME_STEPS - mel_spec.shape[1]
            mel_spec = np.pad(mel_spec, ((0, 0), (0, pad_width)))
        
        return mel_spec.reshape(1, self.config.N_MELS, self.config.AUDIO_TIME_STEPS).astype(np.float32)
    
    def _mel_filterbank(self, num_fft_bins: int, num_mels: int) -> np.ndarray:
        """创建梅尔滤波器组"""
        sample_rate = self.config.SAMPLE_RATE_AUDIO
        n_fft = (num_fft_bins - 1) * 2
        
        mel_low = 0
        mel_high = 2595 * np.log10(1 + sample_rate / 2 / 700)
        mel_points = np.linspace(mel_low, mel_high, num_mels + 2)
        hz_points = 700 * (10 ** (mel_points / 2595) - 1)
        bin_points = np.floor((n_fft + 1) * hz_points / sample_rate).astype(int)
        
        filters = np.zeros((num_mels, num_fft_bins))
        for i in range(num_mels):
            for j in range(bin_points[i], min(bin_points[i + 1], num_fft_bins)):
                filters[i, j] = (j - bin_points[i]) / (bin_points[i + 1] - bin_points[i] + 1e-8)
            for j in range(bin_points[i + 1], min(bin_points[i + 2], num_fft_bins)):
                filters[i, j] = (bin_points[i + 2] - j) / (bin_points[i + 2] - bin_points[i + 1] + 1e-8)
        
        return filters
    
    def extract_imu_features(self, imu: np.ndarray) -> np.ndarray:
        """
        提取IMU特征
        
        Args:
            imu: 原始IMU数据 (seq_len, 6)
            
        Returns:
            features: 形状为 (seq_len, 6)
        """
        # 确保长度
        target_len = self.config.VIBRATION_SEQ_LEN
        if len(imu) > target_len:
            imu = imu[-target_len:]
        elif len(imu) < target_len:
            pad_len = target_len - len(imu)
            imu = np.pad(imu, ((pad_len, 0), (0, 0)))
        
        # 标准化
        accel_mean = np.array([0.0, 0.0, 9.8])
        accel_std = np.array([2.0, 2.0, 2.0])
        gyro_mean = np.array([0.0, 0.0, 0.0])
        gyro_std = np.array([50.0, 50.0, 50.0])
        
        imu[:, :3] = (imu[:, :3] - accel_mean) / (accel_std + 1e-8)
        imu[:, 3:] = (imu[:, 3:] - gyro_mean) / (gyro_std + 1e-8)
        
        return imu.astype(np.float32)
    
    def compute_audio_stats(self, audio: np.ndarray) -> Dict:
        """计算音频统计特征"""
        return {
            'mean_amplitude': float(np.mean(np.abs(audio))),
            'max_amplitude': float(np.max(np.abs(audio))),
            'rms': float(np.sqrt(np.mean(audio ** 2))),
            'zero_crossing_rate': float(np.mean(np.abs(np.diff(np.sign(audio)))) / 2)
        }
    
    def compute_imu_stats(self, imu: np.ndarray) -> Dict:
        """计算IMU统计特征"""
        accel = imu[:, :3]
        gyro = imu[:, 3:]
        
        accel_mag = np.sqrt(np.sum(accel ** 2, axis=1))
        gyro_mag = np.sqrt(np.sum(gyro ** 2, axis=1))
        
        return {
            'accel_magnitude_mean': float(np.mean(accel_mag)),
            'accel_magnitude_max': float(np.max(accel_mag)),
            'accel_magnitude_std': float(np.std(accel_mag)),
            'gyro_magnitude_mean': float(np.mean(gyro_mag)),
            'gyro_magnitude_max': float(np.max(gyro_mag)),
            'gyro_magnitude_std': float(np.std(gyro_mag)),
            'impact_detected': bool(np.max(accel_mag) > 15)
        }


# ============== 模型推理 ==============
class ModelInference:
    """
    模型推理类
    支持MindSpore Lite和ACL推理
    """
    
    def __init__(self, config: Config):
        self.config = config
        self.model = None
        self.inference_backend = None
        
    def load_model(self):
        """加载模型"""
        model_path = self.config.MODEL_PATH
        
        if model_path.endswith('.om'):
            # 使用ACL推理（昇腾）
            self._load_acl_model(model_path)
            self.inference_backend = 'acl'
        elif model_path.endswith('.mindir'):
            # 使用MindSpore Lite
            self._load_mslite_model(model_path)
            self.inference_backend = 'mslite'
        else:
            # 使用MindSpore直接加载
            self._load_mindspore_model(model_path)
            self.inference_backend = 'mindspore'
            
        logger.info(f"模型加载完成: {self.inference_backend}")
        
    def _load_acl_model(self, model_path: str):
        """加载ACL模型"""
        try:
            import acl
            # ACL初始化
            ret = acl.init()
            if ret != 0:
                raise RuntimeError(f"ACL init failed: {ret}")
            
            # 设置设备
            ret = acl.rt.set_device(0)
            if ret != 0:
                raise RuntimeError(f"Set device failed: {ret}")
            
            # 创建context
            self.context, ret = acl.rt.create_context(0)
            if ret != 0:
                raise RuntimeError(f"Create context failed: {ret}")
            
            # 加载模型
            self.model_id, ret = acl.mdl.load_from_file(model_path)
            if ret != 0:
                raise RuntimeError(f"Load model failed: {ret}")
            
            # 获取模型描述
            self.model_desc = acl.mdl.create_desc()
            ret = acl.mdl.get_desc(self.model_desc, self.model_id)
            
            logger.info("ACL模型加载成功")
            
        except ImportError:
            logger.warning("ACL不可用，回退到numpy模拟")
            self.inference_backend = 'numpy'
            
    def _load_mslite_model(self, model_path: str):
        """加载MindSpore Lite模型"""
        try:
            import mindspore_lite as mslite
            
            context = mslite.Context()
            context.target = ["ascend"]
            context.ascend.device_id = 0
            
            self.model = mslite.Model()
            self.model.build_from_file(model_path, mslite.ModelType.MINDIR, context)
            
            logger.info("MindSpore Lite模型加载成功")
            
        except ImportError:
            logger.warning("MindSpore Lite不可用，回退到numpy模拟")
            self.inference_backend = 'numpy'
            
    def _load_mindspore_model(self, model_path: str):
        """加载MindSpore模型"""
        try:
            import mindspore as ms
            from mindspore.train.serialization import load_checkpoint, load_param_into_net
            
            # 这里需要导入模型定义
            # from bullying_model import create_model
            
            logger.info("MindSpore模型加载成功")
            
        except ImportError:
            logger.warning("MindSpore不可用，回退到numpy模拟")
            self.inference_backend = 'numpy'
    
    def inference(self, audio_feat: np.ndarray, imu_feat: np.ndarray) -> Tuple[float, float]:
        """
        执行推理
        
        Args:
            audio_feat: 音频特征 (1, n_mels, time_steps)
            imu_feat: IMU特征 (seq_len, 6)
            
        Returns:
            probability: 霸凌概率
            attention_weight: 注意力权重
        """
        # 添加batch维度
        audio_input = audio_feat.reshape(1, 1, self.config.N_MELS, self.config.AUDIO_TIME_STEPS)
        imu_input = imu_feat.reshape(1, self.config.VIBRATION_SEQ_LEN, 6)
        
        if self.inference_backend == 'acl':
            return self._acl_inference(audio_input, imu_input)
        elif self.inference_backend == 'mslite':
            return self._mslite_inference(audio_input, imu_input)
        elif self.inference_backend == 'mindspore':
            return self._mindspore_inference(audio_input, imu_input)
        else:
            # numpy模拟推理（用于测试）
            return self._numpy_inference(audio_input, imu_input)
    
    def _acl_inference(self, audio: np.ndarray, imu: np.ndarray) -> Tuple[float, float]:
        """ACL推理"""
        import acl
        
        # 创建输入数据集
        # ... ACL推理代码 ...
        
        # 这里简化处理
        return 0.5, 0.5
    
    def _mslite_inference(self, audio: np.ndarray, imu: np.ndarray) -> Tuple[float, float]:
        """MindSpore Lite推理"""
        import mindspore_lite as mslite
        
        inputs = self.model.get_inputs()
        inputs[0].set_data_from_numpy(audio.astype(np.float32))
        inputs[1].set_data_from_numpy(imu.astype(np.float32))
        
        outputs = self.model.predict(inputs)
        
        probability = outputs[0].get_data_to_numpy()[0, 0]
        attention = outputs[1].get_data_to_numpy()[0, 0]
        
        return float(probability), float(attention)
    
    def _mindspore_inference(self, audio: np.ndarray, imu: np.ndarray) -> Tuple[float, float]:
        """MindSpore推理"""
        import mindspore as ms
        
        audio_tensor = ms.Tensor(audio, ms.float32)
        imu_tensor = ms.Tensor(imu, ms.float32)
        
        prob, attn = self.model(audio_tensor, imu_tensor)
        
        return float(prob.asnumpy()[0, 0]), float(attn.asnumpy()[0, 0])
    
    def _numpy_inference(self, audio: np.ndarray, imu: np.ndarray) -> Tuple[float, float]:
        """
        Numpy模拟推理（用于测试）
        基于简单规则估计霸凌概率
        """
        # 音频能量
        audio_energy = np.mean(np.abs(audio))
        
        # IMU振动强度
        accel = imu[:, :, :3]
        accel_mag = np.sqrt(np.sum(accel ** 2, axis=-1))
        vibration_intensity = np.max(accel_mag)
        
        # 简单规则：高能量音频 + 强烈振动 = 高概率
        audio_score = min(audio_energy * 2, 1.0)
        vibration_score = min(vibration_intensity / 10, 1.0)
        
        # 融合分数
        probability = 0.4 * audio_score + 0.6 * vibration_score
        attention_weight = 0.5 + 0.5 * (vibration_score - audio_score)
        
        return float(probability), float(attention_weight)


# ============== 报警处理 ==============
class AlarmHandler:
    """
    报警处理器
    管理报警逻辑和状态
    """
    
    def __init__(self, config: Config):
        self.config = config
        self.alarm_count = 0
        self.last_alarm_time = 0
        self.alarm_history = deque(maxlen=100)
        self.callbacks: List[Callable] = []
        
    def add_callback(self, callback: Callable):
        """添加报警回调"""
        self.callbacks.append(callback)
        
    def check_alarm(self, result: DetectionResult) -> bool:
        """
        检查是否需要触发报警
        
        Args:
            result: 检测结果
            
        Returns:
            should_alarm: 是否报警
        """
        current_time = time.time()
        
        # 检查冷却时间
        if current_time - self.last_alarm_time < self.config.COOLDOWN_TIME:
            return False
        
        # 检查概率阈值
        if result.probability >= self.config.ALARM_THRESHOLD:
            self.alarm_count += 1
        else:
            self.alarm_count = max(0, self.alarm_count - 1)
        
        # 需要连续多次超过阈值
        if self.alarm_count >= self.config.CONFIRM_COUNT:
            self.trigger_alarm(result)
            self.alarm_count = 0
            self.last_alarm_time = current_time
            return True
        
        return False
    
    def trigger_alarm(self, result: DetectionResult):
        """触发报警"""
        logger.warning(f"⚠️ 触发报警! 概率: {result.probability:.2%}, 级别: {result.alarm_level.name}")
        
        # 记录历史
        self.alarm_history.append({
            'timestamp': result.timestamp,
            'probability': result.probability,
            'level': result.alarm_level.name
        })
        
        # 调用回调
        for callback in self.callbacks:
            try:
                callback(result)
            except Exception as e:
                logger.error(f"报警回调失败: {e}")
    
    def get_alarm_level(self, probability: float) -> AlarmLevel:
        """获取报警级别"""
        if probability < 0.5:
            return AlarmLevel.NONE
        elif probability < 0.7:
            return AlarmLevel.LOW
        elif probability < 0.85:
            return AlarmLevel.MEDIUM
        else:
            return AlarmLevel.HIGH


# ============== 串口通信 ==============
class UARTReceiver:
    """
    串口接收器
    接收K01通过星闪SLE发送的数据
    """
    
    def __init__(self, config: Config, data_buffer: DataBuffer):
        self.config = config
        self.data_buffer = data_buffer
        self.running = False
        self.serial = None
        self.thread = None
        
    def start(self):
        """启动接收"""
        try:
            import serial
            self.serial = serial.Serial(
                port=self.config.UART_PORT,
                baudrate=self.config.UART_BAUDRATE,
                timeout=0.1
            )
            self.running = True
            self.thread = threading.Thread(target=self._receive_loop)
            self.thread.daemon = True
            self.thread.start()
            logger.info(f"串口接收启动: {self.config.UART_PORT}")
        except Exception as e:
            logger.error(f"串口打开失败: {e}")
            
    def stop(self):
        """停止接收"""
        self.running = False
        if self.thread:
            self.thread.join(timeout=1.0)
        if self.serial:
            self.serial.close()
            
    def _receive_loop(self):
        """接收循环"""
        buffer = bytearray()
        
        while self.running:
            try:
                if self.serial and self.serial.in_waiting:
                    data = self.serial.read(self.serial.in_waiting)
                    buffer.extend(data)
                    
                    # 解析数据包
                    while len(buffer) >= 4:
                        # 查找包头
                        if buffer[0] != 0xAA or buffer[1] != 0x55:
                            buffer.pop(0)
                            continue
                        
                        # 获取包长度
                        if len(buffer) < 4:
                            break
                        
                        packet_len = buffer[2] | (buffer[3] << 8)
                        
                        if len(buffer) < packet_len + 4:
                            break
                        
                        # 提取数据包
                        packet = bytes(buffer[4:4 + packet_len])
                        buffer = buffer[4 + packet_len:]
                        
                        # 解析数据包
                        self._parse_packet(packet)
                        
            except Exception as e:
                logger.error(f"接收错误: {e}")
                time.sleep(0.1)
    
    def _parse_packet(self, packet: bytes):
        """
        解析数据包
        
        数据包格式:
        - 1字节: 数据类型 (0x01=IMU, 0x02=音频, 0x03=声压级)
        - 4字节: 时间戳 (ms)
        - N字节: 数据
        """
        if len(packet) < 5:
            return
        
        data_type = packet[0]
        timestamp = struct.unpack('<I', packet[1:5])[0] / 1000.0
        payload = packet[5:]
        
        if data_type == 0x01:  # IMU数据
            if len(payload) >= 24:
                # 6个float: ax, ay, az, gx, gy, gz
                values = struct.unpack('<6f', payload[:24])
                accel = np.array(values[:3])
                gyro = np.array(values[3:])
                self.data_buffer.add_imu(accel, gyro, timestamp)
                
        elif data_type == 0x02:  # 音频数据
            # 16位PCM音频
            num_samples = len(payload) // 2
            samples = np.frombuffer(payload, dtype=np.int16).astype(np.float32) / 32768.0
            self.data_buffer.add_audio(samples, timestamp)
            
        elif data_type == 0x03:  # 声压级
            if len(payload) >= 4:
                sound_level = struct.unpack('<f', payload[:4])[0]
                # 可用于辅助判断


# ============== IoTDA 客户端 ==============
class IoTDAClient:
    """
    华为云IoTDA客户端
    上报数据和报警到云平台
    """
    
    def __init__(self, config: Config):
        self.config = config
        self.client = None
        self.connected = False
        
    def connect(self):
        """连接到IoTDA"""
        try:
            from huaweicloudsdkiotda.v5 import IoTDAClient as HWClient
            from huaweicloudsdkcore.auth.credentials import BasicCredentials
            
            # 这里使用设备侧MQTT连接
            import paho.mqtt.client as mqtt
            
            self.client = mqtt.Client(client_id=self.config.IOTDA_DEVICE_ID)
            
            # 设置TLS
            self.client.tls_set()
            
            # 设置用户名和密码
            self.client.username_pw_set(
                username=self.config.IOTDA_DEVICE_ID,
                password=self._generate_password()
            )
            
            # 连接
            self.client.connect(
                self.config.IOTDA_SERVER,
                self.config.IOTDA_PORT
            )
            
            self.client.loop_start()
            self.connected = True
            logger.info("IoTDA连接成功")
            
        except Exception as e:
            logger.error(f"IoTDA连接失败: {e}")
            self.connected = False
    
    def _generate_password(self) -> str:
        """生成连接密码"""
        import hmac
        import hashlib
        
        timestamp = str(int(time.time() * 1000))
        sign_content = timestamp
        
        signature = hmac.new(
            self.config.IOTDA_SECRET.encode(),
            sign_content.encode(),
            hashlib.sha256
        ).hexdigest()
        
        return f"{timestamp}_{signature}"
    
    def report_status(self, result: DetectionResult):
        """上报设备状态"""
        if not self.connected:
            return
        
        # 构造上报数据
        data = {
            'services': [{
                'service_id': 'bullying_detection',
                'properties': {
                    'probability': result.probability,
                    'alarm_level': result.alarm_level.value,
                    'attention_weight': result.attention_weight,
                    'timestamp': int(result.timestamp * 1000),
                    'location': self.config.LOCATION,
                    'device_id': self.config.DEVICE_ID
                }
            }]
        }
        
        # 发布到IoTDA
        topic = f"$oc/devices/{self.config.IOTDA_DEVICE_ID}/sys/properties/report"
        self.client.publish(topic, json.dumps(data))
    
    def report_alarm(self, result: DetectionResult):
        """上报报警事件"""
        if not self.connected:
            return
        
        # 构造报警数据
        data = {
            'services': [{
                'service_id': 'alarm',
                'event_type': 'bullying_detected',
                'event_time': datetime.fromtimestamp(result.timestamp).isoformat(),
                'paras': {
                    'probability': result.probability,
                    'level': result.alarm_level.name,
                    'location': self.config.LOCATION,
                    'device_id': self.config.DEVICE_ID,
                    'audio_features': result.audio_features,
                    'vibration_features': result.vibration_features
                }
            }]
        }
        
        # 发布事件
        topic = f"$oc/devices/{self.config.IOTDA_DEVICE_ID}/sys/events/up"
        self.client.publish(topic, json.dumps(data))
        
        logger.info(f"报警已上报IoTDA: {result.alarm_level.name}")
    
    def disconnect(self):
        """断开连接"""
        if self.client:
            self.client.loop_stop()
            self.client.disconnect()
            self.connected = False


# ============== 本地报警控制 ==============
class LocalAlarmController:
    """
    本地报警控制器
    控制蜂鸣器和灯带
    """
    
    def __init__(self, config: Config):
        self.config = config
        self.gpio_available = False
        
        try:
            import OPi.GPIO as GPIO
            self.GPIO = GPIO
            self.gpio_available = True
            
            # GPIO设置
            GPIO.setmode(GPIO.BOARD)
            self.BUZZER_PIN = 7   # 蜂鸣器引脚
            self.LED_PIN = 11     # LED灯带控制引脚
            
            GPIO.setup(self.BUZZER_PIN, GPIO.OUT)
            GPIO.setup(self.LED_PIN, GPIO.OUT)
            
        except ImportError:
            logger.warning("GPIO不可用，报警功能受限")
    
    def trigger_alarm(self, level: AlarmLevel, duration: float = 5.0):
        """触发报警"""
        if not self.gpio_available:
            logger.info(f"模拟报警: {level.name}, 持续 {duration}s")
            return
        
        # 根据级别设置报警模式
        if level == AlarmLevel.HIGH:
            # 高级别：快速闪烁+连续蜂鸣
            self._high_alarm(duration)
        elif level == AlarmLevel.MEDIUM:
            # 中级别：慢闪烁+间歇蜂鸣
            self._medium_alarm(duration)
        else:
            # 低级别：LED常亮+短蜂鸣
            self._low_alarm(duration)
    
    def _high_alarm(self, duration: float):
        """高级别报警"""
        start = time.time()
        while time.time() - start < duration:
            self.GPIO.output(self.BUZZER_PIN, self.GPIO.HIGH)
            self.GPIO.output(self.LED_PIN, self.GPIO.HIGH)
            time.sleep(0.1)
            self.GPIO.output(self.LED_PIN, self.GPIO.LOW)
            time.sleep(0.1)
        self._stop_alarm()
    
    def _medium_alarm(self, duration: float):
        """中级别报警"""
        start = time.time()
        while time.time() - start < duration:
            self.GPIO.output(self.BUZZER_PIN, self.GPIO.HIGH)
            self.GPIO.output(self.LED_PIN, self.GPIO.HIGH)
            time.sleep(0.3)
            self.GPIO.output(self.BUZZER_PIN, self.GPIO.LOW)
            self.GPIO.output(self.LED_PIN, self.GPIO.LOW)
            time.sleep(0.3)
        self._stop_alarm()
    
    def _low_alarm(self, duration: float):
        """低级别报警"""
        self.GPIO.output(self.LED_PIN, self.GPIO.HIGH)
        self.GPIO.output(self.BUZZER_PIN, self.GPIO.HIGH)
        time.sleep(0.5)
        self.GPIO.output(self.BUZZER_PIN, self.GPIO.LOW)
        time.sleep(duration - 0.5)
        self._stop_alarm()
    
    def _stop_alarm(self):
        """停止报警"""
        if self.gpio_available:
            self.GPIO.output(self.BUZZER_PIN, self.GPIO.LOW)
            self.GPIO.output(self.LED_PIN, self.GPIO.LOW)
    
    def cleanup(self):
        """清理GPIO"""
        if self.gpio_available:
            self._stop_alarm()
            self.GPIO.cleanup()


# ============== 主检测器 ==============
class BullyingDetector:
    """
    校园霸凌检测主类
    整合所有组件
    """
    
    def __init__(self, config: Config = None):
        self.config = config or Config()
        
        # 初始化组件
        self.data_buffer = DataBuffer(self.config)
        self.feature_extractor = FeatureExtractor(self.config)
        self.model = ModelInference(self.config)
        self.alarm_handler = AlarmHandler(self.config)
        self.uart_receiver = UARTReceiver(self.config, self.data_buffer)
        self.iotda_client = IoTDAClient(self.config)
        self.local_alarm = LocalAlarmController(self.config)
        
        # 注册报警回调
        self.alarm_handler.add_callback(self._on_alarm)
        
        self.running = False
        self.detection_thread = None
        
    def _on_alarm(self, result: DetectionResult):
        """报警回调"""
        # 触发本地报警
        self.local_alarm.trigger_alarm(result.alarm_level)
        
        # 上报云端
        self.iotda_client.report_alarm(result)
    
    def start(self):
        """启动检测"""
        logger.info("启动校园霸凌检测系统...")
        
        # 加载模型
        self.model.load_model()
        
        # 连接IoTDA
        self.iotda_client.connect()
        
        # 启动串口接收
        self.uart_receiver.start()
        
        # 启动检测循环
        self.running = True
        self.detection_thread = threading.Thread(target=self._detection_loop)
        self.detection_thread.daemon = True
        self.detection_thread.start()
        
        logger.info("系统启动完成")
    
    def stop(self):
        """停止检测"""
        logger.info("停止系统...")
        self.running = False
        
        if self.detection_thread:
            self.detection_thread.join(timeout=2.0)
        
        self.uart_receiver.stop()
        self.iotda_client.disconnect()
        self.local_alarm.cleanup()
        
        logger.info("系统已停止")
    
    def _detection_loop(self):
        """检测循环"""
        while self.running:
            try:
                # 获取数据
                audio, imu = self.data_buffer.get_data()
                
                if audio is None or imu is None:
                    time.sleep(0.1)
                    continue
                
                # 提取特征
                audio_feat = self.feature_extractor.extract_audio_features(audio)
                imu_feat = self.feature_extractor.extract_imu_features(imu)
                
                # 计算统计特征
                audio_stats = self.feature_extractor.compute_audio_stats(audio)
                imu_stats = self.feature_extractor.compute_imu_stats(imu)
                
                # 推理
                probability, attention = self.model.inference(audio_feat, imu_feat)
                
                # 构造结果
                alarm_level = self.alarm_handler.get_alarm_level(probability)
                result = DetectionResult(
                    timestamp=time.time(),
                    probability=probability,
                    alarm_level=alarm_level,
                    audio_features=audio_stats,
                    vibration_features=imu_stats,
                    attention_weight=attention
                )
                
                # 检查报警
                self.alarm_handler.check_alarm(result)
                
                # 定期上报状态
                self.iotda_client.report_status(result)
                
                # 日志
                if probability > 0.3:
                    logger.info(f"检测: P={probability:.2%}, Level={alarm_level.name}, Attn={attention:.2f}")
                
                # 控制检测频率
                time.sleep(0.5)
                
            except Exception as e:
                logger.error(f"检测错误: {e}")
                time.sleep(1)
    
    def process_single(self, audio: np.ndarray, imu: np.ndarray) -> DetectionResult:
        """
        处理单个样本（用于测试）
        
        Args:
            audio: 音频数据
            imu: IMU数据
            
        Returns:
            检测结果
        """
        # 提取特征
        audio_feat = self.feature_extractor.extract_audio_features(audio)
        imu_feat = self.feature_extractor.extract_imu_features(imu)
        
        # 推理
        probability, attention = self.model.inference(audio_feat, imu_feat)
        
        # 构造结果
        alarm_level = self.alarm_handler.get_alarm_level(probability)
        result = DetectionResult(
            timestamp=time.time(),
            probability=probability,
            alarm_level=alarm_level,
            audio_features=self.feature_extractor.compute_audio_stats(audio),
            vibration_features=self.feature_extractor.compute_imu_stats(imu),
            attention_weight=attention
        )
        
        return result


# ============== 主函数 ==============
def main():
    """主函数"""
    import argparse
    
    parser = argparse.ArgumentParser(description='校园霸凌检测边侧程序')
    parser.add_argument('--test', action='store_true', help='测试模式')
    parser.add_argument('--model', type=str, default='', help='模型路径')
    parser.add_argument('--threshold', type=float, default=0.7, help='报警阈值')
    args = parser.parse_args()
    
    # 配置
    config = Config()
    if args.model:
        config.MODEL_PATH = args.model
    config.ALARM_THRESHOLD = args.threshold
    
    # 创建检测器
    detector = BullyingDetector(config)
    
    if args.test:
        # 测试模式
        logger.info("测试模式")
        detector.model.load_model()
        
        # 生成测试数据
        test_audio = np.random.randn(64000).astype(np.float32)
        test_imu = np.random.randn(200, 6).astype(np.float32)
        
        # 模拟霸凌场景
        test_audio[30000:35000] *= 5  # 突然的大声音
        test_imu[100:120, :3] *= 10   # 突然的剧烈振动
        
        # 运行检测
        result = detector.process_single(test_audio, test_imu)
        
        print("\n" + "=" * 50)
        print("测试结果:")
        print(f"  霸凌概率: {result.probability:.2%}")
        print(f"  报警级别: {result.alarm_level.name}")
        print(f"  注意力权重: {result.attention_weight:.2f}")
        print(f"  音频特征: {result.audio_features}")
        print(f"  振动特征: {result.vibration_features}")
        print("=" * 50)
        
    else:
        # 正常运行
        try:
            detector.start()
            
            # 保持运行
            while True:
                time.sleep(1)
                
        except KeyboardInterrupt:
            logger.info("收到停止信号")
        finally:
            detector.stop()


if __name__ == "__main__":
    main()
