// network.c
// Network Module
// Contains networking related functions.

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "network.h"
#include "command_interface.h"

#define MSG_MAX_LEN 1024
#define MAX_LINES 220
#define MAX_CHAR_PER_LINE 100
#define PORT 12345
#define INT_TO_CHAR_MULT 6


// Static Functions
static void *listenUDP();
static void getUpTime(void);

// Static Variables
static int hours;
static int minutes;
static int seconds;

static int _STOP = 0;
static pthread_t networkThreadId;

// Initialize network module and start network thread.
int Network_init()
{
	hours = 0;
	minutes = 0;
	seconds = 0;
	if(pthread_create(&networkThreadId, NULL, listenUDP, (void*)NULL))
	{
		printf("Error Creating Network Thread\n");
		return -1;
	}

	return 0;
}

// Stop network thread.
void Network_stop()
{
	printf("Stop Network Thread!\n");
	_STOP = 1;

	pthread_join(networkThreadId, NULL);
}


// -- STATIC FUNCTIONS

// Network thread main function.
static void *listenUDP()
{
//	printf("Start Network Thread!\n");
	pthread_detach(pthread_self());

	// Buffer to hold packet data.
	char messageGet[MSG_MAX_LEN];
	char messageSent[MSG_MAX_LEN];

	// Socket Address
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;                   // Connection may be from network.
	sin.sin_addr.s_addr = htonl(INADDR_ANY);    // Host to Network long.
	sin.sin_port = htons(PORT);                 // Host to Network short.

	// Create the socket for UDP.
	int socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);

	// Bind the socket to the port (PORT) that we specify.
	bind (socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));

	while (!_STOP)
	{
		memset(&messageGet,'\0', sizeof(messageGet));
		unsigned int sin_len = sizeof(sin);
		int bytesRx = recvfrom(socketDescriptor, messageGet, MSG_MAX_LEN, 0,
				(struct sockaddr *) &sin, &sin_len);

		printf("Message Received (%d bytes): \n\n'%s'\n", bytesRx, messageGet);

		if (strstr(messageGet, "mode-none") != NULL)
		{
			Interface_setBeat(0);
			printf("Beat Change to None\n");
			//sprintf(messageSent, "%s\n", HELP);
		}
		else if (strstr(messageGet, "mode-rock-1") != NULL)
		{
			Interface_setBeat(1);
			printf("Beat Change to Rock#1\n");
		}
		else if (strstr(messageGet, "mode-rock-2") != NULL)
		{
			Interface_setBeat(2);
			printf("Beat Change to Rock#2\n");
		}
		else if (strstr(messageGet, "mode-rock-3") != NULL)
		{
			Interface_setBeat(3);
			printf("Beat Change to Rock#3\n");
		}
		else if (strstr(messageGet, "volume-up") != NULL)
		{
			Interface_increaseVolume();
			printf("Volume Increased\n");
		}
		else if (strstr(messageGet, "volume-down") != NULL)
		{
			Interface_decreaseVolume();
			printf("Volume Decreased\n");
		}
		else if (strstr(messageGet, "tempo-up") != NULL)
		{
			Interface_increaseTempo();
			printf("Tempo Increased\n");
		}
		else if (strstr(messageGet, "tempo-down") != NULL)
		{
			Interface_decreaseTempo();
			printf("Tempo Decreased\n");
		}
		else if (strstr(messageGet, "snd-snare") != NULL)
		{
			Interface_snare();
			printf("Playing Sound Snare\n");
		}
		else if (strstr(messageGet, "snd-base") != NULL)
		{
			Interface_bass();
			printf("Playing Sound Bass\n");
		}
		else if (strstr(messageGet, "snd-hihat") != NULL)
		{
			Interface_hihat();
			printf("Playing Sound Hihat\n");
		}
		else if (strstr(messageGet, "ping-pong") != NULL)
		{
			getUpTime();
			sprintf(messageSent, "ping %d %d %d %d:%d:%d(H:M:S)", Interface_getVolume(), Interface_getTempo(), Interface_getBeat(), hours, minutes, seconds);
			sendto(socketDescriptor, messageSent, strlen(messageSent), 0, (struct sockaddr *) &sin, sin_len);
		}

	}
	// Close Socket
	close(socketDescriptor);
	return NULL;
}

//read uptime of beaglebone
static void getUpTime(void)
{
  FILE *file = fopen("/proc/uptime", "r");
  if (file == NULL) {
  printf("ERROR: Unable to open file (/proc/uptime) for read\n");
    exit(-1);
  }
  float x, y;

  fscanf(file, "%f %f", &x, &y);
  fclose(file);

  hours = (((int)x/60)/60)%24;
  minutes = ((int)x/60)%60;
  seconds = (int)x%60;

  return;
}




