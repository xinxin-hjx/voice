
/*Includes ***********************************************************/

#include <stdio.h>

#include "stm32f10x.h"                  // Device header

#include "drv_led.h"					// LED Header
#include "drv_fan.h"					// FAN Header
#include "delay.h"						// Delay Header
#include "usart.h"						// USART Header
#include "syn6288e.h"					// Syn6288E Header
#include "esp8266.h"					// ESP8266 Header
#include "tcp.h"						// TCP Header
#include "i2c.h"
#include "SHT2X.h"
#include "spi.h"
#include "ld3320a.h"
#include "timer.h"
/*********************************************************************/
void FS_ZN(void);
extern char NetDataBuf[30];
extern char NetTimeBuf[30];
extern float temperatureC;
extern float humidityRH;
extern uint8_t res , weather, temperature;
static char sayArr[100]={0};
void User_Modification(u8 dat);
int flag3 =1;
int flag1=0;
int flag2=0;
int main()
{
	/*初始化*********************************************************/
	//1》确定中断的管理方式（五选一）  ---  只有第一个中断需要配置 --- 写在main函数的while(1)之前 
	//void NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 选择低2个分组
	
	led_Init();
	usart1_Init();
	//Timer2_Init();
	fan_Init();
	syn6288_Init();
	
	I2C_Configuration();
  SHT2X_Init();
	LD3320A_Init();
	Timer3_Init();
	TIM_SetCompare1(TIM3,300);
		ESP8266_Init(115200);
	PWM_SetCompare1(0);
//	PWM_SetCompare1(500);

//	ESP8266_Init(115200);
//	ESP8266_AT_Test();
//  printf("正在配置ESP8266\r\n");
//  ESP8266_Net_Mode_Choose(STA);
//  while(!ESP8266_JoinAP("bbqjmkll","a12345678"));
//  ESP8266_Enable_MultipleId ( DISABLE );	
//	ESP8266_STA_ConnNet();
//	ESP8266_GetNetWeather();	
//	//ESP8266_GetNetTime();	
//	//PWM_SetCompare1(500);300一档550二档750三挡
//	printf("%s",NetDataBuf);
//	printf("到此为止");
	
	/****************************************************************/
		
	while(1)
	{
		switch(nAsrStatus)
		{
			
			case LD3320A_ASR_RUNING:
			case LD3320A_ASR_ERROR:	
					 break;
			case LD3320A_ASR_NONE:
			{
				nAsrStatus=LD3320A_ASR_RUNING;
				if (LD3320A_RunAsr()==0)	/*	启动一次ASR识别流程：ASR初始化，ASR添加关键词语，启动ASR运算*/
				{
					nAsrStatus = LD3320A_ASR_ERROR;
				}
				break;
			}

			case LD3320A_ASR_FOUNDOK: /*	一次ASR识别流程结束，去取ASR识别结果*/
			{
				nAsrRes = LD3320A_GetResult();		/*获取结果*/												
				User_Modification(nAsrRes);
				nAsrStatus = LD3320A_ASR_NONE;
				break;
			}
			case LD3320A_ASR_FOUNDZERO:
			default:
			{
				nAsrStatus = LD3320A_ASR_NONE;
				break;
			}
		}
	if(flag1 == 1)
	{

	ESP8266_AT_Test();
  printf("正在配置ESP8266\r\n");
  ESP8266_Net_Mode_Choose(STA);
  while(!ESP8266_JoinAP("bbqjmkll","a12345678"));
  ESP8266_Enable_MultipleId ( DISABLE );	
	ESP8266_STA_ConnNet();

		flag1 = 0;
		flag2 = 1;
	}
	}
}

void User_Modification(u8 dat)
{
	static u8 flag = 0;
	if(dat ==0)
	{
		flag=1;
		syn6288_Say("我在");
		printf("收到\r\n");
	}
	else if(flag)
	{
		flag=0;
		switch(nAsrRes)		   /*对结果执行相关操作,客户修改*/
		{
			case CODE_YYCS: ////////
				syn6288_Say("测试成功");
				printf("测试成功\r\n");
				break;
			case CODE_KD:
				syn6288_Say("正在开灯");				
				led_ON();
				printf("打开成功\r\n");
				break;
			case CODE_GD:
				syn6288_Say("已关灯");
				led_OFF();
				printf("关闭成功\r\n");
				break;
			case CODE_KF:  //yi
				syn6288_Say("正在开风扇,默认为2档");
				PWM_SetCompare1(200);
				break;
			case CODE_GF:
				syn6288_Say("正在关风扇");
				PWM_SetCompare1(0);
				break;
			case CODE_TQ:
				printf("flag2=%d\n",flag2);
				if(flag2 == 1)
				{
					ESP8266_GetNetWeather();
					printf("%s",NetDataBuf);
				  syn6288_Say(NetDataBuf);
				}
				break;
			case CODE_WX:
				SHT2X_TEST();
				sprintf(sayArr,"室内温度%0.2f摄氏度,室内湿度百分之%0.2f",temperatureC,humidityRH);
				syn6288_Say(sayArr);				
				break;
			case CODE_ZN:
				flag3 = 0;
			  FS_ZN();
				syn6288_Say("风扇为智能模式");
				break;
			case CODE_SZ:
				syn6288_Say("风扇为语音设置模式");
				flag3 = 1;
			break;
			case CODE_YD:
				if(flag3 == 1)
				{
				PWM_SetCompare1(100);					
				}
				syn6288_Say("风扇已经调节到一档");
			break;
			case CODE_ED:
				if(flag3 == 1)
				{
					PWM_SetCompare1(200);
				}
				syn6288_Say("风扇已经调节到二档");
			break;
			case CODE_SD:
				if(flag3 == 1)
				{
					PWM_SetCompare1(300);
				}
				syn6288_Say("风扇已经调节到三档");
			break;
			case CODE_LW:
				flag1 = 1;
			syn6288_Say("设置为联网模式");
			break;
				
		}
			
	}
	else 	
	{
		printf("请说出一级口令\r\n"); /*text.....*/	
	}
	
}

void FS_ZN(void)  
{
	if(flag3 == 0)
	{
//		if(temperatureC < 20)
//		{
//			PWM_SetCompare1(0);
//		}
		
		 if( temperatureC <=27 )
		{
			PWM_SetCompare1(100);
		}
		
		else if(temperatureC >27 && temperatureC <=30 )
		{
			PWM_SetCompare1(200);
		}
		
		else if(temperatureC > 30 )
		{
			PWM_SetCompare1(300);
		}
	}
}


