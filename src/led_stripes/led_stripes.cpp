#include "led_stripes.h"
#include "../SerialDebug/serial_debug.h"

#define MAX_ANALOG_WRITE_VAL			255
#define PERC_2_ANALOGWRITE(Perc)		((Perc * MAX_ANALOG_WRITE_VAL) / 100)

LedStripe::LedStripe(int8_t Pin, uint16_t DimmingTime, uint8_t MaxBrightnessPerc)
{
	_pin = Pin;
	pinMode(_pin, OUTPUT);
	setDimmingTime(DimmingTime);
	setBrightness(MaxBrightnessPerc);
	_engineTimer.start(_engineCycle);
}

void LedStripe::setDimmingTime(uint16_t Time)
{
	if(Time >= MAX_ANALOG_WRITE_VAL || Time == NO_DIMMING)
	{
		_dimmingTime = Time;
		if(_dimmingTime == NO_DIMMING)
		{
			_engineCycle = _DIMMING_CYCLE_DFTL;
			Debug.logInfo("Dimming impostato a 0");
		}
		else
		{
			_engineCycle = _dimmingTime / MAX_ANALOG_WRITE_VAL;
			Debug.logInfo("Dimming impostato a: " + String(Time));
		}
	}
}

void LedStripe::setStatus(stripe_status NewStatus)
{
	if(NewStatus != _targetStatus)
	{
		_targetStatus = NewStatus;
	}
}

LedStripe::stripe_status LedStripe::getStatus()
{
	return _actualStatus;
}

void LedStripe::setBrightness(uint8_t NewBrightnessPerc)
{
	uint16_t AnalogBright = PERC_2_ANALOGWRITE(NewBrightnessPerc);
	if(AnalogBright != _brightnessTarget && NewBrightnessPerc <= MAX_BRIGHTNESS)
	{
		_brightnessTarget = AnalogBright;
	}
}

void LedStripe::ledStripeEngine()
{
	if(_engineTimer.isOver(true, _engineCycle))
	{
		if(_dimmingTime == NO_DIMMING)
		{
			if(_actualStatus != _targetStatus)
			{
				if(_targetStatus == off_status)
				{
					analogWrite(_pin, 0);
					_actualBrightness = 0;
				}
				else
				{
					analogWrite(_pin, _brightnessTarget);
					_actualBrightness = _brightnessTarget;
				}
				_actualStatus = _targetStatus;
			}
		}
		else
		{
			if(_actualStatus != _targetStatus)
			{
				if(_targetStatus == off_status)
				{
					if(_actualBrightness - 1 > 0)
					{
						_actualBrightness--;
					}
					else
					{
						_actualStatus = _targetStatus;
						_actualBrightness = 0;
					}
				}
				else
				{
					if(_actualBrightness + 1 < _brightnessTarget)
					{
						_actualBrightness++;
					}
					else
					{
						_actualStatus = _targetStatus;
						_actualBrightness = _brightnessTarget;
					}
				}
				analogWrite(_pin, _actualBrightness);
			}
		}
	}
}