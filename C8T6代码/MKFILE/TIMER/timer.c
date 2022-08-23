#include "timer.h"
#include "sys.h"
#include "delay.h"
#include "stm32f10x.h"  
#include "oled.h"
#include "menu.h"
#include "key.h"
//#include "zimo.h"                                    //取字模文件
#include "SOLGUI_Include.h"
#include "HX711.h"
#include "wifi.h"
//定时器初始化
//提供通用定时器2的初始化配一个定时中断
//使用时主函数里添加头文件
//加入函数Timer_TIM2_Init(,);
//高级定时器和普通的有所不同
void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrue;
	NVIC_InitTypeDef NVIC_InitSture;
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	TIM_TimeBaseInitStrue.TIM_Period=arr;//自动装载值
	TIM_TimeBaseInitStrue.TIM_Prescaler=psc;//预分频系数
	TIM_TimeBaseInitStrue.TIM_CounterMode=TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInitStrue.TIM_ClockDivision=TIM_CKD_DIV1;//暂时不用
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStrue);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);//使能定时器更新中断
	
	NVIC_InitSture.NVIC_IRQChannel=TIM3_IRQn;
	NVIC_InitSture.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitSture.NVIC_IRQChannelSubPriority=3;
	NVIC_InitSture.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitSture);
	
	TIM_Cmd(TIM3,ENABLE);
}

void TIM3_IRQHandler(void)
{
	int thisweight;
	static int lastweight;
	static unsigned char i;
	static int lastprintweight;
	static unsigned char flag;
	Get_Weight();
	thisweight = Weight_Shiwu;
	if(thisweight != 0)
	{
		if(lastweight == thisweight)
		{
			i++;
			if(i>10)
			{
				if(thisweight != lastprintweight)
				{
					printf("%d",Weight_Shiwu);
					flag=1;
				}	
				lastprintweight = thisweight;
				i=0;
			}
		}
		else
		{
			i=0;
		}
	}
	else
	{
		if(flag==1)
		{
			printf("%d",0);
		}
		lastprintweight=0;
		flag=0;
	}

	lastweight = thisweight;
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
}





