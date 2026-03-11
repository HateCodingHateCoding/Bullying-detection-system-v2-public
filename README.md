# 明瞳 — 校园欺凌智能干预系统

基于**毫米波雷达 + 麦克风**双模态传感器融合的校园欺凌实时检测系统，采用端-边-云-端四层架构，实现从数据采集、AI推理、证据保全到告警推送的完整闭环。

## 系统架构

```
端侧 (K01 MCU)          ── 星闪SLE ──>  边侧 (香橙派 5 Max)
麦克风 + 毫米波雷达                      MindSpore Ascend NPU 推理
                                               │ MQTT (TLS)
                                        华为云 IoTDA + OBS
                                               │ HTTP 回调
                                        Spring Boot 后端
                                               │ HTTP 轮询
                                        HarmonyOS NEXT APP
```

| 层级 | 技术栈 | 说明 |
|------|--------|------|
| 端侧 | K01 MCU、串口 115200bps、HIF协议 | 麦克风采集 16kHz PCM 音频；雷达采集点云数据 |
| 边侧 | 香橙派 5 Max、MindSpore、Ascend NPU | 双模态融合推理，超阈值(≥0.8)触发告警链 |
| 云平台 | 华为云 IoTDA (MQTT 8883)、OBS 对象存储 | 设备管理 + 数据转发；OBS 存储音频证据 |
| 后端 | Spring Boot 3.2 / Java 21 | 接收 IoTDA 回调，存储告警，提供 REST API |
| 前端 | HarmonyOS NEXT (ArkTS / ArkUI) | 实时告警展示、AI 置信度、证据音频回放 |

## AI 模型

**AscendSentinelNet** — 双分支 Conv1d 特征提取 + 融合分类网络

- 雷达分支: `(1, 5, 64)` → Conv1d → 64 维特征
- 音频分支: `(1, 1, 16000)` → Conv1d → 32 维特征
- 融合层: 96 维拼接 → Dense(64) → Dense(2) → Softmax
- 部署格式: `.ckpt` / `.mindir`，目标设备 Ascend NPU

## 项目结构

```
├── AnQ/
│   ├── moke_infer_iot.py        # 主推理程序（IoTDA + OBS 完整版）
│   ├── moke_infer_simple.py     # 简化版（HTTP 直连后端）
│   ├── moke_infer_full.py       # 中间版（OBS + HTTP）
│   ├── iot_uploader.py          # 华为 IoTDA MQTT 客户端
│   ├── obs_uploader.py          # 华为 OBS 上传
│   ├── backend_api.py           # Flask 轻量后端（备用）
│   ├── backend/                 # Spring Boot 正式后端
│   └── AnQ/                     # HarmonyOS NEXT 前端 APP
├── serial_audio_capture.py      # 串口音频采集
├── serial_radar_capture.py      # 串口雷达数据采集
├── sentinel_model.ckpt          # MindSpore 模型权重
├── sentinel.mindir              # MindIR 离线推理模型
├── bullying_detection_complete/  # 早期代码、文档、训练脚本
└── wheels/                      # 离线 pip 依赖包
```

## 数据流

1. **端侧采集** → 串口传输原始 PCM 音频 + 雷达点云
2. **边侧推理** → MindSpore Ascend 推理，输出置信度
3. **超阈值 (≥0.8)** → 音频证据上传 OBS
4. **MQTT 上报** → IoTDA 属性上报（告警类型、置信度、位置、证据URL）
5. **云端转发** → IoTDA HTTP 回调 → Spring Boot 存储告警
6. **前端展示** → HarmonyOS APP 轮询告警，展示详情 + 播放证据音频

## 隐私设计

- 不采集视频，不保存原始音频明文
- 仅用特征推理 + 短时证据片段，最小化隐私侵入
