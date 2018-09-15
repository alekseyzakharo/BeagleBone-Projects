//Aleksey Zakharov 301295506
//Barebone Assignment: timer.h
//CMPT 433 - Assignment 5

#ifndef _TIMER_H_
#define _TIMER_H_
#include <stdbool.h>

// Start the timer
void Timer_init(void);

// Timer tick controls:
_Bool Timer_isIsrFlagSet(void);
void Timer_clearIsrFlag(void);

// Register your own callback function
void Timer_setTimerIsrCallbackOne(void (*isrCallback)(void));
void Timer_setTimerIsrCallbackTwo(void (*isrCallback)(void));


#endif
