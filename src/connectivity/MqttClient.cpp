#include "connectivity/MqttClient.hpp"

MqttClient::MqttClient(const char *host, uint16_t port) : host_(host), port_(port), client_(wifiClient_) {}

void MqttClient::begin() { client_.setServer(host_, port_); }

void MqttClient::loop() {
    if(!client_.connected()) {
        tryReconnect();
    }
    client_.loop();
}

bool MqttClient::publish(const char *topic, const char *payload) {
    if(!client_.connected())
        return false;
    return client_.publish(topic, payload);
}

bool MqttClient::subscribe(const char *topic) {
    if(!client_.connected())
        return false;
    return client_.subscribe(topic);
}

bool MqttClient::isConnected() { return client_.connected(); }

void MqttClient::setCallback(MQTT_CALLBACK_SIGNATURE) { client_.setCallback(callback); }

void MqttClient::tryReconnect() {
    unsigned long now = millis();

    if(now - lastAttempt_ < RECONNECT_INTERVAL_MS)
        return;

    lastAttempt_ = now;

    Serial.println("MQTT reconnect... ");

    if(client_.connect("esp32_client")) {
        Serial.println("connected.");
        // client_.subscribe("esp32/commands");
    } else {
        Serial.print("failed, rc=");
        Serial.println(client_.state());
    }
}
