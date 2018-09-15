// CMPT 433 - Assignment 2
// potentiometer.h
// Potentiometer Module:
// Contains potentiometer related functions.

#ifndef __POTENTIOMETER_H_
#define __POTENTIOMETER_H_

// Initialize potentiometer module and start potentiometer thread.
int Potentio_init(void);

// Cleanup potentiometer module.
void Potentio_cleanup();

// Stop potentiometer thread.
void Potenio_stopReading();

#endif
