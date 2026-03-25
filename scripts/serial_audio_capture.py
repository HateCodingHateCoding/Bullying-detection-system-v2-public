#!/usr/bin/env python3
"""
serial_audio_capture.py - 离线音频数据采集工具

从串口采集 MCU 发送的 PCM 音频，转换为梅尔频谱后组装为
AscendSentinel2 模型音频分支输入格式并保存。

输出形状: (T_FRAMES, 1, 64, 128) float32
  - T_FRAMES=4 帧时序窗口
  - 每帧: 1 通道梅尔频谱 (64 mel bins × 128 time steps)
  - 对应 0.5s × 4 = 2s 音频片段

保存路径: ../data/real_audio_normal.npy

音频帧协议 (MCU → 串口):
  AA 55           帧头 2B
  seq uint16 LE   序列号 2B
  len uint16 LE   PCM 字节数 2B
  PCM int16 LE    音频负载
  AA 55           帧尾 2B
"""

import serial
import numpy as np
import sys

# ========== 配置参数 ==========
SERIAL_PORT   = '/dev/ttyUSB1'
BAUD_RATE     = 921600
T_FRAMES      = 4       # 与 AscendSentinel2 输入 (B,T,1,64,128) 的 T 对齐
SAMPLE_RATE   = 16000   # 16kHz
FRAME_SEC     = 0.5     # 每帧 0.5 秒
FRAME_SAMPLES = int(SAMPLE_RATE * FRAME_SEC)  # 8000 samples
FRAME_BYTES   = FRAME_SAMPLES * 2             # int16 = 2 bytes
N_MELS        = 64
TIME_STEPS    = 128
N_FFT         = 512

HEADER = bytes([0xAA, 0x55])
TAIL   = bytes([0xAA, 0x55])
MIN_PKT = 8  # header(2)+seq(2)+len(2)+tail(2)


def _pcm_to_mel(pcm_int16):
    """
    int16 PCM (FRAME_SAMPLES,) → (1, 64, 128) float32 归一化梅尔频谱。
    纯 numpy 实现，无需 librosa。
    """
    y = pcm_int16.astype(np.float32) / 32768.0
    hop_length = max(1, FRAME_SAMPLES // TIME_STEPS)
    window = np.hanning(N_FFT).astype(np.float32)

    pad_len = max(0, (TIME_STEPS - 1) * hop_length + N_FFT - len(y))
    y_pad = np.concatenate([y, np.zeros(pad_len, dtype=np.float32)])

    frames = np.stack([
        y_pad[i * hop_length: i * hop_length + N_FFT] * window
        for i in range(TIME_STEPS)
    ], axis=1)  # (N_FFT, TIME_STEPS)
    spec  = np.fft.rfft(frames, n=N_FFT, axis=0)
    power = (np.abs(spec) ** 2).astype(np.float32)  # (N_FFT//2+1, TIME_STEPS)

    # 梅尔滤波器组
    n_bins = N_FFT // 2 + 1
    mel_min = 2595.0 * np.log10(1.0 + 0.0 / 700.0)
    mel_max = 2595.0 * np.log10(1.0 + (SAMPLE_RATE / 2.0) / 700.0)
    mel_pts  = np.linspace(mel_min, mel_max, N_MELS + 2)
    hz_pts   = 700.0 * (10.0 ** (mel_pts / 2595.0) - 1.0)
    bin_pts  = np.floor((N_FFT + 1) * hz_pts / SAMPLE_RATE).astype(int)

    mel_basis = np.zeros((N_MELS, n_bins), dtype=np.float32)
    for m in range(1, N_MELS + 1):
        lo, mid, hi = bin_pts[m-1], bin_pts[m], bin_pts[m+1]
        for k in range(lo, mid):
            if k < n_bins:
                mel_basis[m-1, k] = (k - lo) / (mid - lo + 1e-8)
        for k in range(mid, hi):
            if k < n_bins:
                mel_basis[m-1, k] = (hi - k) / (hi - mid + 1e-8)

    mel_spec = mel_basis @ power   # (64, TIME_STEPS)
    mel_db   = 10.0 * np.log10(mel_spec + 1e-10)
    mel_db   = (mel_db - mel_db.mean()) / (mel_db.std() + 1e-8)
    mel_db   = mel_db[:, :TIME_STEPS]
    return mel_db[np.newaxis].astype(np.float32)  # (1, 64, 128)


def collect_audio_window(ser):
    """
    从串口采集 T_FRAMES 帧梅尔频谱，返回 (T_FRAMES, 1, 64, 128) float32。
    """
    sample_buf = bytearray()
    raw_buf    = bytearray()
    mel_frames = []

    while len(mel_frames) < T_FRAMES:
        data = ser.read(512)
        if data:
            raw_buf.extend(data)

        # 解析完整音频帧
        while True:
            idx = raw_buf.find(HEADER)
            if idx == -1:
                raw_buf = raw_buf[-1:]
                break
            if idx > 0:
                raw_buf = raw_buf[idx:]
            if len(raw_buf) < MIN_PKT:
                break
            payload_len = int.from_bytes(raw_buf[4:6], 'little')
            pkt_len = MIN_PKT + payload_len
            if len(raw_buf) < pkt_len:
                break
            tail_start = 6 + payload_len
            if raw_buf[tail_start:tail_start+2] != TAIL:
                raw_buf = raw_buf[2:]
                continue
            sample_buf.extend(raw_buf[6:6+payload_len])
            raw_buf = raw_buf[pkt_len:]

            # 每凑够一帧就转梅尔频谱
            while len(sample_buf) >= FRAME_BYTES:
                chunk = bytes(sample_buf[:FRAME_BYTES])
                sample_buf = sample_buf[FRAME_BYTES:]
                pcm = np.frombuffer(chunk, dtype=np.int16)
                mel = _pcm_to_mel(pcm)  # (1, 64, 128)
                mel_frames.append(mel)
                print(f"[audio] 采集帧 {len(mel_frames)}/{T_FRAMES}  "
                      f"mel min={mel.min():.3f} max={mel.max():.3f}")
                if len(mel_frames) >= T_FRAMES:
                    break

    window = np.stack(mel_frames[:T_FRAMES], axis=0)  # (T, 1, 64, 128)
    return window


def main():
    print(f"[audio] 打开串口 {SERIAL_PORT} @ {BAUD_RATE}")
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1)

    window = collect_audio_window(ser)
    ser.close()

    out_path = "../data/real_audio_normal.npy"
    np.save(out_path, window)
    print(f"[audio] 已保存: {out_path}  shape={window.shape}  "
          f"min={window.min():.4f} max={window.max():.4f}")
    print(f"  → 模型输入时 reshape 为 (1,{T_FRAMES},1,64,128)")


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\n[audio] 用户中断")
    except Exception as e:
        print(f"[audio] 错误: {e}")
        import traceback; traceback.print_exc()
        sys.exit(1)
