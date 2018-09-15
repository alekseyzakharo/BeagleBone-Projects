//JOYSTICK Module .h

#ifndef __JOYSTICK_H_
#define __JOYSTICK_H_

void initJoystick(void);
void writeToExportFile(int gpio);
int readGPIOStatus(char *fileName);
int readFileStatus(char *fileName);
enum direction joystickDirection(void);

#endif
