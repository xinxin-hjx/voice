#ifndef _USART_H
#define _USART_H


/*Includes **************************************************************/

#include "stm32f10x.h"                  // Device header
#include "drv_led.h"


/************************************************************************/


/*Function Refrence******************************************************/

void usart1_Init(void );			//����1�ĳ�ʼ��
void usart_SendString(char *ch);	//�����ַ���
void uart2_Init(u32 bound);

/************************************************************************/


#endif
