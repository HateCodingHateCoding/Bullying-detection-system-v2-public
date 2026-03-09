import numpy as np
import mindspore as ms
import mindspore.nn as nn
import mindspore.ops as ops
import time
import requests
from datetime import datetime

ms.set_context(mode=ms.GRAPH_MODE, device_target="Ascend", device_id=0)

# ========== 配置后端API ==========
BACKEND_CONFIG = {
    "api_url": "http://115.120.205.25:8080/alert",
    "location": "综合教学楼4层东侧走廊",
    "alarm_threshold": 0.8
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

def send_alert(location):
    """发送告警到后端"""
    current_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    payload = {
        "location": location,
        "time": current_time,
        "status": "NEW"
    }

    try:
        response = requests.post(
            BACKEND_CONFIG["api_url"],
            json=payload,
            headers={"Content-Type": "application/json"},
            timeout=5
        )
        if response.status_code == 200:
            print(f"✅ 告警已上报: {location} at {current_time}")
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

    if pred_class == 1 and confidence >= BACKEND_CONFIG["alarm_threshold"]:
        send_alert(BACKEND_CONFIG["location"])
        time.sleep(10)  # 避免重复上报

    time.sleep(1)
