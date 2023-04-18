#include "OpenT12.h"

//事件计时器
uint32_t EventTimerUpdate = 0;
//屏保事件计时器
static uint32_t ScreenProtectorTimer = 0;
/*** 
 * @description: 用户有动作时将会触发此函数
 * @param {*}
 * @return {*}
 */
void TimerUpdateEvent(void) {
    //printf("重置时间计时器\n");
    EventTimerUpdate = millis();
    ScreenProtectorTimer = millis();
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
 * @description: 双击快速召唤PID设置
 * @param {*}
 * @return {*}
 */
void FastPID_MenuSummon_EventLoop(void){
    //若发现双击，则快速打开PID菜单
    if (SYSKey == 3) System_PIDMenu_Init();
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
    //更新PID菜单快速召唤事件
    FastPID_MenuSummon_EventLoop();

    //刷新手柄触发开关事件：可引发手柄休眠事件
    SW_WakeLOOP();
    
    //更新休眠和停机时间    停机优先级 > 休眠优先级
    if (ShutdownTime != 0 && TimerEventTimer > Minute2Millis(ShutdownTime)) {
        //系统停机
        ShutdownEvent = true;
        SleepEvent = false;
    }
    else if (ReedSwitch_sleepEvent ||
            (SleepTime != 0 && TimerEventTimer > Minute2Millis(SleepTime)) &&
            HandleTrigger != HANDLETRIGGER_ReedSwitch_CHANNEL_1 &&
            HandleTrigger != HANDLETRIGGER_ReedSwitch_CHANNEL_2
            ) {
        //进入休眠
        SleepEvent = true;
    }else{
        SleepEvent = false;
        ShutdownEvent = false;
        //不休眠\不停机 则重置屏保计时器，确保不会进入屏保
        ScreenProtectorTimer = millis();
    }

    //休眠超过一段时间后进入屏保程序
    if (ScreenProtectorTime != 0
        && millis() - ScreenProtectorTimer > ScreenProtectorTime * 1000)
        SleepScreenProtectFlag = true;  //开启屏保程序
    else SleepScreenProtectFlag = false;

    //printf("无动作时间%ld，停机计时%ld，休眠计时%ld\n", TimerEventTimer, Minute2Millis(ShutdownTime), Minute2Millis(SleepTime));
    
}

/*** 
 * @description: 更新系统状态码
 * @param {*}
 * @return {*}
 */
void SYS_StateCode_Update(void) {
    static uint32_t TipEventTimer = 0;  //烙铁安装移除事件计时器：防止事件临界抖动
    if (CalculateTemp((double)LastADC,PTemp) > 500) {

        if (millis() - TipEventTimer > TipEvent_CoolTime) {

            if (TipInstallEvent) {
                //播放设备移除音效
                SetSound(TipRemove);

                //移除烙铁头
                ERROREvent = true;
                TipInstallEvent = false;
                TimerUpdateEvent(); //刷新事件：外界环境变更
                TipEventTimer = millis(); //重置烙铁安装移除事件计时器
                Log(LOG_INFO, "烙铁头移除");
            }
        }
    }
    else
    {
        if (millis() - TipEventTimer > TipEvent_CoolTime) {
            if (!TipInstallEvent) {
                //播放设备安装音效
                SetSound(TipInstall);

                //安装烙铁头
                ERROREvent = false;
                TipInstallEvent = true;
                TimerUpdateEvent(); //刷新事件：外界环境变更
                TipEventTimer = millis(); //重置烙铁安装移除事件计时器
                Log(LOG_INFO, "烙铁头安装");

                //烙铁插入，如果有多个配置则弹出菜单以供选择
                if (TipTotal > 1) {
                    //弹出配置选择菜单
                    System_TipMenu_Init();
                }
            }
        }
    }

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
    
    if (ShutdownEvent) {
        //烙铁进入停机模式
        TempCTRL_Status = TEMP_STATUS_OFF;
        //进入关机事件
        ShutdownEventLoop();
    }else if (SleepEvent) {
        //烙铁进入休眠模式
        TempCTRL_Status = TEMP_STATUS_SLEEP;
    }

    if (ERROREvent==true) {
        //系统错误
        TempCTRL_Status = TEMP_STATUS_ERROR;
    }else if (BoostEvent) {
        //快速升温事件
        TempCTRL_Status = TEMP_STATUS_BOOST;
    }

    //到温声效播放事件
    static uint32_t TempToneStabilitytimer = 0; //到温稳定状态计时器，确保真正播放到温音效
    //到达预定温度 并且足够稳定才能播放到温音效
    if (TempGap <= 6 && TempCTRL_Status != TEMP_STATUS_ERROR) {
        //温差接近目标值：正常
        if (!TempToneFlag && millis() - TempToneStabilitytimer > TempToneStabilitytime) {
            //printf("尝试播放到温音效");
            SetSound(Beep3);
            TempToneFlag = true;
        }
    }else {
        //printf("音效失败 温度:%d 状态:%d 稳定:%d\n", TipTemperature > PID_Setpoint - 10, TempCTRL_Status != TEMP_STATUS_ERROR, millis() - TempToneStabilitytimer > TempToneStabilitytime);
        TempToneStabilitytimer = millis();
        if (TempGap >= 20) TempToneFlag = false;
    }
    
    //读取系统输入电压
    Get_MainPowerVoltage();
    //读取输出电流
    GetCurrent();

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
    //关闭Display()中的PlaySoundLoop
    PlaySoundLoopState = false;

    //屏保以及警示画面
    while(1) {
        Clear();

        DrawIntensiveComputing();

        //文字闪烁
        if ((millis() / 1000) % 2) DrawMsgBox("请关闭电源！");

        Display();
    }
}

bool EnterPasswd(void) {
    //空密码无需授权
    if (strlen(BootPasswd)==0) return 1;

    char passwdBuffer[20] = { 0 };
    TextEditor("[需要授权]", passwdBuffer);

    if (strcmp(passwdBuffer, BootPasswd)) return 0;
    return 1;
}

void SetPasswd(void) {
    if (!EnterPasswd()) {
        Pop_Windows("鉴权失败");
        Pop_Windows("对不起，请重试。");
        return;
    }

    char passwdBuffer[2][20] = {0};
    TextEditor("[设置新密码]", passwdBuffer[0]);
    TextEditor("[确认密码]", passwdBuffer[1]);

    if (strcmp(passwdBuffer[0], passwdBuffer[1])) {
        Pop_Windows("两次密码不一致");
        return;
    }

    strcpy(BootPasswd, passwdBuffer[0]);
    Pop_Windows("密码已更新");
}

/*** 
 * @description: 手柄休眠中断引脚
 * @param {*}
 * @return {*}
 */
void ICACHE_RAM_ATTR SW_IRQHandler(void) {
    //printf("休眠中断 GPIO:%d\n", digitalRead(VIBRATION_SWITCH_PIN));
    //重置事件计时器
    TimerUpdateEvent();
}

/***
 * @description: 手柄休眠引脚循环检测
 * @param {*}
 * @return {*}
 */
void SW_WakeLOOP(void) {
    static bool lastState_channel_1 = 1;  //默认上拉
    //读取休眠控制引脚
    bool state_channel_1 = digitalRead(VIBRATION_SWITCH_PIN);
    bool state_channel_2 = digitalRead(REED_SWITCH_PIN);
    bool change_channel_1 = 0;

    //更新 通道1 休眠引脚状态
    if (state_channel_1 != lastState_channel_1) {
        change_channel_1 = 1;
        lastState_channel_1 = state_channel_1;
    }

    //判断 外部休眠触发 模式
    switch (HandleTrigger) {
    //震动开关 单独使用通道1
    case HANDLETRIGGER_VibrationSwitch_CHANNEL_1: {
        if (change_channel_1) {
            TimerUpdateEvent();
            ReedSwitch_sleepEvent = false;
        }
    }break;
    //干簧管 单独使用通道1
    case HANDLETRIGGER_ReedSwitch_CHANNEL_1: {
        if (state_channel_1 == 0) ReedSwitch_sleepEvent = true;
        else ReedSwitch_sleepEvent = false;
    }break;
    //干簧管 单独使用通道2
    case HANDLETRIGGER_ReedSwitch_CHANNEL_2: {
        if (state_channel_2 == 0) ReedSwitch_sleepEvent = true;
        else ReedSwitch_sleepEvent = false;
    }break;
    //混合模式：电路上干簧管和震动开关独立线路
    case HANDLETRIGGER_MIX_CHANNEL_1_AND_CHANNEL_2: {
        //检查 震动开关CH1 状态
        if (change_channel_1)
            TimerUpdateEvent();
        //检查 干簧管CH2 状态
        //引脚状态变更
        if (!state_channel_2) {
            //干簧管被磁力触发，开启休眠
            ReedSwitch_sleepEvent = true;
        }
        else {
            //干簧管离开磁力源，关闭休眠
            ReedSwitch_sleepEvent = false;
            //重置无动作计时器
            TimerUpdateEvent();
        }
    }break;
    }
    
}
