#include "OpenT12.h"
char BLE_name[20] = "OPEN-T12";
uint8_t BLE_State = false;

void BLE_Init(void) {

    if (!BLE_State) return;

    SerialBT.begin(BLE_name);
}

void BLE_Restart(void){
    SerialBT.end();
    BLE_Init();
}

void BLE_Rename(void) {
    TextEditor("蓝牙设备名", BLE_name);
    BLE_Restart();
}