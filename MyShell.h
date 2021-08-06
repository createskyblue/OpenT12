#ifndef MYSHELL_H
#define MYSHELL_H
void shellInit(void);
int shell_reader(char* data);
void shell_writer(char data);
int command_test(int argc, char** argv);
#endif