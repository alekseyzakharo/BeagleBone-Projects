The purpose of this project is to create a baremetal application that runs on the BeagleBone, the code for this 
application is downloaded onto the BeagleBone via Uboot during startup from a networked file location.

The project demonstrated direct register access and manipulation, foreground/background tasks, watchdog interaction
and method passing.

This program has 2 techniques of blinking LED's, there is also a speed for each technique. The user's options are:
 ? : Display this help message.
 0-9 : Set speed 0 (slow) to 9 (fast).
 a : Select pattern A (bounce).
 b : Select pattern B (bar).
 x : Stop hitting the watchdog (exit).
