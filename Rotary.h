#ifndef sys_Counter_H
#define sys_Counter_H
#include <stdint.h>
void sys_Counter_Set(float min, float max, float step, float c);
void sys_Counter_Add(void);
void sys_Counter_Less(void);
float sys_Counter_Get(void);
uint8_t sys_KeyProcess(void);
#endif