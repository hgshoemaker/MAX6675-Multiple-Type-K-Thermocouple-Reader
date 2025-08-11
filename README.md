# MAX6675 Multiple Type K Thermocouple Reader

A simplified Arduino Mega 2560 project for reading multiple Type K thermocouples using MAX6675 amplifier modules, with calibration features and CSV data output.

## Features

- 🌡️ **Multiple Sensor Support**: Read up to 8 MAX6675 sensors simultaneously
- 🔌 **ADS1115 ADC Integration**: 2-channel 16-bit ADC for precision voltage measurements (A0, A1)
- 🎯 **Calibration System**: Built-in calibration mode for accurate readings
- 📊 **CSV Output**: Clean CSV format for data acquisition systems
- 🔧 **Simple Configuration**: Minimal serial commands for operation
- 📈 **Combined Data**: Temperature and voltage readings in unified CSV format
- ⚡ **Error Handling**: Robust detection of sensor failures and disconnections
- 🔌 **Expandable**: Easy to add more sensors (up to 30+ possible)

## Hardware Requirements

### Components
- Arduino Mega 2560
- 1-8x MAX6675 Cold Junction Compensated K-Thermocouple to Digital Converter
- 1-8x Type K Thermocouples
- 1x ADS1115 16-bit ADC Module (I2C) - using channels A0 and A1 only
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
A2       ──────────── Not used
A3       ──────────── Not used
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

#### 2. CSV Data Stream Mode
Continuous CSV output for data acquisition:
```
23.50,24.10,22.75,25.00,23.25,24.50,23.00,24.75,-0.0012,0.0034
23.75,24.25,23.00,25.25,23.50,24.75,23.25,25.00,-0.0015,0.0031
```
Format: `S1,S2,S3,S4,S5,S6,S7,S8,V1,V2`

### Serial Commands

| Command | Function | Description |
|---------|----------|-------------|
| `CAL` | Calibration mode | Display raw readings for calibration |
| `DEBUG` | Test individual sensors | Display each sensor reading separately |
| `EXIT` | Exit calibration mode | Return to CSV output mode |

### Operation Modes

#### 1. CSV Output Mode (Default)
- Outputs comma-separated values (CSV) format
- Updates every 5 seconds
- Format: `S1_C,S2_C,S3_C,S4_C,S5_C,S6_C,S7_C,S8_C,V1,V2`
- 8 temperature readings + 2 voltage readings
- Error values shown as `-999.00` for temperature, `-999.0000` for voltage

#### 2. Calibration Mode
- Displays raw (uncalibrated) temperature readings
- Shows average and differences from average
- Updates every 3 seconds
- Used for determining calibration offsets

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

## Data Acquisition Integration

### CSV Data Format
The device outputs data in CSV format suitable for most data acquisition systems:

```
Format: S1_C,S2_C,S3_C,S4_C,S5_C,S6_C,S7_C,S8_C,V1,V2
Units:  °C,  °C,  °C,  °C,  °C,  °C,  °C,  °C,  V, V
```

Example output:
```
23.50,24.10,22.75,25.00,23.25,24.50,23.00,24.75,0.1234,0.5678
23.75,24.25,23.00,25.25,23.50,24.75,23.25,25.00,0.1245,0.5689
```

### Error Handling
- Disconnected thermocouples: `-999.00`
- ADS1115 connection issues: `-999.0000`
- Update rate: Every 5 seconds
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

**No CSV output:**
- Device is in calibration mode - send `EXIT` command
- Check serial connection and baud rate (9600)
- Verify Arduino is powered and connected

**ADS1115 voltage readings show -999.0000:**
- Check I2C wiring (SDA=Pin20, SCL=Pin21)
- Verify ADS1115 power connections (VDD=5V, GND=GND)
- Ensure ADS1115 I2C address is 0x48

### Performance Notes

- **Reading cycle time**: ~1.2 seconds for all 8 sensors (150ms delay between sensors)
- **CSV output rate**: Every 5 seconds
- **Voltage channels**: 2 channels (A0, A1) on ADS1115
- **Temperature precision**: ±0.25°C (with proper calibration)
- **Voltage precision**: 16-bit ADC with 0.1875mV resolution

## File Structure

```
MAX6675-Multiple-Type-K-Thermocouple-Reader/
├── src/
│   └── main.cpp                 # Main Arduino code (simplified)
├── include/
│   └── README                   # Include directory (empty)
├── lib/
│   └── README                   # Library directory (empty)
├── test/
│   └── README                   # Test directory (empty)
├── examples/
│   └── README.md                # Example usage
├── platformio.ini               # PlatformIO configuration
├── README.md                    # This file
├── CALIBRATION_GUIDE.md         # Detailed calibration instructions
├── DATA_ACQUISITION_GUIDE.md    # Data acquisition integration guide
├── ADS1115_INTEGRATION_GUIDE.md # ADS1115 voltage measurement guide
├── CSV_TROUBLESHOOTING.md       # Troubleshooting CSV output issues
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
