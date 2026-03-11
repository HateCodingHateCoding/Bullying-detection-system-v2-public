import numpy as np
import mindspore as ms
import mindspore.nn as nn
import mindspore.ops as ops
import time

ms.set_context(mode=ms.GRAPH_MODE, device_target="Ascend", device_id=0)

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

print("1. Build Network and Load CKPT...")
net = AscendSentinelNet()
ms.load_param_into_net(net, ms.load_checkpoint("sentinel_model.ckpt"), strict_load=False)
net = ms.amp.auto_mixed_precision(net, amp_level="O2")
net.set_train(False)

print("2. Load REAL Normal Data...")
real_radar = ms.Tensor(np.load("real_radar_normal.npy").astype(np.float32))
real_audio = ms.Tensor(np.load("real_audio_normal.npy").astype(np.float32))

print("3. Start NPU Warm-up (Compiling Graph)...")
_ = net(real_radar, real_audio) 

print("4. Start Real NPU Speed Test (100 iterations)...")
start_time = time.time()
for _ in range(100):
    logits = net(real_radar, real_audio)
end_time = time.time()

real_latency = ((end_time - start_time) / 100) * 1000
result_logits = logits.asnumpy()
pred_class = np.argmax(result_logits[0])
status = "Warning: Anomaly/Bullying" if pred_class == 1 else "Normal"

print("\n" + "="*50)
print(f"☁️ Cloud Target Logits: [[ 3.9161448 -4.5243397]]")
print(f"💻 Edge NPU Logits:   {result_logits}")
print(f"🎯 Result (Classification): {status}")
print(f"⚡ REAL NPU Average Latency: {real_latency:.2f} ms")
print("="*50)
