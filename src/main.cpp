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
float calibrationOffset1 = 0.0;  // Sensor 1 offset in °C
float calibrationOffset2 = 0.0;  // Sensor 2 offset in °C
float calibrationOffset3 = 0.0;  // Sensor 3 offset in °C
float calibrationOffset4 = 0.0;  // Sensor 4 offset in °C
float calibrationOffset5 = 0.0;  // Sensor 5 offset in °C
float calibrationOffset6 = 0.0;  // Sensor 6 offset in °C
float calibrationOffset7 = 0.0;  // Sensor 7 offset in °C
float calibrationOffset8 = 0.0;  // Sensor 8 offset in °C

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

// Function to read calibrated temperature
float readCalibratedCelsius(MAX6675 &sensor, float offset) {
  double rawTemp = sensor.readCelsius();
  if (isnan(rawTemp)) {
    return NAN;
  }
  return applyCalibratedReading(rawTemp, offset);
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
  
  // Read all sensors without calibration
  double temp1 = thermocouple1.readCelsius();
  double temp2 = thermocouple2.readCelsius();
  double temp3 = thermocouple3.readCelsius();
  double temp4 = thermocouple4.readCelsius();
  double temp5 = thermocouple5.readCelsius();
  double temp6 = thermocouple6.readCelsius();
  double temp7 = thermocouple7.readCelsius();
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
  // Read all sensors
  float temp1C = readCalibratedCelsius(thermocouple1, calibrationOffset1);
  float temp2C = readCalibratedCelsius(thermocouple2, calibrationOffset2);
  float temp3C = readCalibratedCelsius(thermocouple3, calibrationOffset3);
  float temp4C = readCalibratedCelsius(thermocouple4, calibrationOffset4);
  float temp5C = readCalibratedCelsius(thermocouple5, calibrationOffset5);
  float temp6C = readCalibratedCelsius(thermocouple6, calibrationOffset6);
  float temp7C = readCalibratedCelsius(thermocouple7, calibrationOffset7);
  float temp8C = readCalibratedCelsius(thermocouple8, calibrationOffset8);
  
  // Output in CSV format: S1_C,S2_C,S3_C,S4_C,S5_C,S6_C,S7_C,S8_C
  Serial.print(isnan(temp1C) ? -999.0 : temp1C, 2); Serial.print(",");
  Serial.print(isnan(temp2C) ? -999.0 : temp2C, 2); Serial.print(",");
  Serial.print(isnan(temp3C) ? -999.0 : temp3C, 2); Serial.print(",");
  Serial.print(isnan(temp4C) ? -999.0 : temp4C, 2); Serial.print(",");
  Serial.print(isnan(temp5C) ? -999.0 : temp5C, 2); Serial.print(",");
  Serial.print(isnan(temp6C) ? -999.0 : temp6C, 2); Serial.print(",");
  Serial.print(isnan(temp7C) ? -999.0 : temp7C, 2); Serial.print(",");
  Serial.print(isnan(temp8C) ? -999.0 : temp8C, 2);
  Serial.println(); // End line
}

// Function to output data in JSON format (alternative for LabVIEW) - Celsius only
void outputJSONFormat() {
  Serial.print("{");
  
  float temp1C = readCalibratedCelsius(thermocouple1, calibrationOffset1);
  float temp2C = readCalibratedCelsius(thermocouple2, calibrationOffset2);
  float temp3C = readCalibratedCelsius(thermocouple3, calibrationOffset3);
  float temp4C = readCalibratedCelsius(thermocouple4, calibrationOffset4);
  float temp5C = readCalibratedCelsius(thermocouple5, calibrationOffset5);
  float temp6C = readCalibratedCelsius(thermocouple6, calibrationOffset6);
  float temp7C = readCalibratedCelsius(thermocouple7, calibrationOffset7);
  float temp8C = readCalibratedCelsius(thermocouple8, calibrationOffset8);
  
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
    double temp2 = thermocouple2.readCelsius();
    double temp3 = thermocouple3.readCelsius();
    double temp4 = thermocouple4.readCelsius();
    double temp5 = thermocouple5.readCelsius();
    double temp6 = thermocouple6.readCelsius();
    double temp7 = thermocouple7.readCelsius();
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
    Serial.println("SYST:ERR? - System error query");
    Serial.println("SYST:VERS? - System version");
    Serial.println("CONF:SENS:COUN? - Sensor count");
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
  Serial.println("MAX6675 Multiple Type K Thermocouple Test");
  Serial.println("Reading from 8 sensors...");
  Serial.println("Current calibration offsets:");
  Serial.print("Sensor 1: "); Serial.print(calibrationOffset1); Serial.println("°C");
  Serial.print("Sensor 2: "); Serial.print(calibrationOffset2); Serial.println("°C");
  Serial.print("Sensor 3: "); Serial.print(calibrationOffset3); Serial.println("°C");
  Serial.print("Sensor 4: "); Serial.print(calibrationOffset4); Serial.println("°C");
  Serial.print("Sensor 5: "); Serial.print(calibrationOffset5); Serial.println("°C");
  Serial.print("Sensor 6: "); Serial.print(calibrationOffset6); Serial.println("°C");
  Serial.print("Sensor 7: "); Serial.print(calibrationOffset7); Serial.println("°C");
  Serial.print("Sensor 8: "); Serial.print(calibrationOffset8); Serial.println("°C");
  Serial.println("\n=== DEFAULT MODE: VISA Command-Response ===");
  Serial.println("Send '*IDN?' to identify device or 'HELP?' for commands");
  Serial.println("\nAvailable Commands:");
  Serial.println("  CAL    - Enter calibration mode");
  Serial.println("  EXIT   - Exit current mode");
  Serial.println("  LABVIEW - Toggle LabVIEW output format");
  Serial.println("  CSV    - Enable CSV output for LabVIEW");
  Serial.println("  JSON   - Enable JSON output for LabVIEW");
  Serial.println("  HUMAN  - Enable human-readable output");
  Serial.println("  VISA   - Enable VISA command-response mode");
  Serial.println("  VSON   - Enable VISA mode (alias)");
  Serial.println("Waiting for MAX6675 sensors to stabilize...");
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
    } else if (command == "EXIT") {
      calibrationMode = false;
      labviewMode = false;
      visaMode = false;
      Serial.println("Exiting current mode - returning to human-readable output\n");
    } else if (command == "LVON" || command == "LABVIEW" || command == "CSV") {
      labviewMode = true;
      calibrationMode = false;
      visaMode = false;
      //Serial.println("LabVIEW CSV mode enabled");
      //Serial.println("Format: S1_C,S2_C,S3_C,S4_C,S5_C,S6_C,S7_C,S8_C");
      //Serial.println("Error values represented as -999.0");
      delay(1000);
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
    delay(1000); // Faster readings for LabVIEW
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
  
  Serial.println(); // Empty line for readability
  delay(2000); // Read every 2 seconds
}