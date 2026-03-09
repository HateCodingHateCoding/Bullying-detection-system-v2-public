import numpy as np
import mindspore as ms
import mindspore.nn as nn
import mindspore.ops as ops
import time
import sys
import wave
sys.path.append('/path/to/iot_uploader')
from iot_uploader import HuaweiIoTUploader
from obs_uploader import OBSUploader

ms.set_context(mode=ms.GRAPH_MODE, device_target="Ascend", device_id=0)

# ========== 配置IoT参数 ==========
IOT_CONFIG = {
    "device_id": "orangepi_001",
    "secret": "your_device_secret_here",
    "platform_url": "xxx.iot-mqtts.cn-north-4.myhuaweicloud.com",
    "location": "综合教学楼4层东侧走廊",
    "alarm_threshold": 0.8
}

# ========== 配置OBS参数 ==========
OBS_CONFIG = {
    "ak": "your_access_key",
    "sk": "your_secret_key",
    "endpoint": "obs.cn-north-4.myhuaweicloud.com",
    "bucket": "bullying-evidence"
}

class GlobalAvgPool(nn.Cell):
    def construct(self, x):
        return x.mean(axis=-1, keep_dims=True)

class AscendSentinelNet(nn.Cell):
    def __init__(self):
        super(AscendSentinelNet, self).__init__()
        self.radar_branch = nn.SequentialCell([
            nn.Conv1d(5, 32, kernel_size=3, pad_mode='same'),
            nn.BatchNorm1d(32), nn.ReLU(), nn.MaxPool1d(2),
            nn.Conv1d(32, 64, kernel_size=3, pad_mode='same'),
            nn.ReLU(), GlobalAvgPool(), nn.Flatten()
        ])
        self.audio_branch = nn.SequentialCell([
            nn.Conv1d(1, 16, kernel_size=64, stride=16, pad_mode='valid'),
            nn.BatchNorm1d(16), nn.ReLU(), nn.MaxPool1d(4),
            nn.Conv1d(16, 32, kernel_size=3, pad_mode='same'),
            nn.ReLU(), GlobalAvgPool(), nn.Flatten()
        ])
        self.classifier = nn.SequentialCell([
            nn.Dense(64 + 32, 64), nn.ReLU(), nn.Dense(64, 2)
        ])

    def construct(self, r, a):
        feat_r = self.radar_branch(r)
        feat_a = self.audio_branch(a)
        combined = ops.concat((feat_r, feat_a), axis=1)
        return self.classifier(combined)

print("1. 初始化IoT和OBS连接...")
iot = HuaweiIoTUploader(
    IOT_CONFIG["device_id"],
    IOT_CONFIG["secret"],
    IOT_CONFIG["platform_url"]
)
iot.connect()

obs = OBSUploader(
    OBS_CONFIG["ak"],
    OBS_CONFIG["sk"],
    OBS_CONFIG["endpoint"],
    OBS_CONFIG["bucket"]
)

print("2. 加载模型...")
net = AscendSentinelNet()
ms.load_param_into_net(net, ms.load_checkpoint("sentinel_model.ckpt"), strict_load=False)
net = ms.amp.auto_mixed_precision(net, amp_level="O2")
net.set_train(False)

print("3. 模型预热...")
dummy_radar = ms.Tensor(np.zeros([1, 5, 64], dtype=np.float32))
dummy_audio = ms.Tensor(np.zeros([1, 1, 16000], dtype=np.float32))
_ = net(dummy_radar, dummy_audio)

print("4. 开始实时监控...")
while True:
    # 读取实时数据 (替换为实际采集代码)
    radar_data = ms.Tensor(np.load("real_radar_normal.npy").astype(np.float32))
    audio_data = ms.Tensor(np.load("real_audio_normal.npy").astype(np.float32))

    # 推理
    logits = net(radar_data, audio_data)
    probs = ops.Softmax(axis=1)(logits).asnumpy()[0]
    pred_class = np.argmax(probs)
    confidence = probs[pred_class]

    # 判断是否告警
    if pred_class == 1 and confidence >= IOT_CONFIG["alarm_threshold"]:
        audio_level = 90

        # 上传音频证据到OBS
        audio_raw = audio_data.asnumpy()[0, 0, :]  # 提取1秒音频
        audio_bytes = (audio_raw * 32767).astype(np.int16).tobytes()
        evidence_url = obs.upload_audio_evidence(audio_bytes, IOT_CONFIG["device_id"])

        # 上报告警到IoT平台
        iot.upload_alarm(
            alarm_type="bullying",
            confidence=confidence,
            location=IOT_CONFIG["location"],
            audio_level=audio_level,
            radar_summary="异常动作检测",
            evidence_url=evidence_url or ""
        )

    time.sleep(1)
