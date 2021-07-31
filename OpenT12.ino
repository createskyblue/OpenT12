/////////////////////////////////////////////////////////////////
#include <Arduino.h>
#include <Ticker.h>
#include <U8g2lib.h>
#include "ESPRotary.h";

/////////////////////////////////////////////////////////////////
#define ROTARY_PIN1	0
#define ROTARY_PIN2	2

#define CLICKS_PER_STEP   4   // this number depends on your rotary encoder 
#define MIN_POS   5
#define MAX_POS   20


/////////////////////////////////////////////////////////////////
U8G2_SSD1306_128X64_NONAME_F_HW_I2C Disp(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
ESPRotary r = ESPRotary(ROTARY_PIN1, ROTARY_PIN2, CLICKS_PER_STEP, MIN_POS, MAX_POS);
Ticker ESPRotaryLoop;
/////////////////////////////////////////////////////////////////
int counter = 0;
uint32_t DispFlashTimer = 0;
void ESPRotaryInterrupt();

void setup() {
    //初始化串口
    Serial.begin(115200);

    //初始化OLED
    Disp.begin();
    Disp.enableUTF8Print();
    Disp.setFontDirection(0);
    Disp.setFontPosTop();
    Disp.setFont(u8g2_font_wqy12_t_gb2312);

    //初始化GPIO
    pinMode(14, OUTPUT);

    //初始化中断
    ESPRotaryLoop.attach_ms(10, ESPRotaryInterrupt);

    Serial.println("\n\Ranged Counter");
    Serial.println("You can only set values between " + String(MIN_POS) + " and " + String(MAX_POS) + ".");

    r.setChangedHandler(rotate);
    r.setLeftRotationHandler(showDirection);
    r.setRightRotationHandler(showDirection);

}

char buffer[50];
void loop() {

    //r.loop();
    if (millis() - DispFlashTimer > 33) {
        //tone(14, 1000);

        Disp.clearBuffer();
        sprintf(buffer, "计数:%d", counter);
        Disp.setCursor(0, 1);
        Disp.print(buffer);

        sprintf(buffer, "运行时间:%ld", millis());
        Disp.setCursor(0, 13);
        Disp.print(buffer);

        Disp.sendBuffer();

        DispFlashTimer = millis();
        noTone(14);//停止发声
    }

}

/////////////////////////////////////////////////////////////////

// on change
void rotate(ESPRotary& r) {
    counter = r.getPosition();
    Serial.println(counter);
}

// on left or right rotation
void showDirection(ESPRotary& r) {
    Serial.println(r.directionToString(r.getDirection()));
}

void ICACHE_RAM_ATTR ESPRotaryInterrupt() {
    r.loop();
}
/////////////////////////////////////////////////////////////////
