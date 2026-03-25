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
             AscendSentinel2 v3
     (MindSpore 2.7.0, Ascend 310B4 NPU)
      多模态融合 + MindCMA 跨模态注意力
      GRU 时序建模 + 四路特征拼接 (576维)
                       │
          ┌────────────┴────────────┐
          │ pred_class==1           │
          │ confidence >= 0.8       │
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
│   ├── device_key.json          # 真实凭证（已加入 .gitignore）
│   └── device_key.json.template # 凭证模板
├── data/
│   ├── real_radar_normal.npy    # 真实硬件采集雷达数据 [1,5,64]
│   ├── real_audio_normal.npy    # 真实硬件采集音频数据 [1,1,16000]
│   ├── audio_mel.npy            # 训练输入音频梅尔特征
│   ├── radar_X.npy              # 训练输入雷达特征
│   ├── radar_y.npy              # 训练标签
│   └── samples/datasets/
│       ├── DOWNLOAD.md          # 数据集下载说明
│       └── README.md            # 数据集说明
├── logs/
│   ├── train_log.txt            # 训练日志（从 ModelArts Notebook 提取）
│   └── infer_log.txt            # 推理日志示例
├── models/
│   ├── bullying_model.mindir    # 导出的 MindIR 模型
│   ├── sentinel_best.ckpt       # 训练最优权重
│   ├── sentinel_final.ckpt      # 训练最终权重
│   └── bullying_model.ckpt      # 导出前/备份 ckpt
├── scripts/
│   ├── main_pipeline.py         # 主推理流水线（入口）
│   ├── radar_receiver.py        # 雷达串口接收 & HIF 协议解析
│   ├── audio_receiver.py        # 音频串口接收 & AA55 帧解析
│   ├── alarm.py                 # 声光报警串口指令（UART /dev/ttyAMA0）
│   ├── iot_uploader.py          # 华为 IoTDA MQTT/TLS 告警上报
│   ├── obs_uploader.py          # 华为 OBS 音频证据上传
│   ├── serial_radar_capture.py  # 离线雷达数据采集工具
│   ├── serial_audio_capture.py  # 离线音频数据采集工具
│   └── bullying_analysis_v3_advanced.ipynb  # 模型训练 Notebook（Cell 1-7）
├── backend/
│   ├── anq-server/              # Spring Boot 后端（WebSocket + REST + JPA）
│   └── spring-boot/             # Spring Boot 备用/对比后端
├── frontend/
│   └── AnQ/                     # 鸿蒙端 App（ArkTS，DevEco Studio）
├── SDK_Code/
│   ├── HH-D02星闪派SDK/         # 星闪 D02 雷达数据转发 + 声光报警反馈 demo
│   └── RS6240雷达固件SDK/       # RS6240 60GHz 毫米波雷达固件 SDK
└── .gitignore
```

---

## 训练结果

训练平台：华为 ModelArts（Ascend 910）
训练数据：MMFi（雷达）+ MIVIA_DB4（音频）

| 指标 | 值 |
|---|---|
| 训练轮数 | 15 epoch |
| batch_size | 32 |
| 学习率 | 0.001（Adam + weight_decay=1e-4）|
| 最终 loss | < 1e-5（SoftmaxCrossEntropy）|
| 推理速度 | 5~15ms/次（Ascend 310B4 NPU，不含预处理） |
| 模型总参数量 | ~2.4M |

增量微调（Incremental Fine-tuning）支持：Cell 7 实现对新采集数据的在线微调，可重新导出 MindIR → ATC → .om 部署到香橙派。

---

## 模型说明

### 网络结构：AscendSentinel2 v3（含 MindCMA 跨模态注意力融合 + GRU 时序）

模型输入为连续 **4 帧** 传感器数据（T=4 的滑窗），输出霸凌概率及三路辅助 logits。

| 输入 | Shape | 说明 |
|---|---|---|
| 音频 | `(B, T, 1, 64, 128)` | T=4帧梅尔频谱，64梅尔频带，128时间步 |
| 雷达 | `(B, T, 5, 64)` | T=4帧点云，5特征(x,y,z,v,intensity)，64点 |

| 输出 | Shape | 说明 |
|---|---|---|
| 主分类 logits | `(B, 2)` | 二分类（正常/霸凌）|
| 音频辅助 logits | `(B, 2)` | 仅音频分支输出 |
| 雷达辅助 logits | `(B, 2)` | 仅雷达分支输出 |
| 融合注意力权重 α | `(B, 1)` | 0→偏雷达，1→偏音频 |

**各子模块：**

- **AudioMelCNNBranch**（音频分支）：4 层 CNN（`1→32→64→128→256`）+ BN + ReLU + MaxPool + GlobalAvgPool，输入 `(B,1,64,128)` 单帧梅尔频谱，输出 256 维特征
- **RadarPointNet**（雷达分支）：TNet 空间变换 + 两层 PointNet SetAbstraction（`5→[32,64]→[64,128]`）+ GlobalAvgPool，输入 `(B,5,64)` 点云，输出 128 维特征及辅助雷达 logits
- **MindCMA_seq**（时序跨模态注意力）：音频(256维)与雷达(128维)各投影至 128 维语义空间，轻量 MLP 计算可学习权重 α，动态融合得 128 维，逐帧处理后 reshape 为 `(B, T, 128)`
- **GRUTemporalCtx**（时序上下文）：GRU 对 T 帧融合特征建模，输出 64 维时序上下文 tc
- **MindCMA_cls**（分类跨模态注意力）：对最后一帧的 af_last(256) 与 rf_last(128) 再次跨模态注意力融合，得 128 维 fused_last
- **主分类头**：拼接四路特征 `[fused_last(128), tc(64), af_last(256), rf_last(128)]`（共 576 维）→ `Dense(576→256) → BN → ReLU → Dropout(0.5) → Dense(256→128) → BN → ReLU → Dropout(0.3) → Dense(128→64) → ReLU → Dense(64→2)`，输出 2 类（0=正常，1=霸凌）

### 训练数据

- **雷达**：MMFi 数据集，A20-A23（投掷/踢腿）标注为霸凌，其余为正常
- **音频**：MIVIA Audio Surveillance DB4（打斗、尖叫等声音事件）
- **Sim-to-Real 增强**：随机丢点（30%）+ 鬼影噪声点注入

### 训练步骤（ModelArts Notebook）

1. 在 `scripts/bullying_analysis_v3_advanced.ipynb` 中按 Cell 1-7 顺序执行
2. Cell 1：安装 librosa/tqdm
3. Cell 2：数据预处理（MMFi 雷达点云 + MIVIA 音频 → npy）
4. Cell 3：定义 AscendSentinel2 v3 模型（含 MindCMA + GRU）
5. Cell 4：在 ModelArts Ascend 910 上训练，保存 `best_pretrain.ckpt`
6. Cell 5：导出 MindIR → ATC 转换 → `.om` 模型
7. Cell 6：端侧验证推理（香橙派 310B4）
8. Cell 7：增量微调（新数据在线 finetune，保存 `best_finetune.ckpt`）

---

## 快速开始

### 1. 环境要求

```
- 香橙派 AI Pro（昇腾 310B4）
- MindSpore 2.7.0（Ascend 后端）
- Python 3.11
- 依赖：librosa, numpy, scipy, pyserial, paho-mqtt, esdk-obs-python, requests
```

### 2. 配置凭证

```bash
cp config/device_key.json.template config/device_key.json
# 编辑 config/device_key.json，填入 device_id/secret/hostname 和 OBS ak/sk/bucket
```

`device_key.json` 字段说明：

| 字段 | 说明 |
|---|---|
| `device_id` | 华为 IoTDA 设备 ID |
| `secret` | IoTDA 设备密钥 |
| `iot_platform_url` | IoTDA MQTT 接入地址 |
| `obs_ak` / `obs_sk` | OBS 访问密钥 |
| `obs_bucket` | OBS 存储桶名（默认 `bullying2`）|
| `obs_endpoint` | OBS 地域端点 |
| `location` | 设备安装位置描述 |

### 3. 硬件接线

| 设备 | 接口 | 香橙派设备节点 |
|---|---|---|
| RS6240 雷达 | SPI 接 D02——SLE——D02 接 TTL 转 USB | /dev/ttyUSB0 |
| INMP441 全向麦克风 | I2S 接 D02——SLE——D02 接 TTL 转 USB | /dev/ttyUSB1 |
| 声光报警模块 | GPIO UART 接 D02——SLE——D02 接 UART | /dev/ttyAMA0 |

### 4. 启动主流水线

```bash
cd scripts
python main_pipeline.py \
    --radar-port /dev/ttyUSB0 \
    --audio-port /dev/ttyUSB1 \
    --alarm-port /dev/ttyAMA0 \
    --model ../models/sentinel_model.ckpt \
    --config ../config/device_key.json \
    --alert-server http://115.120.205.25:8080/alert/
```

主要参数：

| 参数 | 默认值 | 说明 |
|---|---|---|
| `--radar-port` | `/dev/ttyUSB0` | 雷达串口 |
| `--audio-port` | `/dev/ttyUSB1` | 音频串口 |
| `--alarm-port` | `/dev/ttyAMA0` | 声光报警串口 |
| `--model` | `../models/sentinel_model.ckpt` | 模型权重路径 |
| `--config` | `../config/device_key.json` | 凭证配置文件 |
| `--alert-server` | — | anq-server HTTP 地址 |
| `--threshold` | `0.8` | 霸凌置信度触发阈值 |
| `--alarm-cooldown` | `10` | 报警冷却时间（秒）|

无硬件时可用 mock 脚本调试完整链路：

```bash
python scripts/moke_infer_full.py    # 含 OBS + HTTP 上报
python scripts/moke_infer_iot.py     # 含 IoTDA 上报
python scripts/moke_infer_simple.py  # 最简推理验证
```

### 5. 后端服务（anq-server）

Spring Boot 后端已部署在 `115.120.205.25:8080`，关键端点：

| 端点 | 方法 | 说明 |
|---|---|---|
| `/alert/` | POST | 接收告警（支持平铺 JSON 或 IoTDA services 嵌套格式），存库并 WebSocket 广播 |
| `/alert/create` | GET/POST | 测试用，快速创建一条警情 |
| `/alert/latest` | GET | 查询最新告警 |
| `ws://…/ws/alert` | WebSocket | App 订阅端点，实时推送 Alert JSON |

本地运行（需 Java 17+、Maven）：

```bash
cd backend/anq-server
mvn spring-boot:run
```

### 6. 前端 App（鸿蒙 AnQ）

用 DevEco Studio 打开 `frontend/AnQ/`，连接鸿蒙设备后编译运行。App 启动时自动连接 WebSocket，检测到欺凌时实时弹出警情通知并振动。

---

## 告警上报流程

```
香橙派推理 → main_pipeline.py
    │
    ├─ 1. alarm.py          串口指令 AA 55 01 1E → 声光报警模块
    ├─ 2. obs_uploader.py   音频片段(.wav) → OBS bullying2 桶 evidence/
    ├─ 3. iot_uploader.py   MQTT/TLS → 华为云 IoTDA（AlarmService 属性上报）
    └─ 4. HTTP POST         JSON → anq-server /alert/ → WebSocket → 鸿蒙 App
```

IoTDA 上报 payload 示例（`AlarmService`）：

```json
{
  "services": [{
    "service_id": "AlarmService",
    "properties": {
      "alarmType": "bullying",
      "confidence": 0.872,
      "timestamp": 1711234567000,
      "location": "综合教学楼4层东侧走廊",
      "audioLevel": 0.85,
      "radarData": "...",
      "evidenceUrl": "https://bullying2.obs…/evidence/node_001/…wav"
    }
  }]
}
```

---

## 注意事项

- 雷达帧率约 20Hz，滑动窗口 20 帧覆盖约 1 秒，与音频帧时间窗口对齐
- 无 Ascend NPU 时 `main_pipeline.py` 自动降级为随机占位推理（仅用于链路调试）
- MMFi 数据集需自行申请下载，见 `data/samples/datasets/DOWNLOAD.md`
- `serial_audio_capture.py` 音频帧尾应为 `AA 55`（与帧头相同）
- `config/device_key.json` 含真实密钥，已加入 `.gitignore`，请勿提交
- 增量微调后需重新执行 MindIR 导出 → ATC 转换 → 部署 `.om` 到香橙派
