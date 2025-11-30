#include "motors/MotorDriver.hpp"

MotorDriver::MotorDriver(int in1Pin, int in2Pin, std::optional<int> enablePin)
    : in1_(in1Pin), in2_(in2Pin), en_(enablePin) {}

void MotorDriver::begin() {
    pinMode(in1_, OUTPUT);
    pinMode(in2_, OUTPUT);

    if(en_) {
        pinMode(*en_, OUTPUT);
        analogWrite(*en_, 255);
    }

    stop();
}

void MotorDriver::forward(uint8_t speed) {
    if(en_)
        analogWrite(*en_, speed);

    digitalWrite(in1_, HIGH);
    digitalWrite(in2_, LOW);
}

void MotorDriver::reverse(uint8_t speed) {
    if(en_)
        analogWrite(*en_, speed);

    digitalWrite(in1_, LOW);
    digitalWrite(in2_, HIGH);
}

void MotorDriver::stop() {
    if(en_)
        analogWrite(*en_, 0);

    digitalWrite(in1_, LOW);
    digitalWrite(in2_, LOW);
}

void MotorDriver::setSpeed(int16_t pwm) {
    if(pwm == 0) {
        stop();
        return;
    }
    int speed = abs(pwm);
    if(speed > 255)
        speed = 255;

    uint8_t duty = static_cast<uint8_t>(abs(pwm));

    if(pwm > 0) {
        forward(duty);
    } else {
        reverse(duty);
    }
}