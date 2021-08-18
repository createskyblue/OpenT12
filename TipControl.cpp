#include "OpenT12.h"

//[既然有了nmos 为啥还要pmos]科普:https://www.bilibili.com/video/BV1Mb4y1k7fd?from=search&seid=15411923932488650975

enum MOS_Type{
    PMOS=0,
    NMOS
};
//PWM
uint8_t PWM_Freq = 2000;    // 频率
uint8_t PWM_Channel = 0;    // 通道
uint8_t PWM_Resolution = 8;   // 分辨率
//基础温控
uint8_t MyMOS = PMOS;
uint8_t POWER = 0;
uint8_t PWM = 0;
uint16_t LastADC = 0;
double TipTemperature = 0;
double PID_Output = 0;
double PID_Setpoint = 0;
double TempGap = 0;
uint16_t PIDSampleTime = 10;
uint32_t ADCSamplingInterval = 0; //ADC采样间隔(ms)
//PID
float aggKp = 30.0, aggKi = 0, aggKd = 0.5;
float consKp = 20.0, consKi = 1, consKd = 0.5;

//初始化烙铁头温控系统
void TipControlInit(void) {

    MyPID.SetOutputLimits(0, 255); //PID输出限幅
    MyPID.SetMode(AUTOMATIC); //PID控制模式
    MyPID.SetSampleTime(10); //PID采样时间

    pinMode(TIP_ADC_PIN, INPUT); //ADC
    ledcAttachPin(PWM_PIN, PWM_Channel);  // 将通道与对应的引脚连接
    ledcSetup(PWM_Channel, PWM_Freq, PWM_Resolution); // 设置通道
    SetPOWER(0); //关闭功率管输出
        
}

//计算实际温度
double CalculateTemp(double ADC,double P[]) {
    TipTemperature = P[0] + ADC * P[1] + ADC * ADC * P[2] + ADC * ADC * ADC * P[3];
    return TipTemperature;
}

//PWM输出模块
uint8_t PWMOutput_Lock = true;
void PWMOutput(uint8_t pwm) {
    static uint8_t LastPWM = 0;
    //PWM锁
    if (PWMOutput_Lock || ShutdownEvent || Menu_System_State || ERROREvent) {
        Log(LOG_INFO,"输出被限制");
        printf("输出被限制 PWMOutput_Lock=%d ShutdownEvent=%d Menu_System_State=%d ERROREvent=%d\n", PWMOutput_Lock, ShutdownEvent, Menu_System_State, ERROREvent);
        if (MyMOS == PMOS) pwm = 255;
        else pwm = 0;
    }
    //printf("PWM:%d\n",pwm);
    if (LastPWM != pwm) {
        if (pwm == 255) ledcWrite(PWM_Channel, 256);
        else ledcWrite(PWM_Channel, pwm);

        LastPWM = pwm;
    }
    
}

/**
 *调用卡尔曼滤波器 实践
 */
KFP KFP_Temp = { 0.02,0,0,0,0.01,7.0000};
//获取ADC读数
int GetADC0(void) {
    static uint32_t ADCSamplingTime = 0; //上次采样时间
    static uint32_t ADCReadCoolTimer = 0;

    ADCSamplingInterval = ADCSamplingTime - millis();
    if (ADCSamplingInterval < ADC_PID_Cycle / 20) return -1; //1/4周期后ADC不应该工作，应该把时间留给加热

    
    
    //若原输出非关闭，则在关闭输出后等待一段时间，因为电热偶和加热丝是串在一起的，只有不加热的时候才能安全访问热偶温度
    if (PWMOutput_Lock == false) {
        ADCReadCoolTimer = millis();
        //锁定功率管输出：必须要关闭MOS管才能成功读取电热偶
        PWMOutput_Lock = true;
    }

    if (millis() - ADCReadCoolTimer <= ADC_PID_Cycle / 20) {
        return -1; //数据未准备好
    }

    

    //读取并平滑滤波经过运算放大器放大后的热偶ADC数据
    uint16_t ADC_RAW = analogRead(TIP_ADC_PIN);
    uint16_t ADC = kalmanFilter(&KFP_Temp, (float)ADC_RAW);
    //printf("%d,%d\r\n", ADC_RAW,ADC);

    //解锁功率管输出：前提是没有打开菜单
    if (!Menu_System_State)
        PWMOutput_Lock = false;

    //记录采样间隔时间
    ADCSamplingTime = millis();

    return ADC;
}

//设置输出功率
void SetPOWER(uint8_t power) {
    
    POWER = power;
    //MOS管分类处理
    if (MyMOS==PMOS) {
        //PMOS 低电平触发
        PWM = 255 - power;
    }
    // printf("PWM:%d\n",PWM);
    PWMOutput(PWM);
}


//温度控制循环
void TemperatureControlLoop(void) {
    Clear();
    char buffer[50];
    int ADC;

    PID_Setpoint = sys_Counter_Get();
    if (BoostEvent) {
        //短时功率加成
        PID_Setpoint += BoostTemp;
    }else if (SleepEvent) {
        PID_Setpoint = SleepTemp;
    }

    PID_Setpoint = constrain(PID_Setpoint, TipMinTemp, TipMaxTemp);
    //尝试访问ADC
    ADC = GetADC0();
    if (ADC != -1) {
        LastADC = ADC;
        TipTemperature = CalculateTemp((double)LastADC, PTemp);
        TempGap = abs(PID_Setpoint - TipTemperature);

        /////////////////////////////////////////////////////////////////////////////////////////////
        //控温模式
        if (PIDMode) {
            //PID模式

            //根据温度差选择最优的PID配置，PID参数可被Shell实时更改
            if (TempGap < 30) MyPID.SetTunings(consKp, consKi, consKd);
            else MyPID.SetTunings(aggKp, aggKi, aggKd);
            //更新PID采样时间：采样时间可被Shell实时更改
            //MyPID.SetSampleTime(PIDSampleTime);
            MyPID.SetSampleTime(ADC_PID_Cycle);

            //尝试计算PID
            //printf("计算PID：%d\n PID输出:%lf", MyPID.Compute(), PID_Output);
            MyPID.Compute();
            
        }else{
            //模糊模式
            if(TipTemperature < PID_Setpoint) PID_Output = 255;
            else PID_Output = 0;
        }

        
        //串口打印温度
        //printf("Temp:%lf,%lf,%d\r\n", TipTemperature, PID_Setpoint, ADC);

        /////////////////////////////////////////////////////////////////////////////////////////////
    }
}

