#include "status_led.h"

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
}

void StatusLed::_rapidBlink()
{
	_setLedDigital(ON);
	delay(25);
	_setLedDigital(OFF);
}

void StatusLed::_bootMode()
{
	_ledSwichTimer.start(100);
	if(_ledSwichTimer.isOver(true))
	{
		_ledStatus = !_ledStatus;
		_setLedDigital(_ledStatus);
	}
}

void StatusLed::_errorMode()
{
	_ledSwichTimer.start(500);
	if(_ledSwichTimer.isOver(true))
	{
		_ledStatus = !_ledStatus;
		_setLedDigital(_ledStatus);
	}
}

void StatusLed::_manualSwMode()
{
	_ledStatus = ON;
	_setLedDigital(_ledStatus);
}

void StatusLed::_autoSwMode()
{
	_ledSwichTimer.start(1000);
	if(_ledSwichTimer.isOver(true))
	{
		if(_cnt == 3)
		{
			_rapidBlink();
			_cnt = 0;
		}
		else
		{
			_cnt++;
		}
	}
}

void StatusLed::_offMode()
{
	_ledStatus = OFF;
	_setLedDigital(_ledStatus);
}

StatusLed::StatusLed(int8_t Pin) : _pin(Pin)
{
	pinMode(_pin, OUTPUT);
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
	case booting_mode:
		_bootMode();
		break;
	case error_mode:
		_errorMode();
		break;
	case manual_switch_mode:
		_manualSwMode();
		break;
	case auto_switch_mode:
		_autoSwMode();
		break;
	case off_mode:
		_offMode(); 
		break;
	default:
		break;
	}
	if(_oldStatus != _actualStatus)
	{
		_ledSwichTimer.stop();
		_oldStatus = _actualStatus;
		_cnt = 0;
	}
}