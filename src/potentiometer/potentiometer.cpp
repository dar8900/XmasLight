#include "potentiometer.h"

uint16_t Potenziometer::_readAnalogPort()
{
    _analogInstVal = analogRead(_pin);
}

Potenziometer::Potenziometer(int8_t Pin, uint16_t NSample, uint16_t SamplingRate):
                                _pin(Pin), _nSample(NSample), _samplingRate(SamplingRate)
{
    _analogReadTimer.start(SamplingRate);
}

uint16_t Potenziometer::getAnalogVal()
{
    return _analogVal;
}

float Potenziometer::getVoltageVal()
{
    float Voltage = 0.0f;
    Voltage = roundf(((float)_analogVal * MAX_VOLTAGE_VAL) / (float)MAX_ANALOG_VAL);
    return Voltage;
}

void Potenziometer::analogReadEngine()
{
    if(_analogReadTimer.isOver(true))
    {
        _readAnalogPort();
        if(_nMeasure >= _nSample)
        {
            _analogVal = _analogValAcc / _nMeasure;
            _analogValAcc = 0;
            _nMeasure = 0;
        }
        else
        {
            _analogValAcc += _analogInstVal;
            _nMeasure++;
        }
    }
}