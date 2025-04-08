#include "Uart.h"
#include "IO.h"
#include "DMA.h"
#include "Timer.h"
STRUARecType g_ua_stRecRegs;
STRUASendType g_ua_stSendRegs;
#define UART_COMM USART2
void fnUA_UartInit(U32 baudrate){
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);							//使能时钟	
	USART_InitStructure.USART_BaudRate = baudrate;									//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//字长为8位数据格式.
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_InitStructure.USART_Mode =  USART_Mode_Tx|USART_Mode_Rx;					//收发模式
	USART_Init(UART_COMM, &USART_InitStructure);

	USART_DMACmd(UART_COMM, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);
	
	USART_ITConfig(UART_COMM,USART_IT_TC,  DISABLE);	//关闭发送中断 
	USART_ITConfig(UART_COMM,USART_IT_RXNE,DISABLE);	//开启接收中断！
	USART_ITConfig(UART_COMM, USART_IT_IDLE,DISABLE);	//开启空闲中断 当总线是一个字节周期内没有收到数据时触发
	
	USART_Cmd(UART_COMM, ENABLE);                    //使能串口   

}
void fnUA_IOInit(void)
{
	//使能外设时钟
	
	

	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(UART1_TX_GPIO_CLK | UART1_RX_GPIO_CLK, ENABLE);// 打开串口GPIO的时钟
	
	
	GPIO_InitStructure.GPIO_Pin = UART1_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(UART1_TX_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(UART1_TX_GPIO_PORT, UART1_TX_GPIO_SOURCE, GPIO_AF_USART1);
	
	GPIO_InitStructure.GPIO_Pin = UART1_RX_GPIO_PIN;
	GPIO_Init(UART1_RX_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(UART1_RX_GPIO_PORT, UART1_RX_GPIO_SOURCE, GPIO_AF_USART1);



}


#define UA_RX_CHARTIME  8
void fnUA_RecMsg(void)
{
	U16 outp;
	U8 temp;
	static U8 crc = 0;
	U16 msgid=0;
	if(g_tm_stTimerFlag.Bits.bTimer64ms)
	{
		if(g_ua_stRecRegs.m_uchCharTime)
		{
			g_ua_stRecRegs.m_uchCharTime--;
			if(0 == g_ua_stRecRegs.m_uchCharTime)
			{
				g_ua_stRecRegs.m_uchPointer = 0;
			}
		}
	}
	if(FALSE == g_ua_stSendRegs.m_uchIsSending){
		if(FALSE == g_ua_stRecRegs.m_uchReceived)
		{
			while(g_ua_stRecRegs.count > 0)
			{
					g_ua_stRecRegs.count --;
				outp = g_ua_stRecRegs.m_uchOutP & (UA_RX_RINGBUFF_SIZE - 1);
				g_ua_stRecRegs.m_uchOutP++;
				temp = g_ua_stRecRegs.m_uchRingBuff[outp];
				if(0 == g_ua_stRecRegs.m_uchPointer)
				{
					if(0x55 == temp)
					{
						crc = 0;
						g_ua_stRecRegs.m_uchPointer = 1;
					}
				}
				else if(1 == g_ua_stRecRegs.m_uchPointer)
				{
					if(0xFF == temp)
					{
						g_ua_stRecRegs.m_uchPointer = 2;
						
					}
					else if(0x55 != temp)
					{
						g_ua_stRecRegs.m_uchPointer = 0;
					}
				}
				else if(g_ua_stRecRegs.m_uchPointer <= 3)
				{
					g_ua_stRecRegs.m_uchBuff[g_ua_stRecRegs.m_uchPointer - 2] = temp;
					g_ua_stRecRegs.m_uchLen = 0;
					g_ua_stRecRegs.m_uchPointer ++;
				}
				else if(g_ua_stRecRegs.m_uchPointer <= 5)
				{
					g_ua_stRecRegs.m_uchLen = g_ua_stRecRegs.m_uchLen << 8;
					g_ua_stRecRegs.m_uchLen |= (U16)temp;
					g_ua_stRecRegs.m_uchBuff[g_ua_stRecRegs.m_uchPointer - 2] = temp;
					g_ua_stRecRegs.m_uchPointer ++;
					if(g_ua_stRecRegs.m_uchLen >= (UA_RX_BUFF_SIZE - 5)) //msgid, length;
					{
						g_ua_stRecRegs.m_uchPointer = 0;
					}
				}
				else
				{
					g_ua_stRecRegs.m_uchBuff[g_ua_stRecRegs.m_uchPointer - 2] = temp;
					g_ua_stRecRegs.m_uchPointer++;
					if(g_ua_stRecRegs.m_uchPointer >= (g_ua_stRecRegs.m_uchLen + 7))
					{
						g_ua_stRecRegs.m_uchPointer = 0;
						if(crc == temp)
						{
							g_ua_stRecRegs.m_uchRecvTimer = 6;
							g_ua_stRecRegs.m_uchLen += 5;
							msgid = (U16)g_ua_stRecRegs.m_uchBuff[0] << 8;
							msgid |= (U16)g_ua_stRecRegs.m_uchBuff[1];
							if(g_ua_stRecRegs.m_uiRecMsgID != msgid){
								g_ua_stRecRegs.m_uchReceived = TRUE;
								g_ua_stRecRegs.m_uiRecMsgID = msgid;
							}
							
						}
						break; 
						
					}
				} 
				crc += temp;
			}
		}
	}
}
void fnUA_Monitor(void)
{

	if(g_tm_stTimerFlag.Bits.bTimer64ms)
	{
		if(g_ua_stSendRegs.m_uchSendTimer)
		{
			g_ua_stSendRegs.m_uchSendTimer --;
			if(0 == g_ua_stSendRegs.m_uchSendTimer)
			{
				fnUA_Init();
			}
		}
	}
}
void fnUA_Clear(void){
	g_ua_stRecRegs.m_uchReceived = FALSE;
}
void fnUA_RealTime(void)
{
#if(TRS_TRANSMIT_BYWIFI)
	return;
#else
	if(g_ua_stSendRegs.m_uchIsSending){
		if(fnDMA_UART_IsTXIdle() == TRUE){
			g_ua_stSendRegs.m_uchIsSending = FALSE;
		}
	}
	fnDMA_RealTime();
	fnDMA_UartRead((U8 *)g_ua_stRecRegs.m_uchRingBuff, &g_ua_stRecRegs.m_uchInP, UA_RX_RINGBUFF_SIZE, &g_ua_stRecRegs.count);
			
	fnUA_Monitor();
	fnUA_RecMsg();
	if(g_tm_stTimerFlag.Bits.bTimer100ms){
		if(g_ua_stRecRegs.m_uchRecvTimer)
		{
			g_ua_stRecRegs.m_uchRecvTimer--;
			if(0 == g_ua_stRecRegs.m_uchRecvTimer)
			{
				g_ua_stRecRegs.m_uchReceived = FALSE;
			}
		}
	}
#endif
}
U8 fnUA_SendReq(U16 msgid, U16 len)
{
	
	U8 res = FALSE;
	g_ua_stSendRegs.m_uchBuff[0] = 0x55;
	g_ua_stSendRegs.m_uchBuff[1] = 0xFF;
	g_ua_stSendRegs.m_uchBuff[2] = (U8)(msgid >> 8);
	g_ua_stSendRegs.m_uchBuff[3] = (U8)msgid;
	
	g_ua_stSendRegs.m_uchBuff[4] = (U8)(len >> 8);
	g_ua_stSendRegs.m_uchBuff[5] = (U8)len;
	g_ua_stSendRegs.m_uchCount = len + 6;
	g_ua_stSendRegs.m_uchSendTimer = (200);
	g_ua_stSendRegs.m_uchIndex = 0;
	g_ua_stSendRegs.m_uchIsSending = TRUE;

	///res = fnDMA_UARTTxStart((U8 *)g_ua_stSendRegs.m_uchBuff, g_ua_stSendRegs.m_uchCount);

	
	return(res);
	
}

U8 fnUA_IsSend(void)
{
	U8 res = TRUE;
	if(FALSE == g_ua_stSendRegs.m_uchIsSending)
	{
		res = FALSE;
	}
	
	return res;
}
void fnUA_RegInit(void)
{
	memset(&g_ua_stRecRegs, 0, sizeof(g_ua_stRecRegs));
	memset(&g_ua_stSendRegs, 0, sizeof(g_ua_stSendRegs));
	g_ua_stRecRegs.m_uiRecMsgID = 0xffff;
}


void fnUA_Init(void)
{
#if(TRS_TRANSMIT_BYWIFI)
	return;
#else
	fnUA_RegInit();
	fnUA_IOInit();
	fnUA_UartInit(230400);
	fnDMA_UARTInit(0);
#endif
}


