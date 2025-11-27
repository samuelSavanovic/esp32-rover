#include "connectivity/WsClient.hpp"
#include <Arduino.h>
#include <string.h> // for memcpy

WsClient::WsClient(const char *host, uint16_t port, const char *path) : host_(host), port_(port), path_(path) {}

void WsClient::begin() {
    client_.begin(host_, port_, path_);
    client_.onEvent([this](WStype_t type, uint8_t *payload, size_t len) { this->onWsEvent(type, payload, len); });
    client_.setReconnectInterval(2000);
}

void WsClient::loop() { client_.loop(); }

void WsClient::sendTelemetry(const Telemetry &t) {
    // Send exact binary representation
    client_.sendBIN(reinterpret_cast<const uint8_t *>(&t), sizeof(Telemetry));
}

void WsClient::onWsEvent(WStype_t type, uint8_t *payload, size_t len) {
    switch(type) {
    case WStype_CONNECTED:
        Serial.printf("WS connected to %s:%u%s\n", host_, port_, path_);
        break;

    case WStype_DISCONNECTED:
        Serial.println("WS disconnected");
        break;

    case WStype_BIN:
        handleCommand(payload, len);
        break;

    default:
        break;
    }
}

void WsClient::handleCommand(const uint8_t *data, size_t len) {
    if(len != sizeof(Command))
        return;

    Command cmd;
    memcpy(&cmd, data, sizeof(cmd));

    if(cmd.kind != TYPE_COMMAND) {
        return;
    }

    int16_t left = cmd.left_pwm;
    int16_t right = cmd.right_pwm;

    Serial.printf("CMD L=%d R=%d\n", left, right);
}
