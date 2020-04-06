#ifndef __lcd12864_H__
#define __lcd12864_H__
sbit RS=P2^6; //并行的指令/数据选择信号, H数据, L命令
sbit RW=P2^5; //并行读写选择信号, H读, L写
sbit E=P2^7; //并行使能端, H有效, L无效
sbit PSB=P3^2; //并/串接口选择, H并,L串
sbit RET=P3^4; //复位, L有效
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
