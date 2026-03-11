# 校园霸凌检测系统 - 完整部署指南

## 📋 目录

1. [系统架构概述](#1-系统架构概述)
2. [环境准备](#2-环境准备)
3. [华为云配置](#3-华为云配置)
4. [ModelArts模型训练](#4-modelarts模型训练)
5. [模型导出与部署](#5-模型导出与部署)
6. [香橙派边侧部署](#6-香橙派边侧部署)
7. [K01端侧开发](#7-k01端侧开发)
8. [IoTDA平台配置](#8-iotda平台配置)
9. [系统联调测试](#9-系统联调测试)
10. [常见问题](#10-常见问题)

---

## 1. 系统架构概述

```
┌─────────────────────────────────────────────────────────────────┐
│                         云端 (华为云)                            │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────────────┐ │
│  │ IoTDA    │  │   OBS    │  │   DIS    │  │   ModelArts      │ │
│  │ 设备管理  │  │ 数据存储  │  │ 数据流   │  │ 模型训练/在线服务 │ │
│  └────┬─────┘  └────┬─────┘  └────┬─────┘  └────────┬─────────┘ │
│       │             │             │                  │           │
└───────┼─────────────┼─────────────┼──────────────────┼───────────┘
        │             │             │                  │
        │ MQTT/HTTPS  │             │                  │
        ▼             ▼             ▼                  ▼
┌───────────────────────────────────────────────────────────────────┐
│                    边侧 (香橙派 5 Max)                             │
│  ┌──────────────────────────────────────────────────────────────┐ │
│  │  • 多模态融合AI模型推理 (昇腾NPU)                              │ │
│  │  • 实时霸凌概率计算                                            │ │
│  │  • 报警决策 (概率 > 阈值)                                      │ │
│  │  • 数据上报云端                                                │ │
│  └──────────────────────────────────────────────────────────────┘ │
└───────────────────────────────┬───────────────────────────────────┘
                                │ 星闪SLE/UART
                                ▼
┌───────────────────────────────────────────────────────────────────┐
│                       端侧 (K01 MCU)                              │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  ┌──────────┐ │
│  │ 全向麦克风   │  │ 陀螺仪IMU   │  │ LED灯带     │  │ 蜂鸣器    │ │
│  │ (声音采集)   │  │ (振动采集)  │  │ (光报警)    │  │ (声报警)  │ │
│  └─────────────┘  └─────────────┘  └─────────────┘  └──────────┘ │
└───────────────────────────────────────────────────────────────────┘
```

### 1.1 数据流向

1. **端侧 → 边侧**: K01采集音频+振动数据，通过星闪SLE发送到香橙派
2. **边侧处理**: 香橙派运行AI模型推理，计算霸凌概率
3. **报警触发**: 概率超过阈值时，下发命令给K01触发声光报警
4. **数据上报**: 边侧将检测结果上报到IoTDA云平台
5. **云端存储**: IoTDA将数据转发到OBS存储，用于后续模型训练

---

## 2. 环境准备

### 2.1 华为云账号准备

1. 注册华为云账号: https://www.huaweicloud.com/
2. 完成实名认证
3. 开通以下服务:
   - ModelArts (AI开发平台)
   - IoTDA (物联网平台)
   - OBS (对象存储)
   - DIS (数据接入服务) [可选]
   - FunctionGraph (函数服务) [可选]

4. 创建访问密钥 (AK/SK):
   - 进入"我的凭证" → "访问密钥"
   - 创建并下载 AK/SK

### 2.2 本地开发环境

```bash
# 1. 安装Python 3.8+
python --version  # 确保 >= 3.8

# 2. 创建虚拟环境
python -m venv bullying_env
source bullying_env/bin/activate  # Linux/Mac
# bullying_env\Scripts\activate  # Windows

# 3. 安装依赖
pip install -r requirements.txt

# 4. 配置环境变量
export HUAWEICLOUD_AK="your_access_key"
export HUAWEICLOUD_SK="your_secret_key"
export HUAWEICLOUD_PROJECT_ID="your_project_id"
```

### 2.3 香橙派环境

```bash
# 1. 烧录Ubuntu 22.04镜像到香橙派5 Max

# 2. 安装CANN (昇腾计算架构)
# 从华为官网下载对应版本的CANN
wget https://xxx/Ascend-cann-xxx.run
chmod +x Ascend-cann-xxx.run
./Ascend-cann-xxx.run --install

# 3. 安装MindSpore Lite
pip install mindspore-lite

# 4. 安装项目依赖
pip install numpy scipy paho-mqtt pyserial
```

---

## 3. 华为云配置

### 3.1 创建OBS桶

```bash
# 使用OBS Browser+或控制台创建桶
桶名称: bullying-detection-data
区域: 华北-北京四 (cn-north-4)
存储类别: 标准存储

# 创建目录结构
bullying-detection-data/
├── training_data/      # 训练数据
│   ├── audio/
│   └── imu/
├── models/             # 模型文件
│   ├── checkpoints/
│   └── exported/
├── logs/               # 训练日志
└── configs/            # 配置文件
```

### 3.2 创建ModelArts开发环境

1. 进入ModelArts控制台
2. 创建Notebook实例:
   - 名称: bullying-detection-dev
   - 规格: Ascend (推荐) 或 GPU
   - 镜像: MindSpore 1.8.0
   - 存储: 挂载OBS桶

### 3.3 创建IoTDA实例

1. 进入IoTDA控制台
2. 创建实例:
   - 版本: 企业版 (推荐) 或 基础版
   - 区域: 与OBS相同

---

## 4. ModelArts模型训练

### 4.1 上传代码到OBS

```bash
# 将代码打包上传
cd bullying_detection/modelarts
zip -r training_code.zip *.py

# 上传到OBS
obsutil cp training_code.zip obs://bullying-detection-data/code/
```

### 4.2 准备训练数据

**方式一: 使用合成数据 (快速测试)**

```python
# 在ModelArts Notebook中运行
from data_preprocessing import generate_synthetic_data

# 生成3000个样本 (30%霸凌, 70%正常)
audio_data, imu_data, labels = generate_synthetic_data(
    num_samples=3000,
    bullying_ratio=0.3,
    save_dir='/home/ma-user/work/data'
)
```

**方式二: 使用真实数据**

```python
# 数据格式要求:
# audio_data.npy: shape (N, 1, 64, 128), float32
# imu_data.npy: shape (N, 200, 6), float32  
# labels.npy: shape (N,), 0=正常, 1=霸凌

# 上传到OBS
obsutil cp audio_data.npy obs://bullying-detection-data/training_data/
obsutil cp imu_data.npy obs://bullying-detection-data/training_data/
obsutil cp labels.npy obs://bullying-detection-data/training_data/
```

### 4.3 创建训练作业

**方式一: 在Notebook中训练**

```python
# 在Notebook中运行
%cd /home/ma-user/work

# 克隆或上传代码
!unzip obs://bullying-detection-data/code/training_code.zip

# 开始训练
!python train.py \
    --data_dir /home/ma-user/work/data \
    --output_dir /home/ma-user/work/output \
    --epochs 100 \
    --batch_size 32 \
    --lr 0.001 \
    --use_focal_loss \
    --device_target Ascend
```

**方式二: 创建训练作业 (推荐)**

1. 进入ModelArts → 训练管理 → 训练作业
2. 创建训练作业:

```yaml
作业名称: bullying_detection_train
算法来源: 自定义
代码目录: obs://bullying-detection-data/code/
启动文件: train.py
输入数据: obs://bullying-detection-data/training_data/
输出路径: obs://bullying-detection-data/models/

运行参数:
  - epochs: 100
  - batch_size: 32
  - lr: 0.001
  - use_focal_loss: true
  - use_synthetic: false

资源规格: Ascend: 1*Ascend 910
```

### 4.4 监控训练

```bash
# 查看训练日志
# 在ModelArts控制台查看

# 或使用SDK
from huaweicloudsdkmodelarts.v2 import *

client = ModelArtsClient(...)
job = client.show_training_job(training_job_id="xxx")
print(job.status)
```

### 4.5 训练完成后

训练完成后，以下文件会保存到OBS:
- `best_model.ckpt`: 最佳模型权重
- `final_model.ckpt`: 最终模型权重
- `bullying_model.mindir`: 导出的MindIR模型
- `model_config.json`: 模型配置
- `training_metrics.json`: 训练指标

---

## 5. 模型导出与部署

### 5.1 导出模型格式

```python
# 导出为不同格式

# 1. MindIR格式 (用于MindSpore Lite推理)
python train.py --export --format MINDIR \
    --ckpt_path output/best_model.ckpt \
    --output_dir output/exported/

# 2. AIR格式 (用于昇腾ACL推理)
python train.py --export --format AIR \
    --ckpt_path output/best_model.ckpt \
    --output_dir output/exported/

# 3. ONNX格式 (通用格式)
python train.py --export --format ONNX \
    --ckpt_path output/best_model.ckpt \
    --output_dir output/exported/
```

### 5.2 转换为OM模型 (香橙派部署)

```bash
# 在昇腾环境中使用ATC工具转换
atc --model=bullying_model.air \
    --framework=1 \
    --output=bullying_model \
    --soc_version=Ascend310 \
    --input_shape="audio:1,1,64,128;imu:1,200,6"
```

### 5.3 部署ModelArts在线服务 (可选)

```python
# 运行部署脚本
python deploy_service.py --deploy

# 或手动部署:
# 1. 进入ModelArts → AI应用管理 → 模型
# 2. 导入模型 (选择OBS路径)
# 3. 进入 → 部署 → 在线服务
# 4. 选择模型，配置实例规格
# 5. 部署并获取API地址
```

---

## 6. 香橙派边侧部署

### 6.1 准备工作

```bash
# SSH连接到香橙派
ssh orangepi@192.168.x.x

# 创建项目目录
mkdir -p /home/orangepi/bullying_detection
cd /home/orangepi/bullying_detection

# 上传代码
scp -r edge/* orangepi@192.168.x.x:/home/orangepi/bullying_detection/

# 创建模型目录
mkdir -p /home/orangepi/models
```

### 6.2 上传模型

```bash
# 从OBS下载模型到香橙派
# 方式1: 使用obsutil
obsutil cp obs://bullying-detection-data/models/exported/bullying_model.om \
    /home/orangepi/models/

# 方式2: 使用scp
scp bullying_model.om orangepi@192.168.x.x:/home/orangepi/models/
```

### 6.3 配置系统参数

```python
# 编辑 edge_inference.py 中的 Config 类

class Config:
    # 模型路径
    MODEL_PATH = '/home/orangepi/models/bullying_model.om'
    
    # 报警阈值 (根据实际情况调整)
    ALARM_THRESHOLD = 0.7   # 概率超过70%触发报警
    CONFIRM_COUNT = 2       # 连续2次超过阈值才报警
    COOLDOWN_TIME = 30      # 报警后30秒冷却
    
    # 串口配置 (连接K01)
    UART_PORT = '/dev/ttyS3'
    UART_BAUDRATE = 115200
    
    # IoTDA配置
    IOTDA_DEVICE_ID = 'your_device_id'
    IOTDA_SECRET = 'your_device_secret'
    
    # 设备位置
    LOCATION = '教学楼A-3楼-男厕所'
```

### 6.4 运行测试

```bash
# 测试模式 (不连接硬件)
python edge_inference.py --test

# 正常运行
python edge_inference.py

# 后台运行
nohup python edge_inference.py > /var/log/bullying_detector.log 2>&1 &
```

### 6.5 设置开机自启

```bash
# 创建systemd服务
sudo nano /etc/systemd/system/bullying-detector.service
```

```ini
[Unit]
Description=Bullying Detection Service
After=network.target

[Service]
Type=simple
User=orangepi
WorkingDirectory=/home/orangepi/bullying_detection
ExecStart=/usr/bin/python3 edge_inference.py
Restart=always
RestartSec=10

[Install]
WantedBy=multi-user.target
```

```bash
# 启用服务
sudo systemctl daemon-reload
sudo systemctl enable bullying-detector
sudo systemctl start bullying-detector

# 查看状态
sudo systemctl status bullying-detector
```

---

## 7. K01端侧开发

### 7.1 开发环境搭建

```bash
# 1. 安装HiSilicon SDK
# 从华为开发者联盟下载K01 SDK

# 2. 安装编译工具链
# arm-none-eabi-gcc

# 3. 配置IDE (推荐使用HUAWEI DevEco Device Tool)
```

### 7.2 硬件连接

```
K01开发板引脚连接:

┌─────────────────────────────────────────────────┐
│  K01 MCU                                        │
├─────────────────────────────────────────────────┤
│  GPIO_7 (PWM0)  ──────── 蜂鸣器 (+)             │
│  GND            ──────── 蜂鸣器 (-)             │
│                                                 │
│  GPIO_8         ──────── LED灯带 (DIN)          │
│  GPIO_9         ──────── LED灯带 (EN)           │
│  5V             ──────── LED灯带 (VCC)          │
│  GND            ──────── LED灯带 (GND)          │
│                                                 │
│  I2C0_SDA       ──────── MPU6050 (SDA)          │
│  I2C0_SCL       ──────── MPU6050 (SCL)          │
│  3.3V           ──────── MPU6050 (VCC)          │
│  GND            ──────── MPU6050 (GND)          │
│  GPIO_10        ──────── MPU6050 (INT)          │
│                                                 │
│  ADC0           ──────── 麦克风模块 (OUT)       │
│  3.3V           ──────── 麦克风模块 (VCC)       │
│  GND            ──────── 麦克风模块 (GND)       │
│                                                 │
│  UART_TX        ──────── 星闪模块 (RX)          │
│  UART_RX        ──────── 星闪模块 (TX)          │
└─────────────────────────────────────────────────┘
```

### 7.3 编译和烧录

```bash
# 编译
cd device
make clean
make all

# 烧录 (使用HiBurn工具)
hiburn -p COM3 -b 115200 -f output/k01_main.bin
```

### 7.4 调试

```bash
# 串口调试
minicom -D /dev/ttyUSB0 -b 115200

# 查看输出日志
# [timestamp] Status: SLE=1, IMU=1, Audio=1, Sent=xxx, Failed=xxx
```

---

## 8. IoTDA平台配置

### 8.1 创建产品

```python
# 运行IoTDA配置脚本
python iotda/iotda_client.py --setup
```

或手动操作:

1. 进入IoTDA控制台 → 产品
2. 创建产品:
   - 产品名称: 校园霸凌检测终端
   - 协议类型: MQTT
   - 数据格式: JSON

3. 定义产品模型:

```json
{
  "services": [
    {
      "service_id": "bullying_detection",
      "service_type": "detection",
      "properties": [
        {"property_name": "probability", "data_type": "decimal"},
        {"property_name": "alarm_level", "data_type": "int"},
        {"property_name": "attention_weight", "data_type": "decimal"},
        {"property_name": "timestamp", "data_type": "int"},
        {"property_name": "location", "data_type": "string"},
        {"property_name": "device_id", "data_type": "string"}
      ]
    },
    {
      "service_id": "alarm",
      "service_type": "alarm",
      "events": [
        {
          "event_type": "bullying_detected",
          "paras": [
            {"para_name": "probability", "data_type": "decimal"},
            {"para_name": "level", "data_type": "string"},
            {"para_name": "location", "data_type": "string"}
          ]
        }
      ]
    },
    {
      "service_id": "command",
      "service_type": "command",
      "commands": [
        {
          "command_name": "trigger_alarm",
          "paras": [
            {"para_name": "level", "data_type": "int"},
            {"para_name": "duration", "data_type": "int"}
          ]
        }
      ]
    }
  ]
}
```

### 8.2 注册设备

1. 进入 设备 → 所有设备 → 注册设备
2. 填写:
   - 设备标识码: node_001
   - 设备名称: 教学楼A-3楼-男厕所
   - 所属产品: 校园霸凌检测终端
3. 保存设备密钥

### 8.3 配置规则引擎

**规则1: 数据转发到OBS**

1. 进入 规则 → 数据转发
2. 创建规则:
   - 规则名称: bullying_data_to_obs
   - 数据来源: 设备属性上报
   - 转发目标: OBS
   - 文件路径: `bullying_data/${deviceId}/${timestamp}.json`

**规则2: 高风险告警推送**

1. 创建规则:
   - 规则名称: high_risk_alarm
   - 触发条件: `probability > 0.8`
   - 转发目标: HTTP推送 / SMN消息通知

### 8.4 测试连接

```python
# 模拟设备上报
python iotda/iotda_client.py --device
```

---

## 9. 系统联调测试

### 9.1 单元测试

```bash
# 测试模型推理
python -m pytest tests/test_model.py

# 测试数据预处理
python -m pytest tests/test_preprocessing.py

# 测试边侧推理
python edge_inference.py --test
```

### 9.2 集成测试

```bash
# 1. 启动边侧程序
ssh orangepi@xxx
python edge_inference.py

# 2. 启动端侧程序
# (K01自动运行)

# 3. 模拟霸凌场景
# - 对着麦克风大声喊叫
# - 剧烈晃动IMU传感器

# 4. 观察:
# - 边侧日志显示概率上升
# - 蜂鸣器和LED报警
# - IoTDA收到告警事件
```

### 9.3 性能测试

```bash
# 测试推理延迟
python tests/benchmark.py --iterations 100

# 期望结果:
# - 端到边延迟: < 50ms (星闪SLE)
# - 边侧推理延迟: < 100ms
# - 总响应时间: < 3s
```

### 9.4 压力测试

```bash
# 长时间运行测试
python tests/stress_test.py --duration 3600  # 1小时

# 监控:
# - 内存使用
# - CPU使用
# - 模型推理准确率
```

---

## 10. 常见问题

### Q1: ModelArts训练作业失败

```
检查:
1. OBS路径是否正确
2. 数据格式是否正确 (npy文件)
3. 资源规格是否足够
4. 查看详细日志定位错误
```

### Q2: 香橙派无法加载模型

```bash
# 检查CANN是否正确安装
npu-smi info

# 检查模型格式
file bullying_model.om

# 检查路径权限
ls -la /home/orangepi/models/
```

### Q3: K01与香橙派通信失败

```bash
# 检查串口
ls /dev/ttyS*

# 检查波特率设置
stty -F /dev/ttyS3

# 使用minicom调试
minicom -D /dev/ttyS3 -b 115200
```

### Q4: IoTDA连接失败

```
检查:
1. 设备ID和密钥是否正确
2. 时间戳是否同步 (NTP)
3. 网络是否可达
4. 证书是否正确配置
```

### Q5: 误报率过高

```
调整策略:
1. 提高 ALARM_THRESHOLD (如 0.7 → 0.8)
2. 增加 CONFIRM_COUNT (如 2 → 3)
3. 增加训练数据，特别是"假阳性"样本
4. 使用数据增强扩充训练集
```

### Q6: 漏报率过高

```
调整策略:
1. 降低 ALARM_THRESHOLD (如 0.7 → 0.6)
2. 增加霸凌样本的训练数据
3. 调整Focal Loss的alpha参数
4. 检查传感器灵敏度设置
```

---

## 📞 联系方式

- 团队: 三英战ICT
- 学校: 华中师范大学
- 邮箱: siriusqin@mails.ccnu.edu.cn

---

## 📄 版本历史

| 版本 | 日期 | 更新内容 |
|------|------|----------|
| v1.0 | 2024-xx | 初始版本 |

