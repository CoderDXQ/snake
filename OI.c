#include"stc.h"
#include"intrins.h"
#include"Lcd12864.h"
//#include<stdbool.h>
//typedef enum {false = 0,true = 1}bool;

//bool Lcd_CheckBusy(void);
//���lcd�Ƿ���æ,�����æ����1,��æ����0*************
bool Lcd_CheckBusy(void)
{
	bool busy=false;
	LcdData=0xff;//LcdDate����P0��
	RS=0;//P2.6�ڣ�ѡ������(1)/ָ��(0)
	RW=1;//P2.5�ڣ�ѡ���(1)/д(0)
	E=1;//P2.7��,ѡ����Ч(1)/��Ч(0)
	_nop_();//������intrins.h��,��ʱһ����������
	busy=LcdData&&0x80;//���P0.0�˿�,�����1��æ,����ʹ��
	E=0;
	return busy;
}
//��lcd��д��һ���ֽڵ�����***********************************
void Lcd_WriteData(unsigned char Data)
{  
	while(Lcd_CheckBusy());
	RS=1;//����
	RW=0;//д
	E=0;//��ʹ��
	_nop_();  
	_nop_(); 
	LcdData=Data;//д����
	E=1;//��ʹ��
	_nop_();
	_nop_();
	E=0;//��ʹ��
}
//��lcd�ж�ȡһ���ֽ�����**************************************
unsigned char Lcd_ReadData(void)
{
	unsigned char Temp;
	while(Lcd_CheckBusy());
 	LcdData=0xff;//д��ȫ1
 	RS=1;//����
	RW=1;//��
	E=1;//��ʹ��
	_nop_();
   	Temp=LcdData;//������
   	E=0;//��ʹ��
   	return Temp;
}
//��lcd��дһ�ֽ�ָ��*******************************************
void Lcd_WriteCmd(unsigned char Code)
{  
	while(Lcd_CheckBusy());
   	RS=0;//ָ��
   	RW=0;//д
   	E=0;//��ʹ��
   	_nop_();  
		_nop_();
   	LcdData=Code;//дָ��
   	_nop_(); 
		_nop_();
   	E=1;//��ʹ��
   	_nop_();  
		_nop_();
   	E=0;//��ʹ��
}
//��ָ��λ��д���ַ���*******************************************
void Lcd_WriteStr(unsigned char x,unsigned char y,unsigned char *Str)
{
	if((y>3)||(x>7))
		return;//���ָ��λ�ò�����ʾ�����ڣ������κ�д��ֱ�ӷ���
	EA=0;//���ж�
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
	EA=1;//���ж�
}
//�����,�����ҵ�Ҫ���������ص�************************************
code unsigned int MaskTab[]={0x0001,0x0002,0x0004,0x0008,0x0010,0x0020,0x0040,0x0080,
							 0x0100,0x0200,0x0400,0x0800,0x1000,0x2000,0x4000,0x8000};

//��LCDָ������д��һ������,������ɫ�����֣�0����ף�����ʾ����1����ڣ�����ʾ��
void Lcd_PutPixel(unsigned char x,unsigned char y,bool Color)
{
	unsigned char z,w;
	unsigned int Temp;
	if(x>=128||y>=64)
		return;
	//Color=Color%2;
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

//��LCDָ��λ�û�һ������ΪLength��ָ����ɫ��ˮƽ��********************
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

//ָ�����Ͻ�����,��һ��3*3��ʵ��������*******************
void Lcd_Rectangle(unsigned char x,unsigned char y,bool Color)
{
	Lcd_HoriLine(x,y,x+2,Color);
	Lcd_HoriLine(x,y+1,x+2,Color);
	Lcd_HoriLine(x,y+2,x+2,Color);
}
/*
//ָ�����Ͻ�����,��һ��3*3��x******************************
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


//����ָ��,���m=0ȫ���հ�,���m=,ȫ����1.*******************
void Lcd_Clear(bool m)
{
	unsigned char x,y,ii;
	unsigned char Temp;
	if(m==0)
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
//lcd��ʼ��*********************************************
void Lcd_Reset()
{  
	PSB=1;
	Lcd_WriteCmd(0x30);       //ѡ�����ָ�
	Lcd_WriteCmd(0x0c);       //����ʾ(���αꡢ������)
	Lcd_WriteCmd(0x01);       //�����ʾ�������趨��ַָ��Ϊ00H
	Lcd_WriteCmd(0x06);       //ָ�������ϵĶ�ȡ��д��ʱ���趨�α���ƶ�����ָ����ʾ����λ
}











