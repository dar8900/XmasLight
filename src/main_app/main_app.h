#ifndef MAIN_APP_H
#define MAIN_APP_H
#include <Arduino.h>
#include <stdint.h>
#include "pinout.h"
#include "../int_button/int_button.h"
#include "../led_stripes/led_stripes.h"
#include "../potentiometer/potentiometer.h"
#include "../status_led/status_led.h"
#include "../TimerLib/Timer.hpp"

class MainApp
{
    private:
        typedef enum
        {
            manual_mode = 0,
            auto_mode
        }light_mode;

        typedef enum
        {
            night_led = 0,
            day_led,
            all_off
        }prev_on_led;

        IntButton *_modeSwitch;
        LedStripe *_dayLedStripe;
        LedStripe *_nightLedStripe;
        Potenziometer *_pot;
        StatusLed *_statusLed;
        light_mode _lightsMode = auto_mode;
        light_mode _oldLightMode = auto_mode;
        Timer _switchDayNightTimer;
        prev_on_led _wichStripeWasOn = all_off;

        void _checkChangeMode();
        void _mangeLedStripesSwitching();

    public:
        MainApp();
        ~MainApp();
        void setupApp();
        void runApp();
};

#endif