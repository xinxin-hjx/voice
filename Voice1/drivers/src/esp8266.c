#include "esp8266.h"
#include "usart.h"
#include "delay.h"
#include <stdarg.h>

ESP8266_Fram_Record_Struct  Frame= { 0 };  //������һ������֡�ṹ��

void ESP8266_Init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(ESP8266_RST_Pin_Periph_Clock|ESP8266_CH_PD_Pin_Periph_Clock, ENABLE);

    GPIO_InitStructure.GPIO_Pin = ESP8266_RST_Pin;             
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     //�����������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
    GPIO_Init(ESP8266_RST_Pin_Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = ESP8266_CH_PD_Pin;               
    GPIO_Init(ESP8266_CH_PD_Pin_Port, &GPIO_InitStructure);

    uart2_Init(bound); 
	ESP8266_Rst();
}
//��ESP8266ģ�鷢��ATָ��
// cmd �����͵�ָ��
// ack1,ack2;�ڴ�����Ӧ��ΪNULL������Ӧ������Ϊ���߼���ϵ
// time �ȴ���Ӧʱ��
//����1���ͳɹ��� 0ʧ��
bool ESP8266_Send_AT_Cmd(char *cmd,char *ack1,char *ack2,u32 time)
{ 
    Frame .InfBit .FramLength = 0; //���½����µ����ݰ�
    ESP8266_USART("%s\r\n", cmd);
    if(ack1==0&&ack2==0)     //����Ҫ��������
    {
    return true;
    }
    delay_ms(time);   //��ʱ
		delay_ms(1000);
    Frame.Data_RX_BUF[Frame.InfBit.FramLength ] = '\0';
		
    printf("%s",Frame .Data_RX_BUF);
    if(ack1!=0&&ack2!=0)
    {
        return ( ( bool ) strstr ( Frame .Data_RX_BUF, ack1 ) || 
                         ( bool ) strstr ( Frame .Data_RX_BUF, ack2 ) );
    }
    else if( ack1 != 0 )  //strstr(s1,s2);���s2�Ƿ�Ϊs1��һ���֣��Ƿ��ظ�λ�ã����򷵻�false����ǿ��ת��Ϊbool������
        return ( ( bool ) strstr ( Frame .Data_RX_BUF, ack1 ) );

    else
        return ( ( bool ) strstr ( Frame .Data_RX_BUF, ack2 ) );

}


//��λ����
void ESP8266_Rst(void)
{
    ESP8266_RST_Pin_SetL;
    delay_ms(500); 
    ESP8266_RST_Pin_SetH;
}


//���ͻָ�����Ĭ������ָ�ģ��ָ��ɳ�������
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


//ѡ��ESP8266�Ĺ���ģʽ
// enumMode ģʽ����
//�ɹ�����true��ʧ�ܷ���false
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


//ESP8266�����ⲿ��WIFI
//pSSID WiFi�ʺ�
//pPassWord WiFi����
//���óɹ�����true ��֮false
bool ESP8266_JoinAP( char * pSSID, char * pPassWord)
{
    char cCmd [120];
	
    sprintf ( cCmd, "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassWord );
    return ESP8266_Send_AT_Cmd( cCmd, "OK", NULL, 5000 );
}

//ESP8266 ͸��ʹ��
//enumEnUnvarnishTx  �Ƿ�����ӣ�bool����
//���óɹ�����true����֮false
bool ESP8266_Enable_MultipleId (FunctionalState enumEnUnvarnishTx )
{
    char cStr [20];

    sprintf ( cStr, "AT+CIPMUX=%d", ( enumEnUnvarnishTx ? 1 : 0 ) );

    return ESP8266_Send_AT_Cmd ( cStr, "OK", 0, 500 );

}


//ESP8266 ���ӷ�����
//enumE  ��������
//ip ��������IP
//ComNum  �������˿�
//id�����Ӻţ�ȷ��ͨ�Ų���������
//���óɹ�����true����֮fasle
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


//͸��ʹ��
//���óɹ�����true�� ��֮false
bool ESP8266_UnvarnishSend ( void )
{
    if (!ESP8266_Send_AT_Cmd ( "AT+CIPMODE=1", "OK", 0, 500 ))
        return false;

    return 
        ESP8266_Send_AT_Cmd( "AT+CIPSEND", "OK", ">", 500 );

}


//ESP8266�����ַ���
//enumEnUnvarnishTx�Ƿ�ʹ��͸��ģʽ
//pStr�ַ���
//ulStrLength�ַ�������
//ucId ���Ӻ�
//���óɹ�����true�� ��֮false
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


//ESP8266�˳�͸��ģʽ
void ESP8266_ExitUnvarnishSend ( void )
{
    delay_ms(1000);
    ESP8266_USART( "+++" );
    delay_ms( 500 );    
}


//ESP8266 �������״̬
//����0����ȡ״̬ʧ��
//����2�����ip
//����3���������� 
//����4��ʧȥ���� 
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

    while ( * Data != 0 )     // �ж������Ƿ񵽴������
    {                                         
        if ( * Data == 0x5c )  //'\'
        {                                     
            switch ( *++Data )
            {
                case 'r':                                     //�س���
                USART_SendData(USARTx, 0x0d);
                Data ++;
                break;

                case 'n':                                     //���з�
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
                case 's':                                         //�ַ���
                s = va_arg(ap, const char *);
                for ( ; *s; s++) 
                {
                    USART_SendData(USARTx,*s);
                    while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
                }
                Data++;
                break;

                case 'd':           
                    //ʮ����
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

bool ESP8266_Close_Server(ENUM_ID_NO_TypeDef id)   //�ر�����
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
            //�����һλ������λ
            Frame.Data_RX_BUF[ Frame .InfBit .FramLength ++ ]  = ucCh;   
        }                      
    }

    if( USART_GetITStatus( USART1, USART_IT_IDLE ) == SET )                              //������߿���
    {
        Frame .InfBit .FramFinishFlag = 1;
        ucCh = USART_ReceiveData( USART1 );                                                              //�������������жϱ�־λ���ȶ�USART_SR,Ȼ���USART_DR��
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

    if( USART_GetITStatus( USART2, USART_IT_IDLE ) == SET )                                         //������߿���
    {
        Frame .InfBit .FramFinishFlag = 1;

        ucCh = USART_ReceiveData( USART2 );                                                              //�������������жϱ�־λ���ȶ�USART_SR,Ȼ���USART_DR��
				USART_SendData(USART1,ucCh);
 //       TcpClosedFlag = strstr ( Frame .Data_RX_BUF, "CLOSED\r\n" ) ? 1 : 0;

    }   

}



