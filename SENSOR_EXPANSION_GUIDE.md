# Adding Additional Sensors to Your Arduino Mega 2560

## Current Setup
Your Arduino Mega 2560 is now configured to read from **4 MAX6675 sensors** with Type K thermocouples.

## Hardware Connections for Multiple MAX6675 Sensors

### Shared Pins (All Sensors)
- **VCC** → 5V (all sensors)
- **GND** → GND (all sensors)  
- **SCK** → Pin 52 (shared SPI clock)
- **SO** → Pin 50 (shared MISO)

### Individual Chip Select Pins
- **Sensor 1 CS** → Pin 53
- **Sensor 2 CS** → Pin 49
- **Sensor 3 CS** → Pin 48
- **Sensor 4 CS** → Pin 47

## Additional Sensor Options

### 1. More MAX6675 Sensors
You can add up to 8+ MAX6675 sensors by using different CS pins:
- Available CS pins: 46, 45, 44, 43, 42, 41, 40, etc.

### 2. DS18B20 Digital Temperature Sensors
**Advantages:**
- Cheaper than MAX6675
- Multiple sensors on one wire
- Good accuracy (±0.5°C)
- Temperature range: -55°C to +125°C

**Connections:**
- VCC → 5V
- GND → GND
- Data → Any digital pin (e.g., Pin 2)
- 4.7kΩ pullup resistor between VCC and Data

### 3. Analog Temperature Sensors
**LM35 or TMP36 sensors:**
- Simple 3-pin sensors
- Connect to analog pins (A0, A1, A2, etc.)
- Lower cost but less accurate

**Connections:**
- VCC → 5V (LM35) or 3.3V-5V (TMP36)
- GND → GND
- Output → Analog pin

### 4. I2C Temperature Sensors
**Examples: BME280, SHT30, etc.**
- Can measure temperature, humidity, pressure
- Use I2C communication (SDA/SCL pins)
- Multiple sensors on same bus with different addresses

**Connections:**
- VCC → 3.3V or 5V
- GND → GND
- SDA → Pin 20 (Mega 2560)
- SCL → Pin 21 (Mega 2560)

## Expanding Your Current Setup

### To add more MAX6675 sensors:
1. Wire additional sensors sharing SCK (Pin 52) and SO (Pin 50)
2. Use unique CS pins for each sensor
3. Add sensor objects in code:
   ```cpp
   int thermoCS5 = 46;
   MAX6675 thermocouple5(thermoCLK, thermoCS5, thermoDO);
   ```

### To add DS18B20 sensors:
1. Wire to a digital pin with pullup resistor
2. Multiple sensors can share the same data line
3. Each sensor has a unique 64-bit address

### To add analog sensors:
1. Connect to available analog pins (A0-A15 on Mega)
2. Read using analogRead() function
3. Convert reading to temperature using sensor-specific formula

## Current Code Features
- Reads 4 MAX6675 sensors
- Displays temperatures in °C and °F
- Error handling for disconnected sensors
- Clear serial output formatting
- 2-second reading interval

## Pin Usage Summary
**Used Pins:**
- Pin 50: MISO (shared by all MAX6675)
- Pin 52: SCK (shared by all MAX6675)
- Pins 53, 49, 48, 47: CS for sensors 1-4

**Available Pins:**
- Digital: 2-46 (many available)
- Analog: A0-A15 (all available)
- I2C: Pins 20 (SDA), 21 (SCL)
- SPI: Additional CS pins available

Your Arduino Mega 2560 has plenty of pins for expansion!
