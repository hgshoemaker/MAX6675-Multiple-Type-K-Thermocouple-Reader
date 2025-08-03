#include <Arduino.h>
#include "max6675.h"
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

// ADS1115 ADC setup
Adafruit_ADS1115 ads;  // Create ADS1115 object (default I2C address 0x48)

// MAX6675 pin connections - All sensors share SCK and MISO
// VCC -> 5V (all sensors)
// GND -> GND (all sensors)
// SCK -> Pin 52 (SCK on Mega 2560) - shared by all sensors
// SO  -> Pin 50 (MISO on Mega 2560) - shared by all sensors
// CS  -> Different pins for each sensor

int thermoDO = 50;    // MISO pin (shared)
int thermoCLK = 52;   // SCK pin (shared)

// Chip Select pins for multiple sensors
int thermoCS1 = 53;   // First sensor CS
int thermoCS2 = 49;   // Second sensor CS
int thermoCS3 = 48;   // Third sensor CS
int thermoCS4 = 47;   // Fourth sensor CS
int thermoCS5 = 46;   // Fifth sensor CS
int thermoCS6 = 45;   // Sixth sensor CS
int thermoCS7 = 44;   // Seventh sensor CS
int thermoCS8 = 43;   // Eighth sensor CS

// Relay control pins (D2-D9)
int relay1Pin = 2;    // Relay 1 on pin D2
int relay2Pin = 3;    // Relay 2 on pin D3
int relay3Pin = 4;    // Relay 3 on pin D4
int relay4Pin = 5;    // Relay 4 on pin D5
int relay5Pin = 6;    // Relay 5 on pin D6
int relay6Pin = 7;    // Relay 6 on pin D7
int relay7Pin = 8;    // Relay 7 on pin D8
int relay8Pin = 9;    // Relay 8 on pin D9

// Relay states (false = OFF, true = ON)
bool relay1State = false;
bool relay2State = false;
bool relay3State = false;
bool relay4State = false;
bool relay5State = false;
bool relay6State = false;
bool relay7State = false;
bool relay8State = false;

// ADS1115 ADC configuration
// The ADS1115 has 4 channels: A0, A1, A2, A3
// Default I2C address is 0x48
// Gain setting affects measurement range:
//   GAIN_TWOTHIRDS  +/- 6.144V  1 bit = 0.1875mV (default)
//   GAIN_ONE        +/- 4.096V  1 bit = 0.125mV
//   GAIN_TWO        +/- 2.048V  1 bit = 0.0625mV
//   GAIN_FOUR       +/- 1.024V  1 bit = 0.03125mV
//   GAIN_EIGHT      +/- 0.512V  1 bit = 0.015625mV
//   GAIN_SIXTEEN    +/- 0.256V  1 bit = 0.0078125mV
adsGain_t adsGain = GAIN_TWOTHIRDS;  // +/- 6.144V range

// ADS1115 calibration offsets for each channel (adjust these values)
float voltageOffset1 = 0.0;  // Channel A0 offset in V
float voltageOffset2 = 0.0;  // Channel A1 offset in V
float voltageOffset3 = 0.0;  // Channel A2 offset in V
float voltageOffset4 = 0.0;  // Channel A3 offset in V

// Calibration offsets for each sensor (adjust these values)
float calibrationOffset1 = 0.0;  // Sensor 1 offset in °C
float calibrationOffset2 = -0.25;  // Sensor 2 offset in °C
float calibrationOffset3 = -0.06;  // Sensor 3 offset in °C
float calibrationOffset4 = -0.69;  // Sensor 4 offset in °C
float calibrationOffset5 = -0.69;  // Sensor 5 offset in °C
float calibrationOffset6 = -0.06;  // Sensor 6 offset in °C
float calibrationOffset7 = -0.31;  // Sensor 7 offset in °C
float calibrationOffset8 = -0.19;  // Sensor 8 offset in °C

// Create multiple MAX6675 objects
MAX6675 thermocouple1(thermoCLK, thermoCS1, thermoDO);  // Sensor 1
MAX6675 thermocouple2(thermoCLK, thermoCS2, thermoDO);  // Sensor 2
MAX6675 thermocouple3(thermoCLK, thermoCS3, thermoDO);  // Sensor 3
MAX6675 thermocouple4(thermoCLK, thermoCS4, thermoDO);  // Sensor 4
MAX6675 thermocouple5(thermoCLK, thermoCS5, thermoDO);  // Sensor 5
MAX6675 thermocouple6(thermoCLK, thermoCS6, thermoDO);  // Sensor 6
MAX6675 thermocouple7(thermoCLK, thermoCS7, thermoDO);  // Sensor 7
MAX6675 thermocouple8(thermoCLK, thermoCS8, thermoDO);  // Sensor 8

// Calibration mode flag
bool calibrationMode = false;
bool labviewMode = false;  // Flag for LabVIEW output format
bool visaMode = true;      // Flag for VISA command-response mode (DEFAULT)

// VISA command buffer
String commandBuffer = "";
bool commandReady = false;

// VISA instrument identification
const String INSTRUMENT_ID = "MAX6675_THERMOCOUPLE_READER,v1.0,SN001";

// Function to apply calibration offset
float applyCalibratedReading(float rawTemp, float offset) {
  return rawTemp + offset;
}

// Function prototype for testIndividualSensors
void testIndividualSensors();

// Function to read calibrated temperature
float readCalibratedCelsius(MAX6675 &sensor, float offset) {
  double rawTemp = sensor.readCelsius();
  if (isnan(rawTemp)) {
    return NAN;
  }
  return applyCalibratedReading(rawTemp, offset);
}

// Function to read voltage from ADS1115 channel
float readVoltage(int channel) {
  if (channel < 0 || channel > 3) {
    return NAN;  // Invalid channel
  }
  
  int16_t adc = 0;
  switch (channel) {
    case 0: adc = ads.readADC_SingleEnded(0); break;
    case 1: adc = ads.readADC_SingleEnded(1); break;
    case 2: adc = ads.readADC_SingleEnded(2); break;
    case 3: adc = ads.readADC_SingleEnded(3); break;
  }
  
  // Convert ADC reading to voltage based on gain setting
  float voltage = 0.0;
  switch (adsGain) {
    case GAIN_TWOTHIRDS: voltage = adc * 0.1875 / 1000.0; break;  // 0.1875mV per bit
    case GAIN_ONE:       voltage = adc * 0.125 / 1000.0; break;   // 0.125mV per bit
    case GAIN_TWO:       voltage = adc * 0.0625 / 1000.0; break;  // 0.0625mV per bit
    case GAIN_FOUR:      voltage = adc * 0.03125 / 1000.0; break; // 0.03125mV per bit
    case GAIN_EIGHT:     voltage = adc * 0.015625 / 1000.0; break;// 0.015625mV per bit
    case GAIN_SIXTEEN:   voltage = adc * 0.0078125 / 1000.0; break;// 0.0078125mV per bit
    default:             voltage = adc * 0.1875 / 1000.0; break;  // Default to GAIN_TWOTHIRDS
  }
  
  return voltage;
}

// Function to read calibrated voltage
float readCalibratedVoltage(int channel, float offset) {
  float rawVoltage = readVoltage(channel);
  if (isnan(rawVoltage)) {
    return NAN;
  }
  return rawVoltage + offset;
}

// Function to initialize ADS1115
void initializeADS1115() {
  if (!ads.begin()) {
    Serial.println("WARNING: ADS1115 not found! Voltage readings will show -999.0000");
    Serial.println("Check I2C wiring: SDA=Pin20, SCL=Pin21, VDD=5V, GND=GND");
    return;
  }
  ads.setGain(adsGain);
  Serial.println("ADS1115 initialized successfully - 4 voltage channels available");
}

// Function to initialize relay pins
void initializeRelays() {
  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  pinMode(relay3Pin, OUTPUT);
  pinMode(relay4Pin, OUTPUT);
  pinMode(relay5Pin, OUTPUT);
  pinMode(relay6Pin, OUTPUT);
  pinMode(relay7Pin, OUTPUT);
  pinMode(relay8Pin, OUTPUT);
  
  // Set all relays to OFF initially
  digitalWrite(relay1Pin, LOW);
  digitalWrite(relay2Pin, LOW);
  digitalWrite(relay3Pin, LOW);
  digitalWrite(relay4Pin, LOW);
  digitalWrite(relay5Pin, LOW);
  digitalWrite(relay6Pin, LOW);
  digitalWrite(relay7Pin, LOW);
  digitalWrite(relay8Pin, LOW);
}

// Function to set relay state
void setRelayState(int relayNumber, bool state) {
  int pin = 0;
  bool* relayState = nullptr;
  
  switch(relayNumber) {
    case 1: pin = relay1Pin; relayState = &relay1State; break;
    case 2: pin = relay2Pin; relayState = &relay2State; break;
    case 3: pin = relay3Pin; relayState = &relay3State; break;
    case 4: pin = relay4Pin; relayState = &relay4State; break;
    case 5: pin = relay5Pin; relayState = &relay5State; break;
    case 6: pin = relay6Pin; relayState = &relay6State; break;
    case 7: pin = relay7Pin; relayState = &relay7State; break;
    case 8: pin = relay8Pin; relayState = &relay8State; break;
    default: return; // Invalid relay number
  }
  
  *relayState = state;
  digitalWrite(pin, state ? HIGH : LOW);
}

// Function to get relay state
bool getRelayState(int relayNumber) {
  switch(relayNumber) {
    case 1: return relay1State;
    case 2: return relay2State;
    case 3: return relay3State;
    case 4: return relay4State;
    case 5: return relay5State;
    case 6: return relay6State;
    case 7: return relay7State;
    case 8: return relay8State;
    default: return false; // Invalid relay number
  }
}

// Function to set all relays to a specific state
void setAllRelays(bool state) {
  for(int i = 1; i <= 8; i++) {
    setRelayState(i, state);
  }
}

// Function to display relay status
void displayRelayStatus() {
  Serial.println("=== RELAY STATUS ===");
  for(int i = 1; i <= 8; i++) {
    Serial.print("Relay ");
    Serial.print(i);
    Serial.print(" (D");
    Serial.print(i + 1);
    Serial.print("): ");
    Serial.println(getRelayState(i) ? "ON" : "OFF");
  }
  Serial.println("====================");
}



// Function to enter calibration mode
void enterCalibrationMode() {
  Serial.println("\n========== CALIBRATION MODE ==========");
  Serial.println("Instructions:");
  Serial.println("1. Place ALL sensors in the same reference environment");
  Serial.println("2. Use ice water (0°C) or boiling water (100°C) for reference");
  Serial.println("3. Wait for temperatures to stabilize");
  Serial.println("4. Note the differences from expected temperature");
  Serial.println("5. Update calibration offsets in code");
  Serial.println("=======================================\n");
  
  calibrationMode = true;
}

// Function to display calibration readings
void displayCalibrationReadings() {
  Serial.println("=== CALIBRATION READINGS (RAW) ===");
  
  // Read all sensors without calibration, with delays for stability
  double temp1 = thermocouple1.readCelsius();
  delay(100);
  double temp2 = thermocouple2.readCelsius();
  delay(100);
  double temp3 = thermocouple3.readCelsius();
  delay(100);
  double temp4 = thermocouple4.readCelsius();
  delay(100);
  double temp5 = thermocouple5.readCelsius();
  delay(100);
  double temp6 = thermocouple6.readCelsius();
  delay(100);
  double temp7 = thermocouple7.readCelsius();
  delay(100);
  double temp8 = thermocouple8.readCelsius();
  
  Serial.print("Sensor 1 RAW: ");
  if (isnan(temp1)) {
    Serial.println("ERROR");
  } else {
    Serial.print(temp1); Serial.println("°C");
  }
  
  Serial.print("Sensor 2 RAW: ");
  if (isnan(temp2)) {
    Serial.println("ERROR");
  } else {
    Serial.print(temp2); Serial.println("°C");
  }
  
  Serial.print("Sensor 3 RAW: ");
  if (isnan(temp3)) {
    Serial.println("ERROR");
  } else {
    Serial.print(temp3); Serial.println("°C");
  }
  
  Serial.print("Sensor 4 RAW: ");
  if (isnan(temp4)) {
    Serial.println("ERROR");
  } else {
    Serial.print(temp4); Serial.println("°C");
  }
  
  Serial.print("Sensor 5 RAW: ");
  if (isnan(temp5)) {
    Serial.println("ERROR");
  } else {
    Serial.print(temp5); Serial.println("°C");
  }
  
  Serial.print("Sensor 6 RAW: ");
  if (isnan(temp6)) {
    Serial.println("ERROR");
  } else {
    Serial.print(temp6); Serial.println("°C");
  }
  
  Serial.print("Sensor 7 RAW: ");
  if (isnan(temp7)) {
    Serial.println("ERROR");
  } else {
    Serial.print(temp7); Serial.println("°C");
  }
  
  Serial.print("Sensor 8 RAW: ");
  if (isnan(temp8)) {
    Serial.println("ERROR");
  } else {
    Serial.print(temp8); Serial.println("°C");
  }
  
  // Calculate average (excluding NaN values)
  float sum = 0;
  int count = 0;
  if (!isnan(temp1)) { sum += temp1; count++; }
  if (!isnan(temp2)) { sum += temp2; count++; }
  if (!isnan(temp3)) { sum += temp3; count++; }
  if (!isnan(temp4)) { sum += temp4; count++; }
  if (!isnan(temp5)) { sum += temp5; count++; }
  if (!isnan(temp6)) { sum += temp6; count++; }
  if (!isnan(temp7)) { sum += temp7; count++; }
  if (!isnan(temp8)) { sum += temp8; count++; }
  
  if (count > 0) {
    float average = sum / count;
    Serial.print("Average: "); Serial.print(average); Serial.println("°C");
    
    // Show differences from average
    Serial.println("\nDifferences from average:");
    if (!isnan(temp1)) {
      Serial.print("Sensor 1: "); Serial.print(temp1 - average, 2); Serial.println("°C");
    }
    if (!isnan(temp2)) {
      Serial.print("Sensor 2: "); Serial.print(temp2 - average, 2); Serial.println("°C");
    }
    if (!isnan(temp3)) {
      Serial.print("Sensor 3: "); Serial.print(temp3 - average, 2); Serial.println("°C");
    }
    if (!isnan(temp4)) {
      Serial.print("Sensor 4: "); Serial.print(temp4 - average, 2); Serial.println("°C");
    }
    if (!isnan(temp5)) {
      Serial.print("Sensor 5: "); Serial.print(temp5 - average, 2); Serial.println("°C");
    }
    if (!isnan(temp6)) {
      Serial.print("Sensor 6: "); Serial.print(temp6 - average, 2); Serial.println("°C");
    }
    if (!isnan(temp7)) {
      Serial.print("Sensor 7: "); Serial.print(temp7 - average, 2); Serial.println("°C");
    }
    if (!isnan(temp8)) {
      Serial.print("Sensor 8: "); Serial.print(temp8 - average, 2); Serial.println("°C");
    }
  }
  
  Serial.println("=====================================\n");
}

// Function to output data in LabVIEW-compatible format (Celsius only)
void outputLabVIEWFormat() {
  // Read all sensors with longer delays between readings for stability
  float temp1C = readCalibratedCelsius(thermocouple1, calibrationOffset1);
  delay(150);  // Increased delay for better stability
  float temp2C = readCalibratedCelsius(thermocouple2, calibrationOffset2);
  delay(150);
  float temp3C = readCalibratedCelsius(thermocouple3, calibrationOffset3);
  delay(150);
  float temp4C = readCalibratedCelsius(thermocouple4, calibrationOffset4);
  delay(150);
  float temp5C = readCalibratedCelsius(thermocouple5, calibrationOffset5);
  delay(150);
  float temp6C = readCalibratedCelsius(thermocouple6, calibrationOffset6);
  delay(150);
  float temp7C = readCalibratedCelsius(thermocouple7, calibrationOffset7);
  delay(150);
  float temp8C = readCalibratedCelsius(thermocouple8, calibrationOffset8);
  
  // Read all voltage channels
  float voltage1 = readCalibratedVoltage(0, voltageOffset1);
  float voltage2 = readCalibratedVoltage(1, voltageOffset2);
  float voltage3 = readCalibratedVoltage(2, voltageOffset3);
  float voltage4 = readCalibratedVoltage(3, voltageOffset4);
  
  // Output in CSV format: S1_C,S2_C,S3_C,S4_C,S5_C,S6_C,S7_C,S8_C,V1,V2,V3,V4
  Serial.print(isnan(temp1C) ? -999.0 : temp1C, 2); Serial.print(",");
  Serial.print(isnan(temp2C) ? -999.0 : temp2C, 2); Serial.print(",");
  Serial.print(isnan(temp3C) ? -999.0 : temp3C, 2); Serial.print(",");
  Serial.print(isnan(temp4C) ? -999.0 : temp4C, 2); Serial.print(",");
  Serial.print(isnan(temp5C) ? -999.0 : temp5C, 2); Serial.print(",");
  Serial.print(isnan(temp6C) ? -999.0 : temp6C, 2); Serial.print(",");
  Serial.print(isnan(temp7C) ? -999.0 : temp7C, 2); Serial.print(",");
  Serial.print(isnan(temp8C) ? -999.0 : temp8C, 2); Serial.print(",");
  Serial.print(isnan(voltage1) ? -999.0 : voltage1, 4); Serial.print(",");
  Serial.print(isnan(voltage2) ? -999.0 : voltage2, 4); Serial.print(",");
  Serial.print(isnan(voltage3) ? -999.0 : voltage3, 4); Serial.print(",");
  Serial.print(isnan(voltage4) ? -999.0 : voltage4, 4);
  Serial.println(); // End line
}

// Function to output data in JSON format (alternative for LabVIEW) - Celsius only
void outputJSONFormat() {
  Serial.print("{");
  
  float temp1C = readCalibratedCelsius(thermocouple1, calibrationOffset1);
  delay(100);
  float temp2C = readCalibratedCelsius(thermocouple2, calibrationOffset2);
  delay(100);
  float temp3C = readCalibratedCelsius(thermocouple3, calibrationOffset3);
  delay(100);
  float temp4C = readCalibratedCelsius(thermocouple4, calibrationOffset4);
  delay(100);
  float temp5C = readCalibratedCelsius(thermocouple5, calibrationOffset5);
  delay(100);
  float temp6C = readCalibratedCelsius(thermocouple6, calibrationOffset6);
  delay(100);
  float temp7C = readCalibratedCelsius(thermocouple7, calibrationOffset7);
  delay(100);
  float temp8C = readCalibratedCelsius(thermocouple8, calibrationOffset8);
  
  float voltage1 = readCalibratedVoltage(0, voltageOffset1);
  float voltage2 = readCalibratedVoltage(1, voltageOffset2);
  float voltage3 = readCalibratedVoltage(2, voltageOffset3);
  float voltage4 = readCalibratedVoltage(3, voltageOffset4);
  
  Serial.print("\"sensor1\":{\"celsius\":");
  Serial.print(isnan(temp1C) ? -999.0 : temp1C, 2);
  Serial.print("},");
  
  Serial.print("\"sensor2\":{\"celsius\":");
  Serial.print(isnan(temp2C) ? -999.0 : temp2C, 2);
  Serial.print("},");
  
  Serial.print("\"sensor3\":{\"celsius\":");
  Serial.print(isnan(temp3C) ? -999.0 : temp3C, 2);
  Serial.print("},");
  
  Serial.print("\"sensor4\":{\"celsius\":");
  Serial.print(isnan(temp4C) ? -999.0 : temp4C, 2);
  Serial.print("},");
  
  Serial.print("\"sensor5\":{\"celsius\":");
  Serial.print(isnan(temp5C) ? -999.0 : temp5C, 2);
  Serial.print("},");
  
  Serial.print("\"sensor6\":{\"celsius\":");
  Serial.print(isnan(temp6C) ? -999.0 : temp6C, 2);
  Serial.print("},");
  
  Serial.print("\"sensor7\":{\"celsius\":");
  Serial.print(isnan(temp7C) ? -999.0 : temp7C, 2);
  Serial.print("},");
  
  Serial.print("\"sensor8\":{\"celsius\":");
  Serial.print(isnan(temp8C) ? -999.0 : temp8C, 2);
  Serial.print("},");
  
  Serial.print("\"voltage1\":{\"volts\":");
  Serial.print(isnan(voltage1) ? -999.0 : voltage1, 4);
  Serial.print("},");
  
  Serial.print("\"voltage2\":{\"volts\":");
  Serial.print(isnan(voltage2) ? -999.0 : voltage2, 4);
  Serial.print("},");
  
  Serial.print("\"voltage3\":{\"volts\":");
  Serial.print(isnan(voltage3) ? -999.0 : voltage3, 4);
  Serial.print("},");
  
  Serial.print("\"voltage4\":{\"volts\":");
  Serial.print(isnan(voltage4) ? -999.0 : voltage4, 4);
  Serial.print("}");
  
  Serial.println("}");
}

// Function to process VISA/SCPI commands
void processVisaCommand(String command) {
  command.trim();
  command.toUpperCase();
  
  // Standard SCPI identification query
  if (command == "*IDN?") {
    Serial.println(INSTRUMENT_ID);
    return;
  }
  
  // Reset command
  if (command == "*RST") {
    calibrationMode = false;
    labviewMode = false;
    visaMode = true;
    Serial.println("OK");
    return;
  }
  
  // Temperature measurement queries
  if (command == "MEAS:TEMP? ALL") {
    // Return all temperatures in CSV format
    float temp1C = readCalibratedCelsius(thermocouple1, calibrationOffset1);
    float temp2C = readCalibratedCelsius(thermocouple2, calibrationOffset2);
    float temp3C = readCalibratedCelsius(thermocouple3, calibrationOffset3);
    float temp4C = readCalibratedCelsius(thermocouple4, calibrationOffset4);
    float temp5C = readCalibratedCelsius(thermocouple5, calibrationOffset5);
    float temp6C = readCalibratedCelsius(thermocouple6, calibrationOffset6);
    float temp7C = readCalibratedCelsius(thermocouple7, calibrationOffset7);
    float temp8C = readCalibratedCelsius(thermocouple8, calibrationOffset8);
    
    // Format each temperature with exactly 2 decimal places
    Serial.print(isnan(temp1C) ? "-999.00" : String(temp1C, 2));
    Serial.print(",");
    Serial.print(isnan(temp2C) ? "-999.00" : String(temp2C, 2));
    Serial.print(",");
    Serial.print(isnan(temp3C) ? "-999.00" : String(temp3C, 2));
    Serial.print(",");
    Serial.print(isnan(temp4C) ? "-999.00" : String(temp4C, 2));
    Serial.print(",");
    Serial.print(isnan(temp5C) ? "-999.00" : String(temp5C, 2));
    Serial.print(",");
    Serial.print(isnan(temp6C) ? "-999.00" : String(temp6C, 2));
    Serial.print(",");
    Serial.print(isnan(temp7C) ? "-999.00" : String(temp7C, 2));
    Serial.print(",");
    Serial.print(isnan(temp8C) ? "-999.00" : String(temp8C, 2));
    Serial.println();
    return;
  }
  
  // Individual sensor queries
  if (command.startsWith("MEAS:TEMP? CH")) {
    int channel = command.substring(13).toInt();
    if (channel >= 1 && channel <= 8) {
      float temp = NAN;
      switch(channel) {
        case 1: temp = readCalibratedCelsius(thermocouple1, calibrationOffset1); break;
        case 2: temp = readCalibratedCelsius(thermocouple2, calibrationOffset2); break;
        case 3: temp = readCalibratedCelsius(thermocouple3, calibrationOffset3); break;
        case 4: temp = readCalibratedCelsius(thermocouple4, calibrationOffset4); break;
        case 5: temp = readCalibratedCelsius(thermocouple5, calibrationOffset5); break;
        case 6: temp = readCalibratedCelsius(thermocouple6, calibrationOffset6); break;
        case 7: temp = readCalibratedCelsius(thermocouple7, calibrationOffset7); break;
        case 8: temp = readCalibratedCelsius(thermocouple8, calibrationOffset8); break;
      }
      // Format with exactly 2 decimal places
      if (isnan(temp)) {
        Serial.println("-999.00");
      } else {
        Serial.println(String(temp, 2));
      }
      return;
    } else {
      Serial.println("ERROR: Invalid channel number (1-8)");
      return;
    }
  }
  
  // Voltage measurement queries
  if (command == "MEAS:VOLT? ALL") {
    // Return all voltages in CSV format
    float voltage1 = readCalibratedVoltage(0, voltageOffset1);
    float voltage2 = readCalibratedVoltage(1, voltageOffset2);
    float voltage3 = readCalibratedVoltage(2, voltageOffset3);
    float voltage4 = readCalibratedVoltage(3, voltageOffset4);
    
    // Format each voltage with exactly 4 decimal places
    Serial.print(isnan(voltage1) ? "-999.0000" : String(voltage1, 4));
    Serial.print(",");
    Serial.print(isnan(voltage2) ? "-999.0000" : String(voltage2, 4));
    Serial.print(",");
    Serial.print(isnan(voltage3) ? "-999.0000" : String(voltage3, 4));
    Serial.print(",");
    Serial.print(isnan(voltage4) ? "-999.0000" : String(voltage4, 4));
    Serial.println();
    return;
  }
  
  // Individual voltage channel queries
  if (command.startsWith("MEAS:VOLT? CH")) {
    int channel = command.substring(13).toInt();
    if (channel >= 1 && channel <= 4) {
      float voltage = readCalibratedVoltage(channel - 1, 
        (channel == 1) ? voltageOffset1 :
        (channel == 2) ? voltageOffset2 :
        (channel == 3) ? voltageOffset3 : voltageOffset4);
      
      // Format with exactly 4 decimal places
      if (isnan(voltage)) {
        Serial.println("-999.0000");
      } else {
        Serial.println(String(voltage, 4));
      }
      return;
    } else {
      Serial.println("ERROR: Invalid voltage channel number (1-4)");
      return;
    }
  }
  
  // Combined temperature and voltage measurement
  if (command == "MEAS:ALL?") {
    // Return all temperatures and voltages in CSV format
    float temp1C = readCalibratedCelsius(thermocouple1, calibrationOffset1);
    float temp2C = readCalibratedCelsius(thermocouple2, calibrationOffset2);
    float temp3C = readCalibratedCelsius(thermocouple3, calibrationOffset3);
    float temp4C = readCalibratedCelsius(thermocouple4, calibrationOffset4);
    float temp5C = readCalibratedCelsius(thermocouple5, calibrationOffset5);
    float temp6C = readCalibratedCelsius(thermocouple6, calibrationOffset6);
    float temp7C = readCalibratedCelsius(thermocouple7, calibrationOffset7);
    float temp8C = readCalibratedCelsius(thermocouple8, calibrationOffset8);
    
    float voltage1 = readCalibratedVoltage(0, voltageOffset1);
    float voltage2 = readCalibratedVoltage(1, voltageOffset2);
    float voltage3 = readCalibratedVoltage(2, voltageOffset3);
    float voltage4 = readCalibratedVoltage(3, voltageOffset4);
    
    // Format: T1,T2,T3,T4,T5,T6,T7,T8,V1,V2,V3,V4
    Serial.print(isnan(temp1C) ? "-999.00" : String(temp1C, 2)); Serial.print(",");
    Serial.print(isnan(temp2C) ? "-999.00" : String(temp2C, 2)); Serial.print(",");
    Serial.print(isnan(temp3C) ? "-999.00" : String(temp3C, 2)); Serial.print(",");
    Serial.print(isnan(temp4C) ? "-999.00" : String(temp4C, 2)); Serial.print(",");
    Serial.print(isnan(temp5C) ? "-999.00" : String(temp5C, 2)); Serial.print(",");
    Serial.print(isnan(temp6C) ? "-999.00" : String(temp6C, 2)); Serial.print(",");
    Serial.print(isnan(temp7C) ? "-999.00" : String(temp7C, 2)); Serial.print(",");
    Serial.print(isnan(temp8C) ? "-999.00" : String(temp8C, 2)); Serial.print(",");
    Serial.print(isnan(voltage1) ? "-999.0000" : String(voltage1, 4)); Serial.print(",");
    Serial.print(isnan(voltage2) ? "-999.0000" : String(voltage2, 4)); Serial.print(",");
    Serial.print(isnan(voltage3) ? "-999.0000" : String(voltage3, 4)); Serial.print(",");
    Serial.print(isnan(voltage4) ? "-999.0000" : String(voltage4, 4));
    Serial.println();
    return;
  }

  // System status queries
  if (command == "SYST:ERR?") {
    Serial.println("0,\"No error\"");
    return;
  }
  
  if (command == "SYST:VERS?") {
    Serial.println("1.0");
    return;
  }
  
  // Configuration queries
  if (command == "CONF:SENS:COUN?") {
    Serial.println("8");
    return;
  }
  
  if (command == "CONF:VOLT:COUN?") {
    Serial.println("4");
    return;
  }
  
  if (command == "CONF:RATE?") {
    Serial.println("1.0");  // 1 Hz update rate
    return;
  }
  
  // Relay control commands
  if (command.startsWith("RELAY:SET ")) {
    // Format: RELAY:SET R1,ON or RELAY:SET R1,OFF or RELAY:SET ALL,ON
    String params = command.substring(10); // Remove "RELAY:SET "
    int commaIndex = params.indexOf(',');
    if (commaIndex > 0) {
      String relayStr = params.substring(0, commaIndex);
      String stateStr = params.substring(commaIndex + 1);
      stateStr.trim();
      
      bool state = (stateStr == "ON" || stateStr == "1");
      
      if (relayStr == "ALL") {
        setAllRelays(state);
        Serial.println("OK");
      } else if (relayStr.startsWith("R") && relayStr.length() == 2) {
        int relayNum = relayStr.substring(1).toInt();
        if (relayNum >= 1 && relayNum <= 8) {
          setRelayState(relayNum, state);
          Serial.println("OK");
        } else {
          Serial.println("ERROR: Invalid relay number (1-8)");
        }
      } else {
        Serial.println("ERROR: Invalid relay format (use R1-R8 or ALL)");
      }
    } else {
      Serial.println("ERROR: Invalid command format (use RELAY:SET R1,ON)");
    }
    return;
  }
  
  if (command.startsWith("RELAY:GET? ")) {
    // Format: RELAY:GET? R1 or RELAY:GET? ALL
    String relayStr = command.substring(11); // Remove "RELAY:GET? "
    relayStr.trim();
    
    if (relayStr == "ALL") {
      for(int i = 1; i <= 8; i++) {
        Serial.print(getRelayState(i) ? "1" : "0");
        if (i < 8) Serial.print(",");
      }
      Serial.println();
    } else if (relayStr.startsWith("R") && relayStr.length() == 2) {
      int relayNum = relayStr.substring(1).toInt();
      if (relayNum >= 1 && relayNum <= 8) {
        Serial.println(getRelayState(relayNum) ? "1" : "0");
      } else {
        Serial.println("ERROR: Invalid relay number (1-8)");
      }
    } else {
      Serial.println("ERROR: Invalid relay format (use R1-R8 or ALL)");
    }
    return;
  }
  
  if (command == "RELAY:COUNT?") {
    Serial.println("8");
    return;
  }
  
  // Raw temperature measurement queries (uncalibrated)
  if (command == "MEAS:TEMP:RAW? ALL") {
    // Return all raw temperatures (no calibration offset)
    double temp1 = thermocouple1.readCelsius();
    delay(100);
    double temp2 = thermocouple2.readCelsius();
    delay(100);
    double temp3 = thermocouple3.readCelsius();
    delay(100);
    double temp4 = thermocouple4.readCelsius();
    delay(100);
    double temp5 = thermocouple5.readCelsius();
    delay(100);
    double temp6 = thermocouple6.readCelsius();
    delay(100);
    double temp7 = thermocouple7.readCelsius();
    delay(100);
    double temp8 = thermocouple8.readCelsius();
    
    Serial.print(isnan(temp1) ? "-999.00" : String(temp1, 2));
    Serial.print(",");
    Serial.print(isnan(temp2) ? "-999.00" : String(temp2, 2));
    Serial.print(",");
    Serial.print(isnan(temp3) ? "-999.00" : String(temp3, 2));
    Serial.print(",");
    Serial.print(isnan(temp4) ? "-999.00" : String(temp4, 2));
    Serial.print(",");
    Serial.print(isnan(temp5) ? "-999.00" : String(temp5, 2));
    Serial.print(",");
    Serial.print(isnan(temp6) ? "-999.00" : String(temp6, 2));
    Serial.print(",");
    Serial.print(isnan(temp7) ? "-999.00" : String(temp7, 2));
    Serial.print(",");
    Serial.print(isnan(temp8) ? "-999.00" : String(temp8, 2));
    Serial.println();
    return;
  }
  
  // Individual raw sensor queries
  if (command.startsWith("MEAS:TEMP:RAW? CH")) {
    int channel = command.substring(18).toInt();
    if (channel >= 1 && channel <= 8) {
      double temp = NAN;
      switch(channel) {
        case 1: temp = thermocouple1.readCelsius(); break;
        case 2: temp = thermocouple2.readCelsius(); break;
        case 3: temp = thermocouple3.readCelsius(); break;
        case 4: temp = thermocouple4.readCelsius(); break;
        case 5: temp = thermocouple5.readCelsius(); break;
        case 6: temp = thermocouple6.readCelsius(); break;
        case 7: temp = thermocouple7.readCelsius(); break;
        case 8: temp = thermocouple8.readCelsius(); break;
      }
      if (isnan(temp)) {
        Serial.println("-999.00");
      } else {
        Serial.println(String(temp, 2));
      }
      return;
    } else {
      Serial.println("ERROR: Invalid channel number (1-8)");
      return;
    }
  }
  
  // Mode control
  if (command == "MODE:VISA" || command == "VISA" || command == "VSON") {
    visaMode = true;
    labviewMode = false;
    calibrationMode = false;
    Serial.println("OK");
    return;
  }
  
  if (command == "MODE:LABVIEW" || command == "LABVIEW" || command == "LVON" || command == "CSV") {
    visaMode = false;
    labviewMode = true;
    calibrationMode = false;
    Serial.println("OK");
    return;
  }
  
  if (command == "MODE:HUMAN" || command == "HUMAN" || command == "LVOFF") {
    visaMode = false;
    labviewMode = false;
    calibrationMode = false;
    Serial.println("OK");
    return;
  }
  
  if (command == "MODE:CAL" || command == "CAL") {
    visaMode = false;
    labviewMode = false;
    calibrationMode = true;
    Serial.println("OK");
    return;
  }
  
  if (command == "EXIT") {
    calibrationMode = false;
    labviewMode = false;
    visaMode = false;
    Serial.println("OK - Exiting to human mode");
    return;
  }
  
  // Help command
  if (command == "HELP?" || command == "?") {
    Serial.println("Available VISA Commands:");
    Serial.println("*IDN? - Instrument identification");
    Serial.println("*RST - Reset to VISA mode");
    Serial.println("MEAS:TEMP? ALL - Read all temperatures (calibrated)");
    Serial.println("MEAS:TEMP? CH<n> - Read channel n (1-8, calibrated)");
    Serial.println("MEAS:TEMP:RAW? ALL - Read all temperatures (raw)");
    Serial.println("MEAS:TEMP:RAW? CH<n> - Read channel n (1-8, raw)");
    Serial.println("MEAS:VOLT? ALL - Read all voltages (calibrated)");
    Serial.println("MEAS:VOLT? CH<n> - Read voltage channel n (1-4, calibrated)");
    Serial.println("MEAS:ALL? - Read all temperatures and voltages");
    Serial.println("RELAY:SET R<n>,ON/OFF - Set relay n ON/OFF (1-8)");
    Serial.println("RELAY:SET ALL,ON/OFF - Set all relays ON/OFF");
    Serial.println("RELAY:GET? R<n> - Get relay n state (1-8)");
    Serial.println("RELAY:GET? ALL - Get all relay states");
    Serial.println("RELAY:COUNT? - Number of relays");
    Serial.println("SYST:ERR? - System error query");
    Serial.println("SYST:VERS? - System version");
    Serial.println("CONF:SENS:COUN? - Sensor count");
    Serial.println("CONF:VOLT:COUN? - Voltage channel count");
    Serial.println("CONF:RATE? - Update rate");
    Serial.println("MODE:VISA, VISA, VSON - Enable VISA mode");
    Serial.println("MODE:LABVIEW, LABVIEW, LVON, CSV - Enable LabVIEW mode");
    Serial.println("MODE:HUMAN, HUMAN, LVOFF - Enable human mode");
    Serial.println("MODE:CAL, CAL - Enter calibration mode");
    Serial.println("EXIT - Exit to human mode");
    Serial.println("HELP? - This help message");
    return;
  }
  
  // Unknown command
  Serial.println("ERROR: Unknown command");
}

// Function to handle VISA serial communication
void handleVisaSerial() {
  while (Serial.available()) {
    char c = Serial.read();
    
    if (c == '\n' || c == '\r') {
      if (commandBuffer.length() > 0) {
        commandReady = true;
        break;
      }
    } else {
      commandBuffer += c;
    }
  }
  
  if (commandReady) {
    processVisaCommand(commandBuffer);
    commandBuffer = "";
    commandReady = false;
  }
}

void setup() {
  Serial.begin(9600);
  
  // Initialize I2C for ADS1115
  Wire.begin();
  
  // Initialize ADS1115 ADC
  initializeADS1115();
  
  Serial.println("MAX6675 Multiple Type K Thermocouple Test with ADS1115 ADC");
  Serial.println("Reading from 8 thermocouples and 4 voltage inputs...");
  Serial.println("Current calibration offsets:");
  Serial.print("Sensor 1: "); Serial.print(calibrationOffset1); Serial.println("°C");
  Serial.print("Sensor 2: "); Serial.print(calibrationOffset2); Serial.println("°C");
  Serial.print("Sensor 3: "); Serial.print(calibrationOffset3); Serial.println("°C");
  Serial.print("Sensor 4: "); Serial.print(calibrationOffset4); Serial.println("°C");
  Serial.print("Sensor 5: "); Serial.print(calibrationOffset5); Serial.println("°C");
  Serial.print("Sensor 6: "); Serial.print(calibrationOffset6); Serial.println("°C");
  Serial.print("Sensor 7: "); Serial.print(calibrationOffset7); Serial.println("°C");
  Serial.print("Sensor 8: "); Serial.print(calibrationOffset8); Serial.println("°C");
  Serial.println("Voltage calibration offsets:");
  Serial.print("Channel A0: "); Serial.print(voltageOffset1); Serial.println("V");
  Serial.print("Channel A1: "); Serial.print(voltageOffset2); Serial.println("V");
  Serial.print("Channel A2: "); Serial.print(voltageOffset3); Serial.println("V");
  Serial.print("Channel A3: "); Serial.print(voltageOffset4); Serial.println("V");
  Serial.println("\n=== DEFAULT MODE: CSV Data Stream ===");
  Serial.println("Outputting temperature and voltage data in CSV format every 5 seconds");
  Serial.println("Format: S1_C,S2_C,S3_C,S4_C,S5_C,S6_C,S7_C,S8_C,V1,V2,V3,V4");
  Serial.println("Error values shown as -999.00");
  Serial.println("\nAvailable Commands:");
  Serial.println("  CAL    - Enter calibration mode");
  Serial.println("  DEBUG  - Test individual sensors");
  Serial.println("  TEST   - Test CSV output immediately");
  Serial.println("  EXIT   - Exit current mode");
  Serial.println("  LABVIEW - Toggle LabVIEW output format");
  Serial.println("  CSV    - Enable CSV output for LabVIEW");
  Serial.println("  JSON   - Enable JSON output for LabVIEW");
  Serial.println("  HUMAN  - Enable human-readable output");
  Serial.println("  VISA   - Enable VISA command-response mode");
  Serial.println("  VSON   - Enable VISA mode (alias)");
  Serial.println("Waiting for sensors to stabilize...");
  delay(500); // Wait for MAX6675 to stabilize
}

void loop() {
  // Handle VISA mode separately with command-response protocol
  if (visaMode) {
    handleVisaSerial();
    return;  // Don't do continuous output in VISA mode
  }
  
  // Check for serial commands
  if (Serial.available() > 0) {
    String command = Serial.readString();
    command.trim();
    command.toUpperCase();
    
    if (command == "CAL") {
      enterCalibrationMode();
    } else if (command == "DEBUG" || command == "TEST") {
      testIndividualSensors();
    } else if (command == "EXIT") {
      calibrationMode = false;
      labviewMode = false;
      visaMode = false;
      Serial.println("Exiting current mode - returning to human-readable output\n");
    } else if (command == "LVON" || command == "LABVIEW" || command == "CSV") {
      labviewMode = true;
      calibrationMode = false;
      visaMode = false;
      Serial.println("OK");
      //Serial.println("LabVIEW CSV mode enabled");
      //Serial.println("Format: S1_C,S2_C,S3_C,S4_C,S5_C,S6_C,S7_C,S8_C,V1,V2,V3,V4");
      //Serial.println("Error values represented as -999.00");
      delay(1000);
    } else if (command == "TEST" || command == "TESTCSV") {
      // Immediate test output
      Serial.println("TEST OUTPUT:");
      outputLabVIEWFormat();
      Serial.println("TEST COMPLETE");
    } else if (command == "LVOFF" || command == "HUMAN") {
      labviewMode = false;
      calibrationMode = false;
      visaMode = false;
      Serial.println("Human-readable mode enabled\n");
    } else if (command == "VISA" || command == "VSON") {
      visaMode = true;
      labviewMode = false;
      calibrationMode = false;
      Serial.println("VISA command-response mode enabled");
      Serial.println("Send '*IDN?' to identify device or 'HELP?' for commands");
      return;
    } else if (command.startsWith("RELAY")) {
      // Handle relay commands in non-VISA modes
      if (command.startsWith("RELAY") && command.length() > 5) {
        String relayCmd = command.substring(5); // Remove "RELAY"
        relayCmd.trim();
        
        if (relayCmd.startsWith("ON") || relayCmd.startsWith("OFF")) {
          // Format: RELAYON1 or RELAYOFF1 or RELAYON or RELAYOFF (for all)
          bool state = relayCmd.startsWith("ON");
          String numStr = relayCmd.substring(state ? 2 : 3);
          
          if (numStr.length() == 0) {
            // All relays
            setAllRelays(state);
            Serial.print("All relays turned ");
            Serial.println(state ? "ON" : "OFF");
          } else {
            int relayNum = numStr.toInt();
            if (relayNum >= 1 && relayNum <= 8) {
              setRelayState(relayNum, state);
              Serial.print("Relay ");
              Serial.print(relayNum);
              Serial.print(" turned ");
              Serial.println(state ? "ON" : "OFF");
            } else {
              Serial.println("Invalid relay number (1-8)");
            }
          }
        } else if (relayCmd == "STATUS") {
          displayRelayStatus();
        }
      }
    }
  }
  
  if (calibrationMode) {
    displayCalibrationReadings();
    delay(3000); // Slower readings in calibration mode
    return;
  }
  
  if (labviewMode) {
    // Output only LabVIEW-compatible format
    outputLabVIEWFormat();
    delay(5000); // 5-second intervals for CSV data stream
    return;
  }
  
  // Normal human-readable operation with calibrated readings
  Serial.println("=== CALIBRATED Temperature Readings ===");
  
  // Read from Sensor 1 (calibrated)
  float temp1C = readCalibratedCelsius(thermocouple1, calibrationOffset1);
  if (isnan(temp1C)) {
    Serial.println("Sensor 1: Error reading thermocouple!");
  } else {
    Serial.print("Sensor 1: ");
    Serial.print(temp1C, 2);
    Serial.println("°C");
  }
  
  delay(100);
  
  // Read from Sensor 2 (calibrated)
  float temp2C = readCalibratedCelsius(thermocouple2, calibrationOffset2);
  if (isnan(temp2C)) {
    Serial.println("Sensor 2: Error reading thermocouple!");
  } else {
    Serial.print("Sensor 2: ");
    Serial.print(temp2C, 2);
    Serial.println("°C");
  }
  
  delay(100);
  
  // Read from Sensor 3 (calibrated)
  float temp3C = readCalibratedCelsius(thermocouple3, calibrationOffset3);
  if (isnan(temp3C)) {
    Serial.println("Sensor 3: Error reading thermocouple!");
  } else {
    Serial.print("Sensor 3: ");
    Serial.print(temp3C, 2);
    Serial.println("°C");
  }
  
  delay(100);
  
  // Read from Sensor 4 (calibrated)
  float temp4C = readCalibratedCelsius(thermocouple4, calibrationOffset4);
  if (isnan(temp4C)) {
    Serial.println("Sensor 4: Error reading thermocouple!");
  } else {
    Serial.print("Sensor 4: ");
    Serial.print(temp4C, 2);
    Serial.println("°C");
  }
  
  delay(100);
  
  // Read from Sensor 5 (calibrated)
  float temp5C = readCalibratedCelsius(thermocouple5, calibrationOffset5);
  if (isnan(temp5C)) {
    Serial.println("Sensor 5: Error reading thermocouple!");
  } else {
    Serial.print("Sensor 5: ");
    Serial.print(temp5C, 2);
    Serial.println("°C");
  }
  
  delay(100);
  
  // Read from Sensor 6 (calibrated)
  float temp6C = readCalibratedCelsius(thermocouple6, calibrationOffset6);
  if (isnan(temp6C)) {
    Serial.println("Sensor 6: Error reading thermocouple!");
  } else {
    Serial.print("Sensor 6: ");
    Serial.print(temp6C, 2);
    Serial.println("°C");
  }
  
  delay(100);
  
  // Read from Sensor 7 (calibrated)
  float temp7C = readCalibratedCelsius(thermocouple7, calibrationOffset7);
  if (isnan(temp7C)) {
    Serial.println("Sensor 7: Error reading thermocouple!");
  } else {
    Serial.print("Sensor 7: ");
    Serial.print(temp7C, 2);
    Serial.println("°C");
  }
  
  delay(100);
  
  // Read from Sensor 8 (calibrated)
  float temp8C = readCalibratedCelsius(thermocouple8, calibrationOffset8);
  if (isnan(temp8C)) {
    Serial.println("Sensor 8: Error reading thermocouple!");
  } else {
    Serial.print("Sensor 8: ");
    Serial.print(temp8C, 2);
    Serial.println("°C");
  }
  
  Serial.println("\n=== VOLTAGE Readings (ADS1115 ADC) ===");
  
  // Read from Voltage Channel 1 (A0)
  float voltage1 = readCalibratedVoltage(0, voltageOffset1);
  if (isnan(voltage1)) {
    Serial.println("Channel A0: Error reading voltage!");
  } else {
    Serial.print("Channel A0: ");
    Serial.print(voltage1, 4);
    Serial.println("V");
  }
  
  delay(100);
  
  // Read from Voltage Channel 2 (A1)
  float voltage2 = readCalibratedVoltage(1, voltageOffset2);
  if (isnan(voltage2)) {
    Serial.println("Channel A1: Error reading voltage!");
  } else {
    Serial.print("Channel A1: ");
    Serial.print(voltage2, 4);
    Serial.println("V");
  }
  
  delay(100);
  
  // Read from Voltage Channel 3 (A2)
  float voltage3 = readCalibratedVoltage(2, voltageOffset3);
  if (isnan(voltage3)) {
    Serial.println("Channel A2: Error reading voltage!");
  } else {
    Serial.print("Channel A2: ");
    Serial.print(voltage3, 4);
    Serial.println("V");
  }
  
  delay(100);
  
  // Read from Voltage Channel 4 (A3)
  float voltage4 = readCalibratedVoltage(3, voltageOffset4);
  if (isnan(voltage4)) {
    Serial.println("Channel A3: Error reading voltage!");
  } else {
    Serial.print("Channel A3: ");
    Serial.print(voltage4, 4);
    Serial.println("V");
  }

  Serial.println(); // Empty line for readability
  delay(5000); // Read every 5 seconds
}

// Debug function to test individual sensors
void testIndividualSensors() {
  Serial.println("=== INDIVIDUAL SENSOR TEST ===");
  
  Serial.print("Sensor 1 (Pin 53): ");
  float temp1 = readCalibratedCelsius(thermocouple1, calibrationOffset1);
  Serial.print(isnan(temp1) ? "ERROR" : String(temp1, 2) + "°C");
  Serial.println();
  delay(250);
  
  Serial.print("Sensor 2 (Pin 49): ");
  float temp2 = readCalibratedCelsius(thermocouple2, calibrationOffset2);
  Serial.print(isnan(temp2) ? "ERROR" : String(temp2, 2) + "°C");
  Serial.println();
  delay(250);
  
  Serial.print("Sensor 3 (Pin 48): ");
  float temp3 = readCalibratedCelsius(thermocouple3, calibrationOffset3);
  Serial.print(isnan(temp3) ? "ERROR" : String(temp3, 2) + "°C");
  Serial.println();
  delay(250);
  
  Serial.print("Sensor 4 (Pin 47): ");
  float temp4 = readCalibratedCelsius(thermocouple4, calibrationOffset4);
  Serial.print(isnan(temp4) ? "ERROR" : String(temp4, 2) + "°C");
  Serial.println();
  delay(250);
  
  Serial.print("Sensor 5 (Pin 46): ");
  float temp5 = readCalibratedCelsius(thermocouple5, calibrationOffset5);
  Serial.print(isnan(temp5) ? "ERROR" : String(temp5, 2) + "°C");
  Serial.println();
  delay(250);
  
  Serial.print("Sensor 6 (Pin 45): ");
  float temp6 = readCalibratedCelsius(thermocouple6, calibrationOffset6);
  Serial.print(isnan(temp6) ? "ERROR" : String(temp6, 2) + "°C");
  Serial.println();
  delay(250);
  
  Serial.print("Sensor 7 (Pin 44): ");
  float temp7 = readCalibratedCelsius(thermocouple7, calibrationOffset7);
  Serial.print(isnan(temp7) ? "ERROR" : String(temp7, 2) + "°C");
  Serial.println();
  delay(250);
  
  Serial.print("Sensor 8 (Pin 43): ");
  float temp8 = readCalibratedCelsius(thermocouple8, calibrationOffset8);
  Serial.print(isnan(temp8) ? "ERROR" : String(temp8, 2) + "°C");
  Serial.println();
  
  Serial.println("===================================");
}