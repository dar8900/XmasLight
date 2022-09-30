#ifndef INT_BUTTON_H
#define INT_BUTTON_H
#include <Arduino.h>
#include <stdint.h>

class INT_BUTTON
{
	private:
		int8_t _intPin = -1;
		bool _activeLow = false;
		bool _intReceived = false;

	public:
		INT_BUTTON(int8_t IntPin, bool ActiveLow = false);
		bool isTriggered();

};

#endif