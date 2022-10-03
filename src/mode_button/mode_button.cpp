#include "mode_button.h"

#define ENGINE_CYCLE	20 // in ms

ModeButton::ModeButton(int8_t Pin, uint16_t LongPressDelay, bool ActiveLow) : 
						_pin(Pin),
						_longPressDelay(LongPressDelay),
						_activeLow(ActiveLow)
{
	if(_activeLow)
	{
		pinMode(_pin, INPUT_PULLUP);
	}
	else
	{
		pinMode(_pin, INPUT);
	}
	_modeButtonEngineTimer.start(ENGINE_CYCLE);
}


ModeButton::button_mode ModeButton::getButtonMode()
{
	button_mode ModeRet = _lastMode;
	_lastMode = _actualMode;
	return ModeRet;
}

void ModeButton::modeButtonEngine()
{
	bool Press = false;
	if(_modeButtonEngineTimer.isOver(true))
	{
		Press = (bool)digitalRead(_pin);
		if(Press && !_longPressed)
		{
			_longPressTimer.start(_longPressDelay);
		}
		if(_longPressTimer.isOver() && Press && !_longPressed)
		{
			_actualMode = long_press;
			_lastMode = _actualMode;
			_longPressed = true;
		}
		else
		{
			if(!Press && _longPressTimer.isRunning())
			{
				_longPressTimer.stop();
				_longPressed = false;
				_actualMode = short_press;
				_lastMode = _actualMode;
			}
			if(_longPressed || !Press)
			{
				_actualMode = no_press;
			}
		}
	}
}