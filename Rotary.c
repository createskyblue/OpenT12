#include "Rotary.h"

static float count=0;
static float count_min=0;
static float count_max=0;
static float count_step=1;

#define sys_RotaryCoolTime 3
uint32_t sys_RotaryCoolTimeTimer = 0;

/**
 * @name  sys_Counter_Set
 * @brief 设置计数器参数
 * @param {int16_t} c       计数器初始值
 * @param {int16_t} min     计数器最小值
 * @param {int16_t} max     计数器最大值
 * @param {int16_t} step    计数器增量步进
 * @return {*}
 */
void sys_Counter_Set(int16_t min, int16_t max, int16_t step, int16_t c) {
    
    count_min = min;
    count_max = max;
    count_step = step;
    count = constrain(c,count_min,count_max);
}

void sys_Counter_Add(void) {
    if (millis() - sys_RotaryCoolTimeTimer > sys_RotaryCoolTime) {
        count = constrain(count + count_step, count_min, count_max);

        sys_RotaryCoolTimeTimer = millis();
    }
}

void sys_Counter_Less(void) {
    if (millis() - sys_RotaryCoolTimeTimer > sys_RotaryCoolTime) {
        count = constrain(count - count_step, count_min, count_max);

        sys_RotaryCoolTimeTimer = millis();
    }
}

float sys_Counter_Get(void) {
    return count;
}

//extern int32_t pages_Tip_Display_timer; //菜单系统的页码定时器，需要在按键激活的时候进行刷新
uint8_t sys_KeyProcess_Val=0;
uint8_t sys_KeyProcess(void) {
    // sys_KeyProcess_Val = 0;
    // static uint8_t key=0;
    // static uint8_t lastKey=0;
    // key = bsp_GetKey();
    // if (key!=KEY_NONE) {
    //     pages_Tip_Display_timer = bsp_GetRunTime();
    //     if (key == KEY_DOWN_DOWN || key == KEY_LEFT_DOWN)
    //         sys_Counter_Less();
    //     else if (key == KEY_UP_DOWN || key == KEY_RIGHT_DOWN)
    //         sys_Counter_Add();
    //     else if (ButtonJudgment(4, 1, key, lastKey))
    //         sys_KeyProcess_Val = 1;
    //     else if (ButtonJudgment(4, 3, key, lastKey))
    //         sys_KeyProcess_Val=2;
    //     lastKey=key;
    // }
    // return sys_KeyProcess_Val;
    return 1;
}