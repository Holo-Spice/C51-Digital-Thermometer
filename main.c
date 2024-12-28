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
int i,j;
unsigned char KeyNum;
#define MAX_TEMP_RECORDS 5
#define GPIO_DIG P0
#define GPIO_KEY P1
u8 KeyValue;	//用来存放读取到的键值
u8 code smgduan[17]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,
					0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};//显示0~F的值
u8 KeyValue = 0xFF;  // 初始化为0xFF表示无按键

// 新增温度存储相关变量
float TempRecords[MAX_TEMP_RECORDS];  // 温度记录数组
unsigned char TempCount = 0;          // 当前存储的温度数量
/*******************************************************************************
* 函 数 名         : KeyDown
* 函数功能		   : 检测有按键按下并读取键值
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void KeyDown(void)
{
	char a=0;
	GPIO_KEY=0x0f;
	if(GPIO_KEY!=0x0f)//读取按键是否按下
	{
		delay(1000);//延时10ms进行消抖
		if(GPIO_KEY!=0x0f)//再次检测键盘是否按下
		{	
			//测试列
			GPIO_KEY=0X0F;
			switch(GPIO_KEY)
			{
				case(0X07):	KeyValue=0;break;
				case(0X0b):	KeyValue=1;break;
				case(0X0d): KeyValue=2;break;
				case(0X0e):	KeyValue=3;break;
			}
			//测试行
			GPIO_KEY=0XF0;
			switch(GPIO_KEY)
			{
				case(0X70):	KeyValue=KeyValue;break;
				case(0Xb0):	KeyValue=KeyValue+4;break;
				case(0Xd0): KeyValue=KeyValue+8;break;
				case(0Xe0):	KeyValue=KeyValue+12;break;
			}
			
		}
	}
	while((a<50)&&(GPIO_KEY!=0xf0))	 //检测按键松手检测
	{
		delay(100);
		a++;
	}
}

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
    while (start_time < 3000)                 // 执行3秒
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
    lcd12864_init(); // LCD12864初始化
    delay_ms(1000);
    lcd12864_show_string(0, 2, "TH:");
    lcd12864_show_string(4, 2, "TL:");
    DS1302_deInit();
		
    DS18B20_ConvertT();          // 上电先转换一次温度，防止第一次读数据错误
    delay_ms(1000);               // 等待转换完成
    THigh = AT24C02_ReadByte(0); // 读取温度阈值数据
    TLow = AT24C02_ReadByte(1);
    if (THigh > 100 || TLow < -10 || THigh <= TLow) {
        THigh = 20; // 如果阈值非法，则设为默认值
        TLow = 15;
    }

    LCD_ShowSignedNum(2, 11, THigh, 2);
    LCD_ShowSignedNum(2, 15, TLow, 2);
    Timer0_Init();
    while (1) {
        KeyNum = Key();
				KeyDown();		   //按键判断函数
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
								lcd12864_init(); // LCD12864初始化
                TLow++;
                if (TLow >= THigh) {
                    TLow--;
                }
            }
            if (KeyNum == 4) // K4按键，TLow自减
            {
                TLow--;
                if (TLow < -10) {
                    TLow = -10;
                }
            }
            lcd12864_show_string(0, 2, "TH:");
						lcd12864_show_string(4, 2, "TL:");
						LCD_ShowSignedNum(2, 11, THigh, 2);
						LCD_ShowSignedNum(2, 15, TLow, 2);
            AT24C02_WriteByte(0, THigh); // 写入到At24C02中保存
            delay_ms(5);
            AT24C02_WriteByte(1, TLow);
            delay_ms(5);
        }
				if(KeyValue){
					if(KeyValue==1){
						if(TempCount < MAX_TEMP_RECORDS){
							TempRecords[TempCount] = TShow;
							TempCount++;
							lcd12864_init();
							lcd12864_show_string(0, 1, "Temperature");
							lcd12864_show_string(0, 2, "Stored!");
							delay_ms(2000);
							lcd12864_init();
							KeyValue = 0xFF;  // 清除按键值
						}else{
							lcd12864_init();
							lcd12864_show_string(0, 1, "Storage Full!");
							delay_ms(1000);
							lcd12864_init();
							KeyValue = 0xFF;  // 清除按键值
						}
					}
					if(KeyValue==2){
						lcd12864_init();
						LCD_ShowNum(0, 0, TempRecords[0], 2);                                     // 显示华氏温度整数部分
						LCD_ShowChar(0, 1, '.');                                        // 显示小数点
						LCD_ShowNum(0, 2, (unsigned long)(TempRecords[0] * 100) % 100 * 18 / 10, 2); // 显示华氏温度小数部分
						delay_ms(3000);
						KeyValue = 0xFF;  // 清除按键值
						lcd12864_init();
						}
					if(KeyValue==3){
						lcd12864_init();
						LCD_ShowNum(0, 0, TempRecords[1], 2);                                     // 显示华氏温度整数部分
						LCD_ShowChar(0, 1, '.');                                        // 显示小数点
						LCD_ShowNum(0, 2, (unsigned long)(TempRecords[1] * 100) % 100 * 18 / 10, 2); // 显示华氏温度小数部分
						delay_ms(3000);
						KeyValue = 0xFF;  // 清除按键值
						lcd12864_init();
						}
					if(KeyValue==4){
						lcd12864_init();
						LCD_ShowNum(0, 0, TempRecords[2], 2);                                     // 显示华氏温度整数部分
						LCD_ShowChar(0, 1, '.');                                        // 显示小数点
						LCD_ShowNum(0, 2, (unsigned long)(TempRecords[2] * 100) % 100 * 18 / 10, 2); // 显示华氏温度小数部分
						delay_ms(3000);
						KeyValue = 0xFF;  // 清除按键值
						lcd12864_init();
						}
					if(KeyValue==5){
						lcd12864_init();
						LCD_ShowNum(0, 0, TempRecords[3], 2);                                     // 显示华氏温度整数部分
						LCD_ShowChar(0, 1, '.');                                        // 显示小数点
						LCD_ShowNum(0, 2, (unsigned long)(TempRecords[3] * 100) % 100 * 18 / 10, 2); // 显示华氏温度小数部分
						delay_ms(3000);
						KeyValue = 0xFF;  // 清除按键值
						lcd12864_init();
						}
					if(KeyValue==6){
						lcd12864_init();
						LCD_ShowNum(0, 0, TempRecords[4], 2);                                     // 显示华氏温度整数部分
						LCD_ShowChar(0, 1, '.');                                        // 显示小数点
						LCD_ShowNum(0, 2, (unsigned long)(TempRecords[4] * 100) % 100 * 18 / 10, 2); // 显示华氏温度小数部分
						delay_ms(3000);
						KeyValue = 0xFF;  // 清除按键值
						lcd12864_init();
						}
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
}

void Timer0_Routine() interrupt 1 {
    static unsigned int T0Count;
    TL0 = 0x18; // 设置定时初值
    TH0 = 0xFC; // 设置定时初值
    T0Count++;
    if (T0Count >= 20) {
        T0Count = 0;
        Key_Loop(); // 每20ms调用一次按键驱动函数
    }
}