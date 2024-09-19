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
__IO uint8_t nLD_Mode   = LD3320A_MODE_IDLE;   //工作模式
__IO uint8_t nAsrStatus = LD3320A_ASR_NONE;    //识别状态
__IO uint8_t nAsrRes    = 0;                   //识别的返回值

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  配置LD3320A引脚.
  * @param  None
  * @retval None
  */
static void LD3320A_GPIOInit(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  //配置AFIO时钟
	
	if(IS_LD3320A_GPIO_NEED_REMAP)                        //关闭JTAG下载
		GPIO_PinRemapConfig(LD3320A_GPIO_REMAP, ENABLE);
	
	/* 配置GPIO时钟 */
	RCC_APB2PeriphClockCmd(LD3320A_IRQ_GPIO_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(LD3320A_RST_GPIO_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(LD3320A_CS_GPIO_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(LD3320A_WR_GPIO_CLK, ENABLE);

	/* 配置RST引脚 */
	GPIO_InitStructure.GPIO_Mode       = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin        = LD3320A_RST_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed      = GPIO_Speed_50MHz;
	GPIO_Init(LD3320A_RST_GPIO_PORT, &GPIO_InitStructure);
	
	/* 配置WR引脚 */
	GPIO_InitStructure.GPIO_Pin        = LD3320A_WR_GPIO_PIN;
	GPIO_Init(LD3320A_WR_GPIO_PORT, &GPIO_InitStructure);

	/* 配置CS引脚 */
	GPIO_InitStructure.GPIO_Pin        = LD3320A_CS_GPIO_PIN;
	GPIO_Init(LD3320A_CS_GPIO_PORT, &GPIO_InitStructure);
	
	/* 配置IRQ引脚 */
	GPIO_InitStructure.GPIO_Mode       = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin        = LD3320A_IRQ_GPIO_PIN;
	GPIO_Init(LD3320A_IRQ_GPIO_PORT, &GPIO_InitStructure);
	
	/* 配置IRQ引脚中断线 */
	GPIO_EXTILineConfig(LD3320A_IRQ_GPIO_PortSource, LD3320A_IRQ_GPIO_PinSource);
	
	/* 配置IRQ外部中断 */
	EXTI_InitStructure.EXTI_Line       = LD3320A_IRQ_GPIO_EXTI_Line;
	EXTI_InitStructure.EXTI_Mode       = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger    = EXTI_Trigger_Falling;  
	EXTI_InitStructure.EXTI_LineCmd    = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* 配置IRQ嵌套向量中断 */
	NVIC_InitStructure.NVIC_IRQChannel                    = LD3320A_IRQ_GPIO_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = 0x01;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority         = 0x01;//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd                 = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* 配置SPI */
	SPI_InitStructure.SPI_BaudRatePrescaler  = SPI_BaudRatePrescaler_8;  //LD3320A可接受范围4-48MHz
	SPI_InitStructure.SPI_CPHA               = SPI_CPHA_1Edge;           //时钟相位
	SPI_InitStructure.SPI_CPOL               = SPI_CPOL_High;            //时钟极性
	SPI_InitStructure.SPI_DataSize           = SPI_DataSize_8b;          //8位数据
	SPI_InitStructure.SPI_Direction          = SPI_Direction_2Lines_FullDuplex; //双线双向全双工
	SPI_InitStructure.SPI_FirstBit           = SPI_FirstBit_MSB;         //高位先行
	SPI_InitStructure.SPI_Mode               = SPI_Mode_Master;          //主机模式
	SPI_InitStructure.SPI_NSS                = SPI_NSS_Soft;             //NSS信号由软件控制
	SPI_InitStructure.SPI_CRCPolynomial      = 0;                        //CRC多项式
	BUS_SPIInit(LD3320A_SPI, &SPI_InitStructure);
}

/**
  * @brief  写入寄存器.
  * @param  addr: 寄存器地址 
  * @param  data: 写入的数据 
  * @retval None
  */
static void LD3320A_WriteReg(uint8_t addr, uint8_t data)
{
	/* 片选拉低 */
	LD3320A_CS_GPIO_Clr();
	LD3320A_WR_GPIO_Clr();
	
	/* 写入数据 */
	BUS_SPIExchangeByte(LD3320A_SPI, 0x04);
	BUS_SPIExchangeByte(LD3320A_SPI, addr);
	BUS_SPIExchangeByte(LD3320A_SPI, data);
	
	/* 片选拉高 */
	LD3320A_CS_GPIO_Set();
	LD3320A_WR_GPIO_Set();
}

/**
  * @brief  读取寄存器.
  * @param  addr: 寄存器地址 
  * @retval 读取的值
  */
static uint8_t LD3320A_ReadReg(uint8_t addr)
{
	uint8_t data;
	
	/* 片选拉低 */
	LD3320A_CS_GPIO_Clr();
	LD3320A_WR_GPIO_Clr();
	
	/* 读取数据 */
	BUS_SPIExchangeByte(LD3320A_SPI, 0x05);
	BUS_SPIExchangeByte(LD3320A_SPI, addr);
	data = BUS_SPIExchangeByte(LD3320A_SPI, 0x00);
	
	/* 片选拉高 */
	LD3320A_CS_GPIO_Set();
	LD3320A_WR_GPIO_Set();

	return data;
}

/**
  * @brief  复位.
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
  * @brief  通用配置.
  * @param  None
  * @retval None
  */
static void LD3320A_CommonInit(void)
{
	LD3320A_ReadReg(0x06);        //读取FIFO状态 
	LD3320A_WriteReg(0x17, 0x35); //软复位
	delay_ms(10);
	LD3320A_ReadReg(0x06);        //读取FIFO状态  
	LD3320A_WriteReg(0x89, 0x03); //初始化模拟电路
	delay_ms(5);
	LD3320A_WriteReg(0xCF, 0x43); //初始化内部省电模式
	delay_ms(5);
	LD3320A_WriteReg(0xCB, 0x02); //读取ASR结果
	
	/* PLL setting */
	LD3320A_WriteReg(0x11, LD3320A_PLL_11);       //时钟频率设置1

	/* 音乐模式 */
	if (nLD_Mode == LD3320A_MODE_MP3)
	{
		LD3320A_WriteReg(0x1E, 0x00);              //初始化ADC专用控制
		LD3320A_WriteReg(0x19, LD3320A_PLL_MP3_19);//时钟频率设置2
		LD3320A_WriteReg(0x1B, LD3320A_PLL_MP3_1B);//时钟频率设置3		
		LD3320A_WriteReg(0x1D, LD3320A_PLL_MP3_1D);//时钟频率设置4
	}else
	{
		LD3320A_WriteReg(0x1E, 0x00);              //初始化ADC专用控制
		LD3320A_WriteReg(0x19, LD3320A_PLL_ASR_19);//时钟频率设置2 
		LD3320A_WriteReg(0x1B, LD3320A_PLL_ASR_1B);//时钟频率设置3
		LD3320A_WriteReg(0x1D, LD3320A_PLL_ASR_1D);//时钟频率设置4
	}	
	LD3320A_WriteReg(0xCD, 0x04);//允许DSP休眠
	LD3320A_WriteReg(0x17, 0x4C);//使DSP休眠,省电
	delay_ms(5);
	LD3320A_WriteReg(0xB9, 0x00);//初始化当前添加的词条长度
	LD3320A_WriteReg(0xCF, 0x4F);//初始化MP3和ASR的内部省电模式
	LD3320A_WriteReg(0x6F, 0xFF);//初始化芯片
}

/**
  * @brief  语音识别初始化.
  * @param  None
  * @retval None
  */
static void LD3320A_ASRInit(void)
{
	nLD_Mode = LD3320A_MODE_ASR; //ASR模式
	LD3320A_CommonInit();        //通用配置
	LD3320A_WriteReg(0xBD, 0x00);//初始化控制寄存器为ASR模式
	LD3320A_WriteReg(0x17, 0x48);//激活DSP
	delay_ms(10);
	LD3320A_WriteReg(0x3C, 0x80);//FIFO_EXT下限低8位
	LD3320A_WriteReg(0x3E, 0x07);//FIFO_EXT下限高8位
	LD3320A_WriteReg(0x38, 0xFF);//FIFO_EXT上限低8位 
	LD3320A_WriteReg(0x3A, 0x07);//FIFO_EXT上限高8位
	LD3320A_WriteReg(0x40, 0x00);//FIFO_EXT MCU水线低8位
	LD3320A_WriteReg(0x42, 0x08);//FIFO_EXT MCU水线高8位
	LD3320A_WriteReg(0x44, 0x00);//FIFO_EXT DSP水线低8位
	LD3320A_WriteReg(0x46, 0x08);//FIFO_EXT DSP水线高8位 
	delay_ms(1);
}

/**
  * @brief  检测LD3320A是否空闲.
  * @param  None 
  * @retval 0: 忙碌 
  * @retval 1: 空闲
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
  * @brief  添加词条.
  * @param  None
  * @retval 0: 失败 
  * @retval 1: 成功
  */
static uint8_t LD3320A_AsrAddFixed(void)
{
	uint8_t k, flag, nAsrAddLength;

	#define ENTRY_NUM     30      //数组二维数值
	#define PHRASE_LEN    40     //数组一维数值
	
	/*添加关键词，用户修改*/
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

	/*添加识别码，用户修改*/
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
		CODE_DW  //无网模式
		
	};
	
	flag = 1;
	for(k = 0; k < ENTRY_NUM; k++)
	{	
		if(LD3320A_CheckASRIdleFlag() == 0) //检测ASR是否空闲
		{ 
			flag = 0;
			break;
		}

		LD3320A_WriteReg(0xC1, pCode[k]); //识别码Index(00H—FFH)
		LD3320A_WriteReg(0xC3, 0x00);     //识别码添加时写入00
		LD3320A_WriteReg(0x08, 0x04);     //清除FIFO_EXT
		delay_ms(1);
		LD3320A_WriteReg(0x08, 0x00);     //清除指定FIFO后再写入一次00H
		delay_ms(1);
		
		for (nAsrAddLength = 0;nAsrAddLength < PHRASE_LEN; nAsrAddLength++)
		{
			if (sRecog[k][nAsrAddLength] == 0)
				break;
			LD3320A_WriteReg(0x05, sRecog[k][nAsrAddLength]);//将当前关键字写入FIFO_EXT
		}
		
		LD3320A_WriteReg(0xB9, nAsrAddLength);//写入当前关键字的长度(每添加一条识别句后要设定一次)
		LD3320A_WriteReg(0xB2, 0xFF);         //DSP忙闲状态
		LD3320A_WriteReg(0x37, 0x04);         //通知DSP要添加一项识别句
	}
	return flag;
}

/**
  * @brief  语音识别运行模式配置.
  * @param  None
  * @retval 0: 失败 
  * @retval 1: 成功
  */
static uint8_t LD3320A_AsrRunInit( void )
{
	nAsrStatus = LD3320A_ASR_RUNING;//ASR运行模式
	LD3320A_WriteReg(0x35, MIC_VOL);//麦克风(MIC)音量
	LD3320A_WriteReg(0x1C, 0x09);   //ADC开关控制,保留命令字
	LD3320A_WriteReg(0xBD, 0x20);   //初始化控制寄存器,保留命令字
	LD3320A_WriteReg(0x08, 0x01);   //清除FIFO_DATA
	delay_ms(1);
	LD3320A_WriteReg(0x08, 0x00);   //清除指定FIFO后再写入一次00H
	delay_ms(1);

	if(LD3320A_CheckASRIdleFlag() == 0)//判断ASR是否空闲
		return 0;

	LD3320A_WriteReg(0xB2, 0xFF);//DSP忙闲状态
	LD3320A_WriteReg(0x37, 0x06);//通知DSP开始识别语音
	delay_ms(5);
	LD3320A_WriteReg(0x1C, 0x0B);//配置麦克风做为输入信号
	LD3320A_WriteReg(0x29, 0x10);//允许同步中断	
	LD3320A_WriteReg(0xBD, 0x00);//初始化控制寄存器为ASR模式
	return 1;
}

/**
  * @brief  启动语音识别功能.
  * @param  None
  * @retval 0: 失败 
  * @retval 1: 成功
  */
uint8_t LD3320A_RunAsr(void)
{
	uint8_t i = 0, asrFlag = 0;

	for(i = 0; i < 5; i++)
	{
		LD3320A_ASRInit();             //初始化ASR            
		delay_ms(100);
		
		if(LD3320A_AsrAddFixed() == 0) //添加词条
		{
			LD3320A_Reset();		
			delay_ms(100);			
			continue;
		}
		delay_ms(10);
		
		if(LD3320A_AsrRunInit() == 0)  //配置ASR为运行模式
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
  * @brief  配置LD3320A(初始化).
  * @param  None
  * @retval None
  */
void LD3320A_Init(void)
{
	LD3320A_GPIOInit();           //配置LD3320A引脚
	LD3320A_Reset();              //复位
	nAsrStatus = LD3320A_ASR_NONE;//设置当前状态
	nAsrRes = 0;                  //返回值为0
}

/**
  * @brief  获取识别结果.
  * @param  None
  * @retval None
  */
uint8_t LD3320A_GetResult(void)
{	
	return LD3320A_ReadReg(0xC5);
}


/**
  * @brief  响应中断.
  * @param  None
  * @retval None
  */
void ProcessInt(void)
{
	uint8_t nAsrResCount = 0;
	
	LD3320A_WriteReg(0x29, 0x00);//不允许FIFO中断和同步中断
	LD3320A_WriteReg(0x02, 0x00);//不允许FIFO_DATA中断和FIFO_EXT中断
	
	/* 语音识别有结果产生 && 以确定是一次语音识别流程正常结束 &&  查询到为闲状态 */
	if((LD3320A_ReadReg(0x2B) & 0x10) && LD3320A_ReadReg(0xBF) == 0x35 && LD3320A_ReadReg(0xB2) == 0x21)
	{
		nAsrResCount = LD3320A_ReadReg(0xBA); //中断辅助信息

		if(nAsrResCount > 0 && nAsrResCount < 4)
			nAsrStatus = LD3320A_ASR_FOUNDOK;
		else
			nAsrStatus = LD3320A_ASR_FOUNDZERO;
	}else
		nAsrStatus = LD3320A_ASR_FOUNDZERO;

	LD3320A_WriteReg(0x2B, 0x00); //清空中断请求编号
	LD3320A_WriteReg(0x1C, 0x00); //ADC不可用
}

/**
  * @brief  中断函数.
  * @param  None
  * @retval None
  */
void LD3320A_IRQ_GPIO_IRQHandler(void)
{
//	static int i=0;
	if(EXTI_GetITStatus(LD3320A_IRQ_GPIO_EXTI_Line) != RESET)
	{
		printf("进入中断\r\n");
//		for(i=0; i<2; i++)
//		{
//			syn6288_Say("可呼叫");
//		}
		ProcessInt();
		EXTI_ClearFlag(LD3320A_IRQ_GPIO_EXTI_Line);
		EXTI_ClearITPendingBit(LD3320A_IRQ_GPIO_EXTI_Line);//清除LINE上的中断标志位
	}		
}

/************************ (C) COPYRIGHT 2023 ERIC *********END OF FILE*********/
