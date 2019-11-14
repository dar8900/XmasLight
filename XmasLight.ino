/* https://raw.githubusercontent.com/damellis/attiny/ide-1.6.x-boards-manager/package_damellis_attiny_index.json */

#include <stdint.h>
#include <Chrono.h>

#define TEST_STRIPS	

#define ON 			  true
#define OFF			  false

#define NIGHT_LED_STRIP   	 0
#define DAY_LED_STRIP   	 1
#define STATUS_LED 	 		 2
#define CHANGE_MODE	 		 3

#define NO_STRIPE_ON  		10

#define SWITCH_STRIP_TIME 	1

#define PERCENT(Perc)		((Perc * 255)/100) // Questa è una macro per converire il fading in percentuale

#ifdef TEST_STRIPS
void TestLedStrip(void);
#endif

// Associo dei numeri alle varie modalità operative
enum
{
	NIGHT_MODE = 0,
	DAY_MODE,
	SWITCH_MODE,
	MAX_MODES
};

Chrono SwitchLedStripe(Chrono::SECONDS);
uint8_t WichStripeIsOn;
uint8_t LedStripeMode = SWITCH_MODE;

// Funzione per gestire il led di stato
static void BlinkLed(int WichLed, int Delay, int Times)
{
	for(int i = 0; i < Times; i++)
	{
		digitalWrite(WichLed, HIGH);
		delay(Delay);
		digitalWrite(WichLed, LOW);
		delay(Delay);
	}	
}

// Funzione per gestire i singoli pin digitali
static void TurnPin(int WichLed, bool Status)
{
	if(Status)
		digitalWrite(WichLed, HIGH);
	else
		digitalWrite(WichLed, LOW);
}

// Funzione per gestire le uscite "analogiche" PWM
static void TurnAnalogPin(int WichLed, int Percent)
{
	analogWrite(WichLed, PERCENT(Percent));
}
 
// Funzione per la gestione dello switch tra giorno e notte
static void FadeLedStrips()
{
	uint8_t LedToTurnOn = 0, LedToTurnOff = 0;
	if(WichStripeIsOn == NIGHT_LED_STRIP)
	{
		LedToTurnOn = DAY_LED_STRIP;
		LedToTurnOff = NIGHT_LED_STRIP;
	}
	else
	{
		LedToTurnOn = NIGHT_LED_STRIP;
		LedToTurnOff = DAY_LED_STRIP;
	}
	// Lo switch avviene in maniera soft ovvero per passsare da una
	// modalita all'altra ci mette 2.5s (25ms * 100)
	for(int i = 0; i < 101; i++)
	{
		TurnAnalogPin(LedToTurnOn, i);
		TurnAnalogPin(LedToTurnOff, 100 - i);
		delay(25);
	}
	WichStripeIsOn = LedToTurnOn;
}


void setup()
{
	// Inizializzo variabile per il cronometro
	SwitchLedStripe.restart();
	SwitchLedStripe.stop();

	// Inizializzo i vari pin che mi serviranno
	pinMode(NIGHT_LED_STRIP, OUTPUT);
	pinMode(DAY_LED_STRIP, OUTPUT);
	pinMode(STATUS_LED, OUTPUT);
	pinMode(CHANGE_MODE, INPUT);

	// Inizializzo la modalità di avvio 
	// default: modalità switch
	TurnPin(STATUS_LED, ON);	
	TurnAnalogPin(NIGHT_LED_STRIP, 100);	
	TurnAnalogPin(DAY_LED_STRIP, 0);	
	SwitchLedStripe.restart();
	WichStripeIsOn = NIGHT_LED_STRIP;
	LedStripeMode = SWITCH_MODE;
}


void loop()
{

#ifndef TEST_STRIPS	
	// Gestione del pulsante che consente il cambio di modalità
	if(digitalRead(CHANGE_MODE) == HIGH)
	{
		if(LedStripeMode < MAX_MODES - 1)
			LedStripeMode++;
		else
			LedStripeMode = NIGHT_MODE;
		delay(20);
	}

	// Macchina a stati per la gestione delle varie modalità 
	switch(LedStripeMode)
	{
		case NIGHT_MODE:
			if(WichStripeIsOn != NIGHT_LED_STRIP)
			{
				WichStripeIsOn = NIGHT_LED_STRIP;
				BlinkLed(STATUS_LED, 250, 2);
				TurnPin(STATUS_LED, OFF);				
				TurnAnalogPin(NIGHT_LED_STRIP, 100);
				TurnAnalogPin(DAY_LED_STRIP, 0);
			}
			SwitchLedStripe.restart();
			break;
		case DAY_MODE:
			if(WichStripeIsOn != DAY_LED_STRIP)
			{
				WichStripeIsOn = DAY_LED_STRIP;
				BlinkLed(STATUS_LED, 250, 3);
				TurnPin(STATUS_LED, OFF);				
				TurnAnalogPin(DAY_LED_STRIP, 100);
				TurnAnalogPin(NIGHT_LED_STRIP, 0);
			}	
			SwitchLedStripe.restart();	
			break;
		case SWITCH_MODE:
			if(SwitchLedStripe.hasPassed(90, true)) // 90s per lo switch
			{
				FadeLedStrips();
			}
			TurnPin(STATUS_LED, ON);					
			break;
		default:
			break;
	}
#else
	TestLedStrip();
#endif
}


#ifdef TEST_STRIPS
void TestLedStrip()
{
	// Gestione del pulsante che consente il cambio di modalità
	if(digitalRead(CHANGE_MODE) == HIGH)
	{
		if(LedStripeMode < MAX_MODES - 1)
			LedStripeMode++;
		else
			LedStripeMode = NIGHT_MODE;
		TurnAnalogPin(NIGHT_LED_STRIP, 0);
		TurnAnalogPin(DAY_LED_STRIP, 0);	
		delay(20);	
	}
	switch(LedStripeMode)
	{
		case NIGHT_MODE:
			BlinkLed(STATUS_LED, 500, 2);		
			for(int i = 0; i < 101; i++)
			{
				TurnAnalogPin(NIGHT_LED_STRIP, i);
				TurnAnalogPin(DAY_LED_STRIP, i);
				delay(50);
			}		
			break;
		case DAY_MODE:
			BlinkLed(STATUS_LED, 500, 4);			
			for(int i = 100; i >= 0; i--)
			{
				TurnAnalogPin(NIGHT_LED_STRIP, i);
				TurnAnalogPin(DAY_LED_STRIP, i);
				delay(50);
			}			
			break;
		case SWITCH_MODE:
			BlinkLed(STATUS_LED, 500, 6);			
			TurnAnalogPin(NIGHT_LED_STRIP, 100);
			TurnAnalogPin(DAY_LED_STRIP, 0);
			for(int i = 0; i < 101; i++)
			{
				TurnAnalogPin(NIGHT_LED_STRIP, 100 - i);
				TurnAnalogPin(DAY_LED_STRIP, i);
				delay(50);
			}				
			break;
		default:
			break;
	}
}
#endif