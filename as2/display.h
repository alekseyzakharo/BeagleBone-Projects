// CMPT 433 - Assignment 2
// display.h
// Display Module:
// Contains 14-segment display related functions.

#ifndef __DISPLAY_H_
#define __DISPLAY_H_

// Initialize display module and start display thread.
int Display_init();

// Stop display thread.
void Display_stop();

// Cleanup display module.
void Display_cleanup();

// Set the 14-segment display digits to a specified value.
void Display_setDigits(char* nums);

#endif
