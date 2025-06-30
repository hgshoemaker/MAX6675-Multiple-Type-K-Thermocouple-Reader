# MAX6675 Multiple Type K Thermocouple Reader

A comprehensive Arduino Mega 2560 project for reading multiple Type K thermocouples using MAX6675 amplifier modules, with calibration features and LabVIEW integration.

## Features

- 🌡️ **Multiple Sensor Support**: Read up to 8 MAX6675 sensors simultaneously
- 🎯 **Calibration System**: Built-in calibration mode for accurate readings
- 📊 **LabVIEW Integration**: CSV output format for data acquisition systems
- 🔧 **Easy Configuration**: Simple serial commands for mode switching
- 📈 **Dual Units**: Temperature display in both Celsius and Fahrenheit
- ⚡ **Error Handling**: Robust detection of sensor failures and disconnections
- 🔌 **Expandable**: Easy to add more sensors (up to 30+ possible)

## Hardware Requirements

### Components
- Arduino Mega 2560
- 1-8x MAX6675 Cold Junction Compensated K-Thermocouple to Digital Converter
- 1-8x Type K Thermocouples
- Jumper wires
- Breadboard or PCB for connections

### Wiring Diagram

```
MAX6675 Module 1:    Arduino Mega 2560:
VCC      ──────────── 5V
GND      ──────────── GND
SCK      ──────────── Pin 52 (shared with all sensors)
SO       ──────────── Pin 50 (shared with all sensors)  
CS       ──────────── Pin 53

MAX6675 Module 2:
VCC      ──────────── 5V
GND      ──────────── GND
SCK      ──────────── Pin 52 (shared)
SO       ──────────── Pin 50 (shared)
CS       ──────────── Pin 49

MAX6675 Module 3:
VCC      ──────────── 5V
GND      ──────────── GND
SCK      ──────────── Pin 52 (shared)
SO       ──────────── Pin 50 (shared)
CS       ──────────── Pin 48

MAX6675 Module 4:
VCC      ──────────── 5V
GND      ──────────── GND
SCK      ──────────── Pin 52 (shared)
SO       ──────────── Pin 50 (shared)
CS       ──────────── Pin 47
```

## Software Requirements

### Development Environment
- [PlatformIO](https://platformio.org/) (recommended)
- Or [Arduino IDE](https://www.arduino.cc/en/software)

### Libraries
- `adafruit/MAX6675 library @ ^1.1.0`
- `milesburton/DallasTemperature @ ^3.11.0` (for future expansion)
- `paulstoffregen/OneWire @ ^2.3.8` (for future expansion)

## Installation

### Using PlatformIO (Recommended)

1. **Clone the repository:**
   ```bash
   git clone <repository-url>
   cd Type-K-Temprature
   ```

2. **Open in VS Code with PlatformIO extension installed**

3. **Build and upload:**
   ```bash
   pio run --target upload
   ```

### Using Arduino IDE

1. Copy the contents of `src/main.cpp` to a new Arduino sketch
2. Install the MAX6675 library via Library Manager
3. Select "Arduino Mega 2560" as the board
4. Upload the sketch

## Usage

### Serial Monitor Setup
- **Baud Rate:** 9600
- **Port:** Check your system (typically `/dev/cu.usbmodem*` on macOS or `COM*` on Windows)

### Operating Modes

#### 1. Normal Mode (Default)
Displays human-readable temperature readings:
```
=== CALIBRATED Temperature Readings ===
Sensor 1: 23.50°C / 74.30°F
Sensor 2: 24.10°C / 75.38°F
Sensor 3: 22.75°C / 72.95°F
Sensor 4: 25.00°C / 77.00°F
Sensor 5: 23.25°C / 73.85°F
Sensor 6: 24.50°C / 76.10°F
Sensor 7: 23.00°C / 73.40°F
Sensor 8: 24.75°C / 76.55°F
```

#### 2. Calibration Mode
Enter by sending `CAL` command:
```
=== CALIBRATION READINGS (RAW) ===
Sensor 1 RAW: 2.50°C
Sensor 2 RAW: -1.25°C
Sensor 3 RAW: 0.75°C
Sensor 4 RAW: -0.50°C
Sensor 5 RAW: 1.00°C
Sensor 6 RAW: -0.75°C
Sensor 7 RAW: 0.25°C
Sensor 8 RAW: -1.00°C
Average: 0.13°C
```

#### 3. LabVIEW Mode
Enable by sending `LVON` command for CSV output:
```
23.50,74.30,24.10,75.38,22.75,72.95,25.00,77.00,23.25,73.85,24.50,76.10,23.00,73.40,24.75,76.55
23.75,74.75,24.25,75.65,23.00,73.40,25.25,77.45,23.50,74.30,24.75,76.55,23.25,73.85,25.00,77.00
```

### Serial Commands

| Command | Function | Description |
|---------|----------|-------------|
| `LVON` | Enable LabVIEW mode | CSV output format for data acquisition |
| `LABVIEW` | Same as LVON | Alternative command |
| `CSV` | Same as LVON | Alternative command |
| `LVOFF` | Disable LabVIEW mode | Return to human-readable format |
| `HUMAN` | Same as LVOFF | Alternative command |
| `CAL` | Calibration mode | Display raw readings for calibration |
| `EXIT` | Exit current mode | Return to previous operating mode |

## Calibration

### Why Calibrate?
Individual MAX6675 modules may have slight variations. Calibration ensures all sensors read the same temperature in the same environment.

### Calibration Process

1. **Prepare Reference Environment:**
   - Use ice water (0°C) for best accuracy
   - Or use boiling water (100°C at sea level)

2. **Place All Sensors:**
   - Submerge all thermocouple tips in the reference environment
   - Ensure tips don't touch container walls
   - Wait 2-3 minutes for stabilization

3. **Enter Calibration Mode:**
   ```
   Send: CAL
   ```

4. **Record Readings:**
   - Note the raw temperature readings
   - Calculate offset needed to reach reference temperature
   - Example: If sensor reads 2.5°C in ice water, offset = -2.5

5. **Update Code:**
   ```cpp
   float calibrationOffset1 = -2.5;  // Adjust based on readings
   float calibrationOffset2 = 1.2;   // Adjust based on readings
   float calibrationOffset3 = 0.8;   // Adjust based on readings
   float calibrationOffset4 = -0.3;  // Adjust based on readings
   ```

6. **Upload and Test:**
   - Upload updated code
   - Verify calibrated readings

## LabVIEW Integration

### VISA Configuration
- **Baud Rate:** 9600
- **Data Bits:** 8
- **Stop Bits:** 1
- **Parity:** None
- **Flow Control:** None

### Data Format
- **CSV Format:** `S1_C,S1_F,S2_C,S2_F,S3_C,S3_F,S4_C,S4_F,S5_C,S5_F,S6_C,S6_F,S7_C,S7_F,S8_C,S8_F`
- **Update Rate:** 1 second
- **Error Values:** -999.0 (indicates sensor error/disconnection)

### Sample LabVIEW VI Flow
1. Initialize VISA with correct COM port
2. Send "LVON\\n" command
3. Read string from serial port
4. Parse CSV data using comma delimiter
5. Convert strings to numeric values
6. Display/log temperature data

## Expanding the System

### Adding More MAX6675 Sensors
1. Connect additional sensors using available digital pins as CS:
   - Available pins: 46, 45, 44, 43, 42, etc.
   - Share SCK (Pin 52) and MISO (Pin 50)

2. Add sensor objects in code:
   ```cpp
   int thermoCS5 = 46;
   MAX6675 thermocouple5(thermoCLK, thermoCS5, thermoDO);
   ```

### Adding Other Sensor Types
- **DS18B20**: Digital temperature sensors (cheaper alternative)
- **LM35/TMP36**: Analog temperature sensors
- **BME280**: Temperature, humidity, and pressure sensors

## Troubleshooting

### Common Issues

**"Error reading thermocouple!" message:**
- Check wiring connections
- Verify thermocouple is properly connected to MAX6675
- Ensure 5V power supply is adequate

**Inconsistent readings:**
- Allow sensors to stabilize (2-3 minutes)
- Check for loose connections
- Consider calibration if readings are consistently off

**No serial output:**
- Verify correct COM port selection
- Check baud rate (9600)
- Ensure Arduino is powered and connected

**LabVIEW not receiving data:**
- Send `LVON` command first
- Verify VISA settings match Arduino configuration
- Check for proper string parsing in LabVIEW

### Performance Notes

- **Reading cycle time**: ~1.6 seconds for all 8 sensors (100ms delay between sensors)
- **Memory usage**: ~1886 bytes RAM (23% of Arduino Mega)
- **Flash usage**: ~13254 bytes (5.2% of Arduino Mega)
- **LabVIEW CSV output**: 16 values per line (8 sensors × 2 units each)

## File Structure

```
Type-K-Temprature/
├── src/
│   └── main.cpp                 # Main Arduino code
├── include/
│   └── README                   # Include directory (empty)
├── lib/
│   └── README                   # Library directory (empty)
├── test/
│   └── README                   # Test directory (empty)
├── platformio.ini               # PlatformIO configuration
├── README.md                    # This file
├── CALIBRATION_GUIDE.md         # Detailed calibration instructions
├── LABVIEW_INTEGRATION.md       # LabVIEW setup guide
├── SENSOR_EXPANSION_GUIDE.md    # Guide for adding more sensors
└── 8_SENSOR_WIRING_GUIDE.md     # Complete wiring guide for 8 sensors
```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

## License

This project is open source. Please check the LICENSE file for details.

## Support

For questions or issues:
1. Check the troubleshooting section
2. Review the detailed guides in the documentation
3. Open an issue on the repository

## Version History

- **v1.0.0**: Initial release with basic thermocouple reading
- **v1.1.0**: Added calibration system
- **v1.2.0**: Added LabVIEW integration and multiple output formats
- **v1.3.0**: Enhanced error handling and documentation

---

**Note:** This project is designed for educational and prototyping purposes. For critical applications, consider additional safety measures and professional-grade equipment.
