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
			Debug.logInfo("Dimming impostato a \"NO_DIMMING\"");
		}
		else
		{
			_engineCycle = _dimmingTime / MAX_ANALOG_WRITE_VAL;
			Debug.logInfo("Dimming impostato a: " + String(Time));
		}
	}
}

void LedStripe::setStatus(stripe_status NewStatus, bool Fast)
{
	if(NewStatus != _targetStatus || Fast)
	{
		if(Fast)
		{
			if(NewStatus == off_status)
			{
				analogWrite(_pin, 0);
				_actualBrightness = 0;
				Debug.logInfo("Striscia spenta in Fast mode");
				_actualStatus = off_status;
			}
			else
			{
				analogWrite(_pin, _brightnessTarget);
				_actualBrightness = _brightnessTarget;
				Debug.logInfo("Striscia accesa in Fast mode");
				_actualStatus = on_status;
			}
		}
		_targetStatus = NewStatus;
		Debug.logDebug("Settato nuovo stato della striscia led al valore: " + String(NewStatus));
	}
}

LedStripe::stripe_status LedStripe::getStatus()
{
	return _actualStatus;
}

bool LedStripe::ledSwitching()
{
	return _stripeIsSwitching;
}

void LedStripe::setBrightness(uint8_t NewBrightnessPerc)
{
	uint16_t AnalogBright = PERC_2_ANALOGWRITE(NewBrightnessPerc);
	if(AnalogBright != _brightnessTarget && NewBrightnessPerc <= MAX_BRIGHTNESS)
	{
		_brightnessTarget = AnalogBright;
		Debug.logInfo("Impostata nuova luminosita della striscia a: " + String(NewBrightnessPerc));
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
					Debug.logInfo("Striscia spenta senza dimming");
				}
				else
				{
					analogWrite(_pin, _brightnessTarget);
					_actualBrightness = _brightnessTarget;
					Debug.logInfo("Striscia accesa senza dimming");
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
					if(_actualBrightness > 0)
					{
						_actualBrightness--;
						_stripeIsSwitching = true;
					}
					else
					{
						_actualStatus = _targetStatus;
						_actualBrightness = 0;
						_stripeIsSwitching = false;
						Debug.logInfo("Striscia spenta con dimming");
					}
				}
				else
				{
					if(_actualBrightness < _brightnessTarget)
					{
						_actualBrightness++;
						_stripeIsSwitching = true;
					}
					else
					{
						_actualStatus = _targetStatus;
						_actualBrightness = _brightnessTarget;
						_stripeIsSwitching = false;
						Debug.logInfo("Striscia accesa con dimming");
					}
				}
				analogWrite(_pin, _actualBrightness);
			}
		}
	}
}