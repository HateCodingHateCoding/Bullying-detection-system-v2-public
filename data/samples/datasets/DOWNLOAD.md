# 数据集说明

## MMFI数据集

MMFi (Multi-Modal Human Activity Recognition) 数据集，包含27种人体动作的毫米波雷达点云数据。

官方主页：https://mmfi-dataset.github.io/

## 下载方式

```bash
# 从官方渠道申请下载，或联系数据集作者
# 下载后解压到本目录下，结构如下：
# data/samples/MMFi/
#   S01/E01/A01/mmwave/*.bin
#   S01/E01/A20/mmwave/*.bin  ← 霸凌动作样本
#   ...
```

## 动作标签说明

| 动作ID | 描述 | 标签 |
|---|---|---|
| A01-A19 | 日常动作（站立、行走、跌倒等） | 0（正常）|
| A20 | 投掷（左）| 1（霸凌）|
| A21 | 投掷（右）| 1（霸凌）|
| A22 | 踢腿（左）| 1（霸凌）|
| A23 | 踢腿（右）| 1（霸凌）|
| A24-A27 | 其他日常动作 | 0（正常）|

## 已处理数据

`data/` 目录下的 npy 文件由本数据集处理得到：
- `radar_X.npy`：雷达点云特征，shape `[N, 64, 5]`
- `radar_y.npy`：标签，shape `[N]`
- `audio_X.npy`：音频特征（来自 MIVIA_DB4），shape `[M, 1, 16000]`
- `real_radar_normal.npy`：真实硬件采集雷达数据，shape `[1, 5, 64]`
- `real_audio_normal.npy`：真实硬件采集音频数据，shape `[1, 1, 16000]`

## MIVIA_DB4 音频数据集

音频训练数据来自 MIVIA Audio Surveillance DB4，包含打斗、尖叫等声音事件。

官方主页：https://mivia.unisa.it/datasets/audio-analysis/mivia-audio-events/
