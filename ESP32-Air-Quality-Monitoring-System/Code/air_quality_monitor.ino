/*
 ============================================================================
  Project Name   : ESP32 Air Quality & Environmental Monitoring System
  Microcontroller: ESP32 DevKit V1 (30-pin / 38-pin)
  Sensors        : MQ-2 Gas Sensor (Analog Pin: GPIO 34)
                   DHT11 Temperature & Humidity Sensor (Digital Pin: GPIO 4)
  Display        : 16x2 I2C LCD Display (SDA: GPIO 21, SCL: GPIO 22, Addr: 0x27)
 ============================================================================
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// --- PIN CONFIGURATIONS ---
#define MQ2_PIN       34     // ESP32 Analog Pin for MQ-2 Gas Sensor (ADC1_CH6)
#define DHT_PIN       4      // ESP32 Digital Pin for DHT11 Data
#define DHT_TYPE      DHT11  // Sensor type DHT11

// Initialize 16x2 I2C LCD (Default address 0x27; if display stays blank, try 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Initialize DHT11 Sensor
DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  // Start Serial communication for debugging and Lab Viva evaluation
  Serial.begin(115200);
  delay(500);

  Serial.println("==========================================");
  Serial.println(" ESP32 Air Quality & Climate Monitor     ");
  Serial.println(" Sensors: MQ-2 Gas Sensor & DHT11        ");
  Serial.println("==========================================");

  // Initialize I2C communication (SDA = GPIO 21, SCL = GPIO 22)
  Wire.begin(21, 22);

  // Initialize LCD display
  lcd.init();
  lcd.backlight();

  // Initialize DHT11 sensor
  dht.begin();

  // Welcome / Boot Message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Air Monitoring ");
  lcd.setCursor(0, 1);
  lcd.print("  System Init.. ");
  delay(2000);

  // Sensor Pre-heating Message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MQ-2 & DHT11...");
  lcd.setCursor(0, 1);
  lcd.print("Warming Up...   ");
  delay(2000);

  lcd.clear();
}

void loop() {
  // --- 1. READ MQ-2 GAS SENSOR ---
  // Take 10 analog samples and average them to remove electrical noise
  long sensorSum = 0;
  for (int i = 0; i < 10; i++) {
    sensorSum += analogRead(MQ2_PIN);
    delay(10);
  }
  int rawADC = sensorSum / 10;

  // Map raw ADC (0 - 4095) to estimated Gas PPM (0 - 1000 PPM)
  int gasPPM = map(rawADC, 0, 4095, 0, 1000);
  if (gasPPM < 0) gasPPM = 0;

  // Determine Gas Level Status
  String airStatus = "";
  if (gasPPM <= 150) {
    airStatus = "GOOD";
  } else if (gasPPM <= 350) {
    airStatus = "MOD ";
  } else if (gasPPM <= 600) {
    airStatus = "POOR";
  } else {
    airStatus = "DANG"; // Dangerous / Hazard
  }

  // --- 2. READ DHT11 TEMPERATURE & HUMIDITY ---
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Check if DHT sensor reading is valid
  if (isnan(humidity) || isnan(temperature)) {
    humidity = 0.0;
    temperature = 0.0;
  }

  // --- 3. DISPLAY ON 16x2 LCD SCREEN ---
  // Line 1: Gas reading in PPM and Status (e.g., "Gas: 125PPM  GOOD")
  lcd.setCursor(0, 0);
  char line1[17];
  snprintf(line1, sizeof(line1), "Gas:%4dPPM  %s", gasPPM, airStatus.c_str());
  lcd.print(line1);

  // Line 2: Temperature & Humidity (e.g., "T:28.0C   H:60% ")
  lcd.setCursor(0, 1);
  char line2[17];
  snprintf(line2, sizeof(line2), "T:%.1fC   H:%.0f%% ", temperature, humidity);
  lcd.print(line2);

  // --- 4. PRINT TO SERIAL MONITOR (115200 Baud) ---
  Serial.print("Raw ADC: ");
  Serial.print(rawADC);
  Serial.print(" | Gas: ");
  Serial.print(gasPPM);
  Serial.print(" PPM (");
  Serial.print(airStatus);
  Serial.print(") | Temp: ");
  Serial.print(temperature);
  Serial.print(" *C | Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  delay(1000); // Refresh screen every 1 second
}
