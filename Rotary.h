#ifndef sys_Counter_H
#define sys_Counter_H
#include <stdint.h>
#include "Type.h"

#define ROTARY_PIN1	16
#define ROTARY_PIN2	17
#define BUTTON_PIN	5

#define ROTARY_TYPE 2

extern uint8_t SYSKey;

void sys_RotaryInit(void);
void sys_Counter_Set(double min, double max, double step, double c);
void sys_Counter_SetVal(double c);
void sys_Counter_IRQHandler(void);
double sys_Counter_Get(void);
uint8_t sys_Counter_Change(void);
uint8_t sys_KeyProcess(void);
#endif