#include "connectivity/WifiManager.hpp"

WifiManager::WifiManager() {};

void WifiManager::begin(const char *ssid, const char *pass) {
    ssid_ = ssid;
    pass_ = pass;

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid_, pass_);

    Serial.println("Connecting to WiFi...");
}

void WifiManager::loop() {
    if(WiFi.status() == WL_CONNECTED)
        return;

    unsigned long now = millis();

    if(now - lastAttempt_ >= RECONNECT_INTERVAL_MS) {
        lastAttempt_ = now;

        Serial.println("Reconnecting to WiFi...");
        WiFi.disconnect();
        WiFi.begin(ssid_, pass_);
    }
}
bool WifiManager::isConnected() const { return WiFi.status() == WL_CONNECTED; }

IPAddress WifiManager::localIP() const { return WiFi.localIP(); }
