#include "HAL_usart.h"


//串口接收的数据缓冲区
char RX_DAT[200];
char rx_dat;
int n = 0;
uint8_t bzw;
u8 esp_value;
extern char Switch[200];

void Uart_IT_Init(void)
{
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能USART2和GPIO时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // 配置USART2 Tx (PA2) 为推挽复用输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 配置USART2 Rx (PA3) 为浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // USART参数配置
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);
    
    // 使能接收中断
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    
    // 配置USART2中断优先级
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // 使能USART2
    USART_Cmd(USART2, ENABLE);
}


// 标准库的串口接收中断处理
void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) // 检查接收中断
    {
        // 读取接收到的数据
        rx_dat = USART_ReceiveData(USART2);
        
        RX_DAT[n] = rx_dat;
        if(n<200){
		n++;
		};

        if(rx_dat == '\n')
        {
            RX_DAT[n - 1] = '\0';

            // 清除数组剩余部分
            for(; n < 200; n++)
            {
                RX_DAT[n] = 0;
            }
            
            n = 0;
			esp_value = bzw;
            if(strstr(RX_DAT, "{\"K\":1}") != NULL)
            {
                bzw = 1;
            }
            else if(strstr(RX_DAT, "{\"K\":2}") != NULL)
            {
                bzw = 2;
            }
            else if(strstr(RX_DAT, "{\"K\":3}") != NULL)
            {
                bzw = 3;
            }
            else if(strstr(RX_DAT, "{\"K\":4}") != NULL)
            {
                bzw = 4;
            }
            else if(strstr(RX_DAT, "{\"K\":0}") != NULL)
            {
                bzw = 0;
            }
			
        }
    }
}

uint8_t esp_Getvalue(void)
{
	
	return esp_value;
}

