#include "OpenT12.h"
extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C Disp;


//菜单系统状态 0:自动退出 1:运行菜单系统
uint8_t Menu_System_State = 0;

//菜单层id 菜单项目id
uint8_t MenuLevelId = 0, LastMenuLevelId =0, MenuId = 0; //标注值
int32_t real_Level_Id,Pos_Id,Back_Id=-1; //索引值


//全局变量 控制右下角角标显示时间
uint32_t pages_Tip_Display_timer = 0;
//默认无动作后的1500ms关闭悬浮角标显示
#define pages_Tip_Display_Timeout 1500

/*/////////////////////////////////////////////////////////////////////

    @需要手动设置的项目

*//////////////////////////////////////////////////////////////////////
int     MenuScroll = 0;


//由于sizeof的原理是在编译阶段由编译器替换数组大小，因此我们无法计算指针的sizeof！需要在位图的第一个字节填写 n 阶矩阵图像
/*
    @变量组 *Switch_space[]
    @brief 开关控件 用于存储开关控件的值
    @param -
    
    @注册
        -0->平滑动画
        -1->单选框测试
        -2->选项条固定长度
*/
enum Switch_space_Obj{
    SwitchSpace_SmoothAnimation = 0,
    SwitchSpace_OptionStripFixedLength,

    SwitchSpace_PIDMode,
    SwitchSpace_PanelSettings,
    SwitchSpace_ScreenFlip,
    SwitchSpace_Volume,
    SwitchSpace_RotaryDirection,
    SwitchSpace_HandleTrigger,
    SwitchSpace_Language,

};
uint8_t *Switch_space[] = {
    &SmoothAnimation_Flag,
    &OptionStripFixedLength_Flag,

    &PIDMode,
    &PanelSettings,
    &ScreenFlip,
    &Volume,
    &RotaryDirection,
    &HandleTrigger,
    &Language,
};

/*
    @结构体 Slide_Bar Slide_space[]
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
enum Slide_space_Obj{
    Slide_space_ScreenBrightness = 0,
    Slide_space_Scroll,

    Slide_space_BootTemp,
    Slide_space_SleepTemp,
    Slide_space_BoostTemp,

    Slide_space_ShutdownTime,
    Slide_space_SleepTime,
    Slide_space_BoostTime,

    Slide_space_UndervoltageAlert,
    Slide_space_BootPasswd,

};
struct Slide_Bar Slide_space[] = {
    {&ScreenBrightness,0,255,16}, //亮度调整
    {&MenuScroll,0,SCREEN_ROW / 16,1}, //自适应菜单滚动范围

    {&BootTemp,150,400,5},
    {&SleepTemp,150,400,5},
    {&BoostTemp,0,150,1},

    {&ShutdownTime,0,60,1},
    {&SleepTime,0,60,1},
    {&BoostTime,0,600,1},

    {&UndervoltageAlert,0,36,1},
    {&BootPasswd,0,999,1},
};

/*
    @结构体 Smooth_Animation Menu_Smooth_Animation[]
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
#define Smooth_Animation_Num sizeof(Menu_Smooth_Animation)/sizeof(Menu_Smooth_Animation[0])
struct Smooth_Animation Menu_Smooth_Animation[] = {
    {0,0,0,0.4,1,0}, //菜单项目滚动动画
    {0,0,0,0.3,1,0}, //滚动条平滑动画
    {0,0,0,0.3,1,0}, //菜单选项条长度平滑动画
    {0,0,0,0.4,0,0}, //项目归位动画
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

#define Menu_NULL_IMG 0
#define Menu_HAVE_IMG 1

struct Menu_Level_System MenuLevel[] = {
    {0,0,0,3,Menu_NULL_IMG},
    {1,0,0,5,Menu_HAVE_IMG},
    {2,0,0,6,Menu_HAVE_IMG},
    {3,0,0,4,Menu_HAVE_IMG},
    {4,0,0,4,Menu_HAVE_IMG},
    {5,0,0,6,Menu_HAVE_IMG},
    {6,0,0,5,Menu_HAVE_IMG},
    {7,0,0,6,Menu_HAVE_IMG},
    {8,0,0,2,Menu_HAVE_IMG},
    {9,0,0,9,Menu_NULL_IMG},
    {10,0,0,2,Menu_HAVE_IMG},
    {11,0,0,2,Menu_HAVE_IMG},
    {12,0,0,2,Menu_HAVE_IMG},
    {13,0,0,1,Menu_HAVE_IMG},
    {14,0,0,2,Menu_HAVE_IMG},
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
#define Menu_NULL_F                 0

#define Jump_Menu_Op                0
#define F_Menu_Op                   1
#define Title_Menu_Op               2
#define Switch_Menu_Op              3
#define Progress_Bar_Menu_Op        4
#define SingleBox_Menu_Op           5
#define Menu_NULL_OP                6

struct Menu_System Menu[] = {
    {0,0,       Title_Menu_Op,         "欢迎使用[朱雀]控制器",  Menu_NULL_IMG,              0,                                  0,          Menu_NULL_F},
    {0,1,       Jump_Menu_Op,          "此焊台",               Menu_NULL_IMG,              1,                                  0,          Menu_NULL_F},
    {0,2,       Jump_Menu_Op,          "此系统",               Menu_NULL_IMG,              5,                                  0,          Menu_NULL_F},
    {0,3,       F_Menu_Op,          "返回",               Menu_NULL_IMG,                0,                                  0,          *Save_Exit_Menu_System},

    {1,0,       Title_Menu_Op,         "此焊台",               Menu_NULL_IMG,              0,                                  1,          Menu_NULL_F},
    {1,1,       Jump_Menu_Op,          "烙铁头",               IMG_Tip,              2,                                  0,          Menu_NULL_F},
    {1,2,       Jump_Menu_Op,          "温度场景",             Set1,              3,                                  0,          Menu_NULL_F},
    {1,3,       Jump_Menu_Op,          "定时场景",             Set2,              4,                                  0,          Menu_NULL_F},
    {1,4,       Jump_Menu_Op,          "温控模式",                 Set3,              12,                                  0,          Menu_NULL_F},
    {1,5,       Jump_Menu_Op,          "返回",                Set7,              0,                                  1,          Menu_NULL_F},

    {2,0,       Title_Menu_Op,         "烙铁头管理",               Menu_NULL_IMG,              1,                                  1,          Menu_NULL_F},
    {2,1,       Jump_Menu_Op,          "切换配置",           Set8,              0,                                  0,          Menu_NULL_F},
    {2,2,       Jump_Menu_Op,          "校准",               Set9,              0,                                  0,          *CalibrationTemperature},
    {2,3,       Jump_Menu_Op,          "新建",               IMG_Files,              0,                                  0,          Menu_NULL_F},
    {2,4,       Jump_Menu_Op,          "重命名",             IMG_Pen2,              0,                                  0,          Menu_NULL_F},
    {2,5,       Jump_Menu_Op,          "删除",               Set10,              0,                                  0,          Menu_NULL_F},
    {2,6,       Jump_Menu_Op,          "保存",               Save,              1,                                  1,          Menu_NULL_F},

    {3,0,       Title_Menu_Op,         "温度场景",               Menu_NULL_IMG,              1,                                  2,          Menu_NULL_F},
    {3,1,       Progress_Bar_Menu_Op,  "启动温度",         Set13,              Slide_space_BootTemp,                                  0,          Menu_NULL_F},
    {3,2,       Progress_Bar_Menu_Op,  "休眠温度",         Set11,              Slide_space_SleepTemp,                                  0,          Menu_NULL_F},
    {3,3,       Progress_Bar_Menu_Op,  "提温温度",         Set14,              Slide_space_BoostTemp,                                  0,          Menu_NULL_F},
    {3,4,       Jump_Menu_Op,          "保存",                  Save,              1,                                  2,          Menu_NULL_F},

    {4,0,       Title_Menu_Op,         "定时场景",               Menu_NULL_IMG,              1,                                  3,          Menu_NULL_F},
    {4,1,       Progress_Bar_Menu_Op,  "停机时间(分)",         Set13,              Slide_space_ShutdownTime,                                               0,          Menu_NULL_F},
    {4,2,       Progress_Bar_Menu_Op,  "休眠时间(分)",         Set11,              Slide_space_SleepTime,                                                0,          Menu_NULL_F},
    {4,3,       Progress_Bar_Menu_Op,  "提温时长(秒)",         Set14,              Slide_space_BoostTime,                                                0,          Menu_NULL_F},
    {4,4,       Jump_Menu_Op,          "保存",                  Save,              1,                                  3,          Menu_NULL_F},

    {5,0,       Title_Menu_Op,         "此系统",               Menu_NULL_IMG,              0,                                  2,          Menu_NULL_F},
    {5,1,       Jump_Menu_Op,          "个性化",               IMG_Pen,              6,                                  0,          Menu_NULL_F},
    {5,2,       Progress_Bar_Menu_Op,  "欠压提醒",             Set6,              Slide_space_UndervoltageAlert,                    0,          Menu_NULL_F},
    {5,3,       Progress_Bar_Menu_Op,  "开机密码",             Lock,              Slide_space_BootPasswd,              0,          Menu_NULL_F},
    {5,4,       Jump_Menu_Op,          "语言设置",             Set_LANG,              13,                                  0,          Menu_NULL_F},
    {5,5,       Jump_Menu_Op,          "关于朱雀",             QRC,              0,                                  0,          Menu_NULL_F},
    {5,6,       Jump_Menu_Op,          "返回",                 Set7,              0,                                  2,          Menu_NULL_F},

    {6,0,       Title_Menu_Op,         "个性化",               Menu_NULL_IMG,              5,                                  1,          Menu_NULL_F},
    {6,1,       Jump_Menu_Op,          "显示效果",             Set4,              7,                                  0,          Menu_NULL_F},
    {6,2,       Jump_Menu_Op,          "声音设置",             Set5,              10,                                  0,          Menu_NULL_F},
    {6,3,       Switch_Menu_Op,        "编码器方向",            Set19,           SwitchSpace_RotaryDirection,         0,          *PopMsg_RotaryDirection},
    {6,4,       Jump_Menu_Op,          "手柄触发",             IMG_Trigger,             14,         0,          Menu_NULL_F},
    {6,5,       Jump_Menu_Op,          "返回",                 Set7,              5,                                  1,          Menu_NULL_F},

    {7,0,       Title_Menu_Op,         "显示效果",               Menu_NULL_IMG,              6,                        1,          Menu_NULL_F},
    {7,1,       Jump_Menu_Op,          "面板设置",               Set0,              8,                                  0,          Menu_NULL_F},
    {7,2,       Switch_Menu_Op,        "翻转屏幕",               IMG_Flip,          SwitchSpace_ScreenFlip,               0,        *Update_OLED_Flip},
    {7,3,       Jump_Menu_Op,          "过渡动画",               IMG_Animation,    11,                                  0,          Menu_NULL_F},
    {7,4,       Progress_Bar_Menu_Op,  "屏幕亮度",               IMG_Sun,          Slide_space_ScreenBrightness,        1,          *Update_OLED_Light_Level},
    {7,5,       Jump_Menu_Op,          "选项条定宽",             IMG_Size,              9,                                  0,          Menu_NULL_F},
    {7,6,       Jump_Menu_Op,          "返回",                  Set7,              6,                                  1,          Menu_NULL_F},

    {8,0,       Title_Menu_Op,         "面板设置",               Menu_NULL_IMG,              7,                                  1,          Menu_NULL_F},
    {8,1,       SingleBox_Menu_Op,     "简约",               Set17,              SwitchSpace_PanelSettings,                     0,          *JumpWithTitle},
    {8,2,       SingleBox_Menu_Op,     "详细",               Set18,              SwitchSpace_PanelSettings,                     1,          *JumpWithTitle},

    {9,0,       Title_Menu_Op,         "选项条定宽设置&测试",      Menu_NULL_IMG,              7,                                  5,          Menu_NULL_F},
    {9,1,       SingleBox_Menu_Op,     "固定",                   Menu_NULL_IMG,              SwitchSpace_OptionStripFixedLength, true,          Menu_NULL_F},
    {9,2,       SingleBox_Menu_Op,     "自适应",                 Menu_NULL_IMG,              SwitchSpace_OptionStripFixedLength, false,          Menu_NULL_F},
    {9,3,       Jump_Menu_Op,          "--- 往下翻 ---",         Menu_NULL_IMG,              9,                                  4,          Menu_NULL_F},
    {9,4,       Menu_NULL_OP,          "人民!",                   Menu_NULL_IMG,              0,                                  0,          Menu_NULL_F},
    {9,5,       Menu_NULL_OP,          "只有人民~",               Menu_NULL_IMG,              0,                                  0,          Menu_NULL_F},
    {9,6,       Menu_NULL_OP,          "才是创造世界历史的",      Menu_NULL_IMG,              0,                                  1,          Menu_NULL_F},
    {9,7,       Menu_NULL_OP,          "动 力！",                   Menu_NULL_IMG,              0,                                  0,          Menu_NULL_F},
    {9,8,       Jump_Menu_Op,          "--- 往上翻 ---",         Menu_NULL_IMG,              9,                                  0,          Menu_NULL_F},
    {9,9,       Jump_Menu_Op,          "返回",                   Menu_NULL_IMG,              7,                                  5,          Menu_NULL_F},

    {10,0,       Title_Menu_Op,         "声音设置",               Menu_NULL_IMG,              6,                                  2,          Menu_NULL_F},
    {10,1,       SingleBox_Menu_Op,     "开启",               Set5,              SwitchSpace_Volume,                              true,          *JumpWithTitle},
    {10,2,       SingleBox_Menu_Op,     "关闭",               Set5_1,              SwitchSpace_Volume,                            false,          *JumpWithTitle},

    {11,0,       Title_Menu_Op,         "动画设置",               Menu_NULL_IMG,              7,                                  3,          Menu_NULL_F},
    {11,1,       SingleBox_Menu_Op,     "开启",               IMG_Animation,                  SwitchSpace_SmoothAnimation,        true,          *JumpWithTitle},
    {11,2,       SingleBox_Menu_Op,     "关闭",               IMG_Animation_DISABLE,          SwitchSpace_SmoothAnimation,        false,          *JumpWithTitle},

    {12,0,       Title_Menu_Op,         "温控模式",               Menu_NULL_IMG,              1,                                  4,          Menu_NULL_F},
    {12,1,       SingleBox_Menu_Op,     "PID控制",               Set16,               SwitchSpace_PIDMode,                        true,          *JumpWithTitle},
    {12,2,       SingleBox_Menu_Op,     "模糊控制",              Set15,      SwitchSpace_PIDMode,                                 false,          *JumpWithTitle},

    {13,0,       Title_Menu_Op,         "语言设置",               Menu_NULL_IMG,              5,                                  4,          Menu_NULL_F},
    {13,1,       SingleBox_Menu_Op,     "简体中文",               Lang_CN,               SwitchSpace_Language,                    LANG_Chinese,          *JumpWithTitle},

    {14,0,       Title_Menu_Op,         "手柄触发",               Menu_NULL_IMG,              6,                                  4,          Menu_NULL_F},
    {14,1,       SingleBox_Menu_Op,     "震动开关",               IMG_VibrationSwitch,        SwitchSpace_HandleTrigger,          0,          *JumpWithTitle},
    {14,2,       SingleBox_Menu_Op,     "干簧管",                 IMG_ReedSwitch,             SwitchSpace_HandleTrigger,          1,          *JumpWithTitle},




};
int deg=0;
void System_UI_Init(void) {
    sys_Counter_Set(150, 400, 5, 320);
}
//系统UI
void System_UI(void) {
    Clear();

    char buffer[50];
    for (uint8_t i = 0;i < 5;i++) {
        Disp.setCursor(0, 12 * i + 1);

        switch (i) {
        case 0: sprintf(buffer, "状态:%s 控温:%s", TempCTRL_Status_Mes[TempCTRL_Status], (PIDMode==1)?"PID":"模糊"); break;
        case 1: sprintf(buffer, "设定%.0lf°C 当前%.0lf°C", PID_Setpoint,TipTemperature); break;
        case 2: sprintf(buffer, "ADC:%d PID:%.0lf", LastADC, PID_Output); break;
        case 3: sprintf(buffer, "E:%.2lf V2:%.2lf", Get_MainPowerVoltage(), ESP32_ADC2Vol(analogRead(POWER_ADC_PIN))); break;
        case 4: sprintf(buffer, "%.3lf %.3lf %.3lf", aggKp, aggKi, aggKd); break;
        }
        Disp.print(buffer);
    }
    Display();

    printf("ERROREvent:%d 状态:%s \n", ERROREvent, TempCTRL_Status_Mes[TempCTRL_Status]);

    static uint8_t count = 0;
    //编码器长按按键进入菜单
    if (sys_KeyProcess() == 2) {
        //关闭功率管输出
        SetPOWER(0);
        //初始化菜单
        MenuLevel[0].x = 0;  //复位第一层菜单的位置
        *Slide_space[Slide_space_Scroll].x = 0;//复位第一层菜单的位置
        Next_Menu();
        while (Menu_System_State) Menu_Control();
        //退出菜单后重新初始化主界面
        System_UI_Init();
    }
}

/*/////////////////////////////////////////////////////////////////////

    @自定义功能函数

*//////////////////////////////////////////////////////////////////////
/*
    @函数 Update_OLED_Light_Level
    @brief 更新屏幕亮度设置
    @param -

*/
void Update_OLED_Light_Level(void) {
    Disp.sendF("c",0x81);  //向SSD1306发送指令：设置内部电阻微调
    Disp.sendF("c",(uint8_t)*Slide_space[Slide_space_ScreenBrightness].x); //微调范围（0-255）
}

void Update_OLED_Flip(void) {
    Disp.setFlipMode(ScreenFlip);
    PopMsg_ScreenFlip();
}

void PopMsg_BootPasswd(void) {
    char buffer[20];
    sprintf(buffer, "密码:%d", BootPasswd);
    Pop_Windows(buffer);
    delay(500);
}

void PopMsg_RotaryDirection(void) {
    char buffer[20];
    sprintf(buffer, "编码器:%s", (RotaryDirection==1)?"顺时针":"逆时针");
    Pop_Windows(buffer);
    delay(500);
}

void PopMsg_ScreenFlip(void) {
    char buffer[20];
    sprintf(buffer, "%s", (ScreenFlip == 1) ? "翻转显示" : "正常显示");
    Pop_Windows(buffer);
    delay(500);
}

void SYS_About(void) {
    Disp.clearBuffer();
    Text_Reader("2019大学生电子设计竞赛Ｆ题纸张计数器\n程序：赖浩文\n硬件：周孜宁\n辅助：张家穗");
    Display();
}


/*/////////////////////////////////////////////////////////////////////

    @相关工具函数

*//////////////////////////////////////////////////////////////////////
#define RollingStripWidth 3

/*复选框选中 10*10*/
uint8_t CheckBoxSelection[] = { 0xff,0xc0,0x80,0x40,0x80,0xc0,0x81,0xc0,0x81,0xc0,0x83,0x40,0x9b,0x40,0x8e,0x40,0x86,0x40,0xff,0xc0 };

void Save_Exit_Menu_System() {
    Pop_Windows("保存中 请勿切断电源");

    //保存图标
    Draw_Slow_Bitmap_Resize(128 - 28 -4, 64 - 28 -4, Save + 1, Save[0], Save[0], 28, 28);
    Display();

    //这里放读写flash的代码
    delay(1000);

    Exit_Menu_System();
}

void Exit_Menu_System() {
    Menu_System_State = 0;
    //过渡离开
    Disp.setDrawColor(0);
    Blur(0, 0, SCREEN_COLUMN, SCREEN_ROW, 4, 66 * *Switch_space[SwitchSpace_SmoothAnimation]);
    Disp.setDrawColor(1);
}

//按照标题进行跳转 标题跳转 跳转标题
void JumpWithTitle(void) {
    Run_Menu_Id(MenuLevelId, 0);
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
    int len = strlen(s)/2;
    int Colunm_GBK_Char = SCREEN_COLUMN / CNSize;
    int Row_GBK_Char = SCREEN_ROW / CNSize;
    int pages = len / (Colunm_GBK_Char * Row_GBK_Char);
    int val_Pages = 0, last_Pages = pages + 1;
    char page_s[(SCREEN_COLUMN / CNSize) * 2 + 1];
    int now_pow;

    //重置页码角标显示时间
    pages_Tip_Display_timer = millis();

    //初始化编码器设置
    sys_Counter_Set(0, pages, 1, 0);
    while (!sys_KeyProcess()) {

        //获取输入
        val_Pages = sys_Counter_Get();
        //只有翻页时才刷新屏幕 节省资源::这里页码超时加了100ms是为了超时后可以Display刷新一下屏幕
        if (val_Pages != last_Pages || (millis() - pages_Tip_Display_timer < pages_Tip_Display_Timeout + 100)) {
            last_Pages = val_Pages;
            Disp.clearBuffer();

            //自适应屏幕高度
            for (int i = 0;i < Row_GBK_Char;i++) {
                //计算当前行第一个字符地址
                now_pow = (val_Pages * Colunm_GBK_Char * Row_GBK_Char + Colunm_GBK_Char * i) * 2;
                for (int i = 0;i < sizeof(page_s);i++) {
                    if (now_pow + i < strlen(s) && i < sizeof(page_s) - 1)
                        page_s[i] = s[now_pow + i];
                    else {
                        page_s[i] = '\0';
                        break;
                    }
                }
                //显示当前行Utf8字符
                Draw_Utf(0, i * (CNSize + 1), page_s);
            }
            //绘制滚动条
            Draw_Scale(SCREEN_COLUMN - 5, 0, 5, SCREEN_ROW - 1, pages + 1, map(val_Pages, 0, pages, 0, SCREEN_ROW - SCREEN_ROW /2- 1));
            //绘制页码下标
            Page_Footnotes(val_Pages + 1, pages + 1);
            Display();
        }
        //ESP.wdtFeed();
    }
    //延迟 防止短时间多次触发
    delay(50);
    //初始化菜单控件
    //Next_Menu();
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

/*** 
 * @description: 菜单系统设置编码器
 * @param {*}
 * @return {*}
 */
void MenuSYS_SetCounter() {
    if (!MenuLevel[real_Level_Id].a || SCREEN_ROW <= 32) {
        //设置编码器滚动范围
        sys_Counter_Set(Slide_space[Slide_space_Scroll].min, Slide_space[Slide_space_Scroll].max + 1, 1, *Slide_space[Slide_space_Scroll].x);
    } else {
        printf("Next_Menu:图标模式\n");
        if (Menu[Get_Menu_Id(real_Level_Id, 0)].x) MenuLevel[real_Level_Id].min = 1; //当前处在图标模式 如果目标层菜单的第一项为标题，则给予屏蔽
        sys_Counter_Set(MenuLevel[real_Level_Id].min, MenuLevel[real_Level_Id].max, 1, MenuLevel[real_Level_Id].x);
        *Slide_space[Slide_space_Scroll].x = 0;
    }
}
/*
    @函数 Next_Menu
    @brief 多级菜单跳转初始化参数
*/
void Next_Menu() {
    real_Level_Id = Get_Real_Menu_Level_Id(MenuLevelId);
    uint8_t Id = Get_Menu_Id(MenuLevel[real_Level_Id].id, 0);

    //设置编码器
    MenuSYS_SetCounter();

    if (*Switch_space[SwitchSpace_SmoothAnimation]) {
        if (LastMenuLevelId != MenuLevelId) {
            Disp.setDrawColor(0);
            Blur(0, 0, SCREEN_COLUMN, SCREEN_ROW, 4, 20 * *Switch_space[SwitchSpace_SmoothAnimation]);
            Disp.setDrawColor(1);
        }
        

        //项目归位动画
        Menu_Smooth_Animation[3].last = 0;
        Menu_Smooth_Animation[3].val = 1;
    }
    Menu_System_State = 1;

    printf("退出Next_Menu \n");
}

/*
    @函数 Pop_Windows
    @brief 自适应文本大小信息弹窗
    @param (char* s):字符串首地址
    
*/
void Pop_Windows(char* s) {
    // Disp.setCursor(0, 0);
    // Disp.print(s);
    // Display();
    //Set_Font_Size(2);
    int w = Get_UTF8_Ascii_Pix_Len(1,s) + 2;
    int h = 12;
    // for (int i = 5;i > 0;i--) {
    //     //Set_Font_Size(i);
    //     w = CNSize * Get_Max_Line_Len(s) * Get_Font_Size() / 2;
    //     //h = CNSize * Get_Font_Size() * Get_Str_Next_Line_O(s);
    //     if (w < SCREEN_COLUMN && h < SCREEN_ROW) break;
    // }
    int x = (SCREEN_COLUMN - w) / 2;
    int y = (SCREEN_ROW - h) / 2;

    Disp.setDrawColor(0);
    Blur(0, 0, SCREEN_COLUMN, SCREEN_ROW, 3, 66 * *Switch_space[SwitchSpace_SmoothAnimation]); //<=15FPS以便人眼察觉细节变化

    int ix = 0;
    for (int i = 1;i <= 10;i++) {
        //震荡动画
        if (*Switch_space[SwitchSpace_SmoothAnimation]) ix = (10 * cos((i * 3.14) / 2.0)) / i;

        Disp.setDrawColor(0);
        Blur(0, 0, SCREEN_COLUMN, SCREEN_ROW, 3, 0);
        Disp.drawFrame(x - 1 + ix, y - 3, w + 1, h + 3);
        Disp.setDrawColor(1);
        Disp.drawRBox(x + ix, y - 2, w, h + 2 ,2);
        Disp.setDrawColor(0);
        Draw_Utf(x + 1 + ix, y - 1, s);
        Disp.setDrawColor(1);
        Display();
        delay(20 * *Switch_space[SwitchSpace_SmoothAnimation]);
    }
    //Set_Font_Size(1);
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
    case 2:
        LastMenuLevelId = MenuLevelId; //决定是否播放转场动画
        MenuLevelId = Menu[Id].a;
        if (!MenuLevel[Menu[Id].a].a) { //如果当前菜单层没有开启了图表化显示则对子菜单选项定向跳转执行配置

            uint8_t ExcellentLimit = (MenuLevel[MenuLevelId].max + 1) - SCREEN_FONT_ROW; //(MenuLevel[MenuLevelId].max + 1)是为了从1开始计算
            uint8_t ExcellentMedian = (SCREEN_FONT_ROW/2); //注意：这里从1开始计数
            //计算最优显示区域
            if (Menu[Id].b==0) {
                //头只有最差显示区域
                MenuLevel[Menu[Id].a].x = 0;
                *Slide_space[Slide_space_Scroll].x = (1); //+(1) 是因为实际上计算会-1 ,这里要补回来
            }else if (Menu[Id].b > 0 && Menu[Id].b <= MenuLevel[MenuLevelId].max - ExcellentMedian) {
                //中部拥有绝佳的显示区域
                MenuLevel[Menu[Id].a].x = Menu[Id].b - 1;
                *Slide_space[Slide_space_Scroll].x = 1 + (1); //+(1) 是因为实际上计算会-1 ,这里要补回来
            }else{
                //靠后位置 以及 最差的尾部
                MenuLevel[Menu[Id].a].x = ExcellentLimit;
                *Slide_space[Slide_space_Scroll].x = Menu[Id].b - ExcellentLimit + (1); //+(1) 是因为实际上计算会-1 ,这里要补回来
            }
            // printf("MenuLevelId:%d\nMenuLevel[MenuLevelId].x:%d\n*Slide_space[Slide_space_Scroll].x:%d\n", MenuLevelId, MenuLevel[MenuLevelId].x, *Slide_space[Slide_space_Scroll].x);
        }else{
            //当前是图标模式
            MenuLevel[Menu[Id].a].x = Menu[Id].b;
        }
        //按需求跳转完成后执行函数
        if (Menu[Id].function) Menu[Id].function();
        Next_Menu();
        break;
    case 1:
        Pop_Windows("正在处理");
        if (Menu[Id].function) Menu[Id].function();
        sys_Counter_Set(Slide_space[Slide_space_Scroll].min, Slide_space[Slide_space_Scroll].max, 1, *Slide_space[Slide_space_Scroll].x + 1);
        break;
    case 3:
        *Switch_space[Menu[Id].a] = !*Switch_space[Menu[Id].a];
        if (Menu[Id].function) Menu[Id].function();
        break;
    case 4:
        //*Switch_space[Menu[Id].a]=!*Switch_space[Menu[Id].a];
        sys_Counter_Set(Slide_space[Menu[Id].a].min, Slide_space[Menu[Id].a].max, Slide_space[Menu[Id].a].step, *Slide_space[Menu[Id].a].x);
        Disp.setDrawColor(0);
        Blur(0, 0, SCREEN_COLUMN, SCREEN_ROW, 3, 11 * *Switch_space[SwitchSpace_SmoothAnimation]);
        Disp.setDrawColor(1);

        while (!sys_KeyProcess()) {
            Disp.setDrawColor(0);
            Disp.drawBox(SCREEN_COLUMN / 8 - 2, (SCREEN_ROW - 24) / 2 - 3, 3 * SCREEN_COLUMN / 4 + 4, 24 + 4);
            Disp.setDrawColor(1);

            Disp.drawRFrame(SCREEN_COLUMN / 8 - 3, (SCREEN_ROW - 24) / 2 - 4, 3 * SCREEN_COLUMN / 4 + 4, 24 + 6 ,2);

            *Slide_space[Menu[Id].a].x = sys_Counter_Get();
            Draw_Utf(SCREEN_COLUMN / 8, (SCREEN_ROW - 24) / 2, Menu[Id].name);
            Draw_Num_Bar(*Slide_space[Menu[Id].a].x, Slide_space[Menu[Id].a].min, Slide_space[Menu[Id].a].max, SCREEN_COLUMN / 8, (SCREEN_ROW - 24) / 2 + CNSize + 3, 3 * SCREEN_COLUMN / 4, 7, 1);
           
            Display();
            //当前滑动条为屏幕亮度调节 需要特殊设置对屏幕亮度进行实时预览
            if (Menu[Id].function) Menu[Id].function();
        }
        delay(50);

        //静音
        //PlayTones(0, 0, 0);

        //sys_Counter_Set(MenuLevel[real_Level_Id].min, MenuLevel[real_Level_Id].max, 1, MenuLevel[real_Level_Id].x);
        if (Menu[Id].function) Menu[Id].function();
        MenuSYS_SetCounter();
        break;
    
    case 5://单选模式
        *Switch_space[Menu[Id].a] = Menu[Id].b;
        if (Menu[Id].function) Menu[Id].function();
        break;
    }
}

void Draw_APP(int x, int y, char* bitmap) {
    Disp.setDrawColor(1);
    Disp.drawRBox(x - 3, y - 3, 42 + 6, 42 + 6, 4);
    Disp.setDrawColor(0);

    Disp.setDrawColor(1);
    Draw_Slow_Bitmap_Resize(x, y, bitmap + 1, bitmap[0], bitmap[0], 42, 42);
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
    //printf("MenuLevelId:%d\nMenuLevel[MenuLevelId].x:%d\n*Slide_space[Slide_space_Scroll].x:%d\n", MenuLevelId, MenuLevel[MenuLevelId].x, *Slide_space[Slide_space_Scroll].x);
    if (!Menu_System_State) return;
    Disp.clearBuffer();

    //计算过渡动画
    if (*Switch_space[SwitchSpace_SmoothAnimation]) Smooth_Animation_System();

    //分别获取 菜单层、菜单项 索引值
    real_Level_Id = Get_Real_Menu_Level_Id(MenuLevelId);
    Pos_Id = Get_Menu_Id(MenuLevel[real_Level_Id].id, MenuLevel[real_Level_Id].x + *Slide_space[Slide_space_Scroll].x);

    //若当前菜单层级没有开题图标化则使用普通文本菜单的模式进行渲染显示 若屏幕分辨率低于128*32 则强制启用文本菜单模式
    if (!MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].a || SCREEN_ROW <= 32) {

        //分别获取 菜单层、菜单项 索引值
        //int id = Get_Menu_Id(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].id, MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x);
        //int Pos_Id = Get_Menu_Id(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].id, MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x + *Slide_space[Slide_space_Scroll].x);
        //显示菜单项目名::这里有两行文字是在屏幕外 用于动过渡动画
        for (int i = -1;i < SCREEN_PAGE_NUM / 2 + 1;i++) {
            if (MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x + i >= 0 && MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x + i <= MenuLevel[real_Level_Id].max) {

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
                    Draw_Utf(SCREEN_COLUMN - 32 - 1, (i + Menu_Smooth_Animation[0].x) * 16 + 1, *Switch_space[Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].a] ? "开启" : "关闭");
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
                    if ((*Switch_space[Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].a] == Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].b)) {
                        Draw_Slow_Bitmap(SCREEN_COLUMN - 32 - 1 + DEBUG_3, \
                            (i + Menu_Smooth_Animation[0].x) * 16 + 2, \
                            CheckBoxSelection, \
                            10, 10);
                    } else {
                        Disp.drawFrame(SCREEN_COLUMN - 32 - 1 + DEBUG_3, \
                            (i + Menu_Smooth_Animation[0].x) * 16 + 2, \
                            10, 10);
                    }
                    //当前项高亮
                    if (*Slide_space[Slide_space_Scroll].x == i) {
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
        Draw_Scale(SCREEN_COLUMN - RollingStripWidth, 0, RollingStripWidth, SCREEN_ROW - 1, MenuLevel[real_Level_Id].max + 1, map(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x + *Slide_space[Slide_space_Scroll].x, 0, MenuLevel[real_Level_Id].max + 1, -Menu_Smooth_Animation[1].x * (SCREEN_ROW / (MenuLevel[real_Level_Id].max + 1)), SCREEN_ROW - 1));

        //显示页码角标
        Page_Footnotes(MenuLevel[real_Level_Id].x + 1 + *Slide_space[Slide_space_Scroll].x, MenuLevel[real_Level_Id].max + 1);

        //反色高亮被选项
        Disp.setDrawColor(2);
        Disp.drawRBox(0, \
                            (*Slide_space[Slide_space_Scroll].x - Menu_Smooth_Animation[1].x) * 16, \
                            *Switch_space[SwitchSpace_OptionStripFixedLength]?DEBUG_4:(Get_UTF8_Ascii_Pix_Len(1,Menu[Pos_Id].name) - Menu_Smooth_Animation[2].x + 12 * (Menu[Pos_Id].x != 2) + 1), \
                            CNSize + 2 , \
                            0);
        Disp.setDrawColor(1);

        //项目滚动处理
        *Slide_space[Slide_space_Scroll].x = sys_Counter_Get() - 1;
        if (*Slide_space[Slide_space_Scroll].x >= Slide_space[Slide_space_Scroll].max) {
            MenuLevel[real_Level_Id].x++;
            sys_Counter_SetVal(Slide_space[Slide_space_Scroll].max);
        }else if (*Slide_space[Slide_space_Scroll].x <= -1) {
            MenuLevel[real_Level_Id].x--;
            sys_Counter_SetVal(1);
        }
        //编码器控制页内选择框滚动选择
        //CountMax = constrain(MenuLevel[real_Level_Id].max - MenuLevel[real_Level_Id].x + 1, 0, 7);
        *Slide_space[Slide_space_Scroll].x = constrain(*Slide_space[Slide_space_Scroll].x, 0, Slide_space[Slide_space_Scroll].max - 1);

        // *Slide_space[Slide_space_Scroll].x = constrain(*Slide_space[Slide_space_Scroll].x, 0, min((int)Slide_space[Slide_space_Scroll].max - 2, (int)MenuLevel[real_Level_Id].max));
        MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x = constrain(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x, \
                                                                    MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].min, \
            (MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].max > Slide_space[Slide_space_Scroll].max - 1)?(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].max - (Slide_space[Slide_space_Scroll].max - 1)):0);

        //更新过渡动画
        real_Level_Id = Get_Real_Menu_Level_Id(MenuLevelId);
        Pos_Id = Get_Menu_Id(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].id, MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x + *Slide_space[Slide_space_Scroll].x);
        Menu_Smooth_Animation[0].val = MenuLevel[real_Level_Id].x;
        Menu_Smooth_Animation[1].val = MenuLevel[real_Level_Id].x + *Slide_space[Slide_space_Scroll].x;
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
        case 3:
            //单击和双击则执行当前项目
            Run_Menu_Id(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].id,MenuLevel[real_Level_Id].x +  *Slide_space[Slide_space_Scroll].x);
        break;
        case 2:
            //长按执行 标题跳转
            Run_Menu_Id(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].id,0);
        break;
        
        default:break;
    }
    
    Display();
}
