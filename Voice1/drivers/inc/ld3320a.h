/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LD3320A_H
#define __LD3320A_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "spi.h"


extern __IO uint8_t nAsrStatus;
extern __IO uint8_t nAsrRes;
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* LD3320A IRQ ---------------------------------------------------------------*/

#define LD3320A_IRQ_GPIO_CLK             RCC_APB2Periph_GPIOB
#define LD3320A_IRQ_GPIO_PIN             GPIO_Pin_1
#define LD3320A_IRQ_GPIO_PORT            GPIOB
#define LD3320A_IRQ_GPIO_PortSource      GPIO_PortSourceGPIOB
#define LD3320A_IRQ_GPIO_PinSource       GPIO_PinSource1
#define LD3320A_IRQ_GPIO_EXTI_Line       EXTI_Line1
#define LD3320A_IRQ_GPIO_IRQChannel      EXTI1_IRQn
#define LD3320A_IRQ_GPIO_IRQHandler      EXTI1_IRQHandler

/* LD3320A RST ---------------------------------------------------------------*/
#define LD3320A_RST_GPIO_CLK             RCC_APB2Periph_GPIOB
#define LD3320A_RST_GPIO_PIN             GPIO_Pin_3
#define LD3320A_RST_GPIO_PORT            GPIOB
#define LD3320A_RST_GPIO_Set()           GPIO_SetBits(LD3320A_RST_GPIO_PORT, LD3320A_RST_GPIO_PIN)
#define LD3320A_RST_GPIO_Clr()           GPIO_ResetBits(LD3320A_RST_GPIO_PORT, LD3320A_RST_GPIO_PIN)

/* LD3320A CS ----------------------------------------------------------------*/
#define LD3320A_CS_GPIO_CLK              RCC_APB2Periph_GPIOB
#define LD3320A_CS_GPIO_PIN              GPIO_Pin_0
#define LD3320A_CS_GPIO_PORT             GPIOB
#define LD3320A_CS_GPIO_Set()            GPIO_SetBits(LD3320A_CS_GPIO_PORT, LD3320A_CS_GPIO_PIN)
#define LD3320A_CS_GPIO_Clr()            GPIO_ResetBits(LD3320A_CS_GPIO_PORT, LD3320A_CS_GPIO_PIN)

/* LD3320A WR ----------------------------------------------------------------*/
#define LD3320A_WR_GPIO_CLK              RCC_APB2Periph_GPIOB
#define LD3320A_WR_GPIO_PIN              GPIO_Pin_5
#define LD3320A_WR_GPIO_PORT             GPIOB
#define LD3320A_WR_GPIO_Set()            GPIO_SetBits(LD3320A_WR_GPIO_PORT, LD3320A_WR_GPIO_PIN)
#define LD3320A_WR_GPIO_Clr()            GPIO_ResetBits(LD3320A_WR_GPIO_PORT, LD3320A_WR_GPIO_PIN)

/* LD3320A GPIO Remap --------------------------------------------------------*/
#define IS_LD3320A_GPIO_NEED_REMAP       1 // 0:����Ҫ   1:��Ҫ
#define LD3320A_GPIO_REMAP               GPIO_Remap_SWJ_JTAGDisable

/* LD3320A SPI ---------------------------------------------------------------*/
#define LD3320A_SPI                      spi2

/* ��������״̬����������¼������������ASRʶ����������MP3���� --------------*/
#define LD3320A_MODE_IDLE                0x00
#define LD3320A_MODE_ASR                 0x08
#define LD3320A_MODE_MP3                 0x40

/* LD3320 ʱ��Ƶ�� -----------------------------------------------------------*/
#define LD3320A_CLK_IN                   24 //�û�ע���޸�����ľ���ʱ�Ӵ�С
#define LD3320A_PLL_11                   (uint8_t)((LD3320A_CLK_IN / 2.0) - 1)

#define LD3320A_PLL_MP3_19               0x0F
#define LD3320A_PLL_MP3_1B               0x18
#define LD3320A_PLL_MP3_1D               (uint8_t)(((90.0 * ((LD3320A_PLL_11) + 1)) / (LD3320A_CLK_IN)) - 1)

#define LD3320A_PLL_ASR_19               (uint8_t)(LD3320A_CLK_IN * 32.0 / (LD3320A_PLL_11 + 1) - 0.51)
#define LD3320A_PLL_ASR_1B               0x48
#define LD3320A_PLL_ASR_1D               0x1F

/* �������״̬����������¼������������ASRʶ������е��ĸ�״̬ ---------------*/
#define LD3320A_ASR_NONE                 0x00 //��ʾû������ASRʶ��
#define LD3320A_ASR_RUNING               0x01 //��ʾLD3320������ASRʶ����
#define LD3320A_ASR_FOUNDOK              0x10 //��ʾһ��ʶ�����̽�������һ��ʶ����
#define LD3320A_ASR_FOUNDZERO            0x11 //��ʾһ��ʶ�����̽�����û��ʶ����
#define LD3320A_ASR_ERROR                0x31 //��ʾһ��ʶ��������LD3320оƬ�ڲ����ֲ���ȷ��״̬

#define MIC_VOL                          0x43 //��ͷ����

/* LD3320 CODE ---------------------------------------------------------------*/
#define CODE_CMD                         0x00 // ��������0x00�û����ɽ����޸ġ�
#define CODE_YYCS                        0x01 // ��������
#define CODE_KD                          0x02 // ����
#define CODE_GD                          0x03 // �ص�
#define CODE_SZ							             0x04 //
#define CODE_YD							             0x05 //һ��
#define CODE_ED							             0x06 //��
#define CODE_SD							             0x07 //��
#define CODE_ZN							             0x08 //
#define CODE_WX							             0x09
#define CODE_TQ							             0x10 //
#define CODE_GF							             0x11 //
#define CODE_KF							             0x12 //
#define CODE_LW							             0x13
#define CODE_DW							             0x14
//#define CODE_YYCS						           0x15
/* Exported functions --------------------------------------------------------*/
void LD3320A_Init(void);
uint8_t LD3320A_RunAsr(void);
uint8_t LD3320A_GetResult(void);

#endif /* __LD3320A_H */

/************************ (C) COPYRIGHT 2023 ERIC *********END OF FILE*********/
