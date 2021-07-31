#include <ArduinoJson.h>

/////////////////////////////////////////////////////////////////
#include <Arduino.h>
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

/////////////////////////////////////////////////////////////////
int counter = 0;
uint32_t DispFlashTimer = 0;
void ESPRotaryInterrupt();

void setup() {
  Serial.begin(115200);

  Disp.begin();
  Disp.enableUTF8Print();
  Disp.setFontDirection(0);
  Disp.setFontPosTop();
  Disp.setFont(u8g2_font_wqy12_t_gb2312);


  pinMode(14, OUTPUT);
  pinMode(ROTARY_PIN2, OUTPUT);
  // 设置外置按钮管脚为上拉输入模式
  pinMode(ROTARY_PIN2, INPUT_PULLUP);
  //配置编码器触发中断
  attachInterrupt(ROTARY_PIN2, ESPRotaryInterrupt, CHANGE);

  Serial.println("\n\Ranged Counter");
  Serial.println("You can only set values between " + String(MIN_POS) + " and " + String(MAX_POS) + ".");

  r.setChangedHandler(rotate);
  r.setLeftRotationHandler(showDirection);
  r.setRightRotationHandler(showDirection);

}

char buffer[50];
void loop() {

    r.loop();
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
