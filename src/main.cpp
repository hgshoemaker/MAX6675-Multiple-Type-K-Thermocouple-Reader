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

// Calibration offsets for each sensor (adjust these values)
float calibrationOffset1 = 0.0;  // Sensor 1 offset in °C
float calibrationOffset2 = 0.0;  // Sensor 2 offset in °C
float calibrationOffset3 = 0.0;  // Sensor 3 offset in °C
float calibrationOffset4 = 0.0;  // Sensor 4 offset in °C

// Create multiple MAX6675 objects
MAX6675 thermocouple1(thermoCLK, thermoCS1, thermoDO);  // Sensor 1
MAX6675 thermocouple2(thermoCLK, thermoCS2, thermoDO);  // Sensor 2
MAX6675 thermocouple3(thermoCLK, thermoCS3, thermoDO);  // Sensor 3
MAX6675 thermocouple4(thermoCLK, thermoCS4, thermoDO);  // Sensor 4

// Calibration mode flag
bool calibrationMode = false;
bool labviewMode = false;  // Flag for LabVIEW output format

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

// Function to convert calibrated Celsius to Fahrenheit
float calibratedCelsiusToFahrenheit(float tempC) {
  if (isnan(tempC)) {
    return NAN;
  }
  return (tempC * 9.0/5.0) + 32.0;
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
  
  // Calculate average (excluding NaN values)
  float sum = 0;
  int count = 0;
  if (!isnan(temp1)) { sum += temp1; count++; }
  if (!isnan(temp2)) { sum += temp2; count++; }
  if (!isnan(temp3)) { sum += temp3; count++; }
  if (!isnan(temp4)) { sum += temp4; count++; }
  
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
  }
  
  Serial.println("=====================================\n");
}

// Function to output data in LabVIEW-compatible format
void outputLabVIEWFormat() {
  // Read all sensors
  float temp1C = readCalibratedCelsius(thermocouple1, calibrationOffset1);
  float temp2C = readCalibratedCelsius(thermocouple2, calibrationOffset2);
  float temp3C = readCalibratedCelsius(thermocouple3, calibrationOffset3);
  float temp4C = readCalibratedCelsius(thermocouple4, calibrationOffset4);
  
  // Convert to Fahrenheit
  float temp1F = calibratedCelsiusToFahrenheit(temp1C);
  float temp2F = calibratedCelsiusToFahrenheit(temp2C);
  float temp3F = calibratedCelsiusToFahrenheit(temp3C);
  float temp4F = calibratedCelsiusToFahrenheit(temp4C);
  
  // Output in CSV format: Sensor1_C,Sensor1_F,Sensor2_C,Sensor2_F,Sensor3_C,Sensor3_F,Sensor4_C,Sensor4_F
  Serial.print(isnan(temp1C) ? -999.0 : temp1C, 2); Serial.print(",");
  Serial.print(isnan(temp1F) ? -999.0 : temp1F, 2); Serial.print(",");
  Serial.print(isnan(temp2C) ? -999.0 : temp2C, 2); Serial.print(",");
  Serial.print(isnan(temp2F) ? -999.0 : temp2F, 2); Serial.print(",");
  Serial.print(isnan(temp3C) ? -999.0 : temp3C, 2); Serial.print(",");
  Serial.print(isnan(temp3F) ? -999.0 : temp3F, 2); Serial.print(",");
  Serial.print(isnan(temp4C) ? -999.0 : temp4C, 2); Serial.print(",");
  Serial.print(isnan(temp4F) ? -999.0 : temp4F, 2);
  Serial.println(); // End line
}

// Function to output data in JSON format (alternative for LabVIEW)
void outputJSONFormat() {
  Serial.print("{");
  
  float temp1C = readCalibratedCelsius(thermocouple1, calibrationOffset1);
  float temp2C = readCalibratedCelsius(thermocouple2, calibrationOffset2);
  float temp3C = readCalibratedCelsius(thermocouple3, calibrationOffset3);
  float temp4C = readCalibratedCelsius(thermocouple4, calibrationOffset4);
  
  Serial.print("\"sensor1\":{\"celsius\":");
  Serial.print(isnan(temp1C) ? -999.0 : temp1C, 2);
  Serial.print(",\"fahrenheit\":");
  Serial.print(isnan(temp1C) ? -999.0 : calibratedCelsiusToFahrenheit(temp1C), 2);
  Serial.print("},");
  
  Serial.print("\"sensor2\":{\"celsius\":");
  Serial.print(isnan(temp2C) ? -999.0 : temp2C, 2);
  Serial.print(",\"fahrenheit\":");
  Serial.print(isnan(temp2C) ? -999.0 : calibratedCelsiusToFahrenheit(temp2C), 2);
  Serial.print("},");
  
  Serial.print("\"sensor3\":{\"celsius\":");
  Serial.print(isnan(temp3C) ? -999.0 : temp3C, 2);
  Serial.print(",\"fahrenheit\":");
  Serial.print(isnan(temp3C) ? -999.0 : calibratedCelsiusToFahrenheit(temp3C), 2);
  Serial.print("},");
  
  Serial.print("\"sensor4\":{\"celsius\":");
  Serial.print(isnan(temp4C) ? -999.0 : temp4C, 2);
  Serial.print(",\"fahrenheit\":");
  Serial.print(isnan(temp4C) ? -999.0 : calibratedCelsiusToFahrenheit(temp4C), 2);
  Serial.print("}");
  
  Serial.println("}");
}

void setup() {
  Serial.begin(9600);
  Serial.println("MAX6675 Multiple Type K Thermocouple Test");
  Serial.println("Reading from 4 sensors...");
  Serial.println("Current calibration offsets:");
  Serial.print("Sensor 1: "); Serial.print(calibrationOffset1); Serial.println("°C");
  Serial.print("Sensor 2: "); Serial.print(calibrationOffset2); Serial.println("°C");
  Serial.print("Sensor 3: "); Serial.print(calibrationOffset3); Serial.println("°C");
  Serial.print("Sensor 4: "); Serial.print(calibrationOffset4); Serial.println("°C");
  Serial.println("\nAvailable Commands:");
  Serial.println("  CAL    - Enter calibration mode");
  Serial.println("  EXIT   - Exit current mode");
  Serial.println("  LABVIEW - Toggle LabVIEW output format");
  Serial.println("  CSV    - Enable CSV output for LabVIEW");
  Serial.println("  JSON   - Enable JSON output for LabVIEW");
  Serial.println("  HUMAN  - Enable human-readable output");
  Serial.println("Waiting for MAX6675 sensors to stabilize...");
  delay(500); // Wait for MAX6675 to stabilize
}

void loop() {
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
      Serial.println("Exiting current mode - returning to human-readable output\n");
    } else if (command == "LVON" || command == "LABVIEW" || command == "CSV") {
      labviewMode = true;
      calibrationMode = false;
      Serial.println("LabVIEW CSV mode enabled");
      Serial.println("Format: Sensor1_C,Sensor1_F,Sensor2_C,Sensor2_F,Sensor3_C,Sensor3_F,Sensor4_C,Sensor4_F");
      Serial.println("Error values represented as -999.0");
      delay(1000);
    } else if (command == "LVOFF" || command == "HUMAN") {
      labviewMode = false;
      calibrationMode = false;
      Serial.println("Human-readable mode enabled\n");
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
    float temp1F = calibratedCelsiusToFahrenheit(temp1C);
    Serial.print("Sensor 1: ");
    Serial.print(temp1C, 2);
    Serial.print("°C / ");
    Serial.print(temp1F, 2);
    Serial.println("°F");
  }
  
  delay(100);
  
  // Read from Sensor 2 (calibrated)
  float temp2C = readCalibratedCelsius(thermocouple2, calibrationOffset2);
  if (isnan(temp2C)) {
    Serial.println("Sensor 2: Error reading thermocouple!");
  } else {
    float temp2F = calibratedCelsiusToFahrenheit(temp2C);
    Serial.print("Sensor 2: ");
    Serial.print(temp2C, 2);
    Serial.print("°C / ");
    Serial.print(temp2F, 2);
    Serial.println("°F");
  }
  
  delay(100);
  
  // Read from Sensor 3 (calibrated)
  float temp3C = readCalibratedCelsius(thermocouple3, calibrationOffset3);
  if (isnan(temp3C)) {
    Serial.println("Sensor 3: Error reading thermocouple!");
  } else {
    float temp3F = calibratedCelsiusToFahrenheit(temp3C);
    Serial.print("Sensor 3: ");
    Serial.print(temp3C, 2);
    Serial.print("°C / ");
    Serial.print(temp3F, 2);
    Serial.println("°F");
  }
  
  delay(100);
  
  // Read from Sensor 4 (calibrated)
  float temp4C = readCalibratedCelsius(thermocouple4, calibrationOffset4);
  if (isnan(temp4C)) {
    Serial.println("Sensor 4: Error reading thermocouple!");
  } else {
    float temp4F = calibratedCelsiusToFahrenheit(temp4C);
    Serial.print("Sensor 4: ");
    Serial.print(temp4C, 2);
    Serial.print("°C / ");
    Serial.print(temp4F, 2);
    Serial.println("°F");
  }
  
  Serial.println(); // Empty line for readability
  delay(2000); // Read every 2 seconds
}