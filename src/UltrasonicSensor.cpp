#include "UltrasonicSensor.hpp"

UltrasonicSensor::UltrasonicSensor(int trigPin, int echoPin) : trigPin_(trigPin), echoPin_(echoPin) {}

void UltrasonicSensor::begin() {
    pinMode(trigPin_, OUTPUT);
    pinMode(echoPin_, INPUT_PULLDOWN);
    digitalWrite(trigPin_, LOW);
    delay(50);
}

void UltrasonicSensor::update() {
    const unsigned long now = micros();
    constexpr unsigned long MEASURE_INTERVAL_US = 60000;

    switch(state_) {

    case State::Idle:
        if(now - t_last_measure_ >= MEASURE_INTERVAL_US) {
            digitalWrite(trigPin_, HIGH);
            t_trigger_start_ = now;
            state_ = State::TriggerPulseHigh;
        }
        break;

    case State::TriggerPulseHigh:
        if(now - t_trigger_start_ >= 10) {
            digitalWrite(trigPin_, LOW);
            state_ = State::WaitingForEchoHigh;
        }
        break;

    case State::WaitingForEchoHigh:
        if(digitalRead(echoPin_) == HIGH) {
            t_echo_start_ = now;
            state_ = State::MeasuringEcho;
        }
        break;

    case State::MeasuringEcho:
        if(digitalRead(echoPin_) == LOW) {
            t_echo_end_ = now;
            unsigned long duration = t_echo_end_ - t_echo_start_;

            constexpr float SPEED = 0.0343f; // speed of sound in cm per micro-second
            float d = duration * SPEED * 0.5f;

            if(d < 2.0f || d > 400.0f) {
                computed_distance_ = -1.0f;
            } else {
                computed_distance_ = d;
            }

            t_last_measure_ = now;
            state_ = State::Ready;
        }
        break;

    case State::Ready:
        break;
    }
}

std::optional<float> UltrasonicSensor::readDistance() {
    if(state_ == State::Ready) {
        state_ = State::Idle;

        if(computed_distance_ < 0) {
            return std::nullopt;
        }
        return computed_distance_;
    }

    return std::nullopt;
}

std::optional<float> UltrasonicSensor::readFiltered() {
    auto raw = readDistance();
    if(!raw)
        return std::nullopt;

    float d = *raw;

    if(filterCount_ < FILTER_SIZE) {
        filterBuffer_[filterCount_++] = d;
    } else {
        for(int i = 1; i < FILTER_SIZE; i++)
            filterBuffer_[i - 1] = filterBuffer_[i];
        filterBuffer_[FILTER_SIZE - 1] = d;
    }

    if(filterCount_ < FILTER_SIZE)
        return std::nullopt;

    float sorted[FILTER_SIZE];
    for(int i = 0; i < FILTER_SIZE; i++)
        sorted[i] = filterBuffer_[i];

    std::sort(sorted, sorted + FILTER_SIZE);

    return sorted[FILTER_SIZE / 2];
}