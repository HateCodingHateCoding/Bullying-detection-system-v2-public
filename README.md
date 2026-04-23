# Ascend Sentinel — 基于多模态感知的校园暴力智能干预系统

基于华为昇腾 AIpro（香橙派）的端侧实时校园霸凌检测系统。系统融合 60GHz 毫米波雷达点云与 MEMS 麦克风音频两路模态，通过 MindSpore 推理引擎在 Ascend 310B4 NPU 上实时判断是否发生霸凌行为，检测到后同步触发声光报警、OBS 证据存储与 IoTDA 云端告警上报，最终推送至鸿蒙 App 实现闭环处置。

---

## 系统架构

```
======================= Hardware Sensing Layer ========================

 RS6240 mmWave Radar (60GHz)         INMP441 Omni-MEMS Microphone
 |-- HIF serial output               |-- AA55 frame serial output
 |-- 5ch: x,y,z,doppler,SNR          |-- 16kHz PCM sampling
 |-- 64 points/frame                 |-- 0.5s/frame
          |                                    |
          v                                    v
    radar_receiver.py                  audio_receiver.py
    HIF parse -> (5,64)               PCM -> mel-spec (1,64,128)
          |                                    |
          +-----------------+------------------+
                            |
                            v
                     main_pipeline.py
             time-align + sliding window T=4 (2s)
             single-modality fallback + GRU memory
                            |
                            v
                    AscendSentinel2 v3
              (MindSpore, Ascend 310B4 NPU)
                            |
        +--------+----------+----------+---------+
        |        |          |          |         |
        v        v          v          v         v
     Alarm   OBS Upload   IoTDA    HTTP POST  HarmonyOS
    (buzzer) (evidence)   (MQTT)   (backend)    App

========================== Cloud Layer ============================

 Huawei IoTDA --> anq-server (Spring Boot) --WebSocket--> App
 Huawei OBS  --> Evidence video/audio storage

======================== Mobile App Layer =========================

 HarmonyOS App (AnQ)
 |-- Alert Page   : real-time WebSocket, slide-to-accept, progress
 |-- Stats Page   : historical alert data visualization
 |-- Profile Page : user settings
```

---

## 目录结构

```
├── config/
│   └── device_key.json.template   # 凭证模板（IoTDA / OBS / ModelArts）
├── scripts/
│   ├── sentinel_model.py          # 模型定义（单一真相源）
│   ├── sentinel_config.py         # 公共配置常量
│   ├── main_pipeline.py           # 主推理流水线（实时 / 离线回放 / 参数扫描）
│   ├── radar_receiver.py          # 雷达 HIF 协议串口接收与点云解析
│   ├── audio_receiver.py          # 音频 PCM 串口接收与梅尔频谱提取
│   ├── obs_uploader.py            # 华为云 OBS 证据上传
│   ├── iot_uploader.py            # 华为云 IoTDA MQTT 告警上报
│   ├── alarm.py                   # 声光报警模块串口控制
│   ├── download_audioset_subset.py    # AudioSet 训练数据下载
│   ├── serial_audio_capture.py    # 音频原始数据采集工具
│   ├── serial_radar_capture.py    # 雷达原始数据采集工具
│   └── bullying_analysis_v3_advanced.ipynb  # 训练 Notebook（最终版）
├── models/
│   ├── sentinel_best.ckpt         # 训练最优权重（F1=0.988）
│   ├── sentinel_final.ckpt        # 最终 epoch 权重
│   ├── sentinel_model.ckpt        # 部署权重
│   ├── bullying_model.ckpt        # 导出备份
│   └── bullying_model.mindir      # MindIR 导出（→ ATC → .om）
├── data/
│   ├── audio_mel.npy              # 合并后音频梅尔频谱
│   ├── radar_X.npy                # 合并后雷达点云
│   ├── radar_y.npy                # 合并后标签
│   ├── real_audio_normal.npy      # 离线回放用音频
│   ├── real_radar_normal.npy      # 离线回放用雷达
│   └── samples/datasets/
│       └── DOWNLOAD.md            # 数据集下载说明
├── logs/
│   ├── train_log.txt              # 训练日志
│   └── infer_log.txt              # 推理日志
├── backend/
│   ├── anq-server/                # Spring Boot 后端（WebSocket + REST）
│   ├── spring-boot/               # 早期后端原型
│   └── backend_api.py             # Flask 轻量告警中转 API
├── frontend/AnQ/                  # 鸿蒙 App（ArkTS）
└── SDK_Code/
    ├── RS6240雷达固件SDK/          # RS6240 毫米波雷达固件（C，裸机）
    └── HH-D02星闪派SDK/            # HH-D02 星闪 SLE 通信 SDK
```

---

## 模型：AscendSentinel2 v3

### 输入输出规格

| 输入 | Shape | 说明 |
|---|---|---|
| 音频 | `(B, 4, 1, 64, 128)` | 4帧梅尔频谱，每帧 0.5s @ 16kHz |
| 雷达 | `(B, 4, 5, 64)` | 4帧点云，5通道 (x, y, z, doppler, SNR)，64点 |
| h_prev | `(1, B, 64)` | GRU 跨窗口隐状态（可选） |

| 输出 | Shape | 说明 |
|---|---|---|
| main_logits | `(B, 2)` | 主分类（双模态融合） |
| audio_logits | `(B, 2)` | 音频独立分支（降级用） |
| radar_logits | `(B, 2)` | 雷达独立分支（降级用） |
| alpha | `(B, 1)` | MindCMA 跨模态注意力权重 |
| h_next | `(1, B, 64)` | GRU 跨窗口隐状态 |

### 子模块架构

```
AscendSentinel2
├── AudioMelCNNBranch          # 音频特征提取
│   ├── 4层 CNN: 1→32→64→128→256 channels
│   ├── BatchNorm + ReLU + MaxPool
│   ├── Global Average Pooling → 256d
│   └── 独立分类头 → audio_logits (B,2)
│
├── RadarPointNet              # 雷达特征提取
│   ├── TNet: 输入变换网络（点云对齐）
│   ├── PointNet: 共享 MLP + MaxPool → 128d
│   └── 独立分类头 → radar_logits (B,2)
│
├── MindCMA                    # 跨模态注意力融合
│   ├── Q=audio(256d), K=V=radar(128d)
│   ├── Scaled Dot-Product Attention
│   ├── alpha 权重输出 (B,1)
│   └── 融合向量: concat(audio, attended_radar, alpha) → 385d
│
├── GRUTemporalCtx             # 跨窗口时序建模
│   ├── GRU: input=385d, hidden=64d
│   ├── hidden state 跨窗口传递
│   └── 时间跳变 >1.5s 自动重置
│
└── 主分类头                    # 最终决策
    ├── Linear(385+64=449d → 128d) + ReLU + Dropout
    └── Linear(128d → 2) → main_logits
```

参数量：~2.4M | 模型定义统一在 `scripts/sentinel_model.py`

---

## 训练

### 数据来源

| 模态 | 数据集 | 说明 |
|---|---|---|
| 音频 | Google AudioSet (balanced split) | 筛选霸凌相关类别：Screaming, Crying, Slap, Shout, Whack, Children shouting |
| 雷达 | MMFi Dataset | 毫米波雷达人体动作点云，40个受试者 |

### 训练配置

- 平台：华为 ModelArts Ascend 910 NPU
- 框架：MindSpore
- Epochs：100
- 损失函数：Focal Loss（alpha_pos=0.75, alpha_neg=0.25, gamma=2.0，按类别分配权重缓解 1:3 正负样本不平衡；联合损失 = main + 0.3 x audio + 0.3 x radar）
- 模态 Dropout：15% 概率随机丢弃雷达或音频输入，迫使模型学会单模态推理
- 数据划分：先按帧切分 train/val，再各自做滑窗（无泄露）

### 训练结果

| 指标 | 值 |
|---|---|
| Best val F1 | 0.988（epoch 50，P=1.000 R=0.976） |
| 最终 val F1 | 0.949（epoch 100） |
| 后融合 F1 | 0.877（P=1.000 R=0.780） |
| 最终 loss | 1.02e-2 |
| 参数量 | 2.4M |

---

## 实时推理特性

### 时间对齐
- 音频帧为主节拍（0.5s/帧），雷达按时间窗口聚合
- 对齐容差：0.2s（`ALIGN_TOLERANCE_SEC`）
- 雷达帧聚合策略：按 SNR / 速度 / 距离综合评分选取最优帧

### GRU 跨窗口记忆
- hidden state 在连续窗口间传递，保持时序上下文
- 时间跳变 >1.5s 自动重置 hidden state（`STATE_RESET_GAP_SEC`）
- 缓冲区最大 64 步（`BUFFER_MAX_STEPS`）

### 双向单模态降级
| 场景 | 使用的输出 | 说明 |
|---|---|---|
| 双路正常 | `main_logits` | 576d 融合头，最高精度 |
| 雷达不可用 | `audio_logits` | 音频独立分支 |
| 音频不可用 | `radar_logits` | 雷达独立分支 |
| 双路都挂 | confidence=0 | 不触发报警 |

### 报警与上报
- 声光报警：串口控制外接报警模块，冷却时间 30s（`ALARM_COOLDOWN_SEC`）
- OBS 上传：证据音频/视频上传至华为云 OBS 存储桶
- IoTDA 上报：MQTT 协议上报设备属性至华为云 IoT 平台
- HTTP POST：直接推送至 anq-server 后端
- 线程健康监控：接收线程死亡自动重启

---

## 后端服务

### anq-server (Spring Boot)

主后端服务，负责接收端侧告警并推送至鸿蒙 App。

- REST API：
  - `POST /alert` — 接收 IoTDA 格式告警（兼容嵌套 services 和顶层 location）
  - `POST /alert/create` — 创建告警（JSON body）
  - `GET /alert/create?location=xxx` — 创建告警（query param，测试用）
  - `GET /alert/all` — 获取所有历史警情
- WebSocket：实时推送新警情至已连接的鸿蒙 App 客户端
- 数据持久化：JPA + H2/MySQL
- 技术栈：Spring Boot + Spring WebSocket + Spring Data JPA

### backend_api.py (Flask)

轻量级告警中转 API，用于开发调试。

- `POST /api/alarms/receive` — 接收 IoTDA 推送
- `GET /api/alarms/latest` — 前端轮询最新告警

---

## 鸿蒙 App (AnQ)

基于 HarmonyOS ArkTS 开发的移动端应用，用于安保人员实时接收和处置警情。

### 页面结构

| 页面 | 功能 |
|---|---|
| 警情页 (PageOne) | 实时接收 WebSocket 推送的警情，显示位置/时间/置信度，滑动接警，进度跟踪（告警→派发→到场→完成），处理记录 |
| 统计页 (PageTwo) | 历史警情数据可视化图表（使用 mccharts 组件） |
| 我的页 (PageThree) | 用户设置与个人信息 |

### 交互流程

```
WebSocket 推送新警情
    → App 振动提醒 + 显示警情卡片
    → 安保人员滑动接警
    → 标记"到达现场"
    → 标记"处理完毕"
    → 警情归档
```

---

## 硬件固件

### RS6240 毫米波雷达 SDK

位于 `SDK_Code/RS6240雷达固件SDK/`，基于 CB2201 芯片的裸机 C 固件。

- 芯片：CH2201 (C-SKY 架构)
- 功能：60GHz 毫米波点云输出，HIF 协议串口通信
- 驱动：GPIO, USART, SPI, I2C, Timer, WDT 等外设驱动
- 点云处理：`mmw_point_cloud` 工程，输出 (x, y, z, doppler, SNR) 五通道数据

---

## 配置说明

复制凭证模板并填入真实值：

```bash
cp config/device_key.json.template config/device_key.json
```

配置项：

| 分类 | 字段 | 说明 |
|---|---|---|
| IoTDA | device_id, client_id, username, secret | 设备接入凭证 |
| IoTDA | hostname, port, auth_mode | MQTT 连接参数 |
| OBS | obs_ak, obs_sk, obs_endpoint, obs_bucket | 对象存储凭证 |
| 设备 | location | 设备部署位置（如"校园监控点-01"） |
| ModelArts | modelarts_ak, modelarts_sk, project_id, region | 训练平台凭证（可选） |

---

## 快速开始

### 环境依赖

```
# 推理端（昇腾 AIpro）
mindspore          # MindSpore 推理框架
numpy
pyserial           # 串口通信
huaweicloudsdkiotda  # IoTDA SDK（可选）
esdk-obs-python    # OBS SDK（可选）

# 训练端（ModelArts）
mindspore          # MindSpore 训练框架
numpy

# 数据下载
datasets           # HuggingFace datasets
librosa            # 音频处理
soundfile
huggingface-hub
```

### 启动推理

```bash
cd scripts
python main_pipeline.py \
    --radar-port /dev/ttyUSB0 \
    --audio-port /dev/ttyUSB1 \
    --model ../models/sentinel_best.ckpt \
    --config ../config/device_key.json
```

### 离线回放

```bash
python main_pipeline.py \
    --mode offline \
    --audio-npy ../data/real_audio_normal.npy \
    --radar-npy ../data/real_radar_normal.npy \
    --model ../models/sentinel_best.ckpt
```

### 参数扫描

```bash
python main_pipeline.py \
    --mode sweep \
    --audio-npy ../data/real_audio_normal.npy \
    --radar-npy ../data/real_radar_normal.npy \
    --model ../models/sentinel_best.ckpt \
    --output ../scripts/output/offline_sweep_results.csv
```

### 下载训练数据

```bash
pip install datasets librosa soundfile huggingface-hub
python scripts/download_audioset_subset.py --output-dir data/audioset_bullying
```

### 启动后端

```bash
cd backend/anq-server
./mvnw spring-boot:run
```

---

## 关键配置常量

定义在 `scripts/sentinel_config.py`：

| 常量 | 值 | 说明 |
|---|---|---|
| T_FRAMES | 4 | 滑动窗口帧数 |
| FRAME_SEC | 0.5s | 每帧时长 |
| N_MELS | 64 | 梅尔滤波器组数 |
| TIME_STEPS | 128 | 频谱时间步数 |
| RADAR_CH | 5 | 雷达通道数 (x,y,z,doppler,SNR) |
| RADAR_PTS | 64 | 每帧雷达点数 |
| AUDIO_SAMPLE_RATE | 16000 | 音频采样率 |
| ALIGN_TOLERANCE_SEC | 0.20 | 时间对齐容差 |
| STATE_RESET_GAP_SEC | 1.50 | GRU 状态重置阈值 |
| ALARM_COOLDOWN_SEC | 30.0 | 报警冷却时间 |
| THRESHOLD_DEFAULT | 0.5 | 默认分类阈值 |
| MODALITY_DROPOUT_PROB | 0.15 | 训练时模态丢弃概率 |

---

## 技术栈总览

| 层级 | 技术 |
|---|---|
| 端侧推理 | Python + MindSpore + Ascend 310B4 NPU |
| 传感器通信 | pyserial (HIF / AA55 协议) |
| 云端 IoT | 华为云 IoTDA (MQTT) + OBS |
| 后端服务 | Spring Boot + WebSocket + JPA |
| 移动应用 | HarmonyOS ArkTS |
| 训练平台 | 华为 ModelArts + Ascend 910 NPU |
| 雷达固件 | C (CB2201 裸机, C-SKY 架构) |
