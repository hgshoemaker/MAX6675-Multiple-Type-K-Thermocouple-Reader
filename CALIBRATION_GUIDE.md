# MAX6675 Thermocouple Calibration Guide

## Overview
Your Arduino Mega 2560 now has advanced calibration features for accurate temperature readings from multiple MAX6675 sensors.

## Calibration Features Added

### 1. Calibration Mode
- Enter by sending `CAL` via serial monitor
- Exit by sending `EXIT` via serial monitor
- Shows raw readings from all sensors
- Calculates average and differences between sensors

### 2. Calibration Offsets
Each sensor has its own calibration offset variable:
```cpp
float calibrationOffset1 = 0.0;  // Sensor 1 offset in °C
float calibrationOffset2 = 0.0;  // Sensor 2 offset in °C
float calibrationOffset3 = 0.0;  // Sensor 3 offset in °C
float calibrationOffset4 = 0.0;  // Sensor 4 offset in °C
```

## How to Calibrate Your Sensors

### Method 1: Ice Water Calibration (0°C Reference)
1. **Prepare ice water bath:**
   - Fill container with ice and water
   - Stir to ensure 0°C temperature
   - Let stabilize for 2-3 minutes

2. **Place all sensors:**
   - Submerge all thermocouple tips in ice water
   - Ensure tips don't touch container walls
   - Wait 2-3 minutes for temperature stabilization

3. **Enter calibration mode:**
   - Open serial monitor at 9600 baud
   - Send `CAL` command
   - Wait for readings to stabilize

4. **Record the readings:**
   - Note the raw temperature readings
   - Calculate the offset needed to reach 0°C
   - Example: If sensor reads 2.5°C, offset should be -2.5

### Method 2: Boiling Water Calibration (100°C Reference)
1. **Prepare boiling water:**
   - Bring water to rolling boil
   - Account for altitude (water boils at lower temps at higher altitudes)
   - At sea level: 100°C, at 1000m: ~96.7°C

2. **Follow same steps as ice water method**
3. **Calculate offsets for 100°C reference**

### Method 3: Commercial Reference Thermometer
1. **Use calibrated reference thermometer**
2. **Place all sensors in same environment**
3. **Compare readings and calculate offsets**

## Applying Calibration Offsets

### Step 1: Calculate Offsets
From calibration mode readings:
```
If Sensor 1 reads 2.5°C in ice water (should be 0°C):
calibrationOffset1 = -2.5

If Sensor 2 reads -1.2°C in ice water (should be 0°C):
calibrationOffset2 = 1.2
```

### Step 2: Update Code
Edit the offset variables at the top of main.cpp:
```cpp
float calibrationOffset1 = -2.5;  // Adjust based on your readings
float calibrationOffset2 = 1.2;   // Adjust based on your readings
float calibrationOffset3 = 0.8;   // Adjust based on your readings
float calibrationOffset4 = -0.3;  // Adjust based on your readings
```

### Step 3: Upload Updated Code
- Save the file
- Upload to Arduino
- Verify calibrated readings

## Serial Monitor Commands

| Command | Function |
|---------|----------|
| `CAL` | Enter calibration mode (shows raw readings) |
| `EXIT` | Exit calibration mode (return to normal operation) |

## Calibration Mode Output Example
```
=== CALIBRATION READINGS (RAW) ===
Sensor 1 RAW: 2.50°C
Sensor 2 RAW: -1.25°C
Sensor 3 RAW: 0.75°C
Sensor 4 RAW: -0.50°C
Average: 0.38°C

Differences from average:
Sensor 1: 2.12°C
Sensor 2: -1.63°C
Sensor 3: 0.37°C
Sensor 4: -0.88°C
=====================================
```

## Normal Operation Output Example
```
=== CALIBRATED Temperature Readings ===
Sensor 1: 0.00°C
Sensor 2: 0.00°C
Sensor 3: 0.00°C
Sensor 4: 0.00°C
Sensor 5: 0.00°C
Sensor 6: 0.00°C
Sensor 7: 0.00°C
Sensor 8: 0.00°C
```

## Calibration Tips

### Best Practices
1. **Use stable reference temperatures** (ice water is most reliable)
2. **Allow sensors to stabilize** (2-3 minutes minimum)
3. **Calibrate regularly** (monthly for critical applications)
4. **Check sensor connections** before calibrating
5. **Use the same environment** for all sensors during calibration

### Troubleshooting
- **Sensor reads "ERROR"**: Check wiring connections
- **Readings drift**: May need thermocouple replacement
- **Large offsets (>5°C)**: Check sensor quality and connections
- **Inconsistent readings**: Ensure stable reference temperature

### Accuracy Expectations
- **After calibration**: ±1°C typical accuracy
- **Without calibration**: ±2-5°C typical accuracy
- **MAX6675 resolution**: 0.25°C steps

## Advanced Calibration Options

### Two-Point Calibration
For maximum accuracy, calibrate at two temperatures:
1. Ice water (0°C)
2. Boiling water (100°C at sea level)

This requires more complex offset calculations but provides better accuracy across the full temperature range.

### Environmental Considerations
- **Cold junction compensation**: MAX6675 handles this automatically
- **Altitude effects**: Adjust boiling point reference accordingly
- **Ambient temperature**: Can affect accuracy, especially at temperature extremes
