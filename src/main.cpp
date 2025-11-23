#include <Arduino.h>

#include "connectivity/MqttClient.hpp"
#include "connectivity/WifiManager.hpp"
#include "motors/MotorDriver.hpp"
#include "secrets.hpp"
#include "sensors/Hcsr04.hpp"

static constexpr const char *MQTT_HOST = "192.168.178.33";
static constexpr int MQTT_PORT = 1883;

Hcsr04 sensor(D2, D3);
WifiManager wifi;
MqttClient mqtt(MQTT_HOST, MQTT_PORT);

// Motor A on D9/D10 (ENA jumper installed)
MotorDriver motorA(D9, D10);

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

    wifi.begin(WIFI_SSID, WIFI_PASS);
    mqtt.begin();

    Serial.println("System ready.");
}

void loop() {
    wifi.loop();
    mqtt.loop();
    sensor.update();

    static unsigned long lastSend = 0;
    if(millis() - lastSend >= 50) {
        lastSend = millis();

        if(auto dist = sensor.readFiltered()) {
            char payload[32];
            snprintf(payload, sizeof(payload), "%.2f", *dist);
            mqtt.publish("esp32/distance", payload);

            Serial.print("Distance: ");
            Serial.print(*dist);
            Serial.println(" cm");
        }
    }

    static unsigned long lastMove = 0;
    static int phase = 0;

    if(millis() - lastMove > 2000) {
        lastMove = millis();

        switch(phase) {
        case 0:
            Serial.println("Forward");
            motorA.forward();
            break;
        case 1:
            Serial.println("Stop");
            motorA.stop();
            break;
        case 2:
            Serial.println("Reverse");
            motorA.reverse();
            break;
        case 3:
            Serial.println("Stop");
            motorA.stop();
            break;
        }

        phase = (phase + 1) % 4;
    }
}
