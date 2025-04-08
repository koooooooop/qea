/** Copyright (C) ZZY Electrical Equipment Co,ltd. All Rights Reserved. **/
/****************************************************************************
* File:     Uart.h                                                        *
* Desc:     串口通讯相关定义                                                *
*                                                                           *
* Author:   ZZY                                                     *
* Date:     10-06-24                                                        *
* Notes:                                                                    *
*                                                                           *
*****************************************************************************/


#ifndef _WFUART_H_
#define _WFUART_H_

/*-------------------------------- Includes ----------------------------------*/
#include  "define.h"
#define WFUSART_Rx_LEN  		150  //接收缓存区大小

#define WFUART_TX_GPIO_PORT      GPIOA
#define WFUART_TX_GPIO_PIN       GPIO_Pin_9
#define WFUART_TX_GPIO_CLK       RCC_AHB1Periph_GPIOA
#define WFUART_TX_GPIO_SOURCE    GPIO_PinSource9

#define WFUART_RX_GPIO_PORT      GPIOA
#define WFUART_RX_GPIO_PIN       GPIO_Pin_10
#define WFUART_RX_GPIO_CLK       RCC_AHB1Periph_GPIOA
#define WFUART_RX_GPIO_SOURCE    GPIO_PinSource10


#define WFUA_RX_RINGBUFF_SIZE 1024

#define WFUA_RX_BUFF_SIZE 800
#define WFUA_TX_BUFF_SIZE 1300
#define WFUA_BS_CHARTIME 20

typedef struct
{
	U16 m_uchInP;
	U16 m_uchOutP;
	U16 count;
	char m_uchBuff[WFUA_RX_BUFF_SIZE];
	char m_uchRingBuff[WFUA_RX_RINGBUFF_SIZE];
	U8 m_uchCharTime;
	U8 m_uchReceived;
	U16 m_uchLen;
	U16 m_uchPointer;
	U8 m_uchRecvTimer;
	U16 m_uiRecMsgID;
	
}STRWFUARecType;

typedef struct
{
	U16 m_uchCount;
	U16 m_uchIndex;
	char m_uchBuff[WFUA_TX_BUFF_SIZE];
	U8 m_uchIsSending;
	U8 m_uchSendTimer;
}STRWFUASendType;
extern U8 wfua_en_flag;
extern U8 wfua_en_count;
extern STRWFUARecType g_wfua_stRecRegs;
extern STRWFUASendType g_wfua_stSendRegs;
extern void fnWFUA_Init(void);
extern void fnWFUA_RealTime(void);
extern U8 fnWFUA_SendReq(U16 msgid, U16 len);

extern U8 fnWFUA_IsSend(void);
extern void fnWFUA_Clear(void);
extern void fnWFUA_TimerInt(void);
extern U8 fnWFUA_SendReqEx(U16 msgid, U8 *buff, U16 len);
extern U8 fnWFDMA_SendBaoLuoEn(void);
#endif
