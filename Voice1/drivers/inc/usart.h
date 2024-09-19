#ifndef _USART_H
#define _USART_H


/*Includes **************************************************************/

#include "stm32f10x.h"                  // Device header
#include "drv_led.h"


/************************************************************************/


/*Function Refrence******************************************************/

void usart1_Init(void );			//串口1的初始化
void usart_SendString(char *ch);	//发送字符串
void uart2_Init(u32 bound);

/************************************************************************/


#endif
