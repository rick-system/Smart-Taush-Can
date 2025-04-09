//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
//����Э���
#include "onenet.h"
//�����豸
#include "esp8266.h"
//Ӳ������
#include "delay.h"
#include "usart.h"
//C��
#include <string.h>
#include "PWM.h"
#include "jidianqi.h"

#include "MP3.h"
#include "asrpro_uart.h"
#include "SysTick.h"
#include "oled.h" 


char T[5]={0,0,0};
char H[5]={0,0,0};
u16 temperature=0,humidity=0;
char ret; 
u8 hump_temp[10];


unsigned short timeCount = 0;	//���ͼ������

unsigned char *dataPtr = NULL;

short flag = 0;

u8 Target1=0;
u8 Target2=0;
u8 Target3=0;
u8 Target4=0;

u8 all_staus = 0;

u8 Open_JumpEdge=0;
u8 WakeUp_Flag=0;

u8 action = 0; //��������

///////////////////////////////////////////////////////////////////////���ܺ���////////////////////////////////////////////////////

/*
************************************************************
*	�������ƣ�	Hardware_Init
*
*	�������ܣ�	Ӳ����ʼ��
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		��ʼ����Ƭ�������Լ�����豸
************************************************************
*/
void Hardware_Init(void)
{
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�жϿ�������������

	delay_init();									//systick��ʼ��
//	Usart3_Init(115200);				  			//����3������ESP8266��
	PWM_Init();                   					//��ʼ��PWM
//	ESP8266_Init();					      			//��ʼ��ESP8266
//	LED_Config();									//��ʼ��LEDָʾ�ƻ���˵�Ǽ̵���
	
	YuYin_Init();									//����ʶ���ʼ�� ����1
	
	SysTick_Init();									//������ʱ����ʼ��

	OLED_Init();									//OLED����ʼ��
	
}


///*
//************************************************************
//*	�������ƣ�	 Keep_Connet_Esp8266
//*
//*	�������ܣ�	�����ϴ����ݣ�ʹ֮���ֽ���������ƽ̨
//*
//*	��ڲ�����	��
//*
//*	���ز�����	��
//*
//*	˵����		һ��ֹͣ�����ϴ���������ƽ̨��Ͽ�
//************************************************************
////*/
////void Keep_Connet_Esp8266 (void)
////{
////	
////			if(++timeCount >= 2000)									//���ͼ��20s
////				{
////					OneNet_SendData();									//��������		
////					timeCount = 0;
////					ESP8266_Clear();
////				}
////				
////				dataPtr = ESP8266_GetIPD(0);
////				if(dataPtr != NULL)
////					OneNet_RevPro(dataPtr);
////				
////				delay_ms(10);
////				
////}



/*
************************************************************
*	�������ƣ�	main
*
*	�������ܣ�	
*
*	��ڲ�����	��
*
*	���ز�����	0
*
*	˵����		
***
*********************************************************
*/
int main(void)
{
	

	Hardware_Init();				//��ʼ����ΧӲ��

//	while(OneNet_DevLink())			//����OneNET
// 	delay_ms(500);

	OLED_CLS();
	ShowSmartTrash();				//��ʾ����������Ͱ��
	Show_Author();					//��ʾ����ơ���������
//	ShowTemperature();				//��ʾ����ʪ�ȡ�
	
	
	while(1)
	{				
//			Keep_Connet_Esp8266();	
		
			if( Serial_GetRxFlag() == 1){
			
				action = Serial_GetRxData();	
				
				switch(action){				
					case 0x10 : 
						OLED_CLS(); 	
						ShowNormal();						
						break;	//ϵͳ��������
					
					case 0x30 : 
						OLED_CLS();
						Show_youhai();
						Target3 = 1;     // �������1����
						break;	//�к�����	
					
					case 0x31 :
						OLED_CLS(); 	
						Show_kehuishou();
						Target4 = 1;     // �������2����
						break;	//�ɻ�����
					
					case 0x32 : 
						OLED_CLS(); 	
						Show_chiyu();
						Target1 = 1;     // �������3����
						break;	//��������	
					
					case 0x33 : 
						OLED_CLS(); 	
						Show_qita();
						Target2 = 1;     // �������4����
						break;	//��������
				}
					
				
			}				

		if(SysTick_JumpEdge(&Open_JumpEdge, &SysTickBit.Time1s)==1)// ÿ��ִ��һ�εĶ�ʱ����
		{
		  if(Target1 != 0)
			{
			  Target1++;
				TIM_SetCompare1(TIM4,700);
				if(Target1>6)
				{					
					TIM_SetCompare1(TIM4,500);			//����		
					Target1 = 0;
					OLED_CLS(); 	
					ShowNormal();					
				}					
			}
			if(Target2 != 0)
			{
				Target2++;
				TIM_SetCompare2(TIM4,900);				//����
				if(Target2>6)
				{
					TIM_SetCompare2(TIM4,600);					
					Target2 = 0;
					OLED_CLS(); 	
					ShowNormal();	
				}	
			}	
			if(Target3 != 0)
			{
				Target3++;
				TIM_SetCompare4(TIM4,900);			//�к�
				if(Target3>6)
				{
					TIM_SetCompare4(TIM4,500);
					Target3 = 0;
					OLED_CLS(); 	
					ShowNormal();	
				
					
				}	
			}
			if(Target4 != 0)
			{
				Target4++;
				TIM_SetCompare3(TIM4,900);			//�ɻ���
				if(Target4>6)
				{
					TIM_SetCompare3(TIM4,550);
					Target4 = 0;
					OLED_CLS(); 	
					ShowNormal();	
								
				}	
			}
		}
		
//		switch( flag )
//		{
//			case 1 :GPIO_SetBits(GPIOA,GPIO_Pin_1); break;
//			case 2 :GPIO_ResetBits(GPIOA, GPIO_Pin_1); break; 
//			case 3 :TIM_SetCompare1(TIM4,700);break;
//			case 4 :TIM_SetCompare1(TIM4,500);break;
//			case 5 :TIM_SetCompare2(TIM4,900);break;
//			case 6 :TIM_SetCompare2(TIM4,600);break;
//			case 7 :TIM_SetCompare4(TIM4,900);break;
//			case 8 :TIM_SetCompare4(TIM4,500);break;
//			case 9 :TIM_SetCompare3(TIM4,900);break;
//			case 10:TIM_SetCompare3(TIM4,550);break;

//		}
		
		
	}
}






