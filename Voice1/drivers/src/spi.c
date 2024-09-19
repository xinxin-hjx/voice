

/* Includes ------------------------------------------------------------------*/
#include "spi.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
SPI_TypeDef * BUS_SPI[SPIn] = {
	BUS_SPI1, 
	BUS_SPI2
}; 

GPIO_TypeDef * SPI_SCK_PORT[SPIn] = {
	SPI1_SCK_GPIO_PORT, 
	SPI2_SCK_GPIO_PORT
};

GPIO_TypeDef * SPI_MISO_PORT[SPIn] = {
	SPI1_MISO_GPIO_PORT, 
	SPI2_MISO_GPIO_PORT
};
 
GPIO_TypeDef * SPI_MOSI_PORT[SPIn] = {
	SPI1_MOSI_GPIO_PORT, 
	SPI2_MOSI_GPIO_PORT,
};
 
const uint32_t SPI_CLK[SPIn] = {
	SPI1_CLK, 
	SPI2_CLK
};

const uint32_t SPI_SCK_CLK[SPIn] = {
	SPI1_SCK_GPIO_CLK, 
	SPI2_SCK_GPIO_CLK
};

const uint32_t SPI_MISO_CLK[SPIn] = {
	SPI1_MISO_GPIO_CLK, 
	SPI2_MISO_GPIO_CLK
};

const uint32_t SPI_MOSI_CLK[SPIn] = {
	SPI1_MOSI_GPIO_CLK, 
	SPI2_MOSI_GPIO_CLK
};

const uint16_t SPI_SCK_PIN[SPIn] = {
	SPI1_SCK_GPIO_PIN, 
	SPI2_SCK_GPIO_PIN
};

const uint16_t SPI_MISO_PIN[SPIn] = {
	SPI1_MISO_GPIO_PIN, 
	SPI2_MISO_GPIO_PIN
};

const uint16_t SPI_MOSI_PIN[SPIn] = {
	SPI1_MOSI_GPIO_PIN, 
	SPI2_MOSI_GPIO_PIN
};

const uint8_t IS_SPI_REMAP[SPIn] = {
	IS_SPI1_REMAP, 
	IS_SPI2_REMAP
};

const uint32_t SPI_REMAP[SPIn] = {
	SPI1_GPIO_REMAP, 
	SPI2_GPIO_REMAP
};


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  配置SPI.
  * @param  SPI: 指定的SPI通道.
  *   This parameter can be one of following parameters:    
  *     @arg SPI_1
  *     @arg SPI_2  
  * @param  SPI_InitStruct: pointer to a SPI_InitTypeDef structure that
  *   contains the configuration information for the specified SPI peripheral.
  * @retval None
  */
void BUS_SPIInit(BUS_SPITypeDef SPI, SPI_InitTypeDef* SPI_InitStruct)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 开启GPIO时钟 */
	RCC_APB2PeriphClockCmd(SPI_SCK_CLK[SPI] | SPI_MISO_CLK[SPI] | SPI_MOSI_CLK[SPI], ENABLE);
	
	/* 重映射 */
	if(IS_SPI_REMAP[SPI])
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
		GPIO_PinRemapConfig(SPI_REMAP[SPI], ENABLE);
	}
	
	/* 开启SPI时钟 */
	if(SPI == spi1)
		RCC_APB2PeriphClockCmd(SPI_CLK[SPI], ENABLE); 
	else
		RCC_APB1PeriphClockCmd(SPI_CLK[SPI], ENABLE);
	
	/* 配置SPI的SCK */
	GPIO_InitStructure.GPIO_Mode     = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin      = SPI_SCK_PIN[SPI];
	GPIO_InitStructure.GPIO_Speed    = GPIO_Speed_50MHz;
	GPIO_Init(SPI_SCK_PORT[SPI], &GPIO_InitStructure);
	
	/* 配置SPI的MOSI */
	GPIO_InitStructure.GPIO_Pin      = SPI_MOSI_PIN[SPI];
	GPIO_Init(SPI_MOSI_PORT[SPI], &GPIO_InitStructure);
	
	/* 配置SPI的MISO */
	GPIO_InitStructure.GPIO_Pin      = SPI_MISO_PIN[SPI];
	GPIO_InitStructure.GPIO_Mode     = GPIO_Mode_IN_FLOATING;
	GPIO_Init(SPI_MISO_PORT[SPI], &GPIO_InitStructure);

	SPI_Init(BUS_SPI[SPI], SPI_InitStruct);  //配置SPI
	SPI_Cmd(BUS_SPI[SPI], ENABLE);           //使能SPI
}

/**
  * @brief  SPI接受发送数据.
  * @param  SPI: 指定的SPI通道.
  *   This parameter can be one of following parameters:    
  *     @arg SPI_1
  *     @arg SPI_2  
  * @param  data: 发送的数据
  * @retval 接受到的数据
  */
uint8_t BUS_SPIExchangeByte(BUS_SPITypeDef SPI, uint8_t data)
{
	while(SPI_I2S_GetFlagStatus(BUS_SPI[SPI], SPI_I2S_FLAG_TXE) == RESET);  //等待发送区空
	SPI_I2S_SendData(BUS_SPI[SPI], data);                                   //发送一个字节
	while(SPI_I2S_GetFlagStatus(BUS_SPI[SPI], SPI_I2S_FLAG_RXNE) == RESET); //等待接收完
	return SPI_I2S_ReceiveData(BUS_SPI[SPI]);                               //接受一个字节
}

/************************ (C) COPYRIGHT 2023 ERIC *********END OF FILE*********/
