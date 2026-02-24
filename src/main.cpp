#define ZENOH_FREERTOS_PLUS_TCP 1

#include <Arduino.h>
#include <iostream>
#include <Ethernet.h>
#include "subsystems/Relay.hpp"
#include "config.hpp"
#include "MQTT.hpp"
#include "FixedVector.hpp"
#include "flatbuffers/gen/FillStationControl_generated.h"
#include "flatbuffers/gen/FillStationStatus_generated.h"

/// sets processor frequency in hz
extern "C" uint32_t set_arm_clock(uint32_t frequency);
static uint32_t currentClock = FillStationConfig::CPU_TYPICAL_CLOCK;

#ifndef NDEBUG
    #define serialdbg(...) SerialUSB.write(__VA_ARGS__); SerialUSB.flush();
#else
    #define serialdbg(...) 
#endif // !NDEBUG

static MQTT::Client<1>* mqtt;

// these 2 ids must be unsigned for defined overflow behavior
static uint64_t controlFrameExpected = 0; 
static uint64_t stateFrameNext = 0;

#define MessageBufSize 512
struct Message {
    char buf[MessageBufSize] = {0};
    flatbuffers::SystemMessageType type;

    Message() = default;
    Message(flatbuffers::SystemMessageType type) noexcept : type(type) {};
    template<typename ...Args>
    Message(flatbuffers::SystemMessageType type, const char* fmtStr, Args... fmtArgs) noexcept 
    : type(type) {
        snprintf(this->buf, MessageBufSize, fmtStr, fmtArgs...);
    }
};
#define NMessages 4
static FixedVector<Message, NMessages> messages;

static Relay solenoid1 { FillStationConfig::Solenoid1::PIN, FillStationConfig::Solenoid1::MODE, FillStationConfig::Solenoid1::IS_INITIALLY_CLOSED, FillStationConfig::Solenoid1::IS_ACTIVE_HIGH };
static Relay solenoid2 { FillStationConfig::Solenoid2::PIN, FillStationConfig::Solenoid2::MODE, FillStationConfig::Solenoid2::IS_INITIALLY_CLOSED, FillStationConfig::Solenoid2::IS_ACTIVE_HIGH };

static Relay phenumaicValve1 { FillStationConfig::PhenumaticValve1::PIN, FillStationConfig::PhenumaticValve1::MODE, FillStationConfig::PhenumaticValve1::IS_INITIALLY_CLOSED, FillStationConfig::PhenumaticValve1::IS_ACTIVE_HIGH };
static Relay phenumaicValve2 { FillStationConfig::PhenumaticValve2::PIN, FillStationConfig::PhenumaticValve2::MODE, FillStationConfig::PhenumaticValve2::IS_INITIALLY_CLOSED, FillStationConfig::PhenumaticValve2::IS_ACTIVE_HIGH };

// handle MQTT topic FillStationConfig::MQTT_CTRL_SUBSCRIBE_TOPIC updates
void handle_mqtt_control_update(void*, uint8_t* data, size_t size) {
    const flatbuffers::FillStationControl* ctrl = flatbuffers::GetFillStationControl(data);
    
    uint64_t fNum = ctrl->frame_number();
    if (fNum != controlFrameExpected) {
        // Message msg; 
        // msg.type = flatbuffers::SystemMessageType::WarnCTRLFramePossiblySkipped;
        // sniprintf(msg.buf, MessageBufSize, "WARN: Expected control frame number %llu, but got frame number %llu!", controlFrameExpected, fNum);
        assert(messages.push({
            flatbuffers::SystemMessageType::WarnCTRLFramePossiblySkipped, 
            "WARN: Expected control frame number %llu, but got frame number %llu!",
            controlFrameExpected, fNum
        }));
    }
    controlFrameExpected += 1;
    
    solenoid1.update(ctrl->solenoid_1());
    solenoid2.update(ctrl->solenoid_2());
    phenumaicValve1.update(ctrl->phenumatic_valve_1());
    phenumaicValve2.update(ctrl->phenumatic_valve_2());
}

// verify that we are not overheating
void check_temp() {
    
}

// put your setup code here, to run once:
void setup() {
    // set processor clock speed
    set_arm_clock(FillStationConfig::CPU_TYPICAL_CLOCK);
    currentClock = FillStationConfig::CPU_TYPICAL_CLOCK;

    // init termperature monitoring
    tempmon_init();
    tempmon_Start();
    
    // init serial debug output
#ifndef NDEBUG
    SerialUSB.begin(9600);
#endif // !NDEBUG

    // init subsystems
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

    // init MQTT
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
    // update MQTT and execute MC commands
    mqtt->update();

    static flatbuffers::FlatBufferBuilder fbBuilder(2048);

    // publish current state
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::SystemMessage>>> messagesOffset = 0;
    if (messages.size() >= 0) {
        flatbuffers::Offset<flatbuffers::SystemMessage> msgs[NMessages];
        for (size_t i = 0; i < messages.size(); i++) {
            Message msg;
            assert(messages.at(i, msg));

            msgs[i] = flatbuffers::CreateSystemMessage(fbBuilder, msg.type, fbBuilder.CreateString(msg.buf));
        }
        messagesOffset = fbBuilder.CreateVector(msgs, messages.size());
        messages.empty();
    }
    
    flatbuffers::Offset<flatbuffers::FillStationStatus> fss = flatbuffers::CreateFillStationStatus(fbBuilder, 
        stateFrameNext += 1,
        solenoid1.getState(),
        solenoid2.getState(),
        phenumaicValve1.getState(),
        phenumaicValve2.getState(),
        0.0,
        messagesOffset
    );
    fbBuilder.Finish(fss);

    // generate temperature warnings if overheating
    float temp = tempmonGetTemp();
    if (temp >= FillStationConfig::CPU_THERMAL_THROTTLE_THRESHHOLD) {
        assert(messages.push({
            flatbuffers::SystemMessageType::WarnProcessorThermalThrottle, 
            "WARN: Fill Station Teensy CPU throttling due to thermal. Temperature critical at %f °C!"
        }));
        set_arm_clock(FillStationConfig::CPU_THERMAL_THROTTLE_CLOCK);
        currentClock = FillStationConfig::CPU_THERMAL_THROTTLE_CLOCK;
    } else if (currentClock != FillStationConfig::CPU_TYPICAL_CLOCK) {
        set_arm_clock(FillStationConfig::CPU_TYPICAL_CLOCK);
        currentClock = FillStationConfig::CPU_TYPICAL_CLOCK;
        serialdbg("INFO: Thermal throttle condition ended. Reset.")
    }
    
}
