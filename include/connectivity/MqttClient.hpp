#pragma once
#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

class MqttClient {
  public:
    MqttClient(const char *host, uint16_t port);

    void begin();

    void loop();

    bool publish(const char *topic, const char *payload);
    bool subscribe(const char *topic);

    void setCallback(MQTT_CALLBACK_SIGNATURE);

    bool isConnected();

  private:
    const char *host_;
    uint16_t port_;

    WiFiClient wifiClient_;
    PubSubClient client_;

    unsigned long lastAttempt_ = 0;
    static constexpr unsigned RECONNECT_INTERVAL_MS = 2000;

    void tryReconnect();
};
