# LabVIEW Integration Guide for MAX6675 Temperature Sensors

## How to Send Commands to Arduino

### Method 1: Using VS Code Serial Monitor (Recommended)
1. **Open Serial Monitor in VS Code:**
   - Click on the serial monitor icon in the PlatformIO toolbar
   - Or use Command Palette: `PlatformIO: Serial Monitor`
   - Set baud rate to **9600**

2. **Send Commands:**
   - Type the command in the input field
   - Press Enter to send

### Method 2: Using Arduino IDE Serial Monitor
1. Open Arduino IDE
2. Go to Tools → Serial Monitor
3. Set baud rate to **9600**
4. Type command and click "Send"

### Method 3: Using Terminal (macOS/Linux)
```bash
# Connect to the Arduino
screen /dev/cu.usbmodem2101 9600

# Type commands and press Enter
# To exit: Ctrl+A, then K, then Y
```

## Available Commands

| Command | Function | Example Output |
|---------|----------|----------------|
| `LVON` | Enable LabVIEW CSV mode | CSV data stream |
| `LABVIEW` | Same as LVON | CSV data stream |
| `CSV` | Same as LVON | CSV data stream |
| `LVOFF` | Disable LabVIEW mode | Human-readable format |
| `HUMAN` | Same as LVOFF | Human-readable format |
| `CAL` | Enter calibration mode | Raw sensor readings |
| `EXIT` | Exit current mode | Return to previous mode |

## LabVIEW Integration

### Step 1: Enable LabVIEW Mode
Send the command: `LVON`

**Response:**
```
LabVIEW CSV mode enabled
Format: Sensor1_C,Sensor1_F,Sensor2_C,Sensor2_F,Sensor3_C,Sensor3_F,Sensor4_C,Sensor4_F
Error values represented as -999.0
```

### Step 2: LabVIEW Output Format
**CSV Format (recommended for LabVIEW):**
```
23.50,74.30,24.10,75.38,22.75,72.95,25.00,77.00
23.75,74.75,24.25,75.65,23.00,73.40,25.25,77.45
```

**Data Structure:**
- 8 values per line, comma-separated
- Values in order: Sensor1_°C, Sensor1_°F, Sensor2_°C, Sensor2_°F, Sensor3_°C, Sensor3_°F, Sensor4_°C, Sensor4_°F
- Error/disconnected sensors show as `-999.0`
- Update rate: 1 second in LabVIEW mode (faster than human mode)

### Step 3: LabVIEW VISA Configuration
**Serial Port Settings:**
- **Baud Rate:** 9600
- **Data Bits:** 8
- **Stop Bits:** 1
- **Parity:** None
- **Flow Control:** None
- **Port:** Check Device Manager (Windows) or `/dev/cu.usbmodem*` (macOS)

### Step 4: LabVIEW Data Parsing
**String Processing:**
1. Read line from serial port
2. Split string by comma delimiter
3. Convert strings to numbers
4. Handle error values (-999.0)

**Sample LabVIEW Block Diagram Flow:**
```
VISA Read → String Split → Array of Strings → String to Number → Temperature Array
```

## Sample LabVIEW VI Structure

### Front Panel Elements
- **String Indicator:** Raw serial data
- **Numeric Array:** Temperature values
- **Waveform Chart:** Real-time temperature plotting
- **Boolean Controls:** Start/Stop acquisition
- **String Control:** Send commands to Arduino

### Block Diagram Logic
1. **Initialize VISA:** Configure serial port
2. **Send Command:** Send "LVON" to Arduino
3. **Read Loop:**
   - VISA Read String
   - Parse CSV data
   - Update displays
   - Log data (optional)
4. **Cleanup:** Close VISA connection

## Error Handling

### Common Issues and Solutions

**No Data Received:**
- Check COM port selection
- Verify baud rate (9600)
- Ensure Arduino is connected
- Send `LVON` command first

**Invalid Data:**
- Values of -999.0 indicate sensor errors
- Check thermocouple connections
- Verify MAX6675 wiring

**Inconsistent Readings:**
- Allow sensors to stabilize
- Check for loose connections
- Consider calibration

## Advanced Features

### Automatic Command Sending
You can send commands programmatically from LabVIEW:
```
VISA Write: "LVON\n"
```

### Data Logging
Parse CSV data and log to file:
- Timestamp each reading
- Include sensor identification
- Handle error values appropriately

### Real-time Plotting
- Use Waveform Chart for continuous display
- Set appropriate time scale
- Include temperature limits/alarms

## Command Examples in Different Environments

### VS Code PlatformIO
1. Click Serial Monitor
2. Type: `LVON`
3. Press Enter
4. Watch for CSV output

### Arduino IDE
1. Tools → Serial Monitor
2. Type: `LVON`
3. Click Send
4. Observe data format change

### Python Script (for testing)
```python
import serial
import time

# Open serial connection
ser = serial.Serial('/dev/cu.usbmodem2101', 9600)
time.sleep(2)  # Wait for connection

# Enable LabVIEW mode
ser.write(b'LVON\n')

# Read data
while True:
    if ser.in_waiting:
        data = ser.readline().decode().strip()
        print(f"CSV Data: {data}")
        
        # Parse CSV
        if ',' in data:
            temps = data.split(',')
            print(f"Temps: {temps}")
```

The Arduino is now ready for LabVIEW integration! Send `LVON` to start receiving CSV-formatted temperature data.
