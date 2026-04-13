#!/usr/bin/env python3
"""
download_audioset_subset.py - 从 AudioSet 下载霸凌相关音频子集

使用方法:
  pip install yt-dlp librosa pandas
  python download_audioset_subset.py --output-dir ../data/audioset_bullying

流程:
  1. 下载 AudioSet 的 balanced/unbalanced segments CSV
  2. 按标签筛选霸凌正例和正常负例
  3. 用 yt-dlp 下载对应 YouTube 片段的音频
  4. 切成 0.5s 帧，转梅尔频谱 (1, 64, 128)，保存为 .npy

霸凌正例标签 (AudioSet ontology mid):
  /m/07pjjrj  Screaming
  /m/01d3sd   Crying, sobbing
  /m/07q2z82  Shout
  /m/07pbtc8  Slap, smack
  /t/dd00135  Whack, thwack
  /m/0bpl036  Children shouting

正常负例标签:
  /m/09x0r    Speech
  /m/01j3sz   Laughter
  /m/07rwj3x  Chatter
  /m/04rlf    Music
"""

import os
import sys
import argparse
import subprocess
import tempfile
from pathlib import Path

import numpy as np
import pandas as pd

try:
    import librosa
except ImportError:
    print("请先安装 librosa: pip install librosa")
    sys.exit(1)

# AudioSet 标签定义
BULLYING_LABELS = {
    "/m/07pjjrj": "Screaming",
    "/m/01d3sd": "Crying, sobbing",
    "/m/07q2z82": "Shout",
    "/m/07pbtc8": "Slap, smack",
    "/t/dd00135": "Whack, thwack",
    "/m/0bpl036": "Children shouting",
}

NORMAL_LABELS = {
    "/m/09x0r": "Speech",
    "/m/01j3sz": "Laughter",
    "/m/07rwj3x": "Chatter",
    "/m/04rlf": "Music",
}

# 梅尔频谱参数（与 sentinel_config.py 一致）
SR = 16000
FRAME_SEC = 0.5
N_MELS = 64
TIME_STEPS = 128

AUDIOSET_BALANCED_CSV = "http://storage.googleapis.com/us_audioset/youtube_corpus/v1/csv/balanced_train_segments.csv"
AUDIOSET_UNBALANCED_CSV = "http://storage.googleapis.com/us_audioset/youtube_corpus/v1/csv/unbalanced_train_segments.csv"
AUDIOSET_EVAL_CSV = "http://storage.googleapis.com/us_audioset/youtube_corpus/v1/csv/eval_segments.csv"


def download_csv(url, cache_dir):
    """下载 AudioSet segments CSV，有缓存则跳过。"""
    fname = Path(url).name
    local = Path(cache_dir) / fname
    if local.exists():
        print(f"  CSV 已缓存: {local}")
        return str(local)
    print(f"  下载 {url} ...")
    subprocess.check_call(["curl", "-L", "-o", str(local), url])
    return str(local)


def load_segments(csv_path):
    """解析 AudioSet segments CSV，返回 DataFrame(ytid, start, end, labels)。"""
    rows = []
    with open(csv_path, "r") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            parts = line.split(", ", 3)
            if len(parts) < 4:
                continue
            ytid = parts[0].strip()
            start = float(parts[1].strip())
            end = float(parts[2].strip())
            labels = [l.strip().strip('"') for l in parts[3].split(",")]
            rows.append({"ytid": ytid, "start": start, "end": end, "labels": labels})
    return pd.DataFrame(rows)


def filter_segments(df, target_mids):
    """筛选包含目标标签的片段。"""
    mask = df["labels"].apply(lambda ls: any(m in ls for m in target_mids))
    return df[mask].copy()


def download_audio_clip(ytid, start, end, out_path):
    """用 yt-dlp 下载 YouTube 片段的音频。"""
    url = f"https://www.youtube.com/watch?v={ytid}"
    with tempfile.NamedTemporaryFile(suffix=".wav", delete=False) as tmp:
        tmp_path = tmp.name
    try:
        cmd = [
            "yt-dlp",
            "--no-warnings", "-q",
            "-x", "--audio-format", "wav",
            "--postprocessor-args", f"-ss {start} -to {end} -ac 1 -ar {SR}",
            "-o", tmp_path,
            url,
        ]
        subprocess.run(cmd, timeout=30, capture_output=True)
        if os.path.exists(tmp_path) and os.path.getsize(tmp_path) > 1000:
            os.rename(tmp_path, out_path)
            return True
    except Exception:
        pass
    if os.path.exists(tmp_path):
        os.unlink(tmp_path)
    return False


def wav_to_mel_frames(wav_path, sr=SR, n_mels=N_MELS, time_steps=TIME_STEPS, frame_sec=FRAME_SEC):
    """将 wav 切成 0.5s 帧，每帧转梅尔频谱 (1, 64, 128)。"""
    try:
        y, _ = librosa.load(wav_path, sr=sr, mono=True)
    except Exception:
        return []
    frame_len = int(sr * frame_sec)
    hop = max(1, frame_len // time_steps)
    frames = []
    for start in range(0, len(y) - frame_len + 1, frame_len):
        chunk = y[start:start + frame_len]
        mel = librosa.feature.melspectrogram(
            y=chunk, sr=sr, n_mels=n_mels, n_fft=512, hop_length=hop
        )
        mel_db = librosa.power_to_db(mel + 1e-10)
        mel_db = (mel_db - mel_db.mean()) / (mel_db.std() + 1e-8)
        if mel_db.shape[1] >= time_steps:
            mel_db = mel_db[:, :time_steps]
        else:
            mel_db = np.pad(mel_db, ((0, 0), (0, time_steps - mel_db.shape[1])))
        frames.append(mel_db[np.newaxis].astype(np.float32))  # (1, 64, 128)
    return frames


def main():
    parser = argparse.ArgumentParser(description="下载 AudioSet 霸凌相关音频子集")
    parser.add_argument("--output-dir", default="../data/audioset_bullying")
    parser.add_argument("--max-positive", type=int, default=500,
                        help="最多下载多少条霸凌正例片段")
    parser.add_argument("--max-negative", type=int, default=500,
                        help="最多下载多少条正常负例片段")
    parser.add_argument("--use-balanced-only", action="store_true",
                        help="只用 balanced 子集（更快，约 22K 条）")
    args = parser.parse_args()

    out_dir = Path(args.output_dir)
    wav_dir = out_dir / "wav"
    cache_dir = out_dir / "cache"
    wav_dir.mkdir(parents=True, exist_ok=True)
    cache_dir.mkdir(parents=True, exist_ok=True)

    # 1. 下载 CSV
    print("=== 下载 AudioSet segments CSV ===")
    csv_paths = [download_csv(AUDIOSET_BALANCED_CSV, cache_dir)]
    if not args.use_balanced_only:
        csv_paths.append(download_csv(AUDIOSET_EVAL_CSV, cache_dir))

    # 2. 解析并筛选
    print("=== 筛选霸凌/正常片段 ===")
    all_dfs = [load_segments(p) for p in csv_paths]
    df = pd.concat(all_dfs, ignore_index=True)
    print(f"  总片段数: {len(df)}")

    pos_df = filter_segments(df, BULLYING_LABELS.keys())
    neg_df = filter_segments(df, NORMAL_LABELS.keys())
    # 去掉同时包含正负标签的
    neg_df = neg_df[~neg_df["ytid"].isin(pos_df["ytid"])]

    pos_df = pos_df.sample(n=min(args.max_positive, len(pos_df)), random_state=42)
    neg_df = neg_df.sample(n=min(args.max_negative, len(neg_df)), random_state=42)
    print(f"  霸凌正例: {len(pos_df)} 条")
    print(f"  正常负例: {len(neg_df)} 条")

    # 3. 下载音频
    print("=== 下载音频片段 ===")
    audio_mels = []
    labels = []

    for label_val, subset_df, tag in [(1, pos_df, "正例"), (0, neg_df, "负例")]:
        ok = 0
        for idx, row in subset_df.iterrows():
            wav_path = wav_dir / f"{row['ytid']}_{row['start']:.0f}.wav"
            if not wav_path.exists():
                success = download_audio_clip(
                    row["ytid"], row["start"], row["end"], str(wav_path)
                )
                if not success:
                    continue
            mel_frames = wav_to_mel_frames(str(wav_path))
            for mf in mel_frames:
                audio_mels.append(mf)
                labels.append(label_val)
            ok += 1
            if ok % 50 == 0:
                print(f"  [{tag}] 已处理 {ok}/{len(subset_df)}")

        print(f"  [{tag}] 完成: {ok} 条成功下载")

    if not audio_mels:
        print("没有成功下载任何音频，请检查网络和 yt-dlp 安装")
        return

    # 4. 保存
    audio_arr = np.stack(audio_mels).astype(np.float32)  # (N, 1, 64, 128)
    label_arr = np.array(labels, dtype=np.int32)

    # shuffle
    perm = np.random.RandomState(42).permutation(len(label_arr))
    audio_arr = audio_arr[perm]
    label_arr = label_arr[perm]

    np.save(out_dir / "audio_mel.npy", audio_arr)
    np.save(out_dir / "audio_labels.npy", label_arr)

    pos_count = (label_arr == 1).sum()
    neg_count = (label_arr == 0).sum()
    print(f"\n=== 完成 ===")
    print(f"  audio_mel.npy: {audio_arr.shape}")
    print(f"  霸凌帧: {pos_count}, 正常帧: {neg_count}")
    print(f"  保存到: {out_dir}")


if __name__ == "__main__":
    main()
