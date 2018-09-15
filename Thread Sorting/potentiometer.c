// potentiometer.c
// Potentiometer Module:
// Contains potentiometer related functions.

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "display.h"
#include "main.h"
#include "potentiometer.h"
#include "sorter.h"


// Static Functions
static void* startPotentiometerThread();
static int convertVolt2ArraySize(int voltage);
static void displayCharacters(long long number);
static int turnOnA2D(void);
static int readVoltageRaw(void);

#define A2D_VOLTAGE_FILE "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"
#define CAPE_MNGR_FILE "/sys/devices/platform/bone_capemgr/slots"

// Piece-Wise Linear Interpolation Data
#define PW_NUM_POINTS 10
static const int PW_ARR_SIZE[] = {1, 20, 60, 120, 250, 300, 500, 800, 1200, 2100};
static const int PW_VOLT[] = {0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4100};

// Static Variables
static int _voltageRaw;
static int _arraySize;
static _Bool _readValues;
static long long _prevSortedCount, _currSortedCount;

// Thread Variables
static pthread_t _thread;
static pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;

// Initialize potentiometer module and start potentiometer thread.
int Potentio_init()
{
	_voltageRaw = 0;
	_arraySize = 1;
	_readValues = true;
	_prevSortedCount=0;
	_currSortedCount = 0;

	if (turnOnA2D())
	{
		printf("ERROR:\tUnable to Turn On A2D!\n");
		return -1;
	}

	if (pthread_create(&_thread, NULL, startPotentiometerThread(), (void*) NULL))
	{
		printf("Error Creating Potentiometer Thread!\n");
		return -1;
	}

	return 0;
}

// Cleanup potentiometer module.
void Potentio_cleanup()
{
	pthread_mutex_destroy(&_mutex);
}

// Stop potentiometer thread.
void Potenio_stopReading()
{
	printf("Stop Potentiometer Thread!\n");
	_readValues = false;
}


// -- STATIC FUNCTIONS

// Potentiometer thread main function.
static void* startPotentiometerThread()
{
//	printf("Start Potentiometer Thread!\n");
	pthread_detach(pthread_self());

	while (_readValues)
	{
		if (readVoltageRaw())
		{
			printf("Error Reading Voltage (POT _thread)!\n");
			break;
		}

		pthread_mutex_lock(&_mutex);
		_arraySize = convertVolt2ArraySize(_voltageRaw);
		Sorter_setArraySize(_arraySize);
		printf("New Array Size:\t%d\n", _arraySize);
		pthread_mutex_unlock(&_mutex);

		_prevSortedCount = Sorter_getNumberArraysSorted();

		// Sleep For 1 second.
		nanosleep((const struct timespec[]){{1, 0L}}, NULL);

		_currSortedCount = Sorter_getNumberArraysSorted();
		long long sortedPerSecond = _currSortedCount - _prevSortedCount;

		displayCharacters(sortedPerSecond);
	}

	pthread_exit((void *) 0);
}

// Calculate a proper array size from the voltage reading based on the interpolation data.
static int convertVolt2ArraySize(int voltage)
{
	if (voltage < PW_VOLT[0] || voltage > PW_VOLT[PW_NUM_POINTS-1])
	{
		return -1;
	}

	int index1, index2 = 0;
	for (int i = 1; i < PW_NUM_POINTS; i++)
	{
		// Find Interpolation Range
		if (voltage < PW_VOLT[i])
		{
			index1 = i-1;
			index2 = i;
			break;
		}
	}

	float voltMin = (float) PW_VOLT[index1];
	float voltMax = (float) PW_VOLT[index2];
	float arrMin = (float) PW_ARR_SIZE[index1];
	float arrMax = (float) PW_ARR_SIZE[index2];
	float volt = (float) voltage;

	float arrSize = ((volt-voltMin) / (voltMax-voltMin)) * (arrMax-arrMin) + arrMin;
	return (int) arrSize;
}

// Set 14-segment display digits.
static void displayCharacters(long long number)
{
	char charDigits[2];

	if (number > 99)
	{
		number = 99;
	}
	sprintf(charDigits, "%lld", number);
	Display_setDigits(charDigits);
}

// Turn on A2D.
static int turnOnA2D()
{
	int status = writeToFile(CAPE_MNGR_FILE, "BB-ADC");
	// Wait 1 second for the cape to load.
	nanosleep((const struct timespec[]){{1, 0L}}, NULL);
	return status;
}

// Read raw voltage data from the potentiometer.
static int readVoltageRaw()
{
	FILE *voltageFile = fopen(A2D_VOLTAGE_FILE, "r");
	if (voltageFile == NULL)
	{
		printf("ERROR:\tUnable to Open Export File:\n%s\n", A2D_VOLTAGE_FILE);
		return -1;
	}

	pthread_mutex_lock(&_mutex);
	int itemsRead = fscanf(voltageFile, "%d", &_voltageRaw);
	pthread_mutex_unlock(&_mutex);

	if (itemsRead <= 0)
	{
		printf("ERROR:\tReading Data from Export File:\n%s\n", A2D_VOLTAGE_FILE);
		return -1;
	}

	fclose(voltageFile);
	voltageFile = NULL;
	return 0;
}

