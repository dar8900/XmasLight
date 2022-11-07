#include "main_app.h"
#include "../SerialDebug/serial_debug.h"

#define MAX_BRIGHTNESS_PERC             100
#define DIMMING_TIME                    20000 // in ms
#define POT_SAMPLE                      50
#define POT_SAMPLING_RATE               100 // in ms
// #define SWITCH_LEDS_TIME                2000 // in ms
#define LED_DIMMING_ENGINE_CYCLE        80

// #define USE_POTENTIOMETER

#ifndef USE_POTENTIOMETER
#pragma message("Potentiometer not in use, enable it in main_app.cpp, line 11")
#endif

// #define TEST_PANIC_MODE
#ifdef TEST_PANIC_MODE
#pragma message("Panic test enabled!")
#endif

void MainApp::_panic(const char *PanicMsg)
{
    Timer PanicTimer;
    PanicTimer.start(500);
    _switchLightMode(manual_mode);
    _nightLedStripe->ledStripeEngine();
    _dayLedStripe->ledStripeEngine();
    _statusLed->setStatus(StatusLed::led_mode::error_mode);
    while(1)
    {
        if(PanicTimer.isOver(true))
        {
            if(PanicMsg)
            {
                Debug.logError("PANIC!!! Msg -> " + String(PanicMsg));
            }
            else
            {
                Debug.logError("PANIC!!!");
            }
        }
        _statusLed->ledEngine();
    }
}

void MainApp::_switchLightMode(light_mode NewMode)
{
    if(NewMode == auto_mode)
    {
        _lightsMode = auto_mode;
        _wichStripeWasOn = day_led;
        _nightLedStripe->setStatus(LedDimming::stripe_status::off_status, true);
        _dayLedStripe->setStatus(LedDimming::stripe_status::off_status, true);
        _nightLedStripe->setDimmingTime(DIMMING_TIME);
        _dayLedStripe->setDimmingTime(DIMMING_TIME);
        _nightLedStripe->setBrightness(MAX_BRIGHTNESS_PERC);
        _dayLedStripe->setBrightness(MAX_BRIGHTNESS_PERC);
        _statusLed->setStatus(StatusLed::led_mode::auto_switch_mode);
        Debug.logInfo("Switch in modalita automatica");
    }
    else if(NewMode == manual_mode)
    {
        _lightsMode = manual_mode;
        _manualLedSwitch = day_led;
        _nightLedStripe->setDimmingTime(NO_DIMMING);
        _dayLedStripe->setDimmingTime(NO_DIMMING);
        _nightLedStripe->setStatus(LedDimming::stripe_status::off_status, true);
        _dayLedStripe->setStatus(LedDimming::stripe_status::off_status, true);
        _statusLed->setStatus(StatusLed::led_mode::manual_switch_mode);
        Debug.logInfo("Switch in modalita manuale");
    }
}

void MainApp::_checkChangeMode()
{
    ButtonManager::button_press_mode SwitchMode = ButtonManager::button_press_mode::no_press;
    SwitchMode = _modeSwitch->getButtonMode();
    if(SwitchMode == ModeButton::button_mode::long_press)
    {
        light_mode NewMode;
        _lightsMode == auto_mode ? NewMode = manual_mode : NewMode = auto_mode;
        _switchLightMode(NewMode);
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
            _manualLedSwitch = all_off;
            Debug.logInfo("Switch manuale a tutto spento");
            break;
        case all_off:
            _manualLedSwitch = day_led;
            Debug.logInfo("Switch manuale giorno default");
            break;
        default:
            _panic();
            break;
        }
    }
}


void MainApp::_mangeAutoLedStripesSwitching()
{
    LedDimming::stripe_status DayLedStatus = _dayLedStripe->getStatus();
    LedDimming::stripe_status NightLedStatus = _nightLedStripe->getStatus();
    switch (DayLedStatus)
    {
    case LedDimming::stripe_status::on_status:
        switch (NightLedStatus)
        {
        case LedDimming::stripe_status::on_status:
            /* Never reach this state */
            _panic("Entrambe le strisce sono in modalità ON");
            break;
        case LedDimming::stripe_status::off_status:
            _wichStripeWasOn = day_led;
            _dayLedStripe->setStatus(LedDimming::stripe_status::off_status);
            break;
        default:
            _panic();
            break;
        }
        break;
    case LedDimming::stripe_status::off_status:
        switch (NightLedStatus)
        {
        case LedDimming::stripe_status::on_status:
            _wichStripeWasOn = night_led;
            _nightLedStripe->setStatus(LedDimming::stripe_status::off_status);
            break;
        case LedDimming::stripe_status::off_status:
            if(_wichStripeWasOn == night_led)
            {
                _dayLedStripe->setStatus(LedDimming::stripe_status::on_status);
            }
            else
            {
                _nightLedStripe->setStatus(LedDimming::stripe_status::on_status);
            }
            break;
        default:
            _panic();
            break;
        }
        break;
    default:
        _panic();
        break;
    }
}

void MainApp::_mangeManualLedStripesSwitching()
{
    LedDimming::stripe_status DayLedStatus = _dayLedStripe->getStatus();
    LedDimming::stripe_status NightLedStatus = _nightLedStripe->getStatus();
    switch (_manualLedSwitch)
    {
    case day_led:
        if(DayLedStatus != LedDimming::stripe_status::on_status)
        {
            Debug.logVerbose("Switch manuale ON giorno in corso...");
        }
        _dayLedStripe->setStatus(LedDimming::stripe_status::on_status);
        _nightLedStripe->setStatus(LedDimming::stripe_status::off_status);
        _wichStripeWasOn = night_led;
        break;
    case night_led:
        if(NightLedStatus != LedDimming::stripe_status::on_status)
        {
            Debug.logVerbose("Switch manuale ON notte in corso...");
        }
        _dayLedStripe->setStatus(LedDimming::stripe_status::off_status);
        _nightLedStripe->setStatus(LedDimming::stripe_status::on_status);
        _wichStripeWasOn = day_led;
        break;
    case all_off:
        if(DayLedStatus != LedDimming::stripe_status::off_status && NightLedStatus != LedDimming::stripe_status::off_status)
        {
            Debug.logVerbose("Spegnimento manuale led in corso...");
        }
        _dayLedStripe->setStatus(LedDimming::stripe_status::off_status);
        _nightLedStripe->setStatus(LedDimming::stripe_status::off_status);
        _wichStripeWasOn = all_off;
        break;        
    default:
        _panic();
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
    _modeSwitch->buttonEngine();
}


MainApp::MainApp()
{
    // _modeSwitch = new ModeButton(SWITCH_MODE, 1500, true);
    _modeSwitch = new ButtonManager();
    _dayLedStripe = new LedDimming(LED_DAY, DIMMING_TIME, MAX_BRIGHTNESS_PERC, "Led Giorno");
    _nightLedStripe = new LedDimming(LED_NIGHT, DIMMING_TIME, MAX_BRIGHTNESS_PERC, "Led Notte");
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
	Debug.logInfo("Versione FW: " + String(FW_VERSION));
    Debug.logInfo("Modalita di partenza: " + String(_lightsMode));
    _dayLedStripe->setEngineCycle(LED_DIMMING_ENGINE_CYCLE);
    _nightLedStripe->setEngineCycle(LED_DIMMING_ENGINE_CYCLE);
    _switchLightMode(auto_mode);
    _modeSwitch->setup(SWITCH_MODE, 2000, true);
    _statusLed->rapidBlink(50, 10);
    _mainAppCyle.start(100);
#ifdef TEST_PANIC_MODE
    _panic();
#endif
}

void MainApp::runApp()
{
    if(_mainAppCyle.isOver(true))
    {
        _checkChangeMode();
        if((!_dayLedStripe->ledSwitching() && !_nightLedStripe->ledSwitching()) && 
            _lightsMode == auto_mode)
        {
            Debug.logDebug("Auto mode managament");
            _mangeAutoLedStripesSwitching();
        }
        else if(_lightsMode == manual_mode)
        {
            _mangeManualLedStripesSwitching();
        }
    }
	_execEngines();
}