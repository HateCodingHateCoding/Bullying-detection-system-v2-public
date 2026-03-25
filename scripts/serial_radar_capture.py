#!/usr/bin/env python3
"""
serial_radar_capture.py - 离线雷达数据采集工具

从串口采集 RS6240 雷达点云，组装为 AscendSentinel2 模型输入格式并保存。

输出形状: (T_FRAMES, 5, 64) float32
  - T_FRAMES=4 帧时序窗口
  - 5 通道: x, y, z, velocity, SNR
  - 64 个点（不足补零，超出随机采样）

保存路径: ../data/real_radar_normal.npy
"""

import serial
import numpy as np
import struct
import sys

# ========== 配置参数 ==========
SERIAL_PORT = '/dev/ttyUSB0'
BAUD_RATE   = 921600
T_FRAMES    = 4    # 与 AscendSentinel2 输入 (B,T,5,64) 的 T 对齐
RADAR_PTS   = 64   # 每帧固定点数
RADAR_CH    = 5    # x, y, z, velocity, SNR

# HIF 协议常量
HIF_MAGIC  = 0xA5
HIF_HDR_LEN = 6
_PC_TAG    = b"motion_point_cloud\x00"  # 19 字节


def hif_check8_valid(hdr):
    return (sum(hdr) & 0xFF) == 0xFF


def hif_parse_header(hdr):
    hif_type   = hdr[2] & 0x03
    check_flag = (hdr[2] >> 4) & 0x01
    more       = (hdr[2] >> 5) & 0x01
    msgid      = hdr[3]
    length     = hdr[4] | ((hdr[5] & 0x0F) << 8)
    seq        = (hdr[5] >> 4) & 0x07
    return hif_type, check_flag, more, msgid, length, seq


def sync_to_header(ser):
    for _ in range(8192):
        b = ser.read(1)
        if not b:
            return None
        if b[0] != HIF_MAGIC:
            continue
        rest = ser.read(HIF_HDR_LEN - 1)
        if len(rest) < HIF_HDR_LEN - 1:
            return None
        hdr = bytes([b[0]]) + rest
        if hif_check8_valid(hdr):
            return hdr
    return None


def parse_point_cloud(payload):
    """解析 HIF payload → numpy (N, 5) float32，失败返回 None。"""
    if len(payload) < len(_PC_TAG) + 2:
        return None
    if payload[:len(_PC_TAG)] != _PC_TAG:
        return None
    offset   = len(_PC_TAG)
    n_points = int.from_bytes(payload[offset:offset+2], 'little')
    offset  += 2
    exp_bytes = n_points * RADAR_CH * 4
    if len(payload) < offset + exp_bytes or n_points == 0:
        return None
    pts = np.frombuffer(payload[offset:offset+exp_bytes], dtype=np.float32)
    return pts.reshape(n_points, RADAR_CH)


def points_to_frame(pts):
    """(N,5) → (5,64): 随机采样或补零至 RADAR_PTS 点，然后转置。"""
    n = len(pts)
    if n >= RADAR_PTS:
        idx = np.random.choice(n, RADAR_PTS, replace=False)
        sampled = pts[idx]
    else:
        pad = np.zeros((RADAR_PTS - n, RADAR_CH), dtype=np.float32)
        sampled = np.vstack([pts, pad])
    return sampled.T.astype(np.float32)  # (5, 64)


def collect_radar_window(ser):
    """
    从串口采集 T_FRAMES 帧点云，返回 (T_FRAMES, 5, 64) float32。
    """
    frames = []
    while len(frames) < T_FRAMES:
        hdr = sync_to_header(ser)
        if hdr is None:
            print("[radar] 同步超时，重试...")
            continue
        _, _, _, _, length, _ = hif_parse_header(hdr)
        if length == 0 or length > 4096:
            continue
        payload = ser.read(length)
        if len(payload) < length:
            continue
        pts = parse_point_cloud(payload)
        if pts is None:
            continue
        frame = points_to_frame(pts)  # (5, 64)
        frames.append(frame)
        print(f"[radar] 采集帧 {len(frames)}/{T_FRAMES}  points={len(pts)}")

    window = np.stack(frames, axis=0)  # (T, 5, 64)
    return window


def main():
    print(f"[radar] 打开串口 {SERIAL_PORT} @ {BAUD_RATE}")
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=2)

    window = collect_radar_window(ser)
    ser.close()

    out_path = "../data/real_radar_normal.npy"
    np.save(out_path, window)
    print(f"[radar] 已保存: {out_path}  shape={window.shape}  "
          f"min={window.min():.4f} max={window.max():.4f}")
    print(f"  → 模型输入时 reshape 为 (1,{T_FRAMES},5,64)")


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\n[radar] 用户中断")
    except Exception as e:
        print(f"[radar] 错误: {e}")
        import traceback; traceback.print_exc()
        sys.exit(1)
