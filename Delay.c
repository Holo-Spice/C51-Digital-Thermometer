#include "Delay.h"

void delay_ms(unsigned int xms)
{
	unsigned char i, j;
	while(xms--)
	{
		i = 2;
		j = 239;
		do
		{
			while (--j);
		} while (--i);
	}
}

void delay_10us(unsigned int ten_us) 
{
    while (ten_us--);
}
void delay_ms2(unsigned int ms)
{
	unsigned int i,j;
	for(i=ms;i>0;i--)
		for(j=110;j>0;j--);
}