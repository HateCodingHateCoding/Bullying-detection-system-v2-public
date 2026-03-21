# Ascend Sentinel —— 基于多模态感知的校园暴力智能干预系统

基于华为昇腾 AIpro（香橙派）的端侧实时检测系统，融合 60GHz 毫米波雷达点云与麦克风音频两路模态，通过 MindSpore 推理引擎判断是否发生霸凌行为，并在检测到后同步触发声光报警、OBS 证据存储与 IoTDA 云端告警上报，最终推送至鸿蒙 App。

---

## 系统架构总览

​```text
硬件层
  RS6240 毫米波雷达 (60GHz)          INMP441全向麦克风
        │ SPI->D02->SLE->D02->UART        │ I2S->D02->SLE->D02->UART
        ▼                                 ▼
  radar_receiver.py                 audio_receiver.py
  解析 HIF 协议点云帧               解析 AA55 音频帧
  输出原始 (N,5) float32            输出 [1,1,16000] float32
        │                                 │
        └──────────────┬──────────────────┘
                       ▼
                main_pipeline.py
          雷达滑动窗口（20帧≈1s）
          点云合并 → 随机采样64点 → [1,5,64]
          音频帧为主节拍（1s一次）
                       │
                       ▼
             AscendSentinel2
          (MindSpore 2.7.0, Ascend NPU)
                       │
          ┌────────────┴────────────┐
          │ pred_class==1           │
          │ confidence >= 0.8       │
          └────────────┬────────────┘
                       ▼
    ┌──────────────────┼──────────────────┬──────────────────┐
    ▼                  ▼                  ▼                  ▼
声光报警           OBS 上传           IoTDA 上报         HTTP POST
alarm.py           音频证据 .wav      MQTT 推送          anq-server
/dev/ttyAMA0       bullying2 桶       状态同步           → WebSocket
                                                         → 鸿蒙 App
```

------

## 目录结构

Plaintext



```
Bullying/
├── config/
│   ├── device_key.json          # 真实凭证（已加入 .gitignore）
│   └── device_key.json.template # 凭证模板
├── data/
│   ├── real_radar_normal.npy    # 真实硬件采集雷达数据 [1,5,64]
│   ├── real_audio_normal.npy    # 真实硬件采集音频数据 [1,1,16000]
│   └── samples/MMFi/
│       ├── DOWNLOAD.md          # 数据集下载说明
│       ├── README.md            # MMFi 数据集说明
│       └── MMFi_action_segments.csv  # MMFi 数据集的动作分段标注文件
├── logs/
│   ├── train_log.txt            # 训练日志（从 ModelArts Notebook 提取）
│   └── infer_log.txt            # 推理日志示例
├── models/
│   └── sentinel_model.ckpt      # 训练好的模型权重
├── scripts/
│   ├── main_pipeline.py         # 主推理流水线（入口）
│   ├── radar_receiver.py        # 雷达串口接收 & HIF 解析
│   ├── audio_receiver.py        # 音频串口接收 & 帧解析
│   ├── alarm.py                 # 声光报警串口指令
│   ├── iot_uploader.py          # 华为 IoTDA MQTT 上报
│   ├── obs_uploader.py          # 华为 OBS 音频证据上传
│   ├── serial_radar_capture.py  # 离线雷达数据采集工具
│   ├── serial_audio_capture.py  # 离线音频数据采集工具
│   └── bullying_analysis_v3_advanced.ipynb  # 模型训练 Notebook
├── backend/
│   └── anq-server/              # Spring Boot 后端（WebSocket + REST）
├── frontend/
│   └── AnQ/                     # 鸿蒙端 App（ArkTS）
├── SDK_Code/
│   ├── HH-D02星闪派SDK
│   └── RS6240雷达固件SDK
└── .gitignore
```

------

## 训练结果

训练平台：华为 ModelArts（Ascend 910）

训练数据：MMFi（雷达）+ MIVIA_DB4（音频）

| **指标**   | **值**                            |
| ---------- | --------------------------------- |
| 训练轮数   | 15 epoch                          |
| batch_size | 32                                |
| 学习率     | 0.001（Adam + weight_decay=1e-4） |
| 最终 loss  | < 1e-5（SoftmaxCrossEntropy）     |
| 推理速度   | ~17ms/次（Ascend NPU）            |

------

## 模型说明

### 网络结构：AscendSentinel2（含 MindCMA 跨模态注意力融合）

四路特征拼接 + 深层分类器：

- **雷达分支**（RadarPointNetBranch）：`PointNetSetAbstraction(5→[32,64]) → PointNetSetAbstraction(64→[64,128]) → GlobalAvgPool → Flatten`，输入 `[B,5,64]`，输出 128维
- **音频分支**（AudioCNNBranch）：`Conv1d(1→32,stride=16) → BN → ReLU → MaxPool(4) → Conv1d(32→64) → BN → ReLU → MaxPool(4) → Conv1d(64→128) → ... → GlobalAvgPool → Flatten`，输入 `[B,1,16000]`，输出 256维
- **MindCMA 融合**：将音频(256维)与雷达(128维)各自投影到128维语义空间，经轻量MLP计算可学习权重 α，动态融合输出 128维
- **时序上下文**（TemporalContextModule）：对融合特征做时序卷积，输出 64维
- **分类器**：拼接四路特征（128+64+256+128=576维）→ `Dense(576→256) → BN → ReLU → Dropout(0.5) → Dense(256→128) → BN → ReLU → Dropout(0.3) → Dense(128→64) → ReLU → Dense(64→2)`，输出 2 类（0=正常，1=霸凌）

### 训练数据

- **雷达**：MMFi 数据集，A20-A23（投掷/踢腿）为霸凌，其余为正常
- **音频**：MIVIA_DB4 音频事件数据集（打斗、尖叫等）
- **Sim-to-Real 增强**：随机丢点（30%）+ 鬼影噪声点

### 训练步骤（ModelArts）

1. 上传 MMFi + MIVIA_DB4 数据至 OBS `bullying2` 桶的 `raw_data/` 目录
2. 在 ModelArts 创建 Notebook（mindspore_2.7.0rc1-cann_8.2.rc1-py_3.11-euler_2.10.11-aarch64-snt9b）
3. 打开 `scripts/bullying_analysis_v3_advanced.ipynb`，依次执行 Cell 1-5
4. 训练完成后权重自动保存为 `sentinel_model.ckpt`，下载至 `models/` 目录
5. 执行 Cell 6 查看验证集正反例分析

------

## 实时推理链路详解

### 雷达接收协议（HIF）

Plaintext



```
"motion_point_cloud\0" (19B)
uint16 LE  点数 N（可变，最大 ~200点/帧）
N × [x, y, z, velocity, snr]  每字段 float32 LE
```

`radar_receiver.py` 输出每帧原始点云 `(N, 5) float32`。

### 音频接收协议

Plaintext



```
AA 55 | uint16LE seq | uint16LE payload_len | PCM(int16 LE) | AA 55
```

`audio_receiver.py` 累积 16000 采样（1秒）输出 `[1,1,16000] float32`。

### 模态对齐

雷达 20Hz（50ms/帧），音频 1秒一帧。滑动窗口 20 帧覆盖 1 秒，与音频时间窗对齐，与训练数据语义一致。

------

## 闭环报警链路

Plaintext



```
pred_class==1  AND  confidence >= 0.8
        │
        └── 冷却时间已过（默认10s）
              │
              ├─ 1. 串口 AA55011E → /dev/ttyAMA0（声光报警）
              ├─ 2. OBS 上传音频证据 .wav → bullying2
              ├─ 3. IoTDA 发布告警状态 → MQTT 云端
              └─ 4. HTTP POST /alert/ → anq-server → WebSocket → 鸿蒙 App
```

------

## 快速复现（香橙派 AIpro）

### 1. 环境准备

**系统镜像选择：** 强烈推荐使用官网 Ubuntu 22.04 最新镜像：

- [百度网盘下载 (提取码: k2zs)](https://pan.baidu.com/s/1mH_2tBV-EBvZ7cJxhSS6GA?pwd=k2zs)
- *(文件名为：opiaipro_ubuntu22.04_desktop_aarch64_20250925.img.xz)*

**依赖安装：**

Bash



```
pip install mindspore==2.7.0rc1 pyserial requests esdk-obs-python paho-mqtt
```

> 雷达固件与星闪派D02固件的二次开发SDK参考本仓库 `SDK_Code` 文件夹，内附官方开源完整SDK链接。

### 2. 配置凭证

复制模板并填入真实凭证：

Bash



```
cp config/device_key.json.template config/device_key.json
# 编辑 config/device_key.json，填入：
# 1. IoTDA: device_id / secret / mqtt_endpoint
# 2. OBS: ak / sk / bucket / endpoint
# 3. Server: hostname
```

### 3. 硬件接线

| **设备**           | **接口与传输路径**                 | **香橙派设备节点** |
| ------------------ | ---------------------------------- | ------------------ |
| RS6240 雷达        | SPI 接 D02——SLE——D02 接 TTL 转 USB | `/dev/ttyUSB0`     |
| INMP441 全向麦克风 | I2S 接 D02——SLE——D02 接 TTL 转 USB | `/dev/ttyUSB1`     |
| 声光报警模块       | GPIO UART 接 D02——SLE——D02 接 UART | `/dev/ttyAMA0`     |

### 4. 启动主流水线

Bash



```
cd scripts
python main_pipeline.py \
    --radar-port /dev/ttyUSB0 \
    --audio-port /dev/ttyUSB1 \
    --alarm-port /dev/ttyAMA0 \
    --model ../models/sentinel_model.ckpt \
    --config ../config/device_key.json \
    --alert-server [http://115.120.205.25:8080/alert/](http://115.120.205.25:8080/alert/)
```

### 5. 后端服务

Spring Boot 后端已部署在 `115.120.205.25:8080`，关键端点：

- `POST /alert/` — 接收告警，存库并通过 WebSocket 广播给 App
- `GET /alert/latest` — 查询最新告警
- `WebSocket ws://115.120.205.25:8080/ws/alert` — App 订阅端点

本地运行（需 Java 17+）：

Bash



```
cd backend/anq-server
mvn spring-boot:run
```

### 6. 前端 App

用 DevEco Studio 打开 `frontend/AnQ/`，连接鸿蒙设备后编译运行。

App 启动时自动连接 WebSocket，检测到欺凌时实时弹出警情通知并振动。

------

## 注意事项

- 雷达帧率约20Hz，滑动窗口 20 帧覆盖约 1 秒，与音频帧时间窗口对齐。
- 无 Ascend NPU 时自动降级为随机占位推理（仅用于通信链路调试）。
- MMFi 数据集需自行申请下载，见 `data/samples/MMFi/DOWNLOAD.md`。
- `serial_audio_capture.py` 包尾应为 `AA 55`（与帧头相同），解析时需注意校验。
