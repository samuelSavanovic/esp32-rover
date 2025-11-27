#include "sensors/Hcsr04.hpp"

Hcsr04::Hcsr04(int trigPin, int echoPin) : trigPin_(trigPin), echoPin_(echoPin) {}

void Hcsr04::begin() {
    pinMode(trigPin_, OUTPUT);
    pinMode(echoPin_, INPUT_PULLDOWN);
    digitalWrite(trigPin_, LOW);
    delay(50);
}

void Hcsr04::loop() {
    const unsigned long now = micros();

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
            t_wait_start_ = now;
            state_ = State::WaitingForEchoHigh;
        }
        break;

    case State::WaitingForEchoHigh:
        if(digitalRead(echoPin_) == HIGH) {
            t_echo_start_ = now;
            t_wait_start_ = now;
            state_ = State::MeasuringEcho;
        } else if(now - t_wait_start_ >= ECHO_TIMEOUT_US) {
            // echo never went HIGH → measurement failed
            computed_distance_ = -1.0f;
            t_last_measure_ = now;
            state_ = State::Ready;
        }
        break;

    case State::MeasuringEcho:
        if(digitalRead(echoPin_) == LOW) {
            t_echo_end_ = now;
            unsigned long duration = t_echo_end_ - t_echo_start_;

            float d = duration * SPEED_CM_PER_US * 0.5f;

            // sensor works from 2 to 400cm
            // https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf
            if(d < 2.0f || d > 400.0f)
                computed_distance_ = -1.0f;
            else
                computed_distance_ = d;

            t_last_measure_ = now;
            state_ = State::Ready;
        } else if(now - t_wait_start_ >= ECHO_TIMEOUT_US) {
            // echo did not return to LOW → measurement failed
            computed_distance_ = -1.0f;
            t_last_measure_ = now;
            state_ = State::Ready;
        }
        break;

    case State::Ready:
        break;
    }
}

std::optional<float> Hcsr04::readDistance() {
    if(state_ == State::Ready) {
        state_ = State::Idle;
        if(computed_distance_ < 0)
            return std::nullopt;
        return computed_distance_;
    }
    return std::nullopt;
}

std::optional<float> Hcsr04::readFiltered() {
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