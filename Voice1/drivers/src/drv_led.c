#include "drv_led.h"

/*
如何点亮第一个LED灯
1.查看原理图：找接线引脚、确认点灯方式
2.配置GPIO端口
	1》打开GPIO的时钟
	2》配置GPIO的参数
	3》配置GPIO的初始化状态
3.点亮LED
*/

void led_Init(void )    	//初始化配置LED
{
	GPIO_InitTypeDef GPIO_InitStruct;
	//1.开时钟
	/*
void RCC_AHBPeriphClockCmd(uint32_t RCC_AHBPeriph, FunctionalState NewState); //控制APB总线外设的时钟
void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);//控制APB2总线外设的时钟
void RCC_APB1PeriphClockCmd(uint32_t RCC_APB1Periph, FunctionalState NewState);//控制APB1总线外设的时钟
	
void GPIO_PinRemapConfig(uint32_t GPIO_Remap, FunctionalState NewState); //设置引脚功能
	*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); //开启IO复用时钟
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	//关闭JTAG下载
	
	//2.配置GPIO
	/*
void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);
typedef enum
{ GPIO_Mode_AIN = 0x0,					//模拟输入
  GPIO_Mode_IN_FLOATING = 0x04,			//浮空输入
  GPIO_Mode_IPD = 0x28,					//下拉输入
  GPIO_Mode_IPU = 0x48,					//上拉输入
  GPIO_Mode_Out_OD = 0x14,				//开漏输出
  GPIO_Mode_Out_PP = 0x10,				//推挽输出
  GPIO_Mode_AF_OD = 0x1C,				//复用开漏
  GPIO_Mode_AF_PP = 0x18				//复用推挽
}GPIOMode_TypeDef;
	*/
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;	//推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;		//第4个引脚
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; //通信速率
	
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//3.配置GPIO的初始化状态
	/*
void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin); //设置GPIO引脚为高电平
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);//设置GPIO引脚为低电平
	*/
	GPIO_SetBits(GPIOB,GPIO_Pin_4); //设置PB4高电平	
}
void led_ON(void )			//点亮LED
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_4); //设置PB4低电平
}
void led_OFF(void )		//关闭LED
{
	GPIO_SetBits(GPIOB,GPIO_Pin_4); //设置PB4高电平
}
