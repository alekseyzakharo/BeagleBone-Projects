// main.c

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "display.h"
#include "main.h"
#include "network.h"
#include "potentiometer.h"
#include "sorter.h"


// Static Variables
static pthread_mutex_t _mutexMain = PTHREAD_MUTEX_INITIALIZER;


// Main Function
int main(int argc, char** argv)
{
	srand(time(NULL));

	// Initialize Modules
	Display_init();
	Sorter_init();
	Network_init();
	Potentio_init();

	// Wait Here
	lockMutex();

	// Clean Up Modules
	Display_cleanup();
	Sorter_cleanup();
	Potentio_cleanup();

	// Destroy Main Mutex
	pthread_mutex_destroy(&_mutexMain);

	// Sleep For 1 second.
	nanosleep((const struct timespec[]){{1, 0L}}, NULL);
	return 0;
}

// Get random integer in a range [min, max].
int getRandomInt(int min, int max)
{
	return rand() % (max - min + 1) + min;
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

// Lock Main Mutex
void lockMutex(void)
{
	pthread_mutex_lock(&_mutexMain);
}

// Unlock Main Mutex
void unlockMutex(void)
{
	pthread_mutex_unlock(&_mutexMain);
}

