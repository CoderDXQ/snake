#include"stc.h"
#include"key.h"
#define  io_port P1//定义io端口为P1口

//unsigned char mode=0;//定义全局变量mode
//unsigned char Key_Read(void);
//void Delay10ms(void);

/*阅读键盘指令
输出（0~4）在模式1的情况下输出（0~8）
其中0代表没有键按下*/
unsigned char Key_Read(void)
{
	char n=0;
	unsigned char KeyValue=0;
	io_port = 0x0f;
	if(io_port !=0x0f)
	{
		Delay10ms();//延时10ms进行消抖
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
			if(mode==1)//如果在模式1再检查是否同时按下两个键
			{
				switch(io_port)
				{
					case(0x05) : KeyValue=5 ; break;//0101的情况
					case(0x06) : KeyValue=6 ; break;//0110的情况
					case(0x09) : KeyValue=7 ; break;//1001的情况
					case(0x0a) : KeyValue=8 ; break;//1010的情况
				}
			}
			while((n<50)&&(io_port!=0x0f))//松键消抖
			{
				Delay10ms();
				n++;
			}
		}
	}
	return KeyValue;
}

/*延时函数
每循环一次延时10ms，无误差*/
void Delay10ms(void)
{
    unsigned char a,b,c;
    for(c=1;c>0;c--)
        for(b=38;b>0;b--)
            for(a=130;a>0;a--);
}
