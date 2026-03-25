#!/usr/bin/env python3
"""
audio_receiver.py - 从串口接收音频数据，转换为梅尔频谱后放入 queue

音频帧格式 (MCU 端发送):
  [0x00][0x01] AA 55       帧头 2B
  [0x02][0x03] seq         序列号 uint16 LE
  [0x04][0x05] payload_len PCM 字节数 uint16 LE
  [0x06 ...]   PCM         int16 LE 原始采样
  [...][...]   AA 55       帧尾 2B
共 8 字节开销（帧头2 + seq2 + len2 + 帧尾2），中间为 PCM 负载。

输出格式: numpy array shape (1, 64, 128) float32
  每累积 FRAME_SAMPLES=8000 个 int16 采样（0.5秒 @ 16kHz）
  转换为梅尔频谱 (N_MELS=64, TIME_STEPS=128) 后放入队列。
  与 AscendSentinel2 的音频分支输入 (B,T,1,64,128) 对齐。
"""

import serial
import threading
import queue
import numpy as np


# 音频参数 —— 与 notebook Cell 3 保持一致
AUDIO_SAMPLE_RATE  = 16000   # 16kHz
FRAME_SEC          = 0.5     # 每帧 0.5 秒（与雷达帧间隔对齐）
FRAME_SAMPLES      = int(AUDIO_SAMPLE_RATE * FRAME_SEC)  # 8000 samples/帧
N_MELS             = 64      # 梅尔频带数
TIME_STEPS         = 128     # 时间步数（频谱列数）
N_FFT              = 512


def _pcm_to_mel(pcm_int16):
    """
    将 int16 PCM 数组 (FRAME_SAMPLES,) 转换为归一化梅尔频谱。
    返回 shape (1, 64, 128) float32，与 AudioMelCNNBranch 输入对齐。

    使用纯 numpy 实现，避免依赖 librosa（边侧设备可能未安装）。
    """
    # 归一化到 [-1, 1]
    y = pcm_int16.astype(np.float32) / 32768.0

    # STFT 参数
    hop_length = max(1, FRAME_SAMPLES // TIME_STEPS)  # ~62
    win_length = N_FFT
    window = np.hanning(win_length).astype(np.float32)

    # 补零使帧数恰好覆盖 TIME_STEPS
    n_frames = 1 + (len(y) - win_length) // hop_length
    pad_len = max(0, (TIME_STEPS - 1) * hop_length + win_length - len(y))
    y_pad = np.concatenate([y, np.zeros(pad_len, dtype=np.float32)])

    # 分帧 STFT
    frames = np.stack([
        y_pad[i * hop_length: i * hop_length + win_length] * window
        for i in range(TIME_STEPS)
    ], axis=1)  # (win_length, TIME_STEPS)
    spec = np.fft.rfft(frames, n=N_FFT, axis=0)  # (N_FFT//2+1, TIME_STEPS)
    power = (np.abs(spec) ** 2).astype(np.float32)

    # 梅尔滤波器组（近似三角滤波）
    n_fft_bins = N_FFT // 2 + 1
    mel_f_min = 0.0
    mel_f_max = AUDIO_SAMPLE_RATE / 2.0
    # Hz → Mel
    mel_min = 2595.0 * np.log10(1.0 + mel_f_min / 700.0)
    mel_max = 2595.0 * np.log10(1.0 + mel_f_max / 700.0)
    mel_points = np.linspace(mel_min, mel_max, N_MELS + 2)
    # Mel → Hz
    hz_points = 700.0 * (10.0 ** (mel_points / 2595.0) - 1.0)
    bin_points = np.floor((N_FFT + 1) * hz_points / AUDIO_SAMPLE_RATE).astype(int)

    mel_basis = np.zeros((N_MELS, n_fft_bins), dtype=np.float32)
    for m in range(1, N_MELS + 1):
        f_m_minus = bin_points[m - 1]
        f_m       = bin_points[m]
        f_m_plus  = bin_points[m + 1]
        for k in range(f_m_minus, f_m):
            if k < n_fft_bins:
                mel_basis[m-1, k] = (k - f_m_minus) / (f_m - f_m_minus + 1e-8)
        for k in range(f_m, f_m_plus):
            if k < n_fft_bins:
                mel_basis[m-1, k] = (f_m_plus - k) / (f_m_plus - f_m + 1e-8)

    mel_spec = mel_basis @ power  # (N_MELS, TIME_STEPS)

    # 转为 dB 并标准化
    mel_db = 10.0 * np.log10(mel_spec + 1e-10)
    mel_db = (mel_db - mel_db.mean()) / (mel_db.std() + 1e-8)
    mel_db = mel_db[:, :TIME_STEPS]

    return mel_db[np.newaxis].astype(np.float32)  # (1, 64, 128)


def audio_receiver_thread(port, baud, out_queue, stop_event):
    """
    后台线程：持续从串口读取原始 int16 PCM 音频数据，
    每累积 FRAME_SAMPLES(8000) 个采样点，转换为梅尔频谱
    (1, 64, 128) float32 numpy array 放入 out_queue。

    main_pipeline 按 T_FRAMES=4 组装时序窗口 (T,1,64,128) 送入 AscendSentinel2。
    """
    try:
        ser = serial.Serial(port, baud, timeout=0.1)
        print(f"[audio] 串口已打开: {port} @ {baud} bps")
    except serial.SerialException as e:
        print(f"[audio] 无法打开串口: {e}")
        return

    sample_buf = bytearray()  # 累积的 PCM 字节
    frame_count = 0
    FRAME_BYTES = FRAME_SAMPLES * 2  # int16 = 2 bytes/sample
    raw_buf = bytearray()

    HEADER = bytes([0xAA, 0x55])
    TAIL   = bytes([0xAA, 0x55])
    MIN_PKT = 8  # 帧头(2)+seq(2)+len(2)+帧尾(2)

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
                    # 保留最后1字节防止跨包帧头被截断
                    raw_buf = raw_buf[-1:]
                    break
                if idx > 0:
                    raw_buf = raw_buf[idx:]

                if len(raw_buf) < MIN_PKT:
                    break

                # 读 payload_len
                payload_len = int.from_bytes(raw_buf[4:6], 'little')
                pkt_len = MIN_PKT + payload_len
                if len(raw_buf) < pkt_len:
                    break

                # 验证帧尾
                tail_start = 6 + payload_len
                if raw_buf[tail_start:tail_start+2] != TAIL:
                    raw_buf = raw_buf[2:]
                    continue

                # 提取 PCM 负载
                pcm_bytes = raw_buf[6:6 + payload_len]
                raw_buf = raw_buf[pkt_len:]

                sample_buf.extend(pcm_bytes)

                # 每累积够 FRAME_SAMPLES 个 int16 就输出一帧梅尔频谱
                while len(sample_buf) >= FRAME_BYTES:
                    chunk = bytes(sample_buf[:FRAME_BYTES])
                    sample_buf = sample_buf[FRAME_BYTES:]
                    pcm = np.frombuffer(chunk, dtype=np.int16)
                    mel_frame = _pcm_to_mel(pcm)  # (1, 64, 128)
                    frame_count += 1
                    try:
                        out_queue.put_nowait(mel_frame)
                    except queue.Full:
                        try:
                            out_queue.get_nowait()
                        except queue.Empty:
                            pass
                        out_queue.put_nowait(mel_frame)

    except Exception as e:
        if not stop_event.is_set():
            print(f"[audio] 线程异常: {e}")
    finally:
        ser.close()
        print("[audio] 线程已退出")


def start(port="/dev/ttyUSB1", baud=921600, maxsize=8):
    """
    启动音频接收线程，返回 (out_queue, stop_event, thread)。

    调用方从 out_queue 取单帧 numpy array (1, 64, 128) float32 梅尔频谱。
    main_pipeline 按 T_FRAMES=4 组装时序窗口 (T,1,64,128) 送入 AscendSentinel2。
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
