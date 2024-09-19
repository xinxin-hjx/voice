#ifndef _DRV_FAN_H
#define _DRV_FAN_H

/*Includes *************************************************/
#include "stm32f10x.h"                  // Device header

/***********************************************************/

/*Defines***************************************************/
//R  PA11   L   PA12
#define FAN_INR_Pin 				GPIO_Pin_11
#define FAN_INR_Pin_Port    		GPIOA
#define FAN_INR_Pin_Periph_Clock 	RCC_APB2Periph_GPIOA

#define FAN_INL_Pin 				GPIO_Pin_12
#define FAN_INL_Pin_Port    		GPIOA
#define FAN_INL_Pin_Periph_Clock 	RCC_APB2Periph_GPIOA
/***********************************************************/


/*Function Refrence*****************************************/
void fan_Init(void );	//���ȳ�ʼ��
void fan_Right(void);	//������ת
void fan_Left(void );	//������ת
void fan_Stop(void );	//����ֹͣ

/***********************************************************/

#endif
