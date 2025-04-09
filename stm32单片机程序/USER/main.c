//单片机头文件
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
//网络协议层
#include "onenet.h"
//网络设备
#include "esp8266.h"
//硬件驱动
#include "delay.h"
#include "usart.h"
//C库
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


unsigned short timeCount = 0;	//发送间隔变量

unsigned char *dataPtr = NULL;

short flag = 0;

u8 Target1=0;
u8 Target2=0;
u8 Target3=0;
u8 Target4=0;

u8 all_staus = 0;

u8 Open_JumpEdge=0;
u8 WakeUp_Flag=0;

u8 action = 0; //动作变量

///////////////////////////////////////////////////////////////////////功能函数////////////////////////////////////////////////////

/*
************************************************************
*	函数名称：	Hardware_Init
*
*	函数功能：	硬件初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		初始化单片机功能以及外接设备
************************************************************
*/
void Hardware_Init(void)
{
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断控制器分组设置

	delay_init();									//systick初始化
//	Usart3_Init(115200);				  			//串口3，驱动ESP8266用
	PWM_Init();                   					//初始化PWM
//	ESP8266_Init();					      			//初始化ESP8266
//	LED_Config();									//初始化LED指示灯或者说是继电器
	
	YuYin_Init();									//语音识别初始化 串口1
	
	SysTick_Init();									//公共定时器初始化

	OLED_Init();									//OLED屏初始化
	
}


///*
//************************************************************
//*	函数名称：	 Keep_Connet_Esp8266
//*
//*	函数功能：	不断上传数据，使之保持接入物联网平台
//*
//*	入口参数：	无
//*
//*	返回参数：	无
//*
//*	说明：		一旦停止数据上传，物联网平台会断开
//************************************************************
////*/
////void Keep_Connet_Esp8266 (void)
////{
////	
////			if(++timeCount >= 2000)									//发送间隔20s
////				{
////					OneNet_SendData();									//发送数据		
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
*	函数名称：	main
*
*	函数功能：	
*
*	入口参数：	无
*
*	返回参数：	0
*
*	说明：		
***
*********************************************************
*/
int main(void)
{
	

	Hardware_Init();				//初始化外围硬件

//	while(OneNet_DevLink())			//接入OneNET
// 	delay_ms(500);

	OLED_CLS();
	ShowSmartTrash();				//显示“智能垃圾桶”
	Show_Author();					//显示“设计·作者名“
//	ShowTemperature();				//显示”温湿度”
	
	
	while(1)
	{				
//			Keep_Connet_Esp8266();	
		
			if( Serial_GetRxFlag() == 1){
			
				action = Serial_GetRxData();	
				
				switch(action){				
					case 0x10 : 
						OLED_CLS(); 	
						ShowNormal();						
						break;	//系统正常启动
					
					case 0x30 : 
						OLED_CLS();
						Show_youhai();
						Target3 = 1;     // 触发舵机1动作
						break;	//有害垃圾	
					
					case 0x31 :
						OLED_CLS(); 	
						Show_kehuishou();
						Target4 = 1;     // 触发舵机2动作
						break;	//可回收物
					
					case 0x32 : 
						OLED_CLS(); 	
						Show_chiyu();
						Target1 = 1;     // 触发舵机3动作
						break;	//厨余垃圾	
					
					case 0x33 : 
						OLED_CLS(); 	
						Show_qita();
						Target2 = 1;     // 触发舵机4动作
						break;	//其他垃圾
				}
					
				
			}				

		if(SysTick_JumpEdge(&Open_JumpEdge, &SysTickBit.Time1s)==1)// 每秒执行一次的定时任务
		{
		  if(Target1 != 0)
			{
			  Target1++;
				TIM_SetCompare1(TIM4,700);
				if(Target1>6)
				{					
					TIM_SetCompare1(TIM4,500);			//厨余		
					Target1 = 0;
					OLED_CLS(); 	
					ShowNormal();					
				}					
			}
			if(Target2 != 0)
			{
				Target2++;
				TIM_SetCompare2(TIM4,900);				//其他
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
				TIM_SetCompare4(TIM4,900);			//有害
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
				TIM_SetCompare3(TIM4,900);			//可回收
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






