#include "lcd12864.h"
#include "Delay.h"

//�ܽŶ���
#define LCD12864_DATAPORT	P0	//LCD12864���ݶ˿ڶ���
sbit LCD12864_RS=P2^6;//��������ѡ��
sbit LCD12864_WR=P2^5;//��дѡ��
sbit LCD12864_E=P2^7;//ʹ���ź�

sbit LCD12864_PSB=P3^2;//8λ��4����/����ѡ��

/*******************************************************************************
* �� �� ��       : lcd12864_write_cmd
* ��������		 : LCD12864д����
* ��    ��       : cmd��ָ��
* ��    ��    	 : ��
*******************************************************************************/
void lcd12864_write_cmd(u8 cmd)
{
	LCD12864_RS=0;//ѡ������
	LCD12864_WR=0;//ѡ��д
	LCD12864_E=0;
	LCD12864_DATAPORT=cmd;//׼������
	delay_ms2(1);
	LCD12864_E=1;//ʹ�ܽ�E��������д��
	delay_ms2(1);
	LCD12864_E=0;//ʹ�ܽ�E���������д��
}

/*******************************************************************************
* �� �� ��       : lcd12864_write_data
* ��������		 : LCD12864д����
* ��    ��       : dat������
* ��    ��    	 : ��
*******************************************************************************/
void lcd12864_write_data(u8 dat)
{
	LCD12864_RS=1;//ѡ������
	LCD12864_WR=0;//ѡ��д
	LCD12864_E=0;
	LCD12864_DATAPORT=dat;//׼������
	delay_ms2(1);
	LCD12864_E=1;//ʹ�ܽ�E��������д��
	delay_ms2(1);
	LCD12864_E=0;//ʹ�ܽ�E���������д��
}

/*******************************************************************************
* �� �� ��       : lcd12864_init
* ��������		 : LCD12864��ʼ��
* ��    ��       : ��
* ��    ��    	 : ��
*******************************************************************************/
void lcd12864_init(void)
{
	LCD12864_PSB=1;//ѡ��8λ��4λ���ڷ�ʽ
	lcd12864_write_cmd(0x30);//��������8λ������ָ�����
	lcd12864_write_cmd(0x0c);//������ʾ�أ��α���ʾ�أ��α�������ʾ
	lcd12864_write_cmd(0x06);//д�������ݺ������ƣ���ʾ�����ƶ�
	lcd12864_write_cmd(0x01);//����	
}

/*******************************************************************************
* �� �� ��       : lcd12864_clear
* ��������		 : LCD12864����
* ��    ��       : ��
* ��    ��    	 : ��
*******************************************************************************/
void lcd12864_clear(void)
{
	lcd12864_write_cmd(0x01);	
} 

/*******************************************************************************
* �� �� ��       : lcd12864_show_string
* ��������		 : LCD12864��ʾ�ַ���
* ��    ��       : x,y����ʾ���꣬x=0~7��y=0~3;
				   str����ʾ�ַ�������
* ��    ��    	 : ��
*******************************************************************************/
void lcd12864_show_string(u8 x,u8 y,u8 *str)
{
	if(y<=0)y=0;
	if(y>3)y=3;
	x&=0x0f;	//����x,y���ܴ�����ʾ��Χ

	switch(y)
	{
		case 0: x|=0x80;break;//��1�е�ַ+x��ƫ��
		case 1: x|=0x90;break;//��2�е�ַ+x��ƫ��
		case 2: x|=0x88;break;//��3�е�ַ+x��ƫ��
		case 3: x|=0x98;break;//��4�е�ַ+x��ƫ��
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