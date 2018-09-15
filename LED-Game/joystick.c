//JOYSTICK Module .c

#include "joystick.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int left = 65;
static int right = 47;
static int up = 26;
static int down = 46;

typedef enum direction {None, Up, Down, Left, Right} direction;

void initJoystick(void) {
  writeToExportFile(left);
  writeToExportFile(right);
  writeToExportFile(up);
  writeToExportFile(down);
}

void writeToExportFile(int gpio) {
  // Use fopen() to open the file for write access.
  FILE *pfile = fopen("/sys/class/gpio/export", "w");
  if (pfile == NULL) {
    printf("ERROR: Unable to open export file.\n");
    exit(-1);
  }
  // Write to data to the file using fprintf():
  fprintf(pfile, "%d", gpio);
  // Close the file using fclose():
  fclose(pfile);
}

//contatenates string to create correct file location of gpio
int readGPIOStatus(char *gpio) {
  char* value = malloc(strlen("/sys/class/gpio/")+strlen(gpio)+strlen("/value")+1);
  strcpy(value, "/sys/class/gpio/");
  strcat(value, gpio);
  strcat(value, "/value\0");

  int stat = readFileStatus(value);
  free(value);

  return stat;
}

//read gpio file from location, file has to either contain a 0 or 1 as its value
int readFileStatus(char *fileName) {
  FILE *file = fopen(fileName, "r");
  if (file == NULL) {
  printf("ERROR: Unable to open file (%s) for read\n", fileName);
    exit(-1);
  }
  // Read string (line)
  const int max_length = 1024;
  char buff[max_length];
  fgets(buff, max_length, file);
  // Close
  fclose(file);
  //printf("Read: '%s' \n", buff);
  if(*buff == '0')
    return 0;
  return 1;
}

enum direction joystickDirection(void) {
  if(!readGPIOStatus("gpio65")) {
    return Left;
  }
  if(!readGPIOStatus("gpio47")) {
    return Right;
  }
  if(!readGPIOStatus("gpio26")) {
    return Up;
  }
  if(!readGPIOStatus("gpio46")) {
    return Down;
  }
  return None;
}
