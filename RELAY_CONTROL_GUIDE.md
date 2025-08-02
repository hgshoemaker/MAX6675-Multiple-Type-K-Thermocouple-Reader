# Relay Control Guide

## Overview

The MAX6675 thermocouple reader now includes 8-relay control functionality on digital pins D2 through D9. This allows you to control external devices like heaters, fans, pumps, or other equipment based on temperature readings or manual commands.

## Hardware Setup

### Relay Module Connections
```
Arduino Mega 2560    Relay Module
D2            ────── Relay 1 Control
D3            ────── Relay 2 Control  
D4            ────── Relay 3 Control
D5            ────── Relay 4 Control
D6            ────── Relay 5 Control
D7            ────── Relay 6 Control
D8            ────── Relay 7 Control
D9            ────── Relay 8 Control
5V            ────── VCC (if needed)
GND           ────── GND
```

### Relay Module Types
- **Active LOW relays**: Relay turns ON when pin is LOW (most common)
- **Active HIGH relays**: Relay turns ON when pin is HIGH
- The code currently uses HIGH = ON, LOW = OFF (modify if needed)

## Command Interface

### Simple Commands (All Modes)
| Command | Description | Example |
|---------|-------------|---------|
| `RELAYON1` | Turn relay 1 ON | `RELAYON1` |
| `RELAYOFF1` | Turn relay 1 OFF | `RELAYOFF1` |
| `RELAYON` | Turn all relays ON | `RELAYON` |
| `RELAYOFF` | Turn all relays OFF | `RELAYOFF` |
| `RELAYSTATUS` | Show relay status | `RELAYSTATUS` |

### VISA Commands (VISA Mode Only)
| Command | Description | Response Example |
|---------|-------------|------------------|
| `RELAY:SET R1,ON` | Turn relay 1 ON | `OK` |
| `RELAY:SET R1,OFF` | Turn relay 1 OFF | `OK` |
| `RELAY:SET ALL,ON` | Turn all relays ON | `OK` |
| `RELAY:SET ALL,OFF` | Turn all relays OFF | `OK` |
| `RELAY:GET? R1` | Get relay 1 state | `1` (ON) or `0` (OFF) |
| `RELAY:GET? ALL` | Get all relay states | `1,0,1,0,1,0,1,0` |
| `RELAY:COUNT?` | Number of relays | `8` |

## Usage Examples

### Manual Control
```
> RELAYON1
Relay 1 turned ON

> RELAYOFF1  
Relay 1 turned OFF

> RELAYON
All relays turned ON

> RELAYSTATUS
=== RELAY STATUS ===
Relay 1 (D2): ON
Relay 2 (D3): OFF
Relay 3 (D4): ON
Relay 4 (D5): OFF
Relay 5 (D6): ON
Relay 6 (D7): OFF  
Relay 7 (D8): ON
Relay 8 (D9): OFF
====================
```

### VISA Mode Control
```
> RELAY:SET R1,ON
OK

> RELAY:GET? R1
1

> RELAY:SET ALL,OFF
OK

> RELAY:GET? ALL
0,0,0,0,0,0,0,0
```

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

# Turn relay 1 ON
ser.write(b"RELAY:SET R1,ON\n")
response = ser.readline().decode().strip()
print(f"Relay 1 ON: {response}")

# Check relay status
ser.write(b"RELAY:GET? R1\n")
status = ser.readline().decode().strip()
print(f"Relay 1 Status: {'ON' if status == '1' else 'OFF'}")

# Get all relay states
ser.write(b"RELAY:GET? ALL\n")
all_states = ser.readline().decode().strip()
states = all_states.split(',')
for i, state in enumerate(states, 1):
    print(f"Relay {i}: {'ON' if state == '1' else 'OFF'}")

ser.close()
```

### LabVIEW Integration
1. **VISA Write**: `"RELAY:SET R1,ON\n"`
2. **VISA Read**: `"OK"`
3. **VISA Write**: `"RELAY:GET? ALL\n"`
4. **VISA Read**: `"1,0,1,0,1,0,1,0"`
5. Parse the comma-separated values for relay states

## Temperature-Based Control

### Example Use Cases
- **Heater Control**: Turn on heater when temperature < setpoint
- **Fan Control**: Turn on cooling fan when temperature > setpoint  
- **Multiple Zone Control**: Control different heaters/coolers for different temperature zones
- **Safety Shutoff**: Turn off equipment when temperature exceeds safety limits

### Implementation Strategy
1. Read temperatures using existing commands
2. Implement control logic in your application
3. Send relay commands based on temperature conditions
4. Use CSV mode for continuous monitoring with external control logic

## Safety Considerations

### Important Notes
- **Power Ratings**: Ensure relays can handle your load current/voltage
- **Isolation**: Use optically isolated relays for high-voltage applications
- **Fusing**: Always fuse your relay circuits appropriately
- **Emergency Stop**: Implement emergency stop functionality
- **Default State**: All relays start in OFF state on power-up

### Best Practices
- Test relay operation at low voltage first
- Use appropriate relay types (AC/DC, current rating)
- Consider using solid-state relays for frequent switching
- Add status LEDs to show relay states visually
- Implement watchdog timers for safety-critical applications

## Troubleshooting

### Common Issues

**Relay not switching:**
- Check wiring connections
- Verify relay module power supply
- Test with multimeter on relay pins
- Check if relay is active HIGH or LOW

**Inconsistent operation:**
- Check for loose connections
- Verify power supply current capacity
- Look for electrical noise/interference
- Check relay coil voltage requirements

**Commands not working:**
- Ensure correct command format
- Check for proper line endings (\n)
- Verify you're in the correct mode (VISA vs simple)
- Use `HELP?` to see available commands

### Debug Commands
- `RELAYSTATUS` - Show current relay states
- `RELAY:GET? ALL` - Get all relay states (VISA mode)
- `RELAY:COUNT?` - Verify relay count (VISA mode)

## Advanced Features

### Potential Enhancements
1. **Pulse Control**: Timed relay activation
2. **Temperature Thresholds**: Automatic relay control based on temperature
3. **Scheduling**: Time-based relay control
4. **Interlocks**: Prevent certain relay combinations
5. **Logging**: Record relay state changes with timestamps

The relay control system provides a solid foundation for building temperature control systems, automated test equipment, and process control applications.
