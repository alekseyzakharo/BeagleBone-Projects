# MAKEFILE

# Compiler Info + Flags
CC = gcc
CC_ARM = arm-linux-gnueabihf-gcc
CFLAGS = -g -Wall -Werror -std=c99 -D _POSIX_C_SOURCE=200809L -pthread
#CFLAGS = -g -Wall -std=c99 -D _POSIX_C_SOURCE=200809L -pthread

# All .c Files
SRC_C_ALL = $(wildcard ./*.c)
SRC_C_SORTER = main.c display.c network.c potentiometer.c sorter.c
SRC_C_NOWORKY = noworky.c

# Build Target
TARGET_SORTER = a2-sorter.out
TARGET_SORTER_ARM = a2-sorter_arm.out
TARGET_NOWORKY = noworky.out
TARGET_NOWORKY_ARM = noworky_arm.out

# Shared Directory Path
SHARE_PATH = $(HOME)/cmpt433/public/myApps/

# Default All
all: compile_target copy_target

# Compile Targets
compile_target:
	$(CC) $(CFLAGS) $(SRC_C_SORTER) -o $(TARGET_SORTER)
	$(CC_ARM) $(CFLAGS) $(SRC_C_SORTER) -o $(TARGET_SORTER_ARM)
	$(CC) $(CFLAGS) $(SRC_C_NOWORKY) -o $(TARGET_NOWORKY)
	$(CC_ARM) $(CFLAGS) $(SRC_C_NOWORKY) -o $(TARGET_NOWORKY_ARM)

# Copy Targets
copy_target:
	cp $(TARGET_SORTER_ARM) $(SHARE_PATH)
	cp $(TARGET_NOWORKY_ARM) $(SHARE_PATH)
	
# Clean-Up
clean:
	rm -f *.o $(TARGET_SORTER) $(TARGET_SORTER_ARM)
	rm -f $(SHARE_PATH)/$(TARGET_SORTER_ARM)
	rm -f *.o $(TARGET_NOWORKY) $(TARGET_NOWORKY_ARM)
	rm -f $(SHARE_PATH)/$(TARGET_NOWORKY_ARM)
