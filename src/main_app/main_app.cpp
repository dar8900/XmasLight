#include "main_app.h"
#include "../SerialDebug/serial_debug.h"

#define MAX_BRIGHTNESS_PERC             100
#define DIMMING_TIME                    20000 // in ms
#define POT_SAMPLE                      50
#define POT_SAMPLING_RATE               100 // in ms
#define SWITCH_LEDS_TIME                2000 // in ms

// #define USE_POTENTIOMETER

#ifndef USE_POTENTIOMETER
#pragma message("Potentiometer not in use, enable it in main_app.cpp, line 10")
#endif

void MainApp::_checkChangeMode()
{
    ModeButton::button_mode SwitchMode = ModeButton::button_mode::no_press;
    SwitchMode = _modeSwitch->getButtonMode();
    if(SwitchMode == ModeButton::button_mode::long_press)
    {
        if(_lightsMode == auto_mode)
        {
            _lightsMode = manual_mode;
            _manualLedSwitch = day_led;
            _switchDayNightTimer.stop();
            _nightLedStripe->setDimmingTime(NO_DIMMING);
            _dayLedStripe->setDimmingTime(NO_DIMMING);
            Debug.logInfo("Switch in modalita manuale");
            _statusLed->setStatus(StatusLed::led_mode::manual_switch_mode);
        }
        else
        {
            _lightsMode = auto_mode;
            _wichStripeWasOn = all_off;
            _nightLedStripe->setStatus(LedStripe::stripe_status::off_status, true);
            _dayLedStripe->setStatus(LedStripe::stripe_status::off_status, true);
            _wichStripeWasOn = day_led;
            _nightLedStripe->setDimmingTime(DIMMING_TIME);
            _dayLedStripe->setDimmingTime(DIMMING_TIME);
            _nightLedStripe->setBrightness(MAX_BRIGHTNESS_PERC);
            _dayLedStripe->setBrightness(MAX_BRIGHTNESS_PERC);
            Debug.logInfo("Switch in modalita automatica");
            _statusLed->setStatus(StatusLed::led_mode::auto_switch_mode);
        }
    }
    else if(SwitchMode == ModeButton::button_mode::short_press && _lightsMode == manual_mode)
    {
        switch (_manualLedSwitch)
        {
        case day_led:
            _manualLedSwitch = night_led;
            Debug.logInfo("Switch manuale notte");
            break;
        case night_led:
            _manualLedSwitch = day_led;
            Debug.logInfo("Switch manuale giorno");
            break;
        default:
            _manualLedSwitch = day_led;
            Debug.logInfo("Switch manuale giorno default");
            break;
        }
    }
}


void MainApp::_mangeAutoLedStripesSwitching()
{
    LedStripe::stripe_status DayLedStatus = _dayLedStripe->getStatus();
    LedStripe::stripe_status NightLedStatus = _nightLedStripe->getStatus();

    switch (DayLedStatus)
    {
    case LedStripe::stripe_status::on_status:
        switch (NightLedStatus)
        {
        case LedStripe::stripe_status::off_status:
            _dayLedStripe->setStatus(LedStripe::stripe_status::off_status);
            _wichStripeWasOn = day_led;
            break;
        default:
            break;
        }
        break;
    case LedStripe::stripe_status::off_status:
        switch (NightLedStatus)
        {
        case LedStripe::stripe_status::off_status:
                if(_wichStripeWasOn == night_led)
                {
                    _dayLedStripe->setStatus(LedStripe::stripe_status::on_status);
                }
                else if(_wichStripeWasOn == day_led)
                {
                    _nightLedStripe->setStatus(LedStripe::stripe_status::on_status);
                    _wichStripeWasOn = night_led;
                }
            break;
        case LedStripe::stripe_status::on_status:
            _nightLedStripe->setStatus(LedStripe::stripe_status::off_status);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }       
}

void MainApp::_mangeManualLedStripesSwitching()
{
    switch (_manualLedSwitch)
    {
    case day_led:
        if(_dayLedStripe->getStatus() != LedStripe::stripe_status::on_status)
        {
            Debug.logVerbose("Switch manuale ON giorno in corso...");
        }
        _dayLedStripe->setStatus(LedStripe::stripe_status::on_status);
        _nightLedStripe->setStatus(LedStripe::stripe_status::off_status);
        _wichStripeWasOn = night_led;
        break;
    case night_led:
        if(_nightLedStripe->getStatus() != LedStripe::stripe_status::on_status)
        {
            Debug.logVerbose("Switch manuale ON notte in corso...");
        }
        _dayLedStripe->setStatus(LedStripe::stripe_status::off_status);
        _nightLedStripe->setStatus(LedStripe::stripe_status::on_status);
        _wichStripeWasOn = day_led;
        break;
    default:
        break;
    }    
}


void MainApp::_collectPotBrightness()
{
    uint16_t PotAnalogVal = 0;
#ifdef USE_POTENTIOMETER
    PotAnalogVal = _pot->getAnalogVal();
#else    
    PotAnalogVal = MAX_ANALOG_VAL;
#endif
    _potManualModeBrightness = (PotAnalogVal * MAX_BRIGHTNESS_PERC) / MAX_ANALOG_VAL;
    if(_potManualModeBrightness > MAX_BRIGHTNESS_PERC)
    {
        _potManualModeBrightness = MAX_BRIGHTNESS_PERC;
    }

}

void MainApp::_execEngines()
{
    if(_lightsMode == manual_mode)
    {
        _collectPotBrightness();
        _dayLedStripe->setBrightness(_potManualModeBrightness);
        _nightLedStripe->setBrightness(_potManualModeBrightness);
    }
    _dayLedStripe->ledStripeEngine();
    _nightLedStripe->ledStripeEngine();
    _pot->analogReadEngine();
    _statusLed->ledEngine();
    _modeSwitch->modeButtonEngine();
}


MainApp::MainApp()
{
    _modeSwitch = new ModeButton(SWITCH_MODE, 1500, true);
    _dayLedStripe = new LedStripe(LED_DAY, DIMMING_TIME, MAX_BRIGHTNESS_PERC);
    _nightLedStripe = new LedStripe(LED_NIGHT, DIMMING_TIME, MAX_BRIGHTNESS_PERC);
    _pot = new Potenziometer(POTENTIOMETER, POT_SAMPLE, POT_SAMPLING_RATE);
    _statusLed = new StatusLed(STATUS_LED);
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
	_lightsMode = manual_mode;
	_wichStripeWasOn = all_off;
	_manualLedSwitch = all_off;
	_potManualModeBrightness = 0;
    Debug.init();
	Debug.setLogStatus(true);
	Debug.setTimePrint(true);
	Debug.setDebugLevel(SerialDebug::debug_level::all);
    Debug.logInfo("");
    Debug.logInfo("##################   XSMAS CRECHE    ##################");
    Debug.logInfo("");
	Debug.logInfo("Applicazione partita");
    Debug.logInfo("Modalita di partenza: " + String(_lightsMode));
    if(_lightsMode == manual_mode)
    {
        _nightLedStripe->setStatus(LedStripe::stripe_status::off_status);
        _dayLedStripe->setStatus(LedStripe::stripe_status::off_status);
        _nightLedStripe->setBrightness(MAX_BRIGHTNESS_PERC);
        _dayLedStripe->setBrightness(MAX_BRIGHTNESS_PERC);
        _nightLedStripe->setDimmingTime(NO_DIMMING);
        _dayLedStripe->setDimmingTime(NO_DIMMING);
        _manualLedSwitch = day_led;
        _statusLed->setStatus(StatusLed::led_mode::manual_switch_mode);
    }
    else if(_lightsMode == auto_mode)
    {
        _nightLedStripe->setDimmingTime(DIMMING_TIME);
        _dayLedStripe->setDimmingTime(DIMMING_TIME);
        _nightLedStripe->setStatus(LedStripe::stripe_status::off_status, true);
        _dayLedStripe->setStatus(LedStripe::stripe_status::off_status, true);
        _wichStripeWasOn = day_led;
        _nightLedStripe->setBrightness(MAX_BRIGHTNESS_PERC);
        _dayLedStripe->setBrightness(MAX_BRIGHTNESS_PERC);
        _statusLed->setStatus(StatusLed::led_mode::auto_switch_mode);    
    }
    _statusLed->rapidBlink(50, 20);
}

void MainApp::runApp()
{
	_checkChangeMode();
    if((!_dayLedStripe->ledSwitching() && !_nightLedStripe->ledSwitching()) && 
        _lightsMode == auto_mode)
    {
        _switchDayNightTimer.start(SWITCH_LEDS_TIME);
        if(_switchDayNightTimer.isOver())
        {
	        _mangeAutoLedStripesSwitching();
        }
    }
    else if(_lightsMode == manual_mode)
    {
        _mangeManualLedStripesSwitching();
    }
	_execEngines();
}