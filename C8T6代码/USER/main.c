#include "sys.h"
#include "delay.h"
#include "stm32f10x.h"  
#include "oled.h"
#include "menu.h"
#include "key.h"
#include "zimo.h"                                    //取字模文件
#include "SOLGUI_Include.h"
#include "HX711.h"
#include "timer.h"
#include "usart.h"
/************************************************

************************************************/
 void JTAGDisable()                                  //禁用JTAG	
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
}


int main(void)
{	
	//int key_value=0;
	delay_init();
	JTAGDisable();  	                                  //禁用JTAG	很重要不得删去
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 	//设置NVIC中断分组2:2位
	menu_int();
	Init_HX711pin();
	//KEY_Init();
	SOLGUI_Refresh();
	Get_Maopi();
	delay_ms(50);
	Get_Maopi();
	uart_init(115200);
	TIM3_Int_Init(349,7199);
	while(1)
	{
	
	//SOLGUI_Pictrue(20,0,&zimo1[0],64,64,NORMAL); //显示BMO格式图片
	//  SOLGUI_printf(1,0,F6X8,"    TEXT TEXT TEXT TEXT ");
	   //key_value = Key_GetValue();
	  // SOLGUI_InputKey(key_value);
		 
	   SOLGUI_Menu_PageStage(); 
	   SOLGUI_Refresh();       //OLED刷新		
	}
}



