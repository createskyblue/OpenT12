/////////////////////////////////////////////////////////////////
#include "OpenT12.h"
/*
自己动手
样样有！
*/

/////////////////////////////////////////////////////////////////
U8G2_SSD1306_128X64_NONAME_F_HW_I2C Disp(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
PID MyPID(&TipTemperature, &PID_Output, &PID_Setpoint, aggKp, aggKi, aggKd, DIRECT);
/////////////////////////////////////////////////////////////////
uint32_t DispFlashTimer = 0;
void ESPRotaryInterrupt();



void setup() {
    //初始化串口
    Serial.begin(115200);
    shellInit();

    //初始化GPIO
    pinMode(BEEP_PIN, OUTPUT);
    
    //初始化烙铁头
    TipControlInit();

    //初始化OLED
    Disp.begin();
    //Disp.setBusClock(200000);
    Disp.enableUTF8Print();
    Disp.setFontDirection(0);
    Disp.setFontPosTop();
    Disp.setFont(u8g2_font_wqy12_t_gb2312);
    Disp.setDrawColor(1);
    Disp.setFontMode(1);

    EnterLogo();

    //初始化编码器
    sys_RotaryInit();


    //初始化中断
    //ESPRotaryLoop.attach_ms(10, ESPRotaryInterrupt);

    Next_Menu();
    //CalibrationTemperature();
}

char buffer[50];
void loop() {
    System_UI();
    TemperatureControlLoop();
}









