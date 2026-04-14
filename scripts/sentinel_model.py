"""
sentinel_model.py - AscendSentinel2 霸凌检测模型定义（单一真相源）

所有模型类从 notebook Cell 3 提取。pipeline 和部署统一从此文件导入。
修改模型结构时只改这个文件（和 notebook 保持同步）。

依赖: mindspore (在 Ascend NPU 或 CPU 上运行)
"""

try:
    import mindspore
    import mindspore.nn as nn
    import mindspore.ops as ops
    from mindspore import Tensor
    import numpy as np
    HAS_MINDSPORE = True
except ImportError:
    HAS_MINDSPORE = False

if not HAS_MINDSPORE:
    raise ImportError(
        "sentinel_model 需要 MindSpore。"
        "如果在非 Ascend 环境测试，请安装 mindspore CPU 版本。"
    )

# 从公共配置导入，fallback 到默认值
try:
    from sentinel_config import T_FRAMES, N_MELS, TIME_STEPS, RADAR_CH, RADAR_PTS
except ImportError:
    T_FRAMES = 4
    N_MELS = 64
    TIME_STEPS = 128
    RADAR_CH = 5
    RADAR_PTS = 64


# ═══════════════════════════════════════════════════════════════════
# 音频分支: 4层CNN → 256维
# ═══════════════════════════════════════════════════════════════════
class AudioMelCNNBranch(nn.Cell):
    def __init__(self):
        super().__init__()
        self.conv = nn.SequentialCell([
            nn.Conv2d(1, 32, 3, pad_mode="same"), nn.BatchNorm2d(32), nn.ReLU(), nn.MaxPool2d(2, 2),
            nn.Conv2d(32, 64, 3, pad_mode="same"), nn.BatchNorm2d(64), nn.ReLU(), nn.MaxPool2d(2, 2),
            nn.Conv2d(64, 128, 3, pad_mode="same"), nn.BatchNorm2d(128), nn.ReLU(), nn.MaxPool2d(2, 2),
            nn.Conv2d(128, 256, 3, pad_mode="same"), nn.BatchNorm2d(256), nn.ReLU(), nn.MaxPool2d(2, 2),
        ])
        self.gap = nn.AdaptiveAvgPool2d((1, 1))
        self.flat = nn.Flatten()
        self.fc = nn.SequentialCell([nn.Dense(256, 256), nn.ReLU(), nn.Dropout(p=0.3)])

    def construct(self, x):
        return self.fc(self.flat(self.gap(self.conv(x))))


# ═══════════════════════════════════════════════════════════════════
# T-Net: 输入变换网络
# ═══════════════════════════════════════════════════════════════════
class TNet(nn.Cell):
    def __init__(self, k=5):
        super().__init__()
        self.k = k
        self.mlp = nn.SequentialCell([
            nn.Conv1d(k, 64, 1), nn.BatchNorm1d(64), nn.ReLU(),
            nn.Conv1d(64, 128, 1), nn.BatchNorm1d(128), nn.ReLU(),
            nn.Conv1d(128, 256, 1), nn.BatchNorm1d(256), nn.ReLU(),
        ])
        self.fc = nn.SequentialCell([nn.Dense(256, 128), nn.ReLU(), nn.Dense(128, k * k)])
        self.pool = nn.AdaptiveMaxPool1d(1)
        self.flat = nn.Flatten()

    def construct(self, x):
        B = x.shape[0]
        feat = self.flat(self.pool(self.mlp(x)))
        mat = self.fc(feat).view(B, self.k, self.k)
        eye = ops.eye(self.k, self.k, mindspore.float32).unsqueeze(0).broadcast_to((B, -1, -1))
        return mat + eye


# ═══════════════════════════════════════════════════════════════════
# RadarPointNet: TNet + PointNet全局MaxPool → 128维 + 独立雷达头
# ═══════════════════════════════════════════════════════════════════
class RadarPointNet(nn.Cell):
    def __init__(self):
        super().__init__()
        self.tnet = TNet(k=5)
        self.mlp1 = nn.SequentialCell([
            nn.Conv1d(5, 64, 1), nn.BatchNorm1d(64), nn.ReLU(),
            nn.Conv1d(64, 64, 1), nn.BatchNorm1d(64), nn.ReLU(),
        ])
        self.mlp2 = nn.SequentialCell([
            nn.Conv1d(64, 128, 1), nn.BatchNorm1d(128), nn.ReLU(),
            nn.Conv1d(128, 256, 1), nn.BatchNorm1d(256), nn.ReLU(),
        ])
        self.pool = nn.AdaptiveMaxPool1d(1)
        self.flat = nn.Flatten()
        self.feat_fc = nn.SequentialCell([
            nn.Dense(256, 128), nn.BatchNorm1d(128), nn.ReLU(), nn.Dropout(p=0.3)
        ])
        self.radar_head = nn.SequentialCell([nn.Dense(128, 64), nn.ReLU(), nn.Dense(64, 2)])

    def construct(self, x):
        T_mat = self.tnet(x)
        x = ops.bmm(T_mat, x)
        x = self.mlp1(x)
        x = self.mlp2(x)
        feat = self.feat_fc(self.flat(self.pool(x)))
        return feat, self.radar_head(feat)


# ═══════════════════════════════════════════════════════════════════
# MindCMA: 跨模态注意力融合
# ═══════════════════════════════════════════════════════════════════
class MindCMA(nn.Cell):
    def __init__(self, audio_dim=256, radar_dim=128, unified=128):
        super().__init__()
        self.ap = nn.SequentialCell([nn.Dense(audio_dim, unified), nn.Tanh()])
        self.rp = nn.SequentialCell([nn.Dense(radar_dim, unified), nn.Tanh()])
        self.attn = nn.SequentialCell([nn.Dense(unified * 2, 64), nn.ReLU(), nn.Dense(64, 1)])
        self.sig = nn.Sigmoid()

    def construct(self, af, rf):
        ap = self.ap(af)
        rp = self.rp(rf)
        alpha = self.sig(self.attn(ops.concat([ap, rp], 1)))
        return alpha * ap + (1 - alpha) * rp, alpha


# ═══════════════════════════════════════════════════════════════════
# GRU 时序上下文: 支持跨窗口 hidden state 传递
# ═══════════════════════════════════════════════════════════════════
class GRUTemporalCtx(nn.Cell):
    def __init__(self, input_dim=128, hidden=64):
        super().__init__()
        self.gru = nn.GRU(input_size=input_dim, hidden_size=hidden, batch_first=True)

    def construct(self, x, h_prev=None):
        if h_prev is None:
            out, h_next = self.gru(x)
        else:
            out, h_next = self.gru(x, h_prev)
        return out[:, -1, :], h_next  # (B, hidden), (1, B, hidden)


# ═══════════════════════════════════════════════════════════════════
# AscendSentinel2 v3: 多模态多帧霸凌检测主模型
#
# 输入:  audio (B, T, 1, 64, 128)   radar (B, T, 5, 64)   h_prev (可选)
# 输出:  main_logits (B,2), audio_logits (B,2), radar_logits (B,2),
#        alpha (B,1), h_next (1,B,64)
# ═══════════════════════════════════════════════════════════════════
class AscendSentinel2(nn.Cell):
    def __init__(self, t_frames=T_FRAMES):
        super().__init__()
        self.T = t_frames
        self.ab = AudioMelCNNBranch()
        self.rb = RadarPointNet()
        self.cma_seq = MindCMA()
        self.cma_cls = MindCMA()
        self.gru = GRUTemporalCtx(input_dim=128, hidden=64)
        self.audio_head = nn.SequentialCell([nn.Dense(256, 64), nn.ReLU(), nn.Dense(64, 2)])
        self.clf = nn.SequentialCell([
            nn.Dense(576, 256), nn.BatchNorm1d(256), nn.ReLU(), nn.Dropout(p=0.5),
            nn.Dense(256, 128), nn.BatchNorm1d(128), nn.ReLU(), nn.Dropout(p=0.3),
            nn.Dense(128, 64), nn.ReLU(),
            nn.Dense(64, 2),
        ])

    def construct(self, audio, radar, h_prev=None):
        B, T = audio.shape[0], self.T
        a_flat = audio.view(B * T, 1, N_MELS, TIME_STEPS)
        r_flat = radar.view(B * T, RADAR_CH, RADAR_PTS)
        # 各分支特征提取
        af_flat = self.ab(a_flat)                          # (B*T, 256)
        rf_flat, rl_flat = self.rb(r_flat)                 # (B*T, 128), (B*T, 2)
        # 跨模态融合 → GRU 时序
        fused_flat, _ = self.cma_seq(af_flat, rf_flat)     # (B*T, 128)
        fused_seq = fused_flat.view(B, T, 128)
        tc, h_next = self.gru(fused_seq, h_prev)           # (B, 64), (1, B, 64)
        # 取最后帧用于分类
        af_last = af_flat.view(B, T, 256)[:, T - 1, :]
        rf_last = rf_flat.view(B, T, 128)[:, T - 1, :]
        rl_last = rl_flat.view(B, T, 2)[:, T - 1, :]
        fused_last, alpha = self.cma_cls(af_last, rf_last)
        # 主分类头
        feat = ops.concat([fused_last, tc, af_last, rf_last], 1)  # (B, 576)
        main_logits = self.clf(feat)
        audio_logits = self.audio_head(af_last)
        return main_logits, audio_logits, rl_last, alpha, h_next
