#include"stc.h"
#include"intrins.h"
#include"Lcd12864.h"
//#include<stdbool.h>
//typedef enum {false = 0,true = 1}bool;

//bool Lcd_CheckBusy(void);
//检查lcd是否正忙,如果正忙返回1,不忙返回0*************
bool Lcd_CheckBusy(void)
{
	bool busy=false;
	LcdData=0xff;//LcdDate就是P0口
	RS=0;//P2.6口，选择数据(1)/指令(0)
	RW=1;//P2.5口，选择读(1)/写(0)
	E=1;//P2.7口,选择有效(1)/无效(0)
	_nop_();//包含在intrins.h里,延时一个机器周期
	busy=LcdData&&0x80;//检查P0.0端口,如果是1则忙,不能使用
	E=0;
	return busy;
}
//向lcd中写入一个字节的数据***********************************
void Lcd_WriteData(unsigned char Data)
{  
	while(Lcd_CheckBusy());
	RS=1;//数据
	RW=0;//写
	E=0;//关使能
	_nop_();  
	_nop_(); 
	LcdData=Data;//写数据
	E=1;//开使能
	_nop_();
	_nop_();
	E=0;//关使能
}
//从lcd中读取一个字节数据**************************************
unsigned char Lcd_ReadData(void)
{
	unsigned char Temp;
	while(Lcd_CheckBusy());
 	LcdData=0xff;//写入全1
 	RS=1;//数据
	RW=1;//读
	E=1;//开使能
	_nop_();
   	Temp=LcdData;//读数据
   	E=0;//关使能
   	return Temp;
}
//向lcd中写一字节指令*******************************************
void Lcd_WriteCmd(unsigned char Code)
{  
	while(Lcd_CheckBusy());
   	RS=0;//指令
   	RW=0;//写
   	E=0;//关使能
   	_nop_();  
		_nop_();
   	LcdData=Code;//写指令
   	_nop_(); 
		_nop_();
   	E=1;//开使能
   	_nop_();  
		_nop_();
   	E=0;//关使能
}
//向指定位置写入字符串*******************************************
void Lcd_WriteStr(unsigned char x,unsigned char y,unsigned char *Str)
{
	if((y>3)||(x>7))
		return;//如果指定位置不在显示区域内，则不做任何写入直接返回
	EA=0;//关中断
	switch(y)
	{
		case 0:
				Lcd_WriteCmd(0x80+x);
				break;
		case 1:
				Lcd_WriteCmd(0x90+x);
				break;				
		case 2:
				Lcd_WriteCmd(0x88+x);
				break;
		case 3:
				Lcd_WriteCmd(0x98+x);
				break;
	}
	while(*Str>0)
	{  
		Lcd_WriteData(*Str);
  		Str++;     
	}
	EA=1;//开中断
}
//掩码表,快速找到要操作的像素点************************************
code unsigned int MaskTab[]={0x0001,0x0002,0x0004,0x0008,0x0010,0x0020,0x0040,0x0080,
							 0x0100,0x0200,0x0400,0x0800,0x1000,0x2000,0x4000,0x8000};

//向LCD指定坐标写入一个象素,象素颜色有两种，0代表白（无显示），1代表黑（有显示）
void Lcd_PutPixel(unsigned char x,unsigned char y,bool Color)
{
	unsigned char z,w;
	unsigned int Temp;
	if(x>=128||y>=64)
		return;
	//Color=Color%2;
	w=15-x%16;//确定对这个字的第多少位进行操作
	x=x/16;//确定为一行上的第几字

	if(y<32) //如果为上页
		z=0x80;
	else     //否则如果为下页
		z=0x88;

	y=y%32;
	EA=0;
	Lcd_WriteCmd(0x36);
	Lcd_WriteCmd(y+0x80);        //行地址
	Lcd_WriteCmd(x+z);     //列地址 
	Temp=Lcd_ReadData();//先空读一次
	Temp=(unsigned int)Lcd_ReadData()<<8;//再读出高8位
	Temp|=(unsigned int)Lcd_ReadData();//再读出低8位
	EA=1;
	if(Color==1) //如果写入颜色为1
		Temp|=MaskTab[w];//在此处查表实现加速
	else         //如果写入颜色为0
		Temp&=~MaskTab[w];//在此处查表实现加速
	EA=0;
	Lcd_WriteCmd(y+0x80);        //行地址
	Lcd_WriteCmd(x+z);     //列地址
    Lcd_WriteData(Temp>>8);//先写入高8位，再写入低8位
    Lcd_WriteData(Temp&0x00ff);
	Lcd_WriteCmd(0x30);
	EA=1;
}

//向LCD指定位置画一条长度为Length的指定颜色的水平线********************
void Lcd_HoriLine(unsigned char x,unsigned char y,unsigned char Length,bool Color)
{
	unsigned char i;
	if(Length==0)
		return;
	for(i=0;i<Length;i++)
	{
		Lcd_PutPixel(x+i,y,Color);
	}
}

//指定左上角坐标,画一个3*3的实心正方形*******************
void Lcd_Rectangle(unsigned char x,unsigned char y,bool Color)
{
	Lcd_HoriLine(x,y,x+2,Color);
	Lcd_HoriLine(x,y+1,x+2,Color);
	Lcd_HoriLine(x,y+2,x+2,Color);
}
/*
//指定左上角坐标,画一个3*3的x******************************
void Lcd_XRectangle(unsigned char x,unsigned char y,bool Color)
{
	unsigned char i,j;
	for(i=0;i<=2;i++)
	{
		for(j=0;j<=2;j++)
		{
			if((i+j)%2)
			{
				Lcd_PutPixel(x+i,y+j,~Color);
			}
			else
				Lcd_PutPixel(x+i,y+j,Color);
		}
	}
}
*/	


//清屏指令,如果m=0全部空白,如果m=,全部填1.*******************
void Lcd_Clear(bool m)
{
	unsigned char x,y,ii;
	unsigned char Temp;
	if(m==0)
		Temp=0x00;
	else
		Temp=0xff;
	Lcd_WriteCmd(0x36);//扩充指令 绘图显示
	for(ii=0;ii<9;ii+=8)   
		for(y=0;y<0x20;y++)     
			for(x=0;x<8;x++)
			{ 	
				EA=0;
				Lcd_WriteCmd(y+0x80);        //行地址
				Lcd_WriteCmd(x+0x80+ii);     //列地址     
				Lcd_WriteData(Temp); //写数据 D15－D8 
				Lcd_WriteData(Temp); //写数据 D7－D0 
				EA=1;
			}
	Lcd_WriteCmd(0x30);
}
//lcd初始化*********************************************
void Lcd_Reset()
{  
	PSB=1;
	Lcd_WriteCmd(0x30);       //选择基本指令集
	Lcd_WriteCmd(0x0c);       //开显示(无游标、不反白)
	Lcd_WriteCmd(0x01);       //清除显示，并且设定地址指针为00H
	Lcd_WriteCmd(0x06);       //指定在资料的读取及写入时，设定游标的移动方向及指定显示的移位
}











