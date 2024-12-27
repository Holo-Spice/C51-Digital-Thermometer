#ifndef __LCD1602_H__
#define __LCD1602_H__
#include "public.h"
#include <REGX52.H>
//用户调用函数：
void LCD_Init();
void LCD_Clear();
void LCD_Clear2();
//void LCD_WriteData(unsigned char Data);
void LCD_ShowChar(unsigned char Line,unsigned char Column,char Char);
void LCD_ShowString(unsigned char Line,unsigned char Column,char *String);
void LCD_ShowNum(unsigned char Line,unsigned char Column,unsigned int Number,unsigned char Length);
void LCD_ShowSignedNum(unsigned char Line,unsigned char Column,int Number,unsigned char Length);
void LCD_ShowHexNum(unsigned char Line,unsigned char Column,unsigned int Number,unsigned char Length);
void LCD_ShowBinNum(unsigned char Line,unsigned char Column,unsigned int Number,unsigned char Length);

//¹Ü½Å¶¨Òå
sbit LCD12864_RS=P2^6;//Êý¾ÝÃüÁîÑ¡Ôñ
sbit LCD12864_WR=P2^5;//¶ÁÐ´Ñ¡Ôñ
sbit LCD12864_E=P2^7;//Ê¹ÄÜÐÅºÅ
#define LCD12864_DATAPORT	P0	//LCD12864Êý¾Ý¶Ë¿Ú¶¨Òå
sbit LCD12864_PSB=P3^2;//8Î»»ò4²¢¿Ú/´®¿ÚÑ¡Ôñ
void lcd12864_init(void);
void lcd12864_clear(void);
void lcd12864_show_string(u8 x,u8 y,u8 *str);

#endif
