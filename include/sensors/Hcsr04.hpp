#pragma once
#include <Arduino.h>
#include <optional>

class Hcsr04 {
  public:
    Hcsr04(int trigPin, int echoPin);

    void begin();
    void loop();

    std::optional<float> readDistance();
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
    unsigned long t_wait_start_ = 0;

    float computed_distance_ = -1.0f;

    static constexpr int FILTER_SIZE = 5;
    float filterBuffer_[FILTER_SIZE] = {};
    int filterCount_ = 0;

    static constexpr unsigned MEASURE_INTERVAL_US = 60000;
    static constexpr unsigned ECHO_TIMEOUT_US = 30000;
    static constexpr float SPEED_CM_PER_US = 0.0343f; // speed of sound
};
