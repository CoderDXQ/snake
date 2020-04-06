#include"stc.h"
#include"key.h"
#define  io_port P1//����io�˿�ΪP1��

//unsigned char mode=0;//����ȫ�ֱ���mode
//unsigned char Key_Read(void);
//void Delay10ms(void);

/*�Ķ�����ָ��
�����0~4����ģʽ1������������0~8��
����0����û�м�����*/
unsigned char Key_Read(void)
{
	char n=0;
	unsigned char KeyValue=0;
	io_port = 0x0f;
	if(io_port !=0x0f)
	{
		Delay10ms();//��ʱ10ms��������
		if(io_port!=0x0f)
		{
			io_port=0x0f;
			switch(io_port)
			{
				case(0x07) : KeyValue=1 ; break;//0111
				case(0x0b) : KeyValue=2 ; break;//1011
				case(0x0d) : KeyValue=3 ; break;//1101
				case(0x0e) : KeyValue=4 ; break;//1110
			}
			if(mode==1)//�����ģʽ1�ټ���Ƿ�ͬʱ����������
			{
				switch(io_port)
				{
					case(0x05) : KeyValue=5 ; break;//0101�����
					case(0x06) : KeyValue=6 ; break;//0110�����
					case(0x09) : KeyValue=7 ; break;//1001�����
					case(0x0a) : KeyValue=8 ; break;//1010�����
				}
			}
			while((n<50)&&(io_port!=0x0f))//�ɼ�����
			{
				Delay10ms();
				n++;
			}
		}
	}
	return KeyValue;
}

/*��ʱ����
ÿѭ��һ����ʱ10ms�������*/
void Delay10ms(void)
{
    unsigned char a,b,c;
    for(c=1;c>0;c--)
        for(b=38;b>0;b--)
            for(a=130;a>0;a--);
}
