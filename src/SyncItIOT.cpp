#include "SyncItIOT.h"

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


void SyncItIOT::begin(const char* ssid, const char* pass, const char* customBroker) {
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

    // ⚡ THE FIX: Handle Self-Signed Certificate
    _wifiClient.setCACert(SNOW_FOX_CA); 
    _wifiClient.setInsecure(); // This ignores the "syncitiot" vs "iot.snowfoxlab..." name mismatch
    _wifiClient.setHandshakeTimeout(30000); // Give the C3 plenty of time for RSA math
    
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
        _mqttClient.publish(statusTopic.c_str(), "online");
    }
}
void SyncItIOT::_reconnect() {
    static unsigned long lastAttempt = 0;
    if (millis() - lastAttempt < 5000) return;
    lastAttempt = millis();

    // Log the actual broker being used
    Serial.printf("[SyncIt] Connecting to %s (Port %d)...\n", _broker, _port);
    
    String statusTopic = "devices/" + String(_deviceId) + "/status";
    
    if (_mqttClient.connect(_deviceId, statusTopic.c_str(), 1, true, "offline")) {
        Serial.println("[SyncIt] MQTTS Secure Connection ✅");
        String controlTopic = "devices/" + String(_deviceId) + "/control";
        _mqttClient.subscribe(controlTopic.c_str());
        _mqttClient.publish(statusTopic.c_str(), "online", true);
    } else {
        Serial.printf("[SyncIt] Secure Connect Failed, rc=%d\n", _mqttClient.state());
        // Tip: rc -2 usually means the SSL handshake failed (cert mismatch)
    }
}
void SyncItIOT::sendData(int pin, float value) {
    if (!_mqttClient.connected()) return;
    String dataTopic = "devices/" + String(_deviceId) + "/data";
JsonDocument doc;
    doc["pin"] = pin;
    doc["value"] = value;
    char buffer[128];
    serializeJson(doc, buffer);
    _mqttClient.publish(dataTopic.c_str(), buffer);
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