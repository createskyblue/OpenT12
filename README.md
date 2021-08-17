# OPEN-T12 基于ESP32/8266的开源焊台
![](img/OLED_ScreenshotInit/爆发.png)
主控ESP32,正式版将兼容ESP8266

## 引脚定义
屏幕SCL         ->22
屏幕SDA         ->21
蜂鸣器          ->25
ADC             ->34
电压ADC         ->35
PWM输出(PMOS)   ->23
震动开关        ->18

## 关于电压ADC的接线方式
24V电源＋---47K电阻----(电压ADC引线)---4.7K电阻---24V电源地

##  展示
![](img/OLED_ScreenshotInit/加热.png)
![](img/OLED_ScreenshotInit/错误.png)
<center>👆主页面</center>

![](img/OLED_ScreenshotInit/配置列表.png)
<center>👆支持保存多组不同的独立烙铁头配置（PID与温度系数统统独立）</center>

![](img/OLED_ScreenshotInit/温度系数.png)
![](img/OLED_ScreenshotInit/校准页面.png)
<center>👆9阶温度系数校准</center>

![](img/OLED_ScreenshotInit/重命名.png)
<center>👆支持中文输入</center>

![](img/OLED_ScreenshotInit/PID.png)
![](img/OLED_ScreenshotInit/修改PID.png)
<center>👆每个烙铁头可以设置两组PID</center>

![](img/OLED_ScreenshotInit/温度场景.png)
![](img/OLED_ScreenshotInit/翻转屏幕.png)
![](img/OLED_ScreenshotInit/密码输入.png)
<center>👆个性化设置</center>