//Baremetal Project: resetsource.c

#include <stdint.h>
#include "hw_types.h"
#include "resetsource.h"

#define RESET_REGISTER 0x44E00F00
#define RESET_REGISTER_OFFSET 0x8

#define ICEPICK_RESET_BITSHIFT 9
#define EXTERNAL_WARM_RESET_BITSHIFT 5
#define WATCHDOG_RESET_BITSHIFT 4
#define GLOBAL_WARM_RESET_BITSHIFT 1
#define GLOBAL_COLD_RESET_BITSHIFT 0

#define RESET_REGISTER_RESET_MASK ((1<<ICEPICK_RESET_BITSHIFT)|(1<<EXTERNAL_WARM_RESET_BITSHIFT)|(1<<WATCHDOG_RESET_BITSHIFT)|(1<<GLOBAL_WARM_RESET_BITSHIFT)|(1<<GLOBAL_COLD_RESET_BITSHIFT))

void Resetsource_PrintAndResetRegister(void)
{
	uint32_t reg = HWREG(RESET_REGISTER + RESET_REGISTER_OFFSET);

	if((reg & (1<< ICEPICK_RESET_BITSHIFT))>>ICEPICK_RESET_BITSHIFT)
	{
		ConsoleUtilsPrintf("Reset source (0x200) = IcePick Reset, \n");
	}
	if((reg & (1<< EXTERNAL_WARM_RESET_BITSHIFT))>>EXTERNAL_WARM_RESET_BITSHIFT)
	{
		ConsoleUtilsPrintf("Reset source (0x20) = External Warm Reset, \n");
	}
	if((reg & (1<< WATCHDOG_RESET_BITSHIFT))>>WATCHDOG_RESET_BITSHIFT)
	{
		ConsoleUtilsPrintf("Reset source (0x10) = Watchdog Reset, \n");
	}
	if((reg & (1<< GLOBAL_WARM_RESET_BITSHIFT))>>GLOBAL_WARM_RESET_BITSHIFT)
	{
		ConsoleUtilsPrintf("Reset source (0x2) = Global Warm Reset, \n");
	}
	if((reg & (1<< GLOBAL_COLD_RESET_BITSHIFT))>>GLOBAL_COLD_RESET_BITSHIFT)
	{
		ConsoleUtilsPrintf("Reset source (0x1) = Global Cold Reset, \n");
	}

	//clear all reset values
	HWREG(RESET_REGISTER + RESET_REGISTER_OFFSET) = RESET_REGISTER_RESET_MASK;
}
