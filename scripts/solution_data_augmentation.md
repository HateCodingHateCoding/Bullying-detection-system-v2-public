# 方案2：数据增强降低过拟合

## 问题
- 数据集太小（2376样本）
- 模型记住了8种动作类型
- 缺乏泛化能力

## 解决方案

### 1. 增强雷达数据增强强度
```python
def augment_radar_pointcloud(pc, drop_rate=0.5, noise_count=10):  # 提高强度
    # 1. 更激进的丢点
    keep_mask = np.random.rand(n_points) > drop_rate

    # 2. 添加更多噪声
    noise = np.random.uniform(-1, 1, (noise_count, 4))

    # 3. 随机旋转（新增）
    theta = np.random.uniform(-np.pi/6, np.pi/6)
    rotation = np.array([[np.cos(theta), -np.sin(theta)],
                         [np.sin(theta), np.cos(theta)]])
    pc[:, :2] = pc[:, :2] @ rotation.T

    # 4. 随机缩放（新增）
    scale = np.random.uniform(0.8, 1.2)
    pc[:, :3] *= scale

    return pc
```

### 2. 添加Dropout和正则化
```python
self.classifier = nn.SequentialCell([
    nn.Dense(384, 256), nn.BatchNorm1d(256), nn.ReLU(),
    nn.Dropout(0.5),  # 从0.3提高到0.5
    nn.Dense(256, 128), nn.ReLU(),
    nn.Dropout(0.3),  # 新增
    nn.Dense(128, 2)
])
```

### 3. 减少模型容量
```python
# 简化分类器
self.classifier = nn.SequentialCell([
    nn.Dense(384, 128), nn.ReLU(),  # 直接降维
    nn.Dropout(0.5),
    nn.Dense(128, 2)
])
```

### 4. 使用标签平滑
```python
# 标签平滑，防止过度自信
class LabelSmoothingLoss(nn.Cell):
    def __init__(self, smoothing=0.1):
        super().__init__()
        self.smoothing = smoothing
        self.ce = nn.SoftmaxCrossEntropyWithLogits(sparse=False)

    def construct(self, pred, target):
        n_class = pred.shape[1]
        one_hot = ops.one_hot(target, n_class, 1.0, 0.0)
        smooth_label = one_hot * (1 - self.smoothing) + self.smoothing / n_class
        return self.ce(pred, smooth_label)
```

## 预期效果
- 准确率降到85-92%
- 模型更关注空间特征而非记忆动作
- 泛化能力提升
