#ifndef sys_Counter_H
#define sys_Counter_H
#include <c_types.h>
#include "Type.h"

#define ROTARY_PIN1	D3
#define ROTARY_PIN2	D4

#define ROTARY_TYPE 2

void sys_RotaryInit(void);
void sys_Counter_Set(double min, double max, double step, double c);
void sys_Counter_IRQHandler(void);
double sys_Counter_Get(void);
uint8_t sys_KeyProcess(void);
#endif