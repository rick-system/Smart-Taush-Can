#ifndef __ESP8266_H
#define	__ESP8266_H

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"

#include "HAL_usart.h"

#include "misc.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>

#include "delay.h"



void esp8266_task(void);
void esp8266_Init(void);








#endif 
