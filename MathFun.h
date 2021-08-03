#ifndef MathFun_H
#define MathFun_H
#include "Type.h"
int get_Dec_Bit(int x, int n);
int Get_Dec_Deep(int x);
int Get_Int_Bit_Sum(int x);
uint32_t Get_UTF8_Ascii_Pix_Len(uint8_t size, char* s);
uint32_t UTF8_HMiddle(uint32_t x, uint32_t w, uint8_t size, char* s);

void ShowBuffer(uint8_t* buffer, uint32_t size);
#endif