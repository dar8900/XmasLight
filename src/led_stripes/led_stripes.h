#ifndef LED_STRIPES_H
#define LED_STRIPES_H
#include <Arduino.h>
#include <stdint.h>

class LED_STRIPE
{
	public:
	typedef enum
	{
		off_status = 0,
		on_status
	}stripe_status;

	private:
		int8_t _pin = -1;
		uint16_t _brightnessTarget = 0;
		uint16_t _actualBrightness = 0;
		uint16_t _dimmingTime = 0;

	public:
		const uint16_t MAX_BRIGHTNESS = 1023;
		void setDimmingTime(uint16_t Time);
		void setStatus(stripe_status NewStatus);
		void setBrightness(uint16_t NewBrightness);
		void runEngine();

};

#endif