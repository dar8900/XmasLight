/* https://raw.githubusercontent.com/damellis/attiny/ide-1.6.x-boards-manager/package_damellis_attiny_index.json */

#include <stdint.h>
#include <Chrono.h>

#undef TEST_STRIPS	

#undef POT_FADING	

#define ON 			  true
#define OFF			  false

#define NIGHT_LED_STRIP   	 0
#define DAY_LED_STRIP   	 1
#define STATUS_LED 	 		 4
#define CHANGE_MODE	 		 3

#ifdef POT_FADING
#define POTENTIOMETER		 A2
#endif

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

#ifdef POT_FADING
int PotPercent = 0;
#endif

#ifndef TEST_STRIPS
void CheckButton();
int WichMode = 0;
#endif

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
	// modalita all'altra ci mette 5s (25ms * 100)
	for(int i = 100; i >= 0; i--)
	{
		TurnAnalogPin(LedToTurnOff, i);
		CheckButton();
		delay(25);
	}	
	for(int i = 0; i < 101; i++)
	{
		TurnAnalogPin(LedToTurnOn, i);
		CheckButton();
		delay(25);
	}
	WichStripeIsOn = LedToTurnOn;
}

#ifdef POT_FADING
int ReadPotValue()
{
	int val = 0;
	for(int i = 0; i < 30; i++)
		val += analogRead(POTENTIOMETER);
	val /= 30;
	val = ((val * 255) / 1024);
	return ((val * 100) / 255);
}
#endif

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
	
#ifndef TEST_STRIPS	
	// Inizializzo la modalità di avvio 
	// default: modalità switch
	TurnPin(STATUS_LED, ON);	
	TurnAnalogPin(NIGHT_LED_STRIP, 100);	
	TurnAnalogPin(DAY_LED_STRIP, 0);	
	SwitchLedStripe.restart();
	WichStripeIsOn = NIGHT_LED_STRIP;
	LedStripeMode = SWITCH_MODE;
	WichMode = DAY_MODE;
	TurnPin(STATUS_LED, OFF);
#else
	BlinkLed(STATUS_LED, 100, 5);
	analogWrite(NIGHT_LED_STRIP, 0);	
	analogWrite(DAY_LED_STRIP, 0);	
#endif	
}


#ifndef TEST_STRIPS


void CheckButton()
{
	if(digitalRead(CHANGE_MODE) == HIGH)
	{
		if(LedStripeMode < MAX_MODES - 1)
			LedStripeMode++;
		else
			LedStripeMode = NIGHT_MODE;
		delay(20);
		BlinkLed(STATUS_LED, 5, 1);
	}
}	
#endif



void loop()
{

#ifndef TEST_STRIPS	
	// Gestione del pulsante che consente il cambio di modalità
	// if(digitalRead(CHANGE_MODE) == HIGH)
	// {
	// 	if(LedStripeMode < MAX_MODES - 1)
	// 		LedStripeMode++;
	// 	else
	// 		LedStripeMode = NIGHT_MODE;
	// 	delay(20);
	// }
	CheckButton();
	// Macchina a stati per la gestione delle varie modalità 
	switch(LedStripeMode)
	{
		case NIGHT_MODE:
			if(WichStripeIsOn != NIGHT_LED_STRIP)
			{
				WichMode = NIGHT_MODE;
				WichStripeIsOn = NIGHT_LED_STRIP;
				BlinkLed(STATUS_LED, 100, 2);
				TurnPin(STATUS_LED, OFF);
			#ifdef POT_FADING
				PotPercent = ReadPotValue();
				TurnAnalogPin(NIGHT_LED_STRIP, PotPercent);
			#else			
				TurnAnalogPin(NIGHT_LED_STRIP, 100);
			#endif
				TurnAnalogPin(DAY_LED_STRIP, 0);
			}
			SwitchLedStripe.restart();
			break;
		case DAY_MODE:
			if(WichStripeIsOn != DAY_LED_STRIP)
			{
				WichMode = DAY_MODE;
				WichStripeIsOn = DAY_LED_STRIP;
				BlinkLed(STATUS_LED, 100, 3);
				TurnPin(STATUS_LED, OFF);				
			#ifdef POT_FADING
				PotPercent = ReadPotValue();
				TurnAnalogPin(DAY_LED_STRIP, PotPercent);
			#else			
				TurnAnalogPin(DAY_LED_STRIP, 100);
			#endif
				TurnAnalogPin(NIGHT_LED_STRIP, 0);
			}	
			SwitchLedStripe.restart();	
			break;
		case SWITCH_MODE:
			if(WichMode != SWITCH_MODE)
			{
				WichMode = SWITCH_MODE;
				BlinkLed(STATUS_LED, 100, 5);
				TurnPin(STATUS_LED, OFF);
			}
			if(SwitchLedStripe.hasPassed(90, true)) // 90s per lo switch
			{
				FadeLedStrips();
			}					
			break;
		default:
			break;
	}
#else
	TestLedStrip();
#endif
}


#ifdef TEST_STRIPS

void CheckButton()
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
		
		//BlinkLed(STATUS_LED, 100, 5);
	}	
}


void TestLedStrip()
{

	// for(int i = 0; i < 256; i++)
	// {
	// 	analogWrite(NIGHT_LED_STRIP, i);
	// 	analogWrite(DAY_LED_STRIP, i);
	// 	delay(100);
	// }

	// analogWrite(NIGHT_LED_STRIP, 0);
	// analogWrite(DAY_LED_STRIP, 0);

	
	switch(LedStripeMode)
	{
		case NIGHT_MODE:
			CheckButton();
			BlinkLed(STATUS_LED, 500, 2);		
			for(int i = 0; i < 101; i++)
			{
				TurnAnalogPin(NIGHT_LED_STRIP, i);
				TurnAnalogPin(DAY_LED_STRIP, i);
				delay(50);
			}		
			break;
		case DAY_MODE:
			CheckButton();
			BlinkLed(STATUS_LED, 500, 4);			
			for(int i = 100; i >= 0; i--)
			{
				TurnAnalogPin(NIGHT_LED_STRIP, i);
				TurnAnalogPin(DAY_LED_STRIP, i);
				delay(50);
			}			
			break;
		case SWITCH_MODE:
			CheckButton();
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