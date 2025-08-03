# CSV Mode Troubleshooting Guide

## Issue: CSV Command Shows "OK" But No Data Output

### Problem Description
When you send the `CSV` command, the system responds with "OK" but then doesn't output any CSV data streams.

### Solution Steps

#### 1. **Test CSV Output Immediately**
```
> TEST
```
This will output one line of CSV data immediately to verify the system is reading sensors.

#### 2. **Check System Status**
The system should show this during startup:
```
MAX6675 Multiple Type K Thermocouple Test with ADS1115 ADC
Reading from 8 thermocouples and 4 voltage inputs...
ADS1115 initialized successfully - 4 voltage channels available
(or)
WARNING: ADS1115 not found! Voltage readings will show -999.0000
```

#### 3. **Verify CSV Mode is Active**
After sending `CSV`, wait at least 5 seconds for the first data output. The system outputs data every 5 seconds in CSV mode.

#### 4. **Expected CSV Format**
```
S1_C,S2_C,S3_C,S4_C,S5_C,S6_C,S7_C,S8_C,V1,V2,V3,V4
25.50,26.25,24.75,25.00,26.50,25.25,24.50,25.75,3.3000,1.2500,2.4750,0.5000
```

### Common Issues and Fixes

#### **Issue: No ADS1115 Connected**
**Symptoms:** Warning message about ADS1115 not found
**Solution:** 
- If you don't have an ADS1115, the system will still work but show -999.0000 for voltage readings
- To add ADS1115: Connect SDA→Pin 20, SCL→Pin 21, VDD→5V, GND→GND

#### **Issue: Sensor Errors**
**Symptoms:** Temperature readings show -999.00
**Solutions:**
- Check thermocouple connections
- Verify MAX6675 module wiring
- Use `DEBUG` command to test individual sensors

#### **Issue: System Stuck**
**Symptoms:** No response to commands
**Solutions:**
- Reset Arduino (power cycle)
- Check serial port settings (9600 baud)
- Send `EXIT` to return to human-readable mode

### Quick Diagnostics

#### **Test Sequence:**
1. **Power on** - Check startup messages
2. **Send `TEST`** - Verify immediate CSV output
3. **Send `CSV`** - Enable CSV mode (wait 5+ seconds)
4. **Send `HUMAN`** - Return to human-readable mode
5. **Send `DEBUG`** - Test individual sensors

#### **VISA Mode Testing:**
```
> VSON
VISA command-response mode enabled
> *IDN?
MAX6675_THERMOCOUPLE_READER,v1.0,SN001
> MEAS:ALL?
25.50,26.25,24.75,25.00,26.50,25.25,24.50,25.75,3.3000,1.2500,2.4750,0.5000
```

### Hardware Checklist

#### **MAX6675 Connections (Required):**
- VCC → 5V (all sensors)
- GND → GND (all sensors)  
- SCK → Pin 52 (shared)
- SO/MISO → Pin 50 (shared)
- CS pins → 53,49,48,47,46,45,44,43 (individual sensors)

#### **ADS1115 Connections (Optional):**
- VDD → 5V
- GND → GND
- SDA → Pin 20
- SCL → Pin 21
- A0-A3 → Your analog signals (0-6.144V max)

### Serial Terminal Settings
- **Baud Rate:** 9600
- **Data Bits:** 8
- **Parity:** None
- **Stop Bits:** 1
- **Flow Control:** None
- **Line Ending:** Both NL & CR (or just LF)

### Emergency Recovery
If system becomes unresponsive:
1. **Reset Arduino** (power cycle)
2. **Open Serial Monitor** at 9600 baud
3. **Send `EXIT`** to clear any mode
4. **Send `HUMAN`** to return to readable output

The system will automatically return to CSV mode on startup, outputting data every 5 seconds by default.
