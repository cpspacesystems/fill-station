#define ZENOH_FREERTOS_PLUS_TCP 1

#include <Arduino.h>
#include <iostream>
#include <Ethernet.h>
#include "subsystems/Relay.hpp"
#include "config.hpp"
#include "MQTT.hpp"
#include "flatbuffers/gen/FillStationControl_generated.h"

#ifndef NDEBUG
    #define serialdbg(...) SerialUSB.write(__VA_ARGS__); SerialUSB.flush();
#else
    #define serialdbg(...) 
#endif // !NDEBUG

static MQTT::Client<1>* mqtt;

static Relay solenoid1 { FillStationConfig::Solenoid1::PIN, FillStationConfig::Solenoid1::MODE, FillStationConfig::Solenoid1::IS_INITIALLY_CLOSED, FillStationConfig::Solenoid1::IS_ACTIVE_HIGH };
static Relay solenoid2 { FillStationConfig::Solenoid2::PIN, FillStationConfig::Solenoid2::MODE, FillStationConfig::Solenoid2::IS_INITIALLY_CLOSED, FillStationConfig::Solenoid2::IS_ACTIVE_HIGH };

static Relay phenumaicValve1 { FillStationConfig::PhenumaticValve1::PIN, FillStationConfig::PhenumaticValve1::MODE, FillStationConfig::PhenumaticValve1::IS_INITIALLY_CLOSED, FillStationConfig::PhenumaticValve1::IS_ACTIVE_HIGH };
static Relay phenumaicValve2 { FillStationConfig::PhenumaticValve2::PIN, FillStationConfig::PhenumaticValve2::MODE, FillStationConfig::PhenumaticValve2::IS_INITIALLY_CLOSED, FillStationConfig::PhenumaticValve2::IS_ACTIVE_HIGH };

void handle_mqtt_control_update(void*, uint8_t* data, size_t size) {
    const flatbuffers::FillStationControl* ctrl = flatbuffers::GetFillStationControl(data);
    
    solenoid1.update(ctrl->solenoid_1());
    solenoid2.update(ctrl->solenoid_2());
    phenumaicValve1.update(ctrl->phenumatic_valve_1());
    phenumaicValve2.update(ctrl->phenumatic_valve_2());
}

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

    MQTT::initNetwork(FillStationConfig::MQTTConfig::FILL_STATION_HOSTNAME);

    static IPAddress mqtt_addr;
    if (!MQTT::resolveServer(mqtt_addr, FillStationConfig::MQTTConfig::MQTT_BROKER_SERVER_HOSTNAME)) {
        serialdbg("DEATH: Unable to resolve server.");
        std::abort();
    }

    static MQTT::Client<1> mqtt_obj(FillStationConfig::MQTTConfig::FILL_STATION_HOSTNAME, mqtt_addr, 
        FillStationConfig::MQTTConfig::MQTT_BROKER_SERVER_PORT, FillStationConfig::MQTTConfig::NETWORK_AUTO_RECONNECT, FillStationConfig::MQTTConfig::NETWORK_RECONNECT_DELAY);
    mqtt = &mqtt_obj;

    mqtt->subscribe(FillStationConfig::MQTT_CTRL_SUBSCRIBE_TOPIC, {handle_mqtt_control_update});
}

// put your main code here, to run repeatedly:
void loop() {
    mqtt->update();
}
