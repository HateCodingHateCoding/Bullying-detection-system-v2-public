#!/usr/bin/env python3
"""
radar_receiver.py - 从串口接收雷达 HIF 数据，解析后放入 queue
D02 (UART1 GPIO15/16) → TTL转USB → 香橙派AIpro (/dev/ttyUSBx)

点云 payload 格式 (RS6x/7x HIF 协议):
  "motion_point_cloud\0"  标识字符串 (19 字节含 \0)
  uint16 LE               本帧点数 N
  N × (x, y, z, vel, snr) 每个字段 float32 LE，共 N×20 字节
最大 payload 4KB，N 可变。

输出格式: numpy array shape [5, 64] float32
  轴0: 5 通道 (x, y, z, velocity, snr)
  轴1: RADAR_STEPS=64 (不足补0，超出随机采样)

每帧单独放入 queue，main_pipeline 负责按 T_FRAMES=4 组装时序窗口。
"""

import serial
import sys
import time
import threading
import queue
import numpy as np


# HIF header 结构 (6字节)
# [0] = 0xA5 (magic)
# [1] = check8 补码
# [2] = type(bit0-1) | check_flag(bit4) | more(bit5)
# [3] = msgid
# [4] = len_lo
# [5] = len_hi(bit0-3) | seq(bit4-6)

HIF_MAGIC = 0xA5
HIF_HDR_LEN = 6

RADAR_CHANNELS = 5   # x, y, z, velocity, snr
RADAR_STEPS = 64     # 固定点数，与 AscendSentinel2 输入 (B,T,5,64) 对齐

# 点云标识字符串（含 \0 共 19 字节）
_PC_TAG = b"motion_point_cloud\x00"
_PC_TAG_LEN = len(_PC_TAG)          # 19
_POINT_BYTES = RADAR_CHANNELS * 4   # 每点 5×float32 = 20 字节


def hif_check8_valid(hdr):
    """校验 HIF header 的 check8：6字节之和 == 0xFF"""
    return (sum(hdr) & 0xFF) == 0xFF


def hif_parse_header(hdr):
    """解析 HIF header，返回 (type, check_flag, more, msgid, length, seq)"""
    hif_type = hdr[2] & 0x03
    check_flag = (hdr[2] >> 4) & 0x01
    more = (hdr[2] >> 5) & 0x01
    msgid = hdr[3]
    length = hdr[4] | ((hdr[5] & 0x0F) << 8)
    seq = (hdr[5] >> 4) & 0x07
    return hif_type, check_flag, more, msgid, length, seq


def is_complete_ack(hdr):
    """判断是否是 complete ack (type=3, msgid=0x0C, len=0)"""
    hif_type, _, _, msgid, length, _ = hif_parse_header(hdr)
    return hif_type == 0x03 and msgid == 0x0C and length == 0


def sync_to_header(ser):
    """
    在字节流中寻找有效的 HIF header (0xA5 开头 + check8 校验通过)
    返回 6 字节 header 或 None (超时)
    """
    max_skip = 8192
    skipped = 0

    while skipped < max_skip:
        b = ser.read(1)
        if not b:
            return None
        if b[0] != HIF_MAGIC:
            skipped += 1
            continue
        rest = ser.read(HIF_HDR_LEN - 1)
        if len(rest) < HIF_HDR_LEN - 1:
            return None
        hdr = bytes([b[0]]) + rest
        if hif_check8_valid(hdr):
            return hdr
        skipped += 1
    return None


def parse_point_cloud_payload(payload):
    """
    解析 HIF payload，提取点云数据。
    返回 numpy array shape (N, 5) float32，或 None。
    """
    if len(payload) < _PC_TAG_LEN + 2:
        return None
    if payload[:_PC_TAG_LEN] != _PC_TAG:
        return None
    offset = _PC_TAG_LEN
    n_points = int.from_bytes(payload[offset:offset+2], 'little')
    offset += 2
    expected_bytes = n_points * _POINT_BYTES
    if len(payload) < offset + expected_bytes or n_points == 0:
        return None
    raw = payload[offset:offset + expected_bytes]
    pts = np.frombuffer(raw, dtype=np.float32).reshape(n_points, RADAR_CHANNELS)
    return pts


def points_to_frame(pts):
    """
    将原始点云 (N,5) 转换为固定形状 (5,64):
    - 若 N >= 64: 随机无放回采样 64 点
    - 若 N < 64:  补零至 64 点
    返回 numpy array shape (5, 64) float32。
    """
    n = len(pts)
    if n >= RADAR_STEPS:
        idx = np.random.choice(n, RADAR_STEPS, replace=False)
        sampled = pts[idx]
    else:
        pad = np.zeros((RADAR_STEPS - n, RADAR_CHANNELS), dtype=np.float32)
        sampled = np.vstack([pts, pad])
    return sampled.T.astype(np.float32)  # (5, 64)


def radar_receiver_thread(port, baud, out_queue, stop_event):
    """
    后台线程：持续从串口读取 HIF 数据帧，解析点云后
    将 (5, 64) float32 数组放入 out_queue。
    main_pipeline 按 T_FRAMES 窗口组装 (T,5,64) 再送模型。
    """
    try:
        ser = serial.Serial(port, baud, timeout=0.5)
        print(f"[radar] 串口已打开: {port} @ {baud} bps")
    except serial.SerialException as e:
        print(f"[radar] 无法打开串口: {e}")
        return

    try:
        while not stop_event.is_set():
            hdr = sync_to_header(ser)
            if hdr is None:
                continue

            _, _, _, _, length, _ = hif_parse_header(hdr)

            if is_complete_ack(hdr):
                continue

            if length == 0 or length > 4096:
                continue

            payload = ser.read(length)
            if len(payload) < length:
                continue

            pts = parse_point_cloud_payload(payload)
            if pts is None:
                continue

            frame = points_to_frame(pts)  # (5, 64)
            try:
                out_queue.put_nowait(frame)
            except queue.Full:
                try:
                    out_queue.get_nowait()
                except queue.Empty:
                    pass
                out_queue.put_nowait(frame)

    except Exception as e:
        if not stop_event.is_set():
            print(f"[radar] 线程异常: {e}")
    finally:
        ser.close()
        print("[radar] 线程已退出")


def start(port="/dev/ttyUSB0", baud=921600, maxsize=32):
    """
    启动雷达接收线程，返回 (out_queue, stop_event, thread)。

    调用方从 out_queue 取单帧 numpy array (5, 64) float32。
    main_pipeline 按 T_FRAMES=4 组装时序窗口 (T,5,64) 送入 AscendSentinel2。
    结束时调用 stop_event.set() 并 thread.join()。
    """
    out_queue = queue.Queue(maxsize=maxsize)
    stop_event = threading.Event()
    t = threading.Thread(
        target=radar_receiver_thread,
        args=(port, baud, out_queue, stop_event),
        daemon=True,
        name="radar-receiver",
    )
    t.start()
    return out_queue, stop_event, t


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="雷达 HIF 数据接收器（独立测试模式）")
    parser.add_argument("--port", default="/dev/ttyUSB0")
    parser.add_argument("--baud", type=int, default=921600)
    args = parser.parse_args()

    q, stop, thread = start(args.port, args.baud)
    print("[radar] 独立测试模式，按 Ctrl+C 停止...")
    try:
        while True:
            try:
                frame = q.get(timeout=2.0)
                print(f"[radar] 收到帧 shape={frame.shape} min={frame.min():.4f} max={frame.max():.4f}")
            except queue.Empty:
                print("[radar] 等待数据...")
    except KeyboardInterrupt:
        pass
    finally:
        stop.set()
        thread.join(timeout=3)
        print("[radar] 已停止")
