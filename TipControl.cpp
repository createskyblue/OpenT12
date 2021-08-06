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
double TipTemperature = 0;
double PID_Output = 0;
double PID_Setpoint = 0;
uint32_t ADCSamplingInterval; //ADC采样间隔(ms)
//PID
double aggKp = 11.0, aggKi = 0.5, aggKd = 1.0;
double consKp = 11.0, consKi = 3.0, consKd = 5.0;

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
double CalculateTemp(uint16_t ADC) {
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
KFP KFP_Temp = { 0.02,0,0,0,0.02,7.0000};
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
    printf("%d,%d\r\n", ADC_RAW,ADC);

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
    
    //设置输出PWM
    //if (PWM==255) digitalWrite(PWM_PIN, HIGH);
    //else if (PWM==0) digitalWrite(PWM_PIN, LOW);
    //else PWMOutput(PWM);
    PWMOutput(PWM);
}


//温度控制循环
void TemperatureControlLoop(void) {
    Clear();
    char buffer[50];
    int ADC, LastADC;
    double gap=0;
    sys_Counter_Set(150, 450, 10, 320);
    while(1) {
        shell_task();
        Clear();

        PID_Setpoint = sys_Counter_Get();

        ADC = GetADC0();
        //加热
        if (ADC != -1) {
            LastADC = ADC;
            TipTemperature = CalculateTemp(LastADC);

            gap = abs(PID_Setpoint - TipTemperature);
            if (1) {
                if (gap < 30) MyPID.SetTunings(consKp, consKi, consKd);
                else MyPID.SetTunings(aggKp, aggKi, aggKd);
                //设置PID采样率
                //MyPID.SetSampleTime(ADCSamplingInterval);
                //尝试计算PID
                printf("计算PID：%d\n PID输出:%lf", MyPID.Compute(), PID_Output);
                
            }
            SetPOWER(PID_Output);
        }


        for (uint8_t i=0;i<5;i++) {
            Disp.setCursor(0, 12*i+1);

            switch(i) {
                case 0: sprintf(buffer, "运行时间:%ld", millis()); break;
                case 1: sprintf(buffer, "设定温度：%.0lf°C", PID_Setpoint); break;
                case 2: sprintf(buffer, "当前温度：%.4lf°C", TipTemperature); break;
                case 3: sprintf(buffer, "ADC数据:%d", LastADC); break;
                case 4: sprintf(buffer, "PID输出:%.4lf", PID_Output); break;
            }
            Disp.print(buffer);
        }


        //Progress_Bar(i, 0, FixNum, 0, 64-8, 128, 8, 1);

        Display();
    }
    //关闭功率管输出
    SetPOWER(0);
}