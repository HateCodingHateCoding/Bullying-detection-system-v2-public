"""
sentinel_config.py - 欺凌检测系统公共配置

所有模块共享的常量集中定义在此，避免魔数散落在多个文件中。
修改参数只需改这一个文件。
"""

# ── 模型结构参数 ──────────────────────────────────────────────────
T_FRAMES   = 4      # 时序帧数（滑动窗口长度）
N_MELS     = 64     # 梅尔频带数
TIME_STEPS = 128    # 频谱时间步数
RADAR_CH   = 5      # 雷达通道数 (x, y, z, velocity, snr)
RADAR_PTS  = 64     # 每帧雷达点数

# ── 音频参数 ──────────────────────────────────────────────────────
AUDIO_SAMPLE_RATE = 16000   # 采样率 Hz
FRAME_SEC         = 0.5     # 每帧时长（秒），与雷达帧间隔对齐
FRAME_SAMPLES     = int(AUDIO_SAMPLE_RATE * FRAME_SEC)  # 8000 samples/帧
N_FFT             = 512     # STFT FFT 窗长

# ── 实时对齐参数 ──────────────────────────────────────────────────
ALIGN_TOLERANCE_SEC   = 0.20   # 音频-雷达配对最大时间差（秒）
STATE_RESET_GAP_SEC   = 1.50   # 超过此间隔重置 GRU hidden state（秒）
BUFFER_MAX_STEPS      = 64     # 雷达包缓冲区上限
RADAR_AGG_WINDOW_SEC  = 0.50   # 按音频节拍聚合雷达帧的时间窗口（秒）

# ── 报警参数 ──────────────────────────────────────────────────────
ALARM_COOLDOWN_SEC = 30.0   # 报警冷却时间（秒）
THRESHOLD_DEFAULT  = 0.5    # 默认霸凌置信度阈值
