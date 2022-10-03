#include "led_stripes.h"

#define MAX_ANALOG_WRITE_VAL			255
#define PERC_2_ANALOGWRITE(Perc)		((Perc * MAX_ANALOG_WRITE_VAL) / 100)

LedStripe::LedStripe(int8_t Pin, uint16_t DimmingTime, uint8_t MaxBrightnessPerc) : 
																				_pin(Pin)
{
	pinMode(_pin, OUTPUT);
	setDimmingTime(DimmingTime);
	setBrightness(MaxBrightnessPerc);
	_engineTimer.start(_engineCycle);
}

void LedStripe::setDimmingTime(uint16_t Time)
{
	if(Time != _dimmingTime && Time >= MAX_ANALOG_WRITE_VAL)
	{
		_dimmingTime = Time;
		_engineCycle = _dimmingTime / MAX_ANALOG_WRITE_VAL;
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
		if(_dimmingTime == 0)
		{
			if(_actualStatus != _targetStatus)
			{
				if(_targetStatus == off_status)
				{
					analogWrite(_pin, 0);
					_actualBrightness = 0;
					_actualStatus = off_status;
				}
				else
				{
					analogWrite(_pin, _brightnessTarget);
					_actualBrightness = _brightnessTarget;
					_actualStatus = on_status;
				}
			}
		}
		else
		{
			if(_actualStatus != _targetStatus)
			{
				if(_targetStatus == off_status)
				{
					if(_actualBrightness > 0)
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
					if(_actualBrightness < _brightnessTarget)
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