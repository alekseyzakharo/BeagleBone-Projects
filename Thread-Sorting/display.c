// display.c
// Display Module:
// Contains 14-segment display related functions.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <pthread.h>
#include <time.h>

#include "display.h"
#include "main.h"


#define REG_DIRA 0x00
#define REG_DIRB 0x01
#define REG_OUTA 0x14
#define REG_OUTB 0x15
#define I2C_DEVICE_ADDRESS 0x20

#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"

// Static Functions
static void *displayDigitsThread();
static void turnOnGPIO();
static void turnOnI2C1Bus();
static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value);
static int initI2cBus(char* bus, int address);
static int getNum(char n);
static int* getDigits();

// Static Variables
static int hex14[] = {0xa3, 0x80, 0x31, 0xb0, 0x90, 0xb0, 0xb1, 0x04, 0xb1, 0x90};
static int hex15[] = {0x96, 0x12, 0x0e, 0x0e, 0x8a, 0x8c, 0x8c, 0x14, 0x8e, 0x8e};
static int num1_14, num1_15, num2_14, num2_15;
static long sleepLengthNano;
static pthread_t _thread;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int _STOP;


// Initialize display module and start display thread.
int Display_init()
{
	num1_14 = 0xa3;
	num1_15 = 0x96;
	num2_14 = 0xa3;
	num2_15 = 0x96;
	sleepLengthNano = 5000000;
	_STOP = 0;

	turnOnGPIO();
	turnOnI2C1Bus();

	if(pthread_create(&_thread, NULL, displayDigitsThread, (void*) NULL))
	{
		printf("Error Creating Display Thread\n");
		return -1;
	}
	return 0;
}

// Stop display thread.
void Display_stop()
{
	printf("Stop Display Thread!\n");
	_STOP = 1;
}

// Cleanup display module.
void Display_cleanup()
{
	pthread_mutex_destroy(&mutex);
}

// Set the 14-segment display digits to a specified value.
void Display_setDigits(char* nums)
{

	pthread_mutex_lock(&mutex);

	int num1 = getNum(nums[0]);
	int num2 = getNum(nums[1]);
	if(num2 == -1)
	{
		num2 = num1;
		num1 = 0;
	}

	num1_14 = hex14[num1];
	num1_15 = hex15[num1];
	num2_14 = hex14[num2];
	num2_15 = hex15[num2];

	pthread_mutex_unlock(&mutex);
}

// -- STATIC FUNCTIONS

// Display thread main function.
static void *displayDigitsThread()
{
//	printf("Start Display Thread!\n");
	pthread_detach(pthread_self());

	int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
	writeI2cReg(i2cFileDesc, REG_DIRA, 0x00);
	writeI2cReg(i2cFileDesc, REG_DIRB, 0x00);
	int iterator;
	while(!_STOP)
	{
		int *nums = getDigits();
		for(iterator = 0;iterator < 50;iterator++)
		{
			writeToFile("/sys/class/gpio/gpio61/value", "1");
			writeI2cReg(i2cFileDesc, REG_OUTA, nums[0]);
			writeI2cReg(i2cFileDesc, REG_OUTB, nums[1]);
			nanosleep((const struct timespec[]){{0, sleepLengthNano}}, NULL);
			writeToFile("/sys/class/gpio/gpio61/value", "0");
			writeToFile("/sys/class/gpio/gpio44/value", "1");
			writeI2cReg(i2cFileDesc, REG_OUTA, nums[2]);
			writeI2cReg(i2cFileDesc, REG_OUTB, nums[3]);
			nanosleep((const struct timespec[]){{0, sleepLengthNano}}, NULL);
			writeToFile("/sys/class/gpio/gpio44/value", "0");

		}
		free(nums);
		nums = NULL;
	}
	close(i2cFileDesc);

	pthread_exit((void *) 0);
}

// Turn on GPIO pins.
static void turnOnGPIO()
{
	writeToFile("/sys/class/gpio/export", "61");
	writeToFile("/sys/class/gpio/export", "44");
	writeToFile("/sys/class/gpio/gpio61/direction", "out");
	writeToFile("/sys/class/gpio/gpio44/direction", "out");
	writeToFile("/sys/class/gpio/gpio61/value", "1");
	writeToFile("/sys/class/gpio/gpio44/value", "1");
}

// Turn on I2C bus.
static void turnOnI2C1Bus()
{
	writeToFile("/sys/devices/platform/bone_capemgr/slots", "BB-I2C1");
}

// Write to I2C registers.
static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value)
{
	unsigned char buff[2];
	buff[0] = regAddr;
	buff[1] = value;
	int res = write(i2cFileDesc, buff, 2);
	if (res != 2) {
		perror("I2C: Unable to write I2C register.\n");
		exit(1);
	}
}

// Initialize I2C bus.
static int initI2cBus(char* bus, int address)
{
	int i2cFileDesc = open(bus, O_RDWR);
	if (i2cFileDesc < 0)
	{
		printf("I2C: Unable to open bus for read/write (%s)\n", bus);
		perror("Error Is:");
		exit(1);
	}
	int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
	if (result < 0)
	{
		perror("I2C: Unable to set I2C device to slave address.\n");
		exit(1);
	}
	return i2cFileDesc;
}

// Get an integer representation of a character.
static int getNum(char n)
{
	if (n > '9' || n < '0')
		return -1;
	return n - '0';
}

// Get currently displayed display digit characters.
static int* getDigits()
{
	pthread_mutex_lock(&mutex);
	int *nums =  malloc(sizeof(int) * 4);
	nums[0] = num1_14;
	nums[1] = num1_15;
	nums[2] = num2_14;
	nums[3] = num2_15;
	pthread_mutex_unlock(&mutex);
	return nums;
}

