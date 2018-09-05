//Baremetal Project: Joystick.c

#include "soc_AM335x.h"
#include "beaglebone.h"
#include "gpio_v2.h"
#include "hw_types.h"      // For HWREG(...) macro
#include "uart_irda_cir.h"
#include "consoleUtils.h"
#include <stdint.h>
#include "hw_cm_per.h"

/*****************************************************************************
**                INTERNAL MACRO DEFINITIONS
*****************************************************************************/
// Boot btn on BBB: SOC_GPIO_2_REGS, pin 8
// Down on Zen cape: SOC_GPIO_1_REGS, pin 14  NOTE: Must change other "2" constants to "1" for correct initialization.
// Left on Zen cape: SOC_GPIO_2_REGS, pin 1
// --> This code uses left on the ZEN:

#define LEFT_GPIO_BASE (SOC_GPIO_2_REGS)
#define DOWN_GPIO_BASE (SOC_GPIO_1_REGS)
#define UP_GPIO_BASE (SOC_GPIO_0_REGS)

#define LEFT_BUTTON_PIN (1)
#define DOWN_BUTTON_PIN (14)
#define UP_BUTTON_PIN (26)

static _Bool leftJoystickStatus = false;
static _Bool upJoystickStatus = false;
static _Bool downJoystickStatus = false;
static _Bool leftJoystickPressed = false;
static _Bool upJoystickPressed  = false;
static _Bool downJoystickPressed  = false;
/*****************************************************************************
**                INTERNAL FUNCTION PROTOTYPES
*****************************************************************************/
static void GPIO2ModuleClkConfig(void);
static void initLeftJoystick();
static void initDownJoystick();

void Joystick_init(void)
{

	GPIO2ModuleClkConfig();

	initLeftJoystick();
	initDownJoystick();

}

static void initLeftJoystick()
{
	GPIOModuleEnable(LEFT_GPIO_BASE);
	GPIOModuleReset(LEFT_GPIO_BASE);
	GPIODirModeSet(LEFT_GPIO_BASE, LEFT_BUTTON_PIN, GPIO_DIR_INPUT);
}

static void initDownJoystick()
{
	//LED already initialized this register, so we dont need to enable it again
	GPIODirModeSet(DOWN_GPIO_BASE, DOWN_BUTTON_PIN, GPIO_DIR_INPUT);
}

static void initUpJoystick()
{
	GPIOModuleEnable(UP_GPIO_BASE);
	GPIOModuleReset(UP_GPIO_BASE);
	GPIODirModeSet(UP_GPIO_BASE, UP_BUTTON_PIN, GPIO_DIR_INPUT);
}

static void GPIO2ModuleClkConfig(void)
{

    /* Writing to MODULEMODE field of CM_PER_GPIO1_CLKCTRL register. */
    HWREG(SOC_CM_PER_REGS + CM_PER_GPIO2_CLKCTRL) |=
          CM_PER_GPIO2_CLKCTRL_MODULEMODE_ENABLE;

    /* Waiting for MODULEMODE field to reflect the written value. */
    while(CM_PER_GPIO2_CLKCTRL_MODULEMODE_ENABLE !=
          (HWREG(SOC_CM_PER_REGS + CM_PER_GPIO2_CLKCTRL) &
           CM_PER_GPIO2_CLKCTRL_MODULEMODE));
    /*
    ** Writing to OPTFCLKEN_GPIO_2_GDBCLK bit in CM_PER_GPIO2_CLKCTRL
    ** register.
    */
    HWREG(SOC_CM_PER_REGS + CM_PER_GPIO2_CLKCTRL) |=
          CM_PER_GPIO2_CLKCTRL_OPTFCLKEN_GPIO_2_GDBCLK;

    /*
    ** Waiting for OPTFCLKEN_GPIO_1_GDBCLK bit to reflect the desired
    ** value.
    */
    while(CM_PER_GPIO2_CLKCTRL_OPTFCLKEN_GPIO_2_GDBCLK !=
          (HWREG(SOC_CM_PER_REGS + CM_PER_GPIO2_CLKCTRL) &
           CM_PER_GPIO2_CLKCTRL_OPTFCLKEN_GPIO_2_GDBCLK));

    /*
    ** Waiting for IDLEST field in CM_PER_GPIO2_CLKCTRL register to attain the
    ** desired value.
    */
    while((CM_PER_GPIO2_CLKCTRL_IDLEST_FUNC <<
           CM_PER_GPIO2_CLKCTRL_IDLEST_SHIFT) !=
           (HWREG(SOC_CM_PER_REGS + CM_PER_GPIO2_CLKCTRL) &
            CM_PER_GPIO2_CLKCTRL_IDLEST));

    /*
    ** Waiting for CLKACTIVITY_GPIO_2_GDBCLK bit in CM_PER_L4LS_CLKSTCTRL
    ** register to attain desired value.
    */
    while(CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_GPIO_2_GDBCLK !=
          (HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) &
           CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_GPIO_2_GDBCLK));
}


void Joystick_notifyOnTimeIsr(void)
{
	_Bool leftPress = GPIOPinRead(LEFT_GPIO_BASE, LEFT_BUTTON_PIN) == 0;
	_Bool downPress = GPIOPinRead(DOWN_GPIO_BASE, DOWN_BUTTON_PIN) == 0;
	_Bool upPress = GPIOPinRead(UP_GPIO_BASE, UP_BUTTON_PIN) == 0;

	if(!leftPress && leftJoystickStatus)
	{
		leftJoystickPressed = true;
		leftJoystickStatus = false;
	}
	else if(leftPress)
	{
		leftJoystickStatus = true;
	}

	if(!downPress && downJoystickStatus)
	{
		downJoystickPressed = true;
		downJoystickStatus = false;
	}
	else if(downPress)
	{
		downJoystickStatus = true;
	}

	if(!upPress && upJoystickStatus)
	{
		upJoystickPressed = true;
		upJoystickStatus = false;
	}
	else if(upPress)
	{
		upJoystickStatus = true;
	}
}

int Joystick_getLeftPressed(void)
{
	if(leftJoystickPressed)
	{
		leftJoystickPressed = false;
		return 1;
	}
	return 0;
}

int Joystick_getUpPressed(void)
{
	if(upJoystickPressed)
	{
		upJoystickPressed = false;
		return 1;
	}
	return 0;
}

int Joystick_getDownPressed(void)
{
	if(downJoystickPressed)
	{
		downJoystickPressed = false;
		return 1;
	}
	return 0;
}


