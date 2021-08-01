#ifndef __MENU_H
#define __MENU_H
#include <c_types.h>
#include "Type.h"

#define CNSize 12
#define SCREEN_COLUMN 128
#define SCREEN_ROW    64
#define SCREEN_PAGE_NUM 8

void System_UI(void);
void Menu_Control(void);
void Draw_APP(int x,int y,char* bitmap);
void Page_Footnotes(int a, int b);
void Text_Reader(char* s);
void Run_Menu_Id(uint8_t lid, uint8_t id);
void Next_Menu(void);
void Pop_Windows(char *s);
void Exit_Menu_System(void);
void Save_Exit_Menu_System(void);
void SmoothAnimationSystem_Clean(void);

#define Smooth_Animation_Num 4
#define Slide_Bar_Num 3
void Smooth_Animation_System();

void About(void);
void Text(void);

/*
	@名称 Smooth_Animation
	@变量
		x    过渡值
		last 上次的值
		val 目标值
		w    平滑权重
*/
struct Smooth_Animation {
	float x;	 //过渡值
	int last;	 //上次的值
	int val;	 //目标值
	float w; 	 //平滑权重
	uint8_t a;   //是否允许累加
	uint8_t b;   //选择过渡动画计算函数
};
extern Smooth_Animation Menu_Smooth_Animation[Smooth_Animation_Num];
/*
	@名称 Slide_Bar
	@变量
		x    值
		min  最小值
		max  最大值
		step 步进
*/
struct Slide_Bar {
	int x;	 //值
	int min;
	int max;
	int step;
};
extern Slide_Bar Slide_space[Slide_Bar_Num];

struct Menu_Level_System {
	u8 id;   //当前层id
	int x;   //当前层所指选项
	u8 min;  //最小页
	u8 max;  //最大页
	u8 a;    //参数A 0:无动作 1:开启图标化菜单
};

struct Menu_System {
	u8 lid;     //层id
	u8 id;      //选项id
	u8 x;       //执行操作 0-跳转到菜单 1-执行函数 2-菜单名 3-开关控件 4-滑动条控件
	char* name;  //选项名称
	char* icon;
	u8 a; //附加参数_1 (0-jump_Level_id) (3-开关id) (4-滑动条id)
	u8 b; //附加参数_2 (0-jump_id) (4-滑动条：true?执行函数:无操作)
	void (*function)();
};


#endif
