#include "lcd12864.h"
#include "Delay.h"

//管脚定义
#define LCD12864_DATAPORT	P0	//LCD12864数据端口定义
sbit LCD12864_RS=P2^6;//数据命令选择
sbit LCD12864_WR=P2^5;//读写选择
sbit LCD12864_E=P2^7;//使能信号

sbit LCD12864_PSB=P3^2;//8位或4并口/串口选择

/*******************************************************************************
* 函 数 名       : lcd12864_write_cmd
* 函数功能		 : LCD12864写命令
* 输    入       : cmd：指令
* 输    出    	 : 无
*******************************************************************************/
void lcd12864_write_cmd(u8 cmd)
{
	LCD12864_RS=0;//选择命令
	LCD12864_WR=0;//选择写
	LCD12864_E=0;
	LCD12864_DATAPORT=cmd;//准备命令
	delay_ms2(1);
	LCD12864_E=1;//使能脚E先上升沿写入
	delay_ms2(1);
	LCD12864_E=0;//使能脚E后负跳变完成写入
}

/*******************************************************************************
* 函 数 名       : lcd12864_write_data
* 函数功能		 : LCD12864写数据
* 输    入       : dat：数据
* 输    出    	 : 无
*******************************************************************************/
void lcd12864_write_data(u8 dat)
{
	LCD12864_RS=1;//选择数据
	LCD12864_WR=0;//选择写
	LCD12864_E=0;
	LCD12864_DATAPORT=dat;//准备数据
	delay_ms2(1);
	LCD12864_E=1;//使能脚E先上升沿写入
	delay_ms2(1);
	LCD12864_E=0;//使能脚E后负跳变完成写入
}

/*******************************************************************************
* 函 数 名       : lcd12864_init
* 函数功能		 : LCD12864初始化
* 输    入       : 无
* 输    出    	 : 无
*******************************************************************************/
void lcd12864_init(void)
{
	LCD12864_PSB=1;//选择8位或4位并口方式
	lcd12864_write_cmd(0x30);//数据总线8位，基本指令操作
	lcd12864_write_cmd(0x0c);//整体显示关，游标显示关，游标正常显示
	lcd12864_write_cmd(0x06);//写入新数据后光标右移，显示屏不移动
	lcd12864_write_cmd(0x01);//清屏	
}

/*******************************************************************************
* 函 数 名       : lcd12864_clear
* 函数功能		 : LCD12864清屏
* 输    入       : 无
* 输    出    	 : 无
*******************************************************************************/
void lcd12864_clear(void)
{
	lcd12864_write_cmd(0x01);	
} 

/*******************************************************************************
* 函 数 名       : lcd12864_show_string
* 函数功能		 : LCD12864显示字符串
* 输    入       : x,y：显示坐标，x=0~7，y=0~3;
				   str：显示字符串数据
* 输    出    	 : 无
*******************************************************************************/
void lcd12864_show_string(u8 x,u8 y,u8 *str)
{
	if(y<=0)y=0;
	if(y>3)y=3;
	x&=0x0f;	//限制x,y不能大于显示范围

	switch(y)
	{
		case 0: x|=0x80;break;//第1行地址+x的偏移
		case 1: x|=0x90;break;//第2行地址+x的偏移
		case 2: x|=0x88;break;//第3行地址+x的偏移
		case 3: x|=0x98;break;//第4行地址+x的偏移
	}
	lcd12864_write_cmd(x);
	while(*str!='\0')
	{
		lcd12864_write_data(*str);
		str++;		
	}
		
} 
int LCD_Pow(int X,int Y)
{
	unsigned char i;
	int Result=1;
	for(i=0;i<Y;i++)
	{
		Result*=X;
	}
	return Result;
}
void LCD_SetCursor(unsigned char Line,unsigned char Column)
{
	if(Line==1)
	{
		lcd12864_write_cmd(0x80|(Column-1));
	}
	else if(Line==2)
	{
		lcd12864_write_cmd(0x80|(Column-1+0x40));
	}
}
void LCD_ShowNum(unsigned char Line,unsigned char Column,unsigned int Number,unsigned char Length)
{
	unsigned char i;
	LCD_SetCursor(Line,Column);
	for(i=Length;i>0;i--)
	{
		lcd12864_write_data(Number/LCD_Pow(10,i-1)%10+'0');
	}
}
void LCD_ShowSignedNum(unsigned char Line,unsigned char Column,int Number,unsigned char Length)
{
	unsigned char i;
	unsigned int Number1;
	LCD_SetCursor(Line,Column);
	if(Number>=0)
	{
		lcd12864_write_data('+');
		Number1=Number;
	}
	else
	{
		lcd12864_write_data('-');
		Number1=-Number;
	}
	for(i=Length;i>0;i--)
	{
		lcd12864_write_data(Number1/LCD_Pow(10,i-1)%10+'0');
	}
}
void LCD_ShowChar(unsigned char Line,unsigned char Column,char Char)
{
	LCD_SetCursor(Line,Column);
	lcd12864_write_data(Char);
}
void LCD_ShowString(unsigned char Line,unsigned char Column,char *String)
{
	unsigned char i;
	LCD_SetCursor(Line,Column);
	for(i=0;String[i]!='\0';i++)
	{
		lcd12864_write_data(String[i]);
	}
}