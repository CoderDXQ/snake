#include "stc.h"
#include "Lcd12864.h"
#include "Key.h"


 unsigned char Last = 189;//?¨ò?è???±?á?Lastê?óà2?êy
 static unsigned int seed = 1;

 //?±???úêy·?éúo¯êy***********************
 int Random(void)
 {
	 seed = seed * 1103515245 + 12345;
	 return (unsigned int)(seed/65536)%550;
 }
 
 //?óê±×ó3ìDò*******************************
 void delay(unsigned char t)
 {
	 unsigned char i,j;
	 for (i=0;i<t;i++)
	 {
		 for(j=0;j<10;j++);
	 }
 }
 
 //µ????ú3?ê??¯******************************
 void InitCpu(void)
 {
	 TMOD=0x01;
	 TH0=0;
	 TL0=0;
	 TR0=1;
	 ET0=1;
	 EA=1;
 }
 
 //?¨ò?ó??·1y3ìDèòaµ??á11ì?
 #define N 20  //?¨ò?é?µ?3¤?è?a20
 unsigned int food;
 struct Snake
 {
	 unsigned int x[N];
	 unsigned int d;//?¨ò?é?µ???ò?2???±êdirection
 }snake;
unsigned char Score=0;
unsigned char Wait=5;//µè'yê±??,??±e????µè'y??éù
unsigned char KeyBuffer=0;
 void Timer0Int(void) interrupt 1
 {
	 switch(Key_Read())
	 {
		 case 1:
		 KeyBuffer=1;break;
		 case 2:
		 KeyBuffer=2;break;
		 case 3:
		 KeyBuffer=3;break;
		 case 4:
		 KeyBuffer=4;break;
		 default:
			 break;
	 }
 }
 //?-ó??·?-??
 void DrawRect(void)
 {
	unsigned char n;
	for(n=0;n<31;n++)
	{
		Lcd_Rectangle(3*n,0,1);
		Lcd_Rectangle(3*n,60,1);
	}
	for(n=1;n<21;n++)
	{
		Lcd_Rectangle(0,3*n,1);
		Lcd_Rectangle(90,3*n,1);		
	}
	Lcd_HoriLine(93,31,35,1);
	Lcd_HoriLine(93,63,35,1);
	Lcd_WriteStr(6,2,"ê?óà");	
	if(mode!=1) Lcd_WriteStr(6,0,"·?êy");	
	if(mode==1)
	{
	for(n=1;n<21;n++)
	{
		Lcd_Rectangle(45,3*n,1);
	}
	}
}
//'òó?·?êyoíê?óà2?êy
void Printf(void)
{
	
	unsigned char Str1[4];
	if(mode!=1)
	{
		unsigned char Str0[3];
		Str0[0]=(Score/10)|0x30;//ê???
		Str0[1]=(Score%10)|0x30;//????
		Str0[2]=0;
		Lcd_WriteStr(7,1,Str0);	
	}
	Str1[0]=(Last/100)|0x30;//°ù??
	Str1[1]=(Last/10)|0x30;//ê???
	Str1[2]=(Last%10)|0x30;//????
	Str1[3]=0;
	Lcd_WriteStr(7,1,Str1);	
}

	
		 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 