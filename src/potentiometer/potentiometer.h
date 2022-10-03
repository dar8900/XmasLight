#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H
#include <Arduino.h>
#include <stdint.h>
#include "../TimerLib/Timer.hpp"

#define MAX_VOLTAGE_VAL         5.0
#define MAX_ANALOG_VAL          1023

class Potenziometer
{
	public:
		Potenziometer(int8_t Pin, uint16_t NSample, uint16_t SamplingRate);
		uint16_t getAnalogVal();
		float getVoltageVal();
		void analogReadEngine();

	private:
		uint16_t _engineCycle = 100;
		int8_t _pin = -1;
		uint32_t _analogValAcc = 0;
		uint16_t _analogInstVal = 0;
		uint16_t _analogVal = 0;
		float _voltageVal = 0.0;
		uint16_t _nSample = 0;
		uint16_t _nMeasure = 0;
		uint16_t _samplingRate = 0;
		Timer _analogReadTimer;
		uint16_t _readAnalogPort();
};

#endif