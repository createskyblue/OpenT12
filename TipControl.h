#ifndef TipControl_H
#define TipControl_H
#include "Type.h"



extern uint8_t MyMOS;
extern uint8_t PWM;
extern double TipTemperature;
extern double PID_Output;
extern double PID_Setpoint;

extern double aggKp, aggKi, aggKd;
extern double consKp, consKi, consKd;

void TipControlInit(void);
void PWMOutput(uint8_t pin, uint8_t pwm);
int GetADC0(void);
double CalculateTemp(uint16_t ADC);
void SetPOWER(uint8_t POWER);

void TemperatureControlLoop(void);
#endif