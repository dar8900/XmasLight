/* https://raw.githubusercontent.com/damellis/attiny/ide-1.6.x-boards-manager/package_damellis_attiny_index.json */

#include <stdint.h>
#include <Chrono.h>
#include <EEPROM.h>

#define FW_VERSION	 0.2


#define POT_FADING	

#undef BUTTON_DBG	

#define ON 			  true
#define OFF			  false

#define NIGHT_LED_STRIP   	 0
#define DAY_LED_STRIP   	 1
#define STATUS_LED 	 		 4
#define CHANGE_MODE	 		 3

#define LED_MODE_ADDR		 0

#ifdef POT_FADING
#define POTENTIOMETER		 1
#endif

#define NO_STRIPE_ON  		10

#define SWITCH_STRIP_TIME 	1

#define MIN_BRIGHTNESS	      0
#define MAX_BRIGHTNESS		255

// #define PERCENT(Perc)		(int)((Perc * 255)/MAX_BRIGHTNESS) // Questa è una macro per converire il fading in percentuale



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
uint8_t SwitchTime = 5;//90;
int 	Brightness = MAX_BRIGHTNESS;
int 	OldBrightness = MAX_BRIGHTNESS;


void CheckButton(void);
#ifdef POT_FADING
void ReadPotValue(void);
#endif

void InputCtrl(void);

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
static void TurnAnalogPin(int WichLed, int AnalogBright)
{
	if(AnalogBright >= 0 && AnalogBright <= MAX_BRIGHTNESS)
		analogWrite(WichLed, AnalogBright);
	else
		analogWrite(WichLed, MAX_BRIGHTNESS);
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
	for(int i = Brightness; i >= MIN_BRIGHTNESS; i--)
	{
		TurnAnalogPin(LedToTurnOff, i);
		// CheckButton();
		delay(100);
	}	
	for(int i = MIN_BRIGHTNESS; i <= Brightness; i++)
	{
		TurnAnalogPin(LedToTurnOn, i);
		// CheckButton();
		delay(100);
	}
	TurnPin(STATUS_LED, OFF);
	WichStripeIsOn = LedToTurnOn;
}

#ifdef POT_FADING
void ReadPotValue()
{
	int val = 0;
	int sample = 10;
	// for(int i = 0; i < sample; i++)
	// 	val += analogRead(POTENTIOMETER);
	// val /= sample;
	val = analogRead(POTENTIOMETER);
	val = ((val * 255) / 1023);
	// Brightness = ((val * MAX_BRIGHTNESS) / 255);
	Brightness = val;
	if(Brightness > 255)
		Brightness = 255;
	return;
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
			EEPROM.update(0, LedStripeMode);
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
		EEPROM.update(0, LedStripeMode);
		BlinkLed(STATUS_LED, 5, 1);
		delay(20);
	}
}
#endif

void InputCtrl()
{
	CheckButton();
#ifdef POT_FADING	
	ReadPotValue();
#endif	
}

void EepromInit()
{
	if(EEPROM.read(LED_MODE_ADDR) == 0xFF)
	{
		EEPROM.write(LED_MODE_ADDR, NIGHT_MODE);
		for(int i = 0; i < 3; i ++)
		{
			BlinkLed(STATUS_LED, 10, 3);
			delay(500);
		}
	}
	else
		BlinkLed(STATUS_LED, 1000, 3);

}

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
	EepromInit();
	
	delay(1000);
	Brightness = MIN_BRIGHTNESS;
	BlinkLed(STATUS_LED, 25, 10);
	TurnAnalogPin(NIGHT_LED_STRIP, Brightness);	
	TurnAnalogPin(DAY_LED_STRIP, Brightness);	
	SwitchTimer.restart();
	WichStripeIsOn = NO_STRIPE_ON;
	LedStripeMode = EEPROM.read(0);
	WichMode = MAX_MODES;
	TurnPin(STATUS_LED, OFF);
	Brightness = MAX_BRIGHTNESS;
}



void loop()
{
	InputCtrl();
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
				TurnAnalogPin(NIGHT_LED_STRIP, Brightness);
			#else			
				TurnAnalogPin(NIGHT_LED_STRIP, MAX_BRIGHTNESS);
			#endif
				TurnAnalogPin(DAY_LED_STRIP, 0);
			}
			#ifdef POT_FADING
			if(OldBrightness != Brightness)
			{
				TurnAnalogPin(NIGHT_LED_STRIP, Brightness);
				OldBrightness = Brightness;
			}
			#endif

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
				TurnAnalogPin(DAY_LED_STRIP, Brightness);
			#else			
				TurnAnalogPin(DAY_LED_STRIP, MAX_BRIGHTNESS);
			#endif
				TurnAnalogPin(NIGHT_LED_STRIP, 0);
			}	
			#ifdef POT_FADING
			if(OldBrightness != Brightness)
			{
				TurnAnalogPin(DAY_LED_STRIP, Brightness);
				OldBrightness = Brightness;
			}			
			#endif
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
			#ifdef POT_FADING
			if(OldBrightness != Brightness)
			{
				TurnAnalogPin(DAY_LED_STRIP, Brightness);
				OldBrightness = Brightness;
			}			
			#endif							
			break;
		default:
			break;
	}
}
