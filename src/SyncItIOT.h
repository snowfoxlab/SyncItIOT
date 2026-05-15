#ifndef SyncItIOT_h
#define SyncItIOT_h

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFiClient.h>

class SyncItIOT {
  public:
    SyncItIOT(const char* deviceId);

    void begin(const char* ssid, const char* pass,
               const char* customBroker = nullptr,
               bool useTLS = true);

    void handle();
    void sendData(int pin, float value);
    void setCallback(void (*callback)(int pin, int state));

    // Benchmark config — call these before begin()
    void enableBenchmark(bool enable)          { _benchmarkMode = enable; }
    void setPhase(const char* phase)           { _phase = phase; }
    void setDeviceType(const char* deviceType) { _deviceType = deviceType; }

    int getRSSI() { return WiFi.RSSI(); }

  private:
    // Device identity
    const char* _deviceId;
    const char* _broker;
    int _port = 8883;

    // Connection
    bool _useTLS = true;
    WiFiClientSecure _wifiClient;
    WiFiClient _wifiClientPlain;
    PubSubClient _mqttClient;

    // Benchmark
    bool _benchmarkMode  = false;
    unsigned long _seq   = 0;
    const char* _phase      = "1";
    const char* _deviceType = "unknown";

    // Heartbeat
    unsigned long _lastHeartbeat = 0;

    // Callback
    void _reconnect();
    static SyncItIOT* _instance;
    static void _internalCallback(char* topic, byte* payload, unsigned int length);
    void (*_userCallback)(int pin, int state) = nullptr;
};

#endif