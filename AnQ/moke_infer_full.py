import numpy as np
import mindspore as ms
import mindspore.nn as nn
import mindspore.ops as ops
import time
import requests
from datetime import datetime
from obs import ObsClient
import os

ms.set_context(mode=ms.GRAPH_MODE, device_target="Ascend", device_id=0)

# ========== 配置 ==========
CONFIG = {
    "api_url": "http://115.120.205.25:8080/alert",
    "location": "综合教学楼4层东侧走廊",
    "alarm_threshold": 0.8,
    "obs_ak": "your_access_key",
    "obs_sk": "your_secret_key",
    "obs_endpoint": "obs.cn-north-4.myhuaweicloud.com",
    "obs_bucket": "bullying-evidence",
    "device_id": "orangepi_001"
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

# 初始化OBS客户端
obs_client = ObsClient(
    access_key_id=CONFIG["obs_ak"],
    secret_access_key=CONFIG["obs_sk"],
    server=CONFIG["obs_endpoint"]
)

def upload_audio_evidence(audio_data):
    """上传音频证据到OBS"""
    timestamp = int(time.time() * 1000)
    object_key = f"evidence/{CONFIG['device_id']}/{timestamp}.wav"
    temp_file = f"/tmp/{timestamp}.wav"

    try:
        with open(temp_file, 'wb') as f:
            f.write(audio_data)

        resp = obs_client.putFile(CONFIG["obs_bucket"], object_key, temp_file)
        os.remove(temp_file)

        if resp.status < 300:
            url = f"https://{CONFIG['obs_bucket']}.{CONFIG['obs_endpoint']}/{object_key}"
            return url
        return None
    except Exception as e:
        print(f"❌ 上传证据失败: {e}")
        return None

def send_alert(location, confidence, evidence_url):
    """发送告警到后端（含证据链）"""
    current_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    payload = {
        "location": location,
        "time": current_time,
        "status": "NEW",
        "confidence": round(confidence, 3),
        "evidenceUrl": evidence_url
    }

    try:
        response = requests.post(
            CONFIG["api_url"],
            json=payload,
            headers={"Content-Type": "application/json"},
            timeout=5
        )
        if response.status_code == 200:
            print(f"✅ 告警已上报: 置信度{confidence:.1%}, 证据: {evidence_url}")
        else:
            print(f"❌ 上报失败: {response.status_code}")
    except Exception as e:
        print(f"❌ 上报异常: {e}")

print("1. 加载模型...")
net = AscendSentinelNet()
ms.load_param_into_net(net, ms.load_checkpoint("sentinel_model.ckpt"), strict_load=False)
net = ms.amp.auto_mixed_precision(net, amp_level="O2")
net.set_train(False)

print("2. 模型预热...")
dummy_radar = ms.Tensor(np.zeros([1, 5, 64], dtype=np.float32))
dummy_audio = ms.Tensor(np.zeros([1, 1, 16000], dtype=np.float32))
_ = net(dummy_radar, dummy_audio)

print("3. 开始实时监控...")
while True:
    radar_data = ms.Tensor(np.load("real_radar_normal.npy").astype(np.float32))
    audio_data = ms.Tensor(np.load("real_audio_normal.npy").astype(np.float32))

    logits = net(radar_data, audio_data)
    probs = ops.Softmax(axis=1)(logits).asnumpy()[0]
    pred_class = np.argmax(probs)
    confidence = probs[pred_class]

    if pred_class == 1 and confidence >= CONFIG["alarm_threshold"]:
        # 提取1秒音频并上传
        audio_raw = audio_data.asnumpy()[0, 0, :]
        audio_bytes = (audio_raw * 32767).astype(np.int16).tobytes()
        evidence_url = upload_audio_evidence(audio_bytes)

        # 上报告警
        send_alert(CONFIG["location"], confidence, evidence_url or "")
        time.sleep(10)

    time.sleep(1)
