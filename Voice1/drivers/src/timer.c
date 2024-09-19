#include "timer.h"
#include "drv_led.h"

u8 led_Flag = 0;

void Timer2_Init(void)	//初始化定时器2
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	//开时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	//配置定时器
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode   = TIM_CounterMode_Up;		//计数方式	
	TIM_TimeBaseInitStruct.TIM_Period 		 =7200-1;			//自动重装载寄存器 
	TIM_TimeBaseInitStruct.TIM_Prescaler 	 =10000-1;			//预分频器
//	TIM_TimeBaseInitStruct.TIM_RepetitionCounter =;	//高级定时器的参数
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
	
	//配置中断
	//设置中断源
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);	//TIM_IT_Update计时结束触发
	
	//配置中断优先级
	NVIC_InitStruct.NVIC_IRQChannel =TIM2_IRQn;	//中断的通道
	NVIC_InitStruct.NVIC_IRQChannelCmd =ENABLE;	//中断使能
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority =0;	//主优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority =0;			//从优先级
	NVIC_Init(&NVIC_InitStruct);
	
	//使能串口
	TIM_Cmd(TIM2, ENABLE);
}

#if 1
void Timer3_Init(void)	//初始化定时器3
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	//开时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );	
	//配置GPIO 通道1 OC1  PA６
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;	//第６个引脚
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	//通信速速率	
	GPIO_Init(GPIOA, &GPIO_InitStruct);
		
	//配置定时器
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode   = TIM_CounterMode_Up;		//计数方式	
	TIM_TimeBaseInitStruct.TIM_Period 		 =1000-1;			//自动重装载寄存器 
	TIM_TimeBaseInitStruct.TIM_Prescaler 	 =720-1;			//预分频器
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);	
	//配置输出通道
	/*
初始化定时器通道
void TIM_OC1Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC2Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC3Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC4Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);	
	*/
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;		//输出通道的模式
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;	//有效电平的选择
	TIM_OCInitStruct.TIM_OutputState =TIM_OutputState_Enable;	//使能此通道
	TIM_OCInitStruct.TIM_Pulse = 0;
	TIM_OC1Init(TIM3,&TIM_OCInitStruct);
	
	//更新事件后重写的值才有效
	/*
void TIM_OC1PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);
void TIM_OC2PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);
void TIM_OC3PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);
void TIM_OC4PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);
	*/
	TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	
	//定时器使能
	TIM_Cmd(TIM3,ENABLE);

}
#endif
//中断处理函数
#if 0
void TIM2_IRQHandler()
{
	if (TIM_GetITStatus(TIM2,TIM_IT_Update) == SET)	//获取中断标志位
	{
		//清除标志位
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
		if(led_Flag == 0)
		{
			led_ON();
			led_Flag = 1;
		}
		else
		{
			led_OFF();
			led_Flag = 0;
		}
		
//		if(GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_4))
//		{
//			led_ON();
//		}
//		else
//		{
//			led_OFF();
//		}
	}
}
#endif

void PWM_SetCompare1(uint16_t Compare)
{
	TIM_SetCompare1(TIM3, Compare);
}
