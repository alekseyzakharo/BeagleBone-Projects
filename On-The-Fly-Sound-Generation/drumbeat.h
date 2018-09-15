// drumbeat.h
// Drum Beat Generation Module:
// Contains methods to create drum beats and sounds

#ifndef DRUM_BEAT_H
#define DRUM_BEAT_H

void Drumbeat_init(void);
void Drumbeat_cleanup(void);

int Drumbeat_setDrumbeat(int beat);
int Drumbeat_setDrumTempo(int newTempo);

int Drumbeat_getBeatsPerMinute(void);
int Drumbeat_getDrumbeat(void);

void Drumbeat_playHiHat();
void Drumbeat_playSnare();
void Drumbeat_playBass();

#endif
