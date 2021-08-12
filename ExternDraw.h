#ifndef ExternDraw
#define ExternDraw
#include "Type.h"

void EnterLogo(void);
void ShowBootMsg(void);

void Clear(void);
void Display(void);

void Draw_Utf(int x, int y, char* s);
void Blur(int sx, int sy, int ex, int ey, int f, int delay);
void Draw_Scale(int x, int y, int w, int h, int s, int v);
void Draw_Num_Bar(int i, int a, int b, int x, int y, int w, int h, int c);
void Draw_Slow_Bitmap(int x, int y, const unsigned char* bitmap, unsigned char w, unsigned char h);
void Draw_Slow_Bitmap_Resize(int x, int y, uint8_t* bitmap, int w1, int h1, int w2, int h2);

void DrawIntensiveComputingLine(void);
void DrawIntensiveComputing(void);
void DrawMsgBox(char* s);
#endif