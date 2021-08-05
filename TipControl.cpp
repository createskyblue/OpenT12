#include "OpenT12.h"

//[既然有了nmos 为啥还要pmos]科普:https://www.bilibili.com/video/BV1Mb4y1k7fd?from=search&seid=15411923932488650975

enum MOS_Type{
    PMOS=0,
    NMOS
};
//基础温控
uint8_t MyMOS = PMOS;
uint8_t PWM = 0;
uint16_t TipTemperature = 0;
//PID
double aggKp = 11, aggKi = 0.5, aggKd = 1;
double consKp = 11, consKi = 3, consKd = 5;

//计算实际温度
double CalculateTemp(uint16_t ADC) {
    TipTemperature = PTemp[0] + ADC * PTemp[1] + ADC * ADC * PTemp[2] + ADC * ADC * ADC * PTemp[3];
    return TipTemperature;
}

//获取ADC读数
uint16_t GetADC0(void) {
    //必须要关闭MOS管才能成功读取电热偶
    SetPOWER(0);

    //若原输出非关闭，则在关闭输出后等待一段时间，因为电热偶和加热丝是串在一起的，只有不加热的时候才能安全访问热偶温度
    if (PWM!=0) delay(10);
    //读取并平滑滤波经过运算放大器放大后的热偶ADC数据
    uint16_t ADC = analogRead(ADC_PIN);
    for (uint8_t i = 0;i < 10;i++) {
        ADC += analogRead(ADC_PIN);
        ADC /= 2.0;
    }
    //恢复原PWM配置
    //analogWrite(PWM_PIN, PWM);
    return ADC;
}

//设置输出功率
void SetPOWER(uint8_t POWER) {
    //MOS管分类处理
    if (MyMOS==PMOS) {
        //PMOS 低电平触发
        PWM = 255 - POWER;
    }
    //设置输出PWM
    //analogWrite(PWM_PIN, PWM);
}

