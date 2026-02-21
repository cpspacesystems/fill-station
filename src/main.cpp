#define ZENOH_FREERTOS_PLUS_TCP 1

#include <Arduino.h>
#include <iostream>
#include <Ethernet.h>
#include <zenoh-pico.h>
#include "subsystems/Relay.hpp"
#include "config.hpp"

#ifndef NDEBUG
    #define serialdbg(...) SerialUSB.write(__VA_ARGS__); SerialUSB.flush();
#else
    #define serialdbg(...) 
#endif // !NDEBUG

static Relay solenoid1 { FillStationConfig::SOLENOID1_PIN, FillStationConfig::SOLENOID1_MODE };
static Relay solenoid2 { FillStationConfig::SOLENOID2_PIN, FillStationConfig::SOLENOID2_MODE };

static Relay phenumaicValve1 { FillStationConfig::PHENUMATIC_VALVE1_PIN, FillStationConfig::PHENUMATIC_VALVE1_MODE };
static Relay phenumaicValve2 { FillStationConfig::PHENUMATIC_VALVE2_PIN, FillStationConfig::PHENUMATIC_VALVE2_MODE };

// put your setup code here, to run once:
void setup() {
#ifndef NDEBUG
    SerialUSB.begin(9600);
#endif // !NDEBUG

    if (solenoid1.init() != SubSystem::FunctionStatus::Ok) {
        serialdbg("DEATH: Solenoid1 Failed to initialize!");
        std::abort();
        
    }

    if (solenoid2.init() != SubSystem::FunctionStatus::Ok) {
        serialdbg("DEATH: Solenoid2 Failed to initialize!");
        std::abort();
    }

    if (phenumaicValve1.init() != SubSystem::FunctionStatus::Ok) {
        serialdbg("DEATH: Phenumatic Valve 1 failed to initialize!");
        std::abort();
    }

    if (phenumaicValve2.init() != SubSystem::FunctionStatus::Ok) {
        serialdbg("DEATH: Phenumatic Valve 2 failed to initialize!");
        std::abort();
    }

}

// put your main code here, to run repeatedly:
void loop() {


}
