#include "stc.h"
#include "intrins.h"
#include "Lcd12864.h"
/*************
����LCD�Ƿ���æ״̬
���æ�򷵻�0x80�����򷵻�0
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
    _nop_(); //��ʱһ����������
    Busy=LcdData&0x80;
    E=0;
    return Busy;
}

/*********************************
��LCDд���ֽ�����
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
��LCD�ж�������
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
��LCD��д��ָ�����
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
��LCDָ����ʼλ��д��һ���ַ���
*************************************/
void Lcd_WriteStr(unsigned char x,unsigned char y,unsigned char *Str)
{
	if((y>3)||(x>7))
		return;//���ָ��λ�ò�����ʾ�����ڣ������κ�д��ֱ�ӷ���
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
Ϊ�����߼���������õ�����������������ռ����ȡʱ��İ취
***************************************/
code unsigned int MaskTab[]={0x0001,0x0002,0x0004,0x0008,0x0010,0x0020,0x0040,0x0080,
							 0x0100,0x0200,0x0400,0x0800,0x1000,0x2000,0x4000,0x8000};
/***************************************
��LCDָ������д��һ������,������ɫ�����֣�0����ף�����ʾ����1����ڣ�����ʾ��
****************************************/
void Lcd_PutPixel(unsigned char x,unsigned char y,unsigned char Color)
{
	unsigned char z,w;
	unsigned int Temp;
	if(x>=128||y>=64)
		return;
	w=15-x%16;//ȷ��������ֵĵڶ���λ���в���
	x=x/16;//ȷ��Ϊһ���ϵĵڼ���

	if(y<32) //���Ϊ��ҳ
		z=0x80;
	else     //�������Ϊ��ҳ
		z=0x88;

	y=y%32;
	EA=0;
	Lcd_WriteCmd(0x36);
	Lcd_WriteCmd(y+0x80);        //�е�ַ
	Lcd_WriteCmd(x+z);     //�е�ַ 
	Temp=Lcd_ReadData();//�ȿն�һ��
	Temp=(unsigned int)Lcd_ReadData()<<8;//�ٶ�����8λ
	Temp|=(unsigned int)Lcd_ReadData();//�ٶ�����8λ
	EA=1;
	if(Color==1) //���д����ɫΪ1
		Temp|=MaskTab[w];//�ڴ˴����ʵ�ּ���
	else         //���д����ɫΪ0
		Temp&=~MaskTab[w];//�ڴ˴����ʵ�ּ���
	EA=0;
	Lcd_WriteCmd(y+0x80);        //�е�ַ
	Lcd_WriteCmd(x+z);     //�е�ַ
    Lcd_WriteData(Temp>>8);//��д���8λ����д���8λ
    Lcd_WriteData(Temp&0x00ff);
	Lcd_WriteCmd(0x30);
	EA=1;
}

/******************************************
��LCDָ�������ȡ������ɫֵ
*******************************************/
/*unsigned char Lcd_ReadPixel(unsigned char x,unsigned char y)
{
	unsigned char z,w;
	unsigned int Temp;
	x=x%128;
	y=y%64;
	w=15-x%16;//ȷ��������ֵĵڶ���λ���в���
	x=x/16;//ȷ��Ϊһ���ϵĵڼ���

	if(y<32) //���Ϊ��ҳ
		z=0x80;
	else     //�������Ϊ��ҳ
		z=0x88;

	y=y%32;
	Lcd_WriteCmd(0x36);
	Lcd_WriteCmd(y+0x80);        //�е�ַ
	Lcd_WriteCmd(x+z);     //�е�ַ 
	Temp=Lcd_ReadData();//�ȿն�һ��
	Temp=(unsigned int)Lcd_ReadData()<<8;//�ٶ�����8λ
	Temp|=(unsigned int)Lcd_ReadData();//�ٶ�����8λ

	if((Temp&&MaskTab[w])==0)
		return 0;
	else
		return 1;
}*/

/***************************************
��LCDָ��λ�û�һ������ΪLength��ָ����ɫ��ˮƽ��
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
��LCDָ�����Ͻ���������½����껭һ��ָ����ɫ�ľ���
********************************************/
void Lcd_Rectangle(unsigned char x,unsigned char y,unsigned char Color)
{
	Lcd_HoriLine(x,y,3,Color);
	Lcd_HoriLine(x,y+1,3,Color);
	Lcd_HoriLine(x,y+2,3,Color);
}

/*****************************************
���Lcdȫ����������ģʽModeΪ0����Ϊȫ�����Ϊ��ɫ0�����κ���ʾ��
����Ϊȫ�����Ϊ��ɫ1(ȫ�������ʾ)
******************************************/
void Lcd_Clear(unsigned char Mode)
{
	unsigned char x,y,ii;
	unsigned char Temp;
	if(Mode%2==0)
		Temp=0x00;
	else
		Temp=0xff;
	Lcd_WriteCmd(0x36);//����ָ�� ��ͼ��ʾ
	for(ii=0;ii<9;ii+=8)   
		for(y=0;y<0x20;y++)     
			for(x=0;x<8;x++)
			{ 	
				EA=0;
				Lcd_WriteCmd(y+0x80);        //�е�ַ
				Lcd_WriteCmd(x+0x80+ii);     //�е�ַ     
				Lcd_WriteData(Temp); //д���� D15��D8 
				Lcd_WriteData(Temp); //д���� D7��D0 
				EA=1;
			}
	Lcd_WriteCmd(0x30);
}

/****************************************
LCD��ʼ��
*****************************************/
void Lcd_Reset()
{  
	PSB=1;
	Lcd_WriteCmd(0x30);       //ѡ�����ָ�
	Lcd_WriteCmd(0x0c);       //����ʾ(���αꡢ������)
	Lcd_WriteCmd(0x01);       //�����ʾ�������趨��ַָ��Ϊ00H
	Lcd_WriteCmd(0x06);       //ָ�������ϵĶ�ȡ��д��ʱ���趨�α���ƶ�����ָ����ʾ����λ
}

