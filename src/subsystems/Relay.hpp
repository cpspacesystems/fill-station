#pragma once

#include <Arduino.h>
#include <MQTT.hpp>
#include <cassert>

#include "SubSystem.hpp"
#include "flatbuffers/gen/RelayState_generated.h"

class Relay : public SubSystem {
public:
    /// @brief A 2-State Relay
    /// @param pin the relay control pin
    /// @param mode the relay pin output mode
    /// @param is_initially_closed weather or not this relay should be initialized closed or open
    /// @param is_high_when_closed is this relay Active HIGH (is the relay closed when the PIN is pulled high to VCC) 
    Relay(const uint8_t pin, const uint8_t mode, const bool is_initially_closed, const bool is_high_when_closed)
    :   m_pin(pin), m_pinMode(mode), 
        m_is_initially_closed(is_initially_closed), m_is_high_when_closed(is_high_when_closed),
        m_state_is_closed(is_initially_closed)
    {
        assert(((mode == OUTPUT || mode == OUTPUT_OPENDRAIN)
        , "Output mode of `Relay` is expected to be either OUTPUT or OUTPUT_OPENDRAIN"));
    }

    /// @brief Initializes this Relay and all of it's hardware
    /// @return Ok if success, Err if not
    FunctionStatus init() noexcept override {
        pinMode(m_pin, m_pinMode);
        
        if (m_is_initially_closed) this->close();
        else this->open();

        return FunctionStatus::Ok;
    };

    /// @brief Sets this relay to the open position
    void open() noexcept {
        m_state_is_closed = false;
        if (m_is_high_when_closed) {
            digitalWrite(m_pin, LOW);
        } else {
            digitalWrite(m_pin, HIGH);
        }
    };

    /// @brief sets this relay to the closed position
    void close() noexcept {
        m_state_is_closed = true;
        if (m_is_high_when_closed) {
            digitalWrite(m_pin, HIGH);
        } else {
            digitalWrite(m_pin, LOW);
        }
    };

    /// @brief is this relay closed
    /// @return true if it's closed, false if it's not
    inline bool isClosed() const noexcept {
        return m_state_is_closed;
    };

    /// @brief Helper to create a RelayState from the current state
    /// @return a flatbuffers::RelayState respresenting the current RelayState
    flatbuffers::RelayState getState() const noexcept {
        if (isClosed()) return flatbuffers::RelayState::Closed;
        else return flatbuffers::RelayState::Open;
    }

    /// @brief Helper to set this relay's state using a RelayState
    /// @param state the state to set this relay to 
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

    bool m_state_is_closed;
};