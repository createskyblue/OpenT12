#include "Menu.h"
#include "OpenT12.h"
extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C Disp;


//菜单系统状态 0:自动退出 1:运行菜单系统
uint8_t Menu_System_State = 0;

//菜单层id 菜单项目id
uint8_t MenuLevelId = 0, MenuId = 0; //标注值
int32_t real_Level_Id,Pos_Id,Back_Id=-1; //索引值


//全局变量 控制右下角角标显示时间
int32_t pages_Tip_Display_timer = 0;
//默认无动作后的1500ms关闭悬浮角标显示
#define pages_Tip_Display_Timeout 1500


/*/////////////////////////////////////////////////////////////////////

    @需要手动设置的项目

*//////////////////////////////////////////////////////////////////////
//由于sizeof的原理是在编译阶段由编译器替换数组大小，因此我们无法计算指针的sizeof！需要在位图的第一个字节填写 n 阶矩阵图像
unsigned char Set0[] = { 42,0xff,0xff,0xff,0xff,0xff,0xc0,0xff,0xff,0xff,0xff,0xff,0xc0,0xff,0xff,0xff,0xff,0xff,0xc0,0xff,0xff,0xff,0xff,0xff,0xc0,0xff,0xff,0xff,0xff,0xff,0xc0,0xff,0xff,0xff,0xff,0xff,0xc0,0xff,0xff,0xff,0xff,0xff,0xc0,0xff,0xff,0xff,0xff,0xff,0xc0,0xff,0xff,0xff,0xff,0xff,0xc0,0xff,0xff,0x7f,0xff,0xff,0xc0,0xff,0xfe,0x7f,0xff,0xff,0xc0,0xff,0xfe,0xff,0xff,0xf7,0xc0,0xff,0xfc,0xff,0xff,0xc3,0xc0,0xff,0xf0,0xc7,0xff,0x33,0xc0,0xff,0xfe,0xef,0xcc,0xe7,0xc0,0xff,0xcc,0xcf,0x0f,0xef,0xc0,0xff,0xc1,0x5f,0xcd,0xdf,0xc0,0xff,0xb8,0x1f,0xdc,0x9f,0xc0,0xff,0xb6,0x04,0xdb,0xbf,0xc0,0xff,0x9e,0x4c,0x5b,0x7f,0xc0,0xff,0xf9,0x9c,0x99,0x7f,0xc0,0xff,0xe7,0x3d,0xbf,0x7f,0xc0,0xff,0x9c,0xfc,0x36,0x7f,0xc0,0xfb,0x70,0xf8,0x26,0xff,0xc0,0xf8,0xfd,0x39,0xa4,0xff,0xc0,0xf9,0xfc,0x3f,0xc8,0xff,0xc0,0xf9,0xf1,0xff,0x86,0xff,0xc0,0xfb,0x81,0xff,0x46,0xff,0xc0,0xff,0xd9,0xfe,0x86,0xff,0xc0,0xff,0xf9,0xfe,0x3d,0xff,0xc0,0xff,0xd9,0xff,0x29,0xff,0xc0,0xff,0xf1,0xff,0x3d,0xff,0xc0,0xff,0xff,0xff,0xbf,0xff,0xc0,0xff,0xff,0xff,0xff,0xff,0xc0,0xff,0xff,0xff,0xff,0xff,0xc0,0xff,0xff,0xff,0xff,0xff,0xc0,0xff,0xff,0xff,0xff,0xff,0xc0,0xff,0xff,0xff,0xff,0xff,0xc0,0xff,0xff,0xff,0xff,0xff,0xc0,0xff,0xff,0xff,0xff,0xff,0xc0,0xff,0xff,0xff,0xff,0xff,0xc0,0xff,0xff,0xff,0xff,0xff,0xc0 };

unsigned char Set1[] = { 14,0xc3,0xfc,0x18,0x00,0x18,0x00,0xc3,0xfc,0xff,0xfc,0xff,0x0c,0x00,0x60,0x00,0x60,0xff,0x0c,0xff,0xfc,0xf0,0xfc,0x06,0x00,0x06,0x00,0xf0,0xfc };
unsigned char Set2[] = { 14,0xf0,0x3c,0xce,0x0c,0xbf,0x04,0xb3,0x04,0x73,0x00,0x7f,0x00,0x7e,0x00,0x7c,0x00,0x7c,0x00,0x7c,0x60,0xbc,0x64,0xbe,0x04,0xcf,0x0c,0xf0,0x3c };
unsigned char Set3[] = { 14,0x80,0x0c,0x00,0x1c,0x3f,0xf4,0x3f,0xe0,0x3f,0xc4,0x37,0x8c,0x23,0x18,0x30,0x30,0x38,0x70,0x3c,0xf0,0x3f,0xf0,0x3f,0xf0,0x00,0x00,0x80,0x04 };
unsigned char Set4[] = { 14,0x00,0x04,0x58,0x08,0x58,0x08,0x58,0x08,0x40,0x08,0x7f,0xf8,0x60,0x18,0x5f,0xe8,0x5b,0x68,0x5f,0xe8,0x5b,0x68,0x1c,0xe8,0x5f,0xe8,0x00,0x00 };
/*
    @变量组 Switch_space[]
    @brief 开关控件 用于存储开关控件的值
    @param -
    
    @注册
        -0->平滑动画
        -1->单选框测试
        -2->选项条固定长度
*/
#define SwitchSpace_SmoothAnimation 0
#define SwitchSpace_OptionStripFixedLength 2
uint8_t Switch_space[] = {1,0,0};

/*
    @结构体 Slide_Bar Slide_space[Slide_Bar_Num]
    @brief 滑动条控件 用于存储滑动条控件的值
    @param -
    
    @注册
        -0->屏幕亮度设定值
        -1->自适应菜单滚动范围
    @变量
        int   x    值
        int   min  最小值
        int   max  最大值
        int   step 步进
*/
struct Slide_Bar Slide_space[Slide_Bar_Num] = {
    {255,0,255,1}, //亮度调整
    {0,0,SCREEN_ROW / 16 + 1,1}, //自适应菜单滚动范围
    {0,0,100,1}, //音量调整::默认静音
};

/*
    @结构体 Smooth_Animation Menu_Smooth_Animation[Smooth_Animation_Num]
    @brief 存放过渡动画设定集
    @param -
    
    @变量
        float   x   过渡值
        int32_t     last 上次的值
        int32_t     val 当前值
        float   w   平滑权重
        uint8_t a   是否允许累加
        uint8_t b   选择过渡动画计算函数
*/
struct Smooth_Animation Menu_Smooth_Animation[Smooth_Animation_Num] = {
    {0,0,0,0.4,1,0}, //菜单项目滚动动画
    {0,0,0,0.2,1,0}, //滚动条平滑动画
    {0,0,0,0.2,1,0}, //菜单选项条长度平滑动画
    {0,0,0,0.3,0,0}, //项目归位动画
};


/*
    @结构体 Menu_Level_System MenuLevel[]
    @brief 菜单层级 设定
    @param -
    
    @变量
        uint8_t id    当前层id
        int x    当前层所指选项
        uint8_t min   最小页
        uint8_t max   最大页
        uint8_t a     参数A 0:无动作 1:开启图标化菜单
*/
struct Menu_Level_System MenuLevel[3] = {
    {0,0,0,4,1},
    {1,0,0,4,0},
    {2,0,0,5,0},
};

/*
    @结构体 Menu_System Menu[]
    @brief 菜单层级项目 设定
    @param -
    
    @变量
        uint8_t lid             层id
        uint8_t id              选项id
        uint8_t x               执行操作 0-跳转到菜单 1-执行函数 2-菜单名 3-开关控件 4-滑动条控件
        char name[21]      选项名称 支持半角
        char *icon         图标(此项选填 若当前层级菜单开启了图标化) 详细可以参考关于图标化的说明文档
        uint8_t a               附加参数_1 (0-jump_Level_id) (3-开关id) (4-滑动条id)
        uint8_t b               附加参数_2 (0-jump_id) (4-滑动条：true?执行函数:无操作)
        void (*function)(void);
*/
#define Menu_NULL_IMG               0
#define Menu_NULL_F                 0

#define Jump_Menu_Op                0
#define F_Menu_Op                   1
#define Title_Menu_Op               2
#define Switch_Menu_Op              3
#define Progress_Bar_Menu_Op        4
#define SingleBox_Menu_Op           5

struct Menu_System Menu[] = {
    {0,0,       Title_Menu_Op,          "主菜单",               Menu_NULL_IMG,              0,                                  0,          Menu_NULL_F},
    {0,1,       Jump_Menu_Op,          "学习强国",              Set0,                        0,                                  0,          Menu_NULL_F},
    {0,2,       Jump_Menu_Op,           "个性化",               Set1,                       2,                                  0,          Menu_NULL_F},
    {0,3,       Jump_Menu_Op,           "系统",                 Set2,                       1,                                  0,          Menu_NULL_F},
    {0,4,       F_Menu_Op,              "返回",                 Set3,                       0,                                  0,          (*Exit_Menu_System)},

    {1,0,       Title_Menu_Op,          "系统",                 Menu_NULL_IMG,              0,                                  0,          Menu_NULL_F},
    {1,1,       F_Menu_Op,              "查看曲线系数",          Menu_NULL_IMG,              0,                                  0,          Menu_NULL_F},
    {1,2,       F_Menu_Op,              "校准纸张计数器",        Menu_NULL_IMG,              0,                                  0,          Menu_NULL_F},
    {1,3,       F_Menu_Op,              "关于",                 Menu_NULL_IMG,              0,                                  0,          Menu_NULL_F},
    {1,4,       Jump_Menu_Op,           "返回",                 Menu_NULL_IMG,              0,                                  3,          Menu_NULL_F},

    {2,0,       Title_Menu_Op,          "个性化",               Menu_NULL_IMG,              0,                                  0,          Menu_NULL_F},
    {2,1,       Switch_Menu_Op,         "过渡动画",             Menu_NULL_IMG,              SwitchSpace_SmoothAnimation,        0,          (*SmoothAnimationSystem_Clean)},
    {2,2,       Switch_Menu_Op,         "选项条定宽",           Menu_NULL_IMG,              SwitchSpace_OptionStripFixedLength, 0,          Menu_NULL_F},
    {2,3,       Progress_Bar_Menu_Op,   "屏幕亮度",             Menu_NULL_IMG,              0,                                  1,          Menu_NULL_F},
    {2,4,       Progress_Bar_Menu_Op,   "音量",                 Menu_NULL_IMG,              2,                                  1,         Menu_NULL_F},
    {2,5,       Jump_Menu_Op,           "返回",                 Menu_NULL_IMG,              0,                                  2,          Menu_NULL_F},

};
int deg=0;
//系统UI
void System_UI() {

    // //触发编码器按键进入菜单
    // if (true) {
    //     delay(50);
    //     //初始化菜单
    //     MenuLevel[0].x=0;  //复位第一层菜单的位置
    //     Slide_space[1].x=0;//复位第一层菜单的位置
    //     Next_Menu();
    //     while (Menu_System_State) {
    //         Menu_Control();
    //     }
    // }
}

/*/////////////////////////////////////////////////////////////////////

    @自定义功能函数

*//////////////////////////////////////////////////////////////////////



// void About() {
//     Disp.clearBuffer();
//     Text_Reader("２０１９大学生电子设计竞赛Ｆ题纸张计数器程序：赖浩文        硬件：周孜宁        辅助：张家穗");
//     Display();
// }


/*/////////////////////////////////////////////////////////////////////

    @相关工具函数

*//////////////////////////////////////////////////////////////////////
#define RollingStripWidth 3

/*复选框选中 10*10*/
uint8_t CheckBoxSelection[] = { 0xff,0xc0,0x80,0x40,0x80,0xc0,0x81,0xc0,0x81,0xc0,0x83,0x40,0x9b,0x40,0x8e,0x40,0x86,0x40,0xff,0xc0 };

void Save_Exit_Menu_System() {
    Pop_Windows("正在保存数据\n请勿切断电源");

    //这里放读写flash的代码
    delay(1000);

    Exit_Menu_System();
}

void Exit_Menu_System() {
    Menu_System_State = 0;
    //过渡离开
    Disp.setDrawColor(0);
    Blur(0, 0, SCREEN_COLUMN, SCREEN_ROW, 4, 66 * Switch_space[SwitchSpace_SmoothAnimation]);
    Disp.setDrawColor(1);
}

/*
    @函数 Update_OLED_Light_Level
    @brief 更新屏幕亮度设置
    @param -
    
*/
void Update_OLED_Light_Level() {
    // Write_Cmd(0x81);  //向SSD1306发送指令：设置内部电阻微调
    // Write_Cmd(Slide_space[0].x); //微调范围（0-255）
}

/*
    @函数 Smooth_Animation_System
    @brief 过渡动画运算
*/
void Smooth_Animation_System() {
    for (uint8_t i = 0;i < Smooth_Animation_Num;i++) {
        //优化计算：变形过滤器
        if (Menu_Smooth_Animation[i].x && abs(Menu_Smooth_Animation[i].x * 100) < 1.5) Menu_Smooth_Animation[i].x = 0;
        //动画控制变量是否需要更新
        if (Menu_Smooth_Animation[i].last != Menu_Smooth_Animation[i].val) {
            //是否允许累加
            if (Menu_Smooth_Animation[i].a)  Menu_Smooth_Animation[i].x += Menu_Smooth_Animation[i].val - Menu_Smooth_Animation[i].last;
            else  Menu_Smooth_Animation[i].x = Menu_Smooth_Animation[i].val - Menu_Smooth_Animation[i].last;
            //重置标志
            Menu_Smooth_Animation[i].last = Menu_Smooth_Animation[i].val;
            //动画特殊个性化配置
            switch(i){
                default: break;
            }
        }
        //使用被选的动画计算函数计算动画
        switch(Menu_Smooth_Animation[i].b) {
            case 0:
                Menu_Smooth_Animation[i].x -= Menu_Smooth_Animation[i].x * Menu_Smooth_Animation[i].w;
            break;
        }
    }
}
void SmoothAnimationSystem_Clean(){
    for (uint8_t i = 0;i < Smooth_Animation_Num;i++){
        Menu_Smooth_Animation[i].x=0;
    }
}
/*
    @函数 Page_Footnotes
    @brief 自适应屏幕右下角角标绘制
    @param int a 分子
    @param int b 分母
*/
void Page_Footnotes(int a, int b) {
    char buffer[20];
    uint8_t w = (Get_Dec_Deep(a) + Get_Dec_Deep(b) + 3) * 6;
    uint8_t x = SCREEN_COLUMN - 8 - w;
    uint8_t y = SCREEN_ROW - 12;

    if (millis() < pages_Tip_Display_timer + pages_Tip_Display_Timeout) {
        //绘制白色底色块
        Disp.setDrawColor(1);
        Disp.drawRBox(x + 1, y - 1, w, 13 ,1);
        //绘制下标文字
        Disp.setDrawColor(0);
        Disp.setCursor(x,y + 1);
        sprintf(buffer,"[%d/%d]", a, b);
        Disp.print(buffer);
    }
    //恢复颜色设置
    Disp.setDrawColor(1);
}

/*
    @函数 Text_Reader
    @brief 文本浏览器（全角GBK）
    @param (char* s):字符串首地址
    
*/
void Text_Reader(char* s) {
    // int len = strlen(s)/2;
    // int Colunm_GBK_Char = SCREEN_COLUMN / CNSize;
    // int Row_GBK_Char = SCREEN_ROW / CNSize;
    // int pages = len / (Colunm_GBK_Char * Row_GBK_Char);
    // int val_Pages = 0, last_Pages = pages + 1;
    // char page_s[(SCREEN_COLUMN / CNSize) * 2 + 1];
    // int now_pow;

    // //重置页码角标显示时间
    // pages_Tip_Display_timer = millis();

    // //初始化编码器设置
    // sys_Counter_Set(0, pages, -1, 0);
    // uint8_t key = bsp_GetKey();
    // uint8_t lastKey = 0;
    // while (!sys_KeyProcess()) {

    //     //获取输入
    //     val_Pages = sys_Counter_Get();
    //     //只有翻页时才刷新屏幕 节省资源::这里页码超时加了100ms是为了超时后可以Display刷新一下屏幕
    //     if (val_Pages != last_Pages || (millis() - pages_Tip_Display_timer < pages_Tip_Display_Timeout + 100)) {
    //         last_Pages = val_Pages;
    //         Disp.clearBuffer();

    //         //自适应屏幕高度
    //         for (int i = 0;i < Row_GBK_Char;i++) {
    //             //计算当前行第一个字符地址
    //             now_pow = (val_Pages * Colunm_GBK_Char * Row_GBK_Char + Colunm_GBK_Char * i) * 2;
    //             for (int i = 0;i < sizeof(page_s);i++) {
    //                 if (now_pow + i < strlen(s) && i < sizeof(page_s) - 1)
    //                     page_s[i] = s[now_pow + i];
    //                 else {
    //                     page_s[i] = '\0';
    //                     break;
    //                 }
    //             }
    //             //显示当前行全角GBK字符
    //             Draw_Utf(0, i * (CNSize + 1), page_s);
    //         }
    //         //绘制滚动条
    //         Draw_Scale(SCREEN_COLUMN - 5, 0, 5, SCREEN_ROW - 1, pages + 1, map(val_Pages, 0, pages, 0, SCREEN_ROW - SCREEN_ROW /2- 1));
    //         //绘制页码下标
    //         Page_Footnotes(val_Pages + 1, pages + 1);
    //         Display();
    //     }
    // }
    // //延迟 防止短时间多次触发
    // delay(50);
    // //初始化菜单控件
    // //Next_Menu();
}

/*
    @函数 Get_Real_Menu_Level_Id
    @brief 获取 菜单层对象 的索引值
    @param uint8_t id 菜单层对象id
    @return 菜单层对象 的索引值
*/
int Get_Real_Menu_Level_Id(uint8_t id) {
    uint8_t real_Level_Id;
    for (uint8_t i = 0;i < sizeof(MenuLevel) / sizeof(MenuLevel[0]);i++)
        if (MenuLevel[i].id == id) real_Level_Id = i;
    return real_Level_Id;
}

/*
    @函数 Get_Menu_Id
    @brief 获取 菜单对象 的索引值
    @param 菜单层对象id 菜单对象id
    @return 菜单对象 的索引值
*/
int Get_Menu_Id(uint8_t lid, uint8_t id) {
    uint8_t real_Level_Id;
    for (uint8_t i = 0;i < sizeof(Menu) / sizeof(Menu[0]);i++)
        if (Menu[i].lid == lid && Menu[i].id == id) real_Level_Id = i;
    return real_Level_Id;
}

/*
    @函数 Next_Menu
    @brief 多级菜单跳转初始化参数
*/
void Next_Menu() {
    real_Level_Id = Get_Real_Menu_Level_Id(MenuLevelId);
    uint8_t Id = Get_Menu_Id(MenuLevel[real_Level_Id].id, 0);
    //sys_Counter_Set(MenuLevel[real_Level_Id].min, MenuLevel[real_Level_Id].max, 1, MenuLevel[real_Level_Id].x);
    //Slide_space[1].x = 0;


    if (!MenuLevel[real_Level_Id].a || SCREEN_ROW <= 32) {
        sys_Counter_Set(Slide_space[1].min, Slide_space[1].max, -1, MenuLevel[real_Level_Id].x);
    }else {
            if (Menu[Get_Menu_Id(real_Level_Id, 0)].x) MenuLevel[real_Level_Id].min=1; //当前处在图标模式 如果目标层菜单的第一项为标题，则给予屏蔽
            sys_Counter_Set(MenuLevel[real_Level_Id].min, MenuLevel[real_Level_Id].max, 1, MenuLevel[real_Level_Id].x);
            Slide_space[1].x = 0;
    }

    if (Switch_space[SwitchSpace_SmoothAnimation]) {
        Disp.setDrawColor(0);
        Blur(0, 0, SCREEN_COLUMN, SCREEN_ROW, 4, 20 * Switch_space[SwitchSpace_SmoothAnimation]);
        Disp.setDrawColor(1);

        //项目归位动画
        Menu_Smooth_Animation[3].last = 0;
        Menu_Smooth_Animation[3].val = 1;
    }
    Menu_System_State = 1;
}

/*
    @函数 Pop_Windows
    @brief 自适应文本大小信息弹窗
    @param (char* s):字符串首地址
    
*/
void Pop_Windows(char* s) {
    Disp.setCursor(0, 0);
    Disp.print(s);
    Display();
    // //Set_Font_Size(2);
    // int w, h;
    // for (int i = 5;i > 0;i--) {
    //     //Set_Font_Size(i);
    //     w = CNSize * Get_Max_Line_Len(s) * Get_Font_Size() / 2;
    //     h = CNSize * Get_Font_Size() * Get_Str_Next_Line_O(s);
    //     if (w < SCREEN_COLUMN && h < SCREEN_ROW) break;
    // }
    // int x = (SCREEN_COLUMN - w) / 2;
    // int y = (SCREEN_ROW - h) / 2;

    // Disp.setDrawColor(0);
    // Blur(0, 0, SCREEN_COLUMN, SCREEN_ROW, 3, 66 * Switch_space[SwitchSpace_SmoothAnimation]); //<=15FPS以便人眼察觉细节变化

    // int ix = 0;
    // for (int i = 1;i <= 10;i++) {
    //     //震荡动画
    //     if (Switch_space[SwitchSpace_SmoothAnimation]) ix = (10 * cos((i * 3.14) / 2.0)) / i;

    //     Disp.setDrawColor(0);
    //     Blur(0, 0, SCREEN_COLUMN, SCREEN_ROW, 3, 0);
    //     Disp.drawFrame(x - 1 + ix, y - 3, w + 1, h + 3);
    //     Disp.setDrawColor(1);
    //     Draw_Fill_Round_Rect2(x + ix, y - 2, w, h + 2);
    //     Disp.setDrawColor(0);
    //     Draw_Utf(x + 1 + ix, y - 1, s);
    //     Disp.setDrawColor(1);
    //     Display();
    //     delay(20 * Switch_space[SwitchSpace_SmoothAnimation]);
    // }
    // //Set_Font_Size(1);
}

/*
    @函数 Run_Menu_Id
    @brief 按照菜单项预设的参数执行命令
    @param uint8_t lid 菜单层对象id 
    @param uint8_t id菜单对象id
*/
void Run_Menu_Id(uint8_t lid, uint8_t id) {
    // Serial.print("菜单系统:");
    // Serial.print(lid);
    // Serial.print(" + ");
    // Serial.println(id);
    uint8_t Id;
    uint8_t real_Level_Id = Get_Real_Menu_Level_Id(MenuLevelId);
    Id = Get_Menu_Id(lid, id);
    switch (Menu[Id].x) {
    case 0:
        MenuLevelId = Menu[Id].a;
        if (!MenuLevel[Menu[Id].a].a) { //如果当前菜单层没有开启了图表化显示则对子菜单选项定向跳转执行配置
            /*默认跳转后选项在最下端
            Slide_space[1].x =constrain(Menu[Id].b,0,3);
            MenuLevel[Menu[Id].a].x=constrain(Menu[Id].b + 1 - (SCREEN_ROW/16) ,0,MenuLevel[Menu[Id].a].max);
            */

           //跳转后选项在最上端：可能会出现意外的bug 慎重
           Slide_space[1].x = 0;
           MenuLevel[Menu[Id].a].x=Menu[Id].b;
           if (MenuLevel[Menu[Id].a].max-MenuLevel[Menu[Id].a].x<=(SCREEN_ROW/16-1)) Slide_space[1].x=(SCREEN_ROW/16-1)-(MenuLevel[Menu[Id].a].max-MenuLevel[Menu[Id].a].x);

        }
        if (Menu[Id].function) Menu[Id].function();
        Next_Menu();
        break;
    case 1:
        Pop_Windows("正在处理");
        if (Menu[Id].function) Menu[Id].function();
        sys_Counter_Set(Slide_space[1].min, Slide_space[1].max, -1, Slide_space[1].x + 1);
        break;
    case 3:
        Switch_space[Menu[Id].a] = !Switch_space[Menu[Id].a];
        if (Menu[Id].function) Menu[Id].function();
        break;
    case 4:
        //Switch_space[Menu[Id].a]=!Switch_space[Menu[Id].a];
        sys_Counter_Set(Slide_space[Menu[Id].a].min, Slide_space[Menu[Id].a].max, Slide_space[Menu[Id].a].step, Slide_space[Menu[Id].a].x);
        Disp.setDrawColor(0);
        Blur(0, 0, SCREEN_COLUMN, SCREEN_ROW, 3, 66 * Switch_space[SwitchSpace_SmoothAnimation]);
        Disp.setDrawColor(1);

        while (!sys_KeyProcess()) {
            Disp.setDrawColor(0);
            Disp.drawBox(SCREEN_COLUMN / 8 - 2, (SCREEN_ROW - 24) / 2 - 3, 3 * SCREEN_COLUMN / 4 + 4, 24 + 4);
            Disp.setDrawColor(1);

            Disp.drawRFrame(SCREEN_COLUMN / 8 - 3, (SCREEN_ROW - 24) / 2 - 4, 3 * SCREEN_COLUMN / 4 + 4, 24 + 6 ,2);

            Slide_space[Menu[Id].a].x = sys_Counter_Get();
            Draw_Utf(SCREEN_COLUMN / 8, (SCREEN_ROW - 24) / 2, Menu[Id].name);
            Draw_Num_Bar(Slide_space[Menu[Id].a].x, Slide_space[Menu[Id].a].min, Slide_space[Menu[Id].a].max, SCREEN_COLUMN / 8, (SCREEN_ROW - 24) / 2 + CNSize + 3, 3 * SCREEN_COLUMN / 4, 7, 1);
           
            Display();
            //当前滑动条为屏幕亮度调节 需要特殊设置对屏幕亮度进行实时预览
            if (Menu[Id].function) Menu[Id].function();
        }
        delay(50);

        //静音
        //PlayTones(0, 0, 0);

        //sys_Counter_Set(MenuLevel[real_Level_Id].min, MenuLevel[real_Level_Id].max, 1, MenuLevel[real_Level_Id].x);
        if (Menu[Id].function) Menu[Id].function();
        sys_Counter_Set(Slide_space[1].min, Slide_space[1].max, -1, Slide_space[1].x + 1);
        break;
    
    case 5://单选模式
        Switch_space[Menu[Id].a] = Menu[Id].b;
        if (Menu[Id].function) Menu[Id].function();
        break;
    }
}

void Draw_APP(int x, int y, char* bitmap) {
    Disp.setDrawColor(1);
    Disp.drawRBox(x - 3, y - 3, 42 + 6, 42 + 6, 4);
    Disp.setDrawColor(0);

    Disp.drawBox(x, y, 42, 42);
    Disp.setDrawColor(1);
    if (bitmap[0] == 14)
        Draw_Slow_Bitmap_Resize(x, y, bitmap + 1, 14, 14, 42,42);
    else Draw_Slow_Bitmap(x, y, bitmap + 1, 42, 42);

}


int DEBUG_3=15;
int DEBUG_4=123;
static int Menu_Smooth_Animation_Y = 0;
static int Menu_Smooth_Animation_Last_choose = 0;
/*
    @函数 Menu_Control
    @brief 渲染主菜单
    @param -
*/
void Menu_Control() {
    //if (!Menu_System_State) return;
    Disp.clearBuffer();

    //计算过渡动画
    if (Switch_space[SwitchSpace_SmoothAnimation]) Smooth_Animation_System();

    //分别获取 菜单层、菜单项 索引值
    real_Level_Id = Get_Real_Menu_Level_Id(MenuLevelId);
    Pos_Id = Get_Menu_Id(MenuLevel[real_Level_Id].id, MenuLevel[real_Level_Id].x + Slide_space[1].x);

    //若当前菜单层级没有开题图标化则使用普通文本菜单的模式进行渲染显示 若屏幕分辨率低于128*32 则强制启用文本菜单模式
    if (!MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].a || SCREEN_ROW <= 32) {

        //分别获取 菜单层、菜单项 索引值
        //int id = Get_Menu_Id(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].id, MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x);
        //int Pos_Id = Get_Menu_Id(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].id, MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x + Slide_space[1].x);
        //显示菜单项目名::这里有两行文字是在屏幕外 用于动过渡动画
        for (int i = -1;i < SCREEN_PAGE_NUM / 2 + 1;i++) {
            if (MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x + i <= MenuLevel[real_Level_Id].max) {

                //绘制目录树
                if (Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].x != 2) {
                    //Set_Font_Size(2);
                    Disp.drawUTF8(0, (1 - Menu_Smooth_Animation[3].x * (i != -1)) * ((i + Menu_Smooth_Animation[0].x) * 16 + 1), Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].x == 0 ? "+" : "-");
                }
                //绘制目录名
                Draw_Utf(7 * (Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].x != 2), (1 - Menu_Smooth_Animation[3].x * (i != -1)) * ((i + Menu_Smooth_Animation[0].x) * 16 + 1), Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].name);

                //对特殊菜单控件的分类渲染
                switch (Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].x) {

                    //开关控件
                case 3:
                    Draw_Utf(SCREEN_COLUMN - 32 - 1, (i + Menu_Smooth_Animation[0].x) * 16 + 1, Switch_space[Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].a] ? "开启" : "关闭");
                    break;

                    //滑动条
                case 4:
                    //Disp.setDrawColor(1);
                    char buffer[5];
                    //Set_Cursor(SCREEN_COLUMN - 9 - Get_Font_Size() * 6 * Get_Dec_Deep(Slide_space[Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].a].x), (int)((i + Menu_Smooth_Animation[0].x) * 16) + 3);
                    sprintf(buffer,"%d", Slide_space[Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].a].x);
                    Draw_Utf(SCREEN_COLUMN - 9 - 6 * Get_Dec_Deep(Slide_space[Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].a].x), \
                        (int)((i + Menu_Smooth_Animation[0].x) * 16), \
                        buffer);
                    break;

                    //单选框
                case 5:
                    if ((Switch_space[Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].a] == Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].b)) {
                        // Draw_Slow_Bitmap(SCREEN_COLUMN - 32 - 1 + DEBUG_3, \
                        //     (i + Menu_Smooth_Animation[0].x) * 16 + 2, \
                        //     CheckBoxSelection, \
                        //     10, 10);
                    } else {
                        Disp.drawFrame(SCREEN_COLUMN - 32 - 1 + DEBUG_3, \
                            (i + Menu_Smooth_Animation[0].x) * 16 + 2, \
                            10, 10);
                    }
                    //当前项高亮
                    if (Slide_space[1].x == i) {
                        Disp.setDrawColor(2);
                        Disp.drawBox(SCREEN_COLUMN - 32 - 2 + DEBUG_3, \
                            (i + Menu_Smooth_Animation[0].x) * 16 + 1, \
                            12, 12);
                        Disp.setDrawColor(1);
                    }
                
                    break;
                default:
                    break;
                }

            }
        }

        //绘制滚动条
        Draw_Scale(SCREEN_COLUMN - RollingStripWidth, 0, RollingStripWidth, SCREEN_ROW - 1, MenuLevel[real_Level_Id].max + 1, map(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x + Slide_space[1].x, 0, MenuLevel[real_Level_Id].max + 1, -Menu_Smooth_Animation[1].x * (SCREEN_ROW / (MenuLevel[real_Level_Id].max + 1)), SCREEN_ROW - 1));

        //显示页码角标
        Page_Footnotes(MenuLevel[real_Level_Id].x + 1 + Slide_space[1].x, MenuLevel[real_Level_Id].max + 1);

        //反色高亮被选项
        Disp.setDrawColor(2);
        Disp.drawRBox(0, \
                            (Slide_space[1].x - Menu_Smooth_Animation[1].x) * 16, \
                            Switch_space[SwitchSpace_OptionStripFixedLength]?DEBUG_4:(Get_UTF8_Ascii_Pix_Len(1,Menu[Pos_Id].name) - Menu_Smooth_Animation[2].x + 12 * (Menu[Pos_Id].x != 2) + 1), \
                            CNSize + 2 , \
                            0);
        Disp.setDrawColor(1);

        //编码器控制页内选择框滚动选择
        Slide_space[1].x = sys_Counter_Get() - 1;
        if (Slide_space[1].x >= Slide_space[1].max - 1) {
            MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x++;
            sys_Counter_SetVal(Slide_space[1].max - 1);
        }
        else if (Slide_space[1].x <= -1 && !(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x == 0 && sys_Counter_Get()!= 1)) {
            MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x--;
            sys_Counter_SetVal(1);
        }
        Slide_space[1].x = constrain(Slide_space[1].x, 0, min((int)Slide_space[1].max - 2, (int)MenuLevel[real_Level_Id].max));
        MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x = constrain(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x, \
                                                                    MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].min, \
            (MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].max > Slide_space[1].max - 2)?(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].max - (Slide_space[1].max - 2)):0);

        //更新过渡动画
        real_Level_Id = Get_Real_Menu_Level_Id(MenuLevelId);
        Pos_Id = Get_Menu_Id(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].id, MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x + Slide_space[1].x);
        Menu_Smooth_Animation[0].val = MenuLevel[real_Level_Id].x;
        Menu_Smooth_Animation[1].val = MenuLevel[real_Level_Id].x + Slide_space[1].x;
        Menu_Smooth_Animation[2].val = Get_UTF8_Ascii_Pix_Len(1,Menu[Pos_Id].name);

    }
    else {
        /*  当前菜单使用了图标化的渲染方式 该模式仅支持128*64的屏幕 若宏定义中选择了128*32的屏幕将自动切换为普通文本模式显示菜单
            接受两种尺寸的图标 14x14(推荐) 和 48*48 （不推荐）
            如果为14x14在128*64屏幕中会自动放大到48*48
        */
        int id = Get_Menu_Id(MenuLevel[real_Level_Id].id, MenuLevel[real_Level_Id].x);
        int Pos_Id;

        //居中显示项目名
        Draw_Utf(UTF8_HMiddle(0,128,1,Menu[id].name), 50, Menu[id].name);
        
        
        for (uint8_t i = 0; i < 5; i++) {
            Pos_Id = Get_Menu_Id(MenuLevel[real_Level_Id].id, MenuLevel[real_Level_Id].x + i - 2);

            if (MenuLevel[real_Level_Id].x - 2 + i >= 0 && MenuLevel[real_Level_Id].x - 2 + i <= MenuLevel[real_Level_Id].max) {
                //绘制菜单项目图标
                if (Menu[id].x != 2) {
                    if (Menu[Pos_Id].x != 2) {
                        Draw_APP((1 - Menu_Smooth_Animation[3].x * (i != -1))*(-69 + i * 56 + Menu_Smooth_Animation[0].x * 56), 3, Menu[Pos_Id].icon);
                    }
                }
            }
        }

        // Serial.print("x1:");
        // Serial.print(MenuLevel[real_Level_Id].x);

        MenuLevel[real_Level_Id].x = sys_Counter_Get();
        Menu_Smooth_Animation[0].val=MenuLevel[real_Level_Id].x;

        // Serial.print(" x2:");
        // Serial.print(MenuLevel[real_Level_Id].x);

        // Serial.print(" 编码器:");
        // Serial.println(sys_Counter_Get());
        
    }
    //编码器按下事件
    //菜单被选项激活 触发菜单被选项预设事件
    switch(sys_KeyProcess()) {
        case 1:
            Run_Menu_Id(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].id,MenuLevel[real_Level_Id].x +  Slide_space[1].x);
        break;
        case 2:
            Run_Menu_Id(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].id,MenuLevel[real_Level_Id].x);
        break;
        
        default:break;
    }
    
    Display();
}
