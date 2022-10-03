#include "led_stripes.h"

#define PERC_2_ANALOGWRITE(Perc)		((Perc * 255) / 100)

LedStripe::LedStripe(int8_t Pin, uint16_t DimmingTime, uint8_t MaxBrightnessPerc) : 
																				_pin(Pin)
{
	pinMode(_pin, OUTPUT);
	setDimmingTime(DimmingTime);
	setBrightness(MaxBrightnessPerc);
	_engineTimer.start(ENGINE_CYCLE);
}

void LedStripe::setDimmingTime(uint16_t Time)
{
	if(Time != _dimmingTime && Time > ENGINE_CYCLE)
	{
		_dimmingTime = Time;
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
	if(_engineTimer.isOver(true))
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
					if(_actualBrightness - (_dimmingTime / ENGINE_CYCLE) > 0)
					{
						_actualBrightness -= (_dimmingTime / ENGINE_CYCLE);
					}
					else
					{
						_actualStatus = _targetStatus;
						_actualBrightness = 0;
					}
				}
				else
				{
					if(_actualBrightness + (_dimmingTime / ENGINE_CYCLE) < _brightnessTarget)
					{
						_actualBrightness += (_dimmingTime / ENGINE_CYCLE);
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