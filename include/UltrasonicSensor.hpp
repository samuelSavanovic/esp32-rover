#pragma once
#include <Arduino.h>
#include <optional>

class UltrasonicSensor {
  public:
    UltrasonicSensor(int trigPin, int echoPin);

    void begin();
    void update();

    // Returns a single raw reading if ready
    std::optional<float> readDistance();

    // Returns a filtered reading if enough samples are available
    std::optional<float> readFiltered();

  private:
    enum class State { Idle, TriggerPulseHigh, WaitingForEchoHigh, MeasuringEcho, Ready };

    int trigPin_;
    int echoPin_;
    State state_ = State::Idle;

    unsigned long t_last_measure_ = 0;
    unsigned long t_trigger_start_ = 0;
    unsigned long t_echo_start_ = 0;
    unsigned long t_echo_end_ = 0;

    float computed_distance_ = -1.0f;

    static constexpr int FILTER_SIZE = 5;
    float filterBuffer_[FILTER_SIZE];
    int filterCount_ = 0;
};
