import serial
import numpy as np
import struct

# ========== 配置参数 ==========
SERIAL_PORT = '/dev/ttyUSB0'  # 香橙派上的串口设备，根据实际情况修改
BAUD_RATE = 115200  # 波特率，根据你的设备配置修改
SAMPLE_RATE = 16000  # 采样率
AUDIO_LENGTH = 16000  # 需要的音频采样点数

# 包头包尾定义（根据你的实际协议修改）
PACKET_HEADER = b'\xAA\x55'  # 示例包头
PACKET_FOOTER = b'\x55\xAA'  # 示例包尾

def read_audio_from_serial():
    """从串口读取音频数据并保存为npy文件"""

    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=2)
    print(f"✅ 串口已打开: {SERIAL_PORT} @ {BAUD_RATE}")

    audio_samples = []

    while len(audio_samples) < AUDIO_LENGTH:
        # 1. 查找包头
        header = ser.read(2)
        if header != PACKET_HEADER:
            continue

        # 2. 读取数据长度（假设2字节表示数据长度）
        length_bytes = ser.read(2)
        data_length = struct.unpack('<H', length_bytes)[0]

        # 3. 读取PCM数据（假设16位有符号整数）
        pcm_data = ser.read(data_length)

        # 4. 验证包尾
        footer = ser.read(2)
        if footer != PACKET_FOOTER:
            print("⚠️ 包尾校验失败，跳过此包")
            continue

        # 5. 解析PCM数据（16位有符号整数）
        num_samples = data_length // 2
        samples = struct.unpack(f'<{num_samples}h', pcm_data)
        audio_samples.extend(samples)

        print(f"📦 接收到 {num_samples} 个采样点，总计: {len(audio_samples)}/{AUDIO_LENGTH}")

    # 截取所需长度
    audio_samples = audio_samples[:AUDIO_LENGTH]

    # 转换为float32并归一化到[-1, 1]
    audio_array = np.array(audio_samples, dtype=np.float32) / 32768.0

    # 调整形状为 (1, 1, 16000)
    audio_array = audio_array.reshape(1, 1, AUDIO_LENGTH)

    # 保存为npy文件
    np.save("real_audio_normal.npy", audio_array)
    print(f"✅ 音频数据已保存: real_audio_normal.npy, shape={audio_array.shape}")

    ser.close()
    return audio_array

if __name__ == "__main__":
    try:
        audio_data = read_audio_from_serial()
        print(f"📊 音频统计: min={audio_data.min():.4f}, max={audio_data.max():.4f}, mean={audio_data.mean():.4f}")
    except KeyboardInterrupt:
        print("\n⚠️ 用户中断")
    except Exception as e:
        print(f"❌ 错误: {e}")
