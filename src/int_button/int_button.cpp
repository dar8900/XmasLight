#include "int_button.h"


volatile bool InterruptTriggered = false;

void Trigger()
{
	if(!InterruptTriggered)
	{
		InterruptTriggered = true;
	}
}


INT_BUTTON::INT_BUTTON(int8_t IntPin, bool ActiveLow) : 
						_intPin(IntPin),
						_activeLow(ActiveLow)
{
	if(_activeLow)
	{
		pinMode(_intPin, INPUT_PULLUP);
		attachInterrupt(digitalPinToInterrupt(_intPin), blink, FALLING);
	}
	else
	{
		pinMode(_intPin, INPUT);
		attachInterrupt(digitalPinToInterrupt(_intPin), blink, RISING);
	}

}


bool INT_BUTTON::isTriggered()
{
	_intReceived = InterruptTriggered;
	if(_intReceived)
	{
		noInterrupts();
		InterruptTriggered = false;
		interrupts();
	}
	return _intReceived;
}