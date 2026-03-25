# Ascend Sentinel 模型说明（与当前代码一致）

本文档基于当前仓库代码编写，目标是说明项目里正在运行的边端推理模型与云端训练导出模型的关系，避免配置和结构理解偏差。

## 1. 当前项目中的两套模型形态

当前代码中同时存在两套模型形态：

1. 在线推理模型（边端主链路）
- 代码位置：[scripts/main_pipeline.py](scripts/main_pipeline.py)
- 类名：`AscendSentinelNet`
- 用途：实时接收串口雷达 + 音频，执行二分类并触发报警联动
- 特点：轻量 1D-CNN 双分支，不含 MindCMA/GRU

2. 云端训练模型（Notebook）
- 代码位置：[scripts/bullying_analysis_v3_advanced.ipynb](scripts/bullying_analysis_v3_advanced.ipynb)
- 用途：在 ModelArts 上训练、导出 ckpt/MindIR，并做增量微调
- 特点：v3 多模态时序结构（RadarPointNet + MindCMA + GRU）

结论：当前在线服务以 `main_pipeline.py` 的轻量结构为准；`ipynb` 是训练与导出流程。

---

## 2. 在线推理模型（main_pipeline）

### 2.1 输入输出规格

输入来自接收线程：

- 雷达输入：`(1, 5, 64)` `float32`
  - 来源：[scripts/radar_receiver.py](scripts/radar_receiver.py)
  - 逻辑：窗口内所有点云合并后随机采样 64 点，再转置为 `[C=5, N=64]`

- 音频输入：`(1, 1, 16000)` `float32`
  - 来源：[scripts/audio_receiver.py](scripts/audio_receiver.py)
  - 逻辑：累积 1 秒（16kHz）PCM，归一化到 `[-1,1]`

输出：
- `logits`: `(1,2)`
- `probs = softmax(logits)`，取最大类作为 `pred_class`
- `confidence = probs[pred_class]`

报警触发条件：
- `pred_class == 1` 且 `confidence >= threshold`（默认 0.8）
- 冷却时间 `alarm_cooldown`（默认 10 秒）内不重复触发

### 2.2 网络结构（当前在线）

在线模型为轻量双分支结构：

1. 雷达分支（1D CNN）
- `Conv1d(5,32,k=3,same)`
- `BatchNorm1d(32)` + `ReLU`
- `MaxPool1d(2)`
- `Conv1d(32,64,k=3,same)` + `ReLU`
- `GlobalAvgPool` + `Flatten`
- 输出维度：64

2. 音频分支（1D CNN）
- `Conv1d(1,16,k=64,stride=16,valid)`
- `BatchNorm1d(16)` + `ReLU`
- `MaxPool1d(4)`
- `Conv1d(16,32,k=3,same)` + `ReLU`
- `GlobalAvgPool` + `Flatten`
- 输出维度：32

3. 分类头
- 拼接特征：`64 + 32 = 96`
- `Dense(96,64)` + `ReLU`
- `Dense(64,2)`

### 2.3 推理与部署要点

- 入口：`python scripts/main_pipeline.py --model <ckpt路径>`
- 默认模型参数仍是 `../models/sentinel_model.ckpt`
- 但当前仓库新增模型文件为：
  - [models/sentinel_best.ckpt](models/sentinel_best.ckpt)
  - [models/sentinel_final.ckpt](models/sentinel_final.ckpt)
  - [models/bullying_model.ckpt](models/bullying_model.ckpt)
  - [models/bullying_model.mindir](models/bullying_model.mindir)

建议运行时显式传参，例如：

```bash
python scripts/main_pipeline.py \
  --model ../models/sentinel_best.ckpt \
  --config ../config/device_key.json
```

---

## 3. 数据通路与特征形成（当前在线）

### 3.1 雷达通路

- 协议：RS6x/7x HIF，解析 `motion_point_cloud\0`
- 单帧输出：`(N,5)`，字段为 `x,y,z,velocity,snr`
- 主循环中维护 20 帧滑窗（约 1 秒）
- 合并后随机采样到 64 点，得到 `(1,5,64)`

### 3.2 音频通路

- 协议帧：`AA55 + seq + payload_len + PCM + AA55`
- 数据类型：`int16 LE` PCM
- 每 16000 采样点输出一帧 `(1,1,16000)`

### 3.3 告警闭环

检测触发后执行：

1. 串口声光报警（`/dev/ttyAMA0`）
2. OBS 上传音频证据（`obs_uploader.py`）
3. IoTDA MQTT/TLS 上报（`iot_uploader.py`）
4. HTTP POST 到后端 `/alert/`，再经 WebSocket 推送 App

---

## 4. 云端训练与导出模型（Notebook v3）

Notebook 中定义的是更复杂的 v3 结构（用于训练/导出）：

- AudioCNN + RadarPointNet(T-Net)
- MindCMA 跨模态注意力
- GRU 时序建模（多帧）
- 多头/联合损失训练

关键产物保存路径（Notebook 内）：

- 最优权重：`/home/ma-user/work/sentinel_best.ckpt`
- 最终权重：`/home/ma-user/work/sentinel_final.ckpt`
- 导出备份：`./output/bullying_model.ckpt`
- MindIR：`./output/bullying_model.mindir`

当前仓库已收录对应模型产物到 [models](models) 目录。

---

## 5. 一致性说明（很重要）

由于在线推理网络与 v3 训练网络并非完全同构，使用不同 ckpt 时需注意：

1. `main_pipeline.py` 按轻量 `AscendSentinelNet` 建图加载参数，采用 `strict_load=False`。
2. 若 ckpt 与当前在线结构不完全匹配，MindSpore 会尽量加载同名同形状参数，其余参数被忽略。
3. 若要严格复现 v3 效果，应将 v3 推理代码独立落地，并与导出的 v3 参数一一对应。

---

## 6. 推荐实践

1. 线上稳定优先：固定使用一份经过实机验证的 ckpt（建议 `sentinel_best.ckpt`）。
2. 每次更新模型后，在 [logs/infer_log.txt](logs/infer_log.txt) 记录版本与阈值。
3. 在 README 与部署脚本中统一 `--model` 默认值，避免路径漂移。

---

## 7. 版本信息

- 文档更新时间：2026-03-25
- 对齐代码基线：`main_pipeline.py` 当前主干实现 + `bullying_analysis_v3_advanced.ipynb` 当前训练实现
