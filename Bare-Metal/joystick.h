//Baremetal Project: Joystick.h

#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

//init joystick module
void Joystick_init(void);

//call back function
void Joystick_notifyOnTimeIsr(void);

//return 1 if left joystick pressed and released, reset left joystick bool, 0 otherwise
int Joystick_getLeftPressed(void);

//return 1 if up joystick pressed and released, reset up joystick bool, 0 otherwise
int Joystick_getUpPressed(void);

//return 1 if down joystick pressed and released, reset down joystick bool, 0 otherwise
int Joystick_getDownPressed(void);


#endif
