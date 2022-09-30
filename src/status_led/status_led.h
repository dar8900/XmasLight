#ifndef STATUS_LED_h
#define STATUS_LED_H
#include <Arduino.h>
#include <stdint.h>
#include "../TimerLib/Timer.hpp"

class StatusLed
{
	public:
		typedef enum
		{
			booting_mode = 0,
			error_mode,
			manual_switch_mode,
			auto_switch_mode,
			off_mode
		}led_mode;

	private:
		int8_t _pin = -1;
		uint8_t _cnt = 0;
		bool _ledStatus = false;
		led_mode _actualStatus = off;
		led_mode _oldStatus = off;
		Timer _ledSwichTimer;
		void _setLedDigital(bool Status);
		void _rapidBlink();
		void _bootMode();
		void _errorMode();
		void _manualSwMode();
		void _autoSwMode();
		void _offMode();

	public:
		StatusLed(int8_t Pin);
		void setStatus(led_mode NewStatus);
		void ledEngine();

};

#endif