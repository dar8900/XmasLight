/* https://raw.githubusercontent.com/damellis/attiny/ide-1.6.x-boards-manager/package_damellis_attiny_index.json */

#include <stdint.h>
#include <Chrono.h>
#include <EEPROM.h>

#define FW_VERSION	 0.3


#define POT_FADING	

#undef BUTTON_DBG	

#define ON 			  true
#define OFF			  false

#define NIGHT_LED_STRIP   	 0
#define DAY_LED_STRIP   	 1
#define STATUS_LED 	 		 4
#define CHANGE_MODE	 		 2

#define LED_MODE_ADDR		 0

#ifdef POT_FADING
#define POTENTIOMETER		 A3
#endif

#define NO_STRIPE_ON  		10

#define SWITCH_STRIP_TIME 	1

#define MIN_BRIGHTNESS	      0
#define MAX_BRIGHTNESS		250

#define FADING_DELAY                    (10 * 1000)
#define CALC_FADING_DELAY(brightness)	(delay(FADING_DELAY / brightness))

// #define PERCENT(Perc)		(int)((Perc * 255)/MAX_BRIGHTNESS) // Questa è una macro per converire il fading in percentuale



// Associo dei numeri alle varie modalità operative
enum
{
	OFF_MODE = 0,
	NIGHT_MODE,
	DAY_MODE,
	SWITCH_MODE,
	MAX_MODES
};

Chrono SwitchTimer(Chrono::SECONDS);
uint8_t WichStripeIsOn;
uint8_t LedStripeMode = SWITCH_MODE;
uint8_t SwitchTime = 60;
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
	if(AnalogBright >= MIN_BRIGHTNESS && AnalogBright <= MAX_BRIGHTNESS)
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
	if(Brightness > MIN_BRIGHTNESS)
	{
		// Lo switch avviene in maniera soft ovvero per passsare da una
		// modalita all'altra ci mette 10s (25ms * 100)
		for(int i = Brightness; i >= MIN_BRIGHTNESS; i--)
		{
			TurnAnalogPin(LedToTurnOff, i);
			// CheckButton();
			CALC_FADING_DELAY(Brightness);
		}	
		for(int i = MIN_BRIGHTNESS; i <= Brightness; i++)
		{
			TurnAnalogPin(LedToTurnOn, i);
			// CheckButton();
			CALC_FADING_DELAY(Brightness);
		}
	}
	else
		delay(1000);
	TurnPin(STATUS_LED, OFF);
	WichStripeIsOn = LedToTurnOn;
}

#ifdef POT_FADING
void ReadPotValue()
{
	int val = 0;
	int sample = 10;
	int milleIncrement = 51, pwmincrement = 12;
	for(int i = 0; i < sample; i++)
		val += analogRead(POTENTIOMETER);
	val /= sample;
	// if(val < 512)
	// 	Brightness = 0;
	// else if(val < 900)
	// 	Brightness = 127;
	// else
	// 	Brightness = MAX_BRIGHTNESS;
	for(int i = 0; i < 21; i++)
	{
		if(val >= (milleIncrement * i) && val < (milleIncrement * (i+1)))
		{
			Brightness = pwmincrement * (i + 1);
			break;
		}
	}
	// if(val >= 0 && val < milleIncrement)
	// 	Brightness = pwmincrement;
	// else if(val >= milleIncrement && val < milleIncrement * 2)
	// 	Brightness = 0;
	// else if(val >= milleIncrement * 2 && val < milleIncrement * 3)
	// 	Brightness = pwmincrement * 2;
	// else if(val >= milleIncrement * 3 && val < milleIncrement * 4)
	// 	Brightness = pwmincrement * 4;
	// else if(val >= milleIncrement * 4 && val < milleIncrement * 5)
	// 	Brightness = pwmincrement * 5;
	// else if(val >= milleIncrement * 5 && val < milleIncrement * 6)
	// 	Brightness = pwmincrement * 6;
	// else if(val >= milleIncrement * 6 && val < milleIncrement * 7)
	// 	Brightness = pwmincrement * 7;
	// else if(val >= milleIncrement * 7 && val < milleIncrement * 8)
	// 	Brightness = pwmincrement * 8;
	// else if(val >= milleIncrement * 8 && val < milleIncrement * 9)
	// 	Brightness = pwmincrement * 9;
	// else if(val >= milleIncrement * 9 && val < milleIncrement * 10)
	// 	Brightness = pwmincrement * 10;
	// else if(val >= milleIncrement * 10 && val < milleIncrement * 11)
	// 	Brightness = pwmincrement * 11;
	// else if(val >= milleIncrement * 11 && val < milleIncrement * 12)
	// 	Brightness = pwmincrement * 12;
	// else if(val >= milleIncrement * 12 && val < milleIncrement * 13)
	// 	Brightness = pwmincrement * 13;	
	// else if(val >= milleIncrement * 13 && val < milleIncrement * 14)
	// 	Brightness = pwmincrement * 14;
	// else if(val >= milleIncrement * 14 && val < milleIncrement * 15)
	// 	Brightness = pwmincrement * 15;
	// else if(val >= milleIncrement * 15 && val < milleIncrement * 16)
	// 	Brightness = pwmincrement * 16;
	// else if(val >= milleIncrement * 16 && val < milleIncrement * 17)
	// 	Brightness = pwmincrement * 17;
	// else if(val >= milleIncrement * 17 && val < milleIncrement * 18)
	// 	Brightness = pwmincrement * 18;
	// else if(val >= milleIncrement * 18 && val < milleIncrement * 19)
	// 	Brightness = pwmincrement * 19;
	// else if(val >= milleIncrement * 19 && val < milleIncrement * 20)
	// 	Brightness = pwmincrement * 21;
	// else if(val >= 850 && val < 1023)
	// 	Brightness = pwmincrement * 21;	
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
			EEPROM.update(LED_MODE_ADDR, LedStripeMode);
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
			LedStripeMode = OFF_MODE;
		EEPROM.update(LED_MODE_ADDR, LedStripeMode);
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
	WichStripeIsOn = NIGHT_LED_STRIP;
	LedStripeMode = EEPROM.read(LED_MODE_ADDR);
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
		case OFF_MODE:
			if(WichStripeIsOn != NO_STRIPE_ON)
			{
				BlinkLed(STATUS_LED, 500, 2);
				WichMode = OFF_MODE;
				WichStripeIsOn = NO_STRIPE_ON;
				TurnAnalogPin(NIGHT_LED_STRIP, MIN_BRIGHTNESS);
				TurnAnalogPin(DAY_LED_STRIP, MIN_BRIGHTNESS);
			}
			break;
		case NIGHT_MODE:
			if(WichStripeIsOn != NIGHT_LED_STRIP)
			{
				WichMode = NIGHT_MODE;
				WichStripeIsOn = NIGHT_LED_STRIP;
				BlinkLed(STATUS_LED, 100, 3);
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
				BlinkLed(STATUS_LED, 100, 5);
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
				BlinkLed(STATUS_LED, 80, 10);
				TurnPin(STATUS_LED, OFF);
			}
			if(SwitchTimer.hasPassed(SwitchTime)) // 60s per lo switch
			{
				SwitchTimer.stop();
				FadeLedStrips();
				SwitchTimer.restart();
			}	
			#ifdef POT_FADING
			if(OldBrightness != Brightness)
			{
				TurnAnalogPin(WichStripeIsOn, Brightness);
				OldBrightness = Brightness;
			}			
			#endif							
			break;
		default:
			break;
	}
}
