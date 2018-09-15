// CMPT 433 - Assignment 3
// main.c
// Main Module:
// Contains utility functions used by other modules.

#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "accelerometer_joystick.h"
#include "audioMixer_template.h"
#include "drumbeat.h"
#include "network.h"

// Main Function
int main(int argc, char** argv)
{
	// Initialize Modules
	AudioMixer_init();
	Network_init();
	Accelerometer_Joystick_init();
	Drumbeat_init();

	while(1)
	{

	}

	//functions that will never get called
	AudioMixer_cleanup();
	Network_stop();
	Accelerometer_Joystick_cleanup();
	Drumbeat_cleanup();

	return 0;
}

// Write a specified value to a file with a specified file path.
int writeToFile(char* path, char* value)
{
	FILE *pfile = fopen(path, "w");
	if (pfile == NULL)
	{
		printf("ERROR:\tUnable to Open Export File:\n%s\n", path);
		return -1;
	}

	int numChars = fprintf(pfile, "%s", value);
	fflush(pfile);
	if (numChars <= 0)
	{
		printf("ERROR:\tWriting Data to Export File:\n%s\n", path);
		return -1;
	}

	fclose(pfile);
	return 0;
}


