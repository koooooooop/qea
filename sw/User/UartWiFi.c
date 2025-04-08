#include "UartWiFi.h"
#include "IO.h"
#include "DMAWiFi.h"
#include "Timer.h"
#include "transmit.h"
STRWFUARecType g_wfua_stRecRegs;
STRWFUASendType g_wfua_stSendRegs;
#define WFUART_COMM USART1
void fnWFUA_UartInit(U32 baudrate){
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);							//使能时钟	
	USART_InitStructure.USART_BaudRate = baudrate;									//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//字长为8位数据格式.
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_InitStructure.USART_Mode =  USART_Mode_Tx|USART_Mode_Rx;					//收发模式
	USART_Init(WFUART_COMM, &USART_InitStructure);

	USART_DMACmd(WFUART_COMM, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);
	
	USART_ITConfig(WFUART_COMM,USART_IT_TC,  DISABLE);	//关闭发送中断 
	USART_ITConfig(WFUART_COMM,USART_IT_RXNE,DISABLE);	//开启接收中断！
	USART_ITConfig(WFUART_COMM, USART_IT_IDLE,DISABLE);	//开启空闲中断 当总线是一个字节周期内没有收到数据时触发
	
	USART_Cmd(WFUART_COMM, ENABLE);                    //使能串口   

}
void fnWFUA_IOInit(void)
{
	//使能外设时钟
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(WFUART_TX_GPIO_CLK | WFUART_RX_GPIO_CLK, ENABLE);// 打开串口GPIO的时钟
	
	
	GPIO_InitStructure.GPIO_Pin = WFUART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(WFUART_TX_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(WFUART_TX_GPIO_PORT, WFUART_TX_GPIO_SOURCE, GPIO_AF_USART1);
	
	GPIO_InitStructure.GPIO_Pin = WFUART_RX_GPIO_PIN;
	GPIO_Init(WFUART_RX_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(WFUART_RX_GPIO_PORT, WFUART_RX_GPIO_SOURCE, GPIO_AF_USART1);



}


#define WFUA_RX_CHARTIME  8
void fnWFUA_RecMsg(void)
{
	U16 outp;
	U8 temp;
	static U8 crc = 0;
	U16 msgid=0;
	if(g_tm_stTimerFlag.Bits.bTimer64ms)
	{
		if(g_wfua_stRecRegs.m_uchCharTime)
		{
			g_wfua_stRecRegs.m_uchCharTime--;
			if(0 == g_wfua_stRecRegs.m_uchCharTime)
			{
				g_wfua_stRecRegs.m_uchPointer = 0;
			}
		}
	}
		if(FALSE == g_wfua_stRecRegs.m_uchReceived)
		{
			while(g_wfua_stRecRegs.count > 0)
			{
					g_wfua_stRecRegs.count --;
				outp = g_wfua_stRecRegs.m_uchOutP & (WFUA_RX_RINGBUFF_SIZE - 1);
				g_wfua_stRecRegs.m_uchOutP++;
				temp = g_wfua_stRecRegs.m_uchRingBuff[outp];
				if(0 == g_wfua_stRecRegs.m_uchPointer)
				{
					if(0x55 == temp)
					{
						crc = 0;
						g_wfua_stRecRegs.m_uchPointer = 1;
					}
				}
				else if(1 == g_wfua_stRecRegs.m_uchPointer)
				{
					if(0xFF == temp)
					{
						g_wfua_stRecRegs.m_uchPointer = 2;
						
					}
					else if(0x55 != temp)
					{
						g_wfua_stRecRegs.m_uchPointer = 0;
					}
				}
				else if(g_wfua_stRecRegs.m_uchPointer <= 3)
				{
					g_wfua_stRecRegs.m_uchBuff[g_wfua_stRecRegs.m_uchPointer - 2] = temp;
					g_wfua_stRecRegs.m_uchLen = 0;
					g_wfua_stRecRegs.m_uchPointer ++;
				}
				else if(g_wfua_stRecRegs.m_uchPointer <= 5)
				{
					g_wfua_stRecRegs.m_uchLen = g_wfua_stRecRegs.m_uchLen << 8;
					g_wfua_stRecRegs.m_uchLen |= (U16)temp;
					g_wfua_stRecRegs.m_uchBuff[g_wfua_stRecRegs.m_uchPointer - 2] = temp;
					g_wfua_stRecRegs.m_uchPointer ++;
					if(g_wfua_stRecRegs.m_uchLen >= (WFUA_RX_BUFF_SIZE - 5)) //msgid, length;
					{
						g_wfua_stRecRegs.m_uchPointer = 0;
					}
				}
				else
				{
					g_wfua_stRecRegs.m_uchBuff[g_wfua_stRecRegs.m_uchPointer - 2] = temp;
					g_wfua_stRecRegs.m_uchPointer++;
					if(g_wfua_stRecRegs.m_uchPointer >= (g_wfua_stRecRegs.m_uchLen + 7))
					{
						g_wfua_stRecRegs.m_uchPointer = 0;
						if(crc == temp)
						{
							g_wfua_stRecRegs.m_uchRecvTimer = 6;
							g_wfua_stRecRegs.m_uchLen += 5;
							msgid = (U16)g_wfua_stRecRegs.m_uchBuff[0] << 8;
							msgid |= (U16)g_wfua_stRecRegs.m_uchBuff[1];
							g_wfua_stRecRegs.m_uchReceived = TRUE;
							g_wfua_stRecRegs.m_uiRecMsgID = msgid;

							
						}
						break; 
						
					}
				} 
				crc += temp;
			}
		}
}
void fnWFUA_Monitor(void)
{

	if(g_tm_stTimerFlag.Bits.bTimer64ms)
	{
		if(g_wfua_stSendRegs.m_uchSendTimer)
		{
			g_wfua_stSendRegs.m_uchSendTimer --;
			if(0 == g_wfua_stSendRegs.m_uchSendTimer)
			{
				///fnWFUA_Init();
			}
		}
	}
}
void fnWFUA_Clear(void){
	g_wfua_stRecRegs.m_uchReceived = FALSE;
}

#define WFUA_EN_GPIO_PORT      GPIOA
#define WFUA_EN_GPIO_PIN       GPIO_Pin_3
#define WFUA_EN_GPIO_CLK       RCC_AHB1Periph_GPIOA
#define WFUA_EN_GPIO_SOURCE    GPIO_PinSource3
U8 wfua_en_flag = 0;
U8 wfua_en_count = 0;
void fnWFUA_RealTime(void)
{

	U8 curflag = 0;
	if(g_tm_stTimerFlag.Bits.bTimer100ms){
		if(GET_IO_STA(WFUA_EN_GPIO_PORT, 3)){
			curflag = 1;
		}else{
			curflag = 0;
		}
		if(curflag != wfua_en_flag){
			wfua_en_count ++;
			if(wfua_en_count >= 5){
				wfua_en_flag = curflag;
				wfua_en_count = 0;
			}
		}else{
			wfua_en_count =0;
		}
	}
	if(g_wfua_stSendRegs.m_uchIsSending){
		if(fnWFDMA_UART_IsTXIdle() == TRUE){
			g_wfua_stSendRegs.m_uchIsSending = FALSE;
		}
	}
	fnWFDMA_RealTime();
	fnWFDMA_UartRead((U8 *)g_wfua_stRecRegs.m_uchRingBuff, &g_wfua_stRecRegs.m_uchInP, WFUA_RX_RINGBUFF_SIZE, &g_wfua_stRecRegs.count);
			
	fnWFUA_Monitor();
	fnWFUA_RecMsg();
	if(g_tm_stTimerFlag.Bits.bTimer100ms){
		if(g_wfua_stRecRegs.m_uchRecvTimer)
		{
			g_wfua_stRecRegs.m_uchRecvTimer--;
			if(0 == g_wfua_stRecRegs.m_uchRecvTimer)
			{
				g_wfua_stRecRegs.m_uchReceived = FALSE;
			}
		}
	}

}
U8 fnWFUA_SendReq(U16 msgid, U16 len)
{
	
	U8 res = FALSE;
	g_wfua_stSendRegs.m_uchBuff[0] = 0x55;
	g_wfua_stSendRegs.m_uchBuff[1] = 0xFF;
	g_wfua_stSendRegs.m_uchBuff[2] = (U8)(msgid >> 8);
	g_wfua_stSendRegs.m_uchBuff[3] = (U8)msgid;
	
	g_wfua_stSendRegs.m_uchBuff[4] = (U8)(len >> 8);
	g_wfua_stSendRegs.m_uchBuff[5] = (U8)len;
	g_wfua_stSendRegs.m_uchCount = len + 6;
	g_wfua_stSendRegs.m_uchSendTimer = (200);
	g_wfua_stSendRegs.m_uchIndex = 0;
	g_wfua_stSendRegs.m_uchIsSending = TRUE;

   res = fnWFDMA_UARTTxStart((U8 *)g_wfua_stSendRegs.m_uchBuff, g_wfua_stSendRegs.m_uchCount);

	
	return(res);
	
}

U8 fnWFUA_SendReqEx(U16 msgid, U8 *buff, U16 len)
{
	U16 i;
	U8 res = FALSE;
	g_wfua_stSendRegs.m_uchBuff[0] = 0x55;
	g_wfua_stSendRegs.m_uchBuff[1] = 0xFF;
	g_wfua_stSendRegs.m_uchBuff[2] = (U8)(msgid >> 8);
	g_wfua_stSendRegs.m_uchBuff[3] = (U8)msgid;
	
	g_wfua_stSendRegs.m_uchBuff[4] = (U8)(len >> 8);
	g_wfua_stSendRegs.m_uchBuff[5] = (U8)len;
	for(i = 0; i < len; i++){
			g_wfua_stSendRegs.m_uchBuff[6 + i] = buff[i];
	}
	g_wfua_stSendRegs.m_uchCount = len + 6;
	g_wfua_stSendRegs.m_uchSendTimer = (200);
	g_wfua_stSendRegs.m_uchIndex = 0;
	g_wfua_stSendRegs.m_uchIsSending = TRUE;

   res = fnWFDMA_UARTTxStart((U8 *)g_wfua_stSendRegs.m_uchBuff, g_wfua_stSendRegs.m_uchCount);

	
	return(res);
	
}

U8 fnWFUA_IsSend(void)
{
	U8 res = TRUE;
	if(FALSE == g_wfua_stSendRegs.m_uchIsSending)
	{
		res = FALSE;
	}
	
	return res;
}
void fnWFUA_RegInit(void)
{
	memset(&g_wfua_stRecRegs, 0, sizeof(g_wfua_stRecRegs));
	memset(&g_wfua_stSendRegs, 0, sizeof(g_wfua_stSendRegs));
	g_wfua_stRecRegs.m_uiRecMsgID = 0xffff;
}


void fnWFUA_Init(void)
{
#if(TRS_TRANSMIT_BYWIFI)
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(WFUA_EN_GPIO_CLK, ENABLE);// 打开串口GPIO的时钟
	
	
	GPIO_InitStructure.GPIO_Pin = WFUA_EN_GPIO_PIN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(WFUA_EN_GPIO_PORT, &GPIO_InitStructure);
	fnWFUA_RegInit();
	fnWFUA_IOInit();
	fnWFUA_UartInit(115200);
	fnWFDMA_UARTInit(0);
#endif
}


