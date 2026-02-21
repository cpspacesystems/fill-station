#pragma once
#include <Arduino.h>
#include <cstdint>

namespace FillStationConfig
{
    constexpr uint8_t SOLENOID1_PIN = 2;
    constexpr uint8_t SOLENOID1_MODE = OUTPUT;

    constexpr uint8_t SOLENOID2_PIN = 3;
    constexpr uint8_t SOLENOID2_MODE = OUTPUT;

    constexpr uint8_t PHENUMATIC_VALVE1_PIN = 3;
    constexpr uint8_t PHENUMATIC_VALVE1_MODE = OUTPUT;

    constexpr uint8_t PHENUMATIC_VALVE2_PIN = 4;
    constexpr uint8_t PHENUMATIC_VALVE2_MODE = OUTPUT;

    constexpr uint8_t PRESSURE_TRANSDUCER_PIN = PIN_A4; // 18
    constexpr uint8_t PRESSURE_TRANSDUCER_MODE = INPUT;

} // namespace FillStationConfig

