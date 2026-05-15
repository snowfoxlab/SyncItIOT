/*
|--------------------------------------------------------------------------
| SyncItIOT NON-TLS Benchmark Example
|--------------------------------------------------------------------------
|
| Board Support:
| - ESP32 DevKit V1
| - ESP32-WROOM
| - ESP32-S3
| - ESP32-C3
|
| Features:
| - Fast MQTT Communication
| - Lightweight Connection
| - Benchmark Metadata Support
| - RSSI Monitoring
| - Simulated Sensor Data
|
| Notes:
| - Benchmark mode is DISABLED by default
| - Recommended for local testing
| - Lower latency compared to TLS
|
| Library:
| SyncItIOT by Snow Fox Lab
|
|--------------------------------------------------------------------------
*/

#include "SyncItIOT.h"

#define SENSOR_PIN 34

// Unique device ID
SyncItIOT syncIt("dev-esp32-non-tls-001");

// ------------------------------------------------------------------
// CONFIG
// ------------------------------------------------------------------

const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";

// Benchmark disabled by default
bool ENABLE_BENCHMARK = false;

// Send interval
unsigned long lastSensorRead = 0;
const unsigned long READ_INTERVAL = 2000;

// ------------------------------------------------------------------
// SETUP
// ------------------------------------------------------------------

void setup() {

    Serial.begin(115200);
    delay(1000);

    Serial.println("\n=================================");
    Serial.println(" SyncItIOT NON-TLS Example");
    Serial.println("=================================");

    // --------------------------------------------------------------
    // OPTIONAL BENCHMARK MODE
    // --------------------------------------------------------------
    //
    // Enable only when running latency/performance testing
    //
    // Includes:
    // - Timestamp
    // - Sequence Number
    // - RSSI
    // - Device Type
    // - TLS Status
    //
    // --------------------------------------------------------------

    syncIt.enableBenchmark(ENABLE_BENCHMARK);

    // Optional benchmark metadata
    syncIt.setPhase("non_tls_test");
    syncIt.setDeviceType("ESP32-DEVKIT");

    // --------------------------------------------------------------
    // CONNECT TO SYNCIT CLOUD
    // --------------------------------------------------------------
    //
    // TLS DISABLED
    // MQTT Port: 1883
    //
    // --------------------------------------------------------------

    syncIt.begin(
        WIFI_SSID,
        WIFI_PASS,
        nullptr,
        false   // TLS DISABLED
    );

    Serial.println("[SYNCIT] Device Ready ✅");
}

// ------------------------------------------------------------------
// LOOP
// ------------------------------------------------------------------

void loop() {

    // Required
    syncIt.handle();

    unsigned long now = millis();

    // Send data every 2 seconds
    if (now - lastSensorRead >= READ_INTERVAL) {

        lastSensorRead = now;

        // Simulated sensor value
        int sensorValue = random(780, 920);

        // Send to SyncIt Cloud
        syncIt.sendData(SENSOR_PIN, (float)sensorValue);

        // Serial Debug
        Serial.printf(
            "[NON-TLS] Sensor: %d | RSSI: %d dBm\n",
            sensorValue,
            syncIt.getRSSI()
        );
    }
}