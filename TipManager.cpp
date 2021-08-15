#include "OpenT12.h"

uint8_t TipID = 0;
uint8_t TipTotal = 1;

TIPCONFIG MyTip[MaxTipConfig] = {
    {{51.6128997803,0.1443523616,0.0000061750,-0.0000000001},{{30.0,0,0.5},{20.0,1,0.5}},"深圳K-1"},
};

/*** 
 * @description: 载入烙铁头配置到系统环境
 * @param {*}
 * @return {*}
 */
void LoadTipConfig(void) {
    //载入温度系数
    for (uint8_t i=0;i<4;i++) {
        PTemp[i] = MyTip[TipID].PTemp[i];
    }
    //载入PID参数
    aggKp = MyTip[TipID].PID[0][0];
    aggKi = MyTip[TipID].PID[0][1];
    aggKd = MyTip[TipID].PID[0][2];

    consKp = MyTip[TipID].PID[1][0];
    consKi = MyTip[TipID].PID[1][1];
    consKd = MyTip[TipID].PID[1][2];
    //载入名称
    TipName = MyTip[TipID].name;
}

/***
 * @description: 保存烙铁头配置到系统环境
 * @param {*}
 * @return {*}
 */
void SaveTipConfig(void) {
    //载入温度系数
    for (uint8_t i = 0;i < 4;i++) {
        MyTip[TipID].PTemp[i] = PTemp[i];
    }
    //载入PID参数
    MyTip[TipID].PID[0][0] = aggKp;
    MyTip[TipID].PID[0][1] = aggKi;
    MyTip[TipID].PID[0][2] = aggKd;

    MyTip[TipID].PID[1][0] = consKp;
    MyTip[TipID].PID[1][1] = consKi;
    MyTip[TipID].PID[1][2] = consKd;
    
    //由于名称是用指针传递，所以不需要重新赋值
}

/*** 
 * @description: 更新菜单配置列表
 * @param {*}
 * @return {*}
 */
void FlashTipMenu(void){
    uint8_t  MLID = Get_Real_Menu_Level_Id(15); //查询配置列表所在的菜单系统层级
    uint8_t  MID;

    MenuLevel[MLID].max = TipTotal;  //修改菜单最大项目数量

    for (uint8_t i=0;i<TipTotal;i++) {
        MID = Get_Menu_Id(MLID, i + (1));    //查询配置列表项目id,另外 +(1)是为了跳过标题
        Menu[MID].name = MyTip[i].name;    //刷新名称

    }
}
/*** 
 * @description: 新建配置
 * @param {*}
 * @return {*}
 */
void NewTipConfig(void) {
    if (TipTotal < MaxTipConfig) {

        // char NewName = "---未命名---";
        strcpy(MyTip[TipTotal].name, "NewTip");

        for (uint8_t i = 0;i < 4;i++) {
            MyTip[TipID].PTemp[i] = 0;
        }

        MyTip[TipID].PID[0][0] = 0;
        MyTip[TipID].PID[0][1] = 0;
        MyTip[TipID].PID[0][2] = 0;
        MyTip[TipID].PID[1][0] = 0;
        MyTip[TipID].PID[1][1] = 0;
        MyTip[TipID].PID[1][2] = 0;

        TipID = TipTotal;
        TipTotal++;
        Pop_Windows("新建成功");

        TipRename();
    }else Pop_Windows("达到上限");
}

/*** 
 * @description: 重命名当前的配置
 * @param {*}
 * @return {*}
 */
void TipRename(void) {
    Pop_Windows("双击保存 长按退出");
    delay(1000);
    char newName[20]={0};
    strcpy(newName, MyTip[TipID].name);

    uint8_t charCounter=0;      //光标指针
    char editChar = 'A';

    bool exitRenameGUI = false;
    bool editFlag = 0,lastEditFlag = 1;       //编辑器状态：0:选择要编辑的字符    1:选择ASCII

    uint8_t key=0;              //存储按键状态


    while (!exitRenameGUI) {

        //设置编码器
        if (editFlag != lastEditFlag){
            if (editFlag == 0) sys_Counter_Set(0, 19, 1, charCounter);
            else sys_Counter_Set(0, 255, 1, newName[charCounter]);

            lastEditFlag = editFlag;
        }

        //获取编码器输入
        switch (editFlag) {
            case 0: charCounter = sys_Counter_Get(); break;
            case 1: 
                editChar = sys_Counter_Get(); 
                newName[charCounter] = editChar;
            break;
        }

        //////////////////////////////////////////////////////////
        Clear();

        Disp.setDrawColor(1);
        //第一行显示原来的名字
        Disp.drawUTF8(0,1,MyTip[TipID].name);
        //第二行显示新的名字
        Disp.drawUTF8(0, 12 + 1, newName);

        

        //显示当前选中的ASCII
        Disp.setDrawColor(1);
        Disp.setFont(u8g2_font_logisoso26_tf);

        Disp.setCursor(0, 34);
        Disp.printf("%c", newName[charCounter]);
        
        Disp.setCursor(32, 34);
        Disp.printf("0X%02X", newName[charCounter]);
        
        Disp.setFont(u8g2_font_wqy12_t_gb2312);

        //反色显示光标
        Disp.setDrawColor(2);
        Disp.drawBox(charCounter * 6, 12, 6, 12);
        Disp.drawBox(0,32,32,32);

        Display();

        //////////////////////////////////////////////////////////

        //处理按键事件
        key = sys_KeyProcess();
        switch (key) {
            //单击切换编辑器状态
            case 1: editFlag = !editFlag; break;

            //双击：保存并退出      长按：不保存退出
            case 3:
                strcpy(MyTip[TipID].name,newName);
                Pop_Windows("已保存");
            case 2:
                exitRenameGUI = true;
            break;
        }
    }

    
}

/*** 
 * @description: 删除配置
 * @param {*}
 * @return {*}
 */
void TipDel(void) {
    if (TipTotal > 1) {
        //覆盖并移动配置
        for (uint8_t i=TipID;i<TipTotal-1;i++) {
            MyTip[i] = MyTip[i+1];
        }
        TipTotal--;
        Pop_Windows("删除成功");
    }else Pop_Windows("这是最后一个配置");
    
}