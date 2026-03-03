/**
 * 🚀 SyncIt IOT - Professional Management Platform
 * Developer Template: Standard Node Configuration
 * --------------------------------------------------
 * Optimized for: ESP32-C3, ESP32, ESP8266
 * Security: TLS/SSL Port 8883 (Managed by SyncIt Library)
 */

#include <SyncItIOT.h>

// --- 1. Pin Definitions ---
// Note: On ESP32-C3, GPIO 2 and 3 are recommended for stable boot.
const int RELAY_PIN  = 2;  // Digital Output (Relay/LED)
const int SENSOR_PIN = 3;  // Analog Input (Soil/LDR/NTC)

// --- 2. Instance Configuration ---
// Generate your Unique Device ID from the SyncIt Console
SyncItIOT syncIt("device-*****");

/**
 * @brief Dashboard Callback
 * Triggered when a toggle or slider is adjusted on the SyncIt Web Dashboard.
 */
void onDashboardCommand(int pin, int state) {
    Serial.printf("\n[SyncIt] ⚡ Command Received | Pin: %d | State: %d\n", pin, state);
    
    // Logic for Relay Control
    if (pin == RELAY_PIN) {
        digitalWrite(RELAY_PIN, (state == 1) ? HIGH : LOW);
        Serial.println((state == 1) ? ">> Device Status: ON" : ">> Device Status: OFF");
    }
}

void setup() {
    // Start Serial Debugging
    Serial.begin(115200);
    while(!Serial) { delay(10); } 
    delay(2000); 

    Serial.println("\n🚀 SyncIt IOT v1.0 ||Snow Fox Lab PVT LTD");
    Serial.println("---------------------------------");

    // Configure Local Hardware
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW); // Start with relay OFF
    
    // ESP32-C3 utilizes a 12-bit ADC (0 - 4095)
    analogReadResolution(12); 
    
    // Initialize Library & WiFi
    // Usage: syncIt.begin("SSID", "PASSWORD", "OPTIONAL_CUSTOM_BROKER");
    syncIt.begin("yourSSID_here", "YourPassword_here");
    
    // Attach the callback handler
    syncIt.setCallback(onDashboardCommand);

    Serial.println("[System] Initialization Complete. Syncing with SyncIt Cloud...");
}

void loop() {
    // 3. Engine Maintenance
    // Keeps the SSL connection alive and handles background tasks
    syncIt.handle();

    // 4. Telemetry Uplink (Every 5 Seconds)
    static unsigned long lastUplink = 0;
    if (millis() - lastUplink > 5000) {
        lastUplink = millis();
        
        // Read raw sensor data (0 - 4095)
        int rawData = analogRead(SENSOR_PIN);
        
        /**
         * 💡 DEV TIP: 
         * To send percentage instead of raw data, use:
         * float data = map(rawData, 4095, 0, 0, 100);
         */

        // Upload to SyncIt Cloud
        syncIt.sendData(SENSOR_PIN, (float)rawData);
        
        Serial.printf("📊 Data Uplink: [Pin %d] Value: %d\n", SENSOR_PIN, rawData);
    }
}