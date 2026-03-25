# Ascend Sentinel

基于华为昇腾 AI Pro（香橙派）端侧部署的校园霸凌检测系统，融合 60GHz 毫米波雷达与音频信号，进行实时推理、告警联动和云端上报。

## 核心能力

- 多模态输入：雷达点云 + 麦克风音频
- 端侧推理：MindSpore 模型本地推理
- 告警联动：声光报警、IoTDA 消息上报、OBS 证据上传
- 应用闭环：后端服务 + 鸿蒙前端展示

## 目录结构（公开版）

```text
Bullying/
 config/
    device_key.json.template
 data/
    real_radar_normal.npy
    real_audio_normal.npy
    samples/datasets/
        DOWNLOAD.md
 docs/
    model_description.md
 logs/
    train_log.txt
    infer_log.txt
 models/
    sentinel_model.ckpt
 scripts/
    main_pipeline.py
    radar_receiver.py
    audio_receiver.py
    alarm.py
    iot_uploader.py
    obs_uploader.py
    serial_radar_capture.py
    serial_audio_capture.py
    bullying_analysis_v3_advanced.ipynb
 backend/
    anq-server/
    spring-boot/
 frontend/
    AnQ/
 SDK_Code/
    HH-D02星闪派SDK/
    RS6240雷达固件SDK/
 .gitignore
```

## 运行说明（概览）

1. 按 [config/device_key.json.template](config/device_key.json.template) 生成本地凭证文件。
2. 准备串口设备并确认雷达、音频采集链路可用。
3. 运行 [scripts/main_pipeline.py](scripts/main_pipeline.py) 启动主推理流程。
4. 如需离线采集，使用 [scripts/serial_radar_capture.py](scripts/serial_radar_capture.py) 与 [scripts/serial_audio_capture.py](scripts/serial_audio_capture.py)。

## 安全与合规

- 真实凭证文件（如 config/device_key.json）不纳入版本管理。
- 仓库仅提供必要模板、样例数据与文档，便于复现与演示。
