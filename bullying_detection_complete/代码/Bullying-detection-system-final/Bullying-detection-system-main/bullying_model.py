#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
校园霸凌检测多模态融合模型 - MindSpore 1.10 修复版
"""

import mindspore as ms
import mindspore.nn as nn
import mindspore.ops as ops
from mindspore import Tensor
import numpy as np


class AudioFeatureExtractor(nn.Cell):
    """声音特征提取模块"""
    def __init__(self, n_mels=64, time_steps=128):
        super(AudioFeatureExtractor, self).__init__()
        
        self.conv1 = nn.Conv2d(1, 32, kernel_size=3, stride=1, padding=1, pad_mode='pad', has_bias=True)
        self.bn1 = nn.BatchNorm2d(32)
        self.conv2 = nn.Conv2d(32, 64, kernel_size=3, stride=1, padding=1, pad_mode='pad', has_bias=True)
        self.bn2 = nn.BatchNorm2d(64)
        self.conv3 = nn.Conv2d(64, 128, kernel_size=3, stride=1, padding=1, pad_mode='pad', has_bias=True)
        self.bn3 = nn.BatchNorm2d(128)
        self.conv4 = nn.Conv2d(128, 256, kernel_size=3, stride=1, padding=1, pad_mode='pad', has_bias=True)
        self.bn4 = nn.BatchNorm2d(256)
        
        self.pool = nn.MaxPool2d(kernel_size=2, stride=2)
        self.relu = nn.ReLU()
        self.dropout = nn.Dropout(keep_prob=0.7)
        
        self.flatten = nn.Flatten()
        self.fc = nn.Dense(256 * 4 * 8, 256)
        
    def construct(self, x):
        x = self.pool(self.relu(self.bn1(self.conv1(x))))
        x = self.pool(self.relu(self.bn2(self.conv2(x))))
        x = self.pool(self.relu(self.bn3(self.conv3(x))))
        x = self.pool(self.relu(self.bn4(self.conv4(x))))
        x = self.flatten(x)
        x = self.dropout(x)
        x = self.fc(x)
        return x


class VibrationFeatureExtractor(nn.Cell):
    """振动特征提取模块 - 简化版，移除BatchNorm1d"""
    def __init__(self, input_dim=6, hidden_dim=128, num_layers=2, seq_len=200):
        super(VibrationFeatureExtractor, self).__init__()
        
        self.conv1d_1 = nn.Conv1d(input_dim, 32, kernel_size=5, stride=1, padding=2, pad_mode='pad', has_bias=True)
        self.conv1d_2 = nn.Conv1d(32, 64, kernel_size=5, stride=1, padding=2, pad_mode='pad', has_bias=True)
        self.relu = nn.ReLU()
        self.pool = nn.MaxPool1d(kernel_size=2, stride=2)
        
        # 使用LSTM
        self.lstm = nn.LSTM(
            input_size=64,
            hidden_size=hidden_dim,
            num_layers=num_layers,
            batch_first=True,
            dropout=0.3,
            bidirectional=True
        )
        
        self.fc = nn.Dense(hidden_dim * 2, 128)
        self.dropout = nn.Dropout(keep_prob=0.7)
        
    def construct(self, x):
        # x: (batch, seq_len, 6)
        x = ops.Transpose()(x, (0, 2, 1))  # (batch, 6, seq_len)
        
        # 1D CNN (不用BatchNorm)
        x = self.pool(self.relu(self.conv1d_1(x)))
        x = self.pool(self.relu(self.conv1d_2(x)))
        
        x = ops.Transpose()(x, (0, 2, 1))  # (batch, seq_len/4, 64)
        
        # LSTM
        lstm_out, _ = self.lstm(x)
        
        # 取最后一个时间步
        x = lstm_out[:, -1, :]
        x = self.dropout(x)
        x = self.fc(x)
        return x


class CrossModalAttention(nn.Cell):
    """跨模态注意力机制"""
    def __init__(self, audio_dim=256, vibration_dim=128, hidden_dim=128):
        super(CrossModalAttention, self).__init__()
        
        self.audio_proj = nn.Dense(audio_dim, hidden_dim)
        self.vibration_proj = nn.Dense(vibration_dim, hidden_dim)
        self.attention = nn.Dense(hidden_dim * 2, 1)
        self.tanh = nn.Tanh()
        self.sigmoid = nn.Sigmoid()
        
    def construct(self, audio_feat, vibration_feat):
        audio_proj = self.tanh(self.audio_proj(audio_feat))
        vibration_proj = self.tanh(self.vibration_proj(vibration_feat))
        
        concat = ops.Concat(axis=1)((audio_proj, vibration_proj))
        attention_weights = self.sigmoid(self.attention(concat))
        
        fused = attention_weights * audio_proj + (1 - attention_weights) * vibration_proj
        return fused, attention_weights


class TemporalContextModule(nn.Cell):
    """时序上下文模块"""
    def __init__(self, input_dim=128, hidden_dim=64):
        super(TemporalContextModule, self).__init__()
        
        self.fc1 = nn.Dense(input_dim, hidden_dim)
        self.fc2 = nn.Dense(hidden_dim, 64)
        self.relu = nn.ReLU()
        
    def construct(self, x):
        x = self.relu(self.fc1(x))
        x = self.fc2(x)
        return x


class BullyingDetectionModel(nn.Cell):
    """校园霸凌检测主模型"""
    def __init__(self, n_mels=64, audio_time_steps=128, vibration_seq_len=200):
        super(BullyingDetectionModel, self).__init__()
        
        self.audio_extractor = AudioFeatureExtractor(n_mels, audio_time_steps)
        self.vibration_extractor = VibrationFeatureExtractor(
            input_dim=6, hidden_dim=128, seq_len=vibration_seq_len
        )
        
        self.cross_attention = CrossModalAttention(
            audio_dim=256, vibration_dim=128, hidden_dim=128
        )
        
        self.temporal_context = TemporalContextModule(input_dim=128, hidden_dim=64)
        
        # 分类头
        self.fc1 = nn.Dense(128 + 64 + 256 + 128, 256)
        self.fc2 = nn.Dense(256, 128)
        self.fc3 = nn.Dense(128, 64)
        self.fc4 = nn.Dense(64, 1)
        self.relu = nn.ReLU()
        self.dropout1 = nn.Dropout(keep_prob=0.5)
        self.dropout2 = nn.Dropout(keep_prob=0.7)
        self.sigmoid = nn.Sigmoid()
        
    def construct(self, audio, vibration):
        audio_feat = self.audio_extractor(audio)
        vibration_feat = self.vibration_extractor(vibration)
        
        fused_feat, attention_weights = self.cross_attention(audio_feat, vibration_feat)
        temporal_feat = self.temporal_context(fused_feat)
        
        all_features = ops.Concat(axis=1)([fused_feat, temporal_feat, audio_feat, vibration_feat])
        
        x = self.dropout1(self.relu(self.fc1(all_features)))
        x = self.dropout2(self.relu(self.fc2(x)))
        x = self.relu(self.fc3(x))
        probability = self.sigmoid(self.fc4(x))
        
        return probability, attention_weights


class BullyingDetectionWithLoss(nn.Cell):
    """带损失函数的模型"""
    def __init__(self, network, pos_weight=2.0):
        super(BullyingDetectionWithLoss, self).__init__()
        self.network = network
        self.pos_weight = pos_weight
        
    def construct(self, audio, vibration, label):
        probability, _ = self.network(audio, vibration)
        probability = probability.squeeze(-1)
        
        loss = -self.pos_weight * label * ops.Log()(probability + 1e-7) -                (1 - label) * ops.Log()(1 - probability + 1e-7)
        return loss.mean()


class BullyingDetectionWithFocalLoss(nn.Cell):
    """使用Focal Loss的模型"""
    def __init__(self, network, alpha=0.25, gamma=2.0):
        super(BullyingDetectionWithFocalLoss, self).__init__()
        self.network = network
        self.alpha = alpha
        self.gamma = gamma
        
    def construct(self, audio, vibration, label):
        probability, _ = self.network(audio, vibration)
        pred = probability.squeeze(-1)
        
        pt = pred * label + (1 - pred) * (1 - label)
        focal_weight = ops.Pow()((1 - pt), self.gamma)
        alpha_weight = self.alpha * label + (1 - self.alpha) * (1 - label)
        
        bce = -label * ops.Log()(pred + 1e-7) - (1 - label) * ops.Log()(1 - pred + 1e-7)
        loss = alpha_weight * focal_weight * bce
        
        return loss.mean()


def create_model(config=None):
    if config is None:
        config = {}
    
    n_mels = config.get('n_mels', 64)
    audio_time_steps = config.get('audio_time_steps', 128)
    vibration_seq_len = config.get('vibration_seq_len', 200)
    
    model = BullyingDetectionModel(
        n_mels=n_mels,
        audio_time_steps=audio_time_steps,
        vibration_seq_len=vibration_seq_len
    )
    
    return model


if __name__ == "__main__":
    print(f"MindSpore 版本: {ms.__version__}")
    model = create_model()
    print("模型创建成功!")
