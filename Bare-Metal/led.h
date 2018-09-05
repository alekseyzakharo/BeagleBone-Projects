//Baremetal Project: led.h

#ifndef _LED_H_
#define _LED_H_

//initialize LED module
void LED_init(void);

//Set Flash period to speed [1....9]
void LED_setFlashPeriod(int speed);

//Set Pattern 'a' or 'b'
void LED_setPattern(char pattern);

//switches from one pattern to the other without knowing which pattern you are on
void LED_switchPattern(void);

//light up LED's depending current pattern
void LED_lightUp(void);

//call back function
void LED_notifyOnTimeIsr(void);

//increment speed without knowing what speed you are on
void LED_incrementSpeed(void);

//decrement speed without knowing what speed you are on
void LED_decrementSpeed(void);


#endif
