#include "UltrasonicSensor.hpp"
#include <Arduino.h>

UltrasonicSensor sensor(D2, D3);

void setup() {
    Serial.begin(115200);
    sensor.begin();
}

void loop() {
    sensor.update();

    if(auto dist = sensor.readFiltered()) {
        Serial.print("Filtered: ");
        Serial.print(*dist);
        Serial.println(" cm");
    }
}
