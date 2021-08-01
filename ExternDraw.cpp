#include "OpenT12.h"
extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C Disp;


void Draw_Utf(int x,int y,char *s){
    Disp.setCursor(x,y + 1);
    Disp.print(s);
}
/*
@ 作用：抖动1
*/
void Blur(int sx, int sy, int ex, int ey, int f,int delayMs) {
	for (int i = 0;i < f;i++) {
		for (int y = 0; y < ey; y++) {
			for (int x = 0; x < ex; x++) {
                if (x % 2 == y % 2 && x % 2 == 0 && x >= sx && x <= ex && y >= sy && y <= ey) Disp.drawPixel(x + (i > 0 && i < 3), y + (i > 1));
                //else Disp.drawPixel(x + (i > 0 && i < 3), y + (i > 1), 0);
			}
		}
		if (delayMs) {
            Disp.sendBuffer();
			delay(delayMs);
		}
	}
		
}
/*
@ 作用：画滚动条
*/
void Draw_Scale(int x, int y, int w, int h, int s, int v) {
  //((h / s >= 4)?(h / (float)s):4)
	Disp.setDrawColor(1);
	if (w < h) {
        Disp.drawVLine(x + w / 2.0, y, h);
		if (s < h && h / s >= 4) for (int i = 0;i < s + 1;i++) {
            if (i % 2) Disp.drawHLine(x + w / (float)s, y + (h / (float)s) * i, w / 2.0 + 1);
            else Disp.drawHLine(x, y + (h / (float)s) * i, w);
		}
		if(s>h) s=h;
        Disp.drawBox(x, v, w, h / (float)s);
	}
	else {
        Disp.drawHLine(x, y + h / 2.0, w);
		if (s < h && h / s >= 4) for (int i = 0;i < s + 1;i++) {
            if (i % 2) Disp.drawVLine(x + (w / (float)s) * i, y + (h / (float)s), h / 2.0 + 1);
            else Disp.drawVLine(x + (w / (float)s) * i, y, h);
		}
		if(s>w) s=w;
		Disp.drawBox(v, y, w / (float)s, w);
	}
}