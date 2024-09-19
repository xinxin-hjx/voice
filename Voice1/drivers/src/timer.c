#include "timer.h"
#include "drv_led.h"

u8 led_Flag = 0;

void Timer2_Init(void)	//��ʼ����ʱ��2
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	//��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	//���ö�ʱ��
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode   = TIM_CounterMode_Up;		//������ʽ	
	TIM_TimeBaseInitStruct.TIM_Period 		 =7200-1;			//�Զ���װ�ؼĴ��� 
	TIM_TimeBaseInitStruct.TIM_Prescaler 	 =10000-1;			//Ԥ��Ƶ��
//	TIM_TimeBaseInitStruct.TIM_RepetitionCounter =;	//�߼���ʱ���Ĳ���
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
	
	//�����ж�
	//�����ж�Դ
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);	//TIM_IT_Update��ʱ��������
	
	//�����ж����ȼ�
	NVIC_InitStruct.NVIC_IRQChannel =TIM2_IRQn;	//�жϵ�ͨ��
	NVIC_InitStruct.NVIC_IRQChannelCmd =ENABLE;	//�ж�ʹ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority =0;	//�����ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority =0;			//�����ȼ�
	NVIC_Init(&NVIC_InitStruct);
	
	//ʹ�ܴ���
	TIM_Cmd(TIM2, ENABLE);
}

#if 1
void Timer3_Init(void)	//��ʼ����ʱ��3
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	//��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );	
	//����GPIO ͨ��1 OC1  PA��
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;	//�ڣ�������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	//ͨ��������	
	GPIO_Init(GPIOA, &GPIO_InitStruct);
		
	//���ö�ʱ��
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode   = TIM_CounterMode_Up;		//������ʽ	
	TIM_TimeBaseInitStruct.TIM_Period 		 =1000-1;			//�Զ���װ�ؼĴ��� 
	TIM_TimeBaseInitStruct.TIM_Prescaler 	 =720-1;			//Ԥ��Ƶ��
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);	
	//�������ͨ��
	/*
��ʼ����ʱ��ͨ��
void TIM_OC1Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC2Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC3Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC4Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);	
	*/
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;		//���ͨ����ģʽ
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;	//��Ч��ƽ��ѡ��
	TIM_OCInitStruct.TIM_OutputState =TIM_OutputState_Enable;	//ʹ�ܴ�ͨ��
	TIM_OCInitStruct.TIM_Pulse = 0;
	TIM_OC1Init(TIM3,&TIM_OCInitStruct);
	
	//�����¼�����д��ֵ����Ч
	/*
void TIM_OC1PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);
void TIM_OC2PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);
void TIM_OC3PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);
void TIM_OC4PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);
	*/
	TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	
	//��ʱ��ʹ��
	TIM_Cmd(TIM3,ENABLE);

}
#endif
//�жϴ�����
#if 0
void TIM2_IRQHandler()
{
	if (TIM_GetITStatus(TIM2,TIM_IT_Update) == SET)	//��ȡ�жϱ�־λ
	{
		//�����־λ
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
