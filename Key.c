#include "stc.H"
#include "Key.h"
#define GPIO_KEY P1

static unsigned char KeyValue;//������Ŷ�ȡ���ļ�ֵ

/***********************************************
��������ɨ��
***********************************************/
unsigned char OSReadKey(void)
{
	char a=0;
	GPIO_KEY=0x0f;
	if(GPIO_KEY!=0x0f)//��ȡ�����Ƿ���
	{
		Delay10ms();//��ʱ10ms��������
		if(GPIO_KEY!=0x0f)//�ٴμ������Ƿ���
		{
			
			//������
			GPIO_KEY=0X0f;
			
			switch(GPIO_KEY)
			{
				case(0X07) :	KeyValue=1 ; break;//0111 
				case(0X0b) :	KeyValue=2 ; break;//1011 
				case(0X0d) :  KeyValue=3 ; break;//1101
				case(0X0e) :	KeyValue=4 ; break;//1110
				case(0x05) :  KeyValue=5 ; break;//0101����� 1+3
				case(0x06) :  KeyValue=6 ; break;//0110����� 1+4
				case(0x09) :  KeyValue=7 ; break;//1001����� 2+3
				case(0x0a) :  KeyValue=8 ; break;//1010����� 2+4
				default :
					KeyValue=0;
			}
			while((a<50)&&(GPIO_KEY!=0x0f))	 //��ⰴ�����ּ��
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
* �� �� ��         : Delay10ms
* ��������		   : ��ʱ��������ʱ10ms
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void Delay10ms(void)   //��� 0us
{
    unsigned char a,b,c;
    for(c=1;c>0;c--)
        for(b=38;b>0;b--)
            for(a=130;a>0;a--);
}
/***************************************************************/
