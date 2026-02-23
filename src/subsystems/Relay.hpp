#pragma once

#include <Arduino.h>
#include <MQTT.hpp>
#include <cassert>

#include "SubSystem.hpp"
#include "flatbuffers/gen/RelayState_generated.h"

class Relay : public SubSystem {
public:
    Relay(const uint8_t pin, const uint8_t mode, const bool is_initially_closed, const bool is_high_when_closed)
    : m_pin(pin), m_pinMode(mode), m_is_initially_closed(is_initially_closed), m_is_high_when_closed(is_high_when_closed)
    {
        assert(((mode == OUTPUT || mode == OUTPUT_OPENDRAIN)
        , "Output mode of `Relay` is expected to be either OUTPUT or OUTPUT_OPENDRAIN"));
    }

    FunctionStatus init() noexcept override {
        pinMode(m_pin, m_pinMode);
        
        if (m_is_initially_closed) this->close();
        else this->open();

        return FunctionStatus::Ok;
    };

    inline void open() noexcept {
        if (m_is_high_when_closed) {
            digitalWrite(m_pin, LOW);
        } else {
            digitalWrite(m_pin, HIGH);
        }
    };

    inline void close() noexcept {
        if (m_is_high_when_closed) {
            digitalWrite(m_pin, HIGH);
        } else {
            digitalWrite(m_pin, LOW);
        }
    };

    void update(flatbuffers::RelayState state) noexcept {
        switch (state) {
            case flatbuffers::RelayState::Open: 
                this->open();
                break;
            case flatbuffers::RelayState::Closed:
                this->close();
                break;
            default:
                break;
        }
    }

private:
    const uint8_t m_pin;
    const uint8_t m_pinMode;
    const bool m_is_initially_closed;
    const bool m_is_high_when_closed;
};