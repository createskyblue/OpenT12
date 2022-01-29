#ifndef TipControl_H
#define TipControl_H
#include "Type.h"

extern uint8_t MyMOS;
extern uint8_t POWER;
extern uint8_t PWM;
extern uint8_t LastPWM;
extern uint16_t LastADC;
extern double TipTemperature;
extern double PID_Output;
extern double PID_Setpoint;
extern double TempGap;

extern float SamplingRatioWork;
extern float ADC_PID_Cycle_List[3];

extern float aggKp, aggKi, aggKd;
extern float consKp, consKi, consKd;

extern uint8_t PWMOutput_Lock;

extern double Get_MainPowerVoltage(void);
extern double GetCurrent(void);
extern double GetNTCTemp(void);

void TipControlInit(void);
void SetPOWER(uint8_t power);
void PWMOutput(uint8_t pwm);
int GetADC0(void);
double CalculateTemp(double ADC, double P[]);

void TemperatureControlLoop(void);

#endif