#ifndef _H_DMA_
#define _H_DMA_
#include "define.h"

extern void fnDMA_UARTInit(U8 *dmatxbuff);

extern void fnDMA_UartRead(U8 *buf, U16 *pointer, U16 size, U16 *count);
extern U8 fnDMA_UART_IsTXIdle(void);
extern U8 fnDMA_UARTTxStart(U16 size);
extern void fnDMA_RealTime(void);
extern void fnDMA_GetSendBuff(U8 **sendbuff, U16 *len);
#endif
