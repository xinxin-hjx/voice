#include "delay.h"


void delay_us(u32 us)		//延时微妙数
{
	SysTick->CTRL = 0;			//禁用SysTick
	SysTick->LOAD = 72*us;		//1us计数72次 延时的us数
	SysTick->VAL  = 0;			//清空当前值计数器
	SysTick->CTRL = 5;			//-->101 开启定时器，关闭中断，使用主晶振
	//&:全1为1  有0为0   |：有1全1  全0为0
	while((SysTick->CTRL & (1<<16)) == 0) ;  //等待计数结束
	SysTick->CTRL = 0;			//禁用SysTick
}
void delay_ms(u32 ms)		//延时毫秒数 单次延时最大微妙数233
{
	while(ms--)
		delay_us(1000);	
}
void delay_s(u8 s)			//延时秒数
{
	while(s-- )
		delay_ms(1000);
	
}
