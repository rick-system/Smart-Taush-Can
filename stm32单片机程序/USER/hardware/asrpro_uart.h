#ifndef __asrpro_uart_H
#define	__asrpro_uart_H


#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "misc.h"

#include "stdio.h"

/*

	YuYin_RX_CMD �յ� 
	30 ���к�����
	31 ���ɻ���
	32 ����������
	33 ������
	

*/



extern u8 YuYin_RX_CMD;//���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u8 YuYin_RX_STA;//����״̬���	


                 

void YuYin_Init(void);
void USART1_IRQHandler(void);         	//����1�жϷ������
uint8_t Serial_GetRxFlag(void);
uint8_t Serial_GetRxData(void);

#endif 
