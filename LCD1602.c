

#include "LCD1602.h"
// 引脚配置：
sbit LCD_RS = P2 ^ 6; //
sbit LCD_RW = P2 ^ 5; // LCD12864_WR
sbit LCD_EN = P2 ^ 7; // LCD12864_E

// 函数定义：
/**
 * @brief  LCD1602延时函数，12MHz调用可延时1ms
 * @param  无
 * @retval 无
 */
void LCD_Delay() {
    unsigned char i, j;

    i = 2;
    j = 239;
    do {
        while (--j)
            ;
    } while (--i);
}

/**
 * @brief  LCD1602写命令
 * @param  Command 要写入的命令
 * @retval 无
 */
void LCD_WriteCommand(unsigned char Command) {
    LCD_RS = 0;
    LCD_RW = 0;

    LCD_DataPort = Command;
    LCD_EN = 1;
    LCD_Delay();
    LCD_EN = 0;
    LCD_Delay();
}

/**
 * @brief  LCD1602写数据
 * @param  Data 要写入的数据
 * @retval 无
 */
void LCD_WriteData(unsigned char Data) {
    LCD_RS = 1;
    LCD_RW = 0;
    LCD_DataPort = Data;
    LCD_EN = 1;
    LCD_Delay();
    LCD_EN = 0;
    LCD_Delay();
}

/**
 * @brief  LCD1602设置光标位置
 * @param  Line 行位置，范围：1~2
 * @param  Column 列位置，范围：1~16
 * @retval 无
 */
void LCD_SetCursor(unsigned char Line, unsigned char Column) {
    if (Line == 1) {
        LCD_WriteCommand(0x80 | (Column - 1));
    } else if (Line == 2) {
        LCD_WriteCommand(0x80 | (Column - 1 + 0x40));
    }
}

/**
 * @brief  LCD1602初始化函数
 * @param  无
 * @retval 无
 */
void LCD_Init() {
    LCD12864_PSB = 1;       // Ñ¡Ôñ8Î»»ò4Î»²¢¿Ú·½Ê
    LCD_WriteCommand(0x30); // 八位数据接口，两行显示，5*7点阵
    LCD_WriteCommand(0x0c); // 显示开，光标关，闪烁关
    LCD_WriteCommand(0x06); // 数据读写操作后，光标自动加一，画面不动
    LCD_WriteCommand(0x01); // 光标复位，清屏
}

/**
 * @brief  在LCD1602指定位置上显示一个字符
 * @param  Line 行位置，范围：1~2
 * @param  Column 列位置，范围：1~16
 * @param  Char 要显示的字符
 * @retval 无
 */
void LCD_ShowChar(unsigned char Line, unsigned char Column, char Char) {
    LCD_SetCursor(Line, Column);
    LCD_WriteData(Char);
}

/**
 * @brief  在LCD1602指定位置开始显示所给字符串
 * @param  Line 起始行位置，范围：1~2
 * @param  Column 起始列位置，范围：1~16
 * @param  String 要显示的字符串
 * @retval 无
 */
void LCD_ShowString(unsigned char Line, unsigned char Column, char *String) {
    unsigned char i;
    LCD_SetCursor(Line, Column);
    for (i = 0; String[i] != '\0'; i++) {
        LCD_WriteData(String[i]);
    }
}

/**
 * @brief  返回值=X的Y次方
 */
int LCD_Pow(int X, int Y) {
    unsigned char i;
    int Result = 1;
    for (i = 0; i < Y; i++) {
        Result *= X;
    }
    return Result;
}

/**
 * @brief  在LCD1602指定位置开始显示所给数字
 * @param  Line 起始行位置，范围：1~2
 * @param  Column 起始列位置，范围：1~16
 * @param  Number 要显示的数字，范围：0~65535
 * @param  Length 要显示数字的长度，范围：1~5
 * @retval 无
 */
void LCD_ShowNum(unsigned char Line, unsigned char Column, unsigned int Number, unsigned char Length) {
    unsigned char i;
    LCD_SetCursor(Line, Column);
    for (i = Length; i > 0; i--) {
        LCD_WriteData(Number / LCD_Pow(10, i - 1) % 10 + '0');
    }
}

/**
 * @brief  在LCD1602指定位置开始以有符号十进制显示所给数字
 * @param  Line 起始行位置，范围：1~2
 * @param  Column 起始列位置，范围：1~16
 * @param  Number 要显示的数字，范围：-32768~32767
 * @param  Length 要显示数字的长度，范围：1~5
 * @retval 无
 */
void LCD_ShowSignedNum(unsigned char Line, unsigned char Column, int Number, unsigned char Length) {
    unsigned char i;
    unsigned int Number1;
    LCD_SetCursor(Line, Column);
    if (Number >= 0) {
        LCD_WriteData('+');
        Number1 = Number;
    } else {
        LCD_WriteData('-');
        Number1 = -Number;
    }
    for (i = Length; i > 0; i--) {
        LCD_WriteData(Number1 / LCD_Pow(10, i - 1) % 10 + '0');
    }
}

/**
 * @brief  在LCD1602指定位置开始以十六进制显示所给数字
 * @param  Line 起始行位置，范围：1~2
 * @param  Column 起始列位置，范围：1~16
 * @param  Number 要显示的数字，范围：0~0xFFFF
 * @param  Length 要显示数字的长度，范围：1~4
 * @retval 无
 */
void LCD_ShowHexNum(unsigned char Line, unsigned char Column, unsigned int Number, unsigned char Length) {
    unsigned char i, SingleNumber;
    LCD_SetCursor(Line, Column);
    for (i = Length; i > 0; i--) {
        SingleNumber = Number / LCD_Pow(16, i - 1) % 16;
        if (SingleNumber < 10) {
            LCD_WriteData(SingleNumber + '0');
        } else {
            LCD_WriteData(SingleNumber - 10 + 'A');
        }
    }
}

/**
 * @brief  在LCD1602指定位置开始以二进制显示所给数字
 * @param  Line 起始行位置，范围：1~2
 * @param  Column 起始列位置，范围：1~16
 * @param  Number 要显示的数字，范围：0~1111 1111 1111 1111
 * @param  Length 要显示数字的长度，范围：1~16
 * @retval 无
 */
void LCD_ShowBinNum(unsigned char Line, unsigned char Column, unsigned int Number, unsigned char Length) {
    unsigned char i;
    LCD_SetCursor(Line, Column);
    for (i = Length; i > 0; i--) {
        LCD_WriteData(Number / LCD_Pow(2, i - 1) % 2 + '0');
    }
}

void LCD_Clear() {
    unsigned char i, j;
    for (i = 1; i <= 15; i++) {
        for (j = 1; j <= 45; j++) {
            LCD_ShowChar(i, j, ' '); // 用空格填充屏幕
        }
    }
}
void LCD_Clear2() {
    unsigned char i, j;
    for (i = 1; i <= 15; i++) {
        for (j = 1; j <= 2; j++) {
            LCD_ShowChar(i, j, ' '); // 用空格填充屏幕
        }
    }
}

void lcd12864_write_cmd(u8 cmd) {
    LCD12864_RS = 0; // Ñ¡ÔñÃüÁî
    LCD12864_WR = 0; // Ñ¡ÔñÐ´
    LCD12864_E = 0;
    LCD12864_DATAPORT = cmd; // ×¼±¸ÃüÁî
    delay_ms(1);
    LCD12864_E = 1; // Ê¹ÄÜ½ÅEÏÈÉÏÉýÑØÐ´Èë
    delay_ms(1);
    LCD12864_E = 0; // Ê¹ÄÜ½ÅEºó¸ºÌø±äÍê³ÉÐ´Èë
}

/*******************************************************************************
 * º¯ Êý Ãû       : lcd12864_write_data
 * º¯Êý¹¦ÄÜ		 : LCD12864Ð´Êý¾Ý
 * Êä    Èë       : dat£ºÊý¾Ý
 * Êä    ³ö    	 : ÎÞ
 *******************************************************************************/
void lcd12864_write_data(u8 dat) {
    LCD12864_RS = 1; // Ñ¡ÔñÊý¾Ý
    LCD12864_WR = 0; // Ñ¡ÔñÐ´
    LCD12864_E = 0;
    LCD12864_DATAPORT = dat; // ×¼±¸Êý¾Ý
    delay_ms(1);
    LCD12864_E = 1; // Ê¹ÄÜ½ÅEÏÈÉÏÉýÑØÐ´Èë
    delay_ms(1);
    LCD12864_E = 0; // Ê¹ÄÜ½ÅEºó¸ºÌø±äÍê³ÉÐ´Èë
}

/*******************************************************************************
 * º¯ Êý Ãû       : lcd12864_init
 * º¯Êý¹¦ÄÜ		 : LCD12864³õÊ¼»¯
 * Êä    Èë       : ÎÞ
 * Êä    ³ö    	 : ÎÞ
 *******************************************************************************/
void lcd12864_init(void) {
    LCD12864_PSB = 1;         // Ñ¡Ôñ8Î»»ò4Î»²¢¿Ú·½Ê½
    lcd12864_write_cmd(0x30); // Êý¾Ý×ÜÏß8Î»£¬»ù±¾Ö¸Áî²Ù×÷
    lcd12864_write_cmd(0x0c); // ÕûÌåÏÔÊ¾¹Ø£¬ÓÎ±êÏÔÊ¾¹Ø£¬ÓÎ±êÕý³£ÏÔÊ¾
    lcd12864_write_cmd(0x06); // Ð´ÈëÐÂÊý¾Ýºó¹â±êÓÒÒÆ£¬ÏÔÊ¾ÆÁ²»ÒÆ¶¯
    lcd12864_write_cmd(0x01); // ÇåÆÁ
}

/*******************************************************************************
 * º¯ Êý Ãû       : lcd12864_clear
 * º¯Êý¹¦ÄÜ		 : LCD12864ÇåÆÁ
 * Êä    Èë       : ÎÞ
 * Êä    ³ö    	 : ÎÞ
 *******************************************************************************/
void lcd12864_clear(void) {
    lcd12864_write_cmd(0x01);
}

/*******************************************************************************
* º¯ Êý Ãû       : lcd12864_show_string
* º¯Êý¹¦ÄÜ		 : LCD12864ÏÔÊ¾×Ö·û´®
* Êä    Èë       : x,y£ºÏÔÊ¾×ø±ê£¬x=0~7£¬y=0~3;
                   str£ºÏÔÊ¾×Ö·û´®Êý¾Ý
* Êä    ³ö    	 : ÎÞ
*******************************************************************************/
void lcd12864_show_string(u8 x, u8 y, u8 *str) {
    if (y <= 0)
        y = 0;
    if (y > 3)
        y = 3;
    x &= 0x0f; // ÏÞÖÆx,y²»ÄÜ´óÓÚÏÔÊ¾·¶Î§

    switch (y) {
    case 0:
        x |= 0x80;
        break; // µÚ1ÐÐµØÖ·+xµÄÆ«ÒÆ
    case 1:
        x |= 0x90;
        break; // µÚ2ÐÐµØÖ·+xµÄÆ«ÒÆ
    case 2:
        x |= 0x88;
        break; // µÚ3ÐÐµØÖ·+xµÄÆ«ÒÆ
    case 3:
        x |= 0x98;
        break; // µÚ4ÐÐµØÖ·+xµÄÆ«ÒÆ
    }
    lcd12864_write_cmd(x);
    while (*str != '\0') {
        lcd12864_write_data(*str);
        str++;
    }
}

#include "AT24C02.h"
#include "DS1302.h"
#include "DS18B20.h"
#include "Delay.h"
#include "Key.h"
// #include "LCD1602.h"
#include "Timer0.h"
#include <REGX52.H>
// #include "public.h"
#include "lcd12864.h"

sbit BEEP = P2 ^ 5; // 定义蜂鸣器连接的引脚
unsigned char time;
float T, TShow;
char TLow, THigh;
unsigned char KeyNum;

void Beep_Alert(unsigned int duration) // 蜂鸣器报警函数
{
    unsigned int i;
    for (i = 0; i < duration; i++) {
        BEEP = !BEEP;   // 翻转蜂鸣器引脚
        delay_10us(10); // 延时控制频率
    }
    BEEP = 0; // 关闭蜂鸣器
}

void main() {
    long F;
    unsigned int FInt;  // 整数部分
    unsigned int FFrac; // 小数部分
    int start_time;
    DS1302_Init();
    DS1302_SetTime(); // 设置时间
    start_time = 0;
    lcd12864_init(); // LCD12864初始化

    lcd12864_show_string(1, 0, "数\xfd显温度计");
    lcd12864_show_string(1, 1, "2021217934"); // 第4行字符串显示
    while (start_time < 0)                    // 执行3秒
    {
        DS1302_ReadTime(); // 读取时间
        lcd12864_show_string(0, 2, "TIME:");
        LCD_ShowNum(4, 6, DS1302_Time[3], 2); // 显示时
        lcd12864_show_string(4, 2, ":");
        LCD_ShowNum(4, 9, DS1302_Time[4], 2); // 显示分
        lcd12864_show_string(6, 2, ":");
        LCD_ShowNum(4, 12, DS1302_Time[5], 2); // 显示秒
        delay_ms(100);                         // 每次延时100ms
        start_time += 100;                     // 增加计时器
    }
    lcd12864_init();             // LCD12864初始化
    DS18B20_ConvertT();          // 上电先转换一次温度，防止第一次读数据错误
    delay_ms(100);               // 等待转换完成
    THigh = AT24C02_ReadByte(0); // 读取温度阈值数据
    TLow = AT24C02_ReadByte(1);
    if (THigh > 100 || TLow < -10 || THigh <= TLow) {
        THigh = 20; // 如果阈值非法，则设为默认值
        TLow = 15;
    }
    lcd12864_show_string(2, 9, "TH:");
    lcd12864_show_string(2, 13, "TL:");
    LCD_ShowSignedNum(2, 11, THigh, 2);
    LCD_ShowSignedNum(2, 15, TLow, 2);
    Timer0_Init();
    while (1) {
        KeyNum = Key();

        /* 温度读取及显示 */
        DS18B20_ConvertT();  // 转换温度
        T = DS18B20_ReadT(); // 读取温度
        if (T < 0)           // 如果温度小于0
        {
            LCD_ShowChar(1, 1, '-'); // 显示负号
            TShow = -T;              // 将温度变为正数
        } else                       // 如果温度大于等于0
        {
            LCD_ShowChar(1, 1, '+'); // 显示正号
            TShow = T;
        }
        LCD_ShowNum(1, 2, TShow, 2);                              // 显示温度整数部分
        LCD_ShowChar(1, 3, '.');                                  // 显示小数点
        LCD_ShowNum(1, 4, (unsigned long)(TShow * 100) % 100, 2); // 显示温度小数部分

        // 计算华氏温度
        F = (long)T * 18 / 10 + 32;
        FInt = (unsigned int)(F);        // 整数部分
        FFrac = (unsigned int)(F % 100); // 小数部分

        LCD_ShowNum(1, 6, FInt, 2);                                     // 显示华氏温度整数部分
        LCD_ShowChar(1, 7, '.');                                        // 显示小数点
        LCD_ShowNum(1, 8, (unsigned long)(T * 100) % 100 * 18 / 10, 2); // 显示华氏温度小数部分
        /* 阈值判断及显示 */
        if (KeyNum) {
            if (KeyNum == 1) // K1按键，THigh自增
            {
                THigh++;
                if (THigh > 100) {
                    THigh = 100;
                }
            }
            if (KeyNum == 2) // K2按键，THigh自减
            {
                THigh--;
                if (THigh <= TLow) {
                    THigh++;
                }
            }
            if (KeyNum == 3) // K3按键，TLow自增
            {
                TLow++;
                if (TLow >= THigh) {
                    TLow--;
                }
                lcd12864_init(); // LCD12864初始化
            }
            if (KeyNum == 4) // K4按键，TLow自减
            {
                TLow--;
                if (TLow < -10) {
                    TLow = -10;
                }
            }
            LCD_ShowString(2, 9, "TH:");
            LCD_ShowString(2, 13, "TL:");
            LCD_ShowSignedNum(2, 11, THigh, 2);
            LCD_ShowSignedNum(2, 15, TLow, 2);
            LCD_ShowSignedNum(2, 11, THigh, 2); // 显示阈值数据
            LCD_ShowSignedNum(2, 15, TLow, 2);
            AT24C02_WriteByte(0, THigh); // 写入到At24C02中保存
            delay_ms(5);
            AT24C02_WriteByte(1, TLow);
            delay_ms(5);
        }

        if (T > THigh) // 温度越高
        {
            // LCD_ShowString(1, 13, "OV:H");
            Beep_Alert(1000); // 蜂鸣器报警
        } else if (T < TLow)  // 温度越低
        {
            // LCD_ShowString(1, 13, "OV:L");
            Beep_Alert(1000); // 蜂鸣器报警
        } else {
            // LCD_ShowString(1, 13, "    "); // 清除报警显示
            BEEP = 0; // 关闭蜂鸣器
        }
    }
}

while (1) {
    KeyNum = Key();
    /* 温度读取及显示 */
    DS18B20_ConvertT();  // 转换温度
    T = DS18B20_ReadT(); // 读取温度
    if (T < 0)           // 如果温度小于0
    {
        LCD_ShowChar(1, 1, '-'); // 显示负号
        TShow = -T;              // 将温度变为正数
    } else                       // 如果温度大于等于0
    {
        LCD_ShowChar(1, 1, '+'); // 显示正号
        TShow = T;
    }
    LCD_ShowNum(1, 2, TShow, 2);                              // 显示温度整数部分
    LCD_ShowChar(1, 3, '.');                                  // 显示小数点
    LCD_ShowNum(1, 4, (unsigned long)(TShow * 100) % 100, 2); // 显示温度小数部分

    // 计算华氏温度
    F = (long)T * 18 / 10 + 32;
    FInt = (unsigned int)(F);        // 整数部分
    FFrac = (unsigned int)(F % 100); // 小数部分

    LCD_ShowNum(1, 6, FInt, 2);                                     // 显示华氏温度整数部分
    LCD_ShowChar(1, 7, '.');                                        // 显示小数点
    LCD_ShowNum(1, 8, (unsigned long)(T * 100) % 100 * 18 / 10, 2); // 显示华氏温度小数部分
    /* 阈值判断及显示 */
    if (KeyNum) {
        if (KeyNum == 1) // K1按键，THigh自增
        {
            THigh++;
            if (THigh > 100) {
                THigh = 100;
            }
            lcd12864_init(); // LCD12864初始化
        }
        if (KeyNum == 2) // K2按键，THigh自减
        {
            THigh--;
            if (THigh <= TLow) {
                THigh++;
            }
            lcd12864_init(); // LCD12864初始化
        }
        if (KeyNum == 3) // K3按键，TLow自增
        {
            TLow++;
            if (TLow >= THigh) {
                TLow--;
            }
            lcd12864_init(); // LCD12864初始化
        }
        if (KeyNum == 4) // K4按键，TLow自减
        {
            TLow--;
            if (TLow < -10) {
                TLow = -10;
            }
            lcd12864_init(); // LCD12864初始化
        }
        lcd12864_show_string(1, 2, "TH:");
        lcd12864_show_string(6, 2, "TL:");
        //               LCD_ShowSignedNum(2, 11, THigh, 2);
        //               LCD_ShowSignedNum(2, 15, TLow, 2);
        //               LCD_ShowSignedNum(2, 11, THigh, 2); // 显示阈值数据
        //               LCD_ShowSignedNum(2, 15, TLow, 2);
        AT24C02_WriteByte(0, THigh); // 写入到At24C02中保存
        delay_ms(5);
        AT24C02_WriteByte(1, TLow);
        delay_ms(5);
    }
    lcd12864_show_string(0, 2, "TH:");
    lcd12864_show_string(4, 2, "TL:");
    LCD_ShowSignedNum(2, 11, THigh, 2);
    LCD_ShowSignedNum(2, 15, TLow, 2);
    if (T > THigh) // 温度越高
    {
        // LCD_ShowString(1, 13, "OV:H");
        Beep_Alert(1000); // 蜂鸣器报警
    } else if (T < TLow)  // 温度越低
    {
        // LCD_ShowString(1, 13, "OV:L");
        Beep_Alert(1000); // 蜂鸣器报警
    } else {
        // LCD_ShowString(1, 13, "    "); // 清除报警显示
        BEEP = 0; // 关闭蜂鸣器
    }
}