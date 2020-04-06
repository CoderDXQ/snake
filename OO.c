#include "stc.h"
#include "Lcd12864.h"
#include "Key.h"


 int Last = 189;//定义全局变量Last剩余步数
 static unsigned int seed = 1;

 //伪随机数发生函数***********************
unsigned int Random(void)
 {
	 seed = seed * 1103515245 + 12345;
	 return (unsigned int)(seed/65536)%513;
 }
 
 //延时子程序*******************************
 void delay(unsigned char t)
 {
	 unsigned char i,j;
	 for (i=0;i<t;i++)
	 {
		 for(j=0;j<10;j++);
	 }
 }
 
 //单片机初始化******************************
 void InitCpu(void)
 {
	 TMOD=0x01;
	 TH0=0;
	 TL0=0;
	 TR0=1;
	 ET0=1;
	 EA=1;
 }
 
 //定义游戏过程需要的结构体
 #define N 20  //定义蛇的最长长度为20
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
	 int d;//定义蛇的下一步目标direction
 } snake;
unsigned char Score=0;
unsigned char Wait=5;//等待时间,级别越高等待越少
unsigned char KeyBuffer=0;
 //定义外部产生中断时,把键盘的数值记录入KeyBuffer****************
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
 //画游戏画面**************************************************
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
	Lcd_WriteStr(6,2,"剩余");	
	if(mode!=1) Lcd_WriteStr(6,0,"分数");	
	if(mode==1)
	{
	for(n=1;n<21;n++)
	{
		Lcd_Rectangle(42,3*n,1);
	}
	}
}
//打印分数和剩余步数****************************************
void Printf(void)
{
	
	unsigned char Str1[4];
	if(mode!=1)
	{
		unsigned char Str0[3];
		Str0[0]=(Score/10)|0x30;//十位
		Str0[1]=(Score%10)|0x30;//个位
		Str0[2]=0;
		Lcd_WriteStr(7,1,Str0);	
	}
	Str1[0]=(Last/100)|0x30;//百位
	Str1[1]=(Last/10)|0x30;//十位
	Str1[2]=(Last%10)|0x30;//个位
	Str1[3]=0;
	Lcd_WriteStr(7,1,Str1);	
}

//主菜单***********************************************
void menu(void)
{
	Lcd_Clear(0);
	KeyBuffer=0;	
	mode=0;
//	Lcd_WriteStr(3,1,"开始游戏");
	Lcd_WriteStr(3,2,"等级选择");
	while(KeyBuffer==0);//如果键盘无输入,一直循环
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
	Lcd_WriteStr(2,1,"普通模式");
	Lcd_WriteStr(2,2,"双人模式");
	Lcd_WriteStr(2,3,"塔模式");
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
		Lcd_WriteStr(3,1,"等级");
*/

//游戏结束********************************************
void GameOver(void)
{
	Lcd_WriteStr(1,2,"游戏结束");
	while(1);
}	

//游戏0**************************************************
void Game0()
{
	unsigned char n=2;//蛇当前长度
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
		//检查是否要生成食物
		if(food.yes==1)
		{	
			while(1)
			{
				unsigned char i,j=0;//可以改成bool型
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


//主程序************************************************
void Main()
{  
	InitCpu();//初始化CPU
	Lcd_Reset(); //初始化LCD屏
	Lcd_WriteStr(2,1,"贪吃蛇");
	delay(5000);
	delay(5000);
	Lcd_WriteCmd(0x01);   //清除显示，并且设定地址指针为00H
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
//	Lcd_Clear(0);//清屏
//	menu();
//	DrawBoard();//画界面
//	GamePlay();//玩游戏
	GameOver();//游戏结束
	
	*/
	while(1);//要想重玩，只能重启.
}

			 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 