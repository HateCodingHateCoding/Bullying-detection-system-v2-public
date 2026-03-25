#!/usr/bin/env python3
"""
main_pipeline.py - 欺凌检测主流水线

闭环链路:
  radar_receiver  (串口 → queue (5,64) float32 单帧)
  audio_receiver  (串口 → queue (1,64,128) float32 梅尔频谱单帧)
       ↓
  滑动窗口累积 T_FRAMES=4 帧 → radar (1,4,5,64)  audio (1,4,1,64,128)
       ↓
  AscendSentinel2 推理 (MindSpore / Ascend NPU)
  输出: main_logits, audio_logits, radar_logits, alpha
       ↓
  bullying 置信度 >= threshold
       ↓
  1. 串口声光报警 (/dev/ttyAMA0)
  2. OBS 上传音频证据
  3. IoTDA MQTT 告警上报
  4. HTTP POST → anq-server /alert/ → WebSocket → 鸿蒙 App

用法:
  python main_pipeline.py \\
      --radar-port /dev/ttyUSB0 \\
      --audio-port /dev/ttyUSB1 \\
      --alarm-port /dev/ttyAMA0 \\
      --model     ../models/sentinel_model.ckpt \\
      --config    ../config/device_key.json
"""

import os
import sys
import time
import queue
import json
import argparse
import threading
from collections import deque
from pathlib import Path

import numpy as np

# ── 接收器 ────────────────────────────────────────────────────────
import radar_receiver
import audio_receiver

# ── 报警（串口声光）──────────────────────────────────────────────
import serial

ALARM_CMD = bytes([0xAA, 0x55, 0x01, 0x1E])

# ── IoTDA + OBS（懒加载，首次告警时初始化）────────────────────────
try:
    from iot_uploader import HuaweiIoTUploader
    from obs_uploader import OBSUploader
    HAS_IOT = True
except ImportError as e:
    HAS_IOT = False
    print(f"[warn] IoT/OBS 模块不可用，将跳过云端上报: {e}")

# ── HTTP 推送（→ anq-server → WebSocket → App）──────────────────
try:
    import urllib.request as _urllib_req
    HAS_HTTP = True
except ImportError:
    HAS_HTTP = False


# ── 模型参数（与 notebook Cell 3/4 保持一致）─────────────────────
T_FRAMES   = 4     # 时序帧数
N_MELS     = 64    # 梅尔频带数
TIME_STEPS = 128   # 频谱时间步
RADAR_CH   = 5     # 雷达通道数
RADAR_PTS  = 64    # 每帧雷达点数


def _http_post_alert(server_url, location, time_str, status="NEW"):
    """POST /alert/ 到 anq-server，由服务端通过 WebSocket 广播给 App。"""
    if not HAS_HTTP or not server_url:
        return
    payload = json.dumps({
        "location": location,
        "time": time_str,
        "status": status,
    }, ensure_ascii=False).encode("utf-8")
    try:
        req = _urllib_req.Request(
            server_url,
            data=payload,
            headers={"Content-Type": "application/json"},
            method="POST",
        )
        with _urllib_req.urlopen(req, timeout=3) as resp:
            print(f"[http] POST {server_url} → {resp.status}")
    except Exception as e:
        print(f"[http] POST 失败: {e}")


def load_config(config_path):
    try:
        with open(config_path, encoding="utf-8") as f:
            return json.load(f)
    except Exception as e:
        print(f"[config] 凭证文件读取失败: {e}")
        return {}


def send_alarm(alarm_port, baud=115200):
    """通过串口触发声光报警模块"""
    try:
        ser = serial.Serial(
            port=alarm_port,
            baudrate=baud,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=1,
        )
        time.sleep(0.2)
        ser.write(ALARM_CMD)
        ser.flush()
        ser.close()
        print(f"[alarm] 已发送报警指令 → {alarm_port}: {ALARM_CMD.hex(' ')}")
    except Exception as e:
        print(f"[alarm] 报警发送失败: {e}")


# ── AI 推理栈 ────────────────────────────────────────────────────
try:
    import mindspore as ms
    import mindspore.nn as nn
    import mindspore.ops as ops
    from mindspore import Tensor, context
    HAS_MS = True
except Exception as e:
    HAS_MS = False
    print(f"[warn] MindSpore 不可用，将使用随机推理占位: {e}")


# ── AscendSentinel2 完整定义（与 notebook Cell 3 保持一致）────────
if HAS_MS:
    class _TNet(nn.Cell):
        def __init__(self, k=5):
            super().__init__()
            self.mlp = nn.SequentialCell([
                nn.Conv1d(k, 64, 1), nn.BatchNorm1d(64), nn.ReLU(),
                nn.Conv1d(64, 128, 1), nn.BatchNorm1d(128), nn.ReLU(),
                nn.Conv1d(128, 1024, 1), nn.BatchNorm1d(1024), nn.ReLU(),
            ])
            self.pool = nn.AdaptiveMaxPool1d(1)
            self.fc = nn.SequentialCell([
                nn.Dense(1024, 512), nn.ReLU(),
                nn.Dense(512, 256), nn.ReLU(),
                nn.Dense(256, k * k),
            ])
            self.k = k
            self._eye = ms.Tensor(np.eye(k, dtype=np.float32))

        def construct(self, x):
            B = x.shape[0]
            feat = self.pool(self.mlp(x)).squeeze(-1)
            mat = self.fc(feat).view(B, self.k, self.k)
            return mat + self._eye.unsqueeze(0)

    class _AudioMelCNNBranch(nn.Cell):
        def __init__(self):
            super().__init__()
            self.cnn = nn.SequentialCell([
                nn.Conv2d(1, 32, 3, pad_mode='same'), nn.BatchNorm2d(32), nn.ReLU(),
                nn.MaxPool2d(2, 2),
                nn.Conv2d(32, 64, 3, pad_mode='same'), nn.BatchNorm2d(64), nn.ReLU(),
                nn.MaxPool2d(2, 2),
                nn.Conv2d(64, 128, 3, pad_mode='same'), nn.BatchNorm2d(128), nn.ReLU(),
                nn.MaxPool2d(2, 2),
                nn.Conv2d(128, 256, 3, pad_mode='same'), nn.BatchNorm2d(256), nn.ReLU(),
            ])
            self.pool = nn.AdaptiveAvgPool2d((1, 1))
            self.flat = nn.Flatten()

        def construct(self, x):
            return self.flat(self.pool(self.cnn(x)))

    class _RadarPointNet(nn.Cell):
        def __init__(self):
            super().__init__()
            self.tnet = _TNet(k=5)
            self.mlp1 = nn.SequentialCell([
                nn.Conv1d(5, 64, 1), nn.BatchNorm1d(64), nn.ReLU(),
                nn.Conv1d(64, 64, 1), nn.BatchNorm1d(64), nn.ReLU(),
            ])
            self.mlp2 = nn.SequentialCell([
                nn.Conv1d(64, 128, 1), nn.BatchNorm1d(128), nn.ReLU(),
                nn.Conv1d(128, 256, 1), nn.BatchNorm1d(256), nn.ReLU(),
            ])
            self.pool = nn.AdaptiveMaxPool1d(1)
            self.flat = nn.Flatten()
            self.feat_fc = nn.SequentialCell([
                nn.Dense(256, 128), nn.BatchNorm1d(128), nn.ReLU(), nn.Dropout(p=0.3)
            ])
            self.radar_head = nn.SequentialCell([nn.Dense(128, 64), nn.ReLU(), nn.Dense(64, 2)])

        def construct(self, x):
            T_mat = self.tnet(x)
            x = ops.bmm(T_mat, x)
            x = self.mlp1(x)
            x = self.mlp2(x)
            feat = self.feat_fc(self.flat(self.pool(x)))
            return feat, self.radar_head(feat)

    class _MindCMA(nn.Cell):
        def __init__(self, audio_dim=256, radar_dim=128, unified=128):
            super().__init__()
            self.ap   = nn.SequentialCell([nn.Dense(audio_dim, unified), nn.Tanh()])
            self.rp   = nn.SequentialCell([nn.Dense(radar_dim,  unified), nn.Tanh()])
            self.attn = nn.SequentialCell([nn.Dense(unified * 2, 64), nn.ReLU(), nn.Dense(64, 1)])
            self.sig  = nn.Sigmoid()

        def construct(self, af, rf):
            ap = self.ap(af)
            rp = self.rp(rf)
            alpha = self.sig(self.attn(ops.concat([ap, rp], 1)))
            return alpha * ap + (1 - alpha) * rp, alpha

    class _GRUTemporalCtx(nn.Cell):
        def __init__(self, input_dim=128, hidden=64):
            super().__init__()
            self.gru = nn.GRU(input_size=input_dim, hidden_size=hidden, batch_first=True)

        def construct(self, x):
            out, _ = self.gru(x)
            return out[:, -1, :]

    class AscendSentinel2(nn.Cell):
        """多模态多帧霸凌检测网络 v3（与 notebook Cell 3 完全一致）"""
        def __init__(self, t_frames=T_FRAMES):
            super().__init__()
            self.T       = t_frames
            self.ab      = _AudioMelCNNBranch()
            self.rb      = _RadarPointNet()
            self.cma_seq = _MindCMA()
            self.cma_cls = _MindCMA()
            self.gru     = _GRUTemporalCtx(input_dim=128, hidden=64)
            self.audio_head = nn.SequentialCell([
                nn.Dense(256, 64), nn.ReLU(), nn.Dense(64, 2)
            ])
            self.clf = nn.SequentialCell([
                nn.Dense(576, 256), nn.BatchNorm1d(256), nn.ReLU(), nn.Dropout(p=0.5),
                nn.Dense(256, 128), nn.BatchNorm1d(128), nn.ReLU(), nn.Dropout(p=0.3),
                nn.Dense(128, 64), nn.ReLU(),
                nn.Dense(64, 2),
            ])

        def construct(self, audio, radar):
            # audio: (B, T, 1, 64, 128)   radar: (B, T, 5, 64)
            B, T = audio.shape[0], self.T
            a_flat = audio.view(B * T, 1, 64, 128)
            r_flat = radar.view(B * T, 5, 64)
            af_flat = self.ab(a_flat)                        # (B*T, 256)
            rf_flat, rl_flat = self.rb(r_flat)               # (B*T, 128), (B*T, 2)
            fused_flat, _ = self.cma_seq(af_flat, rf_flat)   # (B*T, 128)
            fused_seq = fused_flat.view(B, T, 128)
            tc = self.gru(fused_seq)                         # (B, 64)
            af_last = af_flat.view(B, T, 256)[:, T-1, :]    # (B, 256)
            rf_last = rf_flat.view(B, T, 128)[:, T-1, :]    # (B, 128)
            rl_last = rl_flat.view(B, T, 2)[:, T-1, :]      # (B, 2)
            fused_last, alpha = self.cma_cls(af_last, rf_last)  # (B,128),(B,1)
            feat = ops.concat([fused_last, tc, af_last, rf_last], 1)  # (B,576)
            main_logits  = self.clf(feat)
            audio_logits = self.audio_head(af_last)
            return main_logits, audio_logits, rl_last, alpha


def load_model(ckpt_path):
    """加载 AscendSentinel2 权重，不可用时返回 None（随机占位）。"""
    if not HAS_MS:
        print("[model] MindSpore 不可用，使用随机推理占位")
        return None
    try:
        context.set_context(mode=context.GRAPH_MODE, device_target="Ascend")
    except Exception:
        context.set_context(mode=context.PYNATIVE_MODE, device_target="CPU")
    net = AscendSentinel2(t_frames=T_FRAMES)
    net.set_train(False)
    if ckpt_path and Path(ckpt_path).exists():
        try:
            ms.load_checkpoint(ckpt_path, net)
            print(f"[model] 已加载权重: {ckpt_path}")
        except Exception as e:
            print(f"[model] 权重加载失败: {e}")
    else:
        print(f"[model] 权重文件不存在: {ckpt_path}，使用随机初始化")
    return net


def infer(net, radar_window, audio_window):
    """
    执行一次推理。

    Args:
        net:           AscendSentinel2 实例（或 None）
        radar_window:  list of (5,64) float32，长度 == T_FRAMES
        audio_window:  list of (1,64,128) float32，长度 == T_FRAMES

    Returns:
        (pred_class, confidence)  int, float
    """
    if net is None:
        # 随机占位，用于无 MindSpore 环境调试
        confidence = float(np.random.rand())
        return int(confidence > 0.5), confidence

    # 组装 (1, T, 5, 64) 和 (1, T, 1, 64, 128)
    r_np = np.stack(radar_window, axis=0)[np.newaxis].astype(np.float32)   # (1,T,5,64)
    a_np = np.stack(audio_window, axis=0)[np.newaxis].astype(np.float32)   # (1,T,1,64,128)

    r_t = Tensor(r_np)
    a_t = Tensor(a_np)

    main_logits, _, _, _ = net(a_t, r_t)
    probs = ms.ops.Softmax(axis=1)(main_logits).asnumpy()[0]  # (2,)
    pred_class = int(np.argmax(probs))
    confidence = float(probs[1])
    return pred_class, confidence


def parse_args():
    p = argparse.ArgumentParser(description="欺凌检测主流水线")
    p.add_argument("--radar-port",    default="/dev/ttyUSB0")
    p.add_argument("--radar-baud",    type=int, default=921600)
    p.add_argument("--audio-port",    default="/dev/ttyUSB1")
    p.add_argument("--audio-baud",    type=int, default=921600)
    p.add_argument("--alarm-port",    default="/dev/ttyAMA0")
    p.add_argument("--alarm-baud",    type=int, default=115200)
    p.add_argument("--model",         default="../models/sentinel_model.ckpt")
    p.add_argument("--config",        default="../config/device_key.json")
    p.add_argument("--threshold",     type=float, default=0.5)
    p.add_argument("--alarm-cooldown",type=float, default=30.0)
    p.add_argument("--alert-server",  default="",
                   help="anq-server HTTP 地址，如 http://192.168.1.100:8000/alert/")
    return p.parse_args()


def main():
    args = parse_args()
    cfg  = load_config(args.config)

    # ── 初始化云端模块 ────────────────────────────────────────────
    iot = None
    if HAS_IOT and cfg.get("iot_device_id"):
        iot = HuaweiIoTUploader(
            device_id=cfg["iot_device_id"],
            secret=cfg["iot_secret"],
            iot_platform_url=cfg["iot_platform_url"],
        )
        if not iot.connect():
            print("[main] IoTDA 连接失败，将跳过云端上报")
            iot = None
        else:
            print("[main] IoTDA 已连接")

    obs = None
    if HAS_IOT and cfg.get("obs_ak"):
        obs = OBSUploader(
            ak=cfg["obs_ak"],
            sk=cfg["obs_sk"],
            endpoint=cfg["obs_endpoint"],
            bucket_name=cfg["obs_bucket"],
        )
        print("[main] OBS 已初始化")

    # ── 加载模型 ──────────────────────────────────────────────────
    net = load_model(args.model)

    # ── 启动接收线程 ──────────────────────────────────────────────
    radar_q, radar_stop, radar_thread = radar_receiver.start(
        port=args.radar_port, baud=args.radar_baud
    )
    audio_q, audio_stop, audio_thread = audio_receiver.start(
        port=args.audio_port, baud=args.audio_baud
    )
    print("[main] 接收线程已启动，等待数据...")

    last_alarm_time = 0.0
    infer_count = 0

    # 滑动窗口：各保留最近 T_FRAMES 帧
    radar_window = deque(maxlen=T_FRAMES)   # 每帧 (5, 64)
    audio_window = deque(maxlen=T_FRAMES)   # 每帧 (1, 64, 128)

    try:
        while True:
            # 持续排空雷达队列，填充滑动窗口
            while True:
                try:
                    frame = radar_q.get_nowait()  # (5, 64)
                    radar_window.append(frame)
                except queue.Empty:
                    break

            # 以音频帧为主节拍（0.5秒一次）
            try:
                mel_frame = audio_q.get(timeout=5.0)  # (1, 64, 128)
            except queue.Empty:
                print("[main] 等待音频数据超时...")
                continue
            audio_window.append(mel_frame)

            # 等待两路窗口都凑满 T_FRAMES 帧
            if len(radar_window) < T_FRAMES or len(audio_window) < T_FRAMES:
                print(f"[main] 预热中: radar={len(radar_window)}/{T_FRAMES} "
                      f"audio={len(audio_window)}/{T_FRAMES}")
                continue

            # 推理
            t0 = time.perf_counter()
            pred_class, confidence = infer(
                net,
                list(radar_window),
                list(audio_window),
            )
            infer_ms = (time.perf_counter() - t0) * 1000
            infer_count += 1

            print(f"[infer] #{infer_count} class={pred_class} conf={confidence:.3f} "
                  f"({infer_ms:.1f}ms)")

            # 触发报警
            if pred_class == 1 and confidence >= args.threshold:
                now = time.time()
                if now - last_alarm_time >= args.alarm_cooldown:
                    print(f"[main] *** 检测到欺凌！置信度={confidence:.3f} → 触发报警 ***")

                    # 1. 串口声光报警
                    send_alarm(args.alarm_port, args.alarm_baud)

                    # 2. OBS 上传音频证据
                    evidence_url = ""
                    if obs is not None:
                        try:
                            # 将最新音频帧的 PCM 还原（近似，直接上传 mel 的 bytes）
                            audio_bytes = list(audio_window)[-1].tobytes()
                            evidence_url = obs.upload_audio_evidence(
                                audio_bytes, cfg.get("device_id", "edge-device")
                            ) or ""
                        except Exception as e:
                            print(f"[main] OBS 上传失败: {e}")

                    # 3. IoTDA 告警上报
                    if iot is not None:
                        try:
                            iot.upload_alarm(
                                alarm_type="bullying",
                                confidence=confidence,
                                location=cfg.get("location", "未知位置"),
                                audio_level=int(np.abs(list(audio_window)[-1]).mean() * 100),
                                radar_summary=f"T={T_FRAMES}帧窗口",
                                evidence_url=evidence_url,
                            )
                        except Exception as e:
                            print(f"[main] IoTDA 上报失败: {e}")

                    # 4. HTTP POST → anq-server → WebSocket → App
                    _http_post_alert(
                        server_url=args.alert_server,
                        location=cfg.get("location", "未知位置"),
                        time_str=time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(now)),
                        status="NEW",
                    )

                    last_alarm_time = now
                else:
                    remaining = args.alarm_cooldown - (now - last_alarm_time)
                    print(f"[main] 欺凌检测（置信度={confidence:.3f}），"
                          f"报警冷却中（剩余 {remaining:.1f}s）")

    except KeyboardInterrupt:
        print("\n[main] 用户中断，正在停止...")
    finally:
        radar_stop.set()
        audio_stop.set()
        radar_thread.join(timeout=3)
        audio_thread.join(timeout=3)
        if iot is not None:
            iot.disconnect()
        if obs is not None:
            obs.close()
        print("[main] 已退出")


if __name__ == "__main__":
    main()
