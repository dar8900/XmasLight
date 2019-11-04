/* https://raw.githubusercontent.com/damellis/attiny/ide-1.6.x-boards-manager/package_damellis_attiny_index.json */

#include <stdint.h>
#include <Chrono.h>

#define ON 			  true
#define OFF			  false

#define LED_STRIP_1   		 0
#define LED_STRIP_2   		 1
#define STATUS_LED 	 		 2
#define TURN_OFF 	 		 3

#define NO_STRIPE_ON  		10

#define SWITCH_STRIP_TIME 	1

#define PERCENT(Perc)		((Perc * 255)/100)
#define MINUTE_TO_SECS(Min)	(Min * 60)

Chrono SwitchLedStripe(Chrono::SECONDS);
uint8_t WichStripeIsOn;
bool TurnOffAll = true;

static void BlinkLed(int WichLed, int Delay)
{
	digitalWrite(WichLed, HIGH);
	delay(Delay);
	digitalWrite(WichLed, LOW);
	delay(Delay);	
}

static void TurnPin(int WichLed, bool Status)
{
	if(Status)
		digitalWrite(WichLed, HIGH);
	else
		digitalWrite(WichLed, LOW);
}

static void TurnAnalogPin(int WichLed, int Percent)
{
	analogWrite(WichLed, PERCENT(Percent));
}

static void FadeLedStrips()
{
	uint8_t LedToTurnOn = 0, LedToTurnOff = 0;
	if(WichStripeIsOn == LED_STRIP_1)
	{
		LedToTurnOn = LED_STRIP_2;
		LedToTurnOff = LED_STRIP_1;
	}
	else
	{
		LedToTurnOn = LED_STRIP_1;
		LedToTurnOff = LED_STRIP_2;
	}
	for(int i = 0; i < 101; i++)
	{
		TurnAnalogPin(LedToTurnOn, i);
		TurnAnalogPin(LedToTurnOff, 100 - i);
		delay(25);
	}
	WichStripeIsOn = LedToTurnOn;
}

static void TurnStatusLed(bool Status)
{
	for(int i = 0; i < 5; i++)
		BlinkLed(STATUS_LED, 50);
	TurnPin(STATUS_LED, Status);	
}

void setup()
{
	SwitchLedStripe.restart();
	SwitchLedStripe.stop();
	pinMode(LED_STRIP_1, OUTPUT);
	pinMode(LED_STRIP_2, OUTPUT);
	pinMode(STATUS_LED, OUTPUT);
	pinMode(TURN_OFF, INPUT);
	if(!TurnOffAll)
	{
		TurnStatusLed(ON);
		TurnAnalogPin(LED_STRIP_1, 100);	
		TurnAnalogPin(LED_STRIP_2, 0);	
		SwitchLedStripe.restart();
		WichStripeIsOn = LED_STRIP_1;
	}
	else
	{
		TurnStatusLed(OFF);
		TurnAnalogPin(LED_STRIP_1, 0);
		TurnAnalogPin(LED_STRIP_2, 0);
		WichStripeIsOn = NO_STRIPE_ON;		
	}
}


void loop()
{
	if(digitalRead(TURN_OFF) == HIGH)
	{
		TurnOffAll = !TurnOffAll;
		if(TurnOffAll)
			TurnStatusLed(OFF);
		else
			TurnStatusLed(ON);
	}
	if(!TurnOffAll)
	{
		if(WichStripeIsOn == NO_STRIPE_ON)
		{
			WichStripeIsOn = LED_STRIP_1;
			TurnAnalogPin(LED_STRIP_1, 100);
		}
		if(SwitchLedStripe.hasPassed(MINUTE_TO_SECS(SWITCH_STRIP_TIME), true))
		{
			FadeLedStrips();
		}
	}
	else
	{
		SwitchLedStripe.restart();
		if(WichStripeIsOn != NO_STRIPE_ON)
		{
			TurnAnalogPin(LED_STRIP_1, 0);
			TurnAnalogPin(LED_STRIP_2, 0);
		}
		WichStripeIsOn = NO_STRIPE_ON;
	}

}
