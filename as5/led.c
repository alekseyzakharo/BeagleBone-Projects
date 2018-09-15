//Aleksey Zakharov 301295506
//Barebone Assignment: led.c
//CMPT 433 - Assignment 5

#include "soc_AM335x.h"
#include "beaglebone.h"
#include "gpio_v2.h"
#include "hw_types.h"      // For HWREG(...) macro

#include <math.h>
#include "led.h"

/*****************************************************************************
 **                INTERNAL MACRO DEFINITIONS
 *****************************************************************************/
#define LED_GPIO_BASE           (SOC_GPIO_1_REGS)
#define LED0_PIN (21)
#define LED1_PIN (22)
#define LED2_PIN (23)
#define LED3_PIN (24)

#define LED_MASK ((1<<LED0_PIN) | (1<<LED1_PIN) | (1<<LED2_PIN) | (1<<LED3_PIN))

#define PATTERN_BOUNCE 0
#define PATTERN_BAR 1

static int LED_TIMER_HITS = 0; //0 timer hits
static int SPEED = 0;
static int FLASH_PERIOD = 512; //initilize to 2^(9-0)
static int PATTERN = PATTERN_BOUNCE; //Initilize to bounce
static int LED = 0; //start from LED 0

/*****************************************************************************
 **                INTERNAL FUNCTION PROTOTYPES
 *****************************************************************************/
static void bouncePattern(void);
static void barPattern(void);
static void turnOffAllLED(void);
/*****************************************************************************
 **                INTERNAL FUNCTION DEFINITIONS
 *****************************************************************************/

void LED_init(void)
{
	/* Enabling functional clocks for GPIO1 instance. */
	GPIO1ModuleClkConfig();

	/* Selecting GPIO1[23] pin for use. */
	//GPIO1Pin23PinMuxSetup();

	/* Enabling the GPIO module. */
	GPIOModuleEnable(LED_GPIO_BASE);

	/* Resetting the GPIO module. */
	GPIOModuleReset(LED_GPIO_BASE);

	/* Setting the GPIO pin as an output pin. */
	GPIODirModeSet(LED_GPIO_BASE,
			LED0_PIN,
			GPIO_DIR_OUTPUT);
	GPIODirModeSet(LED_GPIO_BASE,
			LED1_PIN,
			GPIO_DIR_OUTPUT);
	GPIODirModeSet(LED_GPIO_BASE,
			LED2_PIN,
			GPIO_DIR_OUTPUT);
	GPIODirModeSet(LED_GPIO_BASE,
			LED3_PIN,
			GPIO_DIR_OUTPUT);
}

void LED_setFlashPeriod(int speed)
{
	SPEED = speed;
	FLASH_PERIOD = (int)pow(2,(9-speed));
}

void LED_setPattern(char pattern)
{
	if(pattern == 'a')
	{
		PATTERN = PATTERN_BOUNCE;
	}
	else if(pattern == 'b')
	{
		PATTERN = PATTERN_BAR;
	}
	turnOffAllLED();
	LED = 0;
}

void LED_switchPattern(void)
{
	if(PATTERN == PATTERN_BOUNCE)
	{
		PATTERN = PATTERN_BAR;
		ConsoleUtilsPrintf("Joystick: Bar Pattern\n");
	}
	else
	{
		PATTERN = PATTERN_BOUNCE;
		ConsoleUtilsPrintf("Joystick: Bounce Pattern\n");
	}
	turnOffAllLED();
	LED = 0;
}

void LED_lightUp(void)
{
	if(!PATTERN)
	{
		bouncePattern();
	}
	else
	{
		barPattern();
	}
}

static void bouncePattern(void)
{
	//ConsoleUtilsPrintf("im here %d\n",LED);
	if(LED_TIMER_HITS >= FLASH_PERIOD)
	{
		switch(LED)
		{
		case 0:
			HWREG(LED_GPIO_BASE + GPIO_CLEARDATAOUT) = (1 << LED1_PIN);
			HWREG(LED_GPIO_BASE + GPIO_SETDATAOUT) = (1 << LED0_PIN);
			LED++;
			break;
		case 1:
			HWREG(LED_GPIO_BASE + GPIO_CLEARDATAOUT) = (1 << LED0_PIN);
			HWREG(LED_GPIO_BASE + GPIO_SETDATAOUT) = (1 << LED1_PIN);
			LED++;
			break;
		case 2:
			HWREG(LED_GPIO_BASE + GPIO_CLEARDATAOUT) = (1 << LED1_PIN);
			HWREG(LED_GPIO_BASE + GPIO_SETDATAOUT) = (1 << LED2_PIN);
			LED++;
			break;
		case 3:
			HWREG(LED_GPIO_BASE + GPIO_CLEARDATAOUT) = (1 << LED2_PIN);
			HWREG(LED_GPIO_BASE + GPIO_SETDATAOUT) = (1 << LED3_PIN);
			LED++;
			break;
		case 4:
			HWREG(LED_GPIO_BASE + GPIO_CLEARDATAOUT) = (1 << LED3_PIN);
			HWREG(LED_GPIO_BASE + GPIO_SETDATAOUT) = (1 << LED2_PIN);
			LED++;
			break;
		case 5:
			HWREG(LED_GPIO_BASE + GPIO_CLEARDATAOUT) = (1 << LED2_PIN);
			HWREG(LED_GPIO_BASE + GPIO_SETDATAOUT) = (1 << LED1_PIN);
			LED = 0;
			break;
		default:
			break;
		}
		LED_TIMER_HITS = 0;
	}
}

static void barPattern(void)
{
	if(LED_TIMER_HITS >= FLASH_PERIOD)
		{
			switch(LED)
			{
			case 0:
				HWREG(LED_GPIO_BASE + GPIO_SETDATAOUT) = (1 << LED0_PIN);
				LED++;
				break;
			case 1:
				HWREG(LED_GPIO_BASE + GPIO_SETDATAOUT) = (1 << LED1_PIN);
				LED++;
				break;
			case 2:
				HWREG(LED_GPIO_BASE + GPIO_SETDATAOUT) = (1 << LED2_PIN);
				LED++;
				break;
			case 3:
				HWREG(LED_GPIO_BASE + GPIO_SETDATAOUT) = (1 << LED3_PIN);
				LED++;
				break;
			case 4:
				HWREG(LED_GPIO_BASE + GPIO_CLEARDATAOUT) = (1 << LED3_PIN);
				LED++;
				break;
			case 5:
				HWREG(LED_GPIO_BASE + GPIO_CLEARDATAOUT) = (1 << LED2_PIN);
				LED++;
				break;
			case 6:
				HWREG(LED_GPIO_BASE + GPIO_CLEARDATAOUT) = (1 << LED1_PIN);
				LED++;
				break;
			case 7:
				HWREG(LED_GPIO_BASE + GPIO_CLEARDATAOUT) = (1 << LED0_PIN);
				LED = 0;
				break;
			default:
				break;
			}
			LED_TIMER_HITS = 0;
		}
}

void LED_notifyOnTimeIsr(void)
{
	LED_TIMER_HITS++;
}

static void turnOffAllLED(void)
{
	HWREG(LED_GPIO_BASE + GPIO_DATAOUT) &= ~LED_MASK;
}

void LED_incrementSpeed(void)
{
	if(SPEED == 9)
	{
		ConsoleUtilsPrintf("Joystick: speed at max level 9 already\n");
	}
	else
	{
		ConsoleUtilsPrintf("Joystick: speed set to %d\n", (SPEED+1));
		LED_setFlashPeriod(SPEED+1);
	}
}

void LED_decrementSpeed(void)
{
	if(SPEED == 0)
	{
		ConsoleUtilsPrintf("Joystick: speed at min level 0 already\n");
	}
	else
	{
		ConsoleUtilsPrintf("Joystick: speed set to %d\n", (SPEED-1));
		LED_setFlashPeriod(SPEED-1);
	}
}




