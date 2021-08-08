#include "OpenT12.h"

//[既然有了nmos 为啥还要pmos]科普:https://www.bilibili.com/video/BV1Mb4y1k7fd?from=search&seid=15411923932488650975

enum MOS_Type{
    PMOS=0,
    NMOS
};
//PWM
uint8_t PWM_Freq = 2000;    // 频率
uint8_t PWM_Channel = 1;    // 通道
uint8_t PWM_Resolution = 8;   // 分辨率
//基础温控
uint8_t MyMOS = PMOS;
uint8_t POWER = 0;
uint8_t PWM = 0;
uint16_t LastADC = 0;
double TipTemperature = 0;
double PID_Output = 0;
double PID_Setpoint = 0;
uint16_t PIDSampleTime = 10;
uint32_t ADCSamplingInterval; //ADC采样间隔(ms)
//PID
double aggKp = 30.0, aggKi = 0, aggKd = 0.5;
double consKp = 20.0, consKi = 1, consKd = 0.5;

//初始化烙铁头温控系统
void TipControlInit(void) {

    MyPID.SetOutputLimits(0, 255); //PID输出限幅
    MyPID.SetMode(AUTOMATIC); //PID控制模式
    MyPID.SetSampleTime(10); //PID采样时间

    pinMode(ADC_PIN, INPUT); //ADC
    ledcAttachPin(PWM_PIN, PWM_Channel);  // 将通道与对应的引脚连接
    ledcSetup(PWM_Channel, PWM_Freq, PWM_Resolution); // 设置通道
    SetPOWER(0); //关闭功率管输出
        
}

//计算实际温度
double CalculateTemp(double ADC) {
    TipTemperature = PTemp[0] + ADC * PTemp[1] + ADC * ADC * PTemp[2] + ADC * ADC * ADC * PTemp[3];
    return TipTemperature;
}

//PWM输出模块
void PWMOutput(uint8_t pwm) {
    if (pwm == 255) ledcWrite(PWM_Channel, 256);
    else ledcWrite(PWM_Channel, pwm);
}

/**
 *调用卡尔曼滤波器 实践
 */
KFP KFP_Temp = { 0.02,0,0,0,0.01,7.0000};
//获取ADC读数
int GetADC0(void) {
    static uint32_t ADCSamplingTime = 0; //上次采样时间
    static uint32_t ADCReadCoolTimer = 0;

    //记录采样间隔时间
    ADCSamplingInterval = ADCSamplingTime - millis();
    ADCSamplingTime = millis();
    
    //若原输出非关闭，则在关闭输出后等待一段时间，因为电热偶和加热丝是串在一起的，只有不加热的时候才能安全访问热偶温度
    if (POWER != 0 ) {
        ADCReadCoolTimer = millis();
        //必须要关闭MOS管才能成功读取电热偶
        SetPOWER(0);
    }

    if (millis() - ADCReadCoolTimer <= 2) {
        return -1; //数据未准备好
    }

    //读取并平滑滤波经过运算放大器放大后的热偶ADC数据
    uint16_t ADC_RAW = analogRead(ADC_PIN);
    uint16_t ADC = kalmanFilter(&KFP_Temp, (float)ADC_RAW);
    //printf("%d,%d\r\n", ADC_RAW,ADC);

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
    PWMOutput(PWM);
}


//温度控制循环
void TemperatureControlLoop(void) {
    Clear();
    char buffer[50];
    int ADC;
    double gap=0;

    PID_Setpoint = sys_Counter_Get();

    //尝试访问ADC
    ADC = GetADC0();
    if (ADC != -1) {
        LastADC = ADC;
        TipTemperature = CalculateTemp((double)LastADC);
        gap = abs(PID_Setpoint - TipTemperature);

        //控温模式
        if (PIDMode) {
            //PID模式

            //根据温度差选择最优的PID配置，PID参数可被Shell实时更改
            if (gap < 30) MyPID.SetTunings(consKp, consKi, consKd);
            else MyPID.SetTunings(aggKp, aggKi, aggKd);
            //更新PID采样时间：采样时间可被Shell实时更改
            MyPID.SetSampleTime(PIDSampleTime);

            //尝试计算PID
            //printf("计算PID：%d\n PID输出:%lf", MyPID.Compute(), PID_Output);
            MyPID.Compute();
            
        }else{
            //模糊模式
            if(TipTemperature < PID_Setpoint) PID_Output = 255;
            else PID_Output = 0;
        }

        //更新状态码
        if (PID_Output > 180) {
            //加热
            TempCTRL_Status = TEMP_STATUS_HEAT;
        }else{
            if (gap < 10) {
                //温差接近目标值：正常
                TempCTRL_Status = TEMP_STATUS_WORKY;
            }else{
                //进行PID微调：维持
                TempCTRL_Status = TEMP_STATUS_HOLD;
            }
        }

        if (TipTemperature > 500) {
            //未检测到烙铁头
            TempCTRL_Status = TEMP_STATUS_ERROR;
            //强制关闭输出
            PID_Output = 0;
        }

        //设置功率管输出功率
        SetPOWER(PID_Output);

        //串口打印温度
        printf("Temp:%lf,%lf,%d\r\n", TipTemperature, PID_Setpoint, ADC);
    }
}