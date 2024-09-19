#ifndef _SYN6288E_H
#define _SYN6288E_H


/*Includes ***********************************************/

#include "stm32f10x.h"                  // Device header
#include "delay.h"
/*********************************************************/

/*Function Refrence****************************************/

void syn6288_Usart_Init(void );		//初始化syn6288串口 -- >串口2
void syn6288_Init(void );			//初始化syn6288
void syn6288_BY_Init(void );			//初始化BY引脚

void syn6288_Say(char *fmt);		//播报函数

/**********************************************************/


#endif
