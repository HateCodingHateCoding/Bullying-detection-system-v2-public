import time
import statistics
import os
import json
import sys
from pathlib import Path
from iot_uploader import HuaweiIoTUploader

try:
    import numpy as np
    import mindspore as ms
    import mindspore.nn as nn
    import mindspore.ops as ops
    HAS_AI_STACK = True
    AI_IMPORT_ERROR = ""
except Exception as e:
    HAS_AI_STACK = False
    AI_IMPORT_ERROR = str(e)

if HAS_AI_STACK:
    ai_mode = (os.getenv("AI_MODE", "GRAPH") or "GRAPH").upper()
    ms_mode = ms.PYNATIVE_MODE if ai_mode == "PYNATIVE" else ms.GRAPH_MODE
    ms.set_context(
        mode=ms_mode,
        device_target=os.getenv("MS_DEVICE_TARGET", "Ascend"),
        device_id=int(os.getenv("MS_DEVICE_ID", "0")),
    )
    print(f"ℹ️ MindSpore启动模式: {ai_mode}")

# ========== 配置加载 ==========
def _load_device_key_file():
    """Load device credentials deterministically from workspace root key files."""
    root_dir = Path(__file__).resolve().parent.parent

    def _collect_key_files(base_dir):
        files = []
        files.extend(base_dir.glob("DEVICES-KEY-*.txt"))
        files.extend(base_dir.glob("DEVICES-CONNECT-KEY-*.txt"))
        return files

    def _normalize_key_data(raw):
        # Format A: {"device_id": "...", "secret": "..."}
        if raw.get("device_id") and raw.get("secret"):
            return {
                "device_id": raw.get("device_id", ""),
                "secret": raw.get("secret", ""),
                "username": raw.get("username", ""),
                "client_id": raw.get("clientId", raw.get("client_id", "")),
                "platform_url": raw.get("hostname", raw.get("platform_url", "")),
                "port": raw.get("port", 8883),
                "auth_mode": raw.get("auth_mode", "hmac"),
            }

        # Format B: connect key exported by IoTDA console.
        if raw.get("username") and raw.get("password"):
            return {
                "device_id": raw.get("username", ""),
                "secret": raw.get("password", ""),
                "username": raw.get("username", ""),
                "client_id": raw.get("clientId", ""),
                "platform_url": raw.get("hostname", ""),
                "port": raw.get("port", 8883),
                "auth_mode": "plain",
            }

        return {}

    # 1) Explicit override: use a specific key file path.
    forced_key_file = os.getenv("IOT_KEY_FILE", "").strip()
    if forced_key_file:
        key_path = Path(forced_key_file)
        if not key_path.is_absolute():
            key_path = root_dir / key_path
        if key_path.exists():
            try:
                data = _normalize_key_data(json.loads(key_path.read_text(encoding="utf-8")))
                if data.get("device_id") and data.get("secret"):
                    print(f"✅ 已加载指定设备凭证文件: {key_path.name}")
                    return data
            except Exception:
                pass
        print(f"⚠️ IOT_KEY_FILE 无效或不可读: {forced_key_file}")

    key_files = _collect_key_files(root_dir)

    # 2) Match desired device id (from env), if provided.
    desired_device_id = (os.getenv("IOT_DEVICE_ID") or os.getenv("IOT_USERNAME") or "").strip()
    if desired_device_id:
        for key_file in key_files:
            try:
                data = _normalize_key_data(json.loads(key_file.read_text(encoding="utf-8")))
                if data.get("device_id") == desired_device_id and data.get("secret"):
                    print(f"✅ 已按设备ID匹配凭证文件: {key_file.name}")
                    return data
            except Exception:
                continue

    # 3) Fallback: latest modified valid key file.
    key_files.sort(key=lambda p: p.stat().st_mtime, reverse=True)
    for key_file in key_files:
        try:
            data = _normalize_key_data(json.loads(key_file.read_text(encoding="utf-8")))
            if data.get("device_id") and data.get("secret"):
                print(f"✅ 已加载最新设备凭证文件: {key_file.name}")
                return data
        except Exception:
            continue
    return {}


_device_key = _load_device_key_file()

IOT_CONFIG = {
    "device_id": os.getenv("IOT_DEVICE_ID") or _device_key.get("device_id", ""),
    "secret": os.getenv("IOT_DEVICE_SECRET") or _device_key.get("secret", ""),
    "platform_url": os.getenv("IOT_PLATFORM_URL") or _device_key.get("platform_url", ""),
    "port": int(os.getenv("IOT_PORT") or _device_key.get("port", 8883)),
    "username": os.getenv("IOT_USERNAME") or _device_key.get("username", ""),
    "client_id": os.getenv("IOT_CLIENT_ID") or _device_key.get("client_id", ""),
    "auth_mode": os.getenv("IOT_AUTH_MODE") or _device_key.get("auth_mode", "hmac"),
    "location": os.getenv("IOT_LOCATION", "综合教学楼4层东侧走廊"),
    "alarm_threshold": float(os.getenv("ALARM_THRESHOLD", "0.8"))
}

# ========== 配置OBS参数 ==========
OBS_CONFIG = {
    "ak": os.getenv("OBS_AK", "your_access_key"),
    "sk": os.getenv("OBS_SK", "your_secret_key"),
    "endpoint": os.getenv("OBS_ENDPOINT", "obs.cn-north-4.myhuaweicloud.com"),
    "bucket": os.getenv("OBS_BUCKET", "bullying-evidence")
}

if HAS_AI_STACK:
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

if not IOT_CONFIG["device_id"] or not IOT_CONFIG["secret"]:
    raise RuntimeError(
        "缺少设备凭证。请设置 IOT_DEVICE_ID/IOT_DEVICE_SECRET，"
        "或确保工作区根目录存在有效的 DEVICES-KEY-*.txt 文件。"
    )

if not IOT_CONFIG["platform_url"]:
    raise RuntimeError(
        "缺少 IoTDA 接入地址。请设置环境变量 IOT_PLATFORM_URL，"
        "例如: 12345678.st1.iotda-device.cn-north-4.myhuaweicloud.com"
    )

iot = HuaweiIoTUploader(
    IOT_CONFIG["device_id"],
    IOT_CONFIG["secret"],
    IOT_CONFIG["platform_url"],
    port=IOT_CONFIG["port"],
    username=IOT_CONFIG["username"] or IOT_CONFIG["device_id"],
    client_id=IOT_CONFIG["client_id"] or IOT_CONFIG["device_id"],
    auth_mode=IOT_CONFIG["auth_mode"]
)
iot.connect()

if not HAS_AI_STACK:
    print(f"⚠️ 未检测到推理依赖，跳过模型推理: {AI_IMPORT_ERROR}")
    print("ℹ️ 当前仅做 IoT 连通性测试并发送一条测试告警。")
    iot.upload_alarm(
        alarm_type="bullying_test",
        confidence=1.0,
        location=IOT_CONFIG["location"],
        audio_level=0,
        radar_summary="iot-only-smoke-test",
        evidence_url=""
    )
    time.sleep(2)
    iot.disconnect()
    sys.exit(0)

from obs_uploader import OBSUploader

# 延迟OBS初始化，避免多进程内存问题
obs = None

print("2. 加载模型...")
net = AscendSentinelNet()
ms.load_param_into_net(net, ms.load_checkpoint("sentinel_model.ckpt"), strict_load=False)
amp_level = (os.getenv("MS_AMP_LEVEL", "O2") or "O2").upper()
if amp_level != "O0":
    net = ms.amp.auto_mixed_precision(net, amp_level=amp_level)
    print(f"ℹ️ AMP已启用: {amp_level}")
else:
    print("ℹ️ AMP已关闭(MS_AMP_LEVEL=O0)")
net.set_train(False)

enable_warmup = (os.getenv("ENABLE_WARMUP", "1") or "1").lower() in ("1", "true", "yes", "on")
if enable_warmup:
    print("3. 模型预热...")
    dummy_radar = ms.Tensor(np.zeros([1, 5, 64], dtype=np.float32))
    dummy_audio = ms.Tensor(np.zeros([1, 1, 16000], dtype=np.float32))
    _ = net(dummy_radar, dummy_audio)
    print("✅ 预热完成")
else:
    print("3. 跳过模型预热(ENABLE_WARMUP=0)")

print("4. 加载数据...")
radar_np = np.load("real_radar_normal.npy").astype(np.float32)
audio_np = np.load("real_audio_normal.npy").astype(np.float32)
radar_data = ms.Tensor(radar_np)
audio_data = ms.Tensor(audio_np)

bench_window = max(1, int(os.getenv("BENCH_WINDOW", "100")))
loop_sleep_sec = float(os.getenv("LOOP_SLEEP_SEC", "0"))
lat_ms_window = []
infer_count = 0

print("5. 开始实时监控...")
while True:
    infer_start = time.perf_counter()
    logits = net(radar_data, audio_data)
    probs = ops.Softmax(axis=1)(logits).asnumpy()[0]
    pred_class = np.argmax(probs)
    confidence = probs[pred_class]

    infer_ms = (time.perf_counter() - infer_start) * 1000.0
    lat_ms_window.append(infer_ms)
    infer_count += 1

    if len(lat_ms_window) >= bench_window:
        avg_ms = statistics.mean(lat_ms_window)
        p95_ms = float(np.percentile(lat_ms_window, 95))
        fps = 1000.0 / avg_ms if avg_ms > 0 else 0.0
        print(f"⚡ BENCH[{bench_window}] count={infer_count} avg={avg_ms:.2f}ms p95={p95_ms:.2f}ms fps={fps:.2f}")
        lat_ms_window.clear()

    # 判断是否告警
    if pred_class == 1 and confidence >= IOT_CONFIG["alarm_threshold"]:
        audio_level = 90

        # 懒加载OBS（首次使用时初始化）
        if obs is None:
            print("⚠️ 首次告警，初始化OBS连接...")
            obs = OBSUploader(
                OBS_CONFIG["ak"],
                OBS_CONFIG["sk"],
                OBS_CONFIG["endpoint"],
                OBS_CONFIG["bucket"]
            )

        # 上传音频证据到OBS
        audio_raw = audio_data.asnumpy()[0, 0, :]
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

    if loop_sleep_sec > 0:
        time.sleep(loop_sleep_sec)
