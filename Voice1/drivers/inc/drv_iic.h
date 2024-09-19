#ifndef __DRV_IIC_H__
#define __DRV_IIC_H__
/*Includes **************************************************************/
//GPIO模拟IIC
#include "stm32f10x.h"                  // Device header



/************************************************************************/

//SCL	时钟线	PB6
#define IIC_SCL_PIN		GPIO_Pin_6
#define IIC_SCL_PORT	GPIOB
#define IIC_SCL_CLK		RCC_APB2Periph_GPIOB

//SDA	数据线	PB7
#define IIC_SDA_PIN		GPIO_Pin_7
#define IIC_SDA_PORT	GPIOB
#define IIC_SDA_CLK		RCC_APB2Periph_GPIOB

//电平
#define SDA_HIGH			GPIO_SetBits(IIC_SDA_PORT,IIC_SDA_PIN)
#define SDA_LOW				GPIO_ResetBits(IIC_SDA_PORT,IIC_SDA_PIN) 
#define SDA_READ			GPIO_ReadInputDataBit(IIC_SDA_PORT,IIC_SDA_PIN)

#define SCL_HIGH			GPIO_SetBits(IIC_SCL_PORT,IIC_SCL_PIN)
#define SCL_LOW				GPIO_ResetBits(IIC_SCL_PORT,IIC_SCL_PIN)

#define ACK					0	//应答信号
#define NOACK				1	//非应答信号

/*Function Refrence******************************************************/
//初始化IIC
void IIC_Init(void);
//	1-起始信号：SCL高电平期间，SDA从高电平切换到低电平，产生一个有效的下降沿
void IIC_Start(void);
//	2-停止信号：SCL高电平期间，SDA从低电平切换到高电平，产生一个有效的上升沿
void IIC_Stop(void);
//	3-应答信号：SCL高电平期间，SDA输出一个低电平
void IIC_Send_ACK(void);
//	4-非应答信号：SCL高电平期间，SDA输出一个高电平
void IIC_Send_NOACK(void);
//	5-等待应答信号：拉高SCL线，等待SDA上的低电平
uint8_t IIC_Wait_ACK(void);
//	6-发送数据信号：将SCL拉低准备SDA线上的数据，将SCL拉高，发送SDA上的数据（1byte分8次发送，高位先行）
void IIC_Send_Byte(u8 data);
//	7-接收数据信号：拉高时钟线，获取线上的数据（1byte分8次接收，高位先行）
uint8_t IIC_Read_Byte(u8 ack);
/************************************************************************/



#endif
