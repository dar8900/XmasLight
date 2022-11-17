/* https://raw.githubusercontent.com/damellis/attiny/ide-1.6.x-boards-manager/package_damellis_attiny_index.json */

#include <stdint.h>
#include <Chrono.h>
#include <EEPROM.h>

#define FW_VERSION	 					0.5


#define POT_FADING	

#define ON 			  					true
#define OFF			  					false

#define NIGHT_LED_PIN	   	 			0
#define DAY_LED_PIN		  	 			1
#define STATUS_LED 	 		 			4
#define CHANGE_MODE	 		 			2

#define LED_MODE_ADDR		 			0

#ifdef POT_FADING
#define POTENTIOMETER		 			A3
#endif

#define NO_STRIPE_ON  					10

#define SWITCH_STRIP_TIME 				1

#define MAX_ANALOG_BRIGHTNESS			255
#define MIN_BRIGHTNESS	      			0
#define MAX_BRIGHTNESS					100

#define PERC_TO_BRIGHTNESS(perc)	((perc * MAX_ANALOG_BRIGHTNESS) / 100)

#define FADING_DELAY                    (20 * 1000)
#define CALC_FADING_DELAY(brightness)	(delay(FADING_DELAY / PERC_TO_BRIGHTNESS(brightness)))

#define SWITCH_LED_DELAY				60 // in s

// #define OLD_LED_FADING	

typedef enum
{
	NIGHT_LED = 0,
	DAY_LED,
	NO_LED
}led_type;

// Associo dei numeri alle varie modalità operative
typedef enum
{
	OFF_MODE = 0,
	NIGHT_MODE,
	DAY_MODE,
	SWITCH_MODE,
	MAX_MODES
}led_mode;

Chrono SwitchTimer(Chrono::SECONDS);
led_type WichStripeIsOn;
led_mode LedStripeMode = SWITCH_MODE;
int 	Brightness = MAX_BRIGHTNESS;
int 	OldBrightness = MAX_BRIGHTNESS;


void CheckButton(void);
#ifdef POT_FADING
void ReadPotValue(void);
#endif

void InputCtrl(void);

int WichMode = 0;

/**
 * @brief Funzione per gestire puntualmente il led di stato
 *  
 * @param Status 
 */
static void TurnStatusLed(bool Status)
{
	digitalWrite(STATUS_LED, (uint8_t)Status);
}

/**
 * @brief Funzione per il blinking del led di stato
 * 
 * @param Delay 
 * @param Times 
 */
static void BlinkStatusLed(int Delay, int Times)
{
	bool Toggle = ON;
	for(int i = 0; i < Times * 2; i++)
	{
		TurnStatusLed(Toggle);
		delay(Delay);
		Toggle = !Toggle;
	}	
}


/**
 * @brief Funzione per gestire le uscite "analogiche" PWM
 * 
 * @param WichLed 
 * @param BrightPerc 
 */
static void TurnLedStripe(led_type WichLed, int BrightPerc)
{
	int pin;
	switch (WichLed)
	{
	case NIGHT_LED:
		pin = NIGHT_LED_PIN;
		break;
	case DAY_LED:
		pin = DAY_LED_PIN;
		break;
	default:
		pin = NIGHT_LED_PIN;
		break;
	}
	if(BrightPerc >= MIN_BRIGHTNESS && BrightPerc <= MAX_BRIGHTNESS)
		analogWrite(pin, PERC_TO_BRIGHTNESS(BrightPerc));
	else
		analogWrite(pin, MAX_ANALOG_BRIGHTNESS);
}
 
/**
 * @brief Funzione per la gestione dello switch tra giorno e notte
 * 
 */
static void FadeLedStrips()
{
	led_type LedToTurnOn = NO_LED, LedToTurnOff = NO_LED;
	if(WichStripeIsOn == NIGHT_LED)
	{
		LedToTurnOn = DAY_LED;
		LedToTurnOff = NIGHT_LED;
	}
	else
	{
		LedToTurnOn = NIGHT_LED;
		LedToTurnOff = DAY_LED;
	}
	TurnStatusLed(ON);
	if(Brightness > MIN_BRIGHTNESS)
	{
		// Lo switch avviene in maniera soft ovvero per passsare da una
		// modalita all'altra ci mette FADING_DELAY (25ms * 100)
#ifdef OLD_LED_FADING
		for(int i = Brightness; i >= MIN_BRIGHTNESS; i--)
		{
			TurnLedStripe(LedToTurnOff, i);
			// CheckButton();
			CALC_FADING_DELAY(Brightness);
		}
		for(int i = MIN_BRIGHTNESS; i <= Brightness; i++)
		{
			TurnLedStripe(LedToTurnOn, i);
			// CheckButton();
			CALC_FADING_DELAY(Brightness);
		}
#else
		const uint8_t brightSwitch = 10;
		for(int i = MIN_BRIGHTNESS; i <= Brightness * 2; i++)
		{
			if(i < Brightness - brightSwitch)
			{
				TurnLedStripe(LedToTurnOff, Brightness - i);
			}
			else
			{
				if(i <= Brightness)
				{
					TurnLedStripe(LedToTurnOff, (Brightness - i));
					TurnLedStripe(LedToTurnOn, brightSwitch - (Brightness - i));
				}
				else
				{
					if(brightSwitch + (i - Brightness) <= Brightness)
					{
						TurnLedStripe(LedToTurnOn, brightSwitch + (i - Brightness));
					}
					else
					{
						break;
					}
				}
			}
			CALC_FADING_DELAY(Brightness);
		}
#endif
	}
	else
		delay(1000);
	TurnStatusLed(OFF);
	WichStripeIsOn = LedToTurnOn;
}

void ReadPotValue()
{
#ifdef POT_FADING
	int val = 0;
	const int sample = 10;
	const int maxAnalogRead = 1023;
	for(int i = 0; i < sample; i++)
		val += analogRead(POTENTIOMETER);
	val /= sample;
	if(val >= 0 && val <= maxAnalogRead)
	{
		Brightness = (val * MAX_BRIGHTNESS) / maxAnalogRead;
	}
#else
	Brightness = MAX_BRIGHTNESS;
#endif
}


void CheckButton()
{
	if(digitalRead(CHANGE_MODE) == HIGH)
	{
		switch (LedStripeMode)
		{
		case OFF_MODE:
			LedStripeMode = DAY_MODE;
			break;
		case DAY_MODE:
			LedStripeMode = NIGHT_MODE;
			break;
		case NIGHT_MODE:
			LedStripeMode = SWITCH_MODE;
			break;
		case SWITCH_MODE:
			LedStripeMode = OFF_MODE;
			break;
		default:
			break;
		}
		BlinkStatusLed(5, 1);
		delay(20);
	}
}

/**
 * @brief Funzione per il controllo della pressione del pulsante per il cambio di modalità
 * 			e nel caso di POT_FADING abilitato, controlla la luminosità dei led tramite potenziometro
 * 
 */
void InputCtrl()
{
	CheckButton();	
	ReadPotValue();
}

void setup()
{
	// Inizializzo variabile per il cronometro
	SwitchTimer.restart();
	SwitchTimer.stop();

	// Inizializzo i vari pin che mi serviranno
	pinMode(NIGHT_LED_PIN, OUTPUT);
	pinMode(DAY_LED_PIN, OUTPUT);
	pinMode(STATUS_LED, OUTPUT);
	pinMode(CHANGE_MODE, INPUT);

	delay(2000);

	BlinkStatusLed(500, 5);
	TurnLedStripe(NIGHT_LED, MIN_BRIGHTNESS);	
	TurnLedStripe(DAY_LED, MIN_BRIGHTNESS);	
	SwitchTimer.restart();
	WichStripeIsOn = NIGHT_LED;
	TurnLedStripe(NIGHT_LED, MAX_BRIGHTNESS);
	LedStripeMode = SWITCH_MODE;
	WichMode = MAX_MODES;
	TurnStatusLed(OFF);
	Brightness = MAX_BRIGHTNESS;
}



void loop()
{
	InputCtrl();
	// Macchina a stati per la gestione delle varie modalità 
	switch(LedStripeMode)
	{
		case OFF_MODE:
			if(WichStripeIsOn != NO_LED)
			{
				BlinkStatusLed(500, 2);
				WichMode = OFF_MODE;
				WichStripeIsOn = NO_LED;
				TurnLedStripe(NIGHT_LED, MIN_BRIGHTNESS);
				TurnLedStripe(DAY_LED, MIN_BRIGHTNESS);
			}
			break;
		case DAY_MODE:
			if(WichStripeIsOn != DAY_LED)
			{
				WichMode = DAY_MODE;
				WichStripeIsOn = DAY_LED;
				BlinkStatusLed(100, 5);
				TurnStatusLed(OFF);				
				TurnLedStripe(WichStripeIsOn, Brightness);
				TurnLedStripe(NIGHT_LED, MIN_BRIGHTNESS);
			}		
			SwitchTimer.restart();	
			break;
		case NIGHT_MODE:
			if(WichStripeIsOn != NIGHT_LED)
			{
				WichMode = NIGHT_MODE;
				WichStripeIsOn = NIGHT_LED;
				BlinkStatusLed(100, 3);
				TurnStatusLed(OFF);
				TurnLedStripe(WichStripeIsOn, Brightness);
				TurnLedStripe(DAY_LED, MIN_BRIGHTNESS);
			}
			SwitchTimer.restart();
			break;
		case SWITCH_MODE:
			if(WichMode != SWITCH_MODE)
			{
				WichMode = SWITCH_MODE;
				BlinkStatusLed(80, 10);
				TurnStatusLed(OFF);
			}
			if(SwitchTimer.hasPassed(SWITCH_LED_DELAY)) // 60s per lo switch
			{
				SwitchTimer.stop();
				FadeLedStrips();
				SwitchTimer.restart();
			}				
			break;
		default:
			break;
	}
	if(OldBrightness != Brightness && LedStripeMode != OFF_MODE)
	{
		TurnLedStripe(WichStripeIsOn, Brightness);
		OldBrightness = Brightness;
	}			
}
