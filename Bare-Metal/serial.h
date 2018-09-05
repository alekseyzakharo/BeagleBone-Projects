//Baremetal Project: serial.h

#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <stdint.h>

//serial module init
void Serial_init();

//serial module call back
void Serial_setRxIsrCallback(void (*rxIsrCallback)(uint8_t));

#endif
