
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

const char *Weather_Code[39]={"��","��","��","��","����","������","������","�󲿶���","�󲿶���","��","����","������","��������б���","С��","����","����","����","����","�ش���","����","���ѩ","��ѩ","Сѩ","��ѩ","��ѩ","��ѩ","����","��ɳ","ɳ����","ǿɳ����","��","��","��","���","쫷�","�ȴ��籩","�����","��","��"};
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  ����.
  * @param  None
  * @retval None
  */
void ESP8266_STA_ConnNet(void)
{
	ESP8266_AT_Test();                         	  //�ָ�����
	ESP8266_Net_Mode_Choose(STA);                 //��������ģʽ 
	while(!ESP8266_JoinAP(WIFI_SSID, WIFI_PWD));  //����
	ESP8266_Enable_MultipleId(DISABLE);           //������ģʽ
}

/**
  * @brief  ��ȡ����ʱ��.
  * @param  None
  * @retval None
  */
void ESP8266_GetNetTime(void)
{
	uint8_t res, i, Time[20];
	char *pTime = 0;

	while(!ESP8266_Link_Server(enumTCP, TIME_ADDRESS, TIME_PORT, Single_ID_0));    //��������ʱ�������
	while(!ESP8266_UnvarnishSend());                                               //����Ϊ͸��ģʽ
	while(!ESP8266_SendString(ENABLE, TIME_INFO, strlen(TIME_INFO), Single_ID_0))  //������������
	{
		delay_ms(1000);                   //��ʱ
		if(TcpClosedFlag)                       //�ж��Ƿ�ʧȥ����
		{
			ESP8266_ExitUnvarnishSend();        //�˳�͸��ģʽ
			do
				res = ESP8266_Get_LinkStatus(); //��ȡ����״̬
			while(!res);

			if(res == 4)                        //ȷ��ʧȥ����
			{
				while(!ESP8266_JoinAP(WIFI_SSID, WIFI_PWD)); //����
				while(!ESP8266_Link_Server(enumTCP, TIME_ADDRESS, TIME_PORT, Single_ID_0)); //��������ʱ�������   
			}
			
			while(!ESP8266_UnvarnishSend());   //͸��ģʽ
		}
	}
	
	delay_ms(1000);
	
	/* ��ȡʱ�� */
	pTime = strstr(Frame.Data_RX_BUF, "\"sysTime2\":");
	if(pTime != NULL)
	{
		for(i = 0; i < 19; i++)
			Time[i] = pTime[12+i];
		Time[19] = '\0';
	}

	ESP8266_ExitUnvarnishSend();         //�˳�͸��
	ESP8266_Close_Server(Single_ID_0);   //�ر�����
	
	sprintf(NetTimeBuf, "��ǰʱ��Ϊ,%s", Time);
}

/**
  * @brief  ��ȡ��������.
  * @param  None
  * @retval None
  */
void ESP8266_GetNetWeather(void)
{
	uint8_t res , weather, temperature;
	char *pWeather = 0;

	while(!ESP8266_Link_Server(enumTCP, WEATHER_ADDRESS, WEATHER_PORT, Single_ID_0));  //������������������
	while(!ESP8266_UnvarnishSend());                                                   //����Ϊ͸��ģʽ
	while(!ESP8266_SendString(ENABLE, WEATHER_INFO, strlen(WEATHER_INFO), Single_ID_0))//������������
	{	
		delay_ms(1000);                   //��ʱ 
		if(TcpClosedFlag)                       //�ж��Ƿ�ʧȥ����
		{  
			ESP8266_ExitUnvarnishSend();        //�˳�͸��ģʽ 
			do
				res = ESP8266_Get_LinkStatus(); //��ȡ����״̬
			while(!res);

			if(res == 4)                        //ȷ��ʧȥ����
			{
				while(!ESP8266_JoinAP(WIFI_SSID, WIFI_PWD)); //����
				while(!ESP8266_Link_Server(enumTCP, WEATHER_ADDRESS, WEATHER_PORT, Single_ID_0)); //������������������    
			} 

			while(!ESP8266_UnvarnishSend());   //͸��ģʽ  
		}
	}

	delay_ms(1000);

	/* ��ȡ���� */
	pWeather = strstr(Frame.Data_RX_BUF,"\"code\":");
	if(pWeather != NULL)
		weather = atoi(pWeather + 8);

	/* ��ȡ�¶� */
	pWeather = strstr(Frame.Data_RX_BUF,"\"temperature\":");
	if(pWeather != NULL)
		temperature = atoi(pWeather + 15);

	ESP8266_ExitUnvarnishSend();       //�˳�͸��
	ESP8266_Close_Server(Single_ID_0); //�ر�����
	
	sprintf(NetDataBuf, "տ��,%s,����,%d��", Weather_Code[weather], temperature);
}


/************************ (C) COPYRIGHT 2023 ERIC *********END OF FILE*********/
