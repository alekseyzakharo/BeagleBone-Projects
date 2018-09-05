This purpose of this project is to creates a kernel driver for the BeagleBone

We first register a misc driver which create an LED trigger. Writing to the virtual driver file will append all 
strings written to it. When this file is read, you will have a blocking call which will output the morsecode for the
string onto the screen while simonteneously triggering the LED's to blink it out, only if the LED trigger is set first.