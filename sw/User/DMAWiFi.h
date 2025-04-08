#ifndef _H_WFDMA_
#define _H_WFDMA_
#include "define.h"

extern void fnWFDMA_UARTInit(U8 *dmatxbuff);

extern void fnWFDMA_UartRead(U8 *buf, U16 *pointer, U16 size, U16 *count);
extern U8 fnWFDMA_UART_IsTXIdle(void);
extern void fnWFDMA_RealTime(void);
extern void fnWFDMA_GetSendBuff(U8 **sendbuff, U16 *len);
extern U8 fnWFDMA_UARTTxStart(U8 *data, U16 size);
#endif
