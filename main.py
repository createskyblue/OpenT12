import os
#确保win10终端颜色正常显示
if os.name == "nt":
    os.system("")

#字符转unicode函数
def to_unicode(string):
    ret = ""
    for v in string:
        ret = ret + hex(ord(v)).upper().replace('0X', '$')
    return ret

#启动信息   
print('\033[1;31;43m',"{:=^40}".format("U8g2字库生成器 V1.0"),'\033[0m')
print('\033[1;30;33m',"Email: createskyblue@outlook.com\n",'\033[0m')

#获取字符输入 电子科技大学中山学院
inputText = input('\033[1;31;43m[ASK] 请输入需要生成字库的文本（自动去重）：\033[0m')
# inputText = " 电子科技大学中山学院"

#获取生成目标字体名称
targetFontName = input("\033[1;30;33m[ASK] 生成字库命名 >\033[0m")
# targetFontName = "testFont"

#获取字体文件
ttfFontPath = input("\033[1;30;33m[ASK] 输入ttf格式字体路径 >\033[0m")
tftFontName = os.path.basename(ttfFontPath)
tftFontNameReal = tftFontName.split('.')[0]
# ttfFontPath = "font/Alibaba-PuHuiTi-Medium.ttf"

#获取字体DPI解析度
fontSizeDPI = eval(input("\033[1;30;33m[ASK] 输入生成字库字体DPI >\033[0m"))

#获取字体px大小
fontSizePx = eval(input("\033[1;30;33m[ASK] 输入生成字库字体大小(px) >\033[0m"))
#换算成单位pt
fontSizePt = fontSizePx/(fontSizeDPI/72)

#获取字体间距大小%
fontSPSize = eval(input("\033[1;30;33m[ASK] 输入生成字库字体间距大小(%) >\033[0m"))

#询问：输入是否过滤ascii
del_ASCII_flag  = False
del_ASCII_flag_input = input("\033[1;30;33m[ASK] 是否过滤ASCII? (Y/n) >\033[0m")
if (del_ASCII_flag_input == "y" or del_ASCII_flag_input == "Y" or del_ASCII_flag_input == ""):
    del_ASCII_flag = True

#询问：map字符映射文件是否包含ASCII
add_ASCII_flag = False
add_ASCII_flag_input = input("\033[1;30;33m[ASK] MAP字符映射文件是否包含ASCII? (Y/n) >\033[0m")
if (add_ASCII_flag_input == "y" or add_ASCII_flag_input == "Y" or add_ASCII_flag_input == ""):
    add_ASCII_flag = True

#创建原子字典，对目标字去重
atomText = set(inputText)
print("原子字库：",atomText)

#原子字典转unicode格式
unicodeText = []
for c in atomText:
    #选择过滤ASCII结果
    if (del_ASCII_flag and ord(c) < 255):
        continue
    #unicode转换
    unicodeText.append(to_unicode(c))
#对转换结果进行排序
unicodeText.sort()
print("转unicode原子字库：",unicodeText)

#生成map字符映射表
mapFileDatas = ""
#选择性加入ASCII到map字符映射表
if (add_ASCII_flag):
    mapFileDatas += "32-128"
#合并unicode字典到映射表
for u in unicodeText:
    if (len(mapFileDatas) > 0):
        mapFileDatas += ", " #分隔符：这里必须要有空格
    mapFileDatas += u
print("字符map映射表：",mapFileDatas)

#覆盖创建map文件
mapPath = "map/{}.map".format(targetFontName)
mapFile_f = open(mapPath,"w")
mapFile_f.write(mapFileDatas)
mapFile_f.close()

#生成bdf字库
bdfPath = "bdf/{0}_{1}.bdf".format(tftFontNameReal,fontSizePx)
targetFontName = "{0}_{1}".format(targetFontName,fontSizePx)
c_codePath = "code/{0}.c".format(targetFontName)
otf2bdfCMD = "otf2bdf.exe -v -r {3} -p {0} -o {1} {2}".format(fontSizePt,bdfPath,ttfFontPath,fontSizeDPI)
print(">",otf2bdfCMD)
os.system(otf2bdfCMD)
#生成u8g2目标C语言字库文件
bdfconvCMD = "bdfconv.exe -v -b 0 -f 1 {0} -M {1} -n {2} -o {3} -p {4} -d {0}".format(bdfPath,mapPath,targetFontName,c_codePath,fontSPSize)
print(">",bdfconvCMD)
os.system(bdfconvCMD)
#完成信息
print('\033[1;37;42m',"[操作完成]",'\033[0m')
