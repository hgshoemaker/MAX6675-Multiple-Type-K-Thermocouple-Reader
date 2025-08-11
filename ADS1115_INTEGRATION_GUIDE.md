# ADS1115 ADC Integration Guide

## Overview

The MAX6675 thermocouple reader includes ADS1115 16-bit ADC functionality for reading analog voltages on 2 channels (A0-A1). This provides high-precision voltage measurements alongside the thermocouple temperature readings.

## Hardware Setup

### ADS1115 Module Connections
```
Arduino Mega 2560    ADS1115 Module
SDA (Pin 20)   ────── SDA
SCL (Pin 21)   ────── SCL
5V             ────── VDD
GND            ────── GND
               
Analog Inputs:
A0             ────── Your analog signal 1 (0-6.144V max)
A1             ────── Your analog signal 2 (0-6.144V max)
A2             ────── Not used
A3             ────── Not used
```

### I2C Address Configuration
- **Default Address**: 0x48 (ADDR pin to GND)
- **Alternative Addresses**: 
  - 0x49 (ADDR pin to VDD)
  - 0x4A (ADDR pin to SDA)
  - 0x4B (ADDR pin to SCL)

### Voltage Range Configuration
The ADS1115 supports different voltage ranges by setting the gain:

| Gain Setting | Voltage Range | Resolution |
|--------------|---------------|------------|
| GAIN_TWOTHIRDS | ±6.144V | 0.1875mV |
| GAIN_ONE | ±4.096V | 0.125mV |
| GAIN_TWO | ±2.048V | 0.0625mV |
| GAIN_FOUR | ±1.024V | 0.03125mV |
| GAIN_EIGHT | ±0.512V | 0.015625mV |
| GAIN_SIXTEEN | ±0.256V | 0.0078125mV |

**Default**: GAIN_TWOTHIRDS (±6.144V range) for maximum flexibility.

## Data Output Formats

### CSV Mode (Default)
Combined temperature and voltage data:
```
Format: S1_C,S2_C,S3_C,S4_C,S5_C,S6_C,S7_C,S8_C,V1,V2,V3,V4
Example: 25.50,26.25,24.75,25.00,26.50,25.25,24.50,25.75,3.3000,1.2500,2.4750,0.5000
```
- Temperatures: 2 decimal places (°C)
- Voltages: 4 decimal places (V)
- Error values: -999.00 (temp) or -999.0000 (voltage)

### JSON Mode
```json
{
  "sensor1":{"celsius":25.50},
  "sensor2":{"celsius":26.25},
  "sensor3":{"celsius":24.75},
  "sensor4":{"celsius":25.00},
  "sensor5":{"celsius":26.50},
  "sensor6":{"celsius":25.25},
  "sensor7":{"celsius":24.50},
  "sensor8":{"celsius":25.75},
  "voltage1":{"volts":3.3000},
  "voltage2":{"volts":1.2500},
  "voltage3":{"volts":2.4750},
  "voltage4":{"volts":0.5000}
}
```

### Human-Readable Mode
```
=== CALIBRATED Temperature Readings ===
Sensor 1: 25.50°C
Sensor 2: 26.25°C
...
Sensor 8: 25.75°C

=== VOLTAGE Readings (ADS1115 ADC) ===
Channel A0: 3.3000V
Channel A1: 1.2500V
Channel A2: 2.4750V
Channel A3: 0.5000V
```

## VISA Commands

### Voltage Measurement Commands
| Command | Description | Response Example |
|---------|-------------|------------------|
| `MEAS:VOLT? ALL` | Read all 4 voltage channels | `3.3000,1.2500,2.4750,0.5000` |
| `MEAS:VOLT? CH1` | Read voltage channel 1 (A0) | `3.3000` |
| `MEAS:VOLT? CH2` | Read voltage channel 2 (A1) | `1.2500` |
| `MEAS:VOLT? CH3` | Read voltage channel 3 (A2) | `2.4750` |
| `MEAS:VOLT? CH4` | Read voltage channel 4 (A3) | `0.5000` |
| `MEAS:ALL?` | Read all temperatures and voltages | `25.50,26.25,...,3.3000,1.2500,2.4750,0.5000` |

### Configuration Commands
| Command | Description | Response |
|---------|-------------|----------|
| `CONF:VOLT:COUN?` | Number of voltage channels | `4` |
| `CONF:SENS:COUN?` | Number of temperature sensors | `8` |

## Programming Examples

### Python with VISA Mode
```python
import serial
import time

# Open serial connection
ser = serial.Serial('COM3', 9600, timeout=2)
time.sleep(2)

# Enable VISA mode
ser.write(b"VSON\n")
time.sleep(1)

# Read all voltages
ser.write(b"MEAS:VOLT? ALL\n")
voltages = ser.readline().decode().strip().split(',')
for i, voltage in enumerate(voltages, 1):
    print(f"Channel A{i-1}: {voltage}V")

# Read specific voltage channel
ser.write(b"MEAS:VOLT? CH1\n")
voltage_ch1 = ser.readline().decode().strip()
print(f"Channel A0: {voltage_ch1}V")

# Read all data (temperatures + voltages)
ser.write(b"MEAS:ALL?\n")
all_data = ser.readline().decode().strip().split(',')
temps = all_data[:8]   # First 8 values are temperatures
volts = all_data[8:]   # Last 4 values are voltages

print("Temperatures:", temps)
print("Voltages:", volts)

ser.close()
```

### LabVIEW Integration
1. **Configure VISA Serial**: 9600 baud, 8N1
2. **Enter VISA Mode**: Send `"VSON\n"`
3. **Read All Data**: Send `"MEAS:ALL?\n"`
4. **Parse CSV Response**: Split by comma, first 8 = temps, last 4 = voltages
5. **Individual Channels**: Use `"MEAS:VOLT? CH1\n"` through `"MEAS:VOLT? CH4\n"`

### Arduino Code Integration
```cpp
// Read all voltage channels
float voltages[4];
for (int i = 0; i < 4; i++) {
    voltages[i] = readCalibratedVoltage(i, voltageOffsets[i]);
    Serial.print("A"); Serial.print(i); 
    Serial.print(": "); Serial.println(voltages[i], 4);
}
```

## Calibration

### Voltage Calibration Offsets
Adjust these values in the code to calibrate each channel:
```cpp
float voltageOffset1 = 0.0;  // Channel A0 offset in V
float voltageOffset2 = 0.0;  // Channel A1 offset in V  
float voltageOffset3 = 0.0;  // Channel A2 offset in V
float voltageOffset4 = 0.0;  // Channel A3 offset in V
```

### Calibration Procedure
1. **Connect Known Voltage**: Apply a precise reference voltage (e.g., 3.300V)
2. **Read Raw Value**: Use VISA command `MEAS:VOLT? CH1`
3. **Calculate Offset**: `offset = true_voltage - measured_voltage`
4. **Update Code**: Set the appropriate `voltageOffsetN` value
5. **Verify**: Re-measure to confirm accuracy

## Applications

### Common Use Cases
- **Power Supply Monitoring**: Monitor multiple power rails
- **Sensor Integration**: Read analog sensors (pressure, flow, etc.)
- **Battery Monitoring**: Track battery voltages
- **Signal Conditioning**: Monitor conditioned analog signals
- **Process Control**: Read process variables alongside temperature

### Integration with Temperature Control
```python
# Example: Control heater based on temperature and power supply voltage
temp = float(ser.readline().decode().strip().split(',')[0])  # First temperature
voltage = float(ser.readline().decode().strip().split(',')[8])  # First voltage

if temp < 50.0 and voltage > 11.5:  # Temperature low and power supply OK
    ser.write(b"RELAY:SET R1,ON\n")  # Turn on heater
elif temp > 55.0:  # Temperature reached
    ser.write(b"RELAY:SET R1,OFF\n")  # Turn off heater
```

## Troubleshooting

### Common Issues

**ADS1115 not detected:**
- Check I2C wiring (SDA/SCL connections)
- Verify power supply connections (VDD/GND)
- Check I2C address (default 0x48)
- Use I2C scanner to detect device

**Incorrect voltage readings:**
- Verify input voltage range vs. gain setting
- Check for loose connections
- Verify reference voltage accuracy
- Apply calibration offsets

**Noisy readings:**
- Add bypass capacitors near ADS1115
- Use shielded cables for analog inputs
- Keep analog wires away from digital signals
- Consider using differential inputs for better noise immunity

### Debug Commands
- `MEAS:VOLT? ALL` - Check all voltage channels
- `CONF:VOLT:COUN?` - Verify 4 channels detected
- `*IDN?` - Confirm device communication

## Hardware Specifications

### ADS1115 Key Features
- **Resolution**: 16-bit (65,536 levels)
- **Sample Rate**: Up to 860 SPS
- **Input Channels**: 4 single-ended or 2 differential
- **Interface**: I2C (up to 400kHz)
- **Supply Voltage**: 2.0V to 5.5V
- **Input Voltage**: ±6.144V maximum (with appropriate gain)

### Performance Characteristics
- **Integral Nonlinearity**: ±0.125 LSB
- **Offset Error**: ±3 LSB
- **Gain Error**: ±0.2%
- **Temperature Coefficient**: ±0.3 ppm/°C

The ADS1115 integration provides high-precision voltage measurements that complement the thermocouple temperature readings, enabling comprehensive monitoring and control applications.
