"""
MAX6675 Serial Communication Test Script

This Python script demonstrates how to communicate with the MAX6675 
thermocouple reader us        # Test 5: Return to human mode
        print("\n=== RETURNING TO HUMAN MODE ===")
        send_command(ser, "EXIT")
        time.sleep(2)
        
        # Read a few lines to confirm
        for i in range(5):
            if ser.in_waiting > 0:
                line = ser.readline().decode().strip()
                if line:
                    print(f"  {line}")
        
        # Test 6: Command summary
        print("\n=== AVAILABLE COMMANDS ===")
        print("Commands that work with your Arduino:")
        print("  LVON / LABVIEW / CSV  - Enable CSV output mode")
        print("  LVOFF / HUMAN         - Enable human-readable mode")
        print("  CAL                   - Enter calibration mode")
        print("  DEBUG                 - Test individual sensors")
        print("  EXIT                  - Exit current mode")emented commands.

Requirements:
- pyserial library: pip install pyserial
- Arduino with MAX6675 code running
- Serial connection established

Usage:
1. Update the COM_PORT variable to match your Arduino's port
2. Run the script: python visa_test.py
"""

import serial
import time

# Configuration
COM_PORT = '/dev/cu.usbmodem1101'  # macOS serial port for Arduino Mega 2560
BAUD_RATE = 9600
TIMEOUT = 2.0

def send_command(ser, command):
    """Send a command to Arduino"""
    print(f"Sending command: {command}")
    ser.write(f"{command}\n".encode())
    time.sleep(0.5)  # Give Arduino time to process

def read_data(ser, num_lines=1):
    """Read data from Arduino"""
    data = []
    for _ in range(num_lines):
        if ser.in_waiting > 0:
            line = ser.readline().decode().strip()
            if line:
                data.append(line)
    return data

def main():
    try:
        # Open serial connection
        print(f"Connecting to {COM_PORT}...")
        ser = serial.Serial(COM_PORT, BAUD_RATE, timeout=TIMEOUT)
        time.sleep(3)  # Wait for Arduino to initialize
        
        print("Connected! Testing Arduino commands...")
        
        # Clear any initial data
        ser.flushInput()
        
        # Test 1: Read normal human-readable output
        print("\n=== NORMAL MODE (Human-Readable) ===")
        send_command(ser, "HUMAN")
        time.sleep(2)
        
        # Read several lines of normal output
        for i in range(12):  # Read header + 8 sensors + footer
            if ser.in_waiting > 0:
                line = ser.readline().decode().strip()
                if line:
                    print(f"  {line}")
        
        # Test 2: Enable LabVIEW CSV mode
        print("\n=== LABVIEW CSV MODE ===")
        send_command(ser, "LVON")
        time.sleep(3)  # Wait longer for mode to stabilize
        
        # Read CSV data
        print("Reading CSV data for 8 seconds...")
        start_time = time.time()
        csv_lines = []
        
        while time.time() - start_time < 8:  # Longer reading time
            if ser.in_waiting > 0:
                line = ser.readline().decode().strip()
                if line and ',' in line and not line.startswith('Format'):  # CSV data line
                    csv_lines.append(line)
                    print(f"  CSV: {line}")
                elif line:  # Other output
                    print(f"  Info: {line}")
        
        # Parse CSV data
        if csv_lines:
            print(f"\nParsed {len(csv_lines)} CSV readings:")
            latest_csv = csv_lines[-1]
            temps = latest_csv.split(',')
            print(f"Raw CSV data: {latest_csv}")
            print(f"Number of temperature values: {len(temps)}")
            
            for i, temp in enumerate(temps, 1):
                try:
                    temp_val = float(temp.strip())
                    if temp_val == -999.0:
                        print(f"  Sensor {i}: ERROR (disconnected)")
                    elif temp_val < -100 or temp_val > 1000:
                        print(f"  Sensor {i}: INVALID ({temp_val:.2f}°C - out of range)")
                    else:
                        print(f"  Sensor {i}: {temp_val:.2f}°C")
                except ValueError:
                    print(f"  Sensor {i}: Invalid data ({temp})")
            
            # Show all 8 sensors even if some are missing
            if len(temps) < 8:
                print(f"WARNING: Only {len(temps)} sensors detected, expected 8")
                for i in range(len(temps) + 1, 9):
                    print(f"  Sensor {i}: NOT DETECTED")
        else:
            print("No CSV data received - check sensor connections")
        
        # Test 3: Debug individual sensors
        print("\n=== INDIVIDUAL SENSOR DEBUG ===")
        send_command(ser, "DEBUG")
        time.sleep(3)
        
        # Read debug data
        print("Reading individual sensor test results...")
        for i in range(20):  # Read individual sensor debug output
            if ser.in_waiting > 0:
                line = ser.readline().decode().strip()
                if line:
                    print(f"  {line}")
        
        # Test 4: Calibration mode
        print("\n=== CALIBRATION MODE ===")
        send_command(ser, "CAL")
        time.sleep(3)
        
        # Read calibration data
        print("Reading calibration data...")
        for i in range(15):  # Read calibration output
            if ser.in_waiting > 0:
                line = ser.readline().decode().strip()
                if line:
                    print(f"  {line}")
        
        # Test 5: Return to human mode
        print("\n=== RETURNING TO HUMAN MODE ===")
        send_command(ser, "EXIT")
        time.sleep(2)
        
        # Read a few lines to confirm
        for i in range(5):
            if ser.in_waiting > 0:
                line = ser.readline().decode().strip()
                if line:
                    print(f"  {line}")
        
        # Test 5: Command summary
        print("\n=== AVAILABLE COMMANDS ===")
        print("Commands that work with your Arduino:")
        print("  LVON / LABVIEW / CSV  - Enable CSV output mode")
        print("  LVOFF / HUMAN         - Enable human-readable mode")
        print("  CAL                   - Enter calibration mode")
        print("  EXIT                  - Exit current mode")
        
        print("\nTest completed successfully!")
        
    except serial.SerialException as e:
        print(f"Serial communication error: {e}")
        print("Please check:")
        print("1. COM port is correct")
        print("2. Arduino is connected")
        print("3. No other programs are using the serial port")
        
    except KeyboardInterrupt:
        print("\nTest interrupted by user")
        
    except Exception as e:
        print(f"Unexpected error: {e}")
        
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
            print("Serial connection closed")

if __name__ == "__main__":
    print("MAX6675 VISA Mode Test Script")
    print("============================")
    print("Make sure your Arduino is connected and running the MAX6675 code")
    print("Press Ctrl+C to exit at any time\n")
    
    main()
