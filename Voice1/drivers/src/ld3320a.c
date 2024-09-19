/* Includes ------------------------------------------------------------------*/
#include "ld3320a.h"
#include <stdio.h>
#include "spi.h"
#include "delay.h"
#include "syn6288e.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint8_t nLD_Mode   = LD3320A_MODE_IDLE;   //����ģʽ
__IO uint8_t nAsrStatus = LD3320A_ASR_NONE;    //ʶ��״̬
__IO uint8_t nAsrRes    = 0;                   //ʶ��ķ���ֵ

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  ����LD3320A����.
  * @param  None
  * @retval None
  */
static void LD3320A_GPIOInit(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  //����AFIOʱ��
	
	if(IS_LD3320A_GPIO_NEED_REMAP)                        //�ر�JTAG����
		GPIO_PinRemapConfig(LD3320A_GPIO_REMAP, ENABLE);
	
	/* ����GPIOʱ�� */
	RCC_APB2PeriphClockCmd(LD3320A_IRQ_GPIO_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(LD3320A_RST_GPIO_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(LD3320A_CS_GPIO_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(LD3320A_WR_GPIO_CLK, ENABLE);

	/* ����RST���� */
	GPIO_InitStructure.GPIO_Mode       = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin        = LD3320A_RST_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed      = GPIO_Speed_50MHz;
	GPIO_Init(LD3320A_RST_GPIO_PORT, &GPIO_InitStructure);
	
	/* ����WR���� */
	GPIO_InitStructure.GPIO_Pin        = LD3320A_WR_GPIO_PIN;
	GPIO_Init(LD3320A_WR_GPIO_PORT, &GPIO_InitStructure);

	/* ����CS���� */
	GPIO_InitStructure.GPIO_Pin        = LD3320A_CS_GPIO_PIN;
	GPIO_Init(LD3320A_CS_GPIO_PORT, &GPIO_InitStructure);
	
	/* ����IRQ���� */
	GPIO_InitStructure.GPIO_Mode       = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin        = LD3320A_IRQ_GPIO_PIN;
	GPIO_Init(LD3320A_IRQ_GPIO_PORT, &GPIO_InitStructure);
	
	/* ����IRQ�����ж��� */
	GPIO_EXTILineConfig(LD3320A_IRQ_GPIO_PortSource, LD3320A_IRQ_GPIO_PinSource);
	
	/* ����IRQ�ⲿ�ж� */
	EXTI_InitStructure.EXTI_Line       = LD3320A_IRQ_GPIO_EXTI_Line;
	EXTI_InitStructure.EXTI_Mode       = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger    = EXTI_Trigger_Falling;  
	EXTI_InitStructure.EXTI_LineCmd    = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* ����IRQǶ�������ж� */
	NVIC_InitStructure.NVIC_IRQChannel                    = LD3320A_IRQ_GPIO_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = 0x01;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority         = 0x01;//��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd                 = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* ����SPI */
	SPI_InitStructure.SPI_BaudRatePrescaler  = SPI_BaudRatePrescaler_8;  //LD3320A�ɽ��ܷ�Χ4-48MHz
	SPI_InitStructure.SPI_CPHA               = SPI_CPHA_1Edge;           //ʱ����λ
	SPI_InitStructure.SPI_CPOL               = SPI_CPOL_High;            //ʱ�Ӽ���
	SPI_InitStructure.SPI_DataSize           = SPI_DataSize_8b;          //8λ����
	SPI_InitStructure.SPI_Direction          = SPI_Direction_2Lines_FullDuplex; //˫��˫��ȫ˫��
	SPI_InitStructure.SPI_FirstBit           = SPI_FirstBit_MSB;         //��λ����
	SPI_InitStructure.SPI_Mode               = SPI_Mode_Master;          //����ģʽ
	SPI_InitStructure.SPI_NSS                = SPI_NSS_Soft;             //NSS�ź����������
	SPI_InitStructure.SPI_CRCPolynomial      = 0;                        //CRC����ʽ
	BUS_SPIInit(LD3320A_SPI, &SPI_InitStructure);
}

/**
  * @brief  д��Ĵ���.
  * @param  addr: �Ĵ�����ַ 
  * @param  data: д������� 
  * @retval None
  */
static void LD3320A_WriteReg(uint8_t addr, uint8_t data)
{
	/* Ƭѡ���� */
	LD3320A_CS_GPIO_Clr();
	LD3320A_WR_GPIO_Clr();
	
	/* д������ */
	BUS_SPIExchangeByte(LD3320A_SPI, 0x04);
	BUS_SPIExchangeByte(LD3320A_SPI, addr);
	BUS_SPIExchangeByte(LD3320A_SPI, data);
	
	/* Ƭѡ���� */
	LD3320A_CS_GPIO_Set();
	LD3320A_WR_GPIO_Set();
}

/**
  * @brief  ��ȡ�Ĵ���.
  * @param  addr: �Ĵ�����ַ 
  * @retval ��ȡ��ֵ
  */
static uint8_t LD3320A_ReadReg(uint8_t addr)
{
	uint8_t data;
	
	/* Ƭѡ���� */
	LD3320A_CS_GPIO_Clr();
	LD3320A_WR_GPIO_Clr();
	
	/* ��ȡ���� */
	BUS_SPIExchangeByte(LD3320A_SPI, 0x05);
	BUS_SPIExchangeByte(LD3320A_SPI, addr);
	data = BUS_SPIExchangeByte(LD3320A_SPI, 0x00);
	
	/* Ƭѡ���� */
	LD3320A_CS_GPIO_Set();
	LD3320A_WR_GPIO_Set();

	return data;
}

/**
  * @brief  ��λ.
  * @param  None
  * @retval None
  */
static void LD3320A_Reset(void)
{
	LD3320A_RST_GPIO_Set();
	delay_ms(1);
	LD3320A_RST_GPIO_Clr();
	delay_ms(1);
	LD3320A_RST_GPIO_Set();
	
	delay_ms(1);
	LD3320A_CS_GPIO_Clr();
	delay_ms(1);
	LD3320A_CS_GPIO_Set();
	delay_ms(1);
}

/**
  * @brief  ͨ������.
  * @param  None
  * @retval None
  */
static void LD3320A_CommonInit(void)
{
	LD3320A_ReadReg(0x06);        //��ȡFIFO״̬ 
	LD3320A_WriteReg(0x17, 0x35); //��λ
	delay_ms(10);
	LD3320A_ReadReg(0x06);        //��ȡFIFO״̬  
	LD3320A_WriteReg(0x89, 0x03); //��ʼ��ģ���·
	delay_ms(5);
	LD3320A_WriteReg(0xCF, 0x43); //��ʼ���ڲ�ʡ��ģʽ
	delay_ms(5);
	LD3320A_WriteReg(0xCB, 0x02); //��ȡASR���
	
	/* PLL setting */
	LD3320A_WriteReg(0x11, LD3320A_PLL_11);       //ʱ��Ƶ������1

	/* ����ģʽ */
	if (nLD_Mode == LD3320A_MODE_MP3)
	{
		LD3320A_WriteReg(0x1E, 0x00);              //��ʼ��ADCר�ÿ���
		LD3320A_WriteReg(0x19, LD3320A_PLL_MP3_19);//ʱ��Ƶ������2
		LD3320A_WriteReg(0x1B, LD3320A_PLL_MP3_1B);//ʱ��Ƶ������3		
		LD3320A_WriteReg(0x1D, LD3320A_PLL_MP3_1D);//ʱ��Ƶ������4
	}else
	{
		LD3320A_WriteReg(0x1E, 0x00);              //��ʼ��ADCר�ÿ���
		LD3320A_WriteReg(0x19, LD3320A_PLL_ASR_19);//ʱ��Ƶ������2 
		LD3320A_WriteReg(0x1B, LD3320A_PLL_ASR_1B);//ʱ��Ƶ������3
		LD3320A_WriteReg(0x1D, LD3320A_PLL_ASR_1D);//ʱ��Ƶ������4
	}	
	LD3320A_WriteReg(0xCD, 0x04);//����DSP����
	LD3320A_WriteReg(0x17, 0x4C);//ʹDSP����,ʡ��
	delay_ms(5);
	LD3320A_WriteReg(0xB9, 0x00);//��ʼ����ǰ��ӵĴ�������
	LD3320A_WriteReg(0xCF, 0x4F);//��ʼ��MP3��ASR���ڲ�ʡ��ģʽ
	LD3320A_WriteReg(0x6F, 0xFF);//��ʼ��оƬ
}

/**
  * @brief  ����ʶ���ʼ��.
  * @param  None
  * @retval None
  */
static void LD3320A_ASRInit(void)
{
	nLD_Mode = LD3320A_MODE_ASR; //ASRģʽ
	LD3320A_CommonInit();        //ͨ������
	LD3320A_WriteReg(0xBD, 0x00);//��ʼ�����ƼĴ���ΪASRģʽ
	LD3320A_WriteReg(0x17, 0x48);//����DSP
	delay_ms(10);
	LD3320A_WriteReg(0x3C, 0x80);//FIFO_EXT���޵�8λ
	LD3320A_WriteReg(0x3E, 0x07);//FIFO_EXT���޸�8λ
	LD3320A_WriteReg(0x38, 0xFF);//FIFO_EXT���޵�8λ 
	LD3320A_WriteReg(0x3A, 0x07);//FIFO_EXT���޸�8λ
	LD3320A_WriteReg(0x40, 0x00);//FIFO_EXT MCUˮ�ߵ�8λ
	LD3320A_WriteReg(0x42, 0x08);//FIFO_EXT MCUˮ�߸�8λ
	LD3320A_WriteReg(0x44, 0x00);//FIFO_EXT DSPˮ�ߵ�8λ
	LD3320A_WriteReg(0x46, 0x08);//FIFO_EXT DSPˮ�߸�8λ 
	delay_ms(1);
}

/**
  * @brief  ���LD3320A�Ƿ����.
  * @param  None 
  * @retval 0: æµ 
  * @retval 1: ����
  */
static uint8_t LD3320A_CheckASRIdleFlag(void)
{
	uint8_t j, flag = 0;
	
	for(j = 0; j < 10; j++)
	{
		if(LD3320A_ReadReg(0xB2) == 0x21)
		{			
			flag = 1;
			break;
		}
		delay_ms(10);
	}
	return flag;
}

/**
  * @brief  ��Ӵ���.
  * @param  None
  * @retval 0: ʧ�� 
  * @retval 1: �ɹ�
  */
static uint8_t LD3320A_AsrAddFixed(void)
{
	uint8_t k, flag, nAsrAddLength;

	#define ENTRY_NUM     30      //�����ά��ֵ
	#define PHRASE_LEN    40     //����һά��ֵ
	
	/*��ӹؼ��ʣ��û��޸�*/
	const char sRecog[ENTRY_NUM][PHRASE_LEN] = {
		"tian tian",
		"yu yi ce shi",
		"kai deng",
		"xi deng",				
		"yu yin xiu gai",
		"feng shan tiao wei yi dang",
		"feng shan tiao wei dang",
		"feng shan tiao wei san dang",
		"feng shan tiao wei zi neng mo shi",
		"cha xun wen shi du",
		"tian qi",
		"guan feng shan",
		"da kai feng shan",
		"lian wang mo shi",
    "wu wang"
	};	

	/*���ʶ���룬�û��޸�*/
	const uint8_t pCode[ENTRY_NUM]={
		CODE_CMD, //
		CODE_YYCS,
		CODE_KD, //
		CODE_GD, //
		CODE_SZ,
		CODE_YD,
		CODE_ED,
		CODE_SD,
		CODE_ZN,
		CODE_WX,
		CODE_TQ,
		CODE_GF,
		CODE_KF,
		CODE_LW,
		CODE_DW  //����ģʽ
		
	};
	
	flag = 1;
	for(k = 0; k < ENTRY_NUM; k++)
	{	
		if(LD3320A_CheckASRIdleFlag() == 0) //���ASR�Ƿ����
		{ 
			flag = 0;
			break;
		}

		LD3320A_WriteReg(0xC1, pCode[k]); //ʶ����Index(00H��FFH)
		LD3320A_WriteReg(0xC3, 0x00);     //ʶ�������ʱд��00
		LD3320A_WriteReg(0x08, 0x04);     //���FIFO_EXT
		delay_ms(1);
		LD3320A_WriteReg(0x08, 0x00);     //���ָ��FIFO����д��һ��00H
		delay_ms(1);
		
		for (nAsrAddLength = 0;nAsrAddLength < PHRASE_LEN; nAsrAddLength++)
		{
			if (sRecog[k][nAsrAddLength] == 0)
				break;
			LD3320A_WriteReg(0x05, sRecog[k][nAsrAddLength]);//����ǰ�ؼ���д��FIFO_EXT
		}
		
		LD3320A_WriteReg(0xB9, nAsrAddLength);//д�뵱ǰ�ؼ��ֵĳ���(ÿ���һ��ʶ����Ҫ�趨һ��)
		LD3320A_WriteReg(0xB2, 0xFF);         //DSPæ��״̬
		LD3320A_WriteReg(0x37, 0x04);         //֪ͨDSPҪ���һ��ʶ���
	}
	return flag;
}

/**
  * @brief  ����ʶ������ģʽ����.
  * @param  None
  * @retval 0: ʧ�� 
  * @retval 1: �ɹ�
  */
static uint8_t LD3320A_AsrRunInit( void )
{
	nAsrStatus = LD3320A_ASR_RUNING;//ASR����ģʽ
	LD3320A_WriteReg(0x35, MIC_VOL);//��˷�(MIC)����
	LD3320A_WriteReg(0x1C, 0x09);   //ADC���ؿ���,����������
	LD3320A_WriteReg(0xBD, 0x20);   //��ʼ�����ƼĴ���,����������
	LD3320A_WriteReg(0x08, 0x01);   //���FIFO_DATA
	delay_ms(1);
	LD3320A_WriteReg(0x08, 0x00);   //���ָ��FIFO����д��һ��00H
	delay_ms(1);

	if(LD3320A_CheckASRIdleFlag() == 0)//�ж�ASR�Ƿ����
		return 0;

	LD3320A_WriteReg(0xB2, 0xFF);//DSPæ��״̬
	LD3320A_WriteReg(0x37, 0x06);//֪ͨDSP��ʼʶ������
	delay_ms(5);
	LD3320A_WriteReg(0x1C, 0x0B);//������˷���Ϊ�����ź�
	LD3320A_WriteReg(0x29, 0x10);//����ͬ���ж�	
	LD3320A_WriteReg(0xBD, 0x00);//��ʼ�����ƼĴ���ΪASRģʽ
	return 1;
}

/**
  * @brief  ��������ʶ����.
  * @param  None
  * @retval 0: ʧ�� 
  * @retval 1: �ɹ�
  */
uint8_t LD3320A_RunAsr(void)
{
	uint8_t i = 0, asrFlag = 0;

	for(i = 0; i < 5; i++)
	{
		LD3320A_ASRInit();             //��ʼ��ASR            
		delay_ms(100);
		
		if(LD3320A_AsrAddFixed() == 0) //��Ӵ���
		{
			LD3320A_Reset();		
			delay_ms(100);			
			continue;
		}
		delay_ms(10);
		
		if(LD3320A_AsrRunInit() == 0)  //����ASRΪ����ģʽ
		{
			LD3320A_Reset();	
			delay_ms(100);			
			continue;
		}
		
		asrFlag = 1;
		break;
	}
	return asrFlag;
}


/**
  * @brief  ����LD3320A(��ʼ��).
  * @param  None
  * @retval None
  */
void LD3320A_Init(void)
{
	LD3320A_GPIOInit();           //����LD3320A����
	LD3320A_Reset();              //��λ
	nAsrStatus = LD3320A_ASR_NONE;//���õ�ǰ״̬
	nAsrRes = 0;                  //����ֵΪ0
}

/**
  * @brief  ��ȡʶ����.
  * @param  None
  * @retval None
  */
uint8_t LD3320A_GetResult(void)
{	
	return LD3320A_ReadReg(0xC5);
}


/**
  * @brief  ��Ӧ�ж�.
  * @param  None
  * @retval None
  */
void ProcessInt(void)
{
	uint8_t nAsrResCount = 0;
	
	LD3320A_WriteReg(0x29, 0x00);//������FIFO�жϺ�ͬ���ж�
	LD3320A_WriteReg(0x02, 0x00);//������FIFO_DATA�жϺ�FIFO_EXT�ж�
	
	/* ����ʶ���н������ && ��ȷ����һ������ʶ�������������� &&  ��ѯ��Ϊ��״̬ */
	if((LD3320A_ReadReg(0x2B) & 0x10) && LD3320A_ReadReg(0xBF) == 0x35 && LD3320A_ReadReg(0xB2) == 0x21)
	{
		nAsrResCount = LD3320A_ReadReg(0xBA); //�жϸ�����Ϣ

		if(nAsrResCount > 0 && nAsrResCount < 4)
			nAsrStatus = LD3320A_ASR_FOUNDOK;
		else
			nAsrStatus = LD3320A_ASR_FOUNDZERO;
	}else
		nAsrStatus = LD3320A_ASR_FOUNDZERO;

	LD3320A_WriteReg(0x2B, 0x00); //����ж�������
	LD3320A_WriteReg(0x1C, 0x00); //ADC������
}

/**
  * @brief  �жϺ���.
  * @param  None
  * @retval None
  */
void LD3320A_IRQ_GPIO_IRQHandler(void)
{
//	static int i=0;
	if(EXTI_GetITStatus(LD3320A_IRQ_GPIO_EXTI_Line) != RESET)
	{
		printf("�����ж�\r\n");
//		for(i=0; i<2; i++)
//		{
//			syn6288_Say("�ɺ���");
//		}
		ProcessInt();
		EXTI_ClearFlag(LD3320A_IRQ_GPIO_EXTI_Line);
		EXTI_ClearITPendingBit(LD3320A_IRQ_GPIO_EXTI_Line);//���LINE�ϵ��жϱ�־λ
	}		
}

/************************ (C) COPYRIGHT 2023 ERIC *********END OF FILE*********/
