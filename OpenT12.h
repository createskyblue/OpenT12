#ifndef OPENT12_H
#define OPENT12_H
#include <math.h>
#include <Arduino.h>
#include <Shell.h>
#include <PID_v1.h>
#include <U8g2lib.h>
#include <Ticker.h>
#include <OneButton.h>
#include "BluetoothSerial.h"
#include "Type.h"
#include "serial_putc.h"
#include "MyShell.h"
#include "Rotary.h"
#include "MathFun.h"
#include "ExternDraw.h"
#include "Event.h"
#include "Menu.h"
#include "Calibration.h"
#include "TipControl.h"
#include "Beep.h"
#include "Bezier.h"
#include "TipManager.h"
#include "FilesSystem.h"

//注意Pin36与Pin39连接了内部霍尔传感器,不建议复用高敏感用途
//另外，建议给ADC输入串联一个0.1uF的电容，有利于减少噪声
#define BEEP_PIN      25
#define BEEP_PIN2     18

#define TIP_ADC_PIN   34
#define POWER_ADC_PIN 35
#define PWM_PIN       23
#define SW_PIN        18

//计算主电源电压
#define POWER_ADC_VCC_R1 47000
#define POWER_ADC_R2_GND 4700

#define CNSize 12
#define SCREEN_COLUMN 128
#define SCREEN_ROW    64
#define SCREEN_PAGE_NUM 8
#define SCREEN_FONT_ROW 4

//温度限制
#define TipMaxTemp 450
#define TipMinTemp 150

enum SYSLANG {
    LANG_English = 0,
    LANG_Chinese,
    LANG_Russian,
};

enum PANELSET {
    PANELSET_Simple = 0,
    PANELSET_Detailed,
};

enum HANDLETRIGGER {
    HANDLETRIGGER_VibrationSwitch = 0,
    HANDLETRIGGER_ReedSwitch,
};

enum MESSAGETYPE {
    MSG_INFO = 0,
    MSG_OK,
    MSG_FAILED,
    MSG_WARNING,
    MSG_ERROR,
};

void ShowLog(MESSAGETYPE type, char* s);

extern const char* SYS_SVAE_PATH;
extern uint64_t ChipMAC;
extern char ChipMAC_S[19];
extern char CompileTime[20];

extern BluetoothSerial SerialBT;
extern OneButton RButton;
extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C Disp;
extern PID MyPID;
extern KFP KFP_Temp;

extern uint8_t DEBUG_MODE;

extern char* TipName;

extern float BootTemp;
extern float SleepTemp;
extern float BoostTemp;

extern float ShutdownTime;
extern float SleepTime;
extern float BoostTime;

extern bool TipInstallEvent;

extern bool ERROREvent;
extern bool ShutdownEvent;
extern bool SleepEvent;
extern bool BoostEvent;
extern bool UnderVoltageEvent;

extern uint8_t PIDMode;

extern uint8_t PanelSettings;
extern uint8_t ScreenFlip;
extern uint8_t SmoothAnimation_Flag;
extern float   ScreenBrightness;
extern uint8_t OptionStripFixedLength_Flag;

extern uint8_t Volume;
extern uint8_t RotaryDirection;
extern uint8_t HandleTrigger;

extern double SYS_Voltage;
extern float  UndervoltageAlert;
extern char BootPasswd[20];
extern uint8_t Language;

extern double Get_MainPowerVoltage(void);

enum TEMP_CTRL_STATUS_CODE {
    TEMP_STATUS_ERROR = 0,
    TEMP_STATUS_OFF,
    TEMP_STATUS_SLEEP,
    TEMP_STATUS_BOOST,
    TEMP_STATUS_WORKY,
    TEMP_STATUS_HEAT,
    TEMP_STATUS_HOLD,
};
extern uint8_t TempCTRL_Status;
extern char* TempCTRL_Status_Mes[];

const unsigned char Logo[]                  = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0xf7,0x7a,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x12,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xff,0xfc,0x00,0x7f,0xff,0x81,0xfe,0x12,0x43,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x00,0x1d,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xff,0xfc,0x7c,0x7f,0xff,0x01,0xfe,0x0f,0x83,0xff,0xf8,0x00,0x00,0x00,0x00,0x03,0xff,0xfc,0x7c,0x7f,0xff,0x01,0xff,0x00,0x07,0xff,0xf8,0x00,0x00,0x00,0x00,0x00,0x3e,0x70,0x7c,0x19,0xf0,0x01,0xff,0x0f,0x87,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x0e,0xf0,0x7c,0x1d,0xc0,0x01,0xff,0x1f,0xc7,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xe0,0x7c,0x0f,0x80,0x00,0x3f,0x1f,0xc7,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xe0,0x7c,0x0f,0x80,0x00,0x1f,0x1b,0x47,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xe0,0x7c,0x0f,0x80,0x00,0x1f,0x1b,0x47,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xe0,0x7c,0x0f,0x80,0x00,0x1f,0x1b,0x47,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xe0,0x7c,0x0f,0x80,0x00,0x1f,0x1f,0xc7,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xf0,0x7c,0x0f,0x80,0x00,0x3f,0x1f,0xc7,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xf8,0x44,0x1f,0x80,0x00,0x7f,0x1f,0xc7,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xff,0xff,0xff,0x00,0x00,0xfe,0x1f,0xe3,0xf8,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xff,0xc7,0xff,0x00,0x01,0xfc,0x1f,0xf1,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xc7,0xfe,0x00,0x03,0xf8,0x3f,0xf8,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0xc7,0xfc,0x00,0x07,0xf0,0x7f,0xfc,0x7f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xc7,0xf0,0x00,0x0f,0xe0,0xff,0xfe,0x3f,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x1f,0xc1,0xff,0xff,0x1f,0xc0,0x00,0x00,0x00,0x00,0x00,0x07,0xff,0xd7,0xff,0xc0,0x3f,0x83,0xff,0x7f,0x0f,0xc0,0x00,0x00,0x00,0x00,0x00,0x0f,0xf8,0x00,0x3f,0xe0,0x3f,0x07,0xfe,0x7f,0x07,0xc0,0x00,0x00,0x00,0x00,0x00,0x0f,0xfc,0x10,0x7f,0xe0,0x3e,0x0f,0xfc,0x7f,0x03,0xc0,0x00,0x00,0x00,0x00,0x00,0x07,0xff,0x39,0xff,0xc0,0x3c,0x1f,0xf8,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xab,0x00,0x00,0x00,0x1f,0xf1,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xab,0x80,0x00,0x00,0x3f,0xe3,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0xbb,0xe0,0x00,0x00,0x3f,0xc7,0xff,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0xd7,0xf0,0x00,0x00,0x7f,0x8f,0xff,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0xd7,0xf8,0x00,0x00,0x7f,0x1f,0xff,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7f,0xc7,0xf8,0x00,0x00,0xfe,0x3e,0x3f,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0x45,0xfc,0x00,0x00,0xfc,0x7c,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfc,0x44,0xfc,0x00,0x01,0xf8,0xf8,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xf8,0x44,0x7e,0x00,0x01,0xf1,0xf8,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xf8,0x44,0x3e,0x00,0x03,0xe3,0xff,0xff,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xf0,0x44,0x3e,0x00,0x03,0xc3,0xff,0xff,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xf0,0x44,0x3e,0x00,0x07,0x83,0xff,0xff,0xf8,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xe0,0x44,0x1f,0x00,0x07,0x03,0xc0,0x7f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xe0,0x44,0x1f,0x00,0x0e,0x07,0xc0,0x7f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xc0,0x44,0x1f,0x00,0x0c,0x07,0xc0,0x7f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xc0,0x44,0x1f,0x00,0x00,0x07,0xff,0xff,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xc0,0x44,0x07,0x80,0x00,0x07,0xff,0xff,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x80,0x44,0x07,0x80,0x00,0x07,0xff,0xff,0xf8,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x80,0x44,0x07,0x80,0x00,0x07,0xc0,0xff,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x80,0x44,0x07,0x80,0x00,0x07,0xc0,0xff,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x80,0x44,0x07,0x80,0x00,0x07,0xc0,0xff,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x00,0x4c,0x03,0x80,0x00,0x07,0xff,0xff,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x00,0x4c,0x03,0x80,0x00,0x07,0xff,0xff,0xff,0xf0,0x00,0x00,0x00,0x00,0x00,0x07,0x00,0x4c,0x03,0x80,0x00,0x07,0xff,0xff,0xff,0x80,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x58,0x01,0x80,0x00,0x07,0xc3,0xff,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x58,0x01,0x80,0x00,0x07,0xc0,0x7f,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x78,0x01,0x80,0x00,0x07,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x70,0x00,0x80,0x00,0x07,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x60,0x00,0x80,0x00,0x07,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x60,0x00,0x80,0x00,0x03,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x60,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x80,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x80,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
const unsigned char TranAnimation[]         = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x90,0xaa,0xfd,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xa9,0x56,0x7d,0x6f,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x90,0xa8,0xf6,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x21,0xa5,0xaa,0xf6,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x84,0x48,0xdd,0xdd,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x54,0xfd,0xfb,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x40,0x6a,0x7f,0xf7,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x40,0xad,0x6a,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x10,0x08,0x14,0x51,0xdf,0xfd,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0xa9,0xfd,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xa4,0xd4,0xfa,0xfe,0xbf,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x91,0xaa,0x6b,0xef,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x2a,0xaa,0x76,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xa8,0xd0,0x6a,0xdb,0xdf,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x80,0x2a,0x52,0xaf,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x40,0xb5,0xfa,0xfb,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x04,0x44,0xaa,0xf6,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x20,0x59,0xf5,0xbd,0xef,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x54,0xed,0xf6,0xbf,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x90,0x2a,0x55,0xdb,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x02,0x50,0xd5,0xb6,0xbd,0xfb,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0xca,0xfa,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x48,0x12,0x21,0xa5,0x56,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x88,0xd4,0xaa,0xfe,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x41,0x20,0x6a,0xab,0xfd,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x84,0x90,0x54,0x57,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x54,0xb5,0xef,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x48,0x54,0xf5,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xa4,0x92,0xb5,0xdf,0xf6,0xfd,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0xa0,0x5a,0xb7,0xfb,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x55,0xbf,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xaa,0xaa,0xea,0xea,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x44,0x55,0x7f,0xbb,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x48,0xd0,0xee,0xfe,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x10,0x52,0xab,0xdf,0xfb,0xf7,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x45,0x54,0xf5,0xf6,0xfe,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x41,0x90,0xa2,0x5a,0xbf,0xbf,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x94,0xaa,0xd5,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x20,0x55,0xff,0xef,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0xa9,0xda,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x90,0x52,0xaa,0xfa,0xfd,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x50,0x25,0xd5,0xfd,0xdf,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x50,0xaa,0xda,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0xa5,0x54,0xfb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xaa,0x54,0xfb,0xde,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x92,0xfa,0xff,0xfe,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xa9,0xa4,0x56,0xf5,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x92,0x80,0x6a,0xed,0xff,0xf7,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x2a,0xd5,0xbb,0xbd,0xfd,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x82,0x54,0xd5,0xbe,0xfe,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x44,0xd5,0xfe,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x48,0x55,0xbb,0xff,0xdf,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x22,0xa8,0x6a,0xf5,0xf7,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x20,0xa9,0xbe,0xde,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x21,0x55,0xed,0x7f,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x48,0x52,0x5b,0xed,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xa4,0x52,0xed,0xfe,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x44,0x55,0x6b,0xff,0xfb,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x10,0x90,0xd4,0xd5,0xfe,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x24,0xa2,0xb6,0xbf,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x81,0x48,0x55,0xf5,0xfe,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x49,0x55,0xdd,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xa0,0xa2,0xaa,0xfd,0xff,0xfd,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x48,0x0a,0x55,0xf7,0xef,0xf7,0xff,0xff };
const unsigned char QRC[]                   = { 14, 0x05, 0x80, 0x74, 0xb8, 0x57, 0xa8, 0x76, 0xb8, 0x05, 0x80, 0xf9, 0x7c, 0x46, 0x94, 0xaa, 0xa8, 0xf9, 0x7c, 0x06, 0x88, 0x74, 0xa8, 0x57, 0x8c, 0x75, 0x74, 0x06, 0x98 };
const unsigned char Set0[]                  = { 14, 0x88, 0x24, 0x08, 0x20, 0x38, 0x30, 0x38, 0x30, 0x38, 0x30, 0x38, 0x30, 0x38, 0x30, 0x38, 0x30, 0x39, 0x30, 0x3b, 0xb0, 0x3f, 0xf0, 0x3f, 0xf0, 0x00, 0x00, 0x80, 0x04 };
const unsigned char Set1[]                  = { 14, 0xf8, 0x7c, 0xf3, 0x3c, 0xf4, 0xbc, 0xf6, 0xbc, 0xf4, 0xbc, 0xf6, 0xbc, 0xf4, 0xbc, 0xe4, 0x9c, 0xc8, 0x4c, 0xd0, 0x2c, 0xd0, 0x2c, 0xc8, 0x4c, 0xe7, 0x9c, 0xf0, 0x3c };
const unsigned char Set2[]                  = { 14, 0x8f, 0xc4, 0x10, 0x20, 0x20, 0x10, 0x4c, 0xc8, 0x9f, 0xe4, 0x3d, 0xf0, 0x3d, 0xf0, 0x1d, 0xe0, 0x3e, 0xf0, 0x3f, 0x70, 0x9f, 0xe4, 0x8c, 0xc4, 0x00, 0x00, 0x30, 0x30 };
const unsigned char Set3[]                  = { 14, 0xc3, 0xfc, 0x18, 0x00, 0x18, 0x00, 0xc3, 0xfc, 0xff, 0xfc, 0xff, 0x0c, 0x00, 0x60, 0x00, 0x60, 0xff, 0x0c, 0xff, 0xfc, 0xf0, 0xfc, 0x06, 0x00, 0x06, 0x00, 0xf0, 0xfc };
const unsigned char Set4[]                  = { 14, 0x80, 0x04, 0x00, 0x00, 0x20, 0x00, 0x10, 0x00, 0x08, 0x00, 0x10, 0x00, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x04 };
const unsigned char Set4F[]                 = { 14, 0x80, 0x04, 0x00, 0x00, 0x3f, 0xf0, 0x38, 0x30, 0x33, 0x10, 0x3f, 0x90, 0x38, 0x10, 0x33, 0x90, 0x27, 0x90, 0x26, 0x10, 0x30, 0x90, 0x3f, 0xf0, 0x00, 0x00, 0x80, 0x04 };
const unsigned char Set4FF[]                = { 14, 0x80, 0x04, 0x00, 0x00, 0x3f, 0xf0, 0x24, 0x30, 0x21, 0x90, 0x27, 0x90, 0x27, 0x30, 0x20, 0x70, 0x27, 0xf0, 0x23, 0x30, 0x30, 0x70, 0x3f, 0xf0, 0x00, 0x00, 0x80, 0x04 };
const unsigned char Set5[]                  = { 14, 0xfc, 0xfc, 0xf8, 0x7c, 0xe0, 0x1c, 0xc0, 0x0c, 0xc0, 0x0c, 0xc0, 0x0c, 0xc0, 0x0c, 0xc0, 0x0c, 0xc0, 0x0c, 0x80, 0x04, 0x80, 0x04, 0xff, 0xfc, 0xf8, 0x7c, 0xfc, 0xfc };
const unsigned char Set5_1[]                = { 14, 0xfc, 0xf4, 0xf8, 0x60, 0xe0, 0x44, 0xc0, 0x8c, 0xc1, 0x1c, 0xc2, 0x2c, 0xc4, 0x4c, 0xc8, 0x8c, 0xd1, 0x0c, 0xa2, 0x04, 0xc4, 0x04, 0x8f, 0xfc, 0x18, 0x7c, 0xbc, 0xfc };
const unsigned char Set6[]                  = { 14, 0xf8,0x7c,0xc0,0x0c,0xc0,0x0c,0xcf,0xcc,0xcf,0xcc,0xcf,0xcc,0xcf,0xcc,0xcf,0xcc,0xcf,0xcc,0xc8,0x4c,0xc8,0x4c,0xcf,0xcc,0xc0,0x0c,0xc0,0x0c };
const unsigned char Set_LANG[]              = { 14, 0xf0, 0x3c, 0xce, 0x4c, 0xbc, 0x34, 0xb8, 0x14, 0x18, 0x80, 0x10, 0x80, 0x42, 0x30, 0x60, 0x78, 0x60, 0xf8, 0x44, 0xc0, 0x80, 0x04, 0xb8, 0x84, 0xcf, 0xcc, 0xf0, 0x3c };
const unsigned char Set7[]                  = { 14, 0x80, 0x0c, 0x00, 0x1c, 0x3f, 0xf4, 0x3f, 0xe0, 0x3f, 0xc4, 0x37, 0x8c, 0x23, 0x18, 0x30, 0x30, 0x38, 0x70, 0x3c, 0xf0, 0x3f, 0xf0, 0x3f, 0xf0, 0x00, 0x00, 0x80, 0x04 };
const unsigned char Set11[]                 = { 14, 0xf3, 0xfc, 0xe7, 0xfc, 0xce, 0x0c, 0x8e, 0x1c, 0x9f, 0x9c, 0x1f, 0x3c, 0x1f, 0x0c, 0x0e, 0x0c, 0x0f, 0xfc, 0x87, 0xf8, 0x81, 0xe4, 0xc0, 0x0c, 0xe0, 0x1c, 0xf8, 0x7c };
const unsigned char Set8[]                  = { 14, 0x81, 0xdc, 0x55, 0x9c, 0x29, 0x04, 0x55, 0x00, 0x29, 0x90, 0x55, 0xd0, 0x03, 0xfc, 0xff, 0x00, 0x2e, 0x00, 0x26, 0x70, 0x02, 0x50, 0x82, 0x70, 0xe6, 0x00, 0xee, 0x04 };
const unsigned char Set9[]                  = { 14, 0xf0, 0x3c, 0xce, 0x0c, 0xbf, 0x04, 0xb3, 0x04, 0x73, 0x00, 0x7f, 0x00, 0x7e, 0x00, 0x7c, 0x00, 0x7c, 0x00, 0x7c, 0x60, 0xbc, 0x64, 0xbe, 0x04, 0xcf, 0x0c, 0xf0, 0x3c };
const unsigned char Set10[]                 = { 14, 0xf8, 0x7c, 0xf7, 0xbc, 0x00, 0x00, 0x7f, 0xf8, 0x00, 0x00, 0xbf, 0xf4, 0xab, 0x54, 0xab, 0x54, 0xab, 0x54, 0xab, 0x54, 0xab, 0x54, 0xab, 0x54, 0xbf, 0xf4, 0xc0, 0x0c };
const unsigned char Set13[]                 = { 14, 0x80, 0x04, 0x00, 0x00, 0x3f, 0xf0, 0x3c, 0xf0, 0x34, 0xb0, 0x24, 0x90, 0x24, 0x90, 0x27, 0x90, 0x23, 0x10, 0x30, 0x30, 0x38, 0x70, 0x3f, 0xf0, 0x00, 0x00, 0x80, 0x04 };
const unsigned char Set12[]                 = { 14, 0x80, 0x04, 0x00, 0x00, 0x3f, 0xf0, 0x3c, 0xf0, 0x3c, 0xf0, 0x3c, 0xf0, 0x20, 0x10, 0x20, 0x10, 0x3c, 0xf0, 0x3c, 0xf0, 0x3c, 0xf0, 0x3f, 0xf0, 0x00, 0x00, 0x80, 0x04 };
const unsigned char Set14[]                 = { 14, 0xff, 0x7c, 0xfe, 0x7c, 0xfc, 0xec, 0xe8, 0xdc, 0xc8, 0x4c, 0x98, 0x4c, 0x88, 0x24, 0x08, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x04, 0xe0, 0x1c };
const unsigned char Set15[]                 = { 14, 0x15, 0x54, 0x3f, 0xfc, 0x15, 0x54, 0x3f, 0xfc, 0x15, 0x54, 0x3c, 0x04, 0x14, 0x04, 0x3c, 0xfc, 0x14, 0x54, 0x3c, 0xfc, 0x00, 0x54, 0x00, 0xa8, 0x00, 0x00, 0x00, 0x00 };
const unsigned char Set16[]                 = { 14, 0x15, 0x54, 0x3f, 0xf8, 0x15, 0x50, 0x3f, 0xf0, 0x15, 0x50, 0x38, 0x70, 0x10, 0x20, 0x31, 0x04, 0x13, 0x8c, 0x22, 0xa8, 0x17, 0xfc, 0x2a, 0xa8, 0x00, 0x00, 0x00, 0x00 };
const unsigned char Set17[]                 = { 14, 0x80, 0x00, 0xbf, 0xf8, 0x1f, 0xf8, 0xb0, 0x08, 0x18, 0x08, 0xb9, 0xe8, 0x19, 0xb8, 0xb8, 0x38, 0x18, 0x38, 0xb9, 0xb8, 0xb9, 0xf8, 0xb0, 0xf8, 0xbf, 0xf8, 0x80, 0x00 };
const unsigned char Set18[]                 = { 14, 0x80, 0x00, 0xbf, 0xf8, 0x1f, 0xf8, 0xb0, 0x28, 0x1f, 0xf8, 0xb4, 0x08, 0x1f, 0xf8, 0xb1, 0x08, 0x1f, 0xf8, 0xb0, 0x48, 0xbf, 0xf8, 0xb2, 0x88, 0xbf, 0xf8, 0x80, 0x00 };
const unsigned char Lang_CN[]               = { 14, 0x80, 0x04, 0x00, 0x00, 0x3f, 0xf0, 0x3c, 0xf0, 0x3c, 0xf0, 0x20, 0x10, 0x24, 0x90, 0x24, 0x90, 0x20, 0x10, 0x3c, 0xf0, 0x3c, 0xf0, 0x3f, 0xf0, 0x00, 0x00, 0x80, 0x04 };
const unsigned char Lang_EN[]               = { 14, 0x80, 0x04, 0x00, 0x00, 0x3f, 0xf0, 0x38, 0x70, 0x30, 0x30, 0x23, 0x10, 0x27, 0x90, 0x20, 0x10, 0x20, 0x10, 0x27, 0x90, 0x27, 0x90, 0x3f, 0xf0, 0x00, 0x00, 0x80, 0x04 };
const unsigned char Lang_JP[]               = { 14, 0x80, 0x04, 0x00, 0x00, 0x3f, 0xf0, 0x3d, 0xf0, 0x20, 0x10, 0x3b, 0xf0, 0x3b, 0x70, 0x30, 0x30, 0x2b, 0x50, 0x2a, 0xd0, 0x31, 0xb0, 0x3f, 0xf0, 0x00, 0x00, 0x80, 0x04 };
const unsigned char Save[]                  = { 14, 0x00, 0x04, 0x58, 0x08, 0x58, 0x08, 0x58, 0x08, 0x40, 0x08, 0x7f, 0xf8, 0x60, 0x18, 0x5f, 0xe8, 0x5b, 0x68, 0x5f, 0xe8, 0x5b, 0x68, 0x1c, 0xe8, 0x5f, 0xe8, 0x00, 0x00 };
const unsigned char Lock[]                  = { 14, 0xe0, 0x1c, 0xc0, 0x0c, 0x8f, 0xc4, 0x9f, 0xe4, 0x9f, 0xe4, 0x9f, 0xe4, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x07, 0x80, 0x07, 0x80, 0x03, 0x00, 0x80, 0x04 };
const unsigned char Set19[]                 = { 14, 0xf8, 0x7c, 0x80, 0x04, 0xbc, 0xf4, 0x3f, 0xf0, 0x38, 0x70, 0xb7, 0xb4, 0xb6, 0xb4, 0xb5, 0xb4, 0xb7, 0xb4, 0x38, 0x70, 0x3f, 0xf0, 0xbc, 0xf4, 0x80, 0x04, 0xf8, 0x7c };
const unsigned char IMG_Pen[]               = { 14, 0xf0,0x7c,0xe0,0x3c,0xea,0xbc,0xea,0xbc,0xea,0xbc,0xea,0xbc,0xea,0xbc,0xea,0xbc,0xea,0xbc,0xe0,0x3c,0xef,0xbc,0xf7,0x7c,0xf8,0xfc,0xfd,0xfc };
const unsigned char IMG_Pen2[]              = { 14, 0xe0,0x3c,0xf0,0x7c,0xf0,0x7c,0xf7,0x7c,0xf7,0x7c,0xef,0xbc,0xed,0xbc,0xef,0xbc,0xf5,0x7c,0xf5,0x7c,0xfa,0xfc,0xfa,0xfc,0xfd,0xfc,0xfd,0xfc };
const unsigned char IMG_Tip[]               = { 14, 0xf3,0xfc,0xf1,0xfc,0xf4,0xfc,0xf6,0x7c,0xf7,0x3c,0xf7,0xbc,0xf7,0xbc,0xf7,0xbc,0xf7,0xbc,0xf7,0xbc,0xe7,0xbc,0xe0,0x5c,0xe5,0xdc,0xe5,0x5c };
const unsigned char IMG_Files[]             = { 14, 0x00,0xfc,0x7e,0x7c,0x7e,0x3c,0x70,0x0c,0x47,0xe4,0x77,0xe0,0x47,0xf8,0x74,0x08,0x47,0xf8,0x74,0x08,0x07,0xf8,0xf4,0x08,0xf7,0xf8,0xf0,0x00 };
const unsigned char IMG_Flip[]              = { 14, 0x01,0x30,0x7d,0x38,0xbd,0x1c,0xbd,0x1c,0xbd,0x1c,0xdd,0x0c,0xdd,0x0c,0xdd,0x0c,0xed,0x04,0xed,0x04,0xf5,0x04,0xf5,0x00,0x75,0x00,0x39,0x00 };
const unsigned char IMG_Sun[]               = { 14, 0xbc,0xf4,0x1c,0xe0,0x8f,0xc4,0xd8,0x6c,0xf0,0x3c,0xe3,0x1c,0x27,0x90,0x27,0x90,0xe3,0x1c,0xf0,0x3c,0xd8,0x6c,0x8f,0xc4,0x1c,0xe0,0xbc,0xf4 };
const unsigned char IMG_Size[]              = { 14, 0x07,0xfc,0x1f,0xfc,0x0f,0xfc,0x47,0xfc,0x63,0xfc,0xf1,0xfc,0xf8,0xfc,0xfc,0x7c,0xfe,0x3c,0xff,0x18,0xff,0x88,0xff,0xc0,0xff,0xe0,0xff,0x80 };
const unsigned char IMG_Animation[]         = { 14, 0xe0,0xfc,0xdf,0x7c,0xbf,0xbc,0x63,0xdc,0x67,0xdc,0x6b,0xfc,0x7d,0x0c,0x7e,0xf4,0xbd,0x78,0xdd,0xb8,0xe5,0xd8,0xfd,0xf8,0xfe,0xf4,0xff,0x0c };
const unsigned char IMG_Animation_DISABLE[] = { 14, 0xe0,0xf4,0xdf,0x60,0xbf,0xc4,0x63,0x8c,0x67,0x1c,0x6a,0x3c,0x7c,0x4c,0x78,0xf4,0xb1,0x78,0xe3,0xb8,0xc5,0xd8,0x8d,0xf8,0x1e,0xf4,0xbf,0x0c };
const unsigned char IMG_Trigger[]           = { 14, 0x8f,0xc4,0x10,0x20,0x2c,0xd0,0x4f,0xc8,0x9d,0xe4,0x3d,0xf0,0x1d,0xe0,0x3e,0xf0,0x3f,0x70,0x9f,0xe4,0x4f,0xc8,0x24,0xd0,0x10,0x20,0x8f,0xc4 };
const unsigned char IMG_VibrationSwitch[]   = { 21, 0xfe,0x01,0xf8,0xfd,0xfe,0xf8,0xfb,0x83,0x78,0xfb,0x7b,0x78,0xfb,0x07,0x78,0xfb,0x7b,0x78,0xfb,0x83,0x78,0xfb,0x7b,0x78,0xfb,0x07,0x78,0xfb,0x7b,0x78,0xfb,0x83,0x78,0xfb,0x7b,0x78,0xfb,0x07,0x78,0xfb,0x7b,0x78,0xfa,0x01,0x78,0xfb,0x7b,0x78,0xfa,0x01,0x78,0xf8,0x00,0x78,0xfa,0x01,0x78,0xff,0x7b,0xf8,0xff,0x7b,0xf8 };
const unsigned char IMG_ReedSwitch[]        = { 21, 0xff,0xff,0xf8,0xff,0xff,0xf8,0xff,0xff,0xf8,0xff,0xff,0xf8,0xff,0xff,0xf8,0xc3,0xfe,0x18,0x80,0x00,0x08,0x87,0xff,0x08,0x0f,0xff,0x80,0x1c,0x1f,0xc0,0x01,0xfc,0x00,0x1f,0xc1,0xc0,0x0f,0xff,0x80,0x07,0xff,0x08,0x80,0x00,0x08,0xc3,0xfe,0x18,0xff,0xff,0xf8,0xff,0xff,0xf8,0xff,0xff,0xf8,0xff,0xff,0xf8,0xff,0xff,0xf8 };

const unsigned char PositioningCursor[] PROGMEM = { 0x18,0x3c,0x7e,0xff,0xff,0x7e,0x3c,0x18 };
const unsigned char Pointer[] PROGMEM = { 0x20, 0x20, 0x70, 0xf8 };
const unsigned char Lightning[] PROGMEM = { 0x0f, 0xe0, 0x1f, 0xc0, 0x1f, 0xc0, 0x3f, 0x80, 0x3f, 0xf8, 0x7f, 0xf0, 0x7f, 0xe0, 0x07, 0xc0, 0x07, 0x80, 0x0f, 0x00, 0x0e, 0x00, 0x1c, 0x00, 0x18, 0x00, 0x10, 0x00 };
const unsigned char Battery_NoPower[] PROGMEM = { 0x07,0x80,0x3f,0xf0,0x3f,0xf0,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x37,0xb0,0x37,0xb0,0x30,0x30,0x3f,0xf0,0x3f,0xf0 };

//状态提示图标
//width:14,height:14
const unsigned char c1[] PROGMEM = { 0x03, 0x00, 0x07, 0x80, 0x0f, 0xc0, 0x0c, 0xc0, 0x1c, 0xe0, 0x1c, 0xe0, 0x3c, 0xf0, 0x3c, 0xf0, 0x7f, 0xf8, 0x7f, 0xf8, 0xfc, 0xfc, 0xfc, 0xfc, 0xff, 0xfc, 0x7f, 0xf8 };
const unsigned char c2[] PROGMEM = { 0x7f, 0xf8, 0xff, 0xfc, 0xc0, 0x0c, 0xc3, 0x0c, 0xcb, 0x4c, 0xdb, 0x6c, 0xdb, 0x6c, 0xd8, 0x6c, 0xdc, 0xec, 0xcf, 0xcc, 0xc7, 0x8c, 0xc0, 0x0c, 0xff, 0xfc, 0x7f, 0xf8 };
const unsigned char c3[] PROGMEM = { 0x00, 0x00, 0x06, 0x00, 0x0c, 0x00, 0x18, 0xf8, 0x38, 0xf0, 0x30, 0x30, 0x70, 0x60, 0x70, 0x78, 0x78, 0xf8, 0x78, 0x00, 0x3c, 0x02, 0x3f, 0x0c, 0x1f, 0xf8, 0x0f, 0xf0, 0x03, 0xc0, 0x00, 0x00 };
const unsigned char c5[] PROGMEM = { 0x7f, 0xf0, 0xff, 0xe0, 0xc0, 0x08, 0xc0, 0x1c, 0xc0, 0x38, 0xc8, 0x70, 0xdc, 0xe4, 0xcf, 0xcc, 0xc7, 0x8c, 0xc3, 0x0c, 0xc0, 0x0c, 0xc0, 0x0c, 0xff, 0xfc, 0x7f, 0xf8 };
const unsigned char c6[] PROGMEM = { 0x1e, 0x10, 0x33, 0x38, 0x2d, 0x7c, 0x25, 0x38, 0x2d, 0x38, 0x25, 0x38, 0x2d, 0x38, 0x6d, 0x80, 0xde, 0xc0, 0xbf, 0x40, 0xbf, 0x40, 0xde, 0xc0, 0x61, 0x80, 0x3f, 0x00 };
const unsigned char c7[] PROGMEM = { 0x1f, 0xe0, 0x3f, 0xf0, 0x70, 0x38, 0x60, 0x18, 0x60, 0x18, 0x60, 0x18, 0xff, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xf8, 0x7c, 0xf8, 0x7c, 0xfc, 0xfc, 0x7f, 0xf8 };
extern const unsigned char* C_table[];

#endif