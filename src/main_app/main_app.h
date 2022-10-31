#ifndef MAIN_APP_H
#define MAIN_APP_H
#include <Arduino.h>
#include <stdint.h>
#include "pinout.h"
#include "../mode_button/mode_button.h"
#include "../led_stripes/led_stripes.h"
#include "../potentiometer/potentiometer.h"
#include "../status_led/status_led.h"
#include "../TimerLib/Timer.hpp"
#include "../ButtonLib/ButtonLib.h"

#define FW_VERSION      "0.4b"

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
        }on_led;

        // ModeButton *_modeSwitch;
        LedStripe *_dayLedStripe;
        LedStripe *_nightLedStripe;
        Potenziometer *_pot;
        StatusLed *_statusLed;
        light_mode _lightsMode = auto_mode;
        ButtonManager *_modeSwitch;
        on_led _wichStripeWasOn = all_off;
        on_led _manualLedSwitch = all_off;
        uint16_t _potManualModeBrightness = 0;
        Timer _mainAppCyle;

        void _switchLightMode(light_mode NewMode);
        void _checkChangeMode();
        void _mangeAutoLedStripesSwitching();
        void _mangeManualLedStripesSwitching();
        void _collectPotBrightness();
        void _execEngines();
        void _panic(const char *PanicMsg = NULL);

    public:
        MainApp();
        ~MainApp();
        void setupApp();
        void runApp();
};

#endif