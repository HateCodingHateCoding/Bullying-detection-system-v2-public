#!/usr/bin/env python3
"""
download_audioset_subset.py - 从 HuggingFace AudioSet 提取霸凌相关音频子集

使用方法:
  pip install datasets soundfile librosa numpy torch torchcodec
  python download_audioset_subset.py --output-dir ../data/audioset_bullying

流程 (streaming 模式，不下载完整分片):
  1. 用 datasets streaming 逐条读取 agkphysics/AudioSet balanced split
  2. 按 human_labels 筛选霸凌正例和正常负例
  3. 解码音频 → 重采样 16kHz → 切 0.5s 帧 → 梅尔频谱 (1, 64, 128)
  4. 保存为 .npy
"""

import os
import sys
import argparse
from pathlib import Path

import numpy as np

try:
    import librosa
except ImportError:
    print("请先安装: pip install librosa")
    sys.exit(1)

# 标签定义
BULLYING_LABELS = {"Screaming", "Crying, sobbing", "Shout", "Slap, smack",
                   "Whack, thwack", "Children shouting"}
NORMAL_LABELS = {"Speech", "Laughter", "Chatter", "Music"}

SR = 16000
FRAME_SEC = 0.5
N_MELS = 64
TIME_STEPS = 128


def audio_to_mel_frames(y, sr=SR):
    """将音频波形切成 0.5s 帧，每帧转梅尔频谱 (1, 64, 128)。"""
    frame_len = int(sr * FRAME_SEC)
    hop = max(1, frame_len // TIME_STEPS)
    frames = []
    for start in range(0, len(y) - frame_len + 1, frame_len):
        chunk = y[start:start + frame_len]
        mel = librosa.feature.melspectrogram(
            y=chunk, sr=sr, n_mels=N_MELS, n_fft=512, hop_length=hop
        )
        mel_db = librosa.power_to_db(mel + 1e-10)
        mel_db = (mel_db - mel_db.mean()) / (mel_db.std() + 1e-8)
        if mel_db.shape[1] >= TIME_STEPS:
            mel_db = mel_db[:, :TIME_STEPS]
        else:
            mel_db = np.pad(mel_db, ((0, 0), (0, TIME_STEPS - mel_db.shape[1])))
        frames.append(mel_db[np.newaxis].astype(np.float32))
    return frames


def classify_labels(human_labels):
    """返回 1(霸凌), 0(正常), None(无关)。"""
    labels_set = set(human_labels) if isinstance(human_labels, (list, np.ndarray)) else set()
    if labels_set & BULLYING_LABELS:
        return 1
    if labels_set & NORMAL_LABELS:
        return 0
    return None


def main():
    parser = argparse.ArgumentParser(description="从 HuggingFace AudioSet 提取霸凌音频子集 (streaming)")
    parser.add_argument("--output-dir", default="../data/audioset_bullying")
    parser.add_argument("--max-positive", type=int, default=300)
    parser.add_argument("--max-negative", type=int, default=300)
    args = parser.parse_args()

    from datasets import load_dataset

    out_dir = Path(args.output_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    print(f"=== Streaming AudioSet balanced split ===")
    print(f"目标: 正例 {args.max_positive} 条, 负例 {args.max_negative} 条")

    ds = load_dataset("agkphysics/AudioSet", "balanced", split="train", streaming=True)

    all_mels = []
    all_labels = []
    pos_count = 0
    neg_count = 0
    processed = 0

    for sample in ds:
        if pos_count >= args.max_positive and neg_count >= args.max_negative:
            break

        label = classify_labels(sample["human_labels"])
        if label is None:
            continue
        if label == 1 and pos_count >= args.max_positive:
            continue
        if label == 0 and neg_count >= args.max_negative:
            continue

        try:
            audio = sample["audio"]
            y = np.array(audio["array"], dtype=np.float32)
            sr = audio["sampling_rate"]
            # 转单声道
            if y.ndim > 1:
                y = y.mean(axis=1)
            # 重采样
            if sr != SR:
                y = librosa.resample(y, orig_sr=sr, target_sr=SR)

            frames = audio_to_mel_frames(y)
            if not frames:
                continue
        except Exception as e:
            continue

        for mf in frames:
            all_mels.append(mf)
            all_labels.append(label)

        if label == 1:
            pos_count += 1
        else:
            neg_count += 1

        processed += 1
        if processed % 20 == 0:
            print(f"  已处理 {processed} 条 | 正例={pos_count} 负例={neg_count}")

    if not all_mels:
        print("没有提取到任何音频帧")
        return

    audio_arr = np.stack(all_mels).astype(np.float32)
    label_arr = np.array(all_labels, dtype=np.int32)

    perm = np.random.RandomState(42).permutation(len(label_arr))
    audio_arr = audio_arr[perm]
    label_arr = label_arr[perm]

    np.save(out_dir / "audio_mel.npy", audio_arr)
    np.save(out_dir / "audio_labels.npy", label_arr)

    print(f"\n=== 完成 ===")
    print(f"  audio_mel.npy: {audio_arr.shape}")
    print(f"  霸凌帧: {(label_arr == 1).sum()}, 正常帧: {(label_arr == 0).sum()}")
    print(f"  保存到: {out_dir}")


if __name__ == "__main__":
    main()
