# MAX6675 Multiple Type K Thermocouple Reader

A comprehensive Arduino Mega 2560 project for reading multiple Type K thermocouples using MAX6675 amplifier modules, with calibration features and LabVIEW integration.

## Features

- 🌡️ **Multiple Sensor Support**: Read up to 8 MAX6675 sensors simultaneously
- 🔌 **ADS1115 ADC Integration**: 4-channel 16-bit ADC for precision voltage measurements
- �️ **Relay Control**: 8-channel relay control system for external device automation
- �🎯 **Calibration System**: Built-in calibration mode for accurate readings
- 📊 **LabVIEW Integration**: CSV output format for data acquisition systems
- 🔧 **VISA Command Mode**: SCPI-compliant instrument interface for professional use
- 🔧 **Easy Configuration**: Simple serial commands for mode switching
- 📈 **Combined Data**: Temperature and voltage readings in unified format
- ⚡ **Error Handling**: Robust detection of sensor failures and disconnections
- 🔌 **Expandable**: Easy to add more sensors (up to 30+ possible)

## Hardware Requirements

### Components
- Arduino Mega 2560
- 1-8x MAX6675 Cold Junction Compensated K-Thermocouple to Digital Converter
- 1-8x Type K Thermocouples
- 1x ADS1115 16-bit ADC Module (I2C)
- 1-8x Relay modules (optional, for automation control)
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

MAX6675 Module 5:
VCC      ──────────── 5V
GND      ──────────── GND
SCK      ──────────── Pin 52 (shared)
SO       ──────────── Pin 50 (shared)
CS       ──────────── Pin 46

MAX6675 Module 6:
VCC      ──────────── 5V
GND      ──────────── GND
SCK      ──────────── Pin 52 (shared)
SO       ──────────── Pin 50 (shared)
CS       ──────────── Pin 45

MAX6675 Module 7:
VCC      ──────────── 5V
GND      ──────────── GND
SCK      ──────────── Pin 52 (shared)
SO       ──────────── Pin 50 (shared)
CS       ──────────── Pin 44

MAX6675 Module 8:
VCC      ──────────── 5V
GND      ──────────── GND
SCK      ──────────── Pin 52 (shared)
SO       ──────────── Pin 50 (shared)
CS       ──────────── Pin 43

ADS1115 ADC Module:
VDD      ──────────── 5V  
GND      ──────────── GND
SDA      ──────────── Pin 20 (SDA)
SCL      ──────────── Pin 21 (SCL)
A0       ──────────── Analog input 1 (0-6.144V max)
A1       ──────────── Analog input 2 (0-6.144V max)
A2       ──────────── Analog input 3 (0-6.144V max)  
A3       ──────────── Analog input 4 (0-6.144V max)

Relay Modules (Optional):
Relay 1  ──────────── Pin D2
Relay 2  ──────────── Pin D3
Relay 3  ──────────── Pin D4
Relay 4  ──────────── Pin D5
Relay 5  ──────────── Pin D6
Relay 6  ──────────── Pin D7
Relay 7  ──────────── Pin D8
Relay 8  ──────────── Pin D9
```

## Software Requirements

### Development Environment
- [PlatformIO](https://platformio.org/) (recommended)
- Or [Arduino IDE](https://www.arduino.cc/en/software)

### Libraries
- `adafruit/MAX6675 library @ ^1.1.0`
- `adafruit/Adafruit ADS1X15 @ ^2.5.0`
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
Sensor 1: 23.50°C
Sensor 2: 24.10°C
Sensor 3: 22.75°C
Sensor 4: 25.00°C
Sensor 5: 23.25°C
Sensor 6: 24.50°C
Sensor 7: 23.00°C
Sensor 8: 24.75°C
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
23.50,24.10,22.75,25.00,23.25,24.50,23.00,24.75
23.75,24.25,23.00,25.25,23.50,24.75,23.25,25.00
```

### Serial Commands

| Command | Function | Description |
|---------|----------|-------------|
| `LVON` | Enable LabVIEW mode | CSV output format for data acquisition |
| `LABVIEW` | Same as LVON | Alternative command |
| `CSV` | Same as LVON | Alternative command |
| `LVOFF` | Disable LabVIEW mode | Return to human-readable format |
| `HUMAN` | Same as LVOFF | Alternative command |
| `VISA` | Enable VISA mode | SCPI command-response for instruments |
| `VSON` | Same as VISA | Alternative command |
| `CAL` | Calibration mode | Display raw readings for calibration |
| `EXIT` | Exit current mode | Return to previous operating mode |

### Operation Modes

#### 1. Human-Readable Mode (Default)
- Displays temperature readings in a user-friendly format
- Updates every 2 seconds
- Includes sensor labels and units

#### 2. LabVIEW Mode
- Outputs comma-separated values (CSV) format
- Updates every 1 second
- Optimized for data acquisition systems
- Error values shown as `-999.0`

#### 3. VISA Command-Response Mode (NEW)
- SCPI-compliant instrument behavior
- Query-based temperature readings
- Full command set for automated testing
- Professional instrumentation compatibility
- See [VISA Command Guide](VISA_COMMAND_GUIDE.md) for details

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
- **CSV Format:** `S1_C,S2_C,S3_C,S4_C,S5_C,S6_C,S7_C,S8_C,V1,V2,V3,V4`
  - First 8 values: Temperature readings in °C (2 decimal places)
  - Last 4 values: Voltage readings in V (4 decimal places)
- **Update Rate:** 5 seconds (default for CSV mode)
- **Error Values:** -999.00 (temperature) or -999.0000 (voltage) indicates sensor error

### Sample LabVIEW VI Flow
1. Initialize VISA with correct COM port
2. Send "LVON\\n" command  
3. Read string from serial port
4. Parse CSV data using comma delimiter
5. Split data: indices 0-7 = temperatures, indices 8-11 = voltages
6. Convert strings to numeric values
7. Display/log temperature and voltage data

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
- **LabVIEW CSV output**: 8 values per line (8 sensors in Celsius)

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

## Documentation

### Detailed Guides
- **[ADS1115 Integration Guide](ADS1115_INTEGRATION_GUIDE.md)** - Complete setup and usage for voltage measurements
- **[VISA Command Guide](VISA_COMMAND_GUIDE.md)** - Professional instrument interface documentation  
- **[Relay Control Guide](RELAY_CONTROL_GUIDE.md)** - Hardware automation and control systems
- **[Calibration Guide](CALIBRATION_GUIDE.md)** - Precision measurement calibration procedures
- **[Sensor Expansion Guide](SENSOR_EXPANSION_GUIDE.md)** - Adding more thermocouples
- **[LabVIEW Integration](LABVIEW_INTEGRATION.md)** - Data acquisition system integration

## Version History

- **v1.0.0**: Initial release with basic thermocouple reading
- **v1.1.0**: Added calibration system
- **v1.2.0**: Added LabVIEW integration and multiple output formats
- **v2.1.0**: Updated to Celsius-only output for cleaner data format
- **v3.0.0**: Added ADS1115 ADC integration and relay control system

---

**Note:** This project is designed for educational and prototyping purposes. For critical applications, consider additional safety measures and professional-grade equipment.
