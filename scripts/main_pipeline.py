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
import csv
import queue
import json
import logging
import argparse
import threading
from collections import deque
from pathlib import Path

import numpy as np

# ── 日志 ──────────────────────────────────────────────────────────
logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s [%(name)s] %(levelname)s %(message)s",
    datefmt="%H:%M:%S",
)
logger = logging.getLogger("pipeline")

# ── 接收器 ────────────────────────────────────────────────────────
import radar_receiver
import audio_receiver

# ── 报警（串口声光）──────────────────────────────────────────────
try:
    import serial
except ImportError:
    serial = None

ALARM_CMD = bytes([0xAA, 0x55, 0x01, 0x1E])

# ── IoTDA + OBS（懒加载，首次告警时初始化）────────────────────────
try:
    from iot_uploader import HuaweiIoTUploader
    from obs_uploader import OBSUploader
    HAS_IOT = True
except ImportError as e:
    HAS_IOT = False
    logger.warning(f"IoT/OBS 模块不可用，将跳过云端上报: {e}")

# ── HTTP 推送（→ anq-server → WebSocket → App）──────────────────
try:
    import urllib.request as _urllib_req
    HAS_HTTP = True
except ImportError:
    HAS_HTTP = False


# ── 模型参数（从公共配置导入）─────────────────────────────────────
try:
    from sentinel_config import (
        T_FRAMES, N_MELS, TIME_STEPS, RADAR_CH, RADAR_PTS,
        ALIGN_TOLERANCE_SEC, STATE_RESET_GAP_SEC,
        BUFFER_MAX_STEPS, RADAR_AGG_WINDOW_SEC,
    )
except ImportError:
    T_FRAMES   = 4
    N_MELS     = 64
    TIME_STEPS = 128
    RADAR_CH   = 5
    RADAR_PTS  = 64
    ALIGN_TOLERANCE_SEC = 0.20
    STATE_RESET_GAP_SEC = 1.50
    BUFFER_MAX_STEPS = 64
    RADAR_AGG_WINDOW_SEC = 0.50


def _zero_radar_frame():
    return np.zeros((RADAR_CH, RADAR_PTS), dtype=np.float32)


def _safe_norm(arr):
    arr = arr.astype(np.float32)
    if arr.size == 0:
        return arr
    lo = float(arr.min())
    hi = float(arr.max())
    if hi - lo < 1e-6:
        return np.zeros_like(arr, dtype=np.float32)
    return (arr - lo) / (hi - lo)


def _points_to_salient_frame(points, scores):
    if len(points) == 0:
        return _zero_radar_frame()

    points = points.astype(np.float32)
    scores = scores.astype(np.float32)
    order = np.argsort(-scores)
    ranked = points[order]

    if len(ranked) >= RADAR_PTS:
        selected = ranked[:RADAR_PTS]
    else:
        pad = np.zeros((RADAR_PTS - len(ranked), RADAR_CH), dtype=np.float32)
        selected = np.vstack([ranked, pad])
    return selected.T.astype(np.float32)


def _aggregate_radar_packets(audio_ts, radar_buffer, window_sec, stats):
    window_start = audio_ts - window_sec
    packets = [p for p in radar_buffer if window_start <= p["ts"] <= audio_ts]
    if not packets:
        return None

    point_sets = [p.get("points") for p in packets if p.get("points") is not None and len(p.get("points")) > 0]
    if point_sets:
        merged_points = np.concatenate(point_sets, axis=0).astype(np.float32)
        point_scores = []
        for packet in packets:
            pts = packet.get("points")
            if pts is None or len(pts) == 0:
                continue
            pts = pts.astype(np.float32)
            dist = np.linalg.norm(pts[:, :3], axis=1)
            vel = np.abs(pts[:, 3])
            snr = pts[:, 4]
            recency = 1.0 - min(max(audio_ts - packet["ts"], 0.0) / max(window_sec, 1e-6), 1.0)
            score = (
                0.45 * _safe_norm(snr)
                + 0.35 * _safe_norm(vel)
                + 0.15 * _safe_norm(dist)
                + 0.05 * recency
            )
            point_scores.append(score.astype(np.float32))
        merged_scores = np.concatenate(point_scores, axis=0) if point_scores else np.zeros(len(merged_points), dtype=np.float32)
        agg_frame = _points_to_salient_frame(merged_points, merged_scores)
    else:
        agg_frame = packets[-1]["frame"]

    stats["aggregated"] += 1
    stats["aggregated_packets"] += len(packets)
    stats["aggregated_points"] += int(sum(len(ps) for ps in point_sets))
    return {
        "ts": packets[-1]["ts"],
        "frame": agg_frame,
        "count": len(packets),
    }


def _drain_queue(q, target_buffer):
    while True:
        try:
            item = q.get_nowait()
            target_buffer.append(item)
        except queue.Empty:
            break


def _select_aligned_radar(audio_ts, radar_buffer, last_radar_packet, stats, tolerance_sec):
    matched_idx = None
    best_delta = None
    for idx, packet in enumerate(radar_buffer):
        delta = abs(packet["ts"] - audio_ts)
        if best_delta is None or delta < best_delta:
            best_delta = delta
            matched_idx = idx

    matched_packet = None
    fallback = "none"
    if matched_idx is not None and best_delta is not None and best_delta <= tolerance_sec:
        matched_packet = radar_buffer[matched_idx]
        while len(radar_buffer) > matched_idx + 1:
            radar_buffer.pop()
        while matched_idx >= 0:
            radar_buffer.popleft()
            matched_idx -= 1
        stats["matched"] += 1
        stats["delta_sum"] += best_delta
        stats["delta_max"] = max(stats["delta_max"], best_delta)
    elif last_radar_packet is not None:
        matched_packet = last_radar_packet
        fallback = "hold_last"
        stats["fallback_hold"] += 1
    else:
        matched_packet = {"ts": audio_ts, "frame": _zero_radar_frame()}
        fallback = "zero_fill"
        stats["fallback_zero"] += 1

    return matched_packet, fallback, best_delta


def _http_post_alert(server_url, location, time_str, status="NEW"):
    """POST /alert/ 到 anq-server，由服务端通过 WebSocket 广播给 App。"""
    if not HAS_HTTP or not server_url:
        return
    if not server_url.startswith("https://"):
        logger.warning(f"告警地址未使用 HTTPS: {server_url}，建议升级为加密连接")
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
            logger.info(f"HTTP POST {server_url} → {resp.status}")
    except Exception as e:
        logger.warning(f"HTTP POST 失败: {e}")


def load_config(config_path):
    try:
        with open(config_path, encoding="utf-8") as f:
            return json.load(f)
    except Exception as e:
        logger.warning(f"凭证文件读取失败: {e}")
        return {}


def send_alarm(alarm_port, baud=115200):
    """通过串口触发声光报警模块"""
    if serial is None:
        logger.warning("pyserial 未安装，跳过串口报警")
        return
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
        logger.info(f"已发送报警指令 → {alarm_port}")
    except Exception as e:
        logger.error(f"报警发送失败: {e}")


# ── AI 推理栈 ────────────────────────────────────────────────────
try:
    import mindspore as ms
    from mindspore import Tensor, context
    from sentinel_model import AscendSentinel2
    HAS_MS = True
except Exception as e:
    HAS_MS = False
    logger.warning(f"MindSpore 不可用，将使用随机推理占位: {e}")


def load_model(ckpt_path):
    """加载 AscendSentinel2 权重，不可用时返回 None（随机占位）。"""
    if not HAS_MS:
        logger.warning("MindSpore 不可用，使用随机推理占位")
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
            logger.info(f"已加载权重: {ckpt_path}")
        except Exception as e:
            logger.error(f"权重加载失败: {e}")
    else:
        logger.warning(f"权重文件不存在: {ckpt_path}，使用随机初始化")
    return net


def infer(net, radar_window, audio_window, h_prev=None,
          radar_valid=True, audio_valid=True):
    """
    执行一次推理。

    双模态可用时走主分类头（main_logits）；
    单模态不可用时，用对应的独立分支头（audio_head / radar_head）直接输出，
    同时仍然把零输入喂给缺失的那一路（模态 dropout 训练过），
    让 MindCMA alpha 自动偏向有效模态。

    Args:
        net:           AscendSentinel2 实例（或 None）
        radar_window:  list of (5,64) float32，长度 == T_FRAMES
        audio_window:  list of (1,64,128) float32，长度 == T_FRAMES
        h_prev:        上一次推理保留的 GRU hidden state
        radar_valid:   雷达数据是否可用
        audio_valid:   音频数据是否可用

    Returns:
        (pred_class, confidence, h_next)  int, float, object
    """
    if net is None:
        confidence = float(np.random.rand())
        return int(confidence > 0.5), confidence, None

    r_np = np.stack(radar_window, axis=0)[np.newaxis].astype(np.float32)
    a_np = np.stack(audio_window, axis=0)[np.newaxis].astype(np.float32)

    # 缺失模态置零，让 MindCMA alpha 自动偏向有效的那一路
    if not radar_valid:
        r_np = np.zeros_like(r_np)
    if not audio_valid:
        a_np = np.zeros_like(a_np)

    r_t = Tensor(r_np)
    a_t = Tensor(a_np)

    main_logits, audio_logits, radar_logits, _, h_next = net(a_t, r_t, h_prev)
    sm = ms.ops.Softmax(axis=1)

    if radar_valid and audio_valid:
        # 双模态：用主分类头
        probs = sm(main_logits).asnumpy()[0]
    elif audio_valid and not radar_valid:
        # 仅音频：用音频独立头
        probs = sm(audio_logits).asnumpy()[0]
    elif radar_valid and not audio_valid:
        # 仅雷达：用雷达独立头
        probs = sm(radar_logits).asnumpy()[0]
    else:
        # 双路都挂了，返回低置信度
        return 0, 0.0, h_next

    pred_class = int(np.argmax(probs))
    confidence = float(probs[1])
    return pred_class, confidence, h_next


def _normalize_offline_windows(arr, expected_inner_shape, name):
    arr = np.asarray(arr, dtype=np.float32)
    if tuple(arr.shape) == tuple(expected_inner_shape):
        return arr[np.newaxis, ...]
    if arr.ndim >= 1 and tuple(arr.shape[1:]) == tuple(expected_inner_shape):
        return arr
    if arr.ndim == len(expected_inner_shape) and tuple(arr.shape[1:]) == tuple(expected_inner_shape[1:]):
        total = arr.shape[0]
        if total < T_FRAMES:
            raise ValueError(f"{name} 序列长度不足 T_FRAMES: got={arr.shape}")
        windows = [arr[i:i + T_FRAMES] for i in range(total - T_FRAMES + 1)]
        return np.stack(windows, axis=0).astype(np.float32)
    raise ValueError(f"{name} shape 不符合预期: got={arr.shape}, expected={expected_inner_shape} 或 (N,{','.join(map(str, expected_inner_shape))})")


def _write_sweep_csv(summaries, csv_path):
    fieldnames = [
        "case", "threshold", "reset_every", "align_tolerance", "radar_agg_window",
        "steps", "positives", "mean", "max", "min"
    ]
    with open(csv_path, "w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        for item in summaries:
            writer.writerow({key: item.get(key) for key in fieldnames})
    logger.info(f"sweep 已导出 CSV: {csv_path}")


def _parse_float_list(text, default_value):
    if not text:
        return [default_value]
    return [float(x.strip()) for x in text.split(',') if x.strip()]


def _parse_int_list(text, default_value):
    if not text:
        return [default_value]
    return [int(x.strip()) for x in text.split(',') if x.strip()]


def _run_offline_sequence(net, audio_windows, radar_windows, threshold, total_steps, reset_every=0, tag="offline"):
    hidden_state = None
    positives = 0
    confidences = []

    for idx in range(total_steps):
        if idx > 0 and reset_every > 0 and idx % reset_every == 0:
            hidden_state = None
            logger.info(f"[{tag}] step={idx} 手动重置 hidden state")

        pred_class, confidence, hidden_state = infer(
            net,
            list(radar_windows[idx]),
            list(audio_windows[idx]),
            hidden_state,
        )
        confidences.append(confidence)
        positives += int(pred_class == 1 and confidence >= threshold)
        logger.info(f"[{tag}] step={idx} class={pred_class} conf={confidence:.3f}")

    if confidences:
        conf_arr = np.asarray(confidences, dtype=np.float32)
        summary = {
            "steps": len(confidences),
            "positives": positives,
            "mean": float(conf_arr.mean()),
            "max": float(conf_arr.max()),
            "min": float(conf_arr.min()),
        }
        logger.info(
            f"[{tag}] done steps={summary['steps']} positives={summary['positives']} "
            f"mean={summary['mean']:.3f} max={summary['max']:.3f} min={summary['min']:.3f}"
        )
        return summary

    logger.warning(f"[{tag}] 没有可回放窗口")
    return {"steps": 0, "positives": 0, "mean": 0.0, "max": 0.0, "min": 0.0}


def _load_offline_windows(args):
    audio_np = np.load(args.offline_audio).astype(np.float32)
    radar_np = np.load(args.offline_radar).astype(np.float32)
    audio_windows = _normalize_offline_windows(audio_np, (T_FRAMES, 1, N_MELS, TIME_STEPS), "audio")
    radar_windows = _normalize_offline_windows(radar_np, (T_FRAMES, RADAR_CH, RADAR_PTS), "radar")
    total_steps = min(len(audio_windows), len(radar_windows))
    if args.offline_steps > 0:
        total_steps = min(total_steps, args.offline_steps)
    return audio_windows, radar_windows, total_steps


def run_offline_replay(net, args):
    audio_windows, radar_windows, total_steps = _load_offline_windows(args)
    logger.info(f"offline 加载 audio={audio_windows.shape} radar={radar_windows.shape}，回放步数={total_steps}")
    return _run_offline_sequence(
        net,
        audio_windows,
        radar_windows,
        args.threshold,
        total_steps,
        reset_every=args.offline_reset_every,
        tag="offline",
    )


def run_offline_sweep(net, args):
    audio_windows, radar_windows, total_steps = _load_offline_windows(args)
    logger.info(f"sweep 加载 audio={audio_windows.shape} radar={radar_windows.shape}，回放步数={total_steps}")

    thresholds = _parse_float_list(args.sweep_thresholds, args.threshold)
    reset_values = _parse_int_list(args.sweep_reset_every, args.offline_reset_every)
    align_values = _parse_float_list(args.sweep_align_tolerance, args.align_tolerance)
    agg_values = _parse_float_list(args.sweep_radar_agg_window, args.radar_agg_window)
    summaries = []

    if len(align_values) > 1 or len(agg_values) > 1:
        logger.info("sweep 注意：当前 offline 回放输入是已切窗特征，不含原始时间戳流。")
        logger.info("sweep_align_tolerance / sweep_radar_agg_window 仅会记录到结果中，不会改变当前离线回放输出。")

    case_id = 0
    for threshold in thresholds:
        for reset_every in reset_values:
            for align_tolerance in align_values:
                for radar_agg_window in agg_values:
                    case_id += 1
                    tag = f"sweep#{case_id}"
                    logger.info(
                        f"sweep case={case_id} threshold={threshold:.3f} "
                        f"reset_every={reset_every} align_tol={align_tolerance:.3f} "
                        f"agg_window={radar_agg_window:.3f}"
                    )
                    summary = _run_offline_sequence(
                        net,
                        audio_windows,
                        radar_windows,
                        threshold,
                        total_steps,
                        reset_every=reset_every,
                        tag=tag,
                    )
                    summary.update({
                        "case": case_id,
                        "threshold": threshold,
                        "reset_every": reset_every,
                        "align_tolerance": align_tolerance,
                        "radar_agg_window": radar_agg_window,
                    })
                    summaries.append(summary)

    summaries.sort(key=lambda x: (-x["positives"], -x["mean"], -x["max"], x["reset_every"], x["threshold"]))
    logger.info("sweep 排名结果:")
    for item in summaries:
        logger.info(
            f"sweep case={item['case']} threshold={item['threshold']:.3f} "
            f"reset_every={item['reset_every']} align_tol={item['align_tolerance']:.3f} "
            f"agg_window={item['radar_agg_window']:.3f} positives={item['positives']} "
            f"mean={item['mean']:.3f} max={item['max']:.3f} min={item['min']:.3f}"
        )
    if args.sweep_out:
        _write_sweep_csv(summaries, args.sweep_out)
    return summaries


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
    p.add_argument("--align-tolerance", type=float, default=ALIGN_TOLERANCE_SEC,
                   help="音频与雷达配对允许的最大时间差（秒）")
    p.add_argument("--radar-agg-window", type=float, default=RADAR_AGG_WINDOW_SEC,
                   help="按音频节拍聚合雷达帧的时间窗口（秒）")
    p.add_argument("--state-reset-gap", type=float, default=STATE_RESET_GAP_SEC,
                   help="超过该间隔则重置跨窗 GRU hidden state（秒）")
    p.add_argument("--alert-server",  default="",
                   help="anq-server HTTP 地址，如 http://192.168.1.100:8000/alert/")
    p.add_argument("--offline-audio", default="",
                   help="离线回放音频 .npy 路径，shape=(T,1,64,128) 或 (N,T,1,64,128)")
    p.add_argument("--offline-radar", default="",
                   help="离线回放雷达 .npy 路径，shape=(T,5,64) 或 (N,T,5,64)")
    p.add_argument("--offline-steps", type=int, default=0,
                   help="离线模式最多回放多少个窗口，0 表示全部")
    p.add_argument("--offline-reset-every", type=int, default=0,
                   help="离线模式每隔多少步重置一次 hidden state，0 表示不重置")
    p.add_argument("--offline-sweep", action="store_true",
                   help="离线模式下自动扫描多组参数并输出排名")
    p.add_argument("--sweep-thresholds", default="",
                   help="逗号分隔的 threshold 列表，如 0.4,0.5,0.6")
    p.add_argument("--sweep-reset-every", default="",
                   help="逗号分隔的 offline_reset_every 列表，如 0,1,4")
    p.add_argument("--sweep-align-tolerance", default="",
                   help="逗号分隔的 align_tolerance 列表，如 0.1,0.2,0.3（离线回放仅记录）")
    p.add_argument("--sweep-radar-agg-window", default="",
                   help="逗号分隔的 radar_agg_window 列表，如 0.3,0.5,0.7（离线回放仅记录）")
    p.add_argument("--sweep-out", default="",
                   help="将 sweep 排名结果导出到 CSV 文件")
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
            logger.warning("IoTDA 连接失败，将跳过云端上报")
            iot = None
        else:
            logger.info("IoTDA 已连接")

    obs = None
    if HAS_IOT and cfg.get("obs_ak"):
        obs = OBSUploader(
            ak=cfg["obs_ak"],
            sk=cfg["obs_sk"],
            endpoint=cfg["obs_endpoint"],
            bucket_name=cfg["obs_bucket"],
        )
        logger.info("OBS 已初始化")

    # ── 加载模型 ──────────────────────────────────────────────────
    net = load_model(args.model)

    if args.offline_audio and args.offline_radar:
        if args.offline_sweep:
            run_offline_sweep(net, args)
        else:
            run_offline_replay(net, args)
        return

    # ── 启动接收线程 ──────────────────────────────────────────────
    radar_q, radar_stop, radar_thread, radar_health = radar_receiver.start(
        port=args.radar_port, baud=args.radar_baud
    )
    audio_q, audio_stop, audio_thread, audio_health = audio_receiver.start(
        port=args.audio_port, baud=args.audio_baud
    )
    logger.info("接收线程已启动，等待数据...")

    last_alarm_time = 0.0
    infer_count = 0
    radar_buffer = deque(maxlen=BUFFER_MAX_STEPS)
    audio_window = deque(maxlen=T_FRAMES)
    radar_window = deque(maxlen=T_FRAMES)
    last_radar_packet = None
    stream_hidden_state = None
    last_audio_ts = None
    align_stats = {
        "matched": 0,
        "aggregated": 0,
        "aggregated_packets": 0,
        "aggregated_points": 0,
        "fallback_hold": 0,
        "fallback_zero": 0,
        "delta_sum": 0.0,
        "delta_max": 0.0,
        "state_resets": 0,
    }

    try:
        while True:
            # ── 线程健康检测 ──────────────────────────────────
            if not radar_health.is_set() and not radar_stop.is_set():
                logger.error("雷达接收线程已死亡，尝试重启...")
                radar_q, radar_stop, radar_thread, radar_health = radar_receiver.start(
                    port=args.radar_port, baud=args.radar_baud
                )
            if not audio_health.is_set() and not audio_stop.is_set():
                logger.error("音频接收线程已死亡，尝试重启...")
                audio_q, audio_stop, audio_thread, audio_health = audio_receiver.start(
                    port=args.audio_port, baud=args.audio_baud
                )

            _drain_queue(radar_q, radar_buffer)

            try:
                audio_packet = audio_q.get(timeout=5.0)
            except queue.Empty:
                logger.warning("等待音频数据超时...")
                stream_hidden_state = None
                last_audio_ts = None
                align_stats["state_resets"] += 1
                continue

            audio_ts = audio_packet["ts"]
            mel_frame = audio_packet["frame"]

            _drain_queue(radar_q, radar_buffer)
            aggregated_radar_packet = _aggregate_radar_packets(
                audio_ts,
                radar_buffer,
                args.radar_agg_window,
                align_stats,
            )
            if aggregated_radar_packet is not None:
                matched_radar_packet = aggregated_radar_packet
                fallback = "aggregated"
                delta = abs(aggregated_radar_packet["ts"] - audio_ts)
                align_stats["matched"] += 1
                align_stats["delta_sum"] += delta
                align_stats["delta_max"] = max(align_stats["delta_max"], delta)
            else:
                matched_radar_packet, fallback, delta = _select_aligned_radar(
                    audio_ts,
                    radar_buffer,
                    last_radar_packet,
                    align_stats,
                    args.align_tolerance,
                )
            last_radar_packet = matched_radar_packet

            if last_audio_ts is not None and audio_ts - last_audio_ts > args.state_reset_gap:
                stream_hidden_state = None
                align_stats["state_resets"] += 1
                logger.info(f"音频时间跳变 {audio_ts - last_audio_ts:.3f}s，重置 hidden state")
            last_audio_ts = audio_ts

            audio_window.append(mel_frame)
            radar_window.append(matched_radar_packet["frame"])

            matched_radar_ts = matched_radar_packet["ts"]
            delta_text = "NA" if delta is None else f"{delta:.3f}s"
            logger.debug(
                f"align audio_ts={audio_ts:.3f} radar_ts={matched_radar_ts:.3f} "
                f"delta={delta_text} fallback={fallback} "
                f"buffers(radar={len(radar_buffer)}, audio={len(audio_window)})"
            )

            if len(radar_window) < T_FRAMES or len(audio_window) < T_FRAMES:
                logger.info(f"预热中: radar={len(radar_window)}/{T_FRAMES} "
                      f"audio={len(audio_window)}/{T_FRAMES}")
                continue

            if len(radar_buffer) == radar_buffer.maxlen:
                radar_buffer.popleft()

            # 判断各模态是否可用
            radar_valid = (
                align_stats["fallback_zero"] < 10
                and radar_health.is_set()
            )
            audio_valid = audio_health.is_set()

            if not radar_valid and infer_count % 50 == 0:
                logger.warning("雷达不可用，降级为 audio-only 推理")
            if not audio_valid and infer_count % 50 == 0:
                logger.warning("音频不可用，降级为 radar-only 推理")

            t0 = time.perf_counter()
            pred_class, confidence, stream_hidden_state = infer(
                net,
                list(radar_window),
                list(audio_window),
                stream_hidden_state,
                radar_valid=radar_valid,
                audio_valid=audio_valid,
            )
            infer_ms = (time.perf_counter() - t0) * 1000
            infer_count += 1

            if align_stats["matched"] > 0:
                mean_delta = align_stats["delta_sum"] / align_stats["matched"]
            else:
                mean_delta = 0.0

            logger.info(
                f"infer #{infer_count} class={pred_class} conf={confidence:.3f} "
                f"({infer_ms:.1f}ms) align_mean={mean_delta:.3f}s "
                f"align_max={align_stats['delta_max']:.3f}s agg={align_stats['aggregated']} "
                f"agg_pkts={align_stats['aggregated_packets']} hold={align_stats['fallback_hold']} "
                f"zero={align_stats['fallback_zero']} resets={align_stats['state_resets']}"
            )

            if pred_class == 1 and confidence >= args.threshold:
                now = time.time()
                if now - last_alarm_time >= args.alarm_cooldown:
                    logger.warning(f"*** 检测到欺凌！置信度={confidence:.3f} → 触发报警 ***")

                    send_alarm(args.alarm_port, args.alarm_baud)

                    evidence_url = ""
                    if obs is not None:
                        try:
                            audio_bytes = list(audio_window)[-1].tobytes()
                            evidence_url = obs.upload_audio_evidence(
                                audio_bytes, cfg.get("device_id", "edge-device")
                            ) or ""
                        except Exception as e:
                            logger.error(f"OBS 上传失败: {e}")

                    if iot is not None:
                        try:
                            iot.upload_alarm(
                                alarm_type="bullying",
                                confidence=confidence,
                                location=cfg.get("location", "未知位置"),
                                audio_level=int(np.abs(list(audio_window)[-1]).mean() * 100),
                                radar_summary=(
                                    f"T={T_FRAMES}帧窗口 matched={align_stats['matched']} "
                                    f"agg={align_stats['aggregated']} pkts={align_stats['aggregated_packets']} "
                                    f"hold={align_stats['fallback_hold']} zero={align_stats['fallback_zero']}"
                                ),
                                evidence_url=evidence_url,
                            )
                        except Exception as e:
                            logger.error(f"IoTDA 上报失败: {e}")

                    _http_post_alert(
                        server_url=args.alert_server,
                        location=cfg.get("location", "未知位置"),
                        time_str=time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(now)),
                        status="NEW",
                    )

                    last_alarm_time = now
                else:
                    remaining = args.alarm_cooldown - (now - last_alarm_time)
                    logger.info(f"欺凌检测（置信度={confidence:.3f}），"
                          f"报警冷却中（剩余 {remaining:.1f}s）")

    except KeyboardInterrupt:
        logger.info("用户中断，正在停止...")
    finally:
        radar_stop.set()
        audio_stop.set()
        radar_thread.join(timeout=3)
        audio_thread.join(timeout=3)
        if iot is not None:
            iot.disconnect()
        if obs is not None:
            obs.close()
        logger.info("已退出")


if __name__ == "__main__":
    main()
