/* https://raw.githubusercontent.com/damellis/attiny/ide-1.6.x-boards-manager/package_damellis_attiny_index.json */

#include <stdint.h>
#include <Chrono.h>

#define FW_VERSION	 0.1


#undef POT_FADING	

#define BUTTON_DBG	

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



// Associo dei numeri alle varie modalità operative
enum
{
	NIGHT_MODE = 0,
	DAY_MODE,
	SWITCH_MODE,
	MAX_MODES
};

Chrono SwitchTimer(Chrono::SECONDS);
uint8_t WichStripeIsOn;
uint8_t LedStripeMode = SWITCH_MODE;
uint8_t SwitchTime = 90;

#ifdef POT_FADING
int PotPercent = 0;
#endif

void CheckButton();
int WichMode = 0;

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
	TurnPin(STATUS_LED, ON);
	// Lo switch avviene in maniera soft ovvero per passsare da una
	// modalita all'altra ci mette 10s (25ms * 100)
	for(int i = 100; i >= 0; i--)
	{
		TurnAnalogPin(LedToTurnOff, i);
		CheckButton();
		delay(100);
	}	
	for(int i = 0; i < 101; i++)
	{
		TurnAnalogPin(LedToTurnOn, i);
		CheckButton();
		delay(100);
	}
	TurnPin(STATUS_LED, OFF);
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


#ifdef BUTTON_DBG
void CheckButton()
{
	int Cnt = 0;
	if(digitalRead(CHANGE_MODE) == HIGH)
	{
		delay(100);
		// Per debug, se viene tenuto premuto 5s si cambia il tempo
		// di switch da 90s a 5s e viceversa
		while(digitalRead(CHANGE_MODE) == HIGH) 
		{
			if(Cnt == 250)
			{
				if(SwitchTime != 5)
					SwitchTime = 5;
				else
					SwitchTime = 90;
				break;
			}
			Cnt++;
			delay(20);
		}
		if(Cnt < 50)
		{
			if(LedStripeMode < MAX_MODES - 1)
				LedStripeMode++;
			else
				LedStripeMode = NIGHT_MODE;
			BlinkLed(STATUS_LED, 5, 1);
		}
		else
		{
			BlinkLed(STATUS_LED, 5, 20);
		}
	}
}	
#else
void CheckButton()
{
	if(digitalRead(CHANGE_MODE) == HIGH)
	{
		if(LedStripeMode < MAX_MODES - 1)
			LedStripeMode++;
		else
			LedStripeMode = NIGHT_MODE;
		BlinkLed(STATUS_LED, 5, 1);
		delay(20);
	}
}
#endif


void setup()
{
	// Inizializzo variabile per il cronometro
	SwitchTimer.restart();
	SwitchTimer.stop();

	// Inizializzo i vari pin che mi serviranno
	pinMode(NIGHT_LED_STRIP, OUTPUT);
	pinMode(DAY_LED_STRIP, OUTPUT);
	pinMode(STATUS_LED, OUTPUT);
	pinMode(CHANGE_MODE, INPUT);
	
	// Inizializzo la modalità di avvio 
	// default: modalità switch
	delay(1000);
	BlinkLed(STATUS_LED, 1000, 1);
	BlinkLed(STATUS_LED, 25, 10);
	TurnAnalogPin(NIGHT_LED_STRIP, 0);	
	TurnAnalogPin(DAY_LED_STRIP, 0);	
	SwitchTimer.restart();
	WichStripeIsOn = NO_STRIPE_ON;
	LedStripeMode = NIGHT_MODE;
	WichMode = SWITCH_MODE;
	TurnPin(STATUS_LED, OFF);

}



void loop()
{
	CheckButton();
	// Macchina a stati per la gestione delle varie modalità 
	switch(LedStripeMode)
	{
		case NIGHT_MODE:
			if(WichStripeIsOn != NIGHT_LED_STRIP)
			{
				WichMode = NIGHT_MODE;
				WichStripeIsOn = NIGHT_LED_STRIP;
				BlinkLed(STATUS_LED, 100, 1);
				TurnPin(STATUS_LED, OFF);
			#ifdef POT_FADING
				PotPercent = ReadPotValue();
				TurnAnalogPin(NIGHT_LED_STRIP, PotPercent);
			#else			
				TurnAnalogPin(NIGHT_LED_STRIP, 100);
			#endif
				TurnAnalogPin(DAY_LED_STRIP, 0);
			}
			SwitchTimer.restart();
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
			SwitchTimer.restart();	
			break;
		case SWITCH_MODE:
			if(WichMode != SWITCH_MODE)
			{
				WichMode = SWITCH_MODE;
				BlinkLed(STATUS_LED, 100, 5);
				TurnPin(STATUS_LED, OFF);
			}
			if(SwitchTimer.hasPassed(SwitchTime)) // 90s per lo switch
			{
				SwitchTimer.stop();
				FadeLedStrips();
				SwitchTimer.restart();
			}					
			break;
		default:
			break;
	}
}
