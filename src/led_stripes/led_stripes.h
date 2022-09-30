#ifndef LED_STRIPES_H
#define LED_STRIPES_H
#include <Arduino.h>
#include <stdint.h>
#include "../TimerLib/Timer.hpp"

class LedStripe
{
	public:
	typedef enum
	{
		off_status = 0,
		on_status
	}stripe_status;

	private:
		const uint16_t ENGINE_CYCLE = 50; // in ms
		int8_t _pin = -1;
		uint16_t _brightnessTarget = 0;
		uint16_t _actualBrightness = 0;
		uint16_t _dimmingTime = 0;
		stripe_status _actualStatus = off_status;
		stripe_status _targetStatus = off_status;
		Timer _engineTimer;

	public:
		const uint16_t MAX_BRIGHTNESS = 1023; // Massima luminosità

		LedStripe(int8_t Pin, uint16_t DimmingTime, uint16_t MaxBrightness);
		/**
		 * @brief Imposta il tempo di dimming tra accensione e spegnimento (in ms)
		 * 
		 * @param uint16_t Time 
		 */
		void setDimmingTime(uint16_t Time);

		/**
		 * @brief Imposta il nuovo stato da raggiungere dopo il dimming
		 * 
		 * @param stripe_status NewStatus 
		 */
		void setStatus(stripe_status NewStatus);

		/**
		 * @brief Imposta la massima luminostà da raggiungere
		 * 
		 * @param uint16_t NewBrightness 
		 */
		void setBrightness(uint16_t NewBrightness);

		/**
		 * @brief Esegue il motore per la gestione del dimming e del cambio stato
		 * 
		 */
		void runEngine();

};

#endif