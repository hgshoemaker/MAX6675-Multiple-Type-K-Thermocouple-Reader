# SSR Control Guide

## Overview

The MAX6675 thermocouple reader now includes 3 Solid State Relay (SSR) outputs on digital pins D10, D11, and D12. These SSRs have built-in safety interlocking to prevent all three from being on simultaneously, which could cause system overload or safety hazards.

## Hardware Setup

### SSR Module Connections
```
Arduino Mega 2560    SSR Module
D10           ────── SSR 1 Control Input
D11           ────── SSR 2 Control Input  
D12           ────── SSR 3 Control Input
5V            ────── VCC (if needed)
GND           ────── GND
```

### SSR Types
Most SSRs are **Active HIGH** (turn ON when input is HIGH):
- Input HIGH (5V) = SSR ON
- Input LOW (0V) = SSR OFF

The code assumes Active HIGH SSRs. If you have Active LOW SSRs, modify the digitalWrite statements accordingly.

## Safety Features

### **CRITICAL SAFETY RULE: Never All On**
The system prevents all three SSRs from being on simultaneously through:
- **Hardware Interlocking**: Software prevents unsafe combinations
- **Safety Checks**: Every ON command is validated before execution
- **Error Messages**: Clear feedback when safety violations are attempted

### Safety Logic
- ✅ **1 SSR ON**: Allowed
- ✅ **2 SSRs ON**: Allowed  
- ❌ **3 SSRs ON**: **BLOCKED** - Safety violation

## Command Interface

### Simple Commands (All Modes)
| Command | Description | Example | Safety Check |
|---------|-------------|---------|--------------|
| `SSRON1` | Turn SSR 1 ON | `SSRON1` | ✅ Validated |
| `SSROFF1` | Turn SSR 1 OFF | `SSROFF1` | ✅ Always safe |
| `SSRON2` | Turn SSR 2 ON | `SSRON2` | ✅ Validated |
| `SSROFF2` | Turn SSR 2 OFF | `SSROFF2` | ✅ Always safe |
| `SSRON3` | Turn SSR 3 ON | `SSRON3` | ✅ Validated |
| `SSROFF3` | Turn SSR 3 OFF | `SSROFF3` | ✅ Always safe |
| `SSROFF` | Turn all SSRs OFF | `SSROFF` | ✅ Always safe |
| `SSRSTATUS` | Show SSR status | `SSRSTATUS` | ℹ️ Info only |

⚠️ **Note**: There is NO `SSRON` (all on) command for safety reasons.

### VISA Commands (VISA Mode Only)
| Command | Description | Response | Safety |
|---------|-------------|----------|--------|
| `SSR:SET S1,ON` | Turn SSR 1 ON | `OK` or `ERROR` | ✅ Validated |
| `SSR:SET S1,OFF` | Turn SSR 1 OFF | `OK` | ✅ Always safe |
| `SSR:SET S2,ON` | Turn SSR 2 ON | `OK` or `ERROR` | ✅ Validated |
| `SSR:SET S2,OFF` | Turn SSR 2 OFF | `OK` | ✅ Always safe |
| `SSR:SET S3,ON` | Turn SSR 3 ON | `OK` or `ERROR` | ✅ Validated |
| `SSR:SET S3,OFF` | Turn SSR 3 OFF | `OK` | ✅ Always safe |
| `SSR:SET ALL,OFF` | Turn all SSRs OFF | `OK` | ✅ Always safe |
| `SSR:GET? S1` | Get SSR 1 state | `1` (ON) or `0` (OFF) | ℹ️ Info only |
| `SSR:GET? ALL` | Get all SSR states | `1,0,1` | ℹ️ Info only |
| `SSR:COUNT?` | Number of SSRs | `3` | ℹ️ Info only |
| `SSR:SAFE?` | Check safety status | `1` (safe) or `0` (unsafe) | ℹ️ Info only |

⚠️ **Note**: `SSR:SET ALL,ON` is **NOT SUPPORTED** and will return an error.

## Usage Examples

### Manual Control
```
> SSRON1
SSR 1 turned ON

> SSRON2  
SSR 2 turned ON

> SSRON3
ERROR: Safety violation - would result in all SSRs ON

> SSROFF1
SSR 1 turned OFF

> SSRON3
SSR 3 turned ON

> SSRSTATUS
=== SSR STATUS ===
SSR 1 (D10): OFF
SSR 2 (D11): ON
SSR 3 (D12): ON
Active SSRs: 2/3 (Safety: Never all on)
==================
```

### VISA Mode Control
```
> SSR:SET S1,ON
OK

> SSR:SET S2,ON
OK

> SSR:SET S3,ON
ERROR: Safety violation - would result in all SSRs ON

> SSR:GET? ALL
1,1,0

> SSR:SAFE?
1

> SSR:SET S1,OFF
OK

> SSR:SET S3,ON
OK

> SSR:GET? ALL
0,1,1
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

def set_ssr(ssr_num, state):
    """Safely set SSR state"""
    cmd = f"SSR:SET S{ssr_num},{'ON' if state else 'OFF'}\n"
    ser.write(cmd.encode())
    response = ser.readline().decode().strip()
    if response == "OK":
        print(f"SSR {ssr_num} {'ON' if state else 'OFF'}")
        return True
    else:
        print(f"Failed: {response}")
        return False

def get_ssr_states():
    """Get all SSR states"""
    ser.write(b"SSR:GET? ALL\n")
    states = ser.readline().decode().strip().split(',')
    return [s == '1' for s in states]

def check_safety():
    """Check if system is in safe state"""
    ser.write(b"SSR:SAFE?\n")
    safe = ser.readline().decode().strip() == '1'
    return safe

# Example usage
set_ssr(1, True)   # Turn SSR 1 ON
set_ssr(2, True)   # Turn SSR 2 ON
set_ssr(3, True)   # This will fail due to safety

states = get_ssr_states()
print(f"SSR States: {states}")
print(f"Safe: {check_safety()}")

ser.close()
```

### Temperature-Based SSR Control
```python
# Example: Control heaters based on temperature zones
def control_heaters(temps, setpoints):
    """Control SSRs based on temperature readings"""
    for i, (temp, setpoint) in enumerate(zip(temps[:3], setpoints)):
        ssr_num = i + 1
        
        if temp < setpoint - 2.0:  # Need heating
            if set_ssr(ssr_num, True):
                print(f"Zone {ssr_num}: Heating ON (Temp: {temp}°C)")
            else:
                print(f"Zone {ssr_num}: Cannot heat - safety limit")
                
        elif temp > setpoint + 1.0:  # Too hot
            set_ssr(ssr_num, False)
            print(f"Zone {ssr_num}: Heating OFF (Temp: {temp}°C)")

# Usage with temperature readings
temperatures = [45.5, 52.3, 48.7, 50.1, 49.8, 51.2, 47.9, 46.4]
setpoints = [50.0, 55.0, 50.0]  # Target temperatures for zones 1-3
control_heaters(temperatures, setpoints)
```

## Applications

### Common Use Cases
- **Multi-Zone Heating**: Control heaters for different temperature zones
- **Load Management**: Prevent electrical overload by limiting simultaneous loads
- **Process Control**: Sequential operation of equipment
- **Safety Systems**: Emergency shutdown with interlocked controls

### Typical Wiring for Heater Control
```
SSR 1 → Zone 1 Heater (AC Load)
SSR 2 → Zone 2 Heater (AC Load)  
SSR 3 → Zone 3 Heater (AC Load)

Safety Rule: Maximum 2 heaters on simultaneously
```

## Safety Considerations

### Critical Safety Rules
1. **Never Override Safety**: The system will not allow all SSRs on simultaneously
2. **Power Rating**: Ensure SSRs can handle your load current/voltage
3. **Heat Sinking**: SSRs generate heat - use appropriate heat sinks
4. **Electrical Safety**: Use proper AC wiring practices for high-voltage loads
5. **Emergency Stop**: Implement external emergency stop systems

### Best Practices
- **Test at Low Power**: Verify operation with low-power loads first
- **Monitor Current**: Use current monitoring for load verification
- **Thermal Protection**: Add thermal monitoring to prevent overheating
- **Backup Safety**: Implement hardware interlocks in addition to software
- **Documentation**: Clearly label which SSR controls which load

## Troubleshooting

### Common Issues

**SSR not switching:**
- Check wiring connections (control and load sides)
- Verify SSR power requirements
- Test with multimeter on control input
- Check load circuit continuity

**Safety violations:**
- Use `SSRSTATUS` to check current states
- Turn off one SSR before turning on another
- Use `SSR:SAFE?` in VISA mode to check safety status

**Unexpected behavior:**
- Verify SSR type (Active HIGH/LOW)
- Check for proper heat sinking
- Monitor for electrical interference
- Verify load ratings are not exceeded

### Debug Commands
- `SSRSTATUS` - Show current SSR states and safety status
- `SSR:GET? ALL` - Get all SSR states (VISA mode)
- `SSR:SAFE?` - Check safety status (VISA mode)
- `SSR:COUNT?` - Verify 3 SSRs detected (VISA mode)

### Error Messages
- `"ERROR: Safety violation - would result in all SSRs ON"` - Attempted unsafe operation
- `"ERROR: Cannot turn all SSRs ON (safety violation)"` - Attempted ALL,ON command
- `"Invalid SSR number (1-3)"` - SSR number out of range

## Integration with Temperature Control

### Example: Zone Temperature Control
```python
class ZoneController:
    def __init__(self, ssr_num, setpoint, hysteresis=2.0):
        self.ssr_num = ssr_num
        self.setpoint = setpoint
        self.hysteresis = hysteresis
        self.heating = False
    
    def update(self, temperature):
        if temperature < self.setpoint - self.hysteresis:
            # Need heating
            if set_ssr(self.ssr_num, True):
                self.heating = True
        elif temperature > self.setpoint + self.hysteresis:
            # Too hot
            set_ssr(self.ssr_num, False)
            self.heating = False
        
        return self.heating

# Create zone controllers
zones = [
    ZoneController(1, 50.0),  # SSR 1, 50°C target
    ZoneController(2, 60.0),  # SSR 2, 60°C target  
    ZoneController(3, 55.0),  # SSR 3, 55°C target
]

# Update based on temperature readings
for i, zone in enumerate(zones):
    temp = temperatures[i]  # From sensor readings
    heating = zone.update(temp)
    print(f"Zone {i+1}: {temp}°C → {'Heating' if heating else 'Off'}")
```

The SSR control system provides safe, interlocked control for high-power loads while maintaining the safety requirement that all three SSRs are never on simultaneously.
