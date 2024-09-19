#include "drv_led.h"

/*
��ε�����һ��LED��
1.�鿴ԭ��ͼ���ҽ������š�ȷ�ϵ�Ʒ�ʽ
2.����GPIO�˿�
	1����GPIO��ʱ��
	2������GPIO�Ĳ���
	3������GPIO�ĳ�ʼ��״̬
3.����LED
*/

void led_Init(void )    	//��ʼ������LED
{
	GPIO_InitTypeDef GPIO_InitStruct;
	//1.��ʱ��
	/*
void RCC_AHBPeriphClockCmd(uint32_t RCC_AHBPeriph, FunctionalState NewState); //����APB���������ʱ��
void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);//����APB2���������ʱ��
void RCC_APB1PeriphClockCmd(uint32_t RCC_APB1Periph, FunctionalState NewState);//����APB1���������ʱ��
	
void GPIO_PinRemapConfig(uint32_t GPIO_Remap, FunctionalState NewState); //�������Ź���
	*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); //����IO����ʱ��
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	//�ر�JTAG����
	
	//2.����GPIO
	/*
void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);
typedef enum
{ GPIO_Mode_AIN = 0x0,					//ģ������
  GPIO_Mode_IN_FLOATING = 0x04,			//��������
  GPIO_Mode_IPD = 0x28,					//��������
  GPIO_Mode_IPU = 0x48,					//��������
  GPIO_Mode_Out_OD = 0x14,				//��©���
  GPIO_Mode_Out_PP = 0x10,				//�������
  GPIO_Mode_AF_OD = 0x1C,				//���ÿ�©
  GPIO_Mode_AF_PP = 0x18				//��������
}GPIOMode_TypeDef;
	*/
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;	//�������
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;		//��4������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; //ͨ������
	
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//3.����GPIO�ĳ�ʼ��״̬
	/*
void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin); //����GPIO����Ϊ�ߵ�ƽ
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);//����GPIO����Ϊ�͵�ƽ
	*/
	GPIO_SetBits(GPIOB,GPIO_Pin_4); //����PB4�ߵ�ƽ	
}
void led_ON(void )			//����LED
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_4); //����PB4�͵�ƽ
}
void led_OFF(void )		//�ر�LED
{
	GPIO_SetBits(GPIOB,GPIO_Pin_4); //����PB4�ߵ�ƽ
}
