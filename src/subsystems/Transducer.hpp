#pragma once

#include <Arduino.h>

// the default is 250 ohms, at 4mA and 20mA, 1V and 5V
// using a 150 ohm resistor, at 4mA and 20mA, 0.6V and 3V
enum Defaults1500Hx{
    resistor1500Hx = 150, // ohms
    maxPressure1500Hx = 1500 // SPI
};

class Transducer {
    public:
    Transducer(uint8_t pin, uint8_t mode, uint32_t resistance)
    :   pin(pin),
        mode(mode),
        resistance(resistance),
        maxPressure(maxPressure),
        minVoltage(resistance * .004),
        maxVoltage(resistance * .02)
    {}

    void init() {
        analog_init();
        pinMode(pin, mode);
    }
    int readVoltage() {
        return analogRead(pin);
    }
    double readPSI() {
        return (analogRead(pin) - minVoltage) * ((maxVoltage - minVoltage) * maxPressure); // (v - .6) * 625
    }


    private:
        const uint8_t pin;
        const uint8_t mode;
        const uint32_t resistance;
        const uint16_t maxPressure;
        const double minVoltage;
        const double maxVoltage;
};