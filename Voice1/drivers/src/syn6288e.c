#include "syn6288e.h"
#include <string.h>


static void syn6288_Usart3_SendString(u8 *ch,u8 length)
{
	int i;
	for(i = 0;i<length;i++)
	{
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET)
		{
		}
		USART_SendData(USART3,*ch++);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC) == RESET)
		{
		}
	}	
}
void syn6288_Usart_Init( )
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //����GPIOA��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE); //��������1��ʱ��
	
	
	//����GPIO PB10
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP ; //�����������
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//���� PB11
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_11;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	
	//���ô���
	/*
void USART_Init(USART_TypeDef* USARTx, USART_InitTypeDef* USART_InitStruct); //��ʼ������
	*/
	//���ڲ�������
	USART_InitStruct.USART_BaudRate  = 9600;							//���ô��ڲ�����
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//���ô���Ӳ�������� ��
	USART_InitStruct.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;			//���ô���ģʽ �շ�ģʽ
	USART_InitStruct.USART_Parity = USART_Parity_No;					//���ô���У��λ  ��У��λ
	USART_InitStruct.USART_StopBits = USART_StopBits_1;					//���ô��ڵ�ֹͣλ 1λ
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;			//���ô��ڵ�����λ 8λ
	//���ڵĳ�ʼ��
	USART_Init(USART3,&USART_InitStruct);
	
	 //ʹ�ܴ���1
	 /*
void USART_Cmd(USART_TypeDef* USARTx, FunctionalState NewState);
	 */
	 USART_Cmd(USART3,ENABLE);
}
//����֡�������Լ�����
/**
  * @brief  �����ı�ת���������ݰ�������
  * @param  Music: ��������ѡ��  ȡֵ 0~15
  * @param  data: Ҫ�ϳɵ��ı���Ϣ.
  * @retval none
  */
void Syn6288_FramInfo(u8 Music,const char *data)
{
	/******************֡ͷ**************************/
	unsigned char Fram_info[50];	//��Ҫ���͵�����
	unsigned char Length;			//���͵ĳ���
	unsigned char ecc = 0;				//���У���ֽ�
	Length = strlen(data);		// ����������ı��ĳ���
	Fram_info[0] = 0xFD;		//����֡ͷ
	Fram_info[1] = 0x00;		//�������������ȵĸ��ֽ�
	Fram_info[2] = Length +3;	//��������������
	Fram_info[3] = 0x01;		//�ϳ������֣��ϳɲ�������
	Fram_info[4] = 0x01 | Music << 3;	//���������������������+�����ʽGB2315
	/*****************У�������********************************************/
	int i = 0;
	for(i = 0; i< 5;i++)
	{
		ecc = ecc^(Fram_info[i]);
	}
	for(i = 0;i< Length;i++)
	{
		ecc = ecc^(data[i]);
	}
	/***********************************************************************/
	/*********************����֡��Ϣ****************************************/
	
	memcpy(&Fram_info[5],data,Length);
	Fram_info[5+Length] = ecc;
	syn6288_Usart3_SendString(Fram_info,5 +Length +1);
	
}

void syn6288_BY_Init()			//��ʼ��BY����
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //����GPIOB��ʱ��

	
	//����GPIO PB2
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING ; //�����������
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
}

 void syn6288_Init()
 {
	 syn6288_Usart_Init();
	 syn6288_BY_Init();
 }
 
 /*
 uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);//��ȡ�����ƽ
 */
 
 void syn6288_Say(char *fmt)		//��������
 {
	delay_ms(10);
	while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2))
	{}
	delay_ms(10);
	Syn6288_FramInfo(7,fmt)	;
 }

