#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
数据预处理和特征提取模块
Data Preprocessing and Feature Extraction Module

处理:
1. 音频数据 -> 梅尔频谱图
2. IMU数据 (陀螺仪+加速度计) -> 标准化序列

作者: 三英战ICT团队
"""

import numpy as np
import os
import json
from typing import Tuple, List, Optional, Dict
import warnings
warnings.filterwarnings('ignore')

# 尝试导入音频处理库
try:
    import librosa
    import librosa.display
    HAS_LIBROSA = True
except ImportError:
    HAS_LIBROSA = False
    print("Warning: librosa not installed. Using numpy-based audio processing.")

try:
    from scipy import signal
    from scipy.io import wavfile
    HAS_SCIPY = True
except ImportError:
    HAS_SCIPY = False


class AudioPreprocessor:
    """
    音频预处理器
    将原始音频转换为梅尔频谱图
    """
    
    def __init__(
        self,
        sample_rate: int = 16000,
        n_mels: int = 64,
        n_fft: int = 1024,
        hop_length: int = 512,
        duration: float = 4.0,  # 秒
        normalize: bool = True
    ):
        """
        Args:
            sample_rate: 采样率，默认16kHz
            n_mels: 梅尔频率bins数量
            n_fft: FFT窗口大小
            hop_length: 帧移
            duration: 音频片段持续时间
            normalize: 是否标准化
        """
        self.sample_rate = sample_rate
        self.n_mels = n_mels
        self.n_fft = n_fft
        self.hop_length = hop_length
        self.duration = duration
        self.normalize = normalize
        
        # 计算目标样本数和时间步数
        self.target_samples = int(sample_rate * duration)
        self.time_steps = int(np.ceil(self.target_samples / hop_length))
        
    def load_audio(self, file_path: str) -> np.ndarray:
        """加载音频文件"""
        if HAS_LIBROSA:
            audio, sr = librosa.load(file_path, sr=self.sample_rate, mono=True)
        elif HAS_SCIPY:
            sr, audio = wavfile.read(file_path)
            if len(audio.shape) > 1:
                audio = audio.mean(axis=1)
            if sr != self.sample_rate:
                # 简单重采样
                audio = signal.resample(audio, int(len(audio) * self.sample_rate / sr))
            audio = audio.astype(np.float32) / np.max(np.abs(audio))
        else:
            raise ImportError("需要安装librosa或scipy来处理音频")
        return audio
    
    def pad_or_truncate(self, audio: np.ndarray) -> np.ndarray:
        """将音频填充或截断到目标长度"""
        if len(audio) > self.target_samples:
            # 随机截取
            start = np.random.randint(0, len(audio) - self.target_samples)
            audio = audio[start:start + self.target_samples]
        elif len(audio) < self.target_samples:
            # 零填充
            padding = self.target_samples - len(audio)
            audio = np.pad(audio, (0, padding), mode='constant')
        return audio
    
    def extract_mel_spectrogram(self, audio: np.ndarray) -> np.ndarray:
        """提取梅尔频谱图"""
        if HAS_LIBROSA:
            # 使用librosa
            mel_spec = librosa.feature.melspectrogram(
                y=audio,
                sr=self.sample_rate,
                n_mels=self.n_mels,
                n_fft=self.n_fft,
                hop_length=self.hop_length,
                power=2.0
            )
            # 转换为分贝
            mel_spec_db = librosa.power_to_db(mel_spec, ref=np.max)
        else:
            # 使用numpy实现简化版本
            mel_spec_db = self._numpy_mel_spectrogram(audio)
        
        return mel_spec_db
    
    def _numpy_mel_spectrogram(self, audio: np.ndarray) -> np.ndarray:
        """使用numpy实现的简化版梅尔频谱图"""
        # STFT
        num_frames = 1 + (len(audio) - self.n_fft) // self.hop_length
        frames = np.zeros((num_frames, self.n_fft))
        
        for i in range(num_frames):
            start = i * self.hop_length
            frames[i] = audio[start:start + self.n_fft] * np.hanning(self.n_fft)
        
        # FFT
        spectrum = np.abs(np.fft.rfft(frames, axis=1)) ** 2
        
        # 简化的梅尔滤波器组
        mel_filters = self._create_mel_filterbank(self.n_fft // 2 + 1, self.n_mels)
        mel_spec = np.dot(spectrum, mel_filters.T)
        
        # 转dB
        mel_spec_db = 10 * np.log10(mel_spec + 1e-10)
        
        return mel_spec_db.T
    
    def _create_mel_filterbank(self, num_fft_bins: int, num_mels: int) -> np.ndarray:
        """创建梅尔滤波器组"""
        mel_low = 0
        mel_high = 2595 * np.log10(1 + self.sample_rate / 2 / 700)
        mel_points = np.linspace(mel_low, mel_high, num_mels + 2)
        hz_points = 700 * (10 ** (mel_points / 2595) - 1)
        bin_points = np.floor((self.n_fft + 1) * hz_points / self.sample_rate).astype(int)
        
        filters = np.zeros((num_mels, num_fft_bins))
        for i in range(num_mels):
            for j in range(bin_points[i], bin_points[i + 1]):
                filters[i, j] = (j - bin_points[i]) / (bin_points[i + 1] - bin_points[i])
            for j in range(bin_points[i + 1], bin_points[i + 2]):
                filters[i, j] = (bin_points[i + 2] - j) / (bin_points[i + 2] - bin_points[i + 1])
        
        return filters
    
    def normalize_spectrogram(self, mel_spec: np.ndarray) -> np.ndarray:
        """标准化频谱图"""
        mean = np.mean(mel_spec)
        std = np.std(mel_spec)
        mel_spec = (mel_spec - mean) / (std + 1e-8)
        return mel_spec
    
    def process(self, audio_input) -> np.ndarray:
        """
        处理音频输入
        
        Args:
            audio_input: 文件路径或numpy数组
            
        Returns:
            mel_spec: 形状为 (1, n_mels, time_steps) 的梅尔频谱图
        """
        # 加载音频
        if isinstance(audio_input, str):
            audio = self.load_audio(audio_input)
        else:
            audio = audio_input
        
        # 填充或截断
        audio = self.pad_or_truncate(audio)
        
        # 提取梅尔频谱图
        mel_spec = self.extract_mel_spectrogram(audio)
        
        # 标准化
        if self.normalize:
            mel_spec = self.normalize_spectrogram(mel_spec)
        
        # 调整到固定大小
        if mel_spec.shape[1] > self.time_steps:
            mel_spec = mel_spec[:, :self.time_steps]
        elif mel_spec.shape[1] < self.time_steps:
            padding = self.time_steps - mel_spec.shape[1]
            mel_spec = np.pad(mel_spec, ((0, 0), (0, padding)), mode='constant')
        
        # 添加通道维度 (1, n_mels, time_steps)
        mel_spec = np.expand_dims(mel_spec, axis=0)
        
        return mel_spec.astype(np.float32)


class IMUPreprocessor:
    """
    IMU数据预处理器
    处理陀螺仪和加速度计数据
    """
    
    def __init__(
        self,
        sample_rate: int = 100,  # IMU采样率 Hz
        duration: float = 2.0,    # 时间窗口 秒
        normalize: bool = True,
        use_filter: bool = True,
        filter_cutoff: float = 20.0  # 低通滤波截止频率
    ):
        """
        Args:
            sample_rate: IMU采样率
            duration: 数据窗口时长
            normalize: 是否标准化
            use_filter: 是否使用低通滤波
            filter_cutoff: 滤波截止频率
        """
        self.sample_rate = sample_rate
        self.duration = duration
        self.normalize = normalize
        self.use_filter = use_filter
        self.filter_cutoff = filter_cutoff
        
        # 计算目标序列长度
        self.seq_length = int(sample_rate * duration)
        
        # 数据统计参数（用于标准化）
        self.accel_mean = np.array([0.0, 0.0, 9.8])  # 加速度计默认值
        self.accel_std = np.array([2.0, 2.0, 2.0])
        self.gyro_mean = np.array([0.0, 0.0, 0.0])    # 陀螺仪默认值
        self.gyro_std = np.array([50.0, 50.0, 50.0])
        
    def low_pass_filter(self, data: np.ndarray, cutoff: float = None) -> np.ndarray:
        """低通滤波器"""
        if not HAS_SCIPY:
            return data
        
        cutoff = cutoff or self.filter_cutoff
        nyquist = self.sample_rate / 2
        normalized_cutoff = cutoff / nyquist
        
        if normalized_cutoff >= 1:
            return data
        
        b, a = signal.butter(4, normalized_cutoff, btype='low')
        filtered = signal.filtfilt(b, a, data, axis=0)
        return filtered
    
    def remove_gravity(self, accel_data: np.ndarray) -> np.ndarray:
        """去除重力分量（简化版）"""
        # 使用高通滤波去除低频重力分量
        if HAS_SCIPY:
            nyquist = self.sample_rate / 2
            cutoff = 0.3 / nyquist  # 0.3Hz 高通
            if cutoff < 1:
                b, a = signal.butter(2, cutoff, btype='high')
                return signal.filtfilt(b, a, accel_data, axis=0)
        return accel_data - np.mean(accel_data, axis=0)
    
    def extract_features(self, imu_data: np.ndarray) -> Dict[str, np.ndarray]:
        """
        提取IMU统计特征
        
        Args:
            imu_data: 形状为 (seq_len, 6) 的数据 [ax, ay, az, gx, gy, gz]
            
        Returns:
            特征字典
        """
        accel = imu_data[:, :3]
        gyro = imu_data[:, 3:]
        
        features = {}
        
        # 加速度特征
        features['accel_mean'] = np.mean(accel, axis=0)
        features['accel_std'] = np.std(accel, axis=0)
        features['accel_max'] = np.max(accel, axis=0)
        features['accel_min'] = np.min(accel, axis=0)
        features['accel_magnitude'] = np.sqrt(np.sum(accel**2, axis=1))
        features['accel_magnitude_mean'] = np.mean(features['accel_magnitude'])
        features['accel_magnitude_std'] = np.std(features['accel_magnitude'])
        
        # 陀螺仪特征
        features['gyro_mean'] = np.mean(gyro, axis=0)
        features['gyro_std'] = np.std(gyro, axis=0)
        features['gyro_max'] = np.max(gyro, axis=0)
        features['gyro_min'] = np.min(gyro, axis=0)
        features['gyro_magnitude'] = np.sqrt(np.sum(gyro**2, axis=1))
        features['gyro_magnitude_mean'] = np.mean(features['gyro_magnitude'])
        features['gyro_magnitude_std'] = np.std(features['gyro_magnitude'])
        
        # 动态特征 - 检测冲击
        accel_diff = np.diff(accel, axis=0)
        features['jerk_magnitude'] = np.sqrt(np.sum(accel_diff**2, axis=1))
        features['max_jerk'] = np.max(features['jerk_magnitude'])
        
        return features
    
    def detect_impact(self, imu_data: np.ndarray, threshold: float = 3.0) -> Tuple[bool, float]:
        """
        检测冲击事件
        
        Args:
            imu_data: IMU数据
            threshold: 冲击检测阈值（相对于标准差的倍数）
            
        Returns:
            is_impact: 是否检测到冲击
            impact_score: 冲击强度分数
        """
        accel = imu_data[:, :3]
        
        # 计算加速度幅值
        magnitude = np.sqrt(np.sum(accel**2, axis=1))
        
        # 计算基线
        baseline = np.median(magnitude)
        std = np.std(magnitude)
        
        # 检测异常峰值
        peaks = magnitude > (baseline + threshold * std)
        is_impact = np.any(peaks)
        
        # 计算冲击分数
        if is_impact:
            max_peak = np.max(magnitude)
            impact_score = (max_peak - baseline) / (std + 1e-8)
        else:
            impact_score = 0.0
        
        return is_impact, impact_score
    
    def normalize_data(self, imu_data: np.ndarray) -> np.ndarray:
        """标准化IMU数据"""
        accel = imu_data[:, :3]
        gyro = imu_data[:, 3:]
        
        # 标准化
        accel_norm = (accel - self.accel_mean) / (self.accel_std + 1e-8)
        gyro_norm = (gyro - self.gyro_mean) / (self.gyro_std + 1e-8)
        
        return np.concatenate([accel_norm, gyro_norm], axis=1)
    
    def pad_or_truncate(self, data: np.ndarray) -> np.ndarray:
        """调整序列长度"""
        if len(data) > self.seq_length:
            # 随机截取
            start = np.random.randint(0, len(data) - self.seq_length)
            data = data[start:start + self.seq_length]
        elif len(data) < self.seq_length:
            # 零填充
            padding = self.seq_length - len(data)
            data = np.pad(data, ((0, padding), (0, 0)), mode='constant')
        return data
    
    def process(self, imu_input) -> np.ndarray:
        """
        处理IMU输入
        
        Args:
            imu_input: 文件路径、numpy数组或字典
            
        Returns:
            processed_data: 形状为 (seq_length, 6) 的标准化序列
        """
        # 加载数据
        if isinstance(imu_input, str):
            if imu_input.endswith('.npy'):
                imu_data = np.load(imu_input)
            elif imu_input.endswith('.json'):
                with open(imu_input, 'r') as f:
                    data = json.load(f)
                imu_data = np.array(data['imu'])
            else:
                imu_data = np.loadtxt(imu_input, delimiter=',')
        elif isinstance(imu_input, dict):
            # 从字典构造
            accel = np.array([imu_input['ax'], imu_input['ay'], imu_input['az']]).T
            gyro = np.array([imu_input['gx'], imu_input['gy'], imu_input['gz']]).T
            imu_data = np.concatenate([accel, gyro], axis=1)
        else:
            imu_data = np.array(imu_input)
        
        # 确保形状正确
        if imu_data.shape[1] != 6:
            raise ValueError(f"IMU数据应有6列，但得到{imu_data.shape[1]}列")
        
        # 低通滤波
        if self.use_filter:
            imu_data = self.low_pass_filter(imu_data)
        
        # 调整长度
        imu_data = self.pad_or_truncate(imu_data)
        
        # 标准化
        if self.normalize:
            imu_data = self.normalize_data(imu_data)
        
        return imu_data.astype(np.float32)


class MultiModalDataset:
    """
    多模态数据集类
    管理音频和IMU数据的配对
    """
    
    def __init__(
        self,
        data_dir: str,
        audio_preprocessor: AudioPreprocessor = None,
        imu_preprocessor: IMUPreprocessor = None,
        labels_file: str = None
    ):
        """
        Args:
            data_dir: 数据目录
            audio_preprocessor: 音频预处理器
            imu_preprocessor: IMU预处理器
            labels_file: 标签文件路径
        """
        self.data_dir = data_dir
        self.audio_preprocessor = audio_preprocessor or AudioPreprocessor()
        self.imu_preprocessor = imu_preprocessor or IMUPreprocessor()
        
        self.samples = []
        self.labels = []
        
        if labels_file and os.path.exists(labels_file):
            self._load_labels(labels_file)
        
    def _load_labels(self, labels_file: str):
        """加载标签文件"""
        with open(labels_file, 'r') as f:
            data = json.load(f)
        
        for item in data:
            self.samples.append({
                'audio_path': os.path.join(self.data_dir, item['audio']),
                'imu_path': os.path.join(self.data_dir, item['imu']),
                'label': item['label']
            })
            self.labels.append(item['label'])
    
    def add_sample(self, audio_path: str, imu_path: str, label: int):
        """添加单个样本"""
        self.samples.append({
            'audio_path': audio_path,
            'imu_path': imu_path,
            'label': label
        })
        self.labels.append(label)
    
    def __len__(self) -> int:
        return len(self.samples)
    
    def __getitem__(self, idx: int) -> Tuple[np.ndarray, np.ndarray, int]:
        """获取单个样本"""
        sample = self.samples[idx]
        
        # 处理音频
        audio_feat = self.audio_preprocessor.process(sample['audio_path'])
        
        # 处理IMU
        imu_feat = self.imu_preprocessor.process(sample['imu_path'])
        
        label = sample['label']
        
        return audio_feat, imu_feat, label
    
    def get_batch(self, indices: List[int]) -> Tuple[np.ndarray, np.ndarray, np.ndarray]:
        """获取一批数据"""
        audio_batch = []
        imu_batch = []
        label_batch = []
        
        for idx in indices:
            audio, imu, label = self[idx]
            audio_batch.append(audio)
            imu_batch.append(imu)
            label_batch.append(label)
        
        return (
            np.stack(audio_batch),
            np.stack(imu_batch),
            np.array(label_batch, dtype=np.float32)
        )


def generate_synthetic_data(
    num_samples: int = 1000,
    bullying_ratio: float = 0.3,
    save_dir: str = None
) -> Tuple[np.ndarray, np.ndarray, np.ndarray]:
    """
    生成合成训练数据（用于演示和测试）
    
    Args:
        num_samples: 样本数量
        bullying_ratio: 霸凌事件比例
        save_dir: 保存目录
        
    Returns:
        audio_data: (num_samples, 1, 64, 128)
        imu_data: (num_samples, 200, 6)
        labels: (num_samples,)
    """
    print(f"生成 {num_samples} 个合成样本...")
    
    audio_data = []
    imu_data = []
    labels = []
    
    num_bullying = int(num_samples * bullying_ratio)
    
    for i in range(num_samples):
        is_bullying = i < num_bullying
        
        # 生成音频特征（梅尔频谱图）
        if is_bullying:
            # 霸凌场景：更多的高频成分，更大的能量波动
            audio = np.random.randn(64, 128) * 0.5
            # 添加尖叫/吼叫的特征（高频能量）
            audio[40:60, :] += np.random.randn(20, 128) * 2.0
            # 添加时间上的突变
            burst_start = np.random.randint(0, 100)
            audio[:, burst_start:burst_start+20] += np.random.randn(64, 20) * 1.5
        else:
            # 正常场景：平稳的背景噪声
            audio = np.random.randn(64, 128) * 0.3
            # 偶尔有一些正常的声音
            if np.random.random() > 0.5:
                audio[20:40, 50:70] += np.random.randn(20, 20) * 0.5
        
        audio = audio.reshape(1, 64, 128)
        
        # 生成IMU数据
        imu = np.zeros((200, 6))
        
        if is_bullying:
            # 霸凌场景：剧烈的振动和冲击
            # 基础振动
            imu[:, :3] = np.random.randn(200, 3) * 2.0  # 加速度
            imu[:, 3:] = np.random.randn(200, 3) * 30.0  # 陀螺仪
            
            # 添加冲击事件
            num_impacts = np.random.randint(2, 5)
            for _ in range(num_impacts):
                impact_time = np.random.randint(20, 180)
                impact_duration = np.random.randint(5, 15)
                impact_strength = np.random.uniform(5, 15)
                
                imu[impact_time:impact_time+impact_duration, :3] += \
                    np.random.randn(impact_duration, 3) * impact_strength
                imu[impact_time:impact_time+impact_duration, 3:] += \
                    np.random.randn(impact_duration, 3) * impact_strength * 10
        else:
            # 正常场景：轻微的背景振动
            imu[:, :3] = np.random.randn(200, 3) * 0.3 + np.array([0, 0, 9.8])
            imu[:, 3:] = np.random.randn(200, 3) * 5.0
        
        audio_data.append(audio)
        imu_data.append(imu)
        labels.append(1 if is_bullying else 0)
    
    # 打乱数据
    indices = np.random.permutation(num_samples)
    audio_data = np.array(audio_data)[indices].astype(np.float32)
    imu_data = np.array(imu_data)[indices].astype(np.float32)
    labels = np.array(labels)[indices].astype(np.float32)
    
    print(f"生成完成: 音频 {audio_data.shape}, IMU {imu_data.shape}, 标签 {labels.shape}")
    print(f"霸凌样本数: {int(labels.sum())}, 正常样本数: {len(labels) - int(labels.sum())}")
    
    if save_dir:
        os.makedirs(save_dir, exist_ok=True)
        np.save(os.path.join(save_dir, 'audio_data.npy'), audio_data)
        np.save(os.path.join(save_dir, 'imu_data.npy'), imu_data)
        np.save(os.path.join(save_dir, 'labels.npy'), labels)
        print(f"数据已保存到: {save_dir}")
    
    return audio_data, imu_data, labels


if __name__ == "__main__":
    # 测试数据生成
    audio_data, imu_data, labels = generate_synthetic_data(
        num_samples=100,
        bullying_ratio=0.3,
        save_dir='/tmp/test_data'
    )
    
    # 测试预处理器
    audio_preprocessor = AudioPreprocessor()
    imu_preprocessor = IMUPreprocessor()
    
    print("\n测试音频预处理...")
    test_audio = np.random.randn(16000 * 4).astype(np.float32)
    audio_feat = audio_preprocessor.process(test_audio)
    print(f"音频特征形状: {audio_feat.shape}")
    
    print("\n测试IMU预处理...")
    test_imu = np.random.randn(200, 6).astype(np.float32)
    imu_feat = imu_preprocessor.process(test_imu)
    print(f"IMU特征形状: {imu_feat.shape}")
    
    print("\n预处理测试完成!")
