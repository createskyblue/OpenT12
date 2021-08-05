#include "OpenT12.h"

void EnterLogo(void) {
    for (int16_t x=-128;x<128;x+=8) {
        //绘制Logo
        Disp.setDrawColor(1);
        Draw_Slow_Bitmap(0, 0, Logo, 128, 64);
        //转场特效
        Disp.setBitmapMode(1);
        Disp.setDrawColor(0);

        Disp.drawXBM(x, 0, 128, 64, TranAnimation);
        if (x < 0) Disp.drawBox(128 + x, 0, -x, 64);

        Disp.setBitmapMode(0);
        Display();
    }
    Disp.setDrawColor(1);
}

void Clear(void) {
    Disp.clearBuffer();
}

void Display(void) {
    ESP.wdtFeed();
    Disp.sendBuffer();
    //printf("d\n");
}

void Draw_Utf(int x,int y,char *s){
    // Disp.setCursor(x,y + 1);
    // Disp.print(s);
    Disp.drawUTF8(x,y+1,s);
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

/*
	@作用：绘制数值条
	@输入：i=值 a=值的最小值 b=值的最大值 x=左上顶点x轴坐标 y=左上顶点y轴坐标 w=宽度 h=高度 c=颜色
*/
void Draw_Num_Bar(int i, int a, int b, int x, int y, int w, int h, int c) {
    Disp.setDrawColor(c);
	Disp.drawFrame(x, y, w - 3 * 6 - 1, h);
	Disp.drawBox(x+2, y+2, map(i, a, b, 0, w - 3 * 6 - 1 - 4), h-4);

    char buffer[20];
    sprintf(buffer,"%d",i);

	Disp.drawStr(x + w - 3 * 6, y -1, buffer);
	//进行去棱角操作:增强文字视觉焦点
    Disp.setDrawColor(0);
	Disp.drawPixel(x, y);
	Disp.drawPixel(x, y + h-1);
	Disp.drawPixel(x + w - 3 * 6-2, y);
	Disp.drawPixel(x + w - 3 * 6-2, y + h-1);
    Disp.setDrawColor(1);
}

void Draw_Slow_Bitmap(int x, int y, const unsigned char* bitmap, unsigned char w, unsigned char h)
{
    uint8_t color = Disp.getDrawColor();
    int xi, yi, intWidth = (w + 7) / 8;
    for (yi = 0; yi < h; yi++) {
        for (xi = 0; xi < w; xi++) {
            if (pgm_read_byte(bitmap + yi * intWidth + xi / 8) & (128 >> (xi & 7))) {
                Disp.drawPixel(x + xi, y + yi);
            }else if (color != 2) {
                Disp.setDrawColor(0);
                Disp.drawPixel(x + xi, y + yi);
                Disp.setDrawColor(color);
            }
        }
    }
}

//位图缩放 代码片段改自arduboy2
void Draw_Slow_Bitmap_Resize(int x, int y, uint8_t *bitmap, int w1,int h1,int w2,int h2) {
    uint8_t color = Disp.getDrawColor();
    // Serial.print("颜色");
    // Serial.println(color);
	float mw=(float)w2/w1;
	float mh=(float)h2/h1;
	uint8_t cmw=ceil(mw);
	uint8_t cmh=ceil(mh);
	int xi, yi, byteWidth = (w1 + 7) / 8;
	for (yi = 0; yi < h1; yi++) {
		for (xi = 0; xi < w1; xi++) {
			if (pgm_read_byte(bitmap + yi * byteWidth + xi / 8) & (128 >> (xi & 7))) {
				Disp.drawBox(x + xi*mw, y + yi*mh, cmw, cmh);
            }else if (color != 2) {
                Disp.setDrawColor(0);
                Disp.drawBox(x + xi*mw, y + yi*mh, cmw, cmh);
                Disp.setDrawColor(color);
            }
		}
	}
}