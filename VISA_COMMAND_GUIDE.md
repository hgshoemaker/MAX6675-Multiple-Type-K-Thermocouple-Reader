# VISA Command-Response Mode Guide

## Overview

The VISA command-response mode (VSON) transforms your MAX6675 thermocouple reader into a SCPI-compliant instrument that can be controlled using standard VISA commands. This mode is ideal for automated test equipment, LabVIEW instrument drivers, and other professional measurement applications.

## Enabling VISA Mode

### Method 1: Serial Command
Send the command: `VISA` or `VSON`

**Response:**
```
VISA command-response mode enabled
Send '*IDN?' to identify device or 'HELP?' for commands
```

### Method 2: Programmatically from LabVIEW
```
VISA Write: "VSON\n"
```

## VISA Command Set

### Standard SCPI Commands

#### Identification and System Commands
| Command | Description | Response Example |
|---------|-------------|------------------|
| `*IDN?` | Instrument identification | `MAX6675_THERMOCOUPLE_READER,v1.0,SN001` |
| `*RST` | Reset to VISA mode | `OK` |
| `SYST:ERR?` | System error query | `0,"No error"` |
| `SYST:VERS?` | System version | `1.0` |

#### Measurement Commands
| Command | Description | Response Example |
|---------|-------------|------------------|
| `MEAS:TEMP? ALL` | Read all 8 temperatures | `23.50,24.10,22.75,25.00,23.25,24.50,23.00,24.75` |
| `MEAS:TEMP? CH1` | Read channel 1 temperature | `23.50` |
| `MEAS:TEMP? CH2` | Read channel 2 temperature | `24.10` |
| ... | ... | ... |
| `MEAS:TEMP? CH8` | Read channel 8 temperature | `24.75` |

#### Configuration Commands
| Command | Description | Response Example |
|---------|-------------|------------------|
| `CONF:SENS:COUN?` | Number of sensors | `8` |
| `CONF:RATE?` | Update rate (Hz) | `1.0` |

#### Mode Control Commands
| Command | Description | Response |
|---------|-------------|----------|
| `MODE:VISA` | Enable VISA mode | `OK` |
| `MODE:LABVIEW` | Enable LabVIEW mode | `OK` |
| `MODE:HUMAN` | Enable human-readable mode | `OK` |

#### Help Command
| Command | Description | Response |
|---------|-------------|----------|
| `HELP?` or `?` | Show available commands | Command list |

## Error Handling

### Temperature Error Values
- Disconnected or faulty sensors return: `-999.0`
- NaN values are converted to `-999.0` for consistency

### Command Errors
- Unknown commands return: `ERROR: Unknown command`
- Invalid channel numbers are ignored

### System Errors
- `SYST:ERR?` always returns `0,"No error"` (simple implementation)

## LabVIEW Integration with VISA Mode

### VISA Resource String
Configure your VISA resource (typically a COM port):
- **Resource String:** `ASRL3::INSTR` (replace 3 with your COM port number)
- **Baud Rate:** 9600
- **Data Bits:** 8
- **Stop Bits:** 1
- **Parity:** None
- **Flow Control:** None

### Sample LabVIEW Code Structure

#### 1. Initialize VISA Session
```
VISA Open → Configure Serial Port → Set Termination Character (\n)
```

#### 2. Enable VISA Mode
```
VISA Write: "VSON\n"
```

#### 3. Identify Instrument
```
VISA Write: "*IDN?\n"
VISA Read: "MAX6675_THERMOCOUPLE_READER,v1.0,SN001"
```

#### 4. Read All Temperatures
```
VISA Write: "MEAS:TEMP? ALL\n"
VISA Read: "23.50,24.10,22.75,25.00,23.25,24.50,23.00,24.75"
```

#### 5. Parse Data
```
String Split (delimiter: ",") → String to Number Array
```

#### 6. Read Individual Channel
```
VISA Write: "MEAS:TEMP? CH1\n"
VISA Read: "23.50"
```

### Error Handling in LabVIEW
- Check for `-999.0` values to detect sensor errors
- Implement timeout handling for VISA operations
- Use error clusters to handle communication errors

## Command Timing

### Response Time
- Commands are processed immediately upon receipt
- Temperature readings are current (not cached)
- No artificial delays in VISA mode

### Command Format
- Commands are case-insensitive
- Commands must end with `\n` (newline)
- Responses end with `\n` (newline)

## Differences from LabVIEW Mode

| Feature | LabVIEW Mode | VISA Mode |
|---------|--------------|-----------|
| Operation | Continuous streaming | Command-response |
| Data Format | CSV stream | Query-based responses |
| Control | Limited commands | Full SCPI command set |
| Integration | Simple parsing | VISA instrument driver |
| Timing | Fixed 1Hz updates | On-demand measurements |

## Example Command Sequence

### Manual Testing (Serial Monitor)
```
> VSON
VISA command-response mode enabled
Send '*IDN?' to identify device or 'HELP?' for commands

> *IDN?
MAX6675_THERMOCOUPLE_READER,v1.0,SN001

> MEAS:TEMP? ALL
23.50,24.10,22.75,25.00,23.25,24.50,23.00,24.75

> MEAS:TEMP? CH1
23.50

> CONF:SENS:COUN?
8

> MODE:HUMAN
OK
```

### Programmatic Usage (Python/LabVIEW)
```python
import pyvisa

# Open VISA resource
rm = pyvisa.ResourceManager()
device = rm.open_resource('ASRL3::INSTR')

# Configure serial settings
device.baud_rate = 9600
device.read_termination = '\n'
device.write_termination = '\n'

# Enable VISA mode
device.write('VSON')

# Identify device
response = device.query('*IDN?')
print(f"Device: {response}")

# Read all temperatures
temperatures = device.query('MEAS:TEMP? ALL')
temp_array = [float(x) for x in temperatures.split(',')]
print(f"Temperatures: {temp_array}")

# Read individual channel
temp1 = float(device.query('MEAS:TEMP? CH1'))
print(f"Channel 1: {temp1}°C")

device.close()
```

## Best Practices

### Command Timing
- Allow 100ms between commands for reliable operation
- Use appropriate timeouts for VISA operations

### Error Handling
- Always check for `-999.0` error values
- Implement retry logic for critical measurements
- Monitor system errors with `SYST:ERR?`

### Integration
- Use VISA mode for professional instrumentation
- Use LabVIEW mode for simple data logging
- Use Human mode for debugging and setup

## Troubleshooting

### Common Issues

**Device Not Responding:**
- Ensure VISA mode is enabled with `VSON` command
- Check serial port configuration
- Verify termination characters (`\n`)

**Invalid Responses:**
- Check command syntax (case-insensitive)
- Ensure commands end with newline
- Use `HELP?` to verify available commands

**Sensor Error Values (-999.0):**
- Check thermocouple connections
- Verify MAX6675 wiring
- Allow sensors to stabilize

### Debug Commands
- `*IDN?` - Verify communication
- `HELP?` - List available commands
- `SYST:ERR?` - Check for system errors
- `CONF:SENS:COUN?` - Verify sensor count
