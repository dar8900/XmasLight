#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H
#include <Arduino.h>
#include <stdint.h>

class Potenziometer
{
	public:
		Potenziometer(int8_t Pin);


	private:
		int8_t _pin = -1;
		uint16_t _analogVal = 0;
		uint16_t _nSample = 0;
		uint16_t _samplingRate = 0;
		

};

#endif