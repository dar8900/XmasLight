#ifndef STATUS_LED_H
#define STATUS_LED_H
#include <Arduino.h>
#include <stdint.h>
#include "../TimerLib/Timer.hpp"

class StatusLed
{
	public:
		typedef enum
		{
			error_mode,
			manual_switch_mode,
			auto_switch_mode,
			off_mode
		}led_mode;

	private:
		int8_t _pin = -1;
		uint8_t _cnt = 0;
		bool _ledStatus = false;
		led_mode _actualStatus = off_mode;
		led_mode _oldStatus = off_mode;
		Timer _ledSwichTimer;
		void _setLedDigital(bool Status);
		void _toggleLed();

	public:
		StatusLed(int8_t Pin);
		void rapidBlink(uint16_t Delay = 25, uint16_t BlinkTimes = 0);
		void setStatus(led_mode NewStatus);
		void ledEngine();

};

#endif