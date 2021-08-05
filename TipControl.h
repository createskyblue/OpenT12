#ifndef TipControl_H
#define TipControl_H
#include "Type.h"

extern uint8_t MyMOS;
extern uint8_t PWM;
extern uint16_t TipTemperature;

extern double aggKp, aggKi, aggKd;
extern double consKp, consKi, consKd;

uint16_t GetADC0(void);
double CalculateTemp(uint16_t ADC);
void SetPOWER(uint8_t POWER);
#endif