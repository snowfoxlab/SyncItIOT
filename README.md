# 🚀 SyncIt IOT Arduino Library

**SyncIt IOT** is a professional-grade management platform for IoT devices. This library provides a simple, secure, and robust way to connect your ESP32, ESP32-C3, or ESP8266 devices to the SyncIt ecosystem.

Developed and maintained by **Snow Fox Lab PVT LTD**.

![Platform](https://img.shields.io/badge/Platform-ESP32%20%7C%20ESP32--C3%20%7C%20ESP8266-blue)
![License](https://img.shields.io/badge/License-MIT-green)
![Version](https://img.shields.io/badge/Version-1.0.0-orange)

---

## ✨ Features

- 🔒 **Secure by Default:** Uses TLS/SSL (Port 8883) for all communications.
- 🔄 **Automatic Management:** Handles WiFi reconnection and MQTT heartbeats in the background.
- 📊 **Virtual Pin System:** Map hardware sensors directly to dashboard widgets (Gauges, Charts, Toggles).
- 🕐 **Time Sync:** Built-in NTP synchronization for certificate validation.
- ⚡ **Lightweight:** Minimal memory footprint, optimized for single-core chips like the ESP32-C3.

---

## 📦 Installation

### Option 1 — Arduino Library Manager _(Coming Soon)_

Search for `SyncItIOT` in the Library Manager and click **Install**.

### Option 2 — Manual Installation

1. Download this repository as a `.zip` file.
2. In the Arduino IDE, go to **Sketch → Include Library → Add .ZIP Library...**
3. Select the downloaded `.zip` file.

### Dependencies

Install the following libraries via the Arduino Library Manager before using SyncIt IOT:

| Library                                                   | Recommended Version |
| --------------------------------------------------------- | ------------------- |
| [PubSubClient](https://github.com/knolleary/pubsubclient) | Latest              |
| [ArduinoJson](https://arduinojson.org/)                   | 7.x                 |

---

## 🚀 Quick Start

### Basic Connect & Send Data

```cpp
#include <SyncItIOT.h>

// 1. Initialize SyncIt with your Unique Device ID from the Dashboard
SyncItIOT syncit("your-device-id");

void setup() {
  Serial.begin(115200);

  // 2. Connect to WiFi and initiate Secure Connection
  syncit.begin("YOUR_WIFI_SSID", "YOUR_WIFI_PASSWORD");

  Serial.println("🚀 SyncIt IOT Online");
}

void loop() {
  // 3. Keep the background engine running
  syncit.handle();

  // 4. Send data every 10 seconds
  static unsigned long lastUplink = 0;
  if (millis() - lastUplink > 10000) {
    lastUplink = millis();

    float temp = 24.5 + (random(0, 100) / 50.0);

    // Send to Pin 1 (Connect this to a Gauge widget in your dashboard)
    syncit.sendData(1, temp);

    Serial.printf("📈 Data Sent: %.2f to Pin 1\n", temp);
  }
}
```

---

## 🛠 Hardware Configuration

For **ESP32-C3** users, the following pin configuration is recommended for stability:

| Function       | GPIO   |
| -------------- | ------ |
| Relay / Output | GPIO 2 |
| Sensor / Input | GPIO 3 |

---

## 📖 API Reference

| Method                  | Description                                         |
| ----------------------- | --------------------------------------------------- |
| `SyncItIOT(deviceId)`   | Initialize with your dashboard Device ID            |
| `begin(ssid, password)` | Connect to WiFi and start secure MQTT session       |
| `handle()`              | Must be called in `loop()` — runs background engine |
| `sendData(pin, value)`  | Send a float value to a Virtual Pin                 |

---

## 📖 Documentation

Full API documentation and dashboard setup instructions:
👉 [https://docs.snowfoxlab.com.np](https://docs.snowfoxlab.com.np)

---

## 🤝 Support

Created by **Bikash Adhikari** at Snow Fox Lab PVT LTD.

For technical support or enterprise inquiries:
📧 [bikash@snowfoxlab.com.np](mailto:bikash@snowfoxlab.com.np)

---

© 2026 **Snow Fox Lab PVT LTD**. All rights reserved.
