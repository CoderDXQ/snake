#include "stc.h"
#include "Lcd12864.h"
#include "Key.h"
#include "stdlib.h"
#define N 30
struct Food
{
	unsigned int x;
	unsigned char yes;
}food;//食物结构体
struct Snake
{
	int x[N];
	unsigned char direction;
	unsigned char last;
	unsigned char life;
}snake;//蛇结构体


unsigned char Score=0;


#define UP 4				   
#define DOWN 	2			  
#define LEFT 3			   
#define RIGHT 1		   
#define JUP 5
#define JDOWN 6

static unsigned int Seed = 1;
unsigned char KeyBuffer=0;
unsigned char mode = 0;
unsigned char Wait = 5;
unsigned int Last ;
unsigned char node;


/**********************************
定时器0的初始化,用于产生随机数
**********************************/        
void Timer0_Init()
{  
   TMOD = 0x01;
   TH0 = 0;  
   TL0 = 0; 
   TR0 = 1;
}  
/************************************
伪随机数发生器
*************************************/
int Random(void)
{
	srand(rand()+TL0);
	return rand()%513;
}
/****************************************
延时子程序
****************************************/
void delay(unsigned int t)
{  
	unsigned int i,j;
	for(i=0;i<t;i++)
		for(j=0;j<10;j++);    
}

/*********************************
初始化MPU
**********************************/
void InitCpu(void)
{
	TMOD=0x01;
	TH0=0;
	TL0=0;
	TR0=1;
	ET0=1;
	EA=1;
}

/***********************************
修改键盘缓存,当产生一个外部中断时运行
***********************************/
void Timer0Int(void) interrupt 1
{
	switch(OSReadKey())
	{
		case 1:
				KeyBuffer=1;						 //按键4
				break;
		case 2:
				KeyBuffer=2;						 //按键3
				break;
		case 3:
				KeyBuffer=3;					 //按键2
				break;
	  case 4:
				KeyBuffer=4;						 //按键1
				break;
		case 5:
				KeyBuffer=5;             //按键2和按键4
				break;
		case 6:
				KeyBuffer=6;						//按键1和按键4
				break;
		case 7:
				KeyBuffer=7;						//按键2和按键3
				break;
		case 8:
				KeyBuffer=8;						//按键1和按键3
				break;
		default:
				break;
	}
}

/******************************
画墙壁，初始化界面
*******************************/
 void DrawBoard(void)
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
	Lcd_HoriLine(84,30,44,1);
	Lcd_HoriLine(84,62,44,1);
	Lcd_WriteStr(6,2,"剩余");	
	if(mode!=1) Lcd_WriteStr(6,0,"得分");	
	if(mode==2)
	{
	for(n=1;n<21;n++)
	{
		Lcd_Rectangle(42,3*n,1);
	}
	for(n=1;n<28;n++)
	{
		Lcd_Rectangle(3*n,30,1);
	}
	}
}
/***************************
打印成绩
****************************/
void PrintScore(void)
{
	unsigned char Str[3];
//	Lcd_WriteStr(6,0,"得分");
	Str[0]=(Score/10)|0x30;//十位
	Str[1]=(Score%10)|0x30;//个位
	Str[2]=0;
	Lcd_WriteStr(6,1,Str);
}

/********************************
打印剩余步数
*********************************/
void PrintLast(void)
{
	unsigned char Str[4];
//	Lcd_WriteStr(6,2,"剩余");
	Str[0]=((Last-2)/100)|0x30;
	Str[1]=(((Last-2)%100)/10)|0x30;
	Str[2]=((Last-2)%10)|0x30;
	Str[3]=0;
	Lcd_WriteStr(6,3,Str);
}

/***********************************
游戏结束处理
************************************/
void GameOver(void)
{
	unsigned char i;
	DrawGame(food.x,0);//消隐出食物
	for(i=0;i<=node;i++)
	{
		DrawGame(snake.x[i],0);//消隐食物，蛇头已到墙壁内，故不用消去		
	}
		Lcd_WriteStr(1,1,"蛇死了");
	Lcd_WriteStr(1,2,"游戏结束");
	while(1);
}
/****************************************
游戏菜单栏
****************************************/
void menu(void)
{
	Lcd_Clear(0);
	KeyBuffer=0;	
	mode=0;
	Lcd_WriteStr(2,1,"等级选择");
	Lcd_WriteStr(3,2,"1~4");
	while(KeyBuffer==0);//如果键盘无输入,一直循环
	switch(KeyBuffer)
	{
		case 4 : 
			Wait = 20;break;
		case 3 :
			Wait = 10;break;
		case 2 :
			Wait = 5;break;
		default :
			Wait = 1;
	}
	KeyBuffer=0;
	delay(5000);
	Lcd_Clear(0);
	Lcd_WriteStr(1,1,"1.普通模式");
	Lcd_WriteStr(1,2,"2.双人模式");
	Lcd_WriteStr(1,3,"3.塔模式");
	while(KeyBuffer==0);
	switch(KeyBuffer)
	{
		case 2 :
			mode = 2;break;
		case 3 :
			mode = 1;break;
		default :
			mode = 0;
	}
	delay(5000);
}



	void CheckDirection(unsigned char direction,unsigned char n, int *x)
	{
		switch(direction)
		{
			case UP:
				x[n+1]=x[n]-27;
			break;
			case DOWN:
				x[n+1]=x[n]+27;
				break;
			case LEFT:
				x[n+1]=x[n]-1;
				break;
			default :
				x[n+1]=x[n]+1;
		}
	}
		
unsigned char CheckLife(unsigned char n, int *x)
	{
		unsigned char i,life=0;
		if((x[n+1]<0)||(x[n+1]>512))
			life++;
		else if((x[n+1]%27)==0)
			life++;
		else if((x[n+1]%27)==26)
			life++;
		
		for(i=0;i<=n;i++)
		{
			if(x[i]==x[n+1])
				life++;
		}		
		return life;
	}
	
	
/****************************************
检查是否需要生成食物,如需要,给food.x赋值
****************************************/
void CheckFood(unsigned char node2)
{
	
	if(food.yes==1)
	{	
		while(1)
		{
			unsigned char i,j=0;//可以改成bool型
			food.x=Random();
			for(i=0;i<=node;i++)
			{
				j=(snake.x[i]==food.x)+j;
			}
			if((food.x%27==0)||(food.x%27==26))
				j=j+1;
			if(mode==1)
			{
				for(i=0;i<=node2;i++)
				j=(snake.x[15+i]==food.x)+j;
			}
			if(j==0)
				break;
		}
		}
		DrawFood(food.x);
		food.yes=0;
}	

/********************************
游戏的具体过程，也是贪吃蛇算法的关键部分
*********************************/

void Game0()
{
	unsigned char i=0;
	node=2;//蛇当前长度
	food.yes=1;//1代表要生成食物
	KeyBuffer=0;
	delay(5000);
	snake.x[0]=31;
	DrawGame(snake.x[0],1);
	snake.x[1]=58;
	DrawGame(snake.x[1],1);	
	snake.x[2]=85;
	DrawGame(snake.x[2],1);	
	snake.direction=DOWN;
	snake.last=DOWN;
	snake.life=0;//0代表活着
	Last=283;
	
	while(1)
	{
		Last--;	
		CheckFood(0);		
		snake.last=snake.direction;
		switch(KeyBuffer)
		{
			case 3 :
				if(snake.last!=UP)
				{
					snake.last=snake.direction;
					snake.direction=DOWN;
				}
				break;
			case 4 :
				if(snake.last!=DOWN)
				{
					snake.last=snake.direction;
					snake.direction=UP;
				}
				break;
			case 2:
				if(snake.last!=RIGHT)
				{
					snake.last=snake.direction;
					snake.direction=LEFT;
				}
				break;
			case 1 :
				if(snake.last!=LEFT)
				{
					snake.last=snake.direction;
					snake.direction=RIGHT;
				}
				break;
				default:
					snake.direction=snake.last;
			}
			
			CheckDirection(snake.direction,node,snake.x);		
			snake.life=CheckLife(node,snake.x);

			if((snake.life!=0)||(Last<=1))
				GameOver();
	
			if(snake.x[node+1]==food.x)
			{
				food.yes=1;
			}
			
			DrawGame(snake.x[node+1],1);
			DrawGame(snake.x[0],food.yes);
			
			if(food.yes==1)
			{
				Last=(29-node)*10+2;
				Score++;
				node++;
			}
			else
			{
				for(i=0;i<=node;i++)
				{
					snake.x[i]=snake.x[i+1];
				}
			}
			
			PrintScore();
			PrintLast();
			delay(Wait*1000);
		}
	}
	
	
	
	
//*******************************************************************************************
void Game1()
{
	int *snake2x=&snake.x[15];
	unsigned char i,j;
	unsigned char snake2last=DOWN;
	unsigned char snake2direction=DOWN;
	unsigned char snake2life=0;
	unsigned char node2=2;
	node=2;//蛇当前长度	
	food.yes=1;//1代表要生成食物
	KeyBuffer=0;
	delay(5000);
	snake.x[0]=31;
	DrawGame(snake.x[0],1);
	snake.x[1]=58;
	DrawGame(snake.x[1],1);	
	snake.x[2]=85;
	DrawGame(snake.x[2],1);	
	snake2x[0]=49;
	DrawGame(snake2x[0],1);
	snake2x[1]=76;
	DrawGame(snake2x[1],1);
	snake2x[2]=103;
	DrawGame(snake2x[2],1);
	snake.direction=DOWN;
	snake.last=DOWN;
	snake.life=0;//0代表活着
	Last=303;
	
	while(1)
	{
		Last--;
		CheckFood(node2);
		
		if((KeyBuffer==4)||(KeyBuffer==6)||(KeyBuffer==8))
		{
			if(snake.last==1)
			{
				snake.direction=UP;
				snake.last=snake.direction;
			}
			else
			{
				snake.direction=snake.last-1;
				snake.last=snake.direction;
			}
		}
		else if((KeyBuffer==3)||(KeyBuffer==7)||(KeyBuffer==5))
		{
			if(snake.last==4)
			{
				snake.direction=RIGHT;
				snake.last=snake.direction;
			}
			else
			{
				snake.direction=snake.last+1;
				snake.last=snake.direction;
			}
		}
		else
			snake.direction =snake.last;
		
		if((KeyBuffer==2)||(KeyBuffer==7)||(KeyBuffer==8))
		{
			if(snake2last==1)
			{
				snake2direction=RIGHT;
				snake2last=snake2direction;
			}
			else
			{
				snake2direction=snake2last-1;
				snake2last=snake2direction;
			}
		}
		else if((KeyBuffer==1)||(KeyBuffer==5)||(KeyBuffer==6))
		{
			if(snake2last==4)
			{
				snake2direction=RIGHT;
				snake2last=snake2direction;
			}
			else
			{
				snake2direction=snake2last+1;
				snake2last=snake2direction;
			}
		}
		else
			snake2direction=snake2last;
		
		KeyBuffer=0;
		
		CheckDirection(snake.direction,node,snake.x);
		CheckDirection(snake2direction,node2,snake2x);
		snake.life=CheckLife(node,snake.x);
		for(i=0;i<=node2;i++)
		{
			if(snake2x[i]==snake.x[node+1])
				snake.life++;
		}
		
		snake2life=CheckLife(node2,snake2x);
		for(i=0;i<=node;i++)
		{
			if(snake.x[i]==snake2x[node2+1])
				snake2life++;
		}
		
		if((snake.life+snake2life)!=0)
		{
			if((snake.life==0)&&(snake2life!=0))			
				Lcd_WriteStr(1,1,"Snake1胜");
			else if((snake.life!=0)&&(snake2life==0))
				Lcd_WriteStr(1,1,"Snake2胜");
			else
			{
				if(node>node2)
					Lcd_WriteStr(1,1,"Snake1胜");
				else if(node<node2)
					Lcd_WriteStr(1,1,"Snake2胜");
				else
					Lcd_WriteStr(1,1,"平局");
			}
			Lcd_WriteStr(1,2,"游戏结束");
			while(1);
		}
		
		j=0;
		if(snake.x[node+1]==food.x)
		{
			food.yes=1;
			j=1;
		}	
		else if(snake2x[node2+1]==food.x)
		{
			food.yes=1;
			j=2;
		}
		
		
		DrawGame(snake.x[node+1],1);
		DrawGame(snake.x[0],(food.yes)&&(j==1));
		DrawGame(snake2x[node2+1],1);
		DrawGame(snake2x[0],(food.yes)&&(j==2));

		if((food.yes==1)&&(j==1))
		{
			Last=Last-node;
			node++;
		}
		else
		{
			for(i=0;i<=node;i++)
			{
				snake.x[i]=snake.x[i+1];
			}
		}

		if((food.yes==1)&&(j==2))
		{
			Last=Last-node2;
			node2++;
		}
		else
		{
			for(i=0;i<=node2;i++)
			{
				snake2x[i]=snake2x[i+1];
			}
		}
		PrintLast();
		delay(Wait*1000);
	}
}

/*		
		if((snake.x[node1+1]<0)||(snake.x[node1+1]>512))
			snake.life++;
		else if((snake.x[node1+1]%27)==0)
			snake.life++;
		else if((snake.x[node1+1]%27)==26)
			snake.life++;	
		for(i=0;i<=node;i++)
		{
			if(snake.x[i]==snake.x[node+1])
				snake.life++;
		}
		
		if((snake2x[node2+1]<0)||(snake2x[node2+1]>512))
			snake2life++;
		else if((snake2x[node2+1]%27)==0)
			snake2life++;
		else if((snake2x[node2+1]%27)==26)
			snake2life++;
*/
				
	
	
void Game2(void)
{
	unsigned char i=0;
	node=2;//蛇当前长度
	food.yes=1;//1代表要生成食物
	KeyBuffer=0;
	delay(5000);
	snake.x[0]=68;
	DrawGame2(snake.x[0],1,1);
	snake.x[1]=124;
	DrawGame2(snake.x[1],1,1);	
	snake.x[2]=180;
	DrawGame2(snake.x[2],1,1);	
	snake.direction=DOWN;
	snake.last=DOWN;
	snake.life=0;//0代表活着
	Last=283;
	
	while(1)
	{
		Last--;
		
		if(food.yes==1)
		{	
			while(1)
			{
				unsigned char i,j=0;//可以改成bool型
				food.x=Random();
				for(i=0;i<=node;i++)
				{
					j=(snake.x[i]==food.x)+j;
				}
				if((((food.x%4)%14)==0)||(((food.x%4)%14)==13))
					j=j+1;
				if(food.x>=500)
					j++;
				if(j==0)
					break;
			}
			DrawGame2(food.x,0,1);
			food.yes=0;
		}
		switch(KeyBuffer)
		{
			case 4:
				snake.direction=JUP;
				break;
			case 3:
				snake.direction=JDOWN;
				break;
			case 2:
				if(snake.last==1)
				{
					snake.direction=UP;
					snake.last=UP;
				}
				else
				{
					snake.direction=snake.last-1;
					snake.last=snake.direction;
				}
				break;
			case 1:
				if(snake.last==4)
				{
					snake.direction=RIGHT;
					snake.last=RIGHT;
				}
				else
				{
					snake.direction=snake.last+1;
					snake.last=snake.direction;
				}
				break;
				default :
					snake.direction=snake.last;
		}
		
		KeyBuffer=0;
		
		switch(snake.direction)
		{
				case UP:
					snake.x[node+1]=snake.x[node]-56;
				break;
				case DOWN:
					snake.x[node+1]=snake.x[node]+56;
					break;
				case LEFT:
					snake.x[node+1]=snake.x[node]-4;
					break;
				case RIGHT:
					snake.x[node+1]=snake.x[node]+4;
				break;
				case JUP:
					if(snake.x[node]%4==3)
						snake.x[node+1]=snake.x[node]-3;
						else
							snake.x[node+1]=snake.x[node]+1;
					break;
				case JDOWN:
				{
					if(snake.x[node]%4==0)
						snake.x[node+1]=snake.x[node]+3;
						else
							snake.x[node+1]=snake.x[node]-1;
					}
					
							
		}
		if((snake.x[node+1]<0)||(snake.x[node+1]>503))
			snake.life++;
		else if((snake.x[node+1]/4)%14==0)
			snake.life++;
		else if((snake.x[node+1]/4)%14==13)
			snake.life++;
		for(i=0;i<node;i++)
		{
			if(snake.x[i]==snake.x[node+1])
				snake.life++;
		}
		
		if((snake.life!=0)||(Last<=1))
			GameOver();
		
		if(snake.x[node+1]==food.x)
		{
			food.yes=1;
		}
		
		DrawGame2(snake.x[node+1],1,1);
		DrawGame2(snake.x[0],1,food.yes);
		
		if(food.yes==1)
		{
			Last=(29-node)*10+2;
			Score++;
			node++;
		}
		else
		{
			for(i=0;i<=node;i++)
			{
				snake.x[i]=snake.x[i+1];
			}
		}
		
		PrintScore();
		PrintLast();
		delay(Wait*1000);
	}
}
		

/**********************************
	主程序,运行时从这里开始
**********************************/	
void Main()
{  
	InitCpu();//初始化CPU
	Timer0_Init();
	Lcd_Reset(); //初始化LCD屏
	Lcd_WriteStr(2,1,"贪吃蛇");
	delay(5000);
	delay(5000);
	Lcd_WriteCmd(0x01);   //清除显示，并且设定地址指针为00H
	Lcd_Clear(0);//清屏
	menu();
	Lcd_WriteCmd(0x01);
	DrawBoard();//画界面
	switch(mode)
	{
		case 2:
			Game2();
			break;
		case 1:
			Game1();
			break;
		default:
			Game0();
	}
	GameOver();//游戏结束
	while(1);//要想重玩，只能重启，可继续完善该游戏
}

