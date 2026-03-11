import serial
import numpy as np
import struct

# ========== 配置参数 ==========
SERIAL_PORT = '/dev/ttyUSB0'  # 串口设备
BAUD_RATE = 115200
TARGET_SHAPE = (1, 5, 64)  # 目标雷达数据形状

# HIF包头标识
PACKET_HEADER = 0xA5

def parse_hif_packet(ser):
    """解析一个HIF数据包，返回点云数据"""

    # 1. 查找包头 0xA5
    while True:
        byte = ser.read(1)
        if len(byte) == 0:
            return None
        if byte[0] == PACKET_HEADER:
            break

    # 2. 读取后续5字节（包头剩余部分）
    header_rest = ser.read(5)
    if len(header_rest) < 5:
        return None

    # 3. 读取数据头（包含点数信息）
    data_header = ser.read(5)
    if len(data_header) < 5:
        return None

    point_count = data_header[3]  # 第4个字节是点数

    # 4. 跳过 "motion_point_cloud" 字符串（23字节）
    ser.read(23)

    # 5. 读取点云数据（每个点20字节：x,y,z,velocity,SNR各4字节）
    points = []
    bytes_per_point = 20  # 5个float32

    for _ in range(point_count):
        point_data = ser.read(bytes_per_point)
        if len(point_data) < bytes_per_point:
            break

        # 解析5个float32值：x, y, z, velocity, SNR
        values = struct.unpack('<5f', point_data)
        points.append(values)

    return np.array(points, dtype=np.float32) if points else None

def collect_radar_data():
    """采集雷达数据并转换为目标格式"""

    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=2)
    print(f"✅ 串口已打开: {SERIAL_PORT}")

    all_points = []
    target_points = 64  # 需要64个点

    while len(all_points) < target_points:
        points = parse_hif_packet(ser)

        if points is not None and len(points) > 0:
            # 累积点云数据
            for point in points:
                if len(all_points) < target_points:
                    all_points.append(point)

            print(f"📦 已采集 {len(all_points)}/{target_points} 个点")

    # 转换为 (64, 5) 然后转置为 (5, 64)
    radar_array = np.array(all_points[:target_points], dtype=np.float32).T  # (5, 64)
    radar_array = radar_array.reshape(1, 5, 64)

    # 保存
    np.save("real_radar_normal.npy", radar_array)
    print(f"✅ 雷达数据已保存: real_radar_normal.npy, shape={radar_array.shape}")

    ser.close()
    return radar_array

if __name__ == "__main__":
    try:
        radar_data = collect_radar_data()
        print(f"📊 数据统计: min={radar_data.min():.4f}, max={radar_data.max():.4f}")
    except KeyboardInterrupt:
        print("\n⚠️ 用户中断")
    except Exception as e:
        print(f"❌ 错误: {e}")
        import traceback
        traceback.print_exc()
