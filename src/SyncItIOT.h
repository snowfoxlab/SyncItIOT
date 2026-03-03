#ifndef SyncItIOT_h
#define SyncItIOT_h

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h> 
#include <PubSubClient.h>
#include <ArduinoJson.h>

class SyncItIOT {
  public:
    SyncItIOT(const char* deviceId);
    
    // Cleaned up begin function
    void begin(const char* ssid, const char* pass, const char* customBroker = nullptr);
    
    void handle(); 
    void sendData(int pin, float value);
    void setCallback(void (*callback)(int pin, int state));

  private:
    const char* _deviceId;
    const char* _broker; // Removed inline initialization here to avoid conflicts
    int _port = 8883;        
    
    WiFiClientSecure _wifiClient; 
    PubSubClient _mqttClient;
    
    unsigned long _lastHeartbeat = 0;
    void _reconnect();
    
    // This allows the static callback to talk to your class instance
    static SyncItIOT* _instance; 
    static void _internalCallback(char* topic, byte* payload, unsigned int length);
    void (*_userCallback)(int pin, int state);
};

#endif