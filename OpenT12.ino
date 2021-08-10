/////////////////////////////////////////////////////////////////
#include "OpenT12.h"
/*
自己动手
样样有！
*/

/////////////////////////////////////////////////////////////////
OneButton RButton(BUTTON_PIN, true);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C Disp(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
PID MyPID(&TipTemperature, &PID_Output, &PID_Setpoint, aggKp, aggKi, aggKd, DIRECT);
/////////////////////////////////////////////////////////////////
int BootTemp  = 320;
int SleepTemp = 250;
int BoostTemp = 50;

int ShutdownTime = 10;
int SleepTime    = 5;
int BoostTime    = 30;

bool ERROREvent = false;
bool ShutdownEvent = false;
bool SleepEvent = false;
bool BoostEvent = false;

uint8_t DEBUG_MODE = true;
uint8_t PIDMode = true;

uint8_t PanelSettings = PANELSET_Detailed;
uint8_t ScreenFlip = false;
uint8_t SmoothAnimation_Flag = true;
int     ScreenBrightness = 255;
uint8_t OptionStripFixedLength_Flag = false;

uint8_t Volume = 0;
uint8_t RotaryDirection = false;
uint8_t HandleTrigger = HANDLETRIGGER_VibrationSwitch;

int UndervoltageAlert = 3;
int BootPasswd = false;
uint8_t Language = LANG_Chinese;

//面板状态条
uint8_t TempCTRL_Status = TEMP_STATUS_OFF;
char* TempCTRL_Status_Mes[]={
    "停机",
    "休眠",
    "提温",
    "正常",
    "加热",
    "维持",
    "错误",
};
/////////////////////////////////////////////////////////////////

//先初始化硬件->显示LOGO->初始化软件
void setup() {
    //初始化串口
    Serial.begin(115200);

    //初始化GPIO
    BeepInit();
    pinMode(POWER_ADC_PIN, INPUT);

    //初始化烙铁头
    TipControlInit();

    //初始化编码器
    sys_RotaryInit();

    //初始化OLED
    Disp.begin();
    //Disp.setBusClock(200000);
    Disp.enableUTF8Print();
    Disp.setFontDirection(0);
    Disp.setFontPosTop();
    Disp.setFont(u8g2_font_wqy12_t_gb2312);
    Disp.setDrawColor(1);
    Disp.setFontMode(1);


    SetNote(NOTE_B, 7);
    delay(150);
    SetNote(NOTE_B, 9);
    delay(150);
    SetNote(NOTE_B, 12);
    delay(150);
    SetTone(0);


    

    //显示启动信息
    ShowBootMsg();

    //显示Logo
    EnterLogo();

    //初始化命令解析器
    shellInit();

    //初始化UI
    System_UI_Init();
}

void loop() {
    //命令解析器
    while (Serial.available()) shell_task();
    //温度闭环控制
    TemperatureControlLoop(); 
    //更新系统事件
    TimerEventLoop();
    //更新状态码
    SYS_StateCode_Update();
    //刷新UI
    System_UI();
    //设置输出功率
    SetPOWER(PID_Output);
}
/**
 * @description: 计算主电源电压
 * @param {*}
 * @return 主电源电压估计值
 */
double Get_MainPowerVoltage(void) {
    uint16_t POWER_ADC = analogRead(POWER_ADC_PIN);
    double   TipADC_V_R2 = ESP32_ADC2Vol(POWER_ADC);
    double   TipADC_V_R1 = (TipADC_V_R2*POWER_ADC_VCC_R1)/POWER_ADC_R2_GND;
    return   TipADC_V_R1 + TipADC_V_R2;
}