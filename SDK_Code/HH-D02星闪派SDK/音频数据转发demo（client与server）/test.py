import serial
import time

# --- Configuration ---
# Make sure this matches the device name you found in step 1
SERIAL_PORT = '/dev/ttyUSB0'
# The D02 board uses a baud rate of 115200
BAUD_RATE = 115200

print("--- Orange Pi Serial Listener ---")
print(f"Attempting to open port {SERIAL_PORT} at {BAUD_RATE} baud.")

try:
    # Open the serial port
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    print("Port opened successfully. Listening for data...")

    while True:
        # Read any available data from the serial buffer
        received_bytes = ser.readline()
        
        # If data was received, print it
        if received_bytes:
            print(f"Received from D02: {received_bytes}")
            
except serial.SerialException as e:
    print(f"ERROR: Could not open or read from serial port.")
    print(f"Details: {e}")
    print("Troubleshooting:")
    print("1. Is the USB-to-UART converter plugged into the Orange Pi?")
    print("2. Did you run 'ls /dev/ttyUSB*' to confirm the port name is correct?")
    print("3. Did you run 'sudo chmod 666 /dev/ttyUSB0' to set permissions?")
    
except KeyboardInterrupt:
    print("\nProgram stopped by user.")
    
finally:
    # Ensure the port is closed when the script ends
    if 'ser' in locals() and ser.is_open:
        ser.close()
        print("Serial port closed.")