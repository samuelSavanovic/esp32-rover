#pragma once
#include <Arduino.h>
#include <optional>

class MotorDriver {
  public:
    MotorDriver(int in1Pin, int in2Pin, std::optional<int> enablePin = std::nullopt);

    void begin();
    void setSpeed(int16_t pwm);

  private:
    void forward(uint8_t speed = 255);
    void reverse(uint8_t speed = 255);
    void stop();
    int in1_;
    int in2_;
    std::optional<int> en_;
};
