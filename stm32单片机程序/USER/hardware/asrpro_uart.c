#include "asrpro_uart.h"

u8 YuYin_RX_CMD;//接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
u8 YuYin_RX_STA;//接收状态标记	
/*
************************************************************
*	函数名称：	YuYin_Init()
*
*	函数功能：	串口1初始化
*
*	入口参数：	Nothing
*
*	返回参数：	无
*
*	说明：		TX-PA9		RX-PA10
************************************************************
*/
void YuYin_Init(void)
{
  //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//使能USART1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//GPIOA时钟
	
	USART_DeInit( USART1);
	//USART1_TX  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//USART1_RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1 ;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
  //USART 初始化设置
	USART_InitStructure.USART_BaudRate = 9600;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART1, &USART_InitStructure); //初始化串口
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(USART1, ENABLE);                    //使能串口
}

uint8_t Serial_GetRxFlag(void)
{
	if (YuYin_RX_STA == 1)
	{
		YuYin_RX_STA = 0;
		return 1;
	}
	return 0;
}

uint8_t Serial_GetRxData(void)
{
	return YuYin_RX_CMD;
}

/**********************************
***********************************
串口1中断接收数据  
***********************************
**********************************/
void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		YuYin_RX_CMD = USART_ReceiveData(USART1);	//读取接收到的数据		
		YuYin_RX_STA=1;				  			
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);		
	}	
} 







