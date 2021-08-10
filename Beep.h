#ifndef BEEP_H
#define BEEP_H
#include "Type.h"

void BeepInit(void);
void SetTone(double freq);
// void SetTone2(double freq);
double GetNote(note_t note, uint8_t rp);

void SetNote(note_t note, uint8_t rp);
// void SetNote2(note_t note, uint8_t rp);
#endif