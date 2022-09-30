#include "led_stripes.h"


LedStripe::LedStripe(int8_t Pin, uint16_t DimmingTime, uint16_t MaxBrightness) : 
																				_pin(Pin)
{
	pinMode(_pin, OUTPUT);
	setDimmingTime(DimmingTime);
	setBrightness(MaxBrightness);
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

void LedStripe::setBrightness(uint16_t NewBrightness)
{
	if(NewBrightness != _brightnessTarget && NewBrightness <= MAX_BRIGHTNESS)
	{
		_brightnessTarget = NewBrightness;
	}
}

void LedStripe::runEngine()
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