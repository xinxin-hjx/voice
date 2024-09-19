#ifndef __TIMER_H__
#define __TIMER_H__


/*Includes *********************************************/


#include "stm32f10x.h"                  // Device header


/*******************************************************/


/*Defines*********************************************/

extern u8 led_Flag ;
/*******************************************************/


/*Function Refrenxe*************************************/
void Timer2_Init(void);	//初始化定时器2
void Timer3_Init(void);	//初始化定时器3
void PWM_SetCompare1(uint16_t Compare);


/*******************************************************/



#endif




