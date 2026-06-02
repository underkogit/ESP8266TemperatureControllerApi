#ifndef HALLSENSOR_H
#define HALLSENSOR_H

#include <Arduino.h>

class HallSensor
{
public:
    HallSensor(uint8_t pin, unsigned long avgPeriodMs = 1000, unsigned long debounceUs = 500, uint8_t magnets = 1)
        : _pin(pin), _avgPeriodMs(avgPeriodMs), _debounceUs(debounceUs), _magnets(magnets < 1 ? 1 : magnets) {}

    void begin(bool usePullup = false, int8_t interruptMode = RISING)
    {
        pinMode(_pin, usePullup ? INPUT_PULLUP : INPUT);
        _interruptMode = interruptMode;
        _lastMillis = millis();
        instance() = this;
        attachInterrupt(digitalPinToInterrupt(_pin), isrRouter, _interruptMode);
    }

    void end()
    {
        detachInterrupt(digitalPinToInterrupt(_pin));
        instance() = nullptr;
    }

    void update()
    {
        unsigned long now = millis();
        if (now - _lastMillis >= _avgPeriodMs)
        {
            noInterrupts();
            unsigned long count = _pulseCount;
            interrupts();

            unsigned long delta = count - _lastCount;
            unsigned long elapsedMs = now - _lastMillis;
            _lastCount = count;
            _lastMillis = now;

            if (elapsedMs == 0)
                elapsedMs = 1;
            _frequency = (float)delta * 1000.0f / (float)elapsedMs / (float)_magnets;
            _rpm = _frequency * 60.0f;
        }
    }

    float getFrequency() const { return _frequency; }
    float getRPM() const { return _rpm; }
    unsigned long getTotalPulses() const
    {
        noInterrupts();
        unsigned long v = _pulseCount;
        interrupts();
        return v;
    }

    void resetCounters()
    {
        noInterrupts();
        _pulseCount = 0;
        _lastCount = 0;
        interrupts();
        _frequency = 0.0f;
        _rpm = 0.0f;
    }

private:
    const uint8_t _pin;
    unsigned long _avgPeriodMs;
    unsigned long _debounceUs;
    uint8_t _magnets;

    volatile unsigned long _pulseCount = 0;
    unsigned long _lastCount = 0;
    unsigned long _lastMillis = 0;

    volatile unsigned long _lastPulseMicros = 0;

    float _frequency = 0.0f;
    float _rpm = 0.0f;
    int _interruptMode = RISING;

    static inline HallSensor *&instance()
    {
        static HallSensor *inst = nullptr;
        return inst;
    }

    static void ICACHE_RAM_ATTR isrRouter()
    {
        HallSensor *inst = instance();
        if (inst)
            inst->handleInterrupt();
    }

    void ICACHE_RAM_ATTR handleInterrupt()
    {
        unsigned long nowUs = micros();
        if (_lastPulseMicros != 0)
        {
            unsigned long diff = nowUs - _lastPulseMicros;
            if (diff < _debounceUs)
                return;
        }
        _lastPulseMicros = nowUs;
        _pulseCount++;
    }
};

#endif // HALLSENSOR_H
