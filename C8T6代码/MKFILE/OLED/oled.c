#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"  	 
#include "delay.h"
#include "bmp.h"
//PB6--SCL
//PB7--SDA
/**********************
OLED屏分辨率128*64
列COL 0-127
行分为PAGE0-7
每个PAGE中分为D0-D7
**********************/
u8 OLED_GRAM[128][8];//128列8行

//反显函数
void OLED_ColorTurn(u8 i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xA6,OLED_CMD);//正常显示
		}
	if(i==1)
		{
			OLED_WR_Byte(0xA7,OLED_CMD);//反色显示
		}
}

//屏幕旋转180度
void OLED_DisplayTurn(u8 i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xC8,OLED_CMD);//正常显示
			OLED_WR_Byte(0xA1,OLED_CMD);
		}
	if(i==1)
		{
			OLED_WR_Byte(0xC0,OLED_CMD);//反转显示
			OLED_WR_Byte(0xA0,OLED_CMD);
		}
}

//延时
void IIC_delay(void)
{
	u8 t=3;
	while(t--);
	
}
//产生IIC起始信号
//SCL 为高电平时，SDA 由高电平向低电平跳变，开始传送数据。
void I2C_Start(void)
{
	OLED_SCL_Set();
	OLED_SDA_Set();
	IIC_delay();
	OLED_SDA_Clr();
	IIC_delay();
	OLED_SCL_Clr();
	IIC_delay();
}
//结束信号
//SCL 为高电平时，SDA 由低电平向高电平跳变，结束传送数据。
void I2C_Stop(void)
{
	OLED_SCL_Set();
	OLED_SDA_Clr();
	IIC_delay();
	OLED_SDA_Set();
}
//等待信号响应
//在响应的时钟脉冲期间（第9个时钟周期）且SCL为高时，若SDA为低则为ACK，若SDA为高则为NACK
//？？？
void I2C_WaitAck(void)
{
	OLED_SDA_Set();
	IIC_delay();
	OLED_SCL_Set();
	IIC_delay();
	OLED_SCL_Clr();
	IIC_delay();
}
void Send_Byte(u8 dat)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		if(dat&0x80)			//将dat的8位从最高位依次写入,0x80=1000 0000
		{
			OLED_SDA_Set();	//为1输出高电平
    }
		else
		{
			OLED_SDA_Clr();	//为0输出低电平
    }
		IIC_delay();
		OLED_SCL_Set();
		IIC_delay();
		OLED_SCL_Clr();		//将时钟信号设置为低电平
		dat<<=1;					//左移一位
	}
}
//发送一个字节（写入函数）
//mode:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(u8 dat,u8 mode)
{
	I2C_Start();
	Send_Byte(0x78);
	I2C_WaitAck();
	if(mode){Send_Byte(0x40);}
  else{Send_Byte(0x00);}
	I2C_WaitAck();
	Send_Byte(dat);
	I2C_WaitAck();
	I2C_Stop();
}

//开启OLED显示 
void OLED_DisPlay_On(void)
{
	OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵使能
	OLED_WR_Byte(0x14,OLED_CMD);//开启电荷泵
	OLED_WR_Byte(0xAF,OLED_CMD);//点亮屏幕
}

//关闭OLED显示 
void OLED_DisPlay_Off(void)
{
	OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵使能
	OLED_WR_Byte(0x10,OLED_CMD);//关闭电荷泵
	OLED_WR_Byte(0xAE,OLED_CMD);//关闭屏幕
}

//更新显存到OLED	
void OLED_Refresh(void)
{
	u8 i,n;
	for(i=0;i<8;i++)
	{
		OLED_WR_Byte(0xb0+i,OLED_CMD); //设置行起始地址		1011 0000
		OLED_WR_Byte(0x00,OLED_CMD);   //设置低列起始地址
		OLED_WR_Byte(0x10,OLED_CMD);   //设置高列起始地址
		I2C_Start();
		Send_Byte(0x78);
		I2C_WaitAck();
		Send_Byte(0x40);//设置为写入数据
		I2C_WaitAck();
		for(n=0;n<128;n++)
		{
			Send_Byte(OLED_GRAM[n][i]);
			I2C_WaitAck();
		}
		I2C_Stop();
  }
}

//清屏函数
void OLED_Clear(void)
{
	u8 i,n;
	for(i=0;i<8;i++)
	{
	   for(n=0;n<128;n++)
			{
			 OLED_GRAM[n][i]=0;//清除所有数据
			}
  }
	OLED_Refresh();//更新显示
}


/*************************
画点 
x:0~127
y:0~63
t:1 填充 0,清空(将原本亮的地方清除)	
*************************/
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 i,m,n;
	i=y/8;//PAGE
	m=y%8;//PAGE-LSB
	n=1<<m;
	if(t){OLED_GRAM[x][i]|=n;}
	else
	{
		OLED_GRAM[x][i]=~OLED_GRAM[x][i];	//1111 1111->0000 0000
		OLED_GRAM[x][i]|=n;								//0000 0000->0000 1000
		OLED_GRAM[x][i]=~OLED_GRAM[x][i];	//0000 1000->1111 0111
	}
}


/*************************
画线
x:0~127
y:0~63
x1,y1:起点坐标
x2,y2:结束坐标
*************************/
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1;
	uRow=x1;//画线起点坐标
	uCol=y1;
	if(delta_x>0)incx=1; //设置单步方向 
	else if (delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;}
	
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//水平线 
	else {incy=-1;delta_y=-delta_x;}
	
	if(delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y;
	
	for(t=0;t<distance+1;t++)
	{
		OLED_DrawPoint(uRow,uCol,mode);//画点
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}

/*************************
x,y:圆心坐标
r:圆的半径
*************************/
void OLED_DrawCircle(u8 x,u8 y,u8 r)
{
	int a, b,num;
    a = 0;
    b = r;
    while(2 * b * b >= r * r)      
    {
        OLED_DrawPoint(x + a, y - b,1);
        OLED_DrawPoint(x - a, y - b,1);
        OLED_DrawPoint(x - a, y + b,1);
        OLED_DrawPoint(x + a, y + b,1);
 
        OLED_DrawPoint(x + b, y + a,1);
        OLED_DrawPoint(x + b, y - a,1);
        OLED_DrawPoint(x - b, y - a,1);
        OLED_DrawPoint(x - b, y + a,1);
        
        a++;
        num = (a * a + b * b) - r*r;//计算画的点离圆心的距离
        if(num > 0)
        {
            b--;
            a--;
        }
    }
}


/*************************
在指定位置显示一个字符,包括部分字符
x:0~127
y:0~63
size1:8,12,16,24选择字体 6x8/6x12/8x16/12x24
mode:0,反色显示;1,正常显示
*************************/
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1,u8 mode)
{
	u8 i,m,temp,size2,chr1;
	u8 x0=x,y0=y;
	if(size1==8)size2=6;
	else size2=(size1/8+((size1%8)?1:0))*(size1/2);  //得到字体一个字符对应点阵集所占的字节数
	chr1=chr-' ';  //计算偏移后的值
	for(i=0;i<size2;i++)
	{
		if(size1==8)
			  {temp=asc2_0806[chr1][i];} //调用0806字体
		else if(size1==12)
        {temp=asc2_1206[chr1][i];} //调用1206字体
		else if(size1==16)
        {temp=asc2_1608[chr1][i];} //调用1608字体
		else if(size1==24)
        {temp=asc2_2412[chr1][i];} //调用2412字体
		else return;
		for(m=0;m<8;m++)
		{
			if(temp&0x01)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp>>=1;
			y++;
		}
		x++;
		if((size1!=8)&&((x-x0)==size1/2))
		{x=x0;y0=y0+8;}
		y=y0;
  }
}

/*************************
显示字符串
x,y:起点坐标  
size1:字体大小 
*chr:字符串起始地址 
mode:0,反色显示;1,正常显示
*************************/
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1,u8 mode)
{
	while((*chr>=' ')&&(*chr<='~'))//判断是不是非法字符!
	{
		OLED_ShowChar(x,y,*chr,size1,mode);
		if(size1==8)x+=6;
		else x+=size1/2;
		chr++;
  }
}

//m^n
u32 OLED_Pow(u8 m,u8 n)
{
	u32 result=1;
	while(n--)
	{
	  result*=m;
	}
	return result;
}

/*************************
显示数字
x,y :起点坐标
num :要显示的数字
len :数字的位数
size:8 字体大小
mode:0,反色显示;1,正常显示
*************************/
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode)
{
	u8 t,temp,m=0;
	if(size1==8)m=2;
	for(t=0;t<len;t++)
	{
		temp=(num/OLED_Pow(10,len-t-1))%10;			//从高位取数
			if(temp==0)
			{
				OLED_ShowChar(x+(size1/2+m)*t,y,'0',size1,mode);
      }
			else 
			{
			  OLED_ShowChar(x+(size1/2+m)*t,y,temp+'0',size1,mode);
			}
  }
}

/*************************
显示汉字
x,y:起点坐标
num:汉字对应的序号
size1:16 24 32 64
mode:0,反色显示;1,正常显示
*************************/
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1,u8 mode)
{
	u8 m,temp;
	u8 x0=x,y0=y;
	u16 i,size3=(size1/8+((size1%8)?1:0))*size1;  //得到字体一个字符对应点阵集所占的字节数
	for(i=0;i<size3;i++)
	{
		if(size1==16)
				{temp=Hzk1[num][i];}//调用16*16字体
		else if(size1==24)
				{temp=Hzk2[num][i];}//调用24*24字体
		else if(size1==32)       
				{temp=Hzk3[num][i];}//调用32*32字体
		else if(size1==64)
				{temp=Hzk4[num][i];}//调用64*64字体
		else return;
		for(m=0;m<8;m++)
		{
			if(temp&0x01)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp>>=1;
			y++;
		}
		x++;
		if((x-x0)==size1)
		{x=x0;y0=y0+8;}
		y=y0;
	}
}

/*************************
num 显示汉字的个数
space 每一遍显示的间隔
mode:0,反色显示;1,正常显示
*************************/
void OLED_ScrollDisplay(u8 num,u8 space,u8 mode)
{
	u8 i,n,t=0,m=0,r;
	while(1)
	{
		if(m==0)
		{
	    OLED_ShowChinese(128,24,t,16,mode); //写入一个汉字保存在OLED_GRAM[][]数组中
			t++;
		}
		if(t==num)
			{
				for(r=0;r<16*space;r++)      //显示间隔
				 {
					for(i=1;i<144;i++)
						{
							for(n=0;n<8;n++)
							{
								OLED_GRAM[i-1][n]=OLED_GRAM[i][n];
							}
						}
           OLED_Refresh();
				 }
        t=0;
      }
		m++;
		if(m==16){m=0;}
		for(i=1;i<144;i++)   //实现左移
		{
			for(n=0;n<8;n++)
			{
				OLED_GRAM[i-1][n]=OLED_GRAM[i][n];
			}
		}
		OLED_Refresh();
	}
}

/*************************
x,y：起点坐标
sizex,sizey,图片长宽
BMP[]：要写入的图片数组
mode:0,反色显示;1,正常显示
*************************/
void OLED_FillScreen(u8 gram[X_WIDTH][OLED_Y_PAGE])		//整屏刷新（显存数组指针）
{
	int i=0,n=0;
	for(i=0;i<OLED_Y_PAGE;i++)
	{
		OLED_WR_Byte(0xb0+i,0);
		OLED_WR_Byte(0x00,0);
		OLED_WR_Byte(0x10,0);
		for(n=0;n<X_WIDTH;n++)
		{
			OLED_WR_Byte(gram[n][i],1);
		}
	}	
}


void OLED_ShowPicture(u8 x,u8 y,u8 sizex,u8 sizey,u8 BMP[],u8 mode)
{
	u16 j=0;
	u8 i,n,temp,m;
	u8 x0=x,y0=y;
	sizey=sizey/8+((sizey%8)?1:0);
	for(n=0;n<sizey;n++)
	{
		 for(i=0;i<sizex;i++)
		 {
				temp=BMP[j];
				j++;
				for(m=0;m<8;m++)
				{
					if(temp&0x01)OLED_DrawPoint(x,y,mode);
					else OLED_DrawPoint(x,y,!mode);
					temp>>=1;
					y++;
				}
				x++;
				if((x-x0)==sizex)
				{
					x=x0;
					y0=y0+8;
				}
				y=y0;
     }
	 }
}

//OLED的初始化
void OLED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能B端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	  //初始化PB67
 	GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7);

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	 
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
// 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  //初始化PA2
// 	GPIO_SetBits(GPIOA,GPIO_Pin_2);
//	
//	OLED_RES_Clr();
//	delay_ms(200);
//	OLED_RES_Set();
	
//////	OLED_WR_Byte(0xAE,OLED_CMD); //关闭显示
//////	OLED_WR_Byte(0x02,OLED_CMD); //设置8位起始列地址的低四位 0000 0010 
//////	OLED_WR_Byte(0x10,OLED_CMD); //设置8位起始列地址的高四位 0001 0000
//////	OLED_WR_Byte(0x40,OLED_CMD); /*set display start line*/ 
//////	OLED_WR_Byte(0xB0,OLED_CMD); /*set page address*/
//////	OLED_WR_Byte(0x81,OLED_CMD); /*contract control*/ 
//////	OLED_WR_Byte(0xff,OLED_CMD); /*128*/ 
//////	OLED_WR_Byte(0xA1,OLED_CMD); /*set segment remap*/ 
//////	OLED_WR_Byte(0xA6,OLED_CMD); /*normal / reverse*/ 
//////	OLED_WR_Byte(0xA8,OLED_CMD); /*multiplex ratio*/ 
//////	OLED_WR_Byte(0x3F,OLED_CMD); /*duty = 1/64*/ 
//////	OLED_WR_Byte(0xad,OLED_CMD); /*set charge pump enable*/ 
//////	OLED_WR_Byte(0x8b,OLED_CMD); /* 0x8B 内供 VCC */ 
//////	OLED_WR_Byte(0x33,OLED_CMD); /*0X30---0X33 set VPP 9V */ 
//////	OLED_WR_Byte(0xC8,OLED_CMD); /*Com scan direction*/ 
//////	OLED_WR_Byte(0xD3,OLED_CMD); /*set display offset*/ 
//////	OLED_WR_Byte(0x00,OLED_CMD); /* 0x20 */ 
//////	OLED_WR_Byte(0xD5,OLED_CMD); /*set osc division*/ 
//////	OLED_WR_Byte(0x80,OLED_CMD); 
//////	OLED_WR_Byte(0xD9,OLED_CMD); /*set pre-charge period*/ 
//////	OLED_WR_Byte(0x1f,OLED_CMD); /*0x22*/ 
//////	OLED_WR_Byte(0xDA,OLED_CMD); /*set COM pins*/ 
//////	OLED_WR_Byte(0x12,OLED_CMD); 
//////	OLED_WR_Byte(0xdb,OLED_CMD); /*set vcomh*/ 
//////	OLED_WR_Byte(0x40,OLED_CMD);
//////	OLED_Clear();
//////	OLED_WR_Byte(0xAF,OLED_CMD); /*display ON*/

	OLED_WR_Byte(0xAE,OLED_CMD);//--display off
	OLED_WR_Byte(0x02,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  
	OLED_WR_Byte(0xB0,OLED_CMD);//--set page address
	OLED_WR_Byte(0x81,OLED_CMD); // contract control
	OLED_WR_Byte(0xFF,OLED_CMD);//--128   
	OLED_WR_Byte(0xA1,OLED_CMD);//set segment remap 
	OLED_WR_Byte(0xA6,OLED_CMD);//--normal / reverse
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3F,OLED_CMD);//--1/32 duty
	OLED_WR_Byte(0xC8,OLED_CMD);//Com scan direction
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset
	OLED_WR_Byte(0x00,OLED_CMD);//
	
	OLED_WR_Byte(0xD5,OLED_CMD);//set osc division
	OLED_WR_Byte(0x80,OLED_CMD);//
	
	OLED_WR_Byte(0xD8,OLED_CMD);//set area color mode off
	OLED_WR_Byte(0x05,OLED_CMD);//
	
	OLED_WR_Byte(0xD9,OLED_CMD);//Set Pre-Charge Period
	OLED_WR_Byte(0xF1,OLED_CMD);//
	
	OLED_WR_Byte(0xDA,OLED_CMD);//set com pin configuartion
	OLED_WR_Byte(0x12,OLED_CMD);//
	
	OLED_WR_Byte(0xDB,OLED_CMD);//set Vcomh
	OLED_WR_Byte(0x30,OLED_CMD);//
	
	OLED_WR_Byte(0x8D,OLED_CMD);//set charge pump enable
	OLED_WR_Byte(0x14,OLED_CMD);//
	
	OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
}

