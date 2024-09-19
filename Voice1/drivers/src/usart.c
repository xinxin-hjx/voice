#include "usart.h"
#include "esp8266.h"
#include <stdio.h>
#include <string.h>

uint16_t recvdata;
uint16_t pos = 0;
u8 usart_RecvData[64]  ={0}; //用于存串口接收的数据
//ESP8266_Fram_Record_Struct Frame;



void usart1_Init(void )			//串口1的初始化
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); //开启GPIOA的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE); //开启串口1的时钟
	
	
	//配置GPIO PA9
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP ; //复用推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//配置 PA10
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_10;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	
	//配置串口
	/*
void USART_Init(USART_TypeDef* USARTx, USART_InitTypeDef* USART_InitStruct); //初始化串口
	*/
	//串口参数配置
	USART_InitStruct.USART_BaudRate  = 115200;							//配置串口波特率
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//配置串口硬件流控制 无
	USART_InitStruct.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;			//配置串口模式 收发模式
	USART_InitStruct.USART_Parity = USART_Parity_No;					//配置串口校验位  无校验位
	USART_InitStruct.USART_StopBits = USART_StopBits_1;					//配置串口的停止位 1位
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;			//配置串口的数据位 8位
	//串口的初始化
	USART_Init(USART1,&USART_InitStruct);
	
	//2》配置一个中断源 --- 那个事件能触发中断   --- 一般和初始化函数写在一起
		// 配置中断源的函数在对应外设的库函数中 
		//void USART_ITConfig(USART_TypeDef* USARTx, uint16_t USART_IT, FunctionalState NewState); //串口中断源配置 
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);	//串口接收中断
	//3》设置这个中断源的抢占优先级和响应优先级
	//void NVIC_Init(NVIC_InitTypeDef* NVIC_InitStruct); 
	/*
		typedef struct
		{
		   uint8_t NVIC_IRQChannel;           //中断的通道号      在stm32f10x.h里面的 typedef enum IRQn 枚举中     
		   uint8_t NVIC_IRQChannelPreemptionPriority;  	//主优先级 
		   uint8_t NVIC_IRQChannelSubPriority;  		//从优先级		      
		   FunctionalState NVIC_IRQChannelCmd;          //使能 
		} NVIC_InitTypeDef;	
	*/
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	
	 //使能串口1
	 /*
void USART_Cmd(USART_TypeDef* USARTx, FunctionalState NewState);
	 */
	 USART_Cmd(USART1,ENABLE);
}


void uart2_Init(u32 bound)//串口2  引脚为PA2  PA3
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	    //使能指定端口时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);	//初始化GPIO
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	//初始化GPIO
	
	//Usart2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;      
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         
    NVIC_Init(&NVIC_InitStructure); 

	
	//USART2配置
	USART_InitStructure.USART_BaudRate = bound;	//设置串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//字长为8
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;	//无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无流控
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART2, &USART_InitStructure); //配置USART参数
	
	USART_ITConfig(USART2, USART_IT_RXNE|USART_IT_IDLE, ENABLE);//配置了接收中断和总线空闲中断
	
	USART_Cmd(USART2, ENABLE);                    //使能USART
}
/*
	void USART_SendData(USART_TypeDef* USARTx, uint16_t Data);
*/

/*
FlagStatus USART_GetFlagStatus(USART_TypeDef* USARTx, uint16_t USART_FLAG);//获得寄存器状态
*/
//发送字符串
void usart_SendString(char *ch)
{
	while(*ch != '\0') //  '\0'字符串结束标志
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
//移植printf 输出重定向
/*
1.添加扩展库
	点击魔术棒---》target下勾选 USE MicorLIB --->OK保存
2.重写printf底层函数
	int fputc(int ch ,FILE *f); //定向输出一个字符
FILE:文件流指针

注意：不要忘了添加头文件


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
	  
//4》编写一个中断服务函数	 void 函数名(void ) //函数名不能随便命名 再启动文件中查看 外设_IRQHandle
//实现open开灯 close关灯  数组





