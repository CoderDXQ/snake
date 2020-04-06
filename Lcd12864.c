#include "stc.h"
#include "intrins.h"
#include "Lcd12864.h"
/*************
测试LCD是否处于忙状态
如果忙则返回0x80，否则返回0
**************/
//typedef enum {false = 0,true = 1}bool;

bool l=0;

unsigned char Lcd_CheckBusy(void)
{
    unsigned char Busy;
	LcdData=0xff;
    RS=0;
    RW=1;
    E=1;
    _nop_(); //延时一个机器周期
    Busy=LcdData&0x80;
    E=0;
    return Busy;
}

/*********************************
向LCD写入字节数据
**********************************/
void Lcd_WriteData(unsigned char Data)
{  
	while(Lcd_CheckBusy());
	RS=1;
	RW=0;
	E=0;
	_nop_();  
	_nop_(); 
	LcdData=Data;
	E=1;
	_nop_();
	_nop_();
	E=0;
}

/***********************************
从LCD中读出数据
************************************/

unsigned char Lcd_ReadData(void)
{
	unsigned char Temp;
	while(Lcd_CheckBusy());
 	LcdData=0xff;
 	RS=1;
	RW=1;
	E=1;
	_nop_();
   	Temp=LcdData;
   	E=0;
   	return Temp;
}

/************************************
想LCD中写入指令代码
**************************************/
void Lcd_WriteCmd(unsigned char CmdCode)
{  
	while(Lcd_CheckBusy());
   	RS=0;
   	RW=0;
   	E=0;
   	_nop_();  
	_nop_();
   	LcdData=CmdCode;
   	_nop_(); 
	_nop_();
   	E=1;
   	_nop_();  
	_nop_();
   	E=0;
}

/*************************************
向LCD指定起始位置写入一个字符串
*************************************/
void Lcd_WriteStr(unsigned char x,unsigned char y,unsigned char *Str)
{
	if((y>3)||(x>7))
		return;//如果指定位置不在显示区域内，则不做任何写入直接返回
	EA=0;
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
	EA=1;
}

/**************************************
为加速逻辑运算而设置的掩码表，这是以牺牲空间而换取时间的办法
***************************************/
code unsigned int MaskTab[]={0x0001,0x0002,0x0004,0x0008,0x0010,0x0020,0x0040,0x0080,
							 0x0100,0x0200,0x0400,0x0800,0x1000,0x2000,0x4000,0x8000};
/***************************************
向LCD指定坐标写入一个象素,象素颜色有两种，0代表白（无显示），1代表黑（有显示）
****************************************/
void Lcd_PutPixel(unsigned char x,unsigned char y,unsigned char Color)
{
	unsigned char z,w;
	unsigned int Temp;
	if(x>=128||y>=64)
		return;
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

/******************************************
从LCD指定坐标读取象素颜色值
*******************************************/
/*unsigned char Lcd_ReadPixel(unsigned char x,unsigned char y)
{
	unsigned char z,w;
	unsigned int Temp;
	x=x%128;
	y=y%64;
	w=15-x%16;//确定对这个字的第多少位进行操作
	x=x/16;//确定为一行上的第几字

	if(y<32) //如果为上页
		z=0x80;
	else     //否则如果为下页
		z=0x88;

	y=y%32;
	Lcd_WriteCmd(0x36);
	Lcd_WriteCmd(y+0x80);        //行地址
	Lcd_WriteCmd(x+z);     //列地址 
	Temp=Lcd_ReadData();//先空读一次
	Temp=(unsigned int)Lcd_ReadData()<<8;//再读出高8位
	Temp|=(unsigned int)Lcd_ReadData();//再读出低8位

	if((Temp&&MaskTab[w])==0)
		return 0;
	else
		return 1;
}*/

/***************************************
向LCD指定位置画一条长度为Length的指定颜色的水平线
****************************************/
void Lcd_HoriLine(unsigned char x,unsigned char y,unsigned char Length,unsigned char Color)
{
	unsigned char i;
	if(Length==0)
		return;
	for(i=0;i<Length;i++)
	{
		Lcd_PutPixel(x+i,y,Color);
	}
}

void DrawGame( int n,unsigned char Color)
{
	unsigned char x,y;
	x=((n%27)*3)+3;
	y=((n/27)*3)+3;
	Lcd_Rectangle(x,y,Color);
}

void DrawGame2( int n,unsigned char m,unsigned char Color)
{
	unsigned char x,y;
	x=(n/4)%14;
	if(((n%4)==1)||((n%4)==2))
		x=x+13;
		x=x*3+3;
	y=(n/56);
	if(((n%4)==2)||((n%4)==3))
		y=y+10;
		y=y*3+3;
	if(m==1)
		Lcd_Rectangle(x,y,Color);
	else
	{
			Lcd_PutPixel(x+1,y,1);
			Lcd_PutPixel(x+1,y+2,1);
			Lcd_HoriLine(x,y+1,3,1);
	}
}	

void DrawFood( int n)
{
	int x,y;
	x=((n%27)*3)+3;
	y=((n/27)*3)+3;
	Lcd_PutPixel(x+1,y,1);
	Lcd_PutPixel(x+1,y+2,1);
	Lcd_HoriLine(x,y+1,3,1);
}
/*******************************************
向LCD指定左上角坐标和右下角坐标画一个指定颜色的矩形
********************************************/
void Lcd_Rectangle(unsigned char x,unsigned char y,unsigned char Color)
{
	Lcd_HoriLine(x,y,3,Color);
	Lcd_HoriLine(x,y+1,3,Color);
	Lcd_HoriLine(x,y+2,3,Color);
}

/*****************************************
清除Lcd全屏，如果清除模式Mode为0，则为全屏清除为颜色0（无任何显示）
否则为全屏清除为颜色1(全屏填充显示)
******************************************/
void Lcd_Clear(unsigned char Mode)
{
	unsigned char x,y,ii;
	unsigned char Temp;
	if(Mode%2==0)
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

/****************************************
LCD初始化
*****************************************/
void Lcd_Reset()
{  
	PSB=1;
	Lcd_WriteCmd(0x30);       //选择基本指令集
	Lcd_WriteCmd(0x0c);       //开显示(无游标、不反白)
	Lcd_WriteCmd(0x01);       //清除显示，并且设定地址指针为00H
	Lcd_WriteCmd(0x06);       //指定在资料的读取及写入时，设定游标的移动方向及指定显示的移位
}

