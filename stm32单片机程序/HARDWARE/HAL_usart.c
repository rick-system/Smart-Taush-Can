#include "HAL_usart.h"


//���ڽ��յ����ݻ�����
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
    
    // ʹ��USART2��GPIOʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // ����USART2 Tx (PA2) Ϊ���츴�����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // ����USART2 Rx (PA3) Ϊ��������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // USART��������
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);
    
    // ʹ�ܽ����ж�
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    
    // ����USART2�ж����ȼ�
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // ʹ��USART2
    USART_Cmd(USART2, ENABLE);
}


// ��׼��Ĵ��ڽ����жϴ���
void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) // �������ж�
    {
        // ��ȡ���յ�������
        rx_dat = USART_ReceiveData(USART2);
        
        RX_DAT[n] = rx_dat;
        if(n<200){
		n++;
		};

        if(rx_dat == '\n')
        {
            RX_DAT[n - 1] = '\0';

            // �������ʣ�ಿ��
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

