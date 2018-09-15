//LED Module .h

#ifndef __LED_H_
#define __LED_H_

void initLed(void);
void setLedTriggerNone(char* ledPath);
void flashAllLED(int numberOfFlashes);
void turnOnLed(int ledNum);
void turnOffLed(int ledNum);
char* selectLedPath(int ledNum);
void writeToLedFile(char *path, char* str);
void turnOffAllLed(void);

#endif
