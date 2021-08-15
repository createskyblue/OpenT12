#include "OpenT12.h"

uint32_t EventTimerUpdate = 0;
/*** 
 * @description: 用户有动作时将会触发此函数
 * @param {*}
 * @return {*}
 */
void TimerUpdateEvent(void) {
    //printf("重置时间计时器\n");
    EventTimerUpdate = millis();
    pages_Tip_Display_timer = EventTimerUpdate;
}
/*** 
 * @description: 单击功率爆发技能 事件
 * @param {*}
 * @return {*}
 */
uint32_t BoostTimer = 0;
void BoostButton_EventLoop(void) {
    //单击 进行短时功率爆发
    if (SYSKey == 1) {
        if (!BoostEvent) {
            BoostEvent = true;
            BoostTimer = millis();
        } else BoostEvent = false;
    }
    //功率爆发技能计时器
    if (millis() - BoostTimer > BoostTime * 1000) {
        //技能时间到
        BoostEvent = false;
    }
}
/*** 
 * @description: 计时器事件Loop
 * @param {*}
 * @return {*}
 */
void TimerEventLoop(void) {
    //事件事件距离计时器
    uint32_t TimerEventTimer = millis() - EventTimerUpdate;

    //更新BOOST提温事件
    BoostButton_EventLoop();
    
    //更新休眠和停机时间
    if (ShutdownTime != 0 && TimerEventTimer > Minute2Millis(ShutdownTime)) {
        //系统停机
        SleepEvent = false;
        ShutdownEvent = true;
    }else if (SleepTime != 0 && TimerEventTimer > Minute2Millis(SleepTime)) {
        //进入休眠
        SleepEvent = true;
        ShutdownEvent = false;
    }else{
        SleepEvent = false;
        ShutdownEvent = false;
    }
    //printf("无动作时间%ld，停机计时%ld，休眠计时%ld\n", TimerEventTimer, Minute2Millis(ShutdownTime), Minute2Millis(SleepTime));
    
}

/*** 
 * @description: 更新系统状态码
 * @param {*}
 * @return {*}
 */
void SYS_StateCode_Update(void) {
    //更新状态码
    if (PID_Output > 180) {
        //加热状态
        TempCTRL_Status = TEMP_STATUS_HEAT;
    }else {
        if (TempGap < 10) {
            //温差接近目标值：正常
            TempCTRL_Status = TEMP_STATUS_WORKY;
        }else{
            //进行PID微调：维持
            TempCTRL_Status = TEMP_STATUS_HOLD;
        }
    }

    if (ERROREvent==true) {
        //系统错误
        TempCTRL_Status = TEMP_STATUS_ERROR;
        //强制关闭输出
        PID_Output = 0;
        //暂时消除本次的状态码
        ERROREvent = false;
    }else if (BoostEvent) {
        //快速升温事件
        TempCTRL_Status = TEMP_STATUS_BOOST;
    }
    
    if (ShutdownEvent) {
        //烙铁进入停机模式
        TempCTRL_Status = TEMP_STATUS_OFF;
        //关闭输出
        SetPOWER(0);
        //进入关机事件
        ShutdownEventLoop();
    }else if (SleepEvent) {
        //烙铁进入休眠模式
        TempCTRL_Status = TEMP_STATUS_SLEEP;
        //关闭输出
        PID_Output = 0;
    }

    //欠压警告
    if (UndervoltageAlert!=0 && SYS_Voltage < UndervoltageAlert) UnderVoltageEvent = true;
    else UnderVoltageEvent = false;

}
/*** 
 * @description: 关机事件安全警告画面；重要提示：为了确保您的安全，不建议禁用此功能！
 * @param {*}
 * @return {*}
 */
void ShutdownEventLoop(void) {
    //尝试关闭功率管输出
    SetPOWER(0);

    //屏保以及警示画面
    while(1) {
        Clear();

        DrawIntensiveComputing();

        //文字闪烁
        if ((millis() / 1000) % 2) DrawMsgBox("请关闭电源！");

        Display();
    }
}