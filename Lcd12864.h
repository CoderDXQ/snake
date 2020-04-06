#ifndef __lcd12864_H__
#define __lcd12864_H__
sbit RS=P2^6; //���е�ָ��/����ѡ���ź�, H����, L����
sbit RW=P2^5; //���ж�дѡ���ź�, H��, Lд
sbit E=P2^7; //����ʹ�ܶ�, H��Ч, L��Ч
sbit PSB=P3^2; //��/���ӿ�ѡ��, H��,L��
sbit RET=P3^4; //��λ, L��Ч
#define  LcdData P0
typedef enum {false = 0,true = 1}bool;
unsigned char Check_Busy(void);
void Lcd_WriteData(unsigned char);
unsigned char Lcd_ReadData(void);
void Lcd_WriteCmd(unsigned char);
void Lcd_PutPixel(unsigned char,unsigned char,unsigned char);
//unsigned char Lcd_ReadPixel(unsigned char,unsigned char);
void Lcd_HoriLine(unsigned char,unsigned char,unsigned char Length,unsigned char Color);
//void Lcd_VertLine(unsigned char x,unsigned char y,unsigned char Length,unsigned char Color);
//void Lcd_Line(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2,unsigned char Color);
void DrawGame( int n,unsigned char Color);
void DrawGame2( int n,unsigned char m,unsigned char Color);
void DrawFood( int n);
//void DrawFood(unsigned char n);
void Lcd_Rectangle(unsigned char x,unsigned char y,unsigned char Color);
//void Lcd_Circle(unsigned char x,unsigned char y,unsigned char r,unsigned char Color);
void Lcd_Clear(unsigned char);
void Lcd_WriteStr(unsigned char,unsigned char,unsigned char *);
void Lcd_Reset(void);

#endif
