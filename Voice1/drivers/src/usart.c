#include "usart.h"
#include "esp8266.h"
#include <stdio.h>
#include <string.h>

uint16_t recvdata;
uint16_t pos = 0;
u8 usart_RecvData[64]  ={0}; //���ڴ洮�ڽ��յ�����
//ESP8266_Fram_Record_Struct Frame;



void usart1_Init(void )			//����1�ĳ�ʼ��
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); //����GPIOA��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE); //��������1��ʱ��
	
	
	//����GPIO PA9
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP ; //�����������
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//���� PA10
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_10;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	
	//���ô���
	/*
void USART_Init(USART_TypeDef* USARTx, USART_InitTypeDef* USART_InitStruct); //��ʼ������
	*/
	//���ڲ�������
	USART_InitStruct.USART_BaudRate  = 115200;							//���ô��ڲ�����
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//���ô���Ӳ�������� ��
	USART_InitStruct.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;			//���ô���ģʽ �շ�ģʽ
	USART_InitStruct.USART_Parity = USART_Parity_No;					//���ô���У��λ  ��У��λ
	USART_InitStruct.USART_StopBits = USART_StopBits_1;					//���ô��ڵ�ֹͣλ 1λ
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;			//���ô��ڵ�����λ 8λ
	//���ڵĳ�ʼ��
	USART_Init(USART1,&USART_InitStruct);
	
	//2������һ���ж�Դ --- �Ǹ��¼��ܴ����ж�   --- һ��ͳ�ʼ������д��һ��
		// �����ж�Դ�ĺ����ڶ�Ӧ����Ŀ⺯���� 
		//void USART_ITConfig(USART_TypeDef* USARTx, uint16_t USART_IT, FunctionalState NewState); //�����ж�Դ���� 
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);	//���ڽ����ж�
	//3����������ж�Դ����ռ���ȼ�����Ӧ���ȼ�
	//void NVIC_Init(NVIC_InitTypeDef* NVIC_InitStruct); 
	/*
		typedef struct
		{
		   uint8_t NVIC_IRQChannel;           //�жϵ�ͨ����      ��stm32f10x.h����� typedef enum IRQn ö����     
		   uint8_t NVIC_IRQChannelPreemptionPriority;  	//�����ȼ� 
		   uint8_t NVIC_IRQChannelSubPriority;  		//�����ȼ�		      
		   FunctionalState NVIC_IRQChannelCmd;          //ʹ�� 
		} NVIC_InitTypeDef;	
	*/
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	
	 //ʹ�ܴ���1
	 /*
void USART_Cmd(USART_TypeDef* USARTx, FunctionalState NewState);
	 */
	 USART_Cmd(USART1,ENABLE);
}


void uart2_Init(u32 bound)//����2  ����ΪPA2  PA3
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	    //ʹ��ָ���˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);	//��ʼ��GPIO
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	//��ʼ��GPIO
	
	//Usart2 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;      
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         
    NVIC_Init(&NVIC_InitStructure); 

	
	//USART2����
	USART_InitStructure.USART_BaudRate = bound;	//���ô��ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//�ֳ�Ϊ8
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;	//����żУ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure); //����USART����
	
	USART_ITConfig(USART2, USART_IT_RXNE|USART_IT_IDLE, ENABLE);//�����˽����жϺ����߿����ж�
	
	USART_Cmd(USART2, ENABLE);                    //ʹ��USART
}
/*
	void USART_SendData(USART_TypeDef* USARTx, uint16_t Data);
*/

/*
FlagStatus USART_GetFlagStatus(USART_TypeDef* USARTx, uint16_t USART_FLAG);//��üĴ���״̬
*/
//�����ַ���
void usart_SendString(char *ch)
{
	while(*ch != '\0') //  '\0'�ַ���������־
	{
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET)
		{
		}
		USART_SendData(USART1,*ch++);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET)
		{
		}
	}
}
//��ֲprintf ����ض���
/*
1.�����չ��
	���ħ����---��target�¹�ѡ USE MicorLIB --->OK����
2.��дprintf�ײ㺯��
	int fputc(int ch ,FILE *f); //�������һ���ַ�
FILE:�ļ���ָ��

ע�⣺��Ҫ�������ͷ�ļ�


*/

int fputc(int ch ,FILE *f)
{
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET)
	{
	}
	
	USART_SendData(USART1,(uint16_t)ch);
	
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET)
	{
	}
	
	return ch;
	
}
	  
//4����дһ���жϷ�����	 void ������(void ) //����������������� �������ļ��в鿴 ����_IRQHandle
//ʵ��open���� close�ص�  ����





