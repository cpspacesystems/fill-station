#pragma once

#include <Arduino.h>
#include <cassert>

#include "SubSystem.hpp"

class Relay : public SubSystem {
public:
    Relay(uint8_t pin, uint8_t mode)
    : m_pin(pin), m_pinMode(mode)
    {
        assert(((mode == OUTPUT || mode == OUTPUT_OPENDRAIN)
        , "Output mode of `Relay` is expected to be either OUTPUT or OUTPUT_OPENDRAIN"));
    }

    FunctionStatus init() noexcept override {
        pinMode(m_pin, m_pinMode);
        return FunctionStatus::Ok;
    };

    
private:
    uint8_t m_pin;
    uint8_t m_pinMode;
};