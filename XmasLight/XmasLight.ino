/* https://raw.githubusercontent.com/damellis/attiny/ide-1.6.x-boards-manager/package_damellis_attiny_index.json */

#include <stdint.h>
#include <Chrono.h>
#include <EEPROM.h>

#define FW_VERSION	 0.4


#define POT_FADING	

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

#define FADING_DELAY                    (20 * 1000)
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

/**
 * @brief Funzione per il blinking del led di stato
 * 
 * @param WichLed 
 * @param Delay 
 * @param Times 
 */
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

/**
 * @brief Funzione per gestire i singoli pin digitali
 * 
 * @param WichLed 
 * @param Status 
 */
static void TurnPin(int WichLed, bool Status)
{
	if(Status)
		digitalWrite(WichLed, HIGH);
	else
		digitalWrite(WichLed, LOW);
}

/**
 * @brief Funzione per gestire le uscite "analogiche" PWM
 * 
 * @param WichLed 
 * @param AnalogBright 
 */
static void TurnAnalogPin(int WichLed, int AnalogBright)
{
	if(AnalogBright >= MIN_BRIGHTNESS && AnalogBright <= MAX_BRIGHTNESS)
		analogWrite(WichLed, AnalogBright);
	else
		analogWrite(WichLed, MAX_BRIGHTNESS);
}
 
/**
 * @brief Funzione per la gestione dello switch tra giorno e notte
 * 
 */
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
		// modalita all'altra ci mette FADING_DELAY (25ms * 100)
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
	for(int i = 0; i < 21; i++)
	{
		if(val >= (milleIncrement * i) && val < (milleIncrement * (i+1)))
		{
			Brightness = pwmincrement * (i + 1);
			break;
		}
	}
	return;
}
#endif


void CheckButton()
{
	if(digitalRead(CHANGE_MODE) == HIGH)
	{
		if(LedStripeMode < MAX_MODES - 1)
			LedStripeMode++;
		else
			LedStripeMode = OFF_MODE;
		BlinkLed(STATUS_LED, 5, 1);
		delay(20);
	}
}


void InputCtrl()
{
	CheckButton();
#ifdef POT_FADING	
	ReadPotValue();
#endif	
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

	
	delay(1000);
	Brightness = MIN_BRIGHTNESS;
	BlinkLed(STATUS_LED, 25, 10);
	TurnAnalogPin(NIGHT_LED_STRIP, Brightness);	
	TurnAnalogPin(DAY_LED_STRIP, Brightness);	
	SwitchTimer.restart();
	WichStripeIsOn = NIGHT_LED_STRIP;
	LedStripeMode = SWITCH_MODE;
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
