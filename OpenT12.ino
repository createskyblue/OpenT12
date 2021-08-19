/////////////////////////////////////////////////////////////////
#include "OpenT12.h"
/*
自己动手
样样有！
*/

//重设蓝牙串口缓冲区大小
#define RX_QUEUE_SIZE 2048
#define TX_QUEUE_SIZE 2048

/////////////////////////////////////////////////////////////////
BluetoothSerial SerialBT;
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

OneButton RButton(BUTTON_PIN, true);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C Disp(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
PID MyPID(&TipTemperature, &PID_Output, &PID_Setpoint, aggKp, aggKi, aggKd, DIRECT);
/////////////////////////////////////////////////////////////////
//存档路径
const char* SYS_SVAE_PATH = "/OpenT12.sav";
char* TipName = "文件系统错误：请上报";

float BootTemp  = 300;
float SleepTemp = 250;
float BoostTemp = 50;

float ShutdownTime = 0;
float SleepTime    = 5;
float BoostTime    = 30;

//烙铁头事件
bool TipInstallEvent   = true;
//到温提示音播放完成
bool TempToneFlag = false;
//温控系统状态
bool ERROREvent        = false;
bool ShutdownEvent     = false;
bool SleepEvent        = false;
bool BoostEvent        = false;
bool UnderVoltageEvent = false;
//PWM控制状态
bool PWM_WORKY         = false;

uint8_t DEBUG_MODE = true;

uint8_t PIDMode                     = true;
uint8_t Use_KFP                     = true;
uint8_t PanelSettings               = PANELSET_Detailed;
uint8_t ScreenFlip                  = false;
uint8_t SmoothAnimation_Flag        = true;
float   ScreenBrightness            = 128;
uint8_t OptionStripFixedLength_Flag = false;

uint8_t Volume = true;
uint8_t RotaryDirection = false;
uint8_t HandleTrigger = HANDLETRIGGER_VibrationSwitch;

double  SYS_Voltage = 3.3;
float   UndervoltageAlert = 3;
char    BootPasswd[20] = { 0 };
uint8_t Language = LANG_Chinese;

float ADC_PID_Cycle = 100;

//面板状态条
uint8_t TempCTRL_Status = TEMP_STATUS_OFF;
const unsigned char* C_table[] = { c1, c2, c3, Lightning, c5, c6, c7 };
char* TempCTRL_Status_Mes[]={
    "错误",
    "停机",
    "休眠",
    "提温",
    "正常",
    "加热",
    "维持",
};

//系统信息
uint64_t ChipMAC;
char ChipMAC_S[19] = { 0 };
char CompileTime[20];
//定时器
hw_timer_t* SoundTimer = NULL;
/////////////////////////////////////////////////////////////////

//先初始化硬件->显示LOGO->初始化软件
void setup() {

    ////////////////////////////初始化硬件/////////////////////////////
    //获取系统信息
    ChipMAC = ESP.getEfuseMac();
    sprintf(CompileTime, "%s %s", __DATE__, __TIME__);
    for (uint8_t i = 0;i < 6;i++)  sprintf(ChipMAC_S + i * 3, "%02X%s", ((uint8_t*)&ChipMAC)[i], (i != 5) ? ":" : "");

    //初始化串口
    Serial.begin(921600);
    SerialBT.begin("OpenT12");

    //初始化GPIO
    BeepInit();
    pinMode(POWER_ADC_PIN, INPUT);

    //初始化烙铁头
    TipControlInit();

    //初始化编码器
    sys_RotaryInit();

    //初始化OLED
    Disp.begin();
    Disp.setBusClock(1000000);
    Disp.enableUTF8Print();
    Disp.setFontDirection(0);
    Disp.setFontPosTop();
    Disp.setFont(u8g2_font_wqy12_t_gb2312);
    Disp.setDrawColor(1);
    Disp.setFontMode(1);

    ////////////////////////////初始化软件/////////////////////////////
    //显示启动信息
    ShowBootMsg();

    //显示Logo
    EnterLogo();

    //初始化命令解析器
    shellInit();

    //初始化定时器
    // SoundTimer = timerBegin(0, 240, true);
    // timerAttachInterrupt(SoundTimer, &PlaySoundLoop, true);
    // timerAlarmWrite(SoundTimer, 10000, true);
    // timerAlarmEnable(SoundTimer);

    //启动文件系统，并读取存档
    FilesSystemInit();

    //开机密码
    while (!EnterPasswd()) {
        Pop_Windows("身份验证失败");
    }

    //初始化UI
    System_UI_Init();

    //首次启动的时候根据启动温度配置，重新设定目标温度
    sys_Counter_SetVal(BootTemp);

    //载入烙铁头配置
    LoadTipConfig();

    
    
}

void loop() {
    //获取按键
    sys_KeyProcess();

    if (!Menu_System_State) {
        //温度闭环控制
        TemperatureControlLoop();
        //更新BOOST提温事件
        BoostButton_EventLoop();
        //更新系统事件：：系统事件可能会改变功率输出
        TimerEventLoop();
    }

    
    //更新状态码
    SYS_StateCode_Update();
    //设置输出功率
    SetPOWER(PID_Output);

    //刷新UI
    System_UI();
}
/**
 * @description: 计算主电源电压
 * @param {*}
 * @return 主电源电压估计值
 */
double Get_MainPowerVoltage(void) {
    //uint16_t POWER_ADC = analogRead(POWER_ADC_PIN);
    double TipADC_V_R2 = analogReadMilliVolts(POWER_ADC_PIN) / 1000.0;
    //double   TipADC_V_R2 = ESP32_ADC2Vol(POWER_ADC);
    double   TipADC_V_R1 = (TipADC_V_R2*POWER_ADC_VCC_R1)/POWER_ADC_R2_GND;

    SYS_Voltage = TipADC_V_R1 + TipADC_V_R2;
    return SYS_Voltage;
}

void SYS_Reboot(void) {
    ESP.restart();
}