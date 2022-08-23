#include "sys.h"
#include "delay.h"
#include "stm32f10x.h"  
#include "oled.h"
#include "menu.h"
#include "key.h"
#include "zimo.h"                                    //ȡ��ģ�ļ�
#include "SOLGUI_Include.h"
#include "HX711.h"
#include "timer.h"
#include "usart.h"
/************************************************

************************************************/
 void JTAGDisable()                                  //����JTAG	
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
}


int main(void)
{	
	//int key_value=0;
	delay_init();
	JTAGDisable();  	                                  //����JTAG	����Ҫ����ɾȥ
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 	//����NVIC�жϷ���2:2λ
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
	
	//SOLGUI_Pictrue(20,0,&zimo1[0],64,64,NORMAL); //��ʾBMO��ʽͼƬ
	//  SOLGUI_printf(1,0,F6X8,"    TEXT TEXT TEXT TEXT ");
	   //key_value = Key_GetValue();
	  // SOLGUI_InputKey(key_value);
		 
	   SOLGUI_Menu_PageStage(); 
	   SOLGUI_Refresh();       //OLEDˢ��		
	}
}



