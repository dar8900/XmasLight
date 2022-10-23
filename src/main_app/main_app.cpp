#include "main_app.h"
#include "../SerialDebug/serial_debug.h"

#define MAX_BRIGHTNESS_PERC             100
#define DIMMING_TIME                    20000 // in ms
#define POT_SAMPLE                      50
#define POT_SAMPLING_RATE               100 // in ms
#define SWITCH_LEDS_TIME                2000 // in ms

static SerialDebug Debug(SerialDebug::baudrate::baud_9600, true);

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
        }
        else
        {
            _lightsMode = auto_mode;
            _nightLedStripe->setDimmingTime(DIMMING_TIME);
            _dayLedStripe->setDimmingTime(DIMMING_TIME);
            _dayLedStripe->setBrightness(MAX_BRIGHTNESS_PERC);
            _nightLedStripe->setBrightness(MAX_BRIGHTNESS_PERC);
            Debug.logInfo("Switch in modalita automatica");
        }
    }
    else if(SwitchMode == ModeButton::button_mode::short_press && _lightsMode == manual_mode)
    {
        switch (_manualLedSwitch)
        {
        case day_led:
            _manualLedSwitch = night_led;
            break;
        case night_led:
            _manualLedSwitch = day_led;
            break;
        default:
            _manualLedSwitch = day_led;
            break;
        }
        Debug.logInfo("Modifica manualmente la modalita dei led");
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
        switch (_manualLedSwitch)
        {
        case day_led:
            _dayLedStripe->setStatus(LedStripe::stripe_status::on_status);
            _nightLedStripe->setStatus(LedStripe::stripe_status::off_status);
            _wichStripeWasOn = day_led;
            break;
        case night_led:
            _dayLedStripe->setStatus(LedStripe::stripe_status::off_status);
            _nightLedStripe->setStatus(LedStripe::stripe_status::on_status);
            _wichStripeWasOn = night_led;
            break;
        default:
            break;
        }       
    }
}



void MainApp::_collectPotBrightness()
{
    uint16_t PotAnalogVal = 0;
    PotAnalogVal = _pot->getAnalogVal();
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
    _modeSwitch = new ModeButton(SWITCH_MODE);
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
	_lightsMode = auto_mode;
	_oldLightMode = auto_mode;
	_wichStripeWasOn = all_off;
	_manualLedSwitch = all_off;
	_potManualModeBrightness = 0; 
    Debug.init();
	Debug.setLogStatus(true);
	Debug.setTimePrint(false);
	Debug.setDebugLevel(SerialDebug::debug_level::all);
	Debug.logInfo("Application started");
    for(int i = 0; i < 5; i++)
        _statusLed->rapidBlink(250);
}

void MainApp::runApp()
{
	_checkChangeMode();
	_mangeLedStripesSwitching();
	_execEngines();
}