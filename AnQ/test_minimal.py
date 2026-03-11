import os
import sys
import time
import numpy as np

# 禁用多进程
os.environ['OMP_NUM_THREADS'] = '1'
os.environ['MKL_NUM_THREADS'] = '1'

try:
    import mindspore as ms
    import mindspore.nn as nn
    HAS_MS = True
except:
    HAS_MS = False
    print("❌ MindSpore未安装")
    sys.exit(1)

ms.set_context(mode=ms.PYNATIVE_MODE, device_target="Ascend", device_id=0)

from iot_uploader import HuaweiIoTUploader

print("1. 测试IoT连接...")
iot = HuaweiIoTUploader(
    "69a8ce6be094d6159222bfa4_orangepi_001",
    "your_secret",
    "your_platform_url",
    port=8883
)
print("✅ IoT对象创建成功")

print("2. 测试模型创建（不加载权重）...")
class SimpleNet(nn.Cell):
    def __init__(self):
        super().__init__()
        self.fc = nn.Dense(10, 2)

    def construct(self, x):
        return self.fc(x)

net = SimpleNet()
net.set_train(False)
print("✅ 模型创建成功")

print("3. 测试推理...")
dummy_input = ms.Tensor(np.random.randn(1, 10).astype(np.float32))
output = net(dummy_input)
print(f"✅ 推理成功: {output.shape}")

print("\n🎉 所有测试通过！问题在于checkpoint加载")
