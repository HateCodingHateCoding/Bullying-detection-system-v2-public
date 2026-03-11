# 🔒 明瞳 - 校园欺凌智能干预系统

<p align="center">
  <b>基于华为全栈技术的多模态融合AI校园霸凌检测方案</b>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/MindSpore-1.8.0-blue" alt="MindSpore">
  <img src="https://img.shields.io/badge/Platform-ModelArts-orange" alt="ModelArts">
  <img src="https://img.shields.io/badge/Device-OrangePi%205%20Max-green" alt="OrangePi">
  <img src="https://img.shields.io/badge/MCU-HiSilicon%20K01-red" alt="K01">
</p>

---

## 📋 项目简介

**明瞳**是一套完整的校园霸凌智能检测与干预系统，通过多模态传感器（声音+振动）融合分析，实时检测潜在的校园欺凌行为，并在检测到高风险事件时立即触发声光报警进行干预。

### 🎯 核心特点

- **多模态融合**: 结合声音特征和振动特征，提高检测准确率，降低误报
- **端-边-云协同**: 端侧采集、边侧推理、云端管理的分层架构
- **实时响应**: 端到端响应时间 < 3秒，实现"黄金60秒"内干预
- **隐私保护**: 不采集视频和原始音频，仅使用特征数据
- **自进化能力**: 云端持续训练优化模型，OTA更新到边侧

---

## 🏗️ 系统架构

```
┌──────────────────────────────────────────────────────────────┐
│                        云端 (华为云)                          │
│    IoTDA + OBS + DIS + ModelArts + FunctionGraph             │
└───────────────────────────┬──────────────────────────────────┘
                            │ MQTT/HTTPS
                            ▼
┌──────────────────────────────────────────────────────────────┐
│                   边侧 (香橙派 5 Max)                         │
│              多模态AI模型推理 + 报警决策                       │
└───────────────────────────┬──────────────────────────────────┘
                            │ 星闪SLE
                            ▼
┌──────────────────────────────────────────────────────────────┐
│                      端侧 (K01 MCU)                          │
│         全向麦克风 + 陀螺仪IMU + 蜂鸣器 + LED灯带              │
└──────────────────────────────────────────────────────────────┘
```

---

## 📁 项目结构

```
bullying_detection/
├── modelarts/                    # ModelArts云端代码
│   ├── bullying_model.py         # 多模态融合AI模型
│   ├── data_preprocessing.py     # 数据预处理
│   ├── train.py                  # 训练脚本
│   └── deploy_service.py         # 在线服务部署
│
├── edge/                         # 香橙派边侧代码
│   └── edge_inference.py         # 边侧推理程序
│
├── device/                       # K01端侧代码
│   └── k01_main.c                # 端侧采集程序
│
├── iotda/                        # IoTDA云平台对接
│   └── iotda_client.py           # IoTDA客户端
│
├── docs/                         # 文档
│   └── DEPLOYMENT_GUIDE.md       # 完整部署指南
│
├── config.json                   # 配置文件
├── requirements.txt              # Python依赖
├── quick_test.py                 # 快速测试脚本
└── README.md                     # 本文件
```

---

## 🚀 快速开始

### 1. 环境准备

```bash
# 克隆项目
git clone https://github.com/xxx/bullying_detection.git
cd bullying_detection

# 安装依赖
pip install -r requirements.txt

# 配置华为云凭证
export HUAWEICLOUD_AK="your_access_key"
export HUAWEICLOUD_SK="your_secret_key"
export HUAWEICLOUD_PROJECT_ID="your_project_id"
```

### 2. 快速测试

```bash
# 运行测试脚本
python quick_test.py
```

### 3. 模型训练

```bash
# 使用合成数据训练（快速测试）
cd modelarts
python train.py --use_synthetic --epochs 50 --device_target CPU

# 使用真实数据训练（需要准备数据）
python train.py --data_dir /path/to/data --epochs 100 --device_target Ascend
```

### 4. 部署运行

详见 [完整部署指南](docs/DEPLOYMENT_GUIDE.md)

---

## 🧠 AI模型

### 模型架构

```
输入:
  ├── 音频: 梅尔频谱图 (1, 64, 128)
  └── IMU: 振动序列 (200, 6)
      
      ↓
      
┌─────────────────────────────────────────────┐
│           AudioFeatureExtractor             │
│     CNN (4层) → 256维特征向量               │
└─────────────────────────────────────────────┘
      ↓
      └──────────┐
                 ↓
┌─────────────────────────────────────────────┐
│          CrossModalAttention                │
│    跨模态注意力融合 → 128维融合特征          │
└─────────────────────────────────────────────┘
                 ↑
      ┌──────────┘
      ↓
┌─────────────────────────────────────────────┐
│         VibrationFeatureExtractor           │
│  1D CNN + BiLSTM → 128维特征向量            │
└─────────────────────────────────────────────┘

      ↓
      
┌─────────────────────────────────────────────┐
│           TemporalContext                   │
│        时序上下文分析模块                    │
└─────────────────────────────────────────────┘

      ↓
      
┌─────────────────────────────────────────────┐
│              Classifier                     │
│     全连接层 → Sigmoid → 霸凌概率           │
└─────────────────────────────────────────────┘

输出:
  ├── probability: 霸凌概率 [0, 1]
  └── attention_weight: 注意力权重 [0, 1]
```

### 训练配置

| 参数 | 值 |
|------|-----|
| 优化器 | Adam |
| 学习率 | 0.001 (Cosine退火) |
| 批次大小 | 32 |
| 训练轮数 | 100 |
| 损失函数 | Focal Loss (α=0.25, γ=2.0) |
| 数据增强 | 时间遮罩 + 频率遮罩 + 噪声注入 |

---

## ⚙️ 配置说明

### 报警阈值配置

```python
# 在 edge/edge_inference.py 中配置

ALARM_THRESHOLD = 0.7    # 概率阈值，超过则考虑报警
CONFIRM_COUNT = 2        # 连续超过阈值的次数
COOLDOWN_TIME = 30       # 报警后的冷却时间（秒）
```

### 报警级别

| 级别 | 概率范围 | 表现 |
|------|----------|------|
| NONE | 0 - 0.5 | 正常，无报警 |
| LOW | 0.5 - 0.7 | 低风险，LED黄灯 |
| MEDIUM | 0.7 - 0.85 | 中风险，间歇报警 |
| HIGH | 0.85 - 1.0 | 高风险，持续声光报警 |

---

## 📊 性能指标

| 指标 | 目标值 | 实测值 |
|------|--------|--------|
| 检测准确率 | > 95% | TBD |
| 误报率 | < 5% | TBD |
| 端到端延迟 | < 3s | TBD |
| 推理延迟(边侧) | < 100ms | TBD |
| 星闪通信延迟 | < 1ms | TBD |

---

## 🔧 硬件清单

### 端侧

- **MCU**: HiSilicon K01开发板
- **IMU**: MPU6050 / BMI160
- **麦克风**: 全向MEMS麦克风模块
- **蜂鸣器**: 有源蜂鸣器
- **LED**: WS2812灯带
- **通信**: 星闪SLE模块

### 边侧

- **开发板**: Orange Pi 5 Max
- **NPU**: 昇腾310
- **内存**: 16GB
- **存储**: 64GB eMMC

---

## 👥 团队信息

**团队名称**: 三英战ICT

**学校**: 华中师范大学

**成员**:
- 秦熙明 (队长)
- 张德华
- 胡一凡

**指导老师**: 彭熙

---

## 📄 许可证

本项目仅用于第十届华为ICT大赛中国创新赛参赛作品展示，禁止商业使用。

---

## 🙏 致谢

- 华为云 ModelArts 平台
- 华为 MindSpore 框架
- 华为 IoTDA 物联网平台
- Orange Pi 社区
