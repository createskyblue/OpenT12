#include "OpenT12.h"
extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C Disp;
/*
	@作用 获取数值某一位
	@传入 数值 指定位数
	@传出 数值指定位数的十进制值
*/
int get_Dec_Bit(int x, int n)
{
	return (int)(x / pow(10.0, n)) % 10;
}

/*
	@作用 获取数值长度
	@传入 数值
	@传出 数值长度
*/
int Get_Dec_Deep(int x)
{
	int i = 0;
	do {
		i++;
		x /= 10;
	} while (x != 0);
	return i;
}

/*
	@作用 获取数字每一位的和
	@传入 数字
	@输出 数字每一位的和
*/
int Get_Int_Bit_Sum(int x)
{
	int sum = 0;
	for (int i = 0; i < Get_Dec_Deep(x); i++)
		sum += get_Dec_Bit(x, i);
	return sum;
}

uint32_t Get_UTF8_Ascii_Pix_Len(uint8_t size,char *s) {
  return Disp.getUTF8Width(s);
}

/*
    @作用 UTF8混合字符串计算水平居中
    @输入：UTF8字符串
    @输出：居中位置
*/
uint32_t UTF8_HMiddle(uint32_t x,uint32_t w,uint8_t size,char *s) {
    return x + ( w - Get_UTF8_Ascii_Pix_Len(size,s)) / 2;
}