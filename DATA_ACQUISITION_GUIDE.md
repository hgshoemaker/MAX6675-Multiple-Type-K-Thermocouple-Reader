# Data Acquisition Integration Guide for MAX6675 Temperature Sensors

## Overview

The MAX6675 thermocouple reader provides a simplified CSV data stream for integration with data acquisition systems including LabVIEW, MATLAB, Python, and other platforms.

## CSV Data Format

### Output Format
```
S1_C,S2_C,S3_C,S4_C,S5_C,S6_C,S7_C,S8_C,V1,V2
```

Where:
- `S1_C` to `S8_C`: Temperature readings from sensors 1-8 in Celsius
- `V1`, `V2`: Voltage readings from ADS1115 channels A0 and A1

### Example Output
```
23.50,24.10,22.75,25.00,23.25,24.50,23.00,24.75,0.1234,0.5678
23.75,24.25,23.00,25.25,23.50,24.75,23.25,25.00,0.1245,0.5689
24.00,24.40,23.25,25.50,23.75,25.00,23.50,25.25,0.1256,0.5700
```

### Data Characteristics
- **Update Rate**: Every 5 seconds
- **Temperature Precision**: 2 decimal places (±0.01°C display resolution)
- **Voltage Precision**: 4 decimal places (±0.0001V display resolution)
- **Error Values**: `-999.00` for temperature, `-999.0000` for voltage
- **Data Encoding**: ASCII text, newline terminated

## Serial Communication Setup

### Connection Parameters
- **Baud Rate**: 9600
- **Data Bits**: 8
- **Stop Bits**: 1
- **Parity**: None
- **Flow Control**: None

### Available Commands

| Command | Function | Response |
|---------|----------|----------|
| `CAL` | Enter calibration mode | Raw sensor readings for calibration |
| `DEBUG` | Test individual sensors | Individual sensor status |
| `EXIT` | Exit calibration mode | Return to CSV output |

## LabVIEW Integration

### Step 1: Configure VISA Resource
1. Open LabVIEW
2. Create a new VI
3. Place "VISA Configure Serial Port" on the block diagram
4. Configure:
   - **Resource Name**: Your COM port (e.g., COM3, /dev/ttyUSB0)
   - **Baud Rate**: 9600
   - **Data Bits**: 8
   - **Stop Bits**: 1
   - **Parity**: None

### Step 2: Read CSV Data
1. Place "VISA Read" on the block diagram
2. Configure:
   - **Bytes to Read**: 100 (sufficient for one CSV line)
   - **Termination Character**: Enabled, set to Line Feed (0x0A)

### Step 3: Parse CSV Data
1. Use "Spreadsheet String to Array" function
2. Configure:
   - **Delimiter**: Comma (,)
   - **Expected Data Type**: Double Array
3. The output array will contain:
   - Index 0-7: Temperature readings (S1-S8)
   - Index 8-9: Voltage readings (V1-V2)

### Example LabVIEW VI Structure
```
[VISA Configure Serial Port] → [VISA Read] → [Spreadsheet String to Array] → [Array Index] → [Temperature Displays]
                                    ↓
                               [While Loop with 5s Delay]
```

## Python Integration

### Simple Data Logger
```python
import serial
import time
import csv

# Configure serial connection
ser = serial.Serial('COM3', 9600, timeout=1)  # Adjust COM port
time.sleep(2)  # Wait for Arduino to initialize

# Open CSV file for logging
with open('temperature_log.csv', 'w', newline='') as csvfile:
    writer = csv.writer(csvfile)
    
    # Write header
    writer.writerow(['Timestamp', 'S1_C', 'S2_C', 'S3_C', 'S4_C', 
                     'S5_C', 'S6_C', 'S7_C', 'S8_C', 'V1', 'V2'])
    
    try:
        while True:
            if ser.in_waiting > 0:
                line = ser.readline().decode('utf-8').strip()
                if line and ',' in line:
                    data = line.split(',')
                    if len(data) == 10:  # 8 temperatures + 2 voltages
                        timestamp = time.strftime('%Y-%m-%d %H:%M:%S')
                        writer.writerow([timestamp] + data)
                        csvfile.flush()  # Ensure data is written
                        print(f"{timestamp}: {line}")
            
            time.sleep(0.1)  # Small delay to prevent CPU overload
            
    except KeyboardInterrupt:
        print("Logging stopped by user")
    finally:
        ser.close()
```

## MATLAB Integration

### Real-time Data Acquisition
```matlab
% Configure serial connection
s = serialport("COM3", 9600);  % Adjust COM port
configureTerminator(s, "LF");

% Pre-allocate data arrays
maxSamples = 1000;
timestamps = datetime.empty(maxSamples, 0);
tempData = NaN(maxSamples, 8);
voltData = NaN(maxSamples, 2);

sampleCount = 0;

try
    while sampleCount < maxSamples
        if s.NumBytesAvailable > 0
            line = readline(s);
            data = str2double(split(line, ','));
            
            if length(data) == 10
                sampleCount = sampleCount + 1;
                timestamps(sampleCount) = datetime('now');
                tempData(sampleCount, :) = data(1:8);
                voltData(sampleCount, :) = data(9:10);
                
                % Display current reading
                fprintf('Sample %d: Avg Temp = %.2f°C\n', ...
                    sampleCount, mean(data(1:8)));
            end
        end
        pause(0.1);
    end
catch ME
    fprintf('Error: %s\n', ME.message);
end

% Close serial connection
clear s;

% Plot results
figure;
subplot(2,1,1);
plot(timestamps(1:sampleCount), tempData(1:sampleCount, :));
title('Temperature Readings');
ylabel('Temperature (°C)');
legend(arrayfun(@(x) sprintf('S%d', x), 1:8, 'UniformOutput', false));

subplot(2,1,2);
plot(timestamps(1:sampleCount), voltData(1:sampleCount, :));
title('Voltage Readings');
ylabel('Voltage (V)');
legend({'V1 (A0)', 'V2 (A1)'});
```

## Troubleshooting

### Common Issues

**No data received:**
- Check COM port settings
- Verify baud rate is 9600
- Ensure Arduino is powered and connected
- Check if device is in calibration mode (send `EXIT` command)

**Incomplete data lines:**
- Increase read timeout
- Check for proper line termination handling
- Verify CSV parsing handles variable line lengths

**Invalid temperature readings (-999.00):**
- Check thermocouple connections
- Verify MAX6675 wiring
- Test individual sensors with `DEBUG` command

**Invalid voltage readings (-999.0000):**
- Check ADS1115 I2C connections (SDA=Pin20, SCL=Pin21)
- Verify ADS1115 power supply (VDD=5V, GND=GND)
- Test ADS1115 with multimeter

### Performance Optimization

**For high-frequency logging:**
- Use buffered reading to handle burst data
- Implement data validation before processing
- Consider using asynchronous I/O for continuous operation

**For long-term logging:**
- Implement file rotation to manage log file sizes
- Add timestamp synchronization
- Include error recovery mechanisms

## Data Validation

### Temperature Range Checking
```python
def validate_temperature(temp_str):
    try:
        temp = float(temp_str)
        if temp == -999.00:
            return None, "Sensor disconnected"
        elif temp < -40 or temp > 1000:
            return None, "Temperature out of range"
        else:
            return temp, "OK"
    except ValueError:
        return None, "Invalid data format"
```

### Voltage Range Checking
```python
def validate_voltage(volt_str):
    try:
        volt = float(volt_str)
        if volt == -999.0000:
            return None, "ADC disconnected"
        elif volt < -6.144 or volt > 6.144:
            return None, "Voltage out of range"
        else:
            return volt, "OK"
    except ValueError:
        return None, "Invalid data format"
```

This simplified CSV interface provides reliable data acquisition for various applications while maintaining compatibility with popular analysis platforms.
