# Ascend Sentinel —— 基于多模态感知的校园暴力智能干预系统

基于华为昇腾 AIpro（香橙派）的端侧实时检测系统，融合 60GHz 毫米波雷达点云与麦克风音频两路模态，通过 MindSpore 推理引擎判断是否发生霸凌行为，并在检测到后同步触发声光报警、OBS 证据存储与 IoTDA 云端告警上报，最终推送至鸿蒙 App。

---

## 系统架构

```
  RS6240 毫米波雷达 (60GHz)          INMP441 全向麦克风
        │ HIF 协议                        │ AA55 帧协议
        ▼                                 ▼
  radar_receiver.py                 audio_receiver.py
  点云解析 → (5,64)                 PCM → 梅尔频谱 (1,64,128)
        │                                 │
        └──────────────┬──────────────────┘
                       ▼
                main_pipeline.py
          时间对齐 + 滑动窗口 T=4 帧（2秒）
          单模态降级 + GRU 跨窗口记忆
                       ▼
             AscendSentinel2 v3
     (MindSpore, Ascend 310B4 NPU)
                       ▼
    ┌──────────┬───────────┬──────────┬──────────┐
    ▼          ▼           ▼          ▼          ▼
 声光报警   OBS上传    IoTDA上报   HTTP POST  鸿蒙App
```

---

## 目录结构

```
├── config/
│   ├── device_key.json.template # 凭证模板（含 ModelArts 配置）
│   └── device_key.json          # 真实凭证（.gitignore）
├── models/
│   ├── sentinel_best.ckpt       # 训练最优权重（F1=0.988）
│   ├── bullying_model.ckpt      # 导出备份
│   └── bullying_model.mindir    # MindIR 导出（→ ATC → .om）
├── scripts/
│   ├── sentinel_model.py        # 模型定义（单一真相源）
│   ├── sentinel_config.py       # 公共配置常量
│   ├── main_pipeline.py         # 主推理流水线
│   ├── radar_receiver.py        # 雷达串口接收
│   ├── audio_receiver.py        # 音频串口接收
│   ├── download_audioset_subset.py  # AudioSet 数据下载
│   └── bullying_analysis_v3_advanced.ipynb  # 训练 Notebook
├── backend/anq-server/          # Spring Boot 后端
├── frontend/AnQ/                # 鸿蒙 App
└── SDK_Code/                    # 硬件固件 SDK
```

---

## 模型：AscendSentinel2 v3

| 输入 | Shape | 说明 |
|---|---|---|
| 音频 | `(B, 4, 1, 64, 128)` | 4帧梅尔频谱，每帧 0.5s @ 16kHz |
| 雷达 | `(B, 4, 5, 64)` | 4帧点云，5通道(x,y,z,doppler,SNR)，64点 |

| 输出 | Shape | 说明 |
|---|---|---|
| main_logits | `(B, 2)` | 主分类（双模态融合） |
| audio_logits | `(B, 2)` | 音频独立分支（降级用） |
| radar_logits | `(B, 2)` | 雷达独立分支（降级用） |
| alpha | `(B, 1)` | MindCMA 跨模态注意力权重 |
| h_next | `(1, B, 64)` | GRU 跨窗口隐状态 |

子模块：AudioMelCNNBranch (4层CNN→256d) · RadarPointNet (TNet+PointNet→128d) · MindCMA (跨模态注意力) · GRUTemporalCtx (跨窗口时序) · 主分类头 (576d→2cls)

模型定义统一在 `scripts/sentinel_model.py`，notebook 和 pipeline 共享。

---

## 训练

数据：MMFi mmWave 雷达点云 + Google AudioSet 音频子集（Screaming/Crying/Slap/Shout）
平台：华为 ModelArts Ascend 910 NPU，100 epochs，Focal Loss

训练特性：
- 模态 Dropout：15% 概率随机丢弃雷达或音频，模型学会单模态推理
- 联合损失：main + 0.3×audio + 0.3×radar
- 无泄露滑窗：先按帧切分 train/val，再各自做滑窗

| 指标 | 值 |
|---|---|
| Best val F1 | 0.988（epoch 50，P=1.000 R=0.976） |
| 最终 val F1 | 0.949（epoch 100） |
| 后融合 F1 | 0.877（P=1.000 R=0.780） |
| 最终 loss | 1.02e-2 |
| 参数量 | ~2.4M |

---

## 实时推理特性

- **时间对齐**：音频帧为主节拍（0.5s），雷达按时间窗口聚合（tolerance 0.2s）
- **GRU 跨窗口记忆**：hidden state 连续传递，时间跳变 >1.5s 自动重置
- **双向单模态降级**：
  - 雷达不可用 → 用 `audio_logits`（音频独立头）
  - 音频不可用 → 用 `radar_logits`（雷达独立头）
  - 双路正常 → 用 `main_logits`（576d 融合头）
  - 双路都挂 → confidence=0，不触发报警
- **线程健康监控**：接收线程死亡自动重启
- **结构化日志**：Python logging 模块

---

## 快速开始

```bash
# 1. 配置凭证
cp config/device_key.json.template config/device_key.json

# 2. 启动推理
cd scripts
python main_pipeline.py \
    --radar-port /dev/ttyUSB0 \
    --audio-port /dev/ttyUSB1 \
    --model ../models/sentinel_best.ckpt \
    --config ../config/device_key.json

# 3. 下载训练数据（可选）
pip install huggingface-hub pyarrow soundfile librosa
python download_audioset_subset.py --output-dir ../data/audioset_bullying
```
