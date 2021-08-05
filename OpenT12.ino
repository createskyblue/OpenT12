/////////////////////////////////////////////////////////////////
#include "OpenT12.h"
/*
自己动手
样样有！
*/

/////////////////////////////////////////////////////////////////
U8G2_SSD1306_128X64_NONAME_F_HW_I2C Disp(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
//PID MyPID(&Input, &Output, &Setpoint, aggKp, aggKi, aggKd, REVERSE);
/////////////////////////////////////////////////////////////////
uint32_t DispFlashTimer = 0;
void ESPRotaryInterrupt();

void setup() {
    //初始化GPIO
    pinMode(BEEP_PIN, OUTPUT);
    //analogWrite(PWM_PIN, 255);

    //初始化串口
    Serial.begin(115200);

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

}

char buffer[50];
void loop() {
    System_UI();
    if (millis() - DispFlashTimer > 33) {
        //tone(14, 1000);
        uint16_t ADC = GetADC0();

        Disp.clearBuffer();

        sprintf(buffer, "运行时间:%ld", millis());
        Disp.setCursor(0, 1);
        Disp.print(buffer);

        sprintf(buffer, "ADC:%d", ADC);
        Disp.setCursor(0, 13);
        Disp.print(buffer);

        sprintf(buffer, "换算温度:%lf", CalculateTemp(ADC));
        Disp.setCursor(0, 25);
        Disp.print(buffer);

        Disp.sendBuffer();

        DispFlashTimer = millis();
        //noTone(14);//停止发声
    }
}









