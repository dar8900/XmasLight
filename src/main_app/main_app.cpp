#include "main_app.h"
#include "../int_button/int_button.h"
#include "../led_stripes/led_stripes.h"
#include "../potentiometer/potentiometer.h"
#include "../status_led/status_led.h"

#define MAX_BRIGHTNESS_PERC             100
#define DIMMING_TIME                    20 // in s
#define POT_SAMPLE                      50
#define POT_SAMPLING_RATE               100 // in ms
#define SWITCH_LEDS_TIME                2000 // in ms


void MainApp::_checkChangeMode()
{
    bool SwitchMode = false;
    SwitchMode = _modeSwitch->isTriggered();
    if(SwitchMode)
    {
        if(_lightsMode == auto_mode)
        {
            _lightsMode = manual_mode;
            _switchDayNightTimer.stop();
        }
        else
        {
            _lightsMode = auto_mode;
        }
    }
}


void MainApp::_mangeLedStripesSwitching()
{
    if(_lightsMode == auto_mode)
    {
        if(_dayLedStripe->getStatus() == LedStripe::stripe_status::off_status &&
            _nightLedStripe->getStatus() == LedStripe::stripe_status::on_status)
        {
            _wichStripeWasOn = night_led;
            _switchDayNightTimer.start(SWITCH_LEDS_TIME);
            if(_switchDayNightTimer.isOver())
            {
                _dayLedStripe->setStatus(LedStripe::stripe_status::on_status);
            }
        }
        else if(_dayLedStripe->getStatus() == LedStripe::stripe_status::on_status &&
            _nightLedStripe->getStatus() == LedStripe::stripe_status::off_status)
        {
            _wichStripeWasOn = day_led;
            _switchDayNightTimer.start(SWITCH_LEDS_TIME);
            if(_switchDayNightTimer.isOver())
            {
                _nightLedStripe->setStatus(LedStripe::stripe_status::on_status);
            }
        }
        else if(_dayLedStripe->getStatus() == LedStripe::stripe_status::on_status &&
            _nightLedStripe->getStatus() == LedStripe::stripe_status::on_status)
        {
            _switchDayNightTimer.start(SWITCH_LEDS_TIME);
            if(_switchDayNightTimer.isOver())
            {
                if(_wichStripeWasOn == day_led)
                {
                    _dayLedStripe->setStatus(LedStripe::stripe_status::off_status);
                }
                else if(_wichStripeWasOn == night_led)
                {
                    _nightLedStripe->setStatus(LedStripe::stripe_status::off_status);
                }
            }
        }
        else if(_dayLedStripe->getStatus() == LedStripe::stripe_status::off_status &&
            _nightLedStripe->getStatus() == LedStripe::stripe_status::off_status)
        {
            _dayLedStripe->setStatus(LedStripe::stripe_status::on_status);
        }                
    }
    else
    {
        
    }
}






MainApp::MainApp() : _modeSwitch(new IntButton(SWITCH_MODE)),
                     _dayLedStripe(new LedStripe(LED_DAY, DIMMING_TIME, MAX_BRIGHTNESS_PERC)),
                     _nightLedStripe(new LedStripe(LED_NIGHT, DIMMING_TIME, MAX_BRIGHTNESS_PERC)),
                     _pot(new Potenziometer(POTENTIOMETER, POT_SAMPLE, POT_SAMPLING_RATE)),
                     _statusLed(new StatusLed(STATUS_LED))
{

}

MainApp::~MainApp()
{
    delete _modeSwitch;
    delete _dayLedStripe;
    delete _nightLedStripe;
    delete _pot;
    delete _statusLed;
}


void MainApp::setupApp()
{
    
}

void MainApp::runApp()
{
    _checkChangeMode();
    _mangeLedStripesSwitching();

    _dayLedStripe->ledStripeEngine();
    _nightLedStripe->ledStripeEngine();
    _pot->analogReadEngine();
    _statusLed->ledEngine();
}