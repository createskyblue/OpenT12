# 朱雀 是基于ESP32的开源焊台控制器

![](img/Logo.png)

本控制器除了控制普通焊台以外，只需通过稍作修改，还可以控制其他非焊台的自动化设备

## 硬件

[LCEDA/EasyEDA]https://oshwhub.com/createskyblue/opent12-jing-jian-ban  
[核心驱动电路参考]https://oshwhub.com/47415Y/gong-cheng

**注意：电路板未完全验证**  
**Note: the circuit board is not fully verified**

![PCB_V1.0](img/readme/PCB3.0.jpg)

## 视频
![](img/OLED_ScreenshotInit/爆发.png)
https://www.bilibili.com/video/BV1s64y1v7mY/

## 功能

***对加热系统的兼容***

- [x] 深圳T12
- [ ] 高频焊台     (该测试版不支持)(未测试理论可行)
- [ ] 铝基板PCB热床(该测试版不支持)(未测试理论可行)
- [ ] 热风枪       (该测试版不支持)


***基本功能***

- [x] 支持中文输入
- [x] 多段PID控制加热，多端ADC/PID采样周期设置
- [x] 可设置 开机温度、爆发温度、休眠温度
- [x] 可定时 休眠、关机，自定义爆发模式时间
- [x] 可保存成千上千万组烙铁头配置（默认10组）
- [ ] 双休眠支持：同时支持震动触发与磁力触发

***高级功能***

- [x] PMOS与NMOS（默认PMOS，由于该选项具有危险性，需要在源程序上修改设置）
- [x] 快捷键快速调节PID
- [x] 每个烙铁头允许设置两组PID
- [x] 可通过菜单设置独立ADC/PID采样周期，卡尔曼滤波器参数
- [x] 9段烙铁头温度拟合校准
- [ ] 温度自校准电路（未实现）
- [x] 自定义显示效果：亮度、翻转平面、过渡动画、显示样式、列表模式
- [x] 个性化自定义：提示音、编码器旋转方向、手柄触发模式、系统语言(暂时只支持简体中文)
- [x] 安全保护设置：自定义开机密码、欠压警报、强制关机提示警告
- [x] 蓝牙远程命令行控制
- [ ] 语音控制（未实现）
- [ ] 自定义开机Logo(未实现)

## 引脚定义
目前硬件还没有设计好，如果想自己制作控制器可以参考下表制作硬件

**使用I2C（默认）**
| I2C模式 | PIN | SPI模式 | PIN |
| :-----: | :----: | :-----: | :----: |
| 屏幕SCL | 22 | 屏幕SCK  | 18 |
| 屏幕SDA | 21 |屏幕MOSI | 23 |
|||屏幕DC   | 13 |
|||屏幕RST  | 15 |
|||屏幕CS   | GND (未来有变动)|

**其他引脚**
| 名称 | PIN |
| :-----: | :----: |
|LED|2 (保留)|
|蜂鸣器|25|
|编码器A|14|
|编码器B|12|
|编码器按键|33|
|测温ADC|39|
|热敏ADC|34|
|电压ADC|35|
|电流ADC|32|
|PWM1|26|
|PWM2|27(保留)|
|震动开关|4|
|干簧管|19|

## 使用说明

- 第一次启动时会进行文件系统格式化，需要一定时间
- 主界面中使用 **单击** 可进入爆发提温模式
- 主界面中使用 **双击** 可呼出PID菜单
- 主界面中使用 **长按** 可打开菜单
- 若有多个烙铁配置，检测到烙铁更换时会在主界面弹出配置选择菜单
- 添加新烙铁配置时，默认参数为空，需要自行校准温度以及设置参数
- 主菜单中 **长按** 可以返回上一级

## 编译安装
安装ArduinoIDE

### 安装ESP32开发板
ArduinoIDE-文件-首选项-附加开发板管理网址->添加:

https://dl.espressif.com/dl/package_esp32_index.json

![](img/readme/ArduinoIDE_1.jpg)
![](img/readme/ArduinoIDE_2.jpg)

ArduinoIDE-工具-开发板-开发板管理器->搜索并安装:ESP32

![](img/readme/ArduinoIDE_3.jpg)

### 第三方依赖库
| 名称 | 链接 |
| :-----: | :-----: |
|U8g2                           |<https://github.com/olikraus/u8g2/>|
|QRCode                         |<https://github.com/ricmoo/qrcode/>|
|OneButton                      |<https://github.com/mathertel/OneButton>|
|Arduino-PID-Library            |<http://playground.arduino.cc/Code/PIDLibrary>|
|GeekFactory_Shell_Library      |<https://github.com/geekfactory/Shell>|

### 选择ESP32并配置编译选项

选择开发板ESP32 Dev Module
![](img/readme/ArduinoIDE_4.jpg)

最低配置需要4MB的Flash空间(Minimal SPIFFS 1.9MB APP with OTA/190KB SPIFFS)
![](img/readme/Esp32_IDE_Set.jpg)

### 编译并上传到开发板

![](img/readme/ArduinoIDE_5.jpg)
![](img/readme/ArduinoIDE_6.jpg)

## 展示
<center>👇主页面</center>

![](img/OLED_ScreenshotInit/加热.png)
![](img/OLED_ScreenshotInit/错误.png)

<center>👇支持保存多组不同的独立烙铁头配置（PID与温度系数统统独立）</center>

![](img/OLED_ScreenshotInit/配置列表.png)

<center>👇9阶温度系数校准</center>

![](img/OLED_ScreenshotInit/温度系数.png)
![](img/OLED_ScreenshotInit/校准页面.png)

<center>👇支持中文输入</center>

![](img/OLED_ScreenshotInit/重命名.png)

<center>👇每个烙铁头可以设置两组PID</center>

![](img/OLED_ScreenshotInit/PID.png)
![](img/OLED_ScreenshotInit/修改PID.png)

<center>👇个性化设置</center>

![](img/OLED_ScreenshotInit/温度场景.png)
![](img/OLED_ScreenshotInit/翻转屏幕.png)
![](img/OLED_ScreenshotInit/密码输入.png)