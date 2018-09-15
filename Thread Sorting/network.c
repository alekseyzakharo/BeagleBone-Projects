// Network Module .c
// Contains networking related functions.

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "display.h"
#include "main.h"
#include "network.h"
#include "potentiometer.h"
#include "sorter.h"

#define MSG_MAX_LEN 1024
#define MAX_LINES 220
#define MAX_CHAR_PER_LINE 100
#define PORT 12345
#define INT_TO_CHAR_MULT 6

// Static Functions
static void *listenUDP();
static void arrToChar(int* arr, int arrSize);
static int validateString(char* str, int length);

// Static Variables
static int _STOP = 0;
static int ARRAY_STRING = 1;
static char** ARRAY;
static const char* HELP = "Accepted Commands:\n"
							"count -- display number arrays sorted.\n"
							"get length -- display length of array currently being sorted.\n"
							"get array -- display the full array being sorted.\n"
							"get 10 -- display the tenth element of array currently being sorted.\n"
							"stop -- cause the server program to end.\n";

static pthread_t _thread;

// Initialize network module and start network thread.
int Network_init()
{
	if(pthread_create(&_thread, NULL, listenUDP, (void*)NULL))
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
}


// -- STATIC FUNCTIONS

// Network thread main function.
static void *listenUDP()
{
//	printf("Start Network Thread!\n");
	pthread_detach(pthread_self());

	printf("Connect Using: \n");
	printf("netcat -u 192.168.7.2 %d\n", PORT);

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
		unsigned int sin_len = sizeof(sin);
		int bytesRx = recvfrom(socketDescriptor, messageGet, MSG_MAX_LEN, 0,
				(struct sockaddr *) &sin, &sin_len);

		messageGet[bytesRx-1] = '\0'; // this gets rid of the trailing next line character and adds null character
		printf("Message Received (%d bytes): \n\n'%s'\n", bytesRx, messageGet);

		if (bytesRx == 1)
		{
			continue;
		}
		else if (strstr(messageGet, "help") != NULL)
		{
			sprintf(messageSent, "%s\n", HELP);
		}
		else if (strstr(messageGet, "count") != NULL)
		{
			long long count = Sorter_getNumberArraysSorted();
			sprintf(messageSent, "Number of Sorted Arrays:\t%lld\n", count);
		}
		else if (strstr(messageGet, "get") != NULL)
		{
			if (strstr(messageGet, "length") != NULL)
			{
				int length = Sorter_getArrayLength();
				sprintf(messageSent, "Current Array Length:\t%d\n", length);
			}
			else if (strstr(messageGet, "array") != NULL)
			{
				int* length = (int *) malloc(sizeof(int));
				int* arr = Sorter_getArrayData(length);
				arrToChar(arr, *length);
				free(arr);
				free(length);
				arr = NULL;
				length = NULL;
			}
			else
			{
				printf("message: %s\n", messageGet);
				char* tokens = strtok(messageGet, " ");
				tokens = strtok(NULL, "\n");

				if(!validateString(tokens, strlen(tokens)))
				{
					sprintf(messageSent, "Invalid Command!\n");
				}
				else
				{
					int index = atoi(tokens);
					int value = Sorter_getArrayValue(index);
					if(value != -1)
					{
						sprintf(messageSent, "Current Array Value [%d] = %d\n", index, value);
					}
					else
					{
						sprintf(messageSent, "Index greater than array length!\n");
					}
				}
				tokens = NULL;
			}
		}
		else if (strstr(messageGet, "stop") != NULL)
		{
			sprintf(messageSent, "Good Bye!");

			// CALL STOP FUNCTIONS OF ALL OTHER MODULES
			Display_stop();
			Sorter_stopSorting();
			Potenio_stopReading();
			Network_stop();

			// Unlock Main Mutex
			unlockMutex();
		}
		else
		{
			sprintf(messageSent, "Invalid Command!\n");
		}

		// Transmit the server response.
		sin_len = sizeof(sin);
		if (ARRAY_STRING > 1)
		{
			for (int i =0;i < ARRAY_STRING; i++)
			{
				sendto(socketDescriptor, ARRAY[i], strlen(ARRAY[i]), 0, (struct sockaddr *) &sin, sin_len);
				free(ARRAY[i]);
				ARRAY[i] = NULL;
			}
			ARRAY_STRING = 1;
			free(ARRAY);
			ARRAY = NULL;
		}
		else
		{
			sendto(socketDescriptor, messageSent, strlen(messageSent), 0, (struct sockaddr *) &sin, sin_len);
		}
	}

	// Close Socket
	close(socketDescriptor);
	pthread_exit((void *) 0);
}

// Transform an integer array to a character array (string).
static void arrToChar(int* arr, int arrSize)
{
	ARRAY  = (char **) malloc(MAX_LINES * sizeof(char*));
	int iterator = 0;
	ARRAY[iterator] = (char *) malloc(MAX_CHAR_PER_LINE * sizeof(char));
	ARRAY[iterator][0] = '\0';

	for (int i = 0;i < arrSize; i++)
	{
		char * num = (char *) malloc(4* sizeof(char));
		sprintf(num, "%d", arr[i]);
		strcat(ARRAY[iterator],num);
		if(i != arrSize-1)
		{
			strcat(ARRAY[iterator], ", ");
		}
		if ((i+1)%10 == 0)
		{
			strcat(ARRAY[iterator], "\n");
			ARRAY[++iterator] = (char *) malloc(MAX_CHAR_PER_LINE * sizeof(char));
			ARRAY[iterator][0] = '\0';
			ARRAY_STRING++;
		}
		free(num);
	}
	strcat(ARRAY[iterator], "\n");
}

// Validate a specified string.
static int validateString(char* str, int length)
{
	for (int i = 0;i < length; i++)
	{
		if (str[i] < '0' || str[i] > '9')
		{
			return 0;
		}
	}
	return 1;
}


