#include "OpenT12.h"

void SetTargetTemperature(int argc, char** argv) {
    sys_Counter_SetVal(atof(argv[1]));
}

void SetPIDSampleTime(int argc, char** argv) {
    PIDSampleTime = atof(argv[1]);
}

void SetPID_P(int argc, char** argv) {
    consKp = atof(argv[1]);
}
void SetPID_I(int argc, char** argv) {
    consKi = atof(argv[1]);
}
void SetPID_D(int argc, char** argv) {
    consKd = atof(argv[1]);
}

void SetK_Q(int argc, char** argv) {
    KFP_Temp.Q = atof(argv[1]);
}

void SetK_R(int argc, char** argv) {
    KFP_Temp.R = atof(argv[1]);
}

// void SetEventTimer(int argc, char** argv) {
//     TimerEventTimer = atof(argv[1]);
// }
//////////////////////////////////////////////////////////////////////////
uint8_t EasyCursor[10][2]={
    {74,37}, //温度状态图标
    {91,40}, //状态秒速文字
};
void SetEasyCursor(int argc, char** argv) {
    uint8_t id = atof(argv[1]);
    if (id >=10) return;
    EasyCursor[id][0] = atof(argv[2]);
    EasyCursor[id][1] = atof(argv[3]);
}

//////////////////////////////////////////////////////////////////////////
//截图工具
uint8_t OLED_ScreenshotFlag = 0;
void OLED_ScreenshotInit(void) {
    OLED_ScreenshotFlag = 1;
}
void OLED_ScreenshotPrint(void) {
    if (!OLED_ScreenshotFlag) return;
    Serial.print("OLED_ScreenshotInit\r\n"); //固定请求头
    //SerialBT.print("OLED_ScreenshotInit\r\n"); //固定请求头
    uint8_t* p = (uint8_t*)Disp.getBufferPtr();
    for (uint32_t i = 0;i < SCREEN_PAGE_NUM * SCREEN_COLUMN;i++) {
        Serial.write(*p++);
        //SerialBT.write(*p++);
    }
    OLED_ScreenshotFlag = 0;
}
//////////////////////////////////////////////////////////////////////////

void shellInit(void) {
    shell_init(shell_reader, shell_writer, 0);
    shell_register(command_test, PSTR("test"));

    shell_register(SetPIDSampleTime, PSTR("SetPID_ST"));
    shell_register(SetTargetTemperature, PSTR("SetTemp"));
    shell_register(SetPID_P, PSTR("SetPID_P"));
    shell_register(SetPID_I, PSTR("SetPID_I"));
    shell_register(SetPID_D, PSTR("SetPID_D"));

    shell_register(SetK_Q, PSTR("SetK_Q"));
    shell_register(SetK_R, PSTR("SetK_R"));

    shell_register(SetEasyCursor, PSTR("SetEasyCursor"));
    shell_register(OLED_ScreenshotInit, PSTR("OLED_ScreenshotInit"));
    // shell_register(SetEventTimer, PSTR("SetEventTimer"));

}
void ICACHE_RAM_ATTR ShellLoop(void) {
    //命令解析器
    while (Serial.available()) shell_task();
}

/**
 * Function to read data from serial port
 * Functions to read from physical media should use this prototype:
 * int my_reader_function(char * data)
 */
int shell_reader(char* data)
{
    if (Serial.available()) {
        *data = Serial.read();
        return 1;
    }
    // if (SerialBT.available()) {
    //     *data = SerialBT.read();
    //     return 1;
    // }
    return 0;
}

/**
 * Function to write data to serial port
 * Functions to write to physical media should use this prototype:
 * void my_writer_function(char data)
 */
void shell_writer(char data)
{
    return; //阻止不干净的输出
    Serial.write(data);
    SerialBT.write(data);
}

int command_test(int argc, char** argv)
{
    int i;

    shell_println("-----------------------------------------------");
    shell_println("SHELL DEBUG / TEST UTILITY");
    shell_println("-----------------------------------------------");
    shell_println("");
    shell_printf("Received %d arguments for test command\r\n", argc);

    // Print each argument with string lenghts
    for (i = 0; i < argc; i++)
    {
        // Print formatted text to terminal
        shell_printf("%d - \"%s\" [len:%d]\r\n", i, argv[i], strlen(argv[i]));
    }

    return SHELL_RET_SUCCESS;
}

