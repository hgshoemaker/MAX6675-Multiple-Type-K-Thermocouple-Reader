"""
MAX6675 VISA Mode Test Script

This Python script demonstrates how to communicate with the MAX6675 
thermocouple reader in VISA command-response mode.

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
COM_PORT = 'COM3'  # Update this to match your Arduino's COM port
BAUD_RATE = 9600
TIMEOUT = 2.0

def send_command(ser, command):
    """Send a command and return the response"""
    # Send command
    ser.write(f"{command}\n".encode())
    
    # Read response
    response = ser.readline().decode().strip()
    return response

def main():
    try:
        # Open serial connection
        print(f"Connecting to {COM_PORT}...")
        ser = serial.Serial(COM_PORT, BAUD_RATE, timeout=TIMEOUT)
        time.sleep(2)  # Wait for Arduino to initialize
        
        print("Connected! Enabling VISA mode...")
        
        # Enable VISA mode
        response = send_command(ser, "VSON")
        print(f"VISA Mode Response: {response}")
        
        # Wait a moment
        time.sleep(1)
        
        # Test instrument identification
        print("\n=== INSTRUMENT IDENTIFICATION ===")
        idn = send_command(ser, "*IDN?")
        print(f"Device ID: {idn}")
        
        # Test system information
        print("\n=== SYSTEM INFORMATION ===")
        version = send_command(ser, "SYST:VERS?")
        print(f"Version: {version}")
        
        sensor_count = send_command(ser, "CONF:SENS:COUN?")
        print(f"Sensor Count: {sensor_count}")
        
        update_rate = send_command(ser, "CONF:RATE?")
        print(f"Update Rate: {update_rate} Hz")
        
        # Test temperature readings
        print("\n=== TEMPERATURE READINGS ===")
        
        # Read all temperatures (calibrated)
        all_temps = send_command(ser, "MEAS:TEMP? ALL")
        print(f"All Temperatures - Calibrated (CSV): {all_temps}")
        
        # Read all temperatures (raw)
        all_temps_raw = send_command(ser, "MEAS:TEMP:RAW? ALL")
        print(f"All Temperatures - Raw (CSV): {all_temps_raw}")
        
        # Parse and display individual temperatures
        temp_values = all_temps.split(',')
        print("\nCalibrated Temperatures:")
        for i, temp in enumerate(temp_values, 1):
            temp_str = temp.strip()
            if temp_str == "-999.00":
                print(f"  Channel {i}: ERROR (sensor disconnected)")
            else:
                print(f"  Channel {i}: {temp_str}°C")
        
        # Parse and display raw temperatures
        temp_values_raw = all_temps_raw.split(',')
        print("\nRaw Temperatures:")
        for i, temp in enumerate(temp_values_raw, 1):
            temp_str = temp.strip()
            if temp_str == "-999.00":
                print(f"  Channel {i}: ERROR (sensor disconnected)")
            else:
                print(f"  Channel {i}: {temp_str}°C")
        
        # Test individual channel readings
        print("\n=== INDIVIDUAL CHANNEL READINGS ===")
        for channel in range(1, 4):  # Test first 3 channels
            # Calibrated reading
            temp = send_command(ser, f"MEAS:TEMP? CH{channel}")
            print(f"Channel {channel} (Calibrated): {temp}°C")
            
            # Raw reading
            temp_raw = send_command(ser, f"MEAS:TEMP:RAW? CH{channel}")
            print(f"Channel {channel} (Raw): {temp_raw}°C")
        
        # Test invalid channel
        print("\n=== ERROR HANDLING TEST ===")
        invalid_channel = send_command(ser, "MEAS:TEMP? CH99")
        print(f"Invalid Channel Response: {invalid_channel}")
        
        unknown_command = send_command(ser, "INVALID_COMMAND")
        print(f"Unknown Command Response: {unknown_command}")
        
        # Test system error query
        sys_error = send_command(ser, "SYST:ERR?")
        print(f"System Error: {sys_error}")
        
        # Show help
        print("\n=== HELP INFORMATION ===")
        help_info = send_command(ser, "HELP?")
        print("Available Commands:")
        print(help_info)
        
        # Performance test
        print("\n=== PERFORMANCE TEST ===")
        print("Testing response time for 10 measurements...")
        start_time = time.time()
        
        for i in range(10):
            temp = send_command(ser, "MEAS:TEMP? CH1")
            print(f"  Measurement {i+1}: {temp}°C")
        
        end_time = time.time()
        avg_time = (end_time - start_time) / 10
        print(f"Average response time: {avg_time:.3f} seconds")
        
        # Return to human mode
        print("\n=== RETURNING TO HUMAN MODE ===")
        response = send_command(ser, "MODE:HUMAN")
        print(f"Mode Change Response: {response}")
        
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
