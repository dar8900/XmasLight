#include "status_led.h"
#include "../SerialDebug/serial_debug.h"

#define ON		true
#define OFF		false

void StatusLed::_setLedDigital(bool Status)
{
	if(Status)
	{
		digitalWrite(_pin, HIGH);
	}
	else
	{
		digitalWrite(_pin, LOW);
	}
	_ledStatus = Status;
}

void StatusLed::_toggleLed()
{
	_setLedDigital(!_ledStatus);
}

void StatusLed::rapidBlink(uint16_t Delay, uint16_t BlinkTimes = 0)
{
	if(BlinkTimes == 0)
	{
		digitalWrite(_pin, HIGH);
		delay(Delay);
		digitalWrite(_pin, LOW);
		delay(Delay);
	}
	else
	{
		for(int i = 0; i < BlinkTimes; i++)
		{
			digitalWrite(_pin, HIGH);
			delay(Delay);
			digitalWrite(_pin, LOW);
			delay(Delay);
		}
	}
	_setLedDigital(_ledStatus);
}



StatusLed::StatusLed(int8_t Pin) : _pin(Pin)
{
	pinMode(_pin, OUTPUT);
	_setLedDigital(OFF);
	_ledSwichTimer.start(10);
}

void StatusLed::setStatus(led_mode NewStatus)
{
	if(NewStatus != _actualStatus)
	{
		_actualStatus = NewStatus;
	}
}

void StatusLed::ledEngine()
{
	switch (_actualStatus)
	{
	case error_mode:
		if(_ledSwichTimer.isOver(true, 250))
		{
			_toggleLed();
		}
		break;
	case manual_switch_mode:
		if(_ledSwichTimer.isOver(true, 2500))
		{
			rapidBlink(10);
		}
		break;
	case auto_switch_mode:
		if(_ledSwichTimer.isOver(true, 10000))
		{
			rapidBlink(10);
		}
		break;
	case off_mode:
		_setLedDigital(OFF);
		break;
	default:
		break;
	}
	if(_oldStatus != _actualStatus)
	{
		_oldStatus = _actualStatus;
		rapidBlink(50, 4);
		Debug.logDebug("Modifica modalita status led");
	}
}