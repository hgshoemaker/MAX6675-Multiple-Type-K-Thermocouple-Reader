# SSR Control Guide

## Overview

The MAX6675 thermocouple reader now includes 3 Solid State Relay (SSR) outputs on digital pins D10, D11, and D12. These SSRs have built-in safety interlocking to prevent more than one from being on simultaneously, which prevents system overload and ensures safe operation.

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

### **CRITICAL SAFETY RULE: Only One On At A Time**
The system prevents more than one SSR from being on simultaneously through:
- **Hardware Interlocking**: Software prevents unsafe combinations
- **Safety Checks**: Every ON command is validated before execution
- **Error Messages**: Clear feedback when safety violations are attempted

### Safety Logic
- ✅ **1 SSR ON**: Allowed
- ❌ **2 SSRs ON**: **BLOCKED** - Safety violation
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
| `SSR:GET? ALL` | Get all SSR states | `1,0,0` | ℹ️ Info only |
| `SSR:COUNT?` | Number of SSRs | `3` | ℹ️ Info only |
| `SSR:SAFE?` | Check safety status | `1` (safe) or `0` (unsafe) | ℹ️ Info only |

⚠️ **Note**: `SSR:SET ALL,ON` is **NOT SUPPORTED** and will return an error.

## Updated System Configuration

### **Hardware Changes**
- **3 Relays** (reduced from 8) on pins D2, D3, D4
- **3 SSRs** with **stricter safety** - only one on at a time
- **8 Thermocouples** and **4 Voltage Channels** unchanged

### **Safety Updates**
- **Previous Rule**: Maximum 2 SSRs on simultaneously
- **New Rule**: **Only 1 SSR on at a time** (stricter safety)

## Usage Examples

### Manual Control
```
> SSRON1
SSR 1 turned ON

> SSRON2  
ERROR: Safety violation - only one SSR allowed ON at a time

> SSROFF1
SSR 1 turned OFF

> SSRON2
SSR 2 turned ON

> SSRSTATUS
=== SSR STATUS ===
SSR 1 (D10): OFF
SSR 2 (D11): ON
SSR 3 (D12): OFF
Active SSRs: 1/3 (Safety: Only one on at a time)
==================
```

### VISA Mode Control
```
> SSR:SET S1,ON
OK

> SSR:SET S2,ON
ERROR: Safety violation - only one SSR allowed ON at a time

> SSR:GET? ALL
1,0,0

> SSR:SAFE?
1

> SSR:SET S1,OFF
OK

> SSR:SET S3,ON
OK

> SSR:GET? ALL
0,0,1
```

### Relay Control Examples
```
> RELAYON1
Relay 1 turned ON

> RELAYON2
Relay 2 turned ON

> RELAYON3
Relay 3 turned ON

> RELAYSTATUS
=== RELAY STATUS ===
Relay 1 (D2): ON
Relay 2 (D3): ON
Relay 3 (D4): ON
====================
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
    """Safely set SSR state - only one allowed on at a time"""
    cmd = f"SSR:SET S{ssr_num},{'ON' if state else 'OFF'}\n"
    ser.write(cmd.encode())
    response = ser.readline().decode().strip()
    if response == "OK":
        print(f"SSR {ssr_num} {'ON' if state else 'OFF'}")
        return True
    else:
        print(f"Failed: {response}")
        return False

def set_relay(relay_num, state):
    """Set relay state - all 3 relays can be controlled independently"""
    cmd = f"RELAY:SET R{relay_num},{'ON' if state else 'OFF'}\n"
    ser.write(cmd.encode())
    response = ser.readline().decode().strip()
    if response == "OK":
        print(f"Relay {relay_num} {'ON' if state else 'OFF'}")
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

# Example usage - Sequential SSR control
set_ssr(1, True)   # Turn SSR 1 ON
set_ssr(2, True)   # This will fail due to safety

set_ssr(1, False)  # Turn SSR 1 OFF first
set_ssr(2, True)   # Now SSR 2 can turn ON

# Relay control - all can be on simultaneously
set_relay(1, True)
set_relay(2, True) 
set_relay(3, True)

states = get_ssr_states()
print(f"SSR States: {states}")
print(f"Safe: {check_safety()}")

ser.close()
```

### Zone-Based Control System
```python
class ZoneController:
    def __init__(self, zone_id, setpoint, hysteresis=2.0):
        self.zone_id = zone_id
        self.setpoint = setpoint
        self.hysteresis = hysteresis
        self.active = False
        self.priority = zone_id  # Lower number = higher priority
    
    def needs_heating(self, temperature):
        return temperature < self.setpoint - self.hysteresis
    
    def too_hot(self, temperature):
        return temperature > self.setpoint + self.hysteresis

def sequential_zone_control(zones, temperatures):
    """Control zones sequentially - only one heater on at a time"""
    
    # Turn off any currently active SSR
    for i in range(1, 4):
        set_ssr(i, False)
    
    # Find highest priority zone that needs heating
    zones_needing_heat = []
    for i, zone in enumerate(zones):
        temp = temperatures[i]
        if zone.needs_heating(temp):
            zones_needing_heat.append((zone.priority, i+1, zone))
    
    if zones_needing_heat:
        # Sort by priority (lower number = higher priority)
        zones_needing_heat.sort(key=lambda x: x[0])
        priority, ssr_num, zone = zones_needing_heat[0]
        
        if set_ssr(ssr_num, True):
            print(f"Zone {ssr_num}: Heating ON (Priority {priority}, Temp: {temperatures[ssr_num-1]}°C)")
            return ssr_num
    
    return None

# Create zone controllers with different priorities
zones = [
    ZoneController(1, 50.0),  # Zone 1: High priority (1)
    ZoneController(3, 60.0),  # Zone 2: Low priority (3)  
    ZoneController(2, 55.0),  # Zone 3: Medium priority (2)
]

# Simulate temperature control loop
temperatures = [45.5, 52.3, 48.7, 50.1, 49.8, 51.2, 47.9, 46.4]
active_zone = sequential_zone_control(zones, temperatures[:3])

if active_zone:
    print(f"Zone {active_zone} is being heated")
else:
    print("No zones need heating")
```

## Applications

### Common Use Cases
- **Sequential Process Control**: Control equipment that must run one at a time
- **High-Power Load Management**: Prevent electrical overload from multiple heaters
- **Safety-Critical Systems**: Ensure only one heating element active
- **Priority-Based Control**: Heat most critical zone first

### Typical Wiring for Sequential Heater Control
```
SSR 1 → Zone 1 Critical Heater (AC Load)
SSR 2 → Zone 2 Secondary Heater (AC Load)  
SSR 3 → Zone 3 Backup Heater (AC Load)

Safety Rule: Only 1 heater on at any time
```

### Relay Control for Supporting Equipment
```
Relay 1 → Cooling Fan (can run with any heater)
Relay 2 → Process Pump (can run with any heater)
Relay 3 → Status Indicator (can run independently)

No safety restrictions on relays
```

## Safety Considerations

### Critical Safety Rules
1. **Only One SSR Active**: The system enforces this automatically
2. **Power Rating**: Ensure each SSR can handle its load current/voltage
3. **Heat Sinking**: SSRs generate heat - use appropriate heat sinks
4. **Electrical Safety**: Use proper AC wiring practices for high-voltage loads
5. **Emergency Stop**: Implement external emergency stop systems

### Best Practices
- **Sequential Operation**: Design processes for one-at-a-time operation
- **Priority Systems**: Implement priority-based control for critical zones
- **Monitoring**: Use temperature monitoring to verify heating effectiveness
- **Backup Safety**: Implement hardware interlocks in addition to software
- **Clear Labeling**: Document which SSR controls which load

## Troubleshooting

### Common Issues

**SSR not switching:**
- Check wiring connections (control and load sides)
- Verify SSR power requirements
- Test with multimeter on control input
- Check load circuit continuity

**Safety violations:**
- Use `SSRSTATUS` to check current states
- Turn off current SSR before turning on another
- Use `SSR:SAFE?` in VISA mode to check safety status

**Sequential control not working:**
- Verify only one SSR is meant to be on at a time
- Check priority logic in control software
- Monitor temperature feedback for each zone

### Debug Commands
- `SSRSTATUS` - Show current SSR states and safety status
- `RELAYSTATUS` - Show all relay states (3 relays)
- `SSR:GET? ALL` - Get all SSR states (VISA mode)
- `RELAY:GET? ALL` - Get all relay states (VISA mode)
- `SSR:SAFE?` - Check safety status (VISA mode)
- `SSR:COUNT?` - Verify 3 SSRs detected (VISA mode)
- `RELAY:COUNT?` - Verify 3 relays detected (VISA mode)

### Error Messages
- `"ERROR: Safety violation - only one SSR allowed ON at a time"` - Attempted to turn on second SSR
- `"ERROR: Cannot turn all SSRs ON (safety violation - only one allowed)"` - Attempted ALL,ON command
- `"Invalid relay number (1-3)"` - Relay number out of range
- `"Invalid SSR number (1-3)"` - SSR number out of range

## Integration with Temperature Control

### Example: Priority-Based Zone Control
```python
class PriorityZoneController:
    def __init__(self):
        self.zones = [
            {'id': 1, 'setpoint': 50.0, 'priority': 1, 'hysteresis': 2.0},
            {'id': 2, 'setpoint': 60.0, 'priority': 2, 'hysteresis': 2.0},
            {'id': 3, 'setpoint': 55.0, 'priority': 3, 'hysteresis': 2.0},
        ]
        self.current_heating_zone = None
    
    def update_control(self, temperatures):
        """Update control based on temperature readings"""
        
        # Check if current zone still needs heating
        if self.current_heating_zone:
            zone = self.zones[self.current_heating_zone - 1]
            temp = temperatures[self.current_heating_zone - 1]
            
            if temp > zone['setpoint'] + zone['hysteresis']:
                # Current zone is too hot, turn off
                set_ssr(self.current_heating_zone, False)
                self.current_heating_zone = None
                print(f"Zone {self.current_heating_zone} satisfied, turning off")
        
        # If no zone is heating, find highest priority zone that needs heat
        if not self.current_heating_zone:
            zones_needing_heat = []
            
            for i, zone in enumerate(self.zones):
                temp = temperatures[i]
                if temp < zone['setpoint'] - zone['hysteresis']:
                    zones_needing_heat.append((zone['priority'], zone['id']))
            
            if zones_needing_heat:
                # Sort by priority and activate highest priority zone
                zones_needing_heat.sort()
                priority, zone_id = zones_needing_heat[0]
                
                if set_ssr(zone_id, True):
                    self.current_heating_zone = zone_id
                    print(f"Zone {zone_id} activated (priority {priority})")
        
        return self.current_heating_zone

# Usage example
controller = PriorityZoneController()
temperatures = [45.5, 52.3, 48.7]  # Zone temperatures

active_zone = controller.update_control(temperatures)
if active_zone:
    print(f"Currently heating zone {active_zone}")
else:
    print("No heating required")
```

The updated SSR control system provides safe, sequential control for high-power loads with the enhanced safety requirement that only one SSR can be active at any time, while the 3 relays provide unrestricted control for supporting equipment.
