#ifndef __asrpro_uart_H
#define	__asrpro_uart_H


#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "misc.h"

#include "stdio.h"

/*

	YuYin_RX_CMD 收到 
	30 ：有害垃圾
	31 ：可回收
	32 ：厨余垃圾
	33 ：其他
	

*/



extern u8 YuYin_RX_CMD;//接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u8 YuYin_RX_STA;//接收状态标记	


                 

void YuYin_Init(void);
void USART1_IRQHandler(void);         	//串口1中断服务程序
uint8_t Serial_GetRxFlag(void);
uint8_t Serial_GetRxData(void);

#endif 
