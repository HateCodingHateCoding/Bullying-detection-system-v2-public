# Ascend Sentinel —— 基于多模态感知的校园暴力智能干预系统

基于华为昇腾 AIpro（香橙派）的端侧实时检测系统，融合 60GHz 毫米波雷达点云与麦克风音频两路模态，通过 MindSpore 推理引擎判断是否发生霸凌行为，并在检测到后同步触发声光报警、OBS 证据存储与 IoTDA 云端告警上报，最终推送至鸿蒙 App。

---

## 系统架构总览

```
硬件层
  RS6240 毫米波雷达 (60GHz)          INMP441全向麦克风
        │ SPI->D02->SLE->D02->UART        │ I2S->D02->SLE->D02->UART
        ▼                                 ▼
  radar_receiver.py                 audio_receiver.py
  解析 HIF 协议点云帧               解析 AA55 音频帧 → 梅尔频谱
  输出 (5,64) float32               输出 (1,64,128) float32
        │                                 │
        └──────────────┬──────────────────┘
                       ▼
                main_pipeline.py
          音频帧为主节拍（0.5s/帧）
          雷达按时间窗口聚合对齐
          滑动窗口 T=4 帧（2秒）
                       │
                       ▼
             AscendSentinel2 v3
     (MindSpore 2.7.0, Ascend 310B4 NPU)
      AudioCNN + RadarPointNet + MindCMA
      GRU 跨窗口时序 + 四路特征拼接 (576维)
                       │
          ┌────────────┴────────────┐
          │ confidence >= threshold │
          └────────────┬────────────┘
                       ▼
    ┌──────────────────┼──────────────────┬──────────────────┐
    ▼                  ▼                  ▼                  ▼
声光报警           OBS 上传           IoTDA 上报         HTTP POST
alarm.py           音频证据 .wav      MQTT/TLS 推送      anq-server
/dev/ttyAMA0       bullying2 桶       华为云 IoTDA       → WebSocket
                                                         → 鸿蒙 App
```

---

## 目录结构

```
Bullying/
├── config/
│   ├── device_key.json.template # 凭证模板（含 ModelArts 配置）
│   └── device_key.json          # 真实凭证（.gitignore）
├── data/
│   ├── audioset_bullying/       # AudioSet 霸凌音频子集（本地生成）
│   └── samples/datasets/        # 数据集下载说明
├── models/
│   ├── sentinel_best.ckpt       # 训练最优权重
│   ├── bullying_model.ckpt      # 导出备份权重
│   └── bullying_model.mindir    # MindIR 导出模型
├── scripts/
│   ├── sentinel_model.py        # 模型定义（单一真相源）
│   ├── sentinel_config.py       # 公共配置常量
│   ├── main_pipeline.py         # 主推理流水线（入口）
│   ├── radar_receiver.py        # 雷达串口接收 & HIF 协议解析
│   ├── audio_receiver.py        # 音频串口接收 & 梅尔频谱转换
│   ├── alarm.py                 # 声光报警串口指令
│   ├── iot_uploader.py          # 华为 IoTDA MQTT/TLS 告警上报
│   ├── obs_uploader.py          # 华为 OBS 音频证据上传
│   ├── download_audioset_subset.py  # AudioSet 数据下载工具
│   ├── serial_radar_capture.py  # 离线雷达数据采集工具
│   ├── serial_audio_capture.py  # 离线音频数据采集工具
│   └── bullying_analysis_v3_advanced.ipynb  # 模型训练 Notebook
├── backend/
│   └── anq-server/              # Spring Boot 后端（WebSocket + REST + JPA）
├── frontend/
│   └── AnQ/                     # 鸿蒙端 App（ArkTS，DevEco Studio）
├── SDK_Code/
│   ├── HH-D02星闪派SDK/         # 星闪 D02 数据转发 + 声光报警 demo
│   └── RS6240雷达固件SDK/       # RS6240 60GHz 毫米波雷达固件 SDK
└── .gitignore
```

---

## 模型架构：AscendSentinel2 v3

模型输入连续 **4 帧**（2 秒）传感器数据，输出霸凌概率。定义统一在 `scripts/sentinel_model.py`。

| 输入 | Shape | 说明 |
|---|---|---|
| 音频 | `(B, 4, 1, 64, 128)` | 4帧梅尔频谱，每帧 0.5s @ 16kHz |
| 雷达 | `(B, 4, 5, 64)` | 4帧点云，5通道(x,y,z,doppler,SNR)，64点 |

| 输出 | Shape | 说明 |
|---|---|---|
| 主分类 logits | `(B, 2)` | 二分类（正常/霸凌）|
| 音频辅助 logits | `(B, 2)` | 音频独立分支 |
| 雷达辅助 logits | `(B, 2)` | 雷达独立分支 |
| 融合权重 α | `(B, 1)` | MindCMA 注意力权重 |
| GRU hidden state | `(1, B, 64)` | 跨窗口时序状态 |

子模块：AudioMelCNNBranch (4层CNN→256d) · RadarPointNet (TNet+PointNet→128d) · MindCMA (跨模态注意力融合) · GRUTemporalCtx (跨窗口时序) · 主分类头 (576d→2cls)

---

## 训练数据与结果

| 模态 | 数据集 | 说明 |
|---|---|---|
| 雷达 | MMFi (mmWave) | TI IWR1443 毫米波雷达点云，A20-A23 投掷/踢腿作为霸凌代理 |
| 音频 | Google AudioSet 子集 | Screaming/Crying/Slap/Shout 等标签，从 HuggingFace 下载 |
| 增强 | Sim-to-Real + 模态 Dropout | 随机丢点/鬼影注入/SpecAugment，15%概率丢弃单模态 |

训练平台：华为 ModelArts（Ascend 910 NPU），100 epochs，Focal Loss 联合训练

| 指标 | 值 |
|---|---|
| 最终 loss | 4.57e-3 |
| 验证 F1 | 0.988（P=1.000 R=0.976）|
| 模型参数量 | ~2.4M |

---

## 实时推理特性

- **时间对齐**：音频帧为主节拍，雷达按 0.5s 窗口聚合（tolerance 0.2s）
- **GRU 跨窗口记忆**：hidden state 连续传递，时间跳变 >1.5s 自动重置
- **单模态降级**：雷达或音频不可用时，自动切换到对应的独立分支头（audio_head / radar_head）推理
- **线程健康监控**：接收线程死亡后自动重启
- **结构化日志**：Python logging 模块，支持日志级别控制

---

## 快速开始

### 1. 配置凭证

```bash
cp config/device_key.json.template config/device_key.json
# 编辑填入 IoTDA/OBS/ModelArts 的 AK/SK 等信息
```

### 2. 启动推理

```bash
cd scripts
python main_pipeline.py \
    --radar-port /dev/ttyUSB0 \
    --audio-port /dev/ttyUSB1 \
    --alarm-port /dev/ttyAMA0 \
    --model ../models/sentinel_best.ckpt \
    --config ../config/device_key.json
```

### 3. 下载训练数据（可选）

```bash
pip install huggingface-hub pyarrow soundfile librosa
python scripts/download_audioset_subset.py --output-dir data/audioset_bullying
```

### 4. 重新训练（可选）

在 ModelArts Notebook 上执行 `bullying_analysis_v3_advanced.ipynb` Cell 1-7。
