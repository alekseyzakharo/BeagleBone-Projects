//Aleksey Zakharov 301295506
//Barebone Assignment: main.c
//CMPT 433 - Assignment 5

#include "consoleUtils.h"
#include <stdint.h>
#include <stdbool.h>

// My hardware abstraction modules
#include "serial.h"
#include "timer.h"
#include "wdtimer.h"

// My application's modules
#include "resetsource.h"
#include "led.h"
#include "joystick.h"

static void printCommands();
static void doBackgroundSerialWork(void);
static checkJoystick(void);

static _Bool STOP;

//SERIAL PORT HANDLING
static volatile uint8_t s_rxByte = 0;
static void serialRxIsrCallback(uint8_t rxByte) {
	s_rxByte = rxByte;
}

static void doBackgroundSerialWork(void)
{
	if (s_rxByte != 0) {
		if(s_rxByte == '?')
		{
			printCommands();
		}
		else if(s_rxByte >= '0' && s_rxByte <= '9')
		{
			int num = s_rxByte - '0';
			ConsoleUtilsPrintf("\nSetting LED speed to %d\n",num);
			LED_setFlashPeriod(num);
		}
		else if(s_rxByte == 'a')
		{
			ConsoleUtilsPrintf("\nChanging to bounce mode.\n");
			LED_setPattern('a');
		}
		else if(s_rxByte == 'b')
		{
			ConsoleUtilsPrintf("\nChanging to bar mode.\n");
			LED_setPattern('b');
		}
		else if(s_rxByte == 'x')
		{
			ConsoleUtilsPrintf("\nNo longer hitting the watchdog.\n");
			STOP = true;
		}
		else
		{
			ConsoleUtilsPrintf("\nInvalid command.\n");
		}

		s_rxByte = 0;
	}
}

static checkJoystick(void)
{
	if(Joystick_getLeftPressed())
	{
		LED_switchPattern();
	}
	if(Joystick_getDownPressed())
	{
		LED_decrementSpeed();
	}
	if(Joystick_getUpPressed())
	{
		LED_incrementSpeed();
	}
}

/******************************************************************************
 **              MAIN
 ******************************************************************************/
int main(void)
{
	// Initialization
	Serial_init(serialRxIsrCallback);
	Timer_init();
	Watchdog_init();
	LED_init();
	Joystick_init();

	// Setup callbacks from hardware abstraction modules to application:
	Serial_setRxIsrCallback(serialRxIsrCallback);
	Timer_setTimerIsrCallbackOne(LED_notifyOnTimeIsr);
	Timer_setTimerIsrCallbackTwo(Joystick_notifyOnTimeIsr);

	STOP = false;

	//Welcome Message
	ConsoleUtilsPrintf("\nLightBouncer:\n"
					"\tby Aleksey Zakharov\n---------------------------\n");

	//print reset register
	Resetsource_PrintAndResetRegister();
	//REMOVE THIS AFTER
	printCommands();


	// Main loop:
	while(1) {
		// Handle background processing
		doBackgroundSerialWork();
		LED_lightUp();
		checkJoystick();
		// Timer ISR signals intermittent background activity.
		if(Timer_isIsrFlagSet() && !STOP) {
			Timer_clearIsrFlag();
			Watchdog_hit();
		}
	}
}

static void printCommands()
{
	ConsoleUtilsPrintf("\nCommands:\n"
			" ?   : Display this help message.\n"
			" 0-9 : Set speed 0 (slow) to 9 (fast)\n"
			" a   : Select pattern A (bounce).\n"
			" b   : Select pattern B (bar).\n"
			" x   : Stop hitting the watchdog.\n"
			" BTN : Push-button to toggle mode.\n");
}


