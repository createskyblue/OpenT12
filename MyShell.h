#ifndef MYSHELL_H
#define MYSHELL_H
#include "Type.h"

extern uint8_t EasyCursor[10][2];
extern uint8_t OLED_ScreenshotFlag;

void shellInit(void);
void ICACHE_RAM_ATTR ShellLoop(void);
int shell_reader(char* data);
void shell_writer(char data);
int command_test(int argc, char** argv);

void OLED_ScreenshotInit(void);
void OLED_ScreenshotPrint(void);
#endif