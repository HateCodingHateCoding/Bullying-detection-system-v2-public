# 方案3：混合动作标签，打破记忆模式

## 核心思路
打破"A22=霸凌"的简单映射，让模型学习更复杂的特征组合

## 实施方法

### 1. 重新标注部分数据
```python
# 将部分"霸凌动作"标记为正常（模拟玩闹场景）
# 将部分"正常动作"标记为异常（模拟轻微冲突）

ACTION_MAP_MIXED = {
    'A22': [1, 1, 1, 0],  # 75%霸凌，25%正常
    'A23': [1, 1, 1, 0],
    'A20': [1, 1, 0, 0],  # 50%霸凌，50%正常
    'A21': [1, 1, 0, 0],
    'A01': [0, 0, 0, 1],  # 75%正常，25%异常
    'A02': [0, 0, 0, 1],
    'A03': [0, 0, 0, 0],  # 100%正常
    'A04': [0, 0, 0, 0],
}

# 数据加载时随机选择标签
for act, label_pool in ACTION_MAP_MIXED.items():
    for f in files:
        label = np.random.choice(label_pool)
        radar_X.append(pc)
        radar_y.append(label)
```

### 2. 添加音频-雷达对齐约束
```python
# 强制模型同时使用雷达和音频
# 只有当雷达检测到推搡 + 音频检测到尖叫时，才判定为霸凌

class MultiModalLoss(nn.Cell):
    def __init__(self):
        super().__init__()
        self.ce = nn.SoftmaxCrossEntropyWithLogits(sparse=True)

    def construct(self, radar_feat, audio_feat, fused_logits, label):
        # 主损失
        main_loss = self.ce(fused_logits, label)

        # 模态一致性损失（雷达和音频特征应该相似）
        consistency_loss = ops.mse_loss(radar_feat, audio_feat)

        return main_loss + 0.1 * consistency_loss
```

## 预期效果
- 准确率降到70-85%
- 模型被迫学习更细粒度的特征
- 但可能引入标注噪声
