#include "drv_iic.h"
#include "delay.h"


//SDA�������
static void IIC_SDA_OUT()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;	//��©���
	GPIO_InitStruct.GPIO_Pin = IIC_SDA_PIN;
	GPIO_InitStruct.GPIO_Speed =GPIO_Speed_10MHz;
	GPIO_Init(IIC_SDA_PORT,&GPIO_InitStruct);
	
}

//SDA��������
static void IIC_SDA_IN()
{
	GPIO_InitTypeDef GPIO_InitStruct;	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//��������
	GPIO_InitStruct.GPIO_Pin = IIC_SDA_PIN;
	GPIO_InitStruct.GPIO_Speed =GPIO_Speed_10MHz;
	GPIO_Init(IIC_SDA_PORT,&GPIO_InitStruct);
	
}
//IIC ��ʼ������
void IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	//��������
	RCC_APB2PeriphClockCmd(IIC_SCL_CLK|IIC_SDA_CLK,ENABLE);	
	
	//����SCL
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;	//��©���
	GPIO_InitStruct.GPIO_Pin = IIC_SCL_PIN;
	GPIO_InitStruct.GPIO_Speed =GPIO_Speed_10MHz;
	GPIO_Init(IIC_SCL_PORT,&GPIO_InitStruct);

}

//	1-��ʼ�źţ�SCL�ߵ�ƽ�ڼ䣬SDA�Ӹߵ�ƽ�л����͵�ƽ������һ����Ч���½���
void IIC_Start(void)
{
	SCL_LOW;
	IIC_SDA_OUT();	//SDA���ģʽ
	SDA_HIGH;
	SCL_HIGH;
	delay_us(5);	
	SDA_LOW;
	delay_us(5);
	SCL_LOW;
		
}

//	2-ֹͣ�źţ�SCL�ߵ�ƽ�ڼ䣬SDA�ӵ͵�ƽ�л����ߵ�ƽ������һ����Ч��������
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

//	3-Ӧ���źţ�SCL�ߵ�ƽ�ڼ䣬SDA���һ���͵�ƽ
void IIC_Send_ACK(void)
{
	SCL_LOW;
	IIC_SDA_OUT();
	SDA_LOW;
	SCL_HIGH;
	delay_us(5);
	SCL_LOW;
	
}
//	4-��Ӧ���źţ�SCL�ߵ�ƽ�ڼ䣬SDA���һ���ߵ�ƽ
void IIC_Send_NOACK(void)
{
	SCL_LOW;
	IIC_SDA_OUT();
	SDA_HIGH;
	SCL_HIGH;
	delay_us(5);
	SCL_LOW;	
}

//	5-�ȴ�Ӧ���źţ�����SCL�ߣ��ȴ�SDA�ϵĵ͵�ƽ
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

//	6-���������źţ���SCL����׼��SDA���ϵ����ݣ���SCL���ߣ�����SDA�ϵ����ݣ�1byte��8�η��ͣ���λ���У�
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

//	7-���������źţ�����ʱ���ߣ���ȡ���ϵ����ݣ�1byte��8�ν��գ���λ���У�
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

