// drumbeat.c
// Drum Beat Generation Module:
// Contains methods to create drum beats and sounds

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#include "drumbeat.h"
#include "audioMixer_template.h"

//Drumbeat .wave file paths
#define BEAT_HI_HAT "/mnt/remote/myApps/beatbox-wav-files/drum-sounds/100053__menegass__gui-drum-cc.wav"
#define BEAT_SNARE "/mnt/remote/myApps/beatbox-wav-files/drum-sounds/100059__menegass__gui-drum-snare-soft.wav"
#define BEAT_BASS_DRUM "/mnt/remote/myApps/beatbox-wav-files/drum-sounds/100051__menegass__gui-drum-bd-hard.wav"

typedef enum Drumbeat {None, Drumbeat1, Drumbeat2, Drumbeat3} drumbeat;

#define MAXTEMPO 300
#define MINTEMPO 40

static int BEATS_PER_MINUTE;
static long tempoSleepNano;
static enum Drumbeat BEAT;
static wavedata_t *hihat,*bassdrum,*snare;


//static functions
static void calculateSleepTempo(void);
static void drumbeatSleep(void);
static void drumbeat1(void);
static void drumbeat2(void);
static void drumbeat3(void);
void* drumbeatThread(void* arg);

static _Bool stopping = false;

static pthread_t drumbeatThreadId;

static pthread_mutex_t beatsPerMinuteMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t drumbeatMutex = PTHREAD_MUTEX_INITIALIZER;


void Drumbeat_init()
{
	BEATS_PER_MINUTE = 120;
	calculateSleepTempo();
	BEAT = None;

	//initialize the drum sounds
	hihat = malloc(sizeof(wavedata_t));
	bassdrum = malloc(sizeof(wavedata_t));
	snare = malloc(sizeof(wavedata_t));

	AudioMixer_readWaveFileIntoMemory(BEAT_HI_HAT,hihat);
	AudioMixer_readWaveFileIntoMemory(BEAT_SNARE,snare);
	AudioMixer_readWaveFileIntoMemory(BEAT_BASS_DRUM,bassdrum);

	// Launch drumbeat thread:
	pthread_create(&drumbeatThreadId, NULL, drumbeatThread, NULL);
}

void Drumbeat_cleanup()
{
	printf("Stopping Drum Beat...\n");

	free(hihat);
	free(snare);
	free(bassdrum);

	stopping = true;
	pthread_join(drumbeatThreadId, NULL);

	//destroy mutex's
	pthread_mutex_destroy(&beatsPerMinuteMutex);
	pthread_mutex_destroy(&drumbeatMutex);

	fflush(stdout);
}

//maybe this function can be changed to increment between the beats in a BEAT++ fashion?
int Drumbeat_setDrumbeat(int beat)
{
	if(beat > 3)
	{
		printf("Drumbeat %d does not exist", beat);
		return -1;
	}
	pthread_mutex_lock(&drumbeatMutex);
	switch(beat)
	{
		case 0:
			BEAT = None;
			break;
		case 1:
			BEAT = Drumbeat1;
			break;
		case 2:
			BEAT = Drumbeat2;
			break;
		case 3:
			BEAT = Drumbeat3;
			break;
		default:
			break;
	}

	pthread_mutex_unlock(&drumbeatMutex);
	return 0;
}

int Drumbeat_setDrumTempo(int newTempo)
{
	if(newTempo < MINTEMPO || newTempo > MAXTEMPO)
	{
		return -1;
	}
	pthread_mutex_lock(&beatsPerMinuteMutex);
	BEATS_PER_MINUTE = newTempo;
	calculateSleepTempo();
	pthread_mutex_unlock(&beatsPerMinuteMutex);
	return 0;
}

int Drumbeat_getBeatsPerMinute(void)
{
	return BEATS_PER_MINUTE;
}


void* drumbeatThread(void* arg)
{
	while(!stopping)
	{
		switch(BEAT)
		{
			case None:
				nanosleep((const struct timespec[]){{1, 0}}, NULL); //sleep for 1 second
				break;
			case Drumbeat1:
				drumbeat1();
				break;
			case Drumbeat2:
				drumbeat2();
				break;
			case Drumbeat3:
				drumbeat3();
				break;
			default:
				break;
		}
	}
	return NULL;
}

static void calculateSleepTempo()
{
	//(60 seconds)/(beats per minute)/(single stereo channel) * (1000000000: convert to nano)
	tempoSleepNano = (60.0/((float)BEATS_PER_MINUTE)/2.0)*1000000000;
}

static void drumbeatSleep()
{
	nanosleep((const struct timespec[]){{0, tempoSleepNano}}, NULL);
}

//CHANGE PATH TO NOT BE FROM ROOT
static void drumbeat1()
{
	for(int i =0;i<2;i++) //do this twice
	{
		Drumbeat_playHiHat();
		Drumbeat_playBass();
		drumbeatSleep();

		Drumbeat_playHiHat();
		drumbeatSleep();

		Drumbeat_playHiHat();
		Drumbeat_playSnare();
		drumbeatSleep();

		Drumbeat_playHiHat();
		drumbeatSleep();
	}
}


static void drumbeat2()
{
	for(int i = 0; i < 2; i++)
	{
		Drumbeat_playHiHat();
		Drumbeat_playBass();
		drumbeatSleep();

		Drumbeat_playHiHat();
		drumbeatSleep();

		Drumbeat_playHiHat();
		Drumbeat_playBass();
		Drumbeat_playSnare();
		drumbeatSleep();

		Drumbeat_playHiHat();
		drumbeatSleep();
	}
}

static void drumbeat3()
{
	for(int i = 0; i < 4; i++)
	{
		if(i%2 == 0)
		{
			Drumbeat_playBass();
		}
		else
		{
			Drumbeat_playSnare();
		}
		Drumbeat_playHiHat();
		drumbeatSleep();
	}
}

void Drumbeat_playHiHat()
{
	AudioMixer_queueSound(copyWaveDate_T(hihat));
}

void Drumbeat_playSnare()
{
	AudioMixer_queueSound(copyWaveDate_T(snare));
}

void Drumbeat_playBass()
{
	AudioMixer_queueSound(copyWaveDate_T(bassdrum));
}

int Drumbeat_getDrumbeat()
{
	switch(BEAT)
	{
		case None:
			return 0;
			break;
		case Drumbeat1:
			return 1;
			break;
		case Drumbeat2:
			return 2;
			break;
		case Drumbeat3:
			return 3;
			break;
		default:
			break;
	}
	return -1;
}


