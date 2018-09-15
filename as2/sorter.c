// CMPT 433 - Assignment 2
// sorter.c
// Sorter Module:
// Contains sorter related functions.

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "main.h"
#include "sorter.h"

// Static Functions
static void* startSorterThread();
static void initArray();
static void freeArray();
static void permuteArray();
static void sortArray();
static void swap(int* a, int* b);

// Starting Array Size
static const int START_ARR_SIZE = 1000;

// Static Variables
static int _currentSize;
static int _nextSize;
static long long _sortedCount;
static int* _arr;
static _Bool _sort;

// Thread Variables
static pthread_t _thread;
static pthread_mutex_t _mutexArray = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t _mutexCount = PTHREAD_MUTEX_INITIALIZER;

// Initialize sorter module and start sorter thread.
int Sorter_init()
{
	_currentSize = START_ARR_SIZE;
	_nextSize = START_ARR_SIZE;
	_sortedCount = 0;
	_sort = true;

	if (pthread_create(&_thread, NULL, startSorterThread, (void*) NULL))
	{
		printf("Error Creating Sorter Thread!\n");
		return -1;
	}

	return 0;
}

// Cleanup sorter module.
void Sorter_cleanup()
{
	// Free Most Recent Array
	freeArray();

	pthread_mutex_destroy(&_mutexArray);
	pthread_mutex_destroy(&_mutexCount);
}

// Stop sorter thread.
void Sorter_stopSorting()
{
	printf("Stop Sorter Thread!\n");
	_sort = false;
}

// Set the size of the next array to sort.
void Sorter_setArraySize(int newSize)
{
	_nextSize = newSize;
}

// Get the size of the array currently being sorted.
int Sorter_getArrayLength()
{
	return _currentSize;
}

// Get a copy of the currently sorted array.
int* Sorter_getArrayData(int *length)
{
	pthread_mutex_lock(&_mutexArray);

	*length = _currentSize;
	int* arrayCopy = (int *) malloc(_currentSize * sizeof(int));

	for (int i = 0; i < _currentSize; i++)
	{
		arrayCopy[i] = _arr[i];
	}

	pthread_mutex_unlock(&_mutexArray);
	return arrayCopy;
}

// Get a requested value from the currently sorted array.
int Sorter_getArrayValue(int index)
{
	if (index < 0 || index >= _currentSize)
	{
		// Invalid Array Index
		printf("Invalid Array Index!\n");
		return -1;
	}

	pthread_mutex_lock(&_mutexArray);
	int value = _arr[index];
	pthread_mutex_unlock(&_mutexArray);

	return value;
}

// Get the number of arrays which have been already sorted.
long long Sorter_getNumberArraysSorted()
{
	pthread_mutex_lock(&_mutexCount);
	long long count = _sortedCount;
	pthread_mutex_unlock(&_mutexCount);

	return count;
}


// -- STATIC FUNCTIONS

// Sorter thread main function.
void* startSorterThread()
{
//	printf("Start Sorter Thread!\n");
	pthread_detach(pthread_self());

	while (_sort)
	{
		initArray();
		sortArray();
	}

	pthread_exit((void *) 0);
}

// Free a previously sorted array and initialize a new array to sort.
static void initArray()
{
	pthread_mutex_lock(&_mutexArray);

	if (_arr != NULL)
	{
		freeArray();
	}

	_currentSize = _nextSize;
	_arr = (int *) malloc(_currentSize * sizeof(int));

	for (int i = 0; i < _currentSize; i++)
	{
		_arr[i] = i+1;
	}
	pthread_mutex_unlock(&_mutexArray);

	permuteArray();
}

// Free a currently sorted array.
static void freeArray()
{
	free(_arr);
	_arr = NULL;
}

// Sort the current array using Bubble Sort algorithm.
static void sortArray()
{
	int n = _currentSize;
	for (int i = 0; i < (n-1); i++)
	{
		for (int j = 0; j < (n-1)-i; j++)
		{
			if (_arr[j] > _arr[j+1])
			{
				swap(&_arr[j], &_arr[j+1]);
			}
		}
	}

	pthread_mutex_lock(&_mutexCount);
	_sortedCount++;
	pthread_mutex_unlock(&_mutexCount);

}

// Randomly permute the current array.
static void permuteArray()
{
	int rand;
	for (int i = 0; i < _currentSize; i++)
	{
		rand = getRandomInt(0, _currentSize-1);
		swap(&_arr[i], &_arr[rand]);
	}
}

// Swap two integer values.
static void swap(int* a, int* b)
{
	pthread_mutex_lock(&_mutexArray);
	int temp = *a;
	*a = *b;
	*b = temp;
	pthread_mutex_unlock(&_mutexArray);
}

