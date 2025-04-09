#ifndef __SysTick_H
#define	__SysTick_H

#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "misc.h"
#include "stm32f10x_tim.h"

typedef struct	_SYSTICKBIT_  //ϵͳ������ʱ��		
{	
  u8 Time20ms;    //50ms
	u8 Time100ms;   //100ms
	u8 Time500ms;   //500ms
	u8 Time1s;      //1s
	u8 Time2s;      //2s
	u8 Time5s;      //5s
	u8 Time10s;     //10s
	u8 Time60s;     //60s
	u8 Time5m;      //5����
	u8 Time10m;     //10����
}SYSTICKBIT; 
extern SYSTICKBIT SysTickBit;  //ϵͳ������ʱ��״̬

void SysTick_Init(void);
u8 SysTick_JumpEdge(u8* front, u8* now);

#endif 

