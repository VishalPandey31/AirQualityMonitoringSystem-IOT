# Future Implementation: Bluetooth Dashboard Integration

This document outlines the planned future scope for extending the **ESP32 Air Quality Monitoring System** with wireless Bluetooth telemetry, allowing users to monitor air quality metrics directly on an Android / iOS smartphone without needing an active Wi-Fi router.

---

## 1. Overview & Architecture

The ESP32 features built-in dual-mode Bluetooth (Bluetooth Classic + Bluetooth Low Energy / BLE). By enabling the onboard Bluetooth Serial profile (`BluetoothSerial.h`), the device can broadcast real-time telemetry:
- Raw ADC Readings
- Calibrated PPM (Parts Per Million)
- Air Quality Status (`Excellent`, `Good`, `Moderate`, `Poor`, `Hazardous`)
- Battery / Voltage Status

```
+------------------------+      Wireless Bluetooth     +--------------------------+
|  ESP32 Air Monitor     |  ========================>  |  Smartphone Dashboard    |
|  (MQ-135 + 16x2 LCD)   |     (2.4GHz RF Link)        |  (Android / iOS App)     |
+------------------------+                             +--------------------------+
```

---

## 2. Planned Firmware Upgrade (Bluetooth Code Snippet)

Below is the verified code template to upgrade `air_quality_monitor.ino` to transmit live data over Bluetooth Serial:

```cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "BluetoothSerial.h" // ESP32 Built-in Bluetooth Library

LiquidCrystal_I2C lcd(0x27, 16, 2);
BluetoothSerial SerialBT;

#define MQ135_PIN 34

void setup() {
  Serial.begin(115200);
  
  // Initialize Bluetooth with a custom broadcast name
  SerialBT.begin("ESP32_Air_Monitor");
  Serial.println("Bluetooth Started! Pair with 'ESP32_Air_Monitor'");

  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();
}

void loop() {
  int rawADC = analogRead(MQ135_PIN);
  int aqiPPM = map(rawADC, 0, 4095, 0, 1000);

  // Send Telemetry JSON string over Bluetooth
  if (SerialBT.hasClient()) {
    String payload = "{\"ppm\":" + String(aqiPPM) + ",\"raw\":" + String(rawADC) + "}";
    SerialBT.println(payload);
  }

  delay(1000);
}
```

---

## 3. Smartphone Applications for Testing

Users can test this wireless telemetry immediately using popular free Bluetooth terminal apps:
1. **Serial Bluetooth Terminal** (Kai Morich) on Google Play Store
2. **Bluetooth Terminal / BLE Scanner** on Apple App Store
3. **MIT App Inventor custom companion app** with a dial gauge and color-changing alert banner.

---

## 4. Hardware Extensions in Pipeline
- **Piezo Buzzer (Active 5V Buzzer):** Pin GPIO 25 for audible hazard alert when PPM > 600.
- **DHT11 Temperature & Humidity Sensor:** Pin GPIO 4 for temperature-compensated air quality calculation.
- **MicroSD Card Module:** For standalone offline data logging over 30 days.
