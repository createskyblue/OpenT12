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
int BootTemp  = 320;
int SleepTemp = 250;
int BoostTemp = 50;

int ShutdownTimer = 10;
int SleepTimer    = 5;
int BoostTimer    = 30;

uint8_t DEBUG_MODE = true;
uint8_t PIDMode = true;

uint8_t PanelSettings = PANELSET_Detailed;
uint8_t ScreenFlip = false;
uint8_t SmoothAnimation_Flag = true;
int     ScreenBrightness = 255;
uint8_t OptionStripFixedLength_Flag = false;

uint8_t Volume = 0;
uint8_t RotaryDirection = false;

int UndervoltageAlert = 3;
int BootPasswd = false;
uint8_t Language = LANG_Chinese;
/////////////////////////////////////////////////////////////////


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

    //显示启动信息
    ShowBootMsg();

    //显示Logo
    EnterLogo();

    //初始化编码器
    sys_RotaryInit();


    System_UI_Init();
}

void loop() {
    shell_task();
    TemperatureControlLoop();
    System_UI();
}









