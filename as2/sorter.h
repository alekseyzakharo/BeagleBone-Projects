// CMPT 433 - Assignment 2
// sorter.h
// Sorter Module:
// Contains sorter related functions.

#ifndef _SORTER_H_
#define _SORTER_H_

// Initialize sorter module and start sorter thread.
int Sorter_init();

// Cleanup sorter module.
void Sorter_cleanup();

// Stop sorter thread.
void Sorter_stopSorting();

// Set the size of the next array to sort.
void Sorter_setArraySize(int newSize);

// Get the size of the array currently being sorted.
int Sorter_getArrayLength();

// Get a copy of the currently sorted array.
int* Sorter_getArrayData(int *length);

// Get a requested value from the currently sorted array.
int Sorter_getArrayValue(int index);

// Get the number of arrays which have been already sorted.
long long Sorter_getNumberArraysSorted();

#endif
