/**
 * 🚀 SyncIt IOT - Basic Sensor Uplink
 * --------------------------------------------------
 * This example demonstrates how to send telemetry data
 * to the SyncIt Cloud using a virtual Pin system.
 */

#include <SyncItIOT.h>

// 1. Initialize the SyncIt instance with your unique Device ID
// Get this ID from https://syncit.foxlab.com.np
SyncItIOT syncit("your-device-id");

void setup() {
  Serial.begin(115200);
  
  // 2. Connect to WiFi and initiate Secure MQTTS (Port 8883)
  // The library automatically handles NTP time sync for SSL
  syncit.begin("YOUR_SSID", "YOUR_PASSWORD");
  
  Serial.println("🚀 SyncIt IOT Online");
}

void loop() {
  // 3. Keep the connection alive (Heartbeats & Reconnections)
  syncit.handle();

  // 4. Send sensor data every 10 seconds
  static unsigned long lastUplink = 0;
  if (millis() - lastUplink > 10000) {
    lastUplink = millis();

    // Mocking a sensor (e.g., Temperature)
    float temp = 24.5 + (random(0, 100) / 50.0);
    
    // Send to Pin 1
    // In the SyncIt Dashboard, create a "Gauge" or "Chart" widget 
    // and set its Data Pin to 1 to see this value live.
    syncit.sendData(1, temp); 
    
    Serial.printf("📈 Data Sent: %.2f°C to Pin 1\n", temp);
  }
}