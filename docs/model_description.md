# AscendSentinel2 v3 模型说明文档

## 1. 概述

AscendSentinel2 v3 是一个面向**校园欺凌检测**的多模态时序深度学习模型，运行于华为昇腾 NPU（香橙派 AI Pro）。模型融合**音频梅尔频谱**与**毫米波雷达点云**两路传感器信号，通过跨模态注意力机制和 GRU 时序建模，对连续4帧传感器数据进行欺凌行为判断。

训练框架：MindSpore（GRAPH_MODE，device_target=Ascend）
部署格式：昇腾 .om 模型（ckpt → MindIR → ATC 转换）
推理平台：香橙派 AI Pro（昇腾 310B4）

---

## 2. 输入输出规格

| 项目 | 规格 |
|------|------|
| 音频输入 | `(B, T, 1, 64, 128)` — B=batch, T=4帧, 1通道, 64梅尔频带, 128时间步 |
| 雷达输入 | `(B, T, 5, 64)` — T=4帧, 5特征(x,y,z,v,intensity), 64个点 |
| 主输出 | `(B, 2)` logits — 二分类（欺凌/正常）|
| 音频辅助输出 | `(B, 2)` logits — 仅音频分支分类头 |
| 雷达辅助输出 | `(B, 2)` logits — 仅雷达分支分类头 |
| 注意力权重 | `(B, 1)` alpha — 跨模态融合权重（0→偏雷达，1→偏音频）|

推理时 B=1，T=4（4帧滑窗），输出取 `Softmax(main_logits)[0,1]` 作为欺凌概率。

---

## 3. 整体架构

```
音频输入 (B,T,1,64,128)          雷达输入 (B,T,5,64)
        │                                │
   展平时间维 (B*T,1,64,128)      展平时间维 (B*T,5,64)
        │                                │
 AudioMelCNNBranch              RadarPointNet
  4层CNN + GAP                TNet变换 + PointNet
        │                                │
    (B*T, 256)                   (B*T, 128) + radar_logits
        │                                │
        └──────── MindCMA_seq ───────────┘
                       │
                   (B*T, 128)
                       │
              reshape (B, T, 128)
                       │
              GRUTemporalCtx
                       │
                   (B, 64) tc

最后帧特征:
  af_last (B,256)  rf_last (B,128)
        └──── MindCMA_cls ────┘
                  │
             (B,128) fused_last + alpha

主分类头输入: concat[fused_last(128), tc(64), af_last(256), rf_last(128)] = (B, 576)
主分类头输出: (B, 2)
```

---

## 4. 各子模块详解

### 4.1 AudioMelCNNBranch（音频分支）

**输入**：`(B, 1, 64, 128)` 单帧梅尔频谱图

**结构**：
```
Conv2d(1→32, 3×3) + BN + ReLU + MaxPool(2×2)
Conv2d(32→64, 3×3) + BN + ReLU + MaxPool(2×2)
Conv2d(64→128, 3×3) + BN + ReLU + MaxPool(2×2)
Conv2d(128→256, 3×3) + BN + ReLU + MaxPool(2×2)
AdaptiveAvgPool2d(1,1) → Flatten
Dense(256→256) + ReLU + Dropout(0.3)
```

**输出**：`(B, 256)` 音频语义特征向量

4次 MaxPool(2×2) 后空间分辨率为 4×8，AdaptiveAvgPool 压缩到 1×1，对空间位置做全局平均，提取全局音频语义。

**独立音频头**：`Dense(256→64) + ReLU + Dense(64→2)`，输出 `(B,2)` 用于辅助监督。

---

### 4.2 T-Net（输入变换网络）

**作用**：学习一个 5×5 的空间变换矩阵，对雷达点云做对齐，提升对旋转/平移的鲁棒性（PointNet 标准组件）。

**输入**：`(B, 5, 64)` 雷达点云

**结构**：
```
Conv1d(5→64) + BN + ReLU
Conv1d(64→128) + BN + ReLU
Conv1d(128→256) + BN + ReLU
AdaptiveMaxPool1d(1) → Flatten → (B, 256)
Dense(256→128) + ReLU
Dense(128→25) → view(B, 5, 5)
变换矩阵 = 输出 + I（残差单位矩阵，防止退化）
```

**输出**：`(B, 5, 5)` 变换矩阵，用 `bmm(T_mat, x)` 作用于点云。

---

### 4.3 RadarPointNet（雷达分支）

**输入**：`(B, 5, 64)` 雷达点云（经 T-Net 变换后）

**结构**：
```
T-Net → bmm 对齐
MLP1: Conv1d(5→64) + BN + ReLU → Conv1d(64→64) + BN + ReLU
MLP2: Conv1d(64→128) + BN + ReLU → Conv1d(128→256) + BN + ReLU
AdaptiveMaxPool1d(1) → Flatten → (B, 256)
Dense(256→128) + BN + ReLU + Dropout(0.3)
```

**输出**：
- 特征向量 `(B, 128)`
- 雷达辅助分类头 `(B, 2)`：`Dense(128→64) + ReLU + Dense(64→2)`

AdaptiveMaxPool 做全局最大池化，提取点云中最显著的局部特征（欺凌场景中快速移动的点）。

---

### 4.4 MindCMA（跨模态注意力融合）

**作用**：动态计算音频和雷达特征的融合权重 alpha，实现自适应跨模态融合。

**输入**：音频特征 `af (B,256)`，雷达特征 `rf (B,128)`

**结构**：
```
ap = Dense(256→128) + Tanh   # 音频投影
rp = Dense(128→128) + Tanh   # 雷达投影
alpha = Sigmoid(Dense(64→1)(ReLU(Dense(256→64)(concat[ap, rp]))))
输出特征 = alpha * ap + (1-alpha) * rp
```

**输出**：融合特征 `(B, 128)`，注意力权重 `alpha (B, 1)`

alpha 接近 1 表示当前场景音频更可信（如语音清晰），接近 0 表示雷达更可信（如嘈杂环境）。

**模型中有两个独立的 MindCMA 实例**：
- `cma_seq`：用于所有帧的融合，输出送 GRU 做时序建模
- `cma_cls`：用于最后帧的融合，输出直接参与分类决策

两者参数独立，优化目标不同，避免梯度干扰。

---

### 4.5 GRUTemporalCtx（GRU 时序上下文）

**作用**：对4帧的融合特征序列建模时序依赖，捕捉欺凌行为的时序演变特征。

**输入**：`(B, T, 128)` — T=4帧的跨模态融合特征序列

**结构**：
```
GRU(input_size=128, hidden_size=64, batch_first=True)
取最后时刻隐状态 h[-1]: (B, 64)
```

**输出**：`(B, 64)` 时序上下文向量

---

### 4.6 主分类头

**输入**：4路特征拼接
```
fused_last (B, 128)  — 最后帧跨模态融合
tc         (B,  64)  — GRU时序上下文
af_last    (B, 256)  — 最后帧音频特征
rf_last    (B, 128)  — 最后帧雷达特征
→ concat → (B, 576)
```

**结构**：
```
Dense(576→256) + BN + ReLU + Dropout(0.5)
Dense(256→128) + BN + ReLU + Dropout(0.3)
Dense(128→64)  + ReLU
Dense(64→2)
```

**输出**：`(B, 2)` logits

---

## 5. 数据处理

### 5.1 音频预处理

- 原始音频按雷达帧间隔（0.5秒）切分为独立帧，每帧提取梅尔频谱图 `(1, 64, 128)`
  - 64 梅尔频带，128 时间步（n_fft=512，hop 自适应，sr=16000）
- 4帧独立mel频谱堆叠 → `(T, 1, 64, 128)`，T=4
- 每帧独立标准化（均值0，标准差1）

### 5.2 雷达预处理

- 每帧雷达点云：最多64个点，每点5维特征 `(x, y, z, v, intensity)`
- 不足64点补零，超过截断 → `(5, 64)`
- 逐特征归一化（经验均值/标准差）
- 滑窗4帧 → `(T, 5, 64)`

### 5.3 数据增强（训练时）

**音频 SpecAugment**：
- 时间掩码：每帧随机遮盖0~20个时间步
- 频率掩码：每帧随机遮盖0~10个频带

**雷达坐标抖动**：
- 50%概率对 x,y,z 通道加 N(0, 0.05) 高斯噪声

### 5.4 数据集划分（无泄露滑窗）

关键设计：**先按帧序列切分 train/val，再各自做滑窗**。
```
全部帧: [0, N)
train帧: [0, split_n)   → 训练滑窗
val帧:   [split_n, N)   → 验证滑窗
```
train 和 val 的滑窗不共享任何帧，彻底避免数据泄露。
默认 TRAIN_RATIO=0.8，滑窗步长=1，窗口 label 取最后帧。

---

## 6. 训练配置

| 超参数 | 值 |
|--------|----|
| batch size | 16 |
| epochs | 100 |
| 优化器 | Adam，weight_decay=1e-4 |
| 初始学习率 | 1e-3 |
| LR 调度 | Warmup(5ep) + Cosine Decay，最低 lr=1e-5 |
| 联合损失权重 | main + 0.3×audio + 0.3×radar |
| 保存策略 | 每5epoch在完整验证集评估 F1，按最高 val F1 保存 best.ckpt |

### 6.1 Focal Loss（Softmax 版）

$$\mathcal{L} = -\alpha (1 - p_t)^\gamma \cdot \text{CE}(\text{logits}, y)$$

- $\alpha=0.25$，$\gamma=2.0$
- $p_t$：Softmax 后真实类别的概率（用 one-hot gather 取，GRAPH_MODE 兼容）
- 用 `SoftmaxCrossEntropyWithLogits(sparse=True)` 计算基础 CE
- 适合欺凌/正常样本不均衡场景（欺凌约占25%）

### 6.2 联合损失

$$\mathcal{L}_{total} = \mathcal{L}_{main} + 0.3 \cdot \mathcal{L}_{audio} + 0.3 \cdot \mathcal{L}_{radar}$$

三路辅助监督确保音频分支和雷达分支不退化为无效特征提取器，提升单模态失效时的鲁棒性。

### 6.3 LR 调度

```
前5 epoch: 线性 warmup，0 → 1e-3
后95 epoch: Cosine Decay，1e-3 → 1e-5
```

---

## 7. 推理流程（边侧部署）

### 7.1 数据采集（香橙派边侧）

```
K01传感器 --[星闪SLE/串口]--> 香橙派
  ├─ 0x01 包: 雷达点云帧 (N×5 float32)
  └─ 0x02 包: 音频PCM (int16)
```

### 7.2 滑窗缓冲

- 雷达：维护4帧 deque，每帧归一化后补/截到64点 → `(4,5,64)`
- 音频：每0.5秒积累一帧PCM，满帧后计算mel频谱推入帧队列（deque maxlen=4）→ `(4, frame_samples)`

### 7.3 输入组装

```python
# 4帧独立PCM → 各自提取mel → 堆叠
audio_input = extract_audio_features(audio_4frames)  # (1,4,1,64,128)
radar_input = radar_window                            # (1,4,5,64)
```

每帧音频独立计算梅尔频谱，GRU 能感知真实的4帧音频时序变化。

### 7.4 ACL 推理

1. 将 audio/radar numpy array 拷贝到昇腾设备内存
2. 调用 `acl.mdl.execute()` 执行 .om 模型
3. 读取输出 buffer，取第0个输出的第一个值作为欺凌概率

### 7.5 后融合三级报警

验证阶段使用**后融合**（Late Fusion）策略，综合音频子头和雷达子头的独立预测：

$$p_{fused} = \frac{w_a \cdot p_{audio} + w_r \cdot p_{radar}}{w_a + w_r}$$

其中：
- $w_a = \text{clip}(\text{SNR}_{dB} / 30, 0.1, 1.0)$ — SNR 低时降低音频权重
- $w_r = \text{clip}(\text{radar\_density}, 0.1, 1.0)$ — 点云稀疏时降低雷达权重

| 级别 | 阈值 | 数字码 | 动作 |
|------|------|--------|------|
| RED | $p_{fused} \geq 0.75$ | 3 | 立即上报 IoTDA + 本地声光报警 |
| YELLOW | $0.45 \leq p_{fused} < 0.75$ | 2 | 上报可疑事件，请求人工复核 |
| GREEN | $p_{fused} < 0.45$ | 0 | 正常，静默 |

---

## 8. 模型导出（ckpt → .om）

```python
# Step 1: 导出 MindIR（在 ModelArts/训练环境执行）
import mindspore as ms
from mindspore.train.serialization import export
dummy_audio = ms.Tensor(np.zeros((1,4,1,64,128), dtype=np.float32))
dummy_radar = ms.Tensor(np.zeros((1,4,5,64),    dtype=np.float32))
export(net, dummy_audio, dummy_radar,
       file_name='./output/bullying_model', file_format='MINDIR')
```

```bash
# Step 2: ATC 转换（在香橙派终端执行，需安装 CANN）
atc \
  --model=./output/bullying_model.mindir \
  --framework=1 \
  --output=./output/bullying_model \
  --soc_version=Ascend310B4 \
  --input_shape="audio:1,4,1,64,128;radar:1,4,5,64" \
  --log=error
# 生成: ./output/bullying_model.om
```

```bash
# Step 3: 部署
scp ./output/bullying_model.om orangepi@<IP>:/home/orangepi/models/
# 修改 edge_inference.py:
# Config.MODEL_PATH = '/home/orangepi/models/bullying_model.om'
```

---

## 9. IoTDA 上报格式

### 属性上报（周期性）

```json
{
  "services": [{
    "service_id": "bullying_detection",
    "properties": {
      "probability": 0.82,
      "alarm_level": 3,
      "attention_weight": 0.65,
      "timestamp": 1711234567000,
      "location": "教学楼A-3楼-男厕所",
      "device_id": "node_001"
    }
  }]
}
```

### 事件上报（触发报警时）

```json
{
  "services": [{
    "service_id": "alarm",
    "event_type": "bullying_detected",
    "paras": {
      "probability": 0.82,
      "alarm_level": 3,
      "location": "教学楼A-3楼-男厕所",
      "device_id": "node_001",
      "audio_features": {...},
      "radar_features": {...}
    }
  }]
}
```

`alarm_level` 数字码：**3=RED，2=YELLOW，0=GREEN**（与 notebook LateAlertFusion 和 edge_inference.py 三端统一）。

---

## 10. 文件结构

```
Bullying/
├── scripts/
│   └── bullying_analysis_v3_advanced.ipynb  # 训练 notebook（Cell 1-7）
├── docs/archive/代码/bullying_detection/
│   ├── edge/
│   │   └── edge_inference.py                # 香橙派边侧推理
│   └── iotda/
│       └── iotda_client.py                  # 华为云 IoTDA 对接
└── docs/
    └── model_description.md                 # 本文档
```

---

## 11. 参数量估算

| 模块 | 参数量（约） |
|------|-------------|
| AudioMelCNNBranch | ~1.5M |
| TNet (5×5) | ~0.1M |
| RadarPointNet | ~0.3M |
| MindCMA_seq + MindCMA_cls | ~0.2M |
| GRUTemporalCtx | ~0.05M |
| 主分类头 (576→2) | ~0.2M |
| **合计** | **~2.4M** |

适合昇腾 310B4边侧推理，单次推理延迟约 5~15ms（不含数据预处理）。