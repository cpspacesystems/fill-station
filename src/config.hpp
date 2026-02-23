#pragma once

#include <Arduino.h>
#include <cstdint>

namespace FillStationConfig
{
    constexpr char MQTT_CTRL_SUBSCRIBE_TOPIC[] ="/fill-station/control";

    namespace MQTTConfig {
        constexpr char FILL_STATION_HOSTNAME[] = "fill_station";
        constexpr char MQTT_BROKER_SERVER_HOSTNAME[] = "mission_ctrl.local";
        constexpr uint16_t MQTT_BROKER_SERVER_PORT = 1883;
        constexpr bool NETWORK_AUTO_RECONNECT = true;
        constexpr uint16_t NETWORK_RECONNECT_DELAY = 1000; // ms
    }

    namespace Solenoid1 {
        constexpr uint8_t PIN = 2;
        constexpr uint8_t MODE = OUTPUT;
        constexpr bool IS_INITIALLY_CLOSED = false;
        constexpr bool IS_ACTIVE_HIGH = true;
    }

    namespace Solenoid2 {
        constexpr uint8_t PIN = 3;
        constexpr uint8_t MODE = OUTPUT;
        constexpr bool IS_INITIALLY_CLOSED = false;
        constexpr bool IS_ACTIVE_HIGH = true;
    }

    namespace PhenumaticValve1 {
        constexpr uint8_t PIN = 4;
        constexpr uint8_t MODE = OUTPUT;
        constexpr bool IS_INITIALLY_CLOSED = false;
        constexpr bool IS_ACTIVE_HIGH = true;
    }

    namespace PhenumaticValve2 {
        constexpr uint8_t PIN = 5;
        constexpr uint8_t MODE = OUTPUT;
        constexpr bool IS_INITIALLY_CLOSED = false;
        constexpr bool IS_ACTIVE_HIGH = true;
    }

    constexpr uint8_t PRESSURE_TRANSDUCER_PIN = PIN_A4; // 18
    constexpr uint8_t PRESSURE_TRANSDUCER_MODE = INPUT;

} // namespace FillStationConfig

