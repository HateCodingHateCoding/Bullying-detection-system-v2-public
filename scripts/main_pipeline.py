#!/usr/bin/env python3
"""
main_pipeline.py - 欺凌检测主流水线

闭环链路:
  radar_receiver  (串口 → queue (N,5) float32 原始点云)
  audio_receiver  (串口 → queue [1,1,16000] float32)
       ↓
  滑动窗口累积20帧雷达点云 → 合并采样64点 → [1,5,64]
       ↓
  AscendSentinelNet 推理 (MindSpore / Ascend NPU)
       ↓
  bullying 置信度 >= threshold
       ↓
  1. 串口声光报警 (alarm.py → /dev/ttyAMA0)
  2. OBS 上传音频证据
  3. HTTP POST → anq-server /alert/ → WebSocket 广播 → 鸿蒙 App

用法:
  python main_pipeline.py \
      --radar-port /dev/ttyUSB0 \
      --audio-port /dev/ttyUSB1 \
      --alarm-port /dev/ttyAMA0 \
      --model     ../models/sentinel_model.ckpt \
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

# ── 接收器（改造后的库模式）──────────────────────────────────────
import radar_receiver
import audio_receiver

# ── 报警（串口声光）─────────────────────────────────────────────
import serial

ALARM_CMD = bytes([0xAA, 0x55, 0x01, 0x1E])

# ── IoTDA + OBS（懒加载，首次告警时初始化）──────────────────────
try:
    from iot_uploader import HuaweiIoTUploader
    from obs_uploader import OBSUploader
    HAS_IOT = True
except ImportError as e:
    HAS_IOT = False
    print(f"[warn] IoT/OBS 模块不可用，将跳过云端上报: {e}")

# ── HTTP 推送（→ anq-server → WebSocket → App）─────────────────
try:
    import urllib.request as _urllib_req
    HAS_HTTP = True
except ImportError:
    HAS_HTTP = False


def _http_post_alert(server_url, location, time_str, status="NEW"):
    """POST /alert/ 到 anq-server，由服务端通过 WebSocket 广播给 App。"""
    if not HAS_HTTP:
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
        with _urllib_req.urlopen(req, timeout=5) as resp:
            print(f"[http] 告警已推送至服务器: {resp.status}")
    except Exception as e:
        print(f"[http] 推送失败: {e}")


def _load_config(config_path):
    """从 JSON 文件加载设备凭证，支持 IOT_KEY_FILE 环境变量覆盖。"""
    override = os.getenv("IOT_KEY_FILE", "").strip()
    if override:
        config_path = override
    p = Path(config_path)
    if not p.exists():
        print(f"[config] 凭证文件不存在: {config_path}，跳过云端功能")
        return {}
    try:
        return json.loads(p.read_text(encoding="utf-8"))
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


# ── AI 推理栈 ───────────────────────────────────────────────────
try:
    import mindspore as ms
    import mindspore.nn as nn
    import mindspore.ops as ops
    HAS_MS = True
except Exception as e:
    HAS_MS = False
    print(f"[warn] MindSpore 不可用，将使用随机推理占位: {e}")


class GlobalAvgPool(nn.Cell):
    def construct(self, x):
        return x.mean(axis=-1, keep_dims=True)


class AscendSentinelNet(nn.Cell):
    def __init__(self):
        super().__init__()
        self.radar_branch = nn.SequentialCell([
            nn.Conv1d(5, 32, kernel_size=3, pad_mode='same'),
            nn.BatchNorm1d(32), nn.ReLU(), nn.MaxPool1d(2),
            nn.Conv1d(32, 64, kernel_size=3, pad_mode='same'),
            nn.ReLU(), GlobalAvgPool(), nn.Flatten()
        ])
        self.audio_branch = nn.SequentialCell([
            nn.Conv1d(1, 16, kernel_size=64, stride=16, pad_mode='valid'),
            nn.BatchNorm1d(16), nn.ReLU(), nn.MaxPool1d(4),
            nn.Conv1d(16, 32, kernel_size=3, pad_mode='same'),
            nn.ReLU(), GlobalAvgPool(), nn.Flatten()
        ])
        self.classifier = nn.SequentialCell([
            nn.Dense(64 + 32, 64), nn.ReLU(), nn.Dense(64, 2)
        ])

    def construct(self, r, a):
        return self.classifier(
            ops.concat((self.radar_branch(r), self.audio_branch(a)), axis=1)
        )


def load_model(ckpt_path):
    if not HAS_MS:
        return None
    ai_mode = (os.getenv("AI_MODE", "GRAPH") or "GRAPH").upper()
    ms_mode = ms.PYNATIVE_MODE if ai_mode == "PYNATIVE" else ms.GRAPH_MODE
    ms.set_context(
        mode=ms_mode,
        device_target=os.getenv("MS_DEVICE_TARGET", "Ascend"),
        device_id=int(os.getenv("MS_DEVICE_ID", "0")),
    )
    net = AscendSentinelNet()
    if ckpt_path and os.path.exists(ckpt_path):
        ms.load_param_into_net(net, ms.load_checkpoint(ckpt_path), strict_load=False)
        print(f"[model] 已加载权重: {ckpt_path}")
    else:
        print(f"[model] 未找到权重文件 {ckpt_path}，使用随机初始化")
    net = ms.amp.auto_mixed_precision(net, amp_level="O2")
    net.set_train(False)
    # 预热
    _ = net(
        ms.Tensor(np.zeros([1, 5, 64], dtype=np.float32)),
        ms.Tensor(np.zeros([1, 1, 16000], dtype=np.float32)),
    )
    print("[model] 预热完成")
    return net


def infer(net, radar_np, audio_np):
    """
    返回 (pred_class, confidence)。
    pred_class=1 表示欺凌，pred_class=0 表示正常。
    """
    if net is None:
        # 无 AI 栈时随机占位（仅用于调试链路）
        prob = np.random.rand()
        return (1, prob) if prob > 0.9 else (0, 1 - prob)

    r = ms.Tensor(radar_np)
    a = ms.Tensor(audio_np)
    logits = net(r, a)
    probs = ops.Softmax(axis=1)(logits).asnumpy()[0]
    pred_class = int(np.argmax(probs))
    confidence = float(probs[pred_class])
    return pred_class, confidence


# ── 主循环 ──────────────────────────────────────────────────────

def main():
    parser = argparse.ArgumentParser(description="欺凌检测主流水线")
    parser.add_argument("--radar-port", default="/dev/ttyUSB0", help="雷达串口")
    parser.add_argument("--radar-baud", type=int, default=921600)
    parser.add_argument("--audio-port", default="/dev/ttyUSB1", help="音频串口")
    parser.add_argument("--audio-baud", type=int, default=921600)
    parser.add_argument("--alarm-port", default="/dev/ttyAMA0", help="报警串口")
    parser.add_argument("--alarm-baud", type=int, default=115200)
    parser.add_argument("--model", default="../models/sentinel_model.ckpt", help="模型权重路径")
    parser.add_argument("--threshold", type=float, default=0.8, help="报警置信度阈值")
    parser.add_argument("--alarm-cooldown", type=float, default=10.0,
                        help="两次报警最小间隔秒数，避免连续触发")
    parser.add_argument("--config", default="../config/device_key.json",
                        help="IoTDA+OBS 凭证文件路径")
    parser.add_argument("--alert-server", default="http://115.120.205.25:8080/alert/",
                        help="anq-server 告警接口地址")
    args = parser.parse_args()

    print("[main] 启动欺凌检测流水线")
    print(f"[main] 雷达: {args.radar_port}  音频: {args.audio_port}  报警: {args.alarm_port}")
    print(f"[main] 阈值: {args.threshold}  报警冷却: {args.alarm_cooldown}s")

    # 0. 加载云端凭证
    cfg = _load_config(args.config)
    iot = None
    obs = None
    if HAS_IOT and cfg.get("device_id"):
        iot = HuaweiIoTUploader(
            device_id=cfg["device_id"],
            secret=cfg["secret"],
            iot_platform_url=cfg["hostname"],
            port=cfg.get("port", 8883),
            username=cfg.get("username"),
            client_id=cfg.get("client_id"),
            auth_mode=cfg.get("auth_mode", "plain"),
        )
        if iot.connect():
            print("[main] IoTDA 已连接")
        else:
            print("[main] IoTDA 连接失败，将跳过云端上报")
            iot = None
    if HAS_IOT and cfg.get("obs_ak"):
        obs = OBSUploader(
            ak=cfg["obs_ak"],
            sk=cfg["obs_sk"],
            endpoint=cfg["obs_endpoint"],
            bucket_name=cfg["obs_bucket"],
        )
        print("[main] OBS 已初始化")

    # 1. 加载模型
    net = load_model(args.model)

    # 2. 启动接收线程
    radar_q, radar_stop, radar_thread = radar_receiver.start(
        port=args.radar_port, baud=args.radar_baud
    )
    audio_q, audio_stop, audio_thread = audio_receiver.start(
        port=args.audio_port, baud=args.audio_baud
    )

    print("[main] 接收线程已启动，等待数据...")

    last_alarm_time = 0.0
    infer_count = 0
    # 滑动窗口：保留最近 RADAR_WINDOW_FRAMES 帧的原始点云
    RADAR_WINDOW_FRAMES = 20
    RADAR_SAMPLE_PTS = 64
    radar_window = deque(maxlen=RADAR_WINDOW_FRAMES)

    try:
        while True:
            # 持续排空雷达队列，填充滑动窗口
            while True:
                try:
                    pts = radar_q.get_nowait()  # (N, 5) 原始点云
                    radar_window.append(pts)
                except queue.Empty:
                    break

            # 以音频帧为主节拍（1秒一次）
            try:
                audio_np = audio_q.get(timeout=5.0)
            except queue.Empty:
                print("[main] 等待音频数据超时...")
                continue

            # 窗口内点云合并，随机采样 64 点 → [1, 5, 64]
            if len(radar_window) == 0:
                print("[main] 等待雷达窗口数据...")
                continue
            all_pts = np.vstack(list(radar_window))  # (M, 5)
            n_total = all_pts.shape[0]
            if n_total >= RADAR_SAMPLE_PTS:
                idx = np.random.choice(n_total, RADAR_SAMPLE_PTS, replace=False)
            else:
                idx = np.random.choice(n_total, RADAR_SAMPLE_PTS, replace=True)
            sampled = all_pts[idx]  # (64, 5)
            radar_np = sampled.T.reshape(1, 5, RADAR_SAMPLE_PTS).astype(np.float32)

            # 推理
            t0 = time.perf_counter()
            pred_class, confidence = infer(net, radar_np, audio_np)
            infer_ms = (time.perf_counter() - t0) * 1000
            infer_count += 1

            print(f"[infer] #{infer_count} class={pred_class} conf={confidence:.3f} "
                  f"pts_in_window={n_total} ({infer_ms:.1f}ms)")

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
                            audio_raw = audio_np[0, 0, :]
                            audio_bytes = (audio_raw * 32767).astype(np.int16).tobytes()
                            evidence_url = obs.upload_audio_evidence(
                                audio_bytes,
                                cfg.get("device_id", "unknown")
                            ) or ""
                        except Exception as e:
                            print(f"[main] OBS 上传失败: {e}")

                    # 3. IoTDA 上报
                    if iot is not None:
                        try:
                            iot.upload_alarm(
                                alarm_type="bullying",
                                confidence=confidence,
                                location=cfg.get("location", "未知位置"),
                                audio_level=int(np.abs(audio_np).mean() * 100),
                                radar_summary=f"窗口点数={n_total}",
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
