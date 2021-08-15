#ifndef SYSEvent_H
#define SYSEvent_H
#include "Type.h"

extern uint32_t BoostTimer;

extern uint32_t EventTimerUpdate;
void TimerUpdateEvent(void);

void BoostButton_EventLoop(void);
void ShutdownEventLoop(void);

void TimerEventLoop(void);
void SYS_StateCode_Update(void);
#endif