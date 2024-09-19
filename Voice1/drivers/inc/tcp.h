
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TCP_H
#define __TCP_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"                  // Device header

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/


/* WIFI信息 ------------------------------------------------------------------*/
#define WIFI_SSID                        "bbqjmkll"
#define WIFI_PWD                         "a12345678"

/* 网络时间 ------------------------------------------------------------------*/
#define TIME_ADDRESS                     "quan.suning.com"
#define TIME_PORT                        "80"
#define TIME_INFO                        "GET http://quan.suning.com/getSysTime.do \r\n\r\n"

/* 网络天气 ------------------------------------------------------------------*/
#define WEATHER_ADDRESS                  "api.seniverse.com" 
#define WEATHER_PORT                     "80"
#define WEATHER_INFO                     "GET http://api.seniverse.com/v3/weather/now.json?key=Sz5q2z2kpHClHj4Ee&location=zhanjiang&language=en&unit=c \r\n\r\n"

/* Exported functions --------------------------------------------------------*/
void ESP8266_STA_ConnNet(void);
void ESP8266_GetNetTime(void);
void ESP8266_GetNetWeather(void);

#endif /* __TCP_H */

/************************ (C) COPYRIGHT 2023 ERIC *********END OF FILE*********/
