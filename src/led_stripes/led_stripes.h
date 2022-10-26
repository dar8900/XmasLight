#ifndef LED_STRIPES_H
#define LED_STRIPES_H
#include <Arduino.h>
#include <stdint.h>
#include "../TimerLib/Timer.hpp"

#define NO_DIMMING			0

class LedStripe
{
	public:
		typedef enum
		{
			off_status = 0,
			on_status
		}stripe_status;

	private:
		const uint16_t _DIMMING_CYCLE_DFTL = 100; // in ms
		uint16_t _engineCycle = _DIMMING_CYCLE_DFTL; // in ms
		int8_t _pin = -1;
		uint16_t _brightnessTarget = 0;
		uint16_t _actualBrightness = 0;
		uint16_t _dimmingTime = 0;
		stripe_status _actualStatus = off_status;
		stripe_status _targetStatus = off_status;
		Timer _engineTimer;
		bool _stripeIsSwitching = false;

	public:
		const uint8_t MAX_BRIGHTNESS = 100; // Massima luminosità percentuale

		LedStripe(int8_t Pin, uint16_t DimmingTime, uint8_t MaxBrightnessPercent);
		/**
		 * @brief Imposta il tempo di dimming tra accensione e spegnimento (in ms)
		 * 
		 * @param uint16_t Time 
		 */
		void setDimmingTime(uint16_t Time);

		/**
		 * @brief Imposta il nuovo stato da raggiungere dopo il dimming
		 * 			se Fast imposta subito il nuovo valore senza passare dall'engine
		 * 
		 * @param stripe_status NewStatus 
		 * @param bool Fast 
		 */
		void setStatus(stripe_status NewStatus, bool Fast = false);

		/**
		 * @brief Restituisce lo stato attuale della striscia
		 * 
		 * @return stripe_status ActualStatus 
		 */
		stripe_status getStatus();

		/**
		 * @brief Informa se la striscia sta cambiando
		 * 
		 * @return bool
		 */
		bool ledSwitching();

		/**
		 * @brief Imposta la massima luminostà da raggiungere in percentuale
		 * 
		 * @param uint8_t NewBrightness 
		 */
		void setBrightness(uint8_t NewBrightnessPercent);

		/**
		 * @brief Esegue il motore per la gestione del dimming e del cambio stato
		 * 
		 */
		void ledStripeEngine();

};

#endif