#include <Arduino.h>
#include "max6675.h"

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

// Calibration offsets for each sensor (adjust these values)
float calibrationOffset1 = -0.06;  // Sensor 1 offset in °C
float calibrationOffset2 = -4.31;  // Sensor 2 offset in °C
float calibrationOffset3 = -1.81;  // Sensor 3 offset in °C
float calibrationOffset4 = 0.69;  // Sensor 4 offset in °C
float calibrationOffset5 = 3.69;  // Sensor 5 offset in °C
float calibrationOffset6 = -0.06;  // Sensor 6 offset in °C
float calibrationOffset7 = -0.31;  // Sensor 7 offset in °C
float calibrationOffset8 = 2.19;  // Sensor 8 offset in °C

// Create multiple MAX6675 objects
MAX6675 thermocouple1(thermoCLK, thermoCS1, thermoDO);  // Sensor 1
MAX6675 thermocouple2(thermoCLK, thermoCS2, thermoDO);  // Sensor 2
MAX6675 thermocouple3(thermoCLK, thermoCS3, thermoDO);  // Sensor 3
MAX6675 thermocouple4(thermoCLK, thermoCS4, thermoDO);  // Sensor 4
MAX6675 thermocouple5(thermoCLK, thermoCS5, thermoDO);  // Sensor 5
MAX6675 thermocouple6(thermoCLK, thermoCS6, thermoDO);  // Sensor 6
MAX6675 thermocouple7(thermoCLK, thermoCS7, thermoDO);  // Sensor 7
MAX6675 thermocouple8(thermoCLK, thermoCS8, thermoDO);  // Sensor 8

// VISA command buffer
String commandBuffer = "";
bool commandReady = false;

// VISA instrument identification
const String INSTRUMENT_ID = "MAX6675_THERMOCOUPLE_READER,v2.0,SN001";

// Function prototypes
void testIndividualSensors();

// Function to apply calibration offset
float applyCalibratedReading(float rawTemp, float offset) {
  return rawTemp + offset;
}

// Function to read calibrated temperature
float readCalibratedCelsius(MAX6675 &sensor, float offset) {
  double rawTemp = sensor.readCelsius();
  if (isnan(rawTemp)) {
    return NAN;
  }
  return applyCalibratedReading(rawTemp, offset);
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
  
  if (command == "CONF:RATE?") {
    Serial.println("1.0");  // 1 Hz update rate
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
   // Debug/Test command
  if (command == "DEBUG" || command == "TEST") {
    testIndividualSensors();
    Serial.println("OK");
    return;
  }

  // Help command
  if (command == "HELP?" || command == "?") {
    Serial.println("Available VISA Commands:");
    Serial.println("*IDN? - Instrument identification");
    Serial.println("*RST - Reset device");
    Serial.println("MEAS:TEMP? ALL - Read all temperatures (calibrated)");
    Serial.println("MEAS:TEMP? CH<n> - Read channel n (1-8, calibrated)");
    Serial.println("MEAS:TEMP:RAW? ALL - Read all temperatures (raw)");
    Serial.println("MEAS:TEMP:RAW? CH<n> - Read channel n (1-8, raw)");
    Serial.println("SYST:ERR? - System error query");
    Serial.println("SYST:VERS? - System version");
    Serial.println("CONF:SENS:COUN? - Sensor count");
    Serial.println("CONF:RATE? - Update rate");
    Serial.println("DEBUG, TEST - Test individual sensors");
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
  Serial.println("MAX6675 VISA Thermocouple Interface v2.1");
  Serial.println("8-Channel Type K Thermocouple Reader - VISA Mode Only");
  Serial.println("Current calibration offsets:");
  Serial.print("Sensor 1: "); Serial.print(calibrationOffset1); Serial.println("°C");
  Serial.print("Sensor 2: "); Serial.print(calibrationOffset2); Serial.println("°C");
  Serial.print("Sensor 3: "); Serial.print(calibrationOffset3); Serial.println("°C");
  Serial.print("Sensor 4: "); Serial.print(calibrationOffset4); Serial.println("°C");
  Serial.print("Sensor 5: "); Serial.print(calibrationOffset5); Serial.println("°C");
  Serial.print("Sensor 6: "); Serial.print(calibrationOffset6); Serial.println("°C");
  Serial.print("Sensor 7: "); Serial.print(calibrationOffset7); Serial.println("°C");
  Serial.print("Sensor 8: "); Serial.print(calibrationOffset8); Serial.println("°C");
  Serial.println("\n=== VISA Command-Response Mode ===");
  Serial.println("Send '*IDN?' to identify device or 'HELP?' for all commands");
  Serial.println("Ready for VISA commands...");
  delay(500); // Wait for MAX6675 to stabilize
}

void loop() {
  // Handle VISA command-response protocol
  handleVisaSerial();
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