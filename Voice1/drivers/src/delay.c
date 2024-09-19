#include "delay.h"


void delay_us(u32 us)		//��ʱ΢����
{
	SysTick->CTRL = 0;			//����SysTick
	SysTick->LOAD = 72*us;		//1us����72�� ��ʱ��us��
	SysTick->VAL  = 0;			//��յ�ǰֵ������
	SysTick->CTRL = 5;			//-->101 ������ʱ�����ر��жϣ�ʹ��������
	//&:ȫ1Ϊ1  ��0Ϊ0   |����1ȫ1  ȫ0Ϊ0
	while((SysTick->CTRL & (1<<16)) == 0) ;  //�ȴ���������
	SysTick->CTRL = 0;			//����SysTick
}
void delay_ms(u32 ms)		//��ʱ������ ������ʱ���΢����233
{
	while(ms--)
		delay_us(1000);	
}
void delay_s(u8 s)			//��ʱ����
{
	while(s-- )
		delay_ms(1000);
	
}
