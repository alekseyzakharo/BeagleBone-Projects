//LED Module .c

#include <stdio.h>
#include <time.h>
#include "led.h"

#define LED0 0
#define LED1 1
#define LED2 2
#define LED3 3

#define LED0_TRIGGER "/sys/class/leds/beaglebone:green:usr0/trigger"
#define LED1_TRIGGER "/sys/class/leds/beaglebone:green:usr1/trigger"
#define LED2_TRIGGER "/sys/class/leds/beaglebone:green:usr2/trigger"
#define LED3_TRIGGER "/sys/class/leds/beaglebone:green:usr3/trigger"

#define LED0_BRIGHTNESS "/sys/class/leds/beaglebone:green:usr0/brightness"
#define LED1_BRIGHTNESS "/sys/class/leds/beaglebone:green:usr1/brightness"
#define LED2_BRIGHTNESS "/sys/class/leds/beaglebone:green:usr2/brightness"
#define LED3_BRIGHTNESS "/sys/class/leds/beaglebone:green:usr3/brightness"


void initLed(void) {
  setLedTriggerNone(LED0_TRIGGER);
  setLedTriggerNone(LED1_TRIGGER);
  setLedTriggerNone(LED2_TRIGGER);
  setLedTriggerNone(LED3_TRIGGER);
  turnOffAllLed();
}

void setLedTriggerNone(char* ledPath) {

  FILE *pLedTriggerFile = fopen(ledPath, "w");

  if (pLedTriggerFile == NULL) {
    printf("ERROR OPENING %s.", ledPath);
  }

  int charWritten = fprintf(pLedTriggerFile, "none");
  if (charWritten <= 0) {
    printf("ERROR WRITING DATA");
  }

  fclose(pLedTriggerFile);
}

void flashAllLED(int numberOfFlashes) {

  int i; //iterator
  long seconds = 0;
  long nanoseconds = 100000000;
  struct timespec reqDelay = {seconds, nanoseconds};

  for(i = 0; i < numberOfFlashes; i++) {
    turnOnLed(LED0);
    turnOnLed(LED1);
    turnOnLed(LED2);
    turnOnLed(LED3);
    nanosleep(&reqDelay, (struct timespec *) NULL);
    turnOffLed(LED0);
    turnOffLed(LED1);
    turnOffLed(LED2);
    turnOffLed(LED3);
    nanosleep(&reqDelay, (struct timespec *) NULL);
  }
}

void turnOnLed(int ledNum) {
  writeToLedFile(selectLedPath(ledNum),"1");
}

void turnOffLed(int ledNum) {
  writeToLedFile(selectLedPath(ledNum),"0");
}

char* selectLedPath(int ledNum) {
  switch(ledNum) {
    case 0:
      return LED0_BRIGHTNESS;
    break;
    case 1:
      return LED1_BRIGHTNESS;
    break;
    case 2:
      return LED2_BRIGHTNESS;
    break;
    case 3:
      return LED3_BRIGHTNESS;
    break;
  }
  return "";
}

void writeToLedFile(char *path, char *str) {
  FILE *pLedBrightnessFile = fopen(path, "w");
  if (pLedBrightnessFile == NULL) {
    printf("ERROR OPENING %s.", path);
  }
  int charWritten = fprintf(pLedBrightnessFile, "%s",str);
  if (charWritten <= 0) {
    printf("ERROR WRITING DATA LED%d", str);
  }
  fclose(pLedBrightnessFile);
}

void turnOffAllLed() {
  int i;
  for(i = 0;i < 4; i++) {
    turnOffLed(i);
  }
}
