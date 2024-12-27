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

sbit BEEP = P2 ^ 5; // ������������ӵ�����
unsigned char time;
float T, TShow;
char TLow, THigh;
unsigned char KeyNum;

void Beep_Alert(unsigned int duration) // ��������������
{
    unsigned int i;
    for (i = 0; i < duration; i++) {
        BEEP = !BEEP;   // ��ת����������
        delay_10us(10); // ��ʱ����Ƶ��
    }
    BEEP = 0; // �رշ�����
}

void main() {
    long F;
    unsigned int FInt;  // ��������
    unsigned int FFrac; // С������
    int start_time;
    DS1302_Init();
    DS1302_SetTime(); // ����ʱ��
    start_time = 0;
    lcd12864_init(); // LCD12864��ʼ��

    lcd12864_show_string(1, 0, "��\xfd���¶ȼ�");
    lcd12864_show_string(1, 1, "2021217934"); // ��4���ַ�����ʾ
    while (start_time < 3000)                 // ִ��3��
    {
        DS1302_ReadTime(); // ��ȡʱ��
        lcd12864_show_string(0, 2, "TIME:");
        LCD_ShowNum(4, 6, DS1302_Time[3], 2); // ��ʾʱ
        lcd12864_show_string(4, 2, ":");
        LCD_ShowNum(4, 9, DS1302_Time[4], 2); // ��ʾ��
        lcd12864_show_string(6, 2, ":");
        LCD_ShowNum(4, 12, DS1302_Time[5], 2); // ��ʾ��
        delay_ms(100);                         // ÿ����ʱ100ms
        start_time += 100;                     // ���Ӽ�ʱ��
    }
    lcd12864_init(); // LCD12864��ʼ��
    delay_ms(1000);
    lcd12864_show_string(0, 2, "TH:");
    lcd12864_show_string(4, 2, "TL:");
    DS1302_deInit();
		
    DS18B20_ConvertT();          // �ϵ���ת��һ���¶ȣ���ֹ��һ�ζ����ݴ���
    delay_ms(1000);               // �ȴ�ת�����
    THigh = AT24C02_ReadByte(0); // ��ȡ�¶���ֵ����
    TLow = AT24C02_ReadByte(1);
    if (THigh > 100 || TLow < -10 || THigh <= TLow) {
        THigh = 20; // �����ֵ�Ƿ�������ΪĬ��ֵ
        TLow = 15;
    }

    LCD_ShowSignedNum(2, 11, THigh, 2);
    LCD_ShowSignedNum(2, 15, TLow, 2);
    Timer0_Init();
    while (1) {
        KeyNum = Key();
        /* �¶ȶ�ȡ����ʾ */
        DS18B20_ConvertT();  // ת���¶�
        T = DS18B20_ReadT(); // ��ȡ�¶�
        if (T < 0)           // ����¶�С��0
        {
            LCD_ShowChar(1, 1, '-'); // ��ʾ����
            TShow = -T;              // ���¶ȱ�Ϊ����
        } else                       // ����¶ȴ��ڵ���0
        {
            LCD_ShowChar(1, 1, '+'); // ��ʾ����
            TShow = T;
        }
        LCD_ShowNum(1, 2, TShow, 2);                              // ��ʾ�¶���������
        LCD_ShowChar(1, 3, '.');                                  // ��ʾС����
        LCD_ShowNum(1, 4, (unsigned long)(TShow * 100) % 100, 2); // ��ʾ�¶�С������

        // ���㻪���¶�
        F = (long)T * 18 / 10 + 32;
        FInt = (unsigned int)(F);        // ��������
        FFrac = (unsigned int)(F % 100); // С������

        LCD_ShowNum(1, 6, FInt, 2);                                     // ��ʾ�����¶���������
        LCD_ShowChar(1, 7, '.');                                        // ��ʾС����
        LCD_ShowNum(1, 8, (unsigned long)(T * 100) % 100 * 18 / 10, 2); // ��ʾ�����¶�С������
        /* ��ֵ�жϼ���ʾ */
        if (KeyNum) {
            if (KeyNum == 1) // K1������THigh����
            {
                THigh++;
                if (THigh > 100) {
                    THigh = 100;
                }
            }
            if (KeyNum == 2) // K2������THigh�Լ�
            {
                THigh--;
                if (THigh <= TLow) {
                    THigh++;
                }
            }
            if (KeyNum == 3) // K3������TLow����
            {
								lcd12864_init(); // LCD12864��ʼ��
                TLow++;
                if (TLow >= THigh) {
                    TLow--;
                }
            }
            if (KeyNum == 4) // K4������TLow�Լ�
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
            AT24C02_WriteByte(0, THigh); // д�뵽At24C02�б���
            delay_ms(5);
            AT24C02_WriteByte(1, TLow);
            delay_ms(5);
        }
        lcd12864_show_string(0, 2, "TH:");
        lcd12864_show_string(4, 2, "TL:");
        LCD_ShowSignedNum(2, 11, THigh, 2);
        LCD_ShowSignedNum(2, 15, TLow, 2);
        if (T > THigh) // �¶�Խ��
        {
            // LCD_ShowString(1, 13, "OV:H");
            Beep_Alert(1000); // ����������
        } else if (T < TLow)  // �¶�Խ��
        {
            // LCD_ShowString(1, 13, "OV:L");
            Beep_Alert(1000); // ����������
        } else {
            // LCD_ShowString(1, 13, "    "); // ���������ʾ
            BEEP = 0; // �رշ�����
        }
    }
}

void Timer0_Routine() interrupt 1 {
    static unsigned int T0Count;
    TL0 = 0x18; // ���ö�ʱ��ֵ
    TH0 = 0xFC; // ���ö�ʱ��ֵ
    T0Count++;
    if (T0Count >= 20) {
        T0Count = 0;
        Key_Loop(); // ÿ20ms����һ�ΰ�����������
    }
}