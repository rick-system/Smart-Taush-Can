#ifndef _HAL_USART_H_
#define _HAL_USART_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp8266.h"

#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#include "usart.h"
#include "stdio.h"




int fputc(int ch, FILE *f);
void Uart_IT_Init(void);
void instruct_dat_printf(void);
uint8_t esp_Getvalue(void);



#endif
