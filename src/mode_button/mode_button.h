#ifndef MODE_BUTTON_H
#define MODE_BUTTON_H
#include <Arduino.h>
#include <stdint.h>
#include "../TimerLib/Timer.hpp"

#define LONG_PRESS_DELAY_DFLT			1500 // in ms

class ModeButton
{
	public:
		typedef enum
		{
			short_press = 0,
			long_press,
			no_press
		}button_mode;

		ModeButton(int8_t Pin, uint16_t LongPressDelay = LONG_PRESS_DELAY_DFLT, bool ActiveLow = false);
		button_mode getButtonMode();
		void modeButtonEngine();


	private:
		int8_t _pin = -1;
		bool _activeLow = false;
		uint16_t _longPressDelay = 0;
		button_mode _actualMode = no_press;
		button_mode _lastMode = no_press;
		bool _longPressed = false;
		Timer _longPressTimer;
		Timer _modeButtonEngineTimer;

};

#endif