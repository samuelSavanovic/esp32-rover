#pragma once
#include <Arduino.h>
#include <WiFi.h>

class WifiManager {
  public:
    WifiManager();

    void begin(const char *ssid, const char *pass);
    void loop();

    bool isConnected() const;
    IPAddress localIP() const;

  private:
    const char *ssid_ = nullptr;
    const char *pass_ = nullptr;

    unsigned long lastAttempt_ = 0;
    static constexpr unsigned RECONNECT_INTERVAL_MS = 3000;
};