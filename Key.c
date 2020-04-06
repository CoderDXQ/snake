#include "stc.H"
#include "Key.h"
#define GPIO_KEY P1

static unsigned char KeyValue;//用来存放读取到的键值

/***********************************************
按键驱动扫描
***********************************************/
unsigned char OSReadKey(void)
{
	char a=0;
	GPIO_KEY=0x0f;
	if(GPIO_KEY!=0x0f)//读取按键是否按下
	{
		Delay10ms();//延时10ms进行消抖
		if(GPIO_KEY!=0x0f)//再次检测键盘是否按下
		{
			
			//测试列
			GPIO_KEY=0X0f;
			
			switch(GPIO_KEY)
			{
				case(0X07) :	KeyValue=1 ; break;//0111 
				case(0X0b) :	KeyValue=2 ; break;//1011 
				case(0X0d) :  KeyValue=3 ; break;//1101
				case(0X0e) :	KeyValue=4 ; break;//1110
				case(0x05) :  KeyValue=5 ; break;//0101的情况 1+3
				case(0x06) :  KeyValue=6 ; break;//0110的情况 1+4
				case(0x09) :  KeyValue=7 ; break;//1001的情况 2+3
				case(0x0a) :  KeyValue=8 ; break;//1010的情况 2+4
				default :
					KeyValue=0;
			}
			while((a<50)&&(GPIO_KEY!=0x0f))	 //检测按键松手检测
			{
				Delay10ms();
				a++;
			}
		}	
 
		return KeyValue;	
	}
	return 0;
}	


/*******************************************************************************
* 函 数 名         : Delay10ms
* 函数功能		   : 延时函数，延时10ms
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void Delay10ms(void)   //误差 0us
{
    unsigned char a,b,c;
    for(c=1;c>0;c--)
        for(b=38;b>0;b--)
            for(a=130;a>0;a--);
}
/***************************************************************/
