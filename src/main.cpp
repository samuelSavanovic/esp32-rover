#include "connectivity/WifiManager.hpp"
#include "secrets.hpp"
#include "sensors/Hcsr04.hpp"
#include <Arduino.h>
#include <PubSubClient.h>
#include <connectivity/MqttClient.hpp>

const char *MQTT_HOST = "192.168.178.33";
const int MQTT_PORT = 1883;

Hcsr04 sensor(D2, D3);
WifiManager wifi;
MqttClient mqtt("192.168.178.33", 1883);

void setup() {
    Serial.begin(115200);
    delay(200);
#ifdef DEBUG_SERIAL_WAIT
    while(!Serial) {
        delay(10);
    }
#endif

    sensor.begin();
    wifi.begin(WIFI_SSID, WIFI_PASS);
    mqtt.begin();
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
            Serial.println("cm");
        }
    }
}