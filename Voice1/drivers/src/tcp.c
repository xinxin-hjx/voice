
/* Includes ------------------------------------------------------------------*/
#include "tcp.h"
#include "esp8266.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "delay.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
extern  ESP8266_Fram_Record_Struct Frame;

/* Private variables ---------------------------------------------------------*/
__IO uint8_t TcpClosedFlag;
char NetDataBuf[30];
char NetTimeBuf[30];

const char *Weather_Code[39]={"晴","晴","晴","晴","多云","晴间多云","晴间多云","大部多云","大部多云","阴","阵雨","雷阵雨","雷阵雨伴有冰雹","小雨","中雨","大雨","暴雨","大暴雨","特大暴雨","冻雨","雨夹雪","阵雪","小雪","中雪","大雪","暴雪","浮尘","扬沙","沙尘暴","强沙尘暴","雾","霾","风","大风","飓风","热带风暴","龙卷风","冷","热"};
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  联网.
  * @param  None
  * @retval None
  */
void ESP8266_STA_ConnNet(void)
{
	ESP8266_AT_Test();                         	  //恢复出厂
	ESP8266_Net_Mode_Choose(STA);                 //设置网络模式 
	while(!ESP8266_JoinAP(WIFI_SSID, WIFI_PWD));  //联网
	ESP8266_Enable_MultipleId(DISABLE);           //单连接模式
}

/**
  * @brief  获取网络时间.
  * @param  None
  * @retval None
  */
void ESP8266_GetNetTime(void)
{
	uint8_t res, i, Time[20];
	char *pTime = 0;

	while(!ESP8266_Link_Server(enumTCP, TIME_ADDRESS, TIME_PORT, Single_ID_0));    //连接网络时间服务器
	while(!ESP8266_UnvarnishSend());                                               //设置为透传模式
	while(!ESP8266_SendString(ENABLE, TIME_INFO, strlen(TIME_INFO), Single_ID_0))  //发送数据请求
	{
		delay_ms(1000);                   //延时
		if(TcpClosedFlag)                       //判断是否失去连接
		{
			ESP8266_ExitUnvarnishSend();        //退出透传模式
			do
				res = ESP8266_Get_LinkStatus(); //获取连接状态
			while(!res);

			if(res == 4)                        //确认失去连接
			{
				while(!ESP8266_JoinAP(WIFI_SSID, WIFI_PWD)); //联网
				while(!ESP8266_Link_Server(enumTCP, TIME_ADDRESS, TIME_PORT, Single_ID_0)); //连接网络时间服务器   
			}
			
			while(!ESP8266_UnvarnishSend());   //透传模式
		}
	}
	
	delay_ms(1000);
	
	/* 获取时间 */
	pTime = strstr(Frame.Data_RX_BUF, "\"sysTime2\":");
	if(pTime != NULL)
	{
		for(i = 0; i < 19; i++)
			Time[i] = pTime[12+i];
		Time[19] = '\0';
	}

	ESP8266_ExitUnvarnishSend();         //退出透传
	ESP8266_Close_Server(Single_ID_0);   //关闭连接
	
	sprintf(NetTimeBuf, "当前时间为,%s", Time);
}

/**
  * @brief  获取网络天气.
  * @param  None
  * @retval None
  */
void ESP8266_GetNetWeather(void)
{
	uint8_t res , weather, temperature;
	char *pWeather = 0;

	while(!ESP8266_Link_Server(enumTCP, WEATHER_ADDRESS, WEATHER_PORT, Single_ID_0));  //连接网络天气服务器
	while(!ESP8266_UnvarnishSend());                                                   //设置为透传模式
	while(!ESP8266_SendString(ENABLE, WEATHER_INFO, strlen(WEATHER_INFO), Single_ID_0))//发送数据请求
	{	
		delay_ms(1000);                   //延时 
		if(TcpClosedFlag)                       //判断是否失去连接
		{  
			ESP8266_ExitUnvarnishSend();        //退出透传模式 
			do
				res = ESP8266_Get_LinkStatus(); //获取连接状态
			while(!res);

			if(res == 4)                        //确认失去连接
			{
				while(!ESP8266_JoinAP(WIFI_SSID, WIFI_PWD)); //联网
				while(!ESP8266_Link_Server(enumTCP, WEATHER_ADDRESS, WEATHER_PORT, Single_ID_0)); //连接网络天气服务器    
			} 

			while(!ESP8266_UnvarnishSend());   //透传模式  
		}
	}

	delay_ms(1000);

	/* 获取天气 */
	pWeather = strstr(Frame.Data_RX_BUF,"\"code\":");
	if(pWeather != NULL)
		weather = atoi(pWeather + 8);

	/* 获取温度 */
	pWeather = strstr(Frame.Data_RX_BUF,"\"temperature\":");
	if(pWeather != NULL)
		temperature = atoi(pWeather + 15);

	ESP8266_ExitUnvarnishSend();       //退出透传
	ESP8266_Close_Server(Single_ID_0); //关闭连接
	
	sprintf(NetDataBuf, "湛江,%s,气温,%d℃", Weather_Code[weather], temperature);
}


/************************ (C) COPYRIGHT 2023 ERIC *********END OF FILE*********/
