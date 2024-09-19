#include "esp8266.h"
#include "usart.h"
#include "delay.h"
#include <stdarg.h>

ESP8266_Fram_Record_Struct  Frame= { 0 };  //定义了一个数据帧结构体

void ESP8266_Init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(ESP8266_RST_Pin_Periph_Clock|ESP8266_CH_PD_Pin_Periph_Clock, ENABLE);

    GPIO_InitStructure.GPIO_Pin = ESP8266_RST_Pin;             
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     //复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
    GPIO_Init(ESP8266_RST_Pin_Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = ESP8266_CH_PD_Pin;               
    GPIO_Init(ESP8266_CH_PD_Pin_Port, &GPIO_InitStructure);

    uart2_Init(bound); 
	ESP8266_Rst();
}
//对ESP8266模块发送AT指令
// cmd 待发送的指令
// ack1,ack2;期待的响应，为NULL表不需响应，两者为或逻辑关系
// time 等待响应时间
//返回1发送成功， 0失败
bool ESP8266_Send_AT_Cmd(char *cmd,char *ack1,char *ack2,u32 time)
{ 
    Frame .InfBit .FramLength = 0; //重新接收新的数据包
    ESP8266_USART("%s\r\n", cmd);
    if(ack1==0&&ack2==0)     //不需要接收数据
    {
    return true;
    }
    delay_ms(time);   //延时
		delay_ms(1000);
    Frame.Data_RX_BUF[Frame.InfBit.FramLength ] = '\0';
		
    printf("%s",Frame .Data_RX_BUF);
    if(ack1!=0&&ack2!=0)
    {
        return ( ( bool ) strstr ( Frame .Data_RX_BUF, ack1 ) || 
                         ( bool ) strstr ( Frame .Data_RX_BUF, ack2 ) );
    }
    else if( ack1 != 0 )  //strstr(s1,s2);检测s2是否为s1的一部分，是返回该位置，否则返回false，它强制转换为bool类型了
        return ( ( bool ) strstr ( Frame .Data_RX_BUF, ack1 ) );

    else
        return ( ( bool ) strstr ( Frame .Data_RX_BUF, ack2 ) );

}


//复位重启
void ESP8266_Rst(void)
{
    ESP8266_RST_Pin_SetL;
    delay_ms(500); 
    ESP8266_RST_Pin_SetH;
}


//发送恢复出厂默认设置指令将模块恢复成出厂设置
void ESP8266_AT_Test(void)
{
    char count=0;
    delay_ms(1000); 
    while(count < 10)
    {
        if(ESP8266_Send_AT_Cmd("AT+RESTORE","OK",NULL,500)) 
        {
            printf("OK\r\n");
            return;
        }
        ++ count;
    }
}


//选择ESP8266的工作模式
// enumMode 模式类型
//成功返回true，失败返回false
bool ESP8266_Net_Mode_Choose(ENUM_Net_ModeTypeDef enumMode)
{
    switch ( enumMode )
    {
        case STA:
            return ESP8266_Send_AT_Cmd ( "AT+CWMODE=1", "OK", "no change", 2500 ); 

        case AP:
            return ESP8266_Send_AT_Cmd ( "AT+CWMODE=2", "OK", "no change", 2500 ); 

        case STA_AP:
            return ESP8266_Send_AT_Cmd ( "AT+CWMODE=3", "OK", "no change", 2500 ); 

        default:
          return false;
    }       
}


//ESP8266连接外部的WIFI
//pSSID WiFi帐号
//pPassWord WiFi密码
//设置成功返回true 反之false
bool ESP8266_JoinAP( char * pSSID, char * pPassWord)
{
    char cCmd [120];
	
    sprintf ( cCmd, "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassWord );
    return ESP8266_Send_AT_Cmd( cCmd, "OK", NULL, 5000 );
}

//ESP8266 透传使能
//enumEnUnvarnishTx  是否多连接，bool类型
//设置成功返回true，反之false
bool ESP8266_Enable_MultipleId (FunctionalState enumEnUnvarnishTx )
{
    char cStr [20];

    sprintf ( cStr, "AT+CIPMUX=%d", ( enumEnUnvarnishTx ? 1 : 0 ) );

    return ESP8266_Send_AT_Cmd ( cStr, "OK", 0, 500 );

}


//ESP8266 连接服务器
//enumE  网络类型
//ip ，服务器IP
//ComNum  服务器端口
//id，连接号，确保通信不受外界干扰
//设置成功返回true，反之fasle
bool ESP8266_Link_Server(ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id)
{
    char cStr [100] = { 0 }, cCmd [120];

    switch (  enumE )
    {
        case enumTCP:
          sprintf ( cStr, "\"%s\",\"%s\",%s", "TCP", ip, ComNum );
          break;

        case enumUDP:
          sprintf ( cStr, "\"%s\",\"%s\",%s", "UDP", ip, ComNum );
          break;

        default:
            break;
    }

    if ( id < 5 )
        sprintf ( cCmd, "AT+CIPSTART=%d,%s", id, cStr);

    else
        sprintf ( cCmd, "AT+CIPSTART=%s", cStr );

    return ESP8266_Send_AT_Cmd ( cCmd, "OK", "ALREAY CONNECT", 4000 );

}


//透传使能
//设置成功返回true， 反之false
bool ESP8266_UnvarnishSend ( void )
{
    if (!ESP8266_Send_AT_Cmd ( "AT+CIPMODE=1", "OK", 0, 500 ))
        return false;

    return 
        ESP8266_Send_AT_Cmd( "AT+CIPSEND", "OK", ">", 500 );

}


//ESP8266发送字符串
//enumEnUnvarnishTx是否使能透传模式
//pStr字符串
//ulStrLength字符串长度
//ucId 连接号
//设置成功返回true， 反之false
bool ESP8266_SendString(FunctionalState enumEnUnvarnishTx, char * pStr, u32 ulStrLength, ENUM_ID_NO_TypeDef ucId )
{
    char cStr [20];
    bool bRet = false;


    if ( enumEnUnvarnishTx )
    {
        ESP8266_USART ( "%s", pStr );

        bRet = true;

    }

    else
    {
        if ( ucId < 5 )
            sprintf ( cStr, "AT+CIPSEND=%d,%d", ucId, ulStrLength + 2 );

        else
            sprintf ( cStr, "AT+CIPSEND=%d", ulStrLength + 2 );

        ESP8266_Send_AT_Cmd ( cStr, "> ", 0, 1000 );

        bRet = ESP8266_Send_AT_Cmd ( pStr, "SEND OK", 0, 1000 );
  }

    return bRet;

}


//ESP8266退出透传模式
void ESP8266_ExitUnvarnishSend ( void )
{
    delay_ms(1000);
    ESP8266_USART( "+++" );
    delay_ms( 500 );    
}


//ESP8266 检测连接状态
//返回0：获取状态失败
//返回2：获得ip
//返回3：建立连接 
//返回4：失去连接 
u8 ESP8266_Get_LinkStatus ( void )
{
    if (ESP8266_Send_AT_Cmd( "AT+CIPSTATUS", "OK", 0, 500 ) )
    {
        if ( strstr ( Frame .Data_RX_BUF, "STATUS:2\r\n" ) )
            return 2;

        else if ( strstr ( Frame .Data_RX_BUF, "STATUS:3\r\n" ) )
            return 3;

        else if ( strstr ( Frame .Data_RX_BUF, "STATUS:4\r\n" ) )
            return 4;       

    }

    return 0;
}

static char *itoa( int value, char *string, int radix )
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;

    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */


void USART_printf ( USART_TypeDef * USARTx, char * Data, ... )
{
    const char *s;
    int d;   
    char buf[16];


    va_list ap;
    va_start(ap, Data);

    while ( * Data != 0 )     // 判断数据是否到达结束符
    {                                         
        if ( * Data == 0x5c )  //'\'
        {                                     
            switch ( *++Data )
            {
                case 'r':                                     //回车符
                USART_SendData(USARTx, 0x0d);
                Data ++;
                break;

                case 'n':                                     //换行符
                USART_SendData(USARTx, 0x0a);   
                Data ++;
                break;

                default:
                Data ++;
                break;
            }            
        }

        else if ( * Data == '%')
        {                                     
            switch ( *++Data )
            {               
                case 's':                                         //字符串
                s = va_arg(ap, const char *);
                for ( ; *s; s++) 
                {
                    USART_SendData(USARTx,*s);
                    while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
                }
                Data++;
                break;

                case 'd':           
                    //十进制
                d = va_arg(ap, int);
                itoa(d, buf, 10);
                for (s = buf; *s; s++) 
                {
                    USART_SendData(USARTx,*s);
                    while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
                }
                     Data++;
                     break;
                default:
                     Data++;
                     break;
            }        
        }
        else USART_SendData(USARTx, *Data++);
        while ( USART_GetFlagStatus ( USARTx, USART_FLAG_TXE ) == RESET );

    }
}

bool ESP8266_Close_Server(ENUM_ID_NO_TypeDef id)   //关闭连接
{
	char cCmd[50];
	if(id < 5)
		sprintf(cCmd,"AT+CIPCLOSE=%d",id);
	else
		sprintf(cCmd,"AT+CLOSE");
	
	return ESP8266_Send_AT_Cmd(cCmd,"OK",0,1500);
}

void USART1_IRQHandler()
{
	u8 ucCh;
    if(USART_GetITStatus( USART1, USART_IT_RXNE ) != RESET )
    {
        ucCh  = USART_ReceiveData( USART1 );

        if(Frame.InfBit .FramLength < ( RX_BUF_MAX_LEN - 1 ) ) 
        {
            //留最后一位做结束位
            Frame.Data_RX_BUF[ Frame .InfBit .FramLength ++ ]  = ucCh;   
        }                      
    }

    if( USART_GetITStatus( USART1, USART_IT_IDLE ) == SET )                              //如果总线空闲
    {
        Frame .InfBit .FramFinishFlag = 1;
        ucCh = USART_ReceiveData( USART1 );                                                              //由软件序列清除中断标志位（先读USART_SR,然后读USART_DR）
     //   TcpClosedFlag = strstr ( Frame .Data_RX_BUF, "CLOSED\r\n" ) ? 1 : 0;

    }   
}


void USART2_IRQHandler(void)
{   
    u8 ucCh;

    if(USART_GetITStatus( USART2, USART_IT_RXNE ) != RESET )
    {
        ucCh  = USART_ReceiveData( USART2 );
			  USART_SendData(USART1,ucCh);
        if(Frame .InfBit .FramLength < ( RX_BUF_MAX_LEN - 1 ) ) 
        {

            Frame .Data_RX_BUF[ Frame .InfBit .FramLength ++ ]  = ucCh;   
        }                      
    }

    if( USART_GetITStatus( USART2, USART_IT_IDLE ) == SET )                                         //如果总线空闲
    {
        Frame .InfBit .FramFinishFlag = 1;

        ucCh = USART_ReceiveData( USART2 );                                                              //由软件序列清除中断标志位（先读USART_SR,然后读USART_DR）
				USART_SendData(USART1,ucCh);
 //       TcpClosedFlag = strstr ( Frame .Data_RX_BUF, "CLOSED\r\n" ) ? 1 : 0;

    }   

}



