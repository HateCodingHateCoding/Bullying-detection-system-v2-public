#!/usr/bin/env python3
"""
radar_receiver.py - 从串口接收雷达 HIF 数据，解析后放入 queue
D02 (UART1 GPIO15/16) → TTL转USB → 香橙派AIpro (/dev/ttyUSBx)

点云 payload 格式 (RS6x/7x HIF 协议):
  "motion_point_cloud\0"  标识字符串 (19 字节含 \0)
  uint16 LE               本帧点数 N
  N × (x, y, z, vel, snr) 每个字段 float32 LE，共 N×20 字节
最大 payload 4KB，N 可变。

输出格式: numpy array shape [1, 5, N_max] float32
  轴0: batch=1
  轴1: 5 通道 (x, y, z, velocity, snr)
  轴2: RADAR_STEPS=64 (不足补0，超出截断)
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

        if b[0] == HIF_MAGIC:
            rest = ser.read(5)
            if len(rest) < 5:
                return None

            hdr = bytearray([HIF_MAGIC]) + bytearray(rest)
            if hif_check8_valid(hdr):
                if skipped > 0:
                    print(f"  [SYNC] skipped {skipped} bytes to find header")
                return bytes(hdr)
            else:
                skipped += 1
                continue
        else:
            skipped += 1

    print(f"  [SYNC] failed after skipping {skipped} bytes")
    return None


def _parse_pointcloud(payload_bytes):
    """
    解析一个 HIF payload，返回原始点云 (N, 5) float32，N 为实际点数。
    若未找到标识或无有效点则返回 None。

    协议格式：
      "motion_point_cloud\0" (19B) + uint16LE 点数 N + N×5×float32 点数据
      每个点字段顺序：x, y, z, velocity, snr（均 float32 LE）
    """
    buf = bytes(payload_bytes)

    tag_pos = buf.find(_PC_TAG)
    if tag_pos == -1:
        return None

    offset = tag_pos + _PC_TAG_LEN
    if offset + 2 > len(buf):
        return None

    n_points = int.from_bytes(buf[offset:offset + 2], 'little')
    offset += 2

    data_bytes = n_points * _POINT_BYTES
    if offset + data_bytes > len(buf):
        n_points = (len(buf) - offset) // _POINT_BYTES
        data_bytes = n_points * _POINT_BYTES

    if n_points == 0:
        return None

    raw = np.frombuffer(buf[offset:offset + data_bytes], dtype=np.float32)
    return raw.reshape(n_points, RADAR_CHANNELS)  # (N, 5): x,y,z,vel,snr


def radar_receiver_thread(port, baud, out_queue, stop_event):
    """
    后台线程：持续从串口读取雷达 HIF 数据，每收到一个完整物理帧
    就将原始点云 (N, 5) float32 numpy array 放入 out_queue。
    N 为本帧实际点数，由调用方做窗口累积后再采样到固定维度。

    Args:
        port:       串口设备，如 /dev/ttyUSB0
        baud:       波特率，默认 921600
        out_queue:  queue.Queue，接收方从这里取 (N,5) 原始点云
        stop_event: threading.Event，置位后线程退出
    """
    try:
        ser = serial.Serial(port, baud, timeout=1.0)
        print(f"[radar] 串口已打开: {port} @ {baud} bps")
    except serial.SerialException as e:
        print(f"[radar] 无法打开串口: {e}")
        return

    frame_count = 0
    hif_count = 0
    current_frame_payload = bytearray()  # 当前帧累积的 payload 字节

    try:
        while not stop_event.is_set():
            hdr = sync_to_header(ser)
            if hdr is None:
                continue

            hif_type, check_flag, more, msgid, length, seq = hif_parse_header(hdr)

            if is_complete_ack(hdr):
                # 一帧结束，解析原始点云并放入队列
                frame_count += 1
                if current_frame_payload:
                    pts = _parse_pointcloud(bytes(current_frame_payload))
                    if pts is not None:
                        try:
                            out_queue.put_nowait(pts)
                        except queue.Full:
                            # 队列满则丢弃最旧帧，保持实时性
                            try:
                                out_queue.get_nowait()
                            except queue.Empty:
                                pass
                            out_queue.put_nowait(pts)
                    current_frame_payload = bytearray()

                if frame_count % 50 == 0:
                    print(f"[radar] frames={frame_count} hif_pkts={hif_count}")
                continue

            if length > 0:
                payload_len = length + (4 if check_flag else 0)
                payload = ser.read(payload_len)
                if len(payload) < payload_len:
                    print(f"[radar] payload short: expect {payload_len}, got {len(payload)}")
                    continue

                hif_count += 1
                # 只取实际数据，去掉 check32 尾部（若有）
                data_len = length
                current_frame_payload.extend(payload[:data_len])

    except Exception as e:
        print(f"[radar] 异常: {e}")
    finally:
        ser.close()
        print(f"[radar] 线程退出，共接收 {frame_count} 帧")


def start(port="/dev/ttyUSB0", baud=921600, maxsize=32):
    """
    启动雷达接收线程，返回 (out_queue, stop_event, thread)。

    调用方从 out_queue 取原始点云 numpy array (N, 5) float32。
    N 为本帧实际点数，调用方负责窗口累积和采样。
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
