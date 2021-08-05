#include "OpenT12.h"

//[��Ȼ����nmos Ϊɶ��Ҫpmos]����:https://www.bilibili.com/video/BV1Mb4y1k7fd?from=search&seid=15411923932488650975

enum MOS_Type{
    PMOS=0,
    NMOS
};
//�����¿�
uint8_t MyMOS = PMOS;
uint8_t PWM = 0;
uint16_t TipTemperature = 0;
//PID
double aggKp = 11, aggKi = 0.5, aggKd = 1;
double consKp = 11, consKi = 3, consKd = 5;

//����ʵ���¶�
double CalculateTemp(uint16_t ADC) {
    TipTemperature = PTemp[0] + ADC * PTemp[1] + ADC * ADC * PTemp[2] + ADC * ADC * ADC * PTemp[3];
    return TipTemperature;
}

//��ȡADC����
uint16_t GetADC0(void) {
    //����Ҫ�ر�MOS�ܲ��ܳɹ���ȡ����ż
    SetPOWER(0);

    //��ԭ����ǹرգ����ڹر������ȴ�һ��ʱ�䣬��Ϊ����ż�ͼ���˿�Ǵ���һ��ģ�ֻ�в����ȵ�ʱ����ܰ�ȫ������ż�¶�
    if (PWM!=0) delay(10);
    //��ȡ��ƽ���˲���������Ŵ����Ŵ�����żADC����
    uint16_t ADC = analogRead(ADC_PIN);
    for (uint8_t i = 0;i < 10;i++) {
        ADC += analogRead(ADC_PIN);
        ADC /= 2.0;
    }
    //�ָ�ԭPWM����
    //analogWrite(PWM_PIN, PWM);
    return ADC;
}

//�����������
void SetPOWER(uint8_t POWER) {
    //MOS�ܷ��ദ��
    if (MyMOS==PMOS) {
        //PMOS �͵�ƽ����
        PWM = 255 - POWER;
    }
    //�������PWM
    //analogWrite(PWM_PIN, PWM);
}

