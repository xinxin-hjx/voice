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
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //开启GPIOA的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE); //开启串口1的时钟
	
	
	//配置GPIO PB10
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP ; //复用推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//配置 PB11
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_11;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	
	//配置串口
	/*
void USART_Init(USART_TypeDef* USARTx, USART_InitTypeDef* USART_InitStruct); //初始化串口
	*/
	//串口参数配置
	USART_InitStruct.USART_BaudRate  = 9600;							//配置串口波特率
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//配置串口硬件流控制 无
	USART_InitStruct.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;			//配置串口模式 收发模式
	USART_InitStruct.USART_Parity = USART_Parity_No;					//配置串口校验位  无校验位
	USART_InitStruct.USART_StopBits = USART_StopBits_1;					//配置串口的停止位 1位
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;			//配置串口的数据位 8位
	//串口的初始化
	USART_Init(USART3,&USART_InitStruct);
	
	 //使能串口1
	 /*
void USART_Cmd(USART_TypeDef* USARTx, FunctionalState NewState);
	 */
	 USART_Cmd(USART3,ENABLE);
}
//数据帧的生成以及发送
/**
  * @brief  生成文本转语音的数据包并发送
  * @param  Music: 背景音乐选择  取值 0~15
  * @param  data: 要合成的文本信息.
  * @retval none
  */
void Syn6288_FramInfo(u8 Music,const char *data)
{
	/******************帧头**************************/
	unsigned char Fram_info[50];	//需要发送的数组
	unsigned char Length;			//发送的长度
	unsigned char ecc = 0;				//异或校验字节
	Length = strlen(data);		// 计算待发送文本的长度
	Fram_info[0] = 0xFD;		//构造帧头
	Fram_info[1] = 0x00;		//构造数据区长度的高字节
	Fram_info[2] = Length +3;	//构造数据区长度
	Fram_info[3] = 0x01;		//合成命令字：合成播报命令
	Fram_info[4] = 0x01 | Music << 3;	//构造命令参数：背景音乐+编码格式GB2315
	/*****************校验码计算********************************************/
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
	/*********************发送帧信息****************************************/
	
	memcpy(&Fram_info[5],data,Length);
	Fram_info[5+Length] = ecc;
	syn6288_Usart3_SendString(Fram_info,5 +Length +1);
	
}

void syn6288_BY_Init()			//初始化BY引脚
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //开启GPIOB的时钟

	
	//配置GPIO PB2
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING ; //复用推挽输出
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
 uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);//读取输入电平
 */
 
 void syn6288_Say(char *fmt)		//播报函数
 {
	delay_ms(10);
	while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2))
	{}
	delay_ms(10);
	Syn6288_FramInfo(7,fmt)	;
 }

