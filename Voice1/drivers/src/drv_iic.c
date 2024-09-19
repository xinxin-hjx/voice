#include "drv_iic.h"
#include "delay.h"


//SDA输出配置
static void IIC_SDA_OUT()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;	//开漏输出
	GPIO_InitStruct.GPIO_Pin = IIC_SDA_PIN;
	GPIO_InitStruct.GPIO_Speed =GPIO_Speed_10MHz;
	GPIO_Init(IIC_SDA_PORT,&GPIO_InitStruct);
	
}

//SDA输入配置
static void IIC_SDA_IN()
{
	GPIO_InitTypeDef GPIO_InitStruct;	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入
	GPIO_InitStruct.GPIO_Pin = IIC_SDA_PIN;
	GPIO_InitStruct.GPIO_Speed =GPIO_Speed_10MHz;
	GPIO_Init(IIC_SDA_PORT,&GPIO_InitStruct);
	
}
//IIC 初始化函数
void IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	//配置引脚
	RCC_APB2PeriphClockCmd(IIC_SCL_CLK|IIC_SDA_CLK,ENABLE);	
	
	//配置SCL
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;	//开漏输出
	GPIO_InitStruct.GPIO_Pin = IIC_SCL_PIN;
	GPIO_InitStruct.GPIO_Speed =GPIO_Speed_10MHz;
	GPIO_Init(IIC_SCL_PORT,&GPIO_InitStruct);

}

//	1-起始信号：SCL高电平期间，SDA从高电平切换到低电平，产生一个有效的下降沿
void IIC_Start(void)
{
	SCL_LOW;
	IIC_SDA_OUT();	//SDA输出模式
	SDA_HIGH;
	SCL_HIGH;
	delay_us(5);	
	SDA_LOW;
	delay_us(5);
	SCL_LOW;
		
}

//	2-停止信号：SCL高电平期间，SDA从低电平切换到高电平，产生一个有效的上升沿
void IIC_Stop(void)
{
	SCL_LOW;
	IIC_SDA_OUT();
	SDA_LOW;
	SCL_HIGH;
	delay_us(5);
	SDA_HIGH;
	delay_us(5);
	
}

//	3-应答信号：SCL高电平期间，SDA输出一个低电平
void IIC_Send_ACK(void)
{
	SCL_LOW;
	IIC_SDA_OUT();
	SDA_LOW;
	SCL_HIGH;
	delay_us(5);
	SCL_LOW;
	
}
//	4-非应答信号：SCL高电平期间，SDA输出一个高电平
void IIC_Send_NOACK(void)
{
	SCL_LOW;
	IIC_SDA_OUT();
	SDA_HIGH;
	SCL_HIGH;
	delay_us(5);
	SCL_LOW;	
}

//	5-等待应答信号：拉高SCL线，等待SDA上的低电平
uint8_t IIC_Wait_ACK(void)
{
	u8 i;
	SCL_LOW;
	IIC_SDA_IN();
	SCL_HIGH;
	delay_us(4);
	while(SDA_READ)
	{
		i++;
		if(i>200)
		{
			IIC_Stop();
			return NOACK;
		}
	}
	
	return ACK;
}

//	6-发送数据信号：将SCL拉低准备SDA线上的数据，将SCL拉高，发送SDA上的数据（1byte分8次发送，高位先行）
void IIC_Send_Byte(u8 data)
{
	u8 i=0;
	SCL_LOW;
	IIC_SDA_OUT();
	
	for(i=0;i<8;i++)
	{
		if (data &=(1<<8))	//0x80
		{
			SDA_HIGH;
		}
		else
		{
			SDA_LOW;
		}
		SCL_HIGH;
		delay_us(5);
		SCL_LOW;
		data <<= 1;
	}
	SCL_LOW;
}

//	7-接收数据信号：拉高时钟线，获取线上的数据（1byte分8次接收，高位先行）
uint8_t IIC_Read_Byte(u8 ack)
{
	u8 i=0;
	uint8_t data =0;
	SCL_LOW;
	IIC_SDA_IN();
	
	for(i=0;i<8;i++)
	{
		SCL_HIGH;
		if (SDA_READ)
		{
			data |= 0x01;
		}
		SCL_LOW;
		delay_us(4);
		
	}
	SCL_LOW;
	if (ack == ACK)
		IIC_Send_ACK();
	else
		IIC_Send_NOACK();
	
	return data;
}

