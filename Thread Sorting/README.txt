The main purpose of this project is to implement concurrcy with threading with the use of mutex's

The BeagleBone continously sorts integer arrays, with the size determined by the potentiometer on the controller.
The size of the arrays range from 0 - 4096; the method for sorting is bubble sort.
The BeagleBone also displays the number of arrays sorted per second on it's 14-segment digital displays

The user is able to connect to the beaglebone via netcat and request these commands:

Accepted Commands:
count -- display number arrays sorted.						
get length -- display length of array currently being sorted.							
get array -- display the full array being sorted.							
get 10 -- display the tenth element of array currently being sorted.						
stop -- cause the server program to end.
