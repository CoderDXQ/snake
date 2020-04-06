#include "stc.h"
#include "Lcd12864.h"
#include "Key.h"


 int Last = 189;//����ȫ�ֱ���Lastʣ�ಽ��
 static unsigned int seed = 1;

 //α�������������***********************
unsigned int Random(void)
 {
	 seed = seed * 1103515245 + 12345;
	 return (unsigned int)(seed/65536)%513;
 }
 
 //��ʱ�ӳ���*******************************
 void delay(unsigned char t)
 {
	 unsigned char i,j;
	 for (i=0;i<t;i++)
	 {
		 for(j=0;j<10;j++);
	 }
 }
 
 //��Ƭ����ʼ��******************************
 void InitCpu(void)
 {
	 TMOD=0x01;
	 TH0=0;
	 TL0=0;
	 TR0=1;
	 ET0=1;
	 EA=1;
 }
 
 //������Ϸ������Ҫ�Ľṹ��
 #define N 20  //�����ߵ������Ϊ20
struct Food
 {
	 unsigned int x;
	 bool yes;
 } food;	 
 struct Snake
 {
	 int x[N];
	 unsigned char life;
	 unsigned char l;
	 int d;//�����ߵ���һ��Ŀ��direction
 } snake;
unsigned char Score=0;
unsigned char Wait=5;//�ȴ�ʱ��,����Խ�ߵȴ�Խ��
unsigned char KeyBuffer=0;
 //�����ⲿ�����ж�ʱ,�Ѽ��̵���ֵ��¼��KeyBuffer****************
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
		 case 5:
		 KeyBuffer=5;break;
		 case 6:
		 KeyBuffer=6;break;
		 case 7:
		 KeyBuffer=7;break;
		 case 8:
		 KeyBuffer=8;break;
		 default:
			 KeyBuffer=0;
			 break;
	 }
 }
 //����Ϸ����**************************************************
 void DrawRect(void)
 {
	unsigned char n;
	for(n=1;n<28;n++)
	{
		Lcd_Rectangle(3*n,0,1);
		Lcd_Rectangle(3*n,60,1);
	}
	for(n=0;n<21;n++)
	{
		Lcd_Rectangle(3,3*n,1);
		Lcd_Rectangle(81,3*n,1);		
	}
	Lcd_HoriLine(87,31,44,1);
	Lcd_HoriLine(87,63,44,1);
	Lcd_WriteStr(6,2,"ʣ��");	
	if(mode!=1) Lcd_WriteStr(6,0,"����");	
	if(mode==1)
	{
	for(n=1;n<21;n++)
	{
		Lcd_Rectangle(42,3*n,1);
	}
	}
}
//��ӡ������ʣ�ಽ��****************************************
void Printf(void)
{
	
	unsigned char Str1[4];
	if(mode!=1)
	{
		unsigned char Str0[3];
		Str0[0]=(Score/10)|0x30;//ʮλ
		Str0[1]=(Score%10)|0x30;//��λ
		Str0[2]=0;
		Lcd_WriteStr(7,1,Str0);	
	}
	Str1[0]=(Last/100)|0x30;//��λ
	Str1[1]=(Last/10)|0x30;//ʮλ
	Str1[2]=(Last%10)|0x30;//��λ
	Str1[3]=0;
	Lcd_WriteStr(7,1,Str1);	
}

//���˵�***********************************************
void menu(void)
{
	Lcd_Clear(0);
	KeyBuffer=0;	
	mode=0;
//	Lcd_WriteStr(3,1,"��ʼ��Ϸ");
	Lcd_WriteStr(3,2,"�ȼ�ѡ��");
	while(KeyBuffer==0);//�������������,һֱѭ��
	switch(KeyBuffer)
	{
		case 1 : 
			Wait = 5;break;
		case 2 :
			Wait = 4;break;
		case 3 :
			Wait = 3;break;
		default :
			Wait = 2;
	}
	KeyBuffer=0;
	Lcd_Clear(0);
	Lcd_WriteStr(2,1,"��ͨģʽ");
	Lcd_WriteStr(2,2,"˫��ģʽ");
	Lcd_WriteStr(2,3,"��ģʽ");
	while(KeyBuffer==0);
	switch(KeyBuffer)
	{
		case 3 :
			mode = 2;break;
		case 2 :
			mode = 1;break;
		default :
			mode = 0;
	}
	Lcd_Clear(0);
}

void DrawGame(unsigned n,bool Color)
{
	unsigned x,y;
	x=((n%27)*3)+3;
	y=((n/27)*3)+3;
	Lcd_Rectangle(x,y,Color);
}
	
	
/*
	while((KeyBuffer<1)||(KeyBuffer>2));
	Lcd_Clear(0);
	if(KeyBuffer==2)
	{
		Lcd_WriteStr(3,1,"�ȼ�");
*/

//��Ϸ����********************************************
void GameOver(void)
{
	Lcd_WriteStr(1,2,"��Ϸ����");
	while(1);
}	

//��Ϸ0**************************************************
void Game0()
{
	unsigned char n=2;//�ߵ�ǰ����
	unsigned char i=0;
//	food.x=Random();
	food.yes=1;
//	snake.x=0[20];
	KeyBuffer=0;
	snake.x[0]=31;
	DrawGame(snake.x[0],1);
	snake.x[1]=58;
	DrawGame(snake.x[1],1);	
	snake.x[2]=85;
	DrawGame(snake.x[2],1);	
	snake.d=2;
	snake.l=2;
	snake.life=0;
	
	while(1)
	{
		//����Ƿ�Ҫ����ʳ��
		if(food.yes==1)
		{	
			while(1)
			{
				unsigned char i,j=0;//���Ըĳ�bool��
				food.x=Random();
				Last--;
				for(i=0;i<=n;i++)
				{
					j=(snake.x[i]&&food.x)+j;
				}
				if((food.x%27==0)||(food.x%27==26))
					j=j+1;
				if(j==1)
					break;
			}
			DrawGame(food.x,1);
			food.yes=0;
		}
		switch(KeyBuffer)
		{
			case 1 :
				if(snake.l!=2)
				{
					snake.l=snake.d;
					snake.d=1;
				}
				break;
			case 2 :
				if(snake.l!=1)
				{
					snake.l=snake.d;
					snake.d=2;
				}
				break;
			case 3:
				if(snake.l!=4)
				{
					snake.l=snake.d;
					snake.d=3;
				}
				break;
			case 4 :
				if(snake.l!=3)
				{
					snake.l=snake.d;
					snake.d=4;
				}
				break;
				default:
					snake.d=snake.l;
			}
			switch(snake.d)
			{
				case 1:
					snake.x[n+1]=snake.x[n]-27;
				break;
				case 2:
					snake.x[n+1]=snake.x[n]+27;
					break;
				case 3:
					snake.x[n+1]=snake.x[n]-1;
					break;
				default :
					snake.x[n+1]=snake.x[n]+1;
			}
			
//			snake.life=0;
			if((snake.x[n+1]<0)||(snake.x[n+1]>512))
				snake.life++;
			if((snake.x[n+1]%27)==0)
				snake.life++;
			if((snake.x[n+1]%27)==26)
				snake.life++;
			for(i=0;i<=n;i++)
			{
				if(snake.x[i]==snake.x[n+1])
					snake.life++;
			}
			if(snake.life!=0||(Last<1))
				GameOver();
			
			if(snake.x[n+1]==food.x)
				food.yes=1;
			
			DrawGame(snake.x[n+1],1);
			DrawGame(snake.x[0],food.yes);
			
			if(food.yes==1)
			{
				Last=Last-n;
				Score++;
				n++;
			}
			else
			{
				for(i=0;i<=n;i++)
				{
					snake.x[i]=snake.x[i+1];
				}
			}
			
			Printf();
		}
	}


//������************************************************
void Main()
{  
	InitCpu();//��ʼ��CPU
	Lcd_Reset(); //��ʼ��LCD��
	Lcd_WriteStr(2,1,"̰����");
	delay(5000);
	delay(5000);
	Lcd_WriteCmd(0x01);   //�����ʾ�������趨��ַָ��Ϊ00H
	menu();
	DrawRect();
	Game0();
	GameOver();
	/*
	switch(mode)
	{
		case 2 :
			Game2();break;
		case 1 :
			Game1();break;
		default:
			Game0();
	}
//	Lcd_Clear(0);//����
//	menu();
//	DrawBoard();//������
//	GamePlay();//����Ϸ
	GameOver();//��Ϸ����
	
	*/
	while(1);//Ҫ�����棬ֻ������.
}

			 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 