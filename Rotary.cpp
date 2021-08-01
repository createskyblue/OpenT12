#include "Arduino.h"
#include "Rotary.h"

static double Count=0;
static double Count_min=0;
static double Count_max=0;
static double Count_step=0;

//菜单系统角标计时器
extern int32_t pages_Tip_Display_timer;

void sys_RotaryInit(void) {
    //初始化GPIO
    pinMode(ROTARY_PIN1, INPUT_PULLUP);
    pinMode(ROTARY_PIN2, INPUT_PULLUP);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    //初始化中断
    attachInterrupt(ROTARY_PIN1, sys_Counter_IRQHandler, CHANGE);

    //初始化编码器设置
    sys_Counter_Set(-1.2, 6.6, 0.1, 3.14);
}

/**
 * @name  sys_Counter_Set
 * @brief 设置计数器参数
 * @param {double} c       计数器初始值
 * @param {double} min     计数器最小值
 * @param {double} max     计数器最大值
 * @param {double} step    计数器增量步进
 * @return {*}
 */
void sys_Counter_Set(double min, double max, double step, double c) {
    
    Count_min = min * ROTARY_TYPE;
    Count_max = max * ROTARY_TYPE;
    Count_step = step;
    Count = constrain(c, Count_min, Count_max) * ROTARY_TYPE;
}

void sys_Counter_SetVal(double c) {
    Count = constrain(c, Count_min, Count_max) * ROTARY_TYPE;
}

void ICACHE_RAM_ATTR sys_Counter_IRQHandler(void) {
    //重置菜单系统角标计时器
    pages_Tip_Display_timer = millis();

    static volatile uint8_t a0, b0;
    static volatile uint8_t ab0;
    uint8_t a = digitalRead(ROTARY_PIN1);
    uint8_t b = digitalRead(ROTARY_PIN2);
    if (a != a0) {
        a0 = a;
        if (b != b0) {
            b0 = b;
            Count = constrain(Count + ((a == b) ? Count_step : -Count_step), Count_min, Count_max);
            if ((a == b) != ab0) {
                Count = constrain(Count + ((a == b) ? Count_step : -Count_step), Count_min, Count_max);
            }
            ab0 = (a == b);
        }
    }
    //Serial.println("OK");
}

double sys_Counter_Get(void) {
    return Count / ROTARY_TYPE;
}

//编码器按键按下 + 软件滤波
bool sys_KeyProcess(void) {
    if (!digitalRead(BUTTON_PIN)) {
        delay(50);
        if(!digitalRead(BUTTON_PIN)) {
            //重置菜单系统角标计时器
            pages_Tip_Display_timer = millis();
            return 1;
        }
    }
    return 0;
}