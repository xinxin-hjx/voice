#include "drv_fan.h"


void fan_Init(void )	//���ȳ�ʼ��
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(FAN_INR_Pin_Periph_Clock|FAN_INL_Pin_Periph_Clock,ENABLE);
	
	//PA11
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = FAN_INR_Pin;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(FAN_INR_Pin_Port,&GPIO_InitStruct);
	//PA12
	GPIO_InitStruct.GPIO_Pin = FAN_INL_Pin;
	GPIO_Init(FAN_INL_Pin_Port,&GPIO_InitStruct);
	
	GPIO_ResetBits(FAN_INR_Pin_Port,FAN_INR_Pin);
	GPIO_ResetBits(FAN_INL_Pin_Port,FAN_INL_Pin);
}
void fan_Right(void)	//������ת
{
	GPIO_SetBits(FAN_INR_Pin_Port,FAN_INR_Pin);
	GPIO_ResetBits(FAN_INL_Pin_Port,FAN_INL_Pin);
	
}
void fan_Left(void )	//������ת
{
	GPIO_ResetBits(FAN_INR_Pin_Port,FAN_INR_Pin);
	GPIO_SetBits(FAN_INL_Pin_Port,FAN_INL_Pin);
}
void fan_Stop(void )	//����ֹͣ
{
	GPIO_ResetBits(FAN_INR_Pin_Port,FAN_INR_Pin);
	GPIO_ResetBits(FAN_INL_Pin_Port,FAN_INL_Pin);
}
