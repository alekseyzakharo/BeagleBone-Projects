//MAIN Module .h

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "joystick.h"
#include "led.h"

#define TOP_LED 0
#define BOTTOM_LED 3

typedef enum direction {None, Up, Down, Left, Right} direction;

static void ledGame(void);

int main(void)
{
  ledGame();
  return 0;
}

static void ledGame() {
  printf("Press the Zen cape's Joystick in the direction of the LED.\n");
  printf("\tUP for LED 0 (top)\n\tDOWN for LED3 (bottom)\n\tLEFT/RIGHT for exit app.\n");

  initJoystick();
  initLed();

  int iterator = 0; //intialize i outside of loop, as per c99
  int correctAns = 0;
  enum direction dir;

  time_t t;
  srand((unsigned) time(&t));

  while(1) { //loop continously

    dir = None;
    int led = (rand()%2);
    if(led) {
      turnOnLed(TOP_LED);
    } else {
      turnOnLed(BOTTOM_LED);
    }
    printf("Press joystick; current score (%d / %d)\n",correctAns, iterator);
    while(dir == None)
    {
      dir = joystickDirection();
    }
    if((led && dir == Up) || (!led && dir == Down)) {
      correctAns++;
      flashAllLED(1);
      printf("Correct!\n");
    } else if(dir == Left || dir == Right) {
      printf("Your final score was (%d / %d )\n",correctAns, iterator);
      printf("Thank you for playing!\n");
      turnOffAllLed();
      break;
    } else {
      flashAllLED(5);
      printf("Incorrect!\n");
    }
    while(1)
    {
      if(joystickDirection() == None) {
        break;
      }
    }
    iterator++;
  }
}
