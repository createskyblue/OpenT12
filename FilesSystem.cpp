#include "OpenT12.h"

void listDir(fs::FS& fs, const char* dirname, uint8_t levels) {
    printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if (!root) {
        printf("- failed to open directory\n");
        return;
    }
    if (!root.isDirectory()) {
        printf(" - not a directory\n");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            printf("  DIR : \n");
            printf(file.name());
            if (levels) {
                listDir(fs, file.name(), levels - 1);
            }
        }
        else {
            printf("  FILE: \n");
            printf(file.name());
            printf("\tSIZE: \n");
            printf(file.size());
        }
        file = root.openNextFile();
    }
}

void readFile(fs::FS& fs, const char* path) {
    printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if (!file || file.isDirectory()) {
        printf("- failed to open file for reading\n");
        return;
    }

    printf("- read from file:\n");
    while (file.available()) {
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS& fs, const char* path, const char* message) {
    printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if (!file) {
        printf("- failed to open file for writing\n");
        return;
    }
    if (file.print(message)) {
        printf("- file written\n");
    }
    else {
        printf("- write failed\n");
    }
    file.close();
}

void appendFile(fs::FS& fs, const char* path, const char* message) {
    printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if (!file) {
        printf("- failed to open file for appending\n");
        return;
    }
    if (file.print(message)) {
        printf("- message appended\n");
    }
    else {
        printf("- append failed\n");
    }
    file.close();
}

void renameFile(fs::FS& fs, const char* path1, const char* path2) {
    printf("Renaming file %s to %s\r\n", path1, path2);
    if (fs.rename(path1, path2)) {
        printf("- file renamed\n");
    }
    else {
        printf("- rename failed\n");
    }
}

void deleteFile(fs::FS& fs, const char* path) {
    printf("Deleting file: %s\r\n", path);
    if (fs.remove(path)) {
        printf("- file deleted\n");
    }
    else {
        printf("- delete failed\n");
    }
}

void testFileIO(fs::FS& fs, const char* path) {
    printf("Testing file I/O with %s\r\n", path);

    static uint8_t buf[512];
    size_t len = 0;
    File file = fs.open(path, FILE_WRITE);
    if (!file) {
        printf("- failed to open file for writing\n");
        return;
    }

    size_t i;
    printf("- writing\n");
    uint32_t start = millis();
    for (i = 0; i < 2048; i++) {
        if ((i & 0x001F) == 0x001F) {
            printf(".\n");
        }
        file.write(buf, 512);
    }
    printf("\n");
    uint32_t end = millis() - start;
    printf(" - %u bytes written in %u ms\r\n", 2048 * 512, end);
    file.close();

    file = fs.open(path);
    start = millis();
    end = start;
    i = 0;
    if (file && !file.isDirectory()) {
        len = file.size();
        size_t flen = len;
        start = millis();
        printf("- reading\n");
        while (len) {
            size_t toRead = len;
            if (toRead > 512) {
                toRead = 512;
            }
            file.read(buf, toRead);
            if ((i++ & 0x001F) == 0x001F) {
                printf(".\n");
            }
            len -= toRead;
        }
        printf("\n");
        end = millis() - start;
        printf("- %u bytes read in %u ms\r\n", flen, end);
        file.close();
    }
    else {
        printf("- failed to open file for reading\n");
    }
}


void SYS_Save(void) {
    Pop_Windows("保存中 请勿切断电源");

    //软盘图标
    Draw_Slow_Bitmap_Resize(128 - 28 - 4, 64 - 28 - 4, Save + 1, Save[0], Save[0], 28, 28);
    Display();

    File file = SPIFFS.open(SYS_SVAE_PATH,FILE_WRITE);
    if (!file) {
        ShowLog(MSG_ERROR, "存档写入失败!");
        Pop_Windows("写入失败");
        return;
    }

    //注意这个是判断版本的依据，载入不同版本的存档可能会导致致命错误
    file.write((uint8_t*)&ChipMAC, sizeof(ChipMAC));
    file.write((uint8_t*)&CompileTime, sizeof(CompileTime));

    file.write((uint8_t*)&BootTemp, sizeof(BootTemp));
    file.write((uint8_t*)&SleepTemp, sizeof(SleepTemp));
    file.write((uint8_t*)&BoostTemp, sizeof(BoostTemp));

    file.write((uint8_t*)&ShutdownTime, sizeof(ShutdownTime));
    file.write((uint8_t*)&SleepTime   , sizeof(SleepTime   ));
    file.write((uint8_t*)&BoostTime   , sizeof(BoostTime   ));

    file.write((uint8_t*)&PIDMode, sizeof(PIDMode));
    file.write((uint8_t*)&PIDMode                    , sizeof(PIDMode                    ));
    file.write((uint8_t*)&PanelSettings              , sizeof(PanelSettings              ));
    file.write((uint8_t*)&ScreenFlip                 , sizeof(ScreenFlip                 ));
    file.write((uint8_t*)&SmoothAnimation_Flag       , sizeof(SmoothAnimation_Flag       ));
    file.write((uint8_t*)&ScreenBrightness           , sizeof(ScreenBrightness           ));
    file.write((uint8_t*)&OptionStripFixedLength_Flag, sizeof(OptionStripFixedLength_Flag));

    file.write((uint8_t*)&Volume, sizeof(Volume));
    file.write((uint8_t*)&RotaryDirection, sizeof(RotaryDirection));
    file.write((uint8_t*)&HandleTrigger, sizeof(HandleTrigger));
    
    file.write((uint8_t*)&SYS_Voltage, sizeof(SYS_Voltage));
    file.write((uint8_t*)&UndervoltageAlert, sizeof(UndervoltageAlert));
    file.write((uint8_t*)&BootPasswd, sizeof(BootPasswd));
    file.write((uint8_t*)&Language, sizeof(Language));

    file.write((uint8_t*)&MyTip, sizeof(MyTip));
    file.write((uint8_t*)&TipID, sizeof(TipID));
    file.write((uint8_t*)&TipTotal, sizeof(TipTotal));

    file.close();
    ShowLog(MSG_OK, "存档保存成功!");
}

void SYS_Load(void) {
    //软盘图标
    Draw_Slow_Bitmap_Resize(128 - 28 - 4, 64 - 28 - 4, Save + 1, Save[0], Save[0], 28, 28);
    Display();

    File file = SPIFFS.open(SYS_SVAE_PATH);
    if (!file.available()) {
        ShowLog(MSG_ERROR,"存档不存在！");
        Pop_Windows("存档不存在！");
        file.close();
        return;
    }

    //写入系统信息，载入不同版本的存档可能会导致致命错误
    uint64_t FSChipMAC;
    char     FSCompileTime[20];

    file.read((uint8_t*)&FSChipMAC, sizeof(FSChipMAC));
    file.read((uint8_t*)&FSCompileTime, sizeof(FSCompileTime));

    //判断存档版本，载入不同版本的存档可能会导致致命错误
    if (memcmp(CompileTime, FSCompileTime, sizeof(FSCompileTime))) {
        ShowLog(MSG_ERROR, "存档版本不一致，拒绝加载存档！");
        Pop_Windows("存档版本不一致");
        return;
    }

    file.read((uint8_t*)&BootTemp, sizeof(BootTemp));
    file.read((uint8_t*)&SleepTemp, sizeof(SleepTemp));
    file.read((uint8_t*)&BoostTemp, sizeof(BoostTemp));

    file.read((uint8_t*)&ShutdownTime, sizeof(ShutdownTime));
    file.read((uint8_t*)&SleepTime, sizeof(SleepTime));
    file.read((uint8_t*)&BoostTime, sizeof(BoostTime));

    file.read((uint8_t*)&PIDMode, sizeof(PIDMode));
    file.read((uint8_t*)&PIDMode, sizeof(PIDMode));
    file.read((uint8_t*)&PanelSettings, sizeof(PanelSettings));
    file.read((uint8_t*)&ScreenFlip, sizeof(ScreenFlip));
    file.read((uint8_t*)&SmoothAnimation_Flag, sizeof(SmoothAnimation_Flag));
    file.read((uint8_t*)&ScreenBrightness, sizeof(ScreenBrightness));
    file.read((uint8_t*)&OptionStripFixedLength_Flag, sizeof(OptionStripFixedLength_Flag));

    file.read((uint8_t*)&Volume, sizeof(Volume));
    file.read((uint8_t*)&RotaryDirection, sizeof(RotaryDirection));
    file.read((uint8_t*)&HandleTrigger, sizeof(HandleTrigger));

    file.read((uint8_t*)&SYS_Voltage, sizeof(SYS_Voltage));
    file.read((uint8_t*)&UndervoltageAlert, sizeof(UndervoltageAlert));
    file.read((uint8_t*)&BootPasswd, sizeof(BootPasswd));
    file.read((uint8_t*)&Language, sizeof(Language));

    file.read((uint8_t*)&MyTip, sizeof(MyTip));
    file.read((uint8_t*)&TipID, sizeof(TipID));
    file.read((uint8_t*)&TipTotal, sizeof(TipTotal));

    file.close();
    
    Update_OLED_Flip();
    Update_OLED_Light_Level();

    ShowLog(MSG_OK, "存档读取成功!");
}

void FilesSystemInit(void) {
    if (!SPIFFS.begin(false)) {
        ShowLog(MSG_ERROR, "文件系统打开失败，自动格式化\n");
        ShowLog(MSG_INFO,"格式化...");
        SPIFFS.format();
        ShowLog(MSG_OK,"文件系统初始化完成！");
    }
    printf("[OK]文件系统\n");

    SYS_Load();

    // listDir(SPIFFS, "/", 0);
    // writeFile(SPIFFS, "/hello.txt", "Hello \n");
    // appendFile(SPIFFS, "/hello.txt", "World!\r\n\n");
    // readFile(SPIFFS, "/hello.txt\n");
    // renameFile(SPIFFS, "/hello.txt", "/foo.txt\n");
    // readFile(SPIFFS, "/foo.txt\n");
    // deleteFile(SPIFFS, "/foo.txt\n");
    // testFileIO(SPIFFS, "/test.txt\n");
    // deleteFile(SPIFFS, "/test.txt\n");
    // printf("Test complete\n");
}