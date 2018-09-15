// accelerometer_joystick.c
// Accelerometer and Joystick Module:
// Contains accelerometer and joystick related functions.

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

#include "accelerometer_joystick.h"
#include "main.h"
#include "command_interface.h"

//Accelerometer
#define I2C_DEVICE_ADDRESS 0x1C
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2C_CTRL_REG_1 0x2A
#define I2C_CTRL_REG_2 0x2B
#define REGISTER_BYTES 7

//Joystick
typedef enum direction {None, Up, Down, Left, Right, Center} direction;
#define	LEFT "65"
#define RIGHT "47"
#define UP "26"
#define DOWN "46"
#define CENTER "27"

// Static Functions
static void *displayDigitsThread();
static void turnOnI2C1Bus();
static int initI2cBus(char* bus, int address);
static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value);
static char* readI2cReg(int i2cFileDesc, unsigned char regAddr);

static void exportJoystickGpio();
static int readGPIOStatus(char *gpio);
static int readFileStatus(char *fileName);
static enum direction joystickDirection(void);
static float hexToFloat(char c1, char c2);


static long sleepLengthNano;
static pthread_t accelerometerJoystickThreadId;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int _STOP;

static enum direction dir;


// Initialize display module and start display thread.
int Accelerometer_Joystick_init()
{
	sleepLengthNano = 500000000;
	_STOP = 0;
	dir = None;
	turnOnI2C1Bus();
	exportJoystickGpio();

	if(pthread_create(&accelerometerJoystickThreadId, NULL, displayDigitsThread, (void*) NULL))
	{
		printf("Error Creating Display Thread\n");
		return -1;
	}
	return 0;
}

// Cleanup display module.
void Accelerometer_Joystick_cleanup()
{
	printf("Stop Display Thread!\n");

	_STOP = 1;

	pthread_join(accelerometerJoystickThreadId, NULL);
	pthread_mutex_destroy(&mutex);
}

// -- STATIC FUNCTIONS

// Display thread main function.
static void *displayDigitsThread()
{
//	printf("Start Display Thread!\n");
	pthread_detach(pthread_self());


	int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);

	//set CTRL_REG1 = ACTIVE && CTRL_REG2 = ACTIVE
	writeI2cReg(i2cFileDesc, I2C_CTRL_REG_1, 0x01);
	writeI2cReg(i2cFileDesc, I2C_CTRL_REG_2, 0x01);

	float x, y ,z;
	while(!_STOP)
	{
		char* values = readI2cReg(i2cFileDesc, 0x00); //read registers starting from 0x00

		dir = joystickDirection();
		if(dir != None)
		{
			switch(dir)
			{
				case Left:
					Interface_decreaseTempo();
					printf("Tempo Decreased to: %d\n", Interface_getTempo());
					break;
				case Right:
					printf("Tempo Increased to: %d\n", Interface_getTempo());
					Interface_increaseTempo();
					break;
				case Up:
					Interface_increaseVolume();
					printf("Volume Increased to: %d\n", Interface_getVolume());
					break;
				case Down:
					Interface_decreaseVolume();
					printf("Volume Decreased to: %d\n", Interface_getVolume());
					break;
				case Center:
					printf("Beat changed\n");
					Interface_nextBeat();
					break;
				default:
					break;
			}
		}
		//clean this up
		x = hexToFloat(values[1],values[2]);
		if(x > 2000 || x < -2000)
		{
			Interface_snare();
			printf("X : Left/Right was triggered \n");
		}
		y = hexToFloat(values[3],values[4]);
		if(y > 2000 || y < -2000)
		{
			Interface_bass();
			printf("Y : Away/Toward was triggered \n");
		}
		z = hexToFloat(values[5],values[6]);
		if(z > 2000 || z < -2000)
		{
			Interface_hihat();
			printf("Z : Up/Down was triggered \n");
		}

		free(values);
		nanosleep((const struct timespec[]){{0, sleepLengthNano}}, NULL);
	}
	close(i2cFileDesc);

	return NULL;
}


// Turn on I2C bus.
static void turnOnI2C1Bus()
{
	writeToFile("/sys/devices/platform/bone_capemgr/slots", "BB-I2C1");
}

// turn on joystick functionality
static void exportJoystickGpio()
{
	writeToFile("/sys/class/gpio/export", LEFT);
	writeToFile("/sys/class/gpio/export", RIGHT);
	writeToFile("/sys/class/gpio/export", UP);
	writeToFile("/sys/class/gpio/export", DOWN);
	writeToFile("/sys/class/gpio/export", CENTER);
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

static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value)
{
	unsigned char buff[2];
	buff[0] = regAddr;
	buff[1] = value;
	int res = write(i2cFileDesc, buff, 2);
	if (res != 2) {
		perror("I2C: Unable to write i2c register.");
		exit(1);
	}
}

static char* readI2cReg(int i2cFileDesc, unsigned char regAddr)
{
	// To read a register, must first write the address
	int res = write(i2cFileDesc, &regAddr, sizeof(regAddr));
	if (res != sizeof(regAddr)) {
		perror("I2C: Unable to write to i2c register.");
		exit(1);
	}
	// Now read the value and return it
	char *value = malloc(sizeof(char)*REGISTER_BYTES);
	char str[REGISTER_BYTES];
	res = read(i2cFileDesc, &str, sizeof(char)*REGISTER_BYTES);

	if (res != sizeof(str)) {
		perror("I2C: Unable to read from i2c register");
		exit(1);
	}
	for(int i =1;i<REGISTER_BYTES;i++)
	{
		value[i] = str[i];
	}
	return value;
}

static int readGPIOStatus(char *gpio) {
  char* value = malloc(strlen("/sys/class/gpio/")+strlen(gpio)+strlen("/value")+1);
  strcpy(value, "/sys/class/gpio/");
  strcat(value, gpio);
  strcat(value, "/value\0");

  int stat = readFileStatus(value);
  free(value);

  return stat;
}

//read gpio file from location, file has to either contain a 0 or 1 as its value
static int readFileStatus(char *fileName) {
  FILE *file = fopen(fileName, "r");
  if (file == NULL) {
  printf("ERROR: Unable to open file (%s) for read\n", fileName);
    exit(-1);
  }
  // Read string (line)
  const int max_length = 1024;
  char buff[max_length];
  fgets(buff, max_length, file);
  // Close
  fclose(file);
  if(*buff == '0')
    return 0;
  return 1;
}

static enum direction joystickDirection(void) {
  if(!readGPIOStatus("gpio65")) {
    return Left;
  }
  if(!readGPIOStatus("gpio47")) {
    return Right;
  }
  if(!readGPIOStatus("gpio26")) {
    return Up;
  }
  if(!readGPIOStatus("gpio46")) {
    return Down;
  }
  if(!readGPIOStatus("gpio27")) {
      return Center;
    }
  return None;
}

//12 bit hex to float
static float hexToFloat(char c1, char c2)
{
	if(((int)c1 & 0x80) >> 7 )
	{
		return (float)(-1.0) * (float)(((((int)c1) << 4) | ((int)c2 >> 4) ) ^0xFFF);
	}
	return (float)(((int)c1 << 4) | ((int)c2 >> 4));
}




