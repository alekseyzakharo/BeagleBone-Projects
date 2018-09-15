// command_interface.h
// Command Interface Module:
// Contains methods to control audio, tempo, and sound

#ifndef COMMAND_INTERFACE_H
#define COMMAND_INTERFACE_H

//Beat control methods
int Interface_getBeat(void);
int Interface_setBeat(int beat);
int Interface_nextBeat(void);

//Play single Sound
void Interface_hihat(void);
void Interface_snare(void);
void Interface_bass(void);

//Volume control methods
int Interface_getVolume(void);
int Interface_setVolume(int newVolume);
int Interface_increaseVolume(void);
int Interface_decreaseVolume(void);

//Tempo control methods
int Interface_getTempo(void);
int Interface_setTempo(int newTempo);
int Interface_increaseTempo(void);
int Interface_decreaseTempo(void);

#endif
