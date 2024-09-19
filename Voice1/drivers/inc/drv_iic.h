#ifndef __DRV_IIC_H__
#define __DRV_IIC_H__
/*Includes **************************************************************/
//GPIOģ��IIC
#include "stm32f10x.h"                  // Device header



/************************************************************************/

//SCL	ʱ����	PB6
#define IIC_SCL_PIN		GPIO_Pin_6
#define IIC_SCL_PORT	GPIOB
#define IIC_SCL_CLK		RCC_APB2Periph_GPIOB

//SDA	������	PB7
#define IIC_SDA_PIN		GPIO_Pin_7
#define IIC_SDA_PORT	GPIOB
#define IIC_SDA_CLK		RCC_APB2Periph_GPIOB

//��ƽ
#define SDA_HIGH			GPIO_SetBits(IIC_SDA_PORT,IIC_SDA_PIN)
#define SDA_LOW				GPIO_ResetBits(IIC_SDA_PORT,IIC_SDA_PIN) 
#define SDA_READ			GPIO_ReadInputDataBit(IIC_SDA_PORT,IIC_SDA_PIN)

#define SCL_HIGH			GPIO_SetBits(IIC_SCL_PORT,IIC_SCL_PIN)
#define SCL_LOW				GPIO_ResetBits(IIC_SCL_PORT,IIC_SCL_PIN)

#define ACK					0	//Ӧ���ź�
#define NOACK				1	//��Ӧ���ź�

/*Function Refrence******************************************************/
//��ʼ��IIC
void IIC_Init(void);
//	1-��ʼ�źţ�SCL�ߵ�ƽ�ڼ䣬SDA�Ӹߵ�ƽ�л����͵�ƽ������һ����Ч���½���
void IIC_Start(void);
//	2-ֹͣ�źţ�SCL�ߵ�ƽ�ڼ䣬SDA�ӵ͵�ƽ�л����ߵ�ƽ������һ����Ч��������
void IIC_Stop(void);
//	3-Ӧ���źţ�SCL�ߵ�ƽ�ڼ䣬SDA���һ���͵�ƽ
void IIC_Send_ACK(void);
//	4-��Ӧ���źţ�SCL�ߵ�ƽ�ڼ䣬SDA���һ���ߵ�ƽ
void IIC_Send_NOACK(void);
//	5-�ȴ�Ӧ���źţ�����SCL�ߣ��ȴ�SDA�ϵĵ͵�ƽ
uint8_t IIC_Wait_ACK(void);
//	6-���������źţ���SCL����׼��SDA���ϵ����ݣ���SCL���ߣ�����SDA�ϵ����ݣ�1byte��8�η��ͣ���λ���У�
void IIC_Send_Byte(u8 data);
//	7-���������źţ�����ʱ���ߣ���ȡ���ϵ����ݣ�1byte��8�ν��գ���λ���У�
uint8_t IIC_Read_Byte(u8 ack);
/************************************************************************/



#endif
