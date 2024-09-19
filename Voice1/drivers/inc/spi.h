/**
  ******************************************************************************
  * @file    Utilities/SPI/spi.h
  * @author  Eric
  * @version V1.0.0
  * @date    2023-01-01
  * @brief   Header for spi.c module
  ******************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2023 ERIC</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_H
#define __SPI_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"                  // Device header

/* Exported types ------------------------------------------------------------*/
typedef enum {
	spi1 = 0,
	spi2 = 1
} BUS_SPITypeDef;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

#define SPIn                             2

/* BUS_SPI1 ------------------------------------------------------------------*/
#define BUS_SPI1                         SPI1
#define SPI1_CLK                         RCC_APB2Periph_SPI1
#define BUS_SPI1_IRQn                    SPI1_IRQn

#define IS_SPI1_REMAP                    0 // 是否重映射  0:否  1:是
#define SPI1_GPIO_REMAP                  GPIO_Remap_SPI1

#define SPI1_SCK_GPIO_CLK                RCC_APB2Periph_GPIOA
#define SPI1_SCK_GPIO_PIN                GPIO_Pin_5
#define SPI1_SCK_GPIO_PORT               GPIOA

#define SPI1_MISO_GPIO_CLK               RCC_APB2Periph_GPIOA
#define SPI1_MISO_GPIO_PIN               GPIO_Pin_6
#define SPI1_MISO_GPIO_PORT              GPIOA

#define SPI1_MOSI_GPIO_CLK               RCC_APB2Periph_GPIOA
#define SPI1_MOSI_GPIO_PIN               GPIO_Pin_7
#define SPI1_MOSI_GPIO_PORT              GPIOA

/* BUS_SPI2 ------------------------------------------------------------------*/
#define BUS_SPI2                         SPI2
#define SPI2_CLK                         RCC_APB1Periph_SPI2
#define BUS_SPI2_IRQn                    SPI2_IRQn

#define IS_SPI2_REMAP                    0 // SPI2没有重映射
#define SPI2_GPIO_REMAP                  ((uint32_t)0x00000000)

#define SPI2_SCK_GPIO_CLK                RCC_APB2Periph_GPIOB
#define SPI2_SCK_GPIO_PIN                GPIO_Pin_13
#define SPI2_SCK_GPIO_PORT               GPIOB

#define SPI2_MISO_GPIO_CLK               RCC_APB2Periph_GPIOB
#define SPI2_MISO_GPIO_PIN               GPIO_Pin_14
#define SPI2_MISO_GPIO_PORT              GPIOB

#define SPI2_MOSI_GPIO_CLK               RCC_APB2Periph_GPIOB
#define SPI2_MOSI_GPIO_PIN               GPIO_Pin_15
#define SPI2_MOSI_GPIO_PORT              GPIOB

/* Exported functions --------------------------------------------------------*/
void BUS_SPIInit(BUS_SPITypeDef SPI, SPI_InitTypeDef* SPI_InitStruct);
uint8_t BUS_SPIExchangeByte(BUS_SPITypeDef SPI, uint8_t data);

#endif /* __SPI_H */

/************************ (C) COPYRIGHT 2023 ERIC *********END OF FILE*********/
