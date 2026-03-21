import serial
import time

UART_DEV = "/dev/ttyAMA0"
BAUDRATE = 115200

ALARM_CMD = bytes([0xAA, 0x55, 0x01, 0x1E])

def send_alarm_cmd() -> None:
    ser = serial.Serial(
        port=UART_DEV,
        baudrate=BAUDRATE,
        bytesize=serial.EIGHTBITS,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        timeout=1,
    )

    try:
        time.sleep(0.2)
        ser.write(ALARM_CMD)
        ser.flush()
        print(f"sent alarm cmd to {UART_DEV}: {ALARM_CMD.hex(' ')}")
    finally:
        ser.close()

if __name__ == "__main__":
    send_alarm_cmd()