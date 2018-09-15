// command_interface.c
// Command Interface Module
// Contains methods to control audio, tempo, and sound

#include <stdio.h>

#include "command_interface.h"
#include "drumbeat.h"
#include "audioMixer_template.h"

//DRUMBEAT
int Interface_getBeat(void)
{
	return Drumbeat_getDrumbeat();
}

int Interface_setBeat(int beat)
{
	return Drumbeat_setDrumbeat(beat);
}

int Interface_nextBeat(void)
{
	return Interface_setBeat((Interface_getBeat()+1)%4);
}

void Interface_hihat(void)
{
	Drumbeat_playHiHat();
}

void Interface_snare(void)
{
	Drumbeat_playSnare();
}

void Interface_bass(void)
{
	Drumbeat_playBass();
}

//VOLUME
int Interface_getVolume(void)
{
	return AudioMixer_getVolume();
}

int Interface_setVolume(int newVolume)
{
	if(newVolume < 0 || newVolume > 100)
	{
		printf("Invalid Volume: %d\n", newVolume);
		return -1;
	}
	AudioMixer_setVolume(newVolume);
	return 0;
}

int Interface_increaseVolume(void)
{
	int volume = Interface_getVolume();
	if(volume >= 100)
	{
		printf("Cannot increase volume anymore\n");
		return -1;
	}
	Interface_setVolume(volume+5);
	return 0;
}

int Interface_decreaseVolume(void)
{
	int volume = Interface_getVolume();
	if(volume <= 0)
	{
		printf("Cannot decrease volume anymore\n");
		return -1;
	}
	Interface_setVolume(volume-5);
	return 0;
}


//TEMPO
int Interface_getTempo(void)
{
	return Drumbeat_getBeatsPerMinute();
}

int setTempo(int newTempo)
{
	if(newTempo < 40 || newTempo > 300)
	{
		printf("Invalid new Tempo\n");
		return -1;
	}
	Drumbeat_setDrumTempo(newTempo);
	return 0;
}

int Interface_increaseTempo(void)
{
	int tempo = Drumbeat_getBeatsPerMinute();
	if(tempo >= 300)
	{
		printf("Cannot increase tempo anymore\n");
		return -1;
	}
	setTempo(tempo+5);
	return 0;
}

int Interface_decreaseTempo(void)
{
	int tempo = Drumbeat_getBeatsPerMinute();
	if(tempo <= 40)
	{
		printf("Cannot decrease tempo anymore\n");
		return -1;
	}
	setTempo(tempo-5);
	return 0;
}

