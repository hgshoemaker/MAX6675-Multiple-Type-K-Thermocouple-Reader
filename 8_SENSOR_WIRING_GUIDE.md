# 8-Sensor MAX6675 Wiring Guide

## Hardware Connections for 8 MAX6675 Sensors

### Shared Connections (All Sensors)
```
Arduino Mega 2560    →    All MAX6675 Modules
Pin 52 (SCK)        →    SCK (shared SPI clock)
Pin 50 (MISO)       →    SO  (shared serial output)
5V                  →    VCC (power - all modules)
GND                 →    GND (ground - all modules)
```

### Individual Chip Select (CS) Connections
```
Sensor #    Arduino Pin    MAX6675 CS Pin
Sensor 1    Pin 53        →    CS
Sensor 2    Pin 49        →    CS  
Sensor 3    Pin 48        →    CS
Sensor 4    Pin 47        →    CS
Sensor 5    Pin 46        →    CS
Sensor 6    Pin 45        →    CS
Sensor 7    Pin 44        →    CS
Sensor 8    Pin 43        →    CS
```

### Complete Wiring Table
```
MAX6675 Pin    Arduino Mega 2560 Pin    Notes
-----------    ---------------------    -----
VCC            5V                       Power (all sensors)
GND            GND                      Ground (all sensors)
SCK            Pin 52                   SPI Clock (shared)
SO             Pin 50                   MISO (shared)
CS (Sensor 1)  Pin 53                   Chip Select 1
CS (Sensor 2)  Pin 49                   Chip Select 2
CS (Sensor 3)  Pin 48                   Chip Select 3
CS (Sensor 4)  Pin 47                   Chip Select 4
CS (Sensor 5)  Pin 46                   Chip Select 5
CS (Sensor 6)  Pin 45                   Chip Select 6
CS (Sensor 7)  Pin 44                   Chip Select 7
CS (Sensor 8)  Pin 43                   Chip Select 8
```

## Visual Wiring Diagram

```
Arduino Mega 2560                    MAX6675 Modules (x8)
                                     
     5V  ●─────────────────────────●─● VCC (All modules)
    GND  ●─────────────────────────●─● GND (All modules)
Pin 52   ●─────────────────────────●─● SCK (All modules)
Pin 50   ●─────────────────────────●─● SO  (All modules)
                                     
Pin 53   ●───────────────────────────● CS (Sensor 1)
Pin 49   ●───────────────────────────● CS (Sensor 2)
Pin 48   ●───────────────────────────● CS (Sensor 3)
Pin 47   ●───────────────────────────● CS (Sensor 4)
Pin 46   ●───────────────────────────● CS (Sensor 5)
Pin 45   ●───────────────────────────● CS (Sensor 6)
Pin 44   ●───────────────────────────● CS (Sensor 7)
Pin 43   ●───────────────────────────● CS (Sensor 8)
```

## Type K Thermocouple Connections

Each MAX6675 module connects to one Type K thermocouple:

```
MAX6675 Module        Type K Thermocouple
--------------        -------------------
T+                →   Positive lead (usually yellow)
T-                →   Negative lead (usually red)
```

**Important Notes:**
- Type K thermocouple polarity matters
- Ensure proper connections to avoid reversed readings
- Each thermocouple connects to only one MAX6675 module

## Power Considerations

- **Total current**: 8 × 50mA = 400mA maximum
- **Arduino 5V supply**: Can handle up to 800mA typically
- **USB power**: Usually sufficient for 8 modules
- **External power**: Recommended for more than 8 modules

## Available Pins for Expansion

If you want to add even more sensors later:

**Additional CS pins available:**
- Pin 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30

**Maximum theoretical sensors**: 
- Limited by available digital pins (approximately 30+ sensors possible)

## Installation Checklist

### Before Connecting:
- [ ] Verify 5V power supply capacity
- [ ] Check all jumper wire connections
- [ ] Ensure thermocouples are Type K compatible
- [ ] Double-check pin assignments

### After Connecting:
- [ ] Power on system
- [ ] Open serial monitor at 9600 baud
- [ ] Verify all sensors show readings (not "ERROR")
- [ ] Test calibration mode with `CAL` command
- [ ] Test LabVIEW mode with `LVON` command

## Troubleshooting

### Common Issues:

**"ERROR" readings for some sensors:**
- Check CS pin connections
- Verify thermocouple connections
- Ensure unique CS pins for each sensor

**Inconsistent readings:**
- Check power supply stability
- Verify shared SCK/MISO connections
- Allow sensors to stabilize (2-3 minutes)

**Some sensors not responding:**
- Check individual CS pin wiring
- Verify power connections to all modules
- Test each module individually

### Testing Individual Sensors:

To test one sensor at a time, temporarily disconnect other CS pins and verify each sensor works independently.

## Performance Notes

- **Reading cycle time**: ~1.6 seconds for all 8 sensors (100ms delay between sensors)
- **Memory usage**: ~1886 bytes RAM (23% of Arduino Mega)
- **Flash usage**: ~13254 bytes (5.2% of Arduino Mega)
- **LabVIEW CSV output**: 8 values per line (8 sensors in Celsius only)
