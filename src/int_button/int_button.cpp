#include "int_button.h"


volatile bool InterruptTriggered = false;

void Trigger()
{
	if(!InterruptTriggered)
	{
		InterruptTriggered = true;
	}
}


IntButton::IntButton(int8_t IntPin, bool ActiveLow) : 
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


bool IntButton::isTriggered()
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