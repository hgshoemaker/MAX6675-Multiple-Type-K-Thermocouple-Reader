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

// ADS1115 ADC configuration
// The ADS1115 has 2 channels used: A0, A1
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

// Function to read voltage from ADS1115 channel (A0 and A1 only)
float readVoltage(int channel) {
  if (channel < 0 || channel > 1) {
    return NAN;  // Invalid channel (only 0 and 1 supported)
  }
  
  int16_t adc = 0;
  switch (channel) {
    case 0: adc = ads.readADC_SingleEnded(0); break;
    case 1: adc = ads.readADC_SingleEnded(1); break;
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
  if (!ads.begin(0X48)) {
    Serial.println("WARNING: ADS1115 not found! Voltage readings will show -999.0000");
    Serial.println("Check I2C wiring: SDA=Pin20, SCL=Pin21, VDD=5V, GND=GND");
    return;
  }
  ads.setGain(adsGain);
  Serial.println("ADS1115 initialized successfully - 2 voltage channels available");
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

// Function to output data in CSV format
void outputCSVFormat() {
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
  
  // Read voltage channels (A0 and A1 only)
  float voltage1 = readCalibratedVoltage(0, voltageOffset1);
  float voltage2 = readCalibratedVoltage(1, voltageOffset2);
  
  // Output in CSV format: S1_C,S2_C,S3_C,S4_C,S5_C,S6_C,S7_C,S8_C,V1,V2
  Serial.print(isnan(temp1C) ? -999.0 : temp1C, 2); Serial.print(",");
  Serial.print(isnan(temp2C) ? -999.0 : temp2C, 2); Serial.print(",");
  Serial.print(isnan(temp3C) ? -999.0 : temp3C, 2); Serial.print(",");
  Serial.print(isnan(temp4C) ? -999.0 : temp4C, 2); Serial.print(",");
  Serial.print(isnan(temp5C) ? -999.0 : temp5C, 2); Serial.print(",");
  Serial.print(isnan(temp6C) ? -999.0 : temp6C, 2); Serial.print(",");
  Serial.print(isnan(temp7C) ? -999.0 : temp7C, 2); Serial.print(",");
  Serial.print(isnan(temp8C) ? -999.0 : temp8C, 2); Serial.print(",");
  Serial.print(isnan(voltage1) ? -999.0 : voltage1, 4); Serial.print(",");
  Serial.print(isnan(voltage2) ? -999.0 : voltage2, 4);
  Serial.println(); // End line
}

void setup() {
  Serial.begin(9600);
  
  // Initialize I2C for ADS1115
  Wire.begin();
  
  // Initialize ADS1115 ADC
  initializeADS1115();
  
  /*Serial.println("MAX6675 Multiple Type K Thermocouple Reader with ADS1115 ADC");
  Serial.println("Reading from 8 thermocouples and 2 voltage inputs...");
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
  Serial.println("\n=== CSV Data Stream Mode ===");
  Serial.println("Outputting temperature and voltage data in CSV format every 5 seconds");
  Serial.println("Format: S1_C,S2_C,S3_C,S4_C,S5_C,S6_C,S7_C,S8_C,V1,V2");
  Serial.println("Error values shown as -999.00");
  Serial.println("\nAvailable Commands:");
  Serial.println("  CAL    - Enter calibration mode");
  Serial.println("  DEBUG  - Test individual sensors");
  Serial.println("  EXIT   - Exit calibration mode");
  Serial.println("Waiting for sensors to stabilize...");
  */
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
    } else if (command == "DEBUG" || command == "TEST") {
      testIndividualSensors();
    } else if (command == "EXIT") {
      calibrationMode = false;
      Serial.println("Exiting calibration mode - returning to CSV output\n");
    }
  }
  
  if (calibrationMode) {
    displayCalibrationReadings();
    delay(3000); // Slower readings in calibration mode
    return;
  }
  
  // Normal CSV output operation
  outputCSVFormat();
  delay(5000); // 5-second intervals for CSV data stream
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
  
  Serial.print("Voltage A0: ");
  float voltage1 = readCalibratedVoltage(0, voltageOffset1);
  Serial.print(isnan(voltage1) ? "ERROR" : String(voltage1, 4) + "V");
  Serial.println();
  
  Serial.print("Voltage A1: ");
  float voltage2 = readCalibratedVoltage(1, voltageOffset2);
  Serial.print(isnan(voltage2) ? "ERROR" : String(voltage2, 4) + "V");
  Serial.println();
  
  Serial.println("===================================");
}
