#!/usr/bin/env python3
"""
audio_receiver.py - 从串口接收音频数据，累积后放入 queue

音频帧格式 (MCU 端发送):
  [0x00][0x01] AA 55       帧头 2B
  [0x02][0x03] seq         序列号 uint16 LE
  [0x04][0x05] payload_len PCM 字节数 uint16 LE
  [0x06 ...]   PCM         int16 LE 原始采样
  [...][...]   AA 55       帧尾 2B
共 8 字节开销（帧头2 + seq2 + len2 + 帧尾2），中间为 PCM 负载。

输出格式: numpy array shape [1, 1, 16000] float32
每累积 16000 个 int16 采样（1秒 @ 16kHz）组成一帧放入队列。
"""

import serial
import threading
import queue
import numpy as np


# 模型期望的音频帧维度
AUDIO_SAMPLE_RATE = 16000  # 16kHz
AUDIO_FRAME_SAMPLES = 16000  # 1秒


def audio_receiver_thread(port, baud, out_queue, stop_event):
    """
    后台线程：持续从串口读取原始 int16 PCM 音频数据，
    每累积 AUDIO_FRAME_SAMPLES 个采样点就将
    [1, 1, 16000] float32 numpy array 放入 out_queue。

    串口传输格式：原始 int16 little-endian PCM 字节流
    （aa 55 为帧头，帧头后紧跟 PCM 数据字节）

    Args:
        port:       串口设备，如 /dev/ttyUSB1
        baud:       波特率，默认 921600
        out_queue:  queue.Queue，接收方从这里取数据
        stop_event: threading.Event，置位后线程退出
    """
    try:
        ser = serial.Serial(port, baud, timeout=0.1)
        print(f"[audio] 串口已打开: {port} @ {baud} bps")
    except serial.SerialException as e:
        print(f"[audio] 无法打开串口: {e}")
        return

    sample_buf = bytearray()  # 累积的 PCM 字节
    frame_count = 0
    FRAME_BYTES = AUDIO_FRAME_SAMPLES * 2  # int16 = 2 bytes/sample
    # 串口接收缓冲区，用于跨 read() 拼包
    raw_buf = bytearray()

    HEADER = bytes([0xAA, 0x55])
    TAIL   = bytes([0xAA, 0x55])
    # 最小包长 = 帧头(2) + seq(2) + payload_len(2) + 帧尾(2) = 8
    MIN_PKT = 8

    try:
        while not stop_event.is_set():
            data = ser.read(512)
            if not data:
                continue

            raw_buf.extend(data)

            # 在缓冲区中解析完整音频帧
            while True:
                # 搜索帧头
                idx = raw_buf.find(HEADER)
                if idx == -1:
                    # 无帧头，保留最后 1 字节防止跨包帧头被截断
                    raw_buf = raw_buf[-1:]
                    break
                if idx > 0:
                    raw_buf = raw_buf[idx:]  # 丢弃帧头前的垃圾字节

                if len(raw_buf) < MIN_PKT:
                    break  # 数据不足，等待更多

                # 解析 payload_len (offset 4, uint16 LE)
                payload_len = int.from_bytes(raw_buf[4:6], 'little')
                pkt_len = MIN_PKT + payload_len  # 完整包长

                if len(raw_buf) < pkt_len:
                    break  # 包未收完，等待

                # 验证帧尾
                tail_start = 6 + payload_len
                if raw_buf[tail_start:tail_start + 2] != TAIL:
                    # 帧尾不对，跳过这个帧头继续搜索
                    raw_buf = raw_buf[2:]
                    continue

                # 提取 PCM 负载
                pcm_data = raw_buf[6:6 + payload_len]
                raw_buf = raw_buf[pkt_len:]  # 消耗掉这个包

                sample_buf.extend(pcm_data)

            # 每凑满一帧就输出
            while len(sample_buf) >= FRAME_BYTES:
                raw_bytes = bytes(sample_buf[:FRAME_BYTES])
                sample_buf = sample_buf[FRAME_BYTES:]

                # int16 -> float32，归一化到 [-1, 1]
                samples = np.frombuffer(raw_bytes, dtype=np.int16).astype(np.float32)
                samples = samples / 32768.0
                frame = samples.reshape(1, 1, AUDIO_FRAME_SAMPLES)

                frame_count += 1
                try:
                    out_queue.put_nowait(frame)
                except queue.Full:
                    # 队列满则丢弃最旧帧，保持实时性
                    try:
                        out_queue.get_nowait()
                    except queue.Empty:
                        pass
                    out_queue.put_nowait(frame)

                if frame_count % 20 == 0:
                    print(f"[audio] frames={frame_count}")

    except Exception as e:
        print(f"[audio] 异常: {e}")
    finally:
        ser.close()
        print(f"[audio] 线程退出，共接收 {frame_count} 帧")


def start(port="/dev/ttyUSB1", baud=921600, maxsize=4):
    """
    启动音频接收线程，返回 (out_queue, stop_event, thread)。

    调用方从 out_queue 取 numpy array [1, 1, 16000] float32。
    结束时调用 stop_event.set() 并 thread.join()。
    """
    out_queue = queue.Queue(maxsize=maxsize)
    stop_event = threading.Event()
    t = threading.Thread(
        target=audio_receiver_thread,
        args=(port, baud, out_queue, stop_event),
        daemon=True,
        name="audio-receiver",
    )
    t.start()
    return out_queue, stop_event, t


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="音频串口接收器（独立测试模式）")
    parser.add_argument("--port", default="/dev/ttyUSB1")
    parser.add_argument("--baud", type=int, default=921600)
    args = parser.parse_args()

    q, stop, thread = start(args.port, args.baud)
    print("[audio] 独立测试模式，按 Ctrl+C 停止...")
    try:
        while True:
            try:
                frame = q.get(timeout=2.0)
                print(f"[audio] 收到帧 shape={frame.shape} min={frame.min():.4f} max={frame.max():.4f}")
            except queue.Empty:
                print("[audio] 等待数据...")
    except KeyboardInterrupt:
        pass
    finally:
        stop.set()
        thread.join(timeout=3)
        print("[audio] 已停止")
