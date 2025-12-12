#include <Arduino.h>

#include "connectivity/MqttClient.hpp"
#include "connectivity/Protocol.hpp"
#include "connectivity/WifiManager.hpp"
#include "connectivity/WsClient.hpp"
#include "motors/MotorDriver.hpp"
#include "secrets.hpp"
#include "sensors/Hcsr04.hpp"

static constexpr const char *SERVER_HOST = "192.168.178.33";
static constexpr int MQTT_PORT = 1883;
static constexpr int WS_PORT = 9000;

Hcsr04 sensor(D2, D3);
WifiManager wifi;
// MqttClient mqtt(SERVER_HOST, MQTT_PORT);
WsClient ws(SERVER_HOST, WS_PORT, "/ws");

MotorDriver motorA(D4, D5, D6);
MotorDriver motorB(D9, D10, D11);
void setup() {

    Serial.begin(115200);
    delay(200);
#ifdef DEBUG_SERIAL_WAIT
    while(!Serial) {
        delay(10);
    }
#endif

    sensor.begin();
    motorA.begin();
    motorB.begin();

    wifi.begin(WIFI_SSID, WIFI_PASS);
    // mqtt.begin();
    ws.begin();

    Serial.println("System ready.");
}

void loop() {
    wifi.loop();
    // mqtt.loop();
    sensor.loop();
    ws.loop();

    static unsigned long lastSend = 0;
    if(millis() - lastSend >= 50) {
        lastSend = millis();

        if(auto dist = sensor.readFiltered()) {
            char payload[32];
            snprintf(payload, sizeof(payload), "%.2f", *dist);
            // mqtt.publish("esp32/distance", payload);

            // Serial.print("Distance: ");
            // Serial.print(*dist);
            // Serial.println(" cm");

            Telemetry t{TYPE_TELEMETRY, uint32_t(*dist * 10)};
            ws.sendTelemetry(t);
        }
    }

    if(auto cmd = ws.getLastCommand()) {
        int16_t left = cmd->left_pwm;
        int16_t right = cmd->right_pwm;
        motorA.setSpeed(left);
        motorB.setSpeed(right);
    }
}
