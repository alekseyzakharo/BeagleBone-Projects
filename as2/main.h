// CMPT 433 - Assignment 2
// main.h
// Main Module:
// Contains utility functions used by other modules.

#ifndef MAIN_H_
#define MAIN_H_

// Get random integer in a range [min, max].
int getRandomInt(int min, int max);

// Write a specified value to a file with a specified file path.
int writeToFile(char* path, char* value);

// Lock/Unlock Main Mutex
void unlockMutex(void);
void lockMutex(void);

#endif
