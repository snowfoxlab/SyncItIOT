# 🚀 SyncIt IOT Arduino Library

**SyncIt IOT** is a professional-grade IoT device management platform designed for ESP32-based microcontrollers.

This library provides a simple, secure, and lightweight way to connect devices to the SyncIt Cloud infrastructure using MQTT with optional TLS encryption.

Developed and maintained by **Snow Fox Lab PVT LTD**.

---

![Platform](https://img.shields.io/badge/Platform-ESP32%20%7C%20ESP32--C3%20%7C%20ESP32--S3-blue)
![MQTT](https://img.shields.io/badge/MQTT-TLS%20%7C%20NON--TLS-orange)
![License](https://img.shields.io/badge/License-MIT-green)
![Version](https://img.shields.io/badge/Version-1.0.0-red)

---

# ✨ Features

- 🔒 Secure MQTT over TLS/SSL (Port 8883)
- ⚡ Optional NON-TLS mode for low-latency local deployments
- 🔄 Automatic WiFi + MQTT reconnection
- 📡 RSSI monitoring support
- 📊 Virtual Pin data system
- 🕐 Automatic NTP time synchronization
- 📈 Benchmark / latency testing mode
- 🧠 Lightweight architecture optimized for ESP32-C3
- 🛠 Supports:
  - ESP32
  - ESP32-C3
  - ESP32-S3
  - ESP32-WROOM

---

# 📦 Installation

## Option 1 — Arduino Library Manager

Search for:

```txt
SyncItIOT
```

Then click **Install**.

---

## Option 2 — Manual Installation

1. Download this repository as ZIP
2. Open Arduino IDE
3. Go to:

```txt
Sketch → Include Library → Add .ZIP Library...
```

4. Select the downloaded ZIP

---

<!-- # 📚 Dependencies

Install these libraries from Arduino Library Manager:

| Library      | Recommended Version |
| ------------ | ------------------- |
| PubSubClient | Latest              |
| ArduinoJson  | 7.x                 |

--- -->

# 📚 Dependencies

## External Libraries (Required)

Install these libraries from Arduino Library Manager:

| Library      | Recommended Version |
| ------------ | ------------------- |
| PubSubClient | Latest              |
| ArduinoJson  | 7.x                 |

---

## ESP32 Core Libraries (Included Automatically)

These libraries are provided by the ESP32 Arduino Core package and do not need manual installation:

```cpp
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <Arduino.h>
```

---

# 🛠 Required Board Package

Install the ESP32 board package from:

```txt
Arduino IDE → Boards Manager
```

Search:

```txt
esp32 by Espressif Systems
```

Recommended Version:

```txt
v3.x or newer
```

# 🚀 Quick Start

## Secure TLS Connection (Recommended)

```cpp
#include <SyncItIOT.h>

SyncItIOT syncit("your-device-id");

void setup() {

    Serial.begin(115200);

    // TLS enabled by default
    syncit.begin(
        "YOUR_WIFI_NAME",
        "YOUR_WIFI_PASSWORD"
    );
}

void loop() {

    syncit.handle();

    syncit.sendData(1, 24.5);
}
```

---

# 🔓 NON-TLS Connection

Use NON-TLS mode for:

- Local testing
- Benchmarking
- Low-latency deployments
- Internal networks

MQTT Port used:

```txt
1883
```

Example:

```cpp
#include <SyncItIOT.h>

SyncItIOT syncit("your-device-id");

void setup() {

    Serial.begin(115200);

    syncit.begin(
        "YOUR_WIFI_NAME",
        "YOUR_WIFI_PASSWORD",
        nullptr,
        false
    );
}

void loop() {

    syncit.handle();

    syncit.sendData(1, 99.2);
}
```

---

# 🔒 TLS vs 🔓 NON-TLS

| Feature     | TLS             | NON-TLS       |
| ----------- | --------------- | ------------- |
| Port        | 8883            | 1883          |
| Encryption  | ✅ Yes          | ❌ No         |
| Security    | High            | Low           |
| Speed       | Slightly Slower | Faster        |
| Recommended | Production      | Local Testing |

---

# 📊 Benchmark Mode

Benchmark mode is designed for:

- Latency testing
- MQTT performance testing
- TLS overhead comparison
- Device benchmarking
- RSSI tracking

---

## Enable Benchmark Mode

```cpp
syncit.enableBenchmark(true);
```

---

## Optional Benchmark Metadata

```cpp
syncit.setPhase("tls_test");
syncit.setDeviceType("ESP32-C3");
```

---

## Benchmark Payload Includes

| Field      | Description            |
| ---------- | ---------------------- |
| ts         | Timestamp (ms)         |
| seq        | Packet sequence number |
| rssi       | WiFi signal strength   |
| tls        | TLS enabled/disabled   |
| phase      | Benchmark phase        |
| deviceType | Board/device type      |

---

# 📡 Example Benchmark Payload

```json
{
  "pin": 34,
  "value": 812,
  "ts": 1747362000123,
  "seq": 12,
  "rssi": -49,
  "tls": true,
  "phase": "tls_test",
  "deviceType": "ESP32-C3"
}
```

---

# 📂 Included Examples

| Example                 | Description           |
| ----------------------- | --------------------- |
| ESP32_TLS_Benchmark     | Secure MQTT benchmark |
| ESP32_NON_TLS_Benchmark | Fast MQTT benchmark   |
| BasicSendData           | Simple sensor example |

---

# 📖 API Reference

---

## Constructor

```cpp
SyncItIOT(deviceId)
```

Initialize SyncIt with unique device ID.

Example:

```cpp
SyncItIOT syncit("device-001");
```

---

## begin()

```cpp
begin(ssid, password, broker, useTLS)
```

Connect to WiFi and MQTT broker.

### Parameters

| Parameter | Type         | Description              |
| --------- | ------------ | ------------------------ |
| ssid      | const char\* | WiFi SSID                |
| password  | const char\* | WiFi password            |
| broker    | const char\* | Custom broker (optional) |
| useTLS    | bool         | Enable TLS               |

---

## handle()

```cpp
handle()
```

Runs:

- MQTT loop
- Reconnection logic
- Heartbeat engine

⚠ Must be called continuously inside `loop()`.

---

## sendData()

```cpp
sendData(pin, value)
```

Send sensor data to virtual pin.

Example:

```cpp
syncit.sendData(1, 25.6);
```

---

## setCallback()

Receive control commands from dashboard.

```cpp
syncit.setCallback(myCallback);
```

Example:

```cpp
void myCallback(int pin, int state) {

    digitalWrite(pin, state);
}
```

---

## getRSSI()

Returns WiFi signal strength.

```cpp
int rssi = syncit.getRSSI();
```

---

# 🛠 Supported Boards

| Board           | Supported |
| --------------- | --------- |
| ESP32 DevKit V1 | ✅        |
| ESP32-WROOM     | ✅        |
| ESP32-C3        | ✅        |
| ESP32-S3        | ✅        |

---

# ⚠ ESP32-C3 Notes

ESP32-C3 devices may experience:

- Longer TLS handshake time
- Reduced TLS performance
- Higher RSA computation time

Recommendations:

- Use stable WiFi
- Use latest ESP32 Arduino Core
- Prefer NON-TLS during heavy benchmarking

---

# 📡 MQTT Topics

| Topic                      | Purpose               |
| -------------------------- | --------------------- |
| devices/{deviceId}/data    | Sensor uplink         |
| devices/{deviceId}/control | Device control        |
| devices/{deviceId}/status  | Online/offline status |

---

# 🕐 NTP Time Synchronization

TLS certificates require valid device time.

SyncIt automatically synchronizes time using:

- pool.ntp.org
- time.google.com

No additional configuration required.

---

# 📈 Serial Output Example

## TLS Mode

```txt
[SyncIt] WiFi Connected ✅
[SyncIt] Time Synced ✅
[SyncIt] Mode: TLS 🔐
[SyncIt] TLS MQTT Connection ✅
```

---

## NON-TLS Mode

```txt
[SyncIt] WiFi Connected ✅
[SyncIt] Mode: NON-TLS 🔓
[SyncIt] NON-TLS MQTT Connection ✅
```

---

# 📖 Documentation

Official Documentation:

👉 https://docs.snowfoxlab.com.np

---

# 🤝 Support

Created by **Bikash Adhikari**  
Snow Fox Lab PVT LTD

📧 bikash@snowfoxlab.com.np

---

# 📜 License

MIT License

---

© 2026 Snow Fox Lab PVT LTD. All rights reserved.
