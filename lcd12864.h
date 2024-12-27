#ifndef _lcd12864_H
#define _lcd12864_H

#include <REGX52.H>
#include "public.h"


//º¯ÊýÉùÃ÷
void lcd12864_init(void);
void lcd12864_clear(void);
void lcd12864_show_string(u8 x,u8 y,u8 *str);
void LCD_SetCursor(unsigned char Line,unsigned char Column);
void LCD_ShowNum(unsigned char Line,unsigned char Column,unsigned int Number,unsigned char Length);
int LCD_Pow(int X,int Y);
void LCD_ShowSignedNum(unsigned char Line,unsigned char Column,int Number,unsigned char Length);
void LCD_ShowChar(unsigned char Line,unsigned char Column,char Char);
void LCD_ShowString(unsigned char Line,unsigned char Column,char *String);
#endif
