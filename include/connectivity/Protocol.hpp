#pragma once
#include <stdint.h>

static const uint8_t TYPE_TELEMETRY = 0x01;
static const uint8_t TYPE_COMMAND = 0x02;

#pragma pack(push, 1)
struct Telemetry {
    uint8_t kind;
    uint32_t distance_mm;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Command {
    uint8_t kind;
    int16_t left_pwm;
    int16_t right_pwm;
};
#pragma pack(pop)
