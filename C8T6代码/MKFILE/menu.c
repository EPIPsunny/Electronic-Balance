#include "stm32f10x.h"   // �൱��51��Ƭ���е�  #include <reg51.h>
#include "oled.h"
#include "delay.h"
#include "SOLGUI_Include.h"
#include "string.h"
#include "menu.h"
#include "key.h"
#include "HX711.h"


MENU_PAGE FirstPage;
MENU_PAGE MODE1;
MENU_PAGE MODE2;
extern int mode;

void menu_int(void)
{
   SOLGUI_Init(&FirstPage);
}

__M_PAGE(FirstPage,"First Page",PAGE_NULL,
{
	SOLGUI_Cursor(6,0,4);                                //�����ı�ѡ����
  //SOLGUI_Widget_GotoPage(0,&MODE1);                     //������ҳ��
	//SOLGUI_Widget_GotoPage(1,&MODE2);
	SOLGUI_Widget_OptionText(1,"weight:%d g",Weight_Shiwu);
});

__M_PAGE(MODE1,"MODE1",&FirstPage,                       //���������˵�
{
//  mode=1;                                               //ģʽ��־λ
//  SOLGUI_Cursor(6,0,7);                                 //
//	SOLGUI_Widget_OptionText(0, "date1: %d", date1);      //�����ı�ѡ��
//	SOLGUI_Widget_Spin(1,"page",INT32,100,0,&page);       //��ť���ں���
});
__M_PAGE( MODE2,"MODE2",&MODE1,                           //���������˵� **&MODE1**�ص㷵�� MODE1 ҳ��
{
	
	
		
});



