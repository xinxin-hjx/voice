#ifndef _SYN6288E_H
#define _SYN6288E_H


/*Includes ***********************************************/

#include "stm32f10x.h"                  // Device header
#include "delay.h"
/*********************************************************/

/*Function Refrence****************************************/

void syn6288_Usart_Init(void );		//��ʼ��syn6288���� -- >����2
void syn6288_Init(void );			//��ʼ��syn6288
void syn6288_BY_Init(void );			//��ʼ��BY����

void syn6288_Say(char *fmt);		//��������

/**********************************************************/


#endif
