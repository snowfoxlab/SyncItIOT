#include "SyncItIOT.h"
#include <sys/time.h>

const char* SNOW_FOX_CA = R"EOF(
-----BEGIN CERTIFICATE-----
MIID/TCCAuWgAwIBAgIUD7Y3yRtpK9HFvjNKcoygWEc8aLAwDQYJKoZIhvcNAQEL
BQAwgY0xCzAJBgNVBAYTAk5QMRAwDgYDVQQIDAdCQUdNQVRJMRAwDgYDVQQHDAdD
SElUV0FOMR0wGwYDVQQKDBRTTk9XIEZPWCBMQUIgUFZUIExURDESMBAGA1UEAwwJ
c3luY2l0aW90MScwJQYJKoZIhvcNAQkBFhhiaWthc2hAc25vd2ZveGxhYi5jb20u
bnAwHhcNMjYwMjI2MDU1NzMxWhcNMjcwMjI2MDU1NzMxWjCBjTELMAkGA1UEBhMC
TlAxEDAOBgNVBAgMB0JBR01BVEkxEDAOBgNVBAcMB0NISVRXQU4xHTAbBgNVBAoM
FFNOT1cgRk9YIExBQiBQVlQgTFREMRIwEAYDVQQDDAlzeW5jaXRpb3QxJzAlBgkq
hkiG9w0BCQEWGGJpa2FzaEBzbm93Zm94bGFiLmNvbS5ucDCCASIwDQYJKoZIhvcN
AQEBBQADggEPADCCAQoCggEBAKsqhS6pOYHLKLx+NnKgqcWle+vGWW3LnGiGJXa7
QfYja6Gnh4WjIOYFii9f4z2oo4Nd1IDsL6ZTuZ6VwNVFxiDvL29V0K2ABAk5G0wl
2xUANh83be8okaYQyfWbQFa05/bxak2rEF11FJMkcveMqVzpWe64KSKjaftJo0CS
8ke1HxN0IQAGDUpFyeX0v9RgelxvIK011GEMGWe4VZYcrScbs5UKkwbpox3ftQoS
JBRJPArLyJRGXBqQTYD7WLTgNsZJNlhRCu6voXpNRo79VzDTFNjqZ/eUZW1Tqcan
QG01PCmcj57SZyHA/2svWmK4+51MP3K4HA5D6QQwV+fuTOUCAwEAAaNTMFEwHQYD
VR0OBBYEFJNqtyppJmVy6dbw4m0NWaZ9w8C4MB8GA1UdIwQYMBaAFJNqtyppJmVy
6dbw4m0NWaZ9w8C4MA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQELBQADggEB
ABrO6IPEpytx8Y+gAr2VQ2xehZYXJFdqcytcPR7TOPlLHbMTxgnRhJldeXDFSzEe
SxtUXWMIVeMpDaEA6f3IrjhXsHeATZoLPjPlt2UFjQZImfou6QA9m8WLJfIzrGpG
wL+dN+xb7Kn1G64w/YHFH5W7/uiRrcLluvv41Ls+OFB/Jl14tm/+5H1yHA6V6rCw
P6JD2/Uknd7gveLekM62Gy2+S5zKIcsL19P42qgp66WYL8lzs4Y+JkCmGezVGE4t
XUNZfnUsqFrvEjKAYWxuZOZ2HtCXFSSwsjzxhMYnMv+BwI+NP0sFud4z8EfYy7rU
WtVfNyztWB9bWyDZ/8gv9iw=
-----END CERTIFICATE-----
)EOF";


SyncItIOT* SyncItIOT::_instance = nullptr;
SyncItIOT::SyncItIOT(const char* deviceId) : 
    _deviceId(deviceId), 
    _mqttClient(_wifiClient) 
{
    _instance = this;
    _broker = "iot.snowfoxlab.com.np"; // ⚡ Use your actual domain
    _port = 8883;
}
 
 
void SyncItIOT::begin(const char* ssid, const char* pass, const char* customBroker, bool useTLS) {
    _useTLS = useTLS;
    _broker = (customBroker != nullptr) ? customBroker : "iot.snowfoxlab.com.np";
 
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
    Serial.println("\n[SyncIt] WiFi Connected ✅");
 
    // ⚡ THE FIX: Wait for NTP Sync (Crucial for SSL)
    configTime(5.75 * 3600, 0, "pool.ntp.org", "time.google.com");
    Serial.print("[SyncIt] Syncing Time");
    time_t now = time(nullptr);
    while (now < 10000) { // Wait until we get a real timestamp
        delay(500);
        Serial.print(".");
        now = time(nullptr);
    }
    Serial.println("\n[SyncIt] Time Synced ✅");

 
    // // ⚡ THE FIX: Handle Self-Signed Certificate
    // _wifiClient.setCACert(SNOW_FOX_CA); 
    // _wifiClient.setInsecure(); // This ignores the "syncitiot" vs "iot.snowfoxlab..." name mismatch
    // _wifiClient.setHandshakeTimeout(30000); // Give the C3 plenty of time for RSA math
    if (_useTLS) {
    Serial.println("[SyncIt] Mode: TLS 🔐");
    _port = 8883;

    _wifiClient.setCACert(SNOW_FOX_CA);
//    _wifiClient.setInsecure(); // This ignores the "syncitiot" vs "iot.snowfoxlab..." name mismatch
    _wifiClient.setHandshakeTimeout(30000);

    _mqttClient.setClient(_wifiClient);

    } else {
    Serial.println("[SyncIt] Mode: NON-TLS 🔓");
    _port = 1883;

    _mqttClient.setClient(_wifiClientPlain);
   }
    
    _mqttClient.setServer(_broker, _port);
    _mqttClient.setCallback(_internalCallback);
}
void SyncItIOT::handle() {
    if (WiFi.status() != WL_CONNECTED) return;
    
    // Attempt reconnection if lost
    if (!_mqttClient.connected()) {
        _reconnect();
    }
    
    _mqttClient.loop();
 
    // Heartbeat logic
    if (millis() - _lastHeartbeat > 20000) {
        _lastHeartbeat = millis();
        String statusTopic = "devices/" + String(_deviceId) + "/status";
        // 📶 Include RSSI in heartbeat so server tracks signal quality over time
        String payload = "{\"status\":\"online\",\"rssi\":" + String(WiFi.RSSI()) + "}";
        _mqttClient.publish(statusTopic.c_str(), payload.c_str());
    }
}
// void SyncItIOT::_reconnect() {
//     static unsigned long lastAttempt = 0;
//     if (millis() - lastAttempt < 5000) return;
//     lastAttempt = millis();
 
//     // Log the actual broker being used
//     Serial.printf("[SyncIt] Connecting to %s (Port %d)...\n", _broker, _port);
    
//     String statusTopic = "devices/" + String(_deviceId) + "/status";
    
//     if (_mqttClient.connect(_deviceId, statusTopic.c_str(), 1, true, "offline")) {
//            Serial.println(_useTLS ?
//         "[SyncIt] TLS MQTT Connection ✅" :
//         "[SyncIt] NON-TLS MQTT Connection ✅");
//         String controlTopic = "devices/" + String(_deviceId) + "/control";
//         _mqttClient.subscribe(controlTopic.c_str());
//         _mqttClient.publish(statusTopic.c_str(), "online", true);
//     } else {
//         Serial.printf("[SyncIt] Secure Connect Failed, rc=%d\n", _mqttClient.state());
//         // Tip: rc -2 usually means the SSL handshake failed (cert mismatch)
//     }
// }
void SyncItIOT::_reconnect() {
    static unsigned long lastAttempt = 0;
    if (millis() - lastAttempt < 5000) return;
    lastAttempt = millis();

    Serial.printf("[SyncIt] Connecting to %s:%d (%s)...\n",
                  _broker, _port,
                  _useTLS ? "TLS" : "NON-TLS");

    String statusTopic = "devices/" + String(_deviceId) + "/status";

    unsigned long start = millis();

    if (_mqttClient.connect(_deviceId, statusTopic.c_str(), 1, true, "offline")) {

        unsigned long connectTime = millis() - start;

        Serial.printf(_useTLS ?
            "[SyncIt] TLS MQTT Connection ✅ (%lu ms)\n" :
            "[SyncIt] NON-TLS MQTT Connection ✅ (%lu ms)\n",
            connectTime);

        String controlTopic = "devices/" + String(_deviceId) + "/control";
        _mqttClient.subscribe(controlTopic.c_str());
        _mqttClient.publish(statusTopic.c_str(), "online", true);

    } else {

        Serial.printf("[SyncIt] Connect Failed (%s), rc=%d\n",
                      _useTLS ? "TLS" : "NON-TLS",
                      _mqttClient.state());
    }
}
// void SyncItIOT::sendData(int pin, float value) {
//     if (!_mqttClient.connected()) return;
//     String dataTopic = "devices/" + String(_deviceId) + "/data";
// JsonDocument doc;
//     doc["pin"] = pin;
//     doc["value"] = value;
//     char buffer[128];
//     serializeJson(doc, buffer);
//     _mqttClient.publish(dataTopic.c_str(), buffer);
// }
void SyncItIOT::sendData(int pin, float value) {
    if (!_mqttClient.connected()) return;
 
    String dataTopic = "devices/" + String(_deviceId) + "/data";
 
    JsonDocument doc;
    doc["pin"] = pin;
    doc["value"] = value;
 
  if (_benchmarkMode) {
        // ✅ FIXED timestamp (no overflow)
        struct timeval tv;
        gettimeofday(&tv, NULL);
 
        unsigned long long ts =
            (unsigned long long)tv.tv_sec * 1000ULL + tv.tv_usec / 1000;
 
        doc["ts"] = ts;
        doc["seq"] = _seq++;
        doc["rssi"] = WiFi.RSSI();  // 📶 WiFi signal strength (dBm)
         doc["tls"] = _useTLS;  // ✅ use the field that's actually set
        doc["phase"]      = _phase;         // String — set by user
        doc["deviceType"] = _deviceType;    // String — set by user
    }
 
    char buffer[192]; // increased buffer size
    serializeJson(doc, buffer);
 
    _mqttClient.publish(dataTopic.c_str(), buffer);
 
    // Optional debug
    Serial.print("[SyncIt] Sent: ");
    Serial.println(buffer);
}
void SyncItIOT::_internalCallback(char* topic, byte* payload, unsigned int length) {
   JsonDocument doc;
    if (deserializeJson(doc, payload, length) == DeserializationError::Ok) {
        if (_instance && _instance->_userCallback) {
            _instance->_userCallback(doc["pin"], doc["state"]);
        }
    }
}
 
void SyncItIOT::setCallback(void (*callback)(int pin, int state)) {
    _userCallback = callback;
}