#include "jidianqi.h"

void LED_Config(void)
{
	
	 GPIO_InitTypeDef GPIO_InitStructure;
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
	/*led config*/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);  
	
	/*jidianqi config*/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_1 );   
  GPIO_SetBits(GPIOC, GPIO_Pin_13);    //默认置高电平
 
}
