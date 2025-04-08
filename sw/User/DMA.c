#include "DMA.h"

#define DMA_UART_RX_BUFFSIZE 128
#define DMA_UART_TX_BUFFSIZE 1024
U8 g_dma_uart_rxbuff[DMA_UART_RX_BUFFSIZE];
U8 g_dma_uart_txbuff[DMA_UART_TX_BUFFSIZE];
typedef struct{
	U8 m_uchSend;
	U8 m_uchEn;
	U16 m_uchRevPointer;
	U16 m_uchRevCount;
	
}STRDMAUartRegs;
STRDMAUartRegs g_dma_uartregs;
void fnDMA_UARTRxInit(void)
{

	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);/*DMA2的时钟使能*/
	
	   
	while(DMA_GetCmdStatus(DMA1_Stream5)!=DISABLE);/*等待DMA可以配置*/
	DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TCIF4);
	DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TEIF4);
	DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_DMEIF4);
	DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_HTIF4);
	DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_FEIF4);

	DMA_InitStructure.DMA_Channel = DMA_Channel_4;/*DMA通道0*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = (U32)&USART2->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr    = (U32)g_dma_uart_rxbuff;///g_adc_samplebuff;/*存取器地址*/
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;/*方向从外设到内存*/
	DMA_InitStructure.DMA_BufferSize = DMA_UART_RX_BUFFSIZE;/*数据传输的数量为3*/
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;/*地址不增加*/
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;/*地址不增加*/
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;/*数据长度半字*/
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;/*数据长度半字*/
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;/*高优先级*/
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;/*循环模式*/
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;/*禁止FIFO*/
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;/*FIFO的值*/
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;/*单次传输*/
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;/*单次传输*/
	DMA_Init(DMA1_Stream5,&DMA_InitStructure);/**/
	DMA_Cmd(DMA1_Stream5,ENABLE);//开启DMA传输
	
}

void fnDMA_UARTTxInit(U16 datasize)
{
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);/*DMA2的时钟使能*/

	RCC->AHB1RSTR |= RCC_AHB1RSTR_DMA1RST;

	RCC->AHB1RSTR &= ~RCC_AHB1RSTR_DMA1RST;
	while(DMA_GetCmdStatus(DMA1_Stream6)!=DISABLE);/*等待DMA可以配置*/
	
	DMA_ClearFlag(DMA1_Stream6, DMA_FLAG_TCIF7);
	DMA_ClearFlag(DMA1_Stream6, DMA_FLAG_TEIF7);
	DMA_ClearFlag(DMA1_Stream6, DMA_FLAG_DMEIF7);
	DMA_ClearFlag(DMA1_Stream6, DMA_FLAG_HTIF7);
	DMA_ClearFlag(DMA1_Stream6, DMA_FLAG_FEIF7);
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;/*DMA通道0*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = (U32)&USART2->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr    = (U32)g_dma_uart_txbuff;/*存取器地址*/
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;/*方向从外设到内存*/
	DMA_InitStructure.DMA_BufferSize = datasize;/*数据传输的数量为3*/
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;/*地址不增加*/
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;/*地址不增加*/
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;/*数据长度半字*/
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;/*数据长度半字*/
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;/*高优先级*/
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;/*循环模式*/
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;/*禁止FIFO*/
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;/*FIFO的值*/
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;/*单次传输*/
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;/*单次传输*/
	DMA_Init(DMA1_Stream6,&DMA_InitStructure);/**/
	DMA_Cmd(DMA1_Stream6, ENABLE);//开启DMA传输
	
}

U8 fnDMA_UART_IsTXIdle(void){
	U8 res = FALSE;
	if(g_dma_uartregs.m_uchEn == FALSE){
		res = TRUE;
	}
	return(res);
}
U8 fnDMA_UARTTxStart(U16 size)
{
	U8 res = FALSE;
	fnDMA_UARTTxInit(size);
	res = TRUE;
	g_dma_uartregs.m_uchEn = TRUE;
	g_dma_uartregs.m_uchSend = TRUE;
	return(res);
	
	
}

void fnDMA_UARTInit(U8 *dmatxbuff)
{
	memset(&g_dma_uartregs, 0, sizeof(g_dma_uartregs));
	g_dma_uartregs.m_uchRevCount = DMA_UART_RX_BUFFSIZE;
	g_dma_uartregs.m_uchRevPointer = 0;
	///g_dma_uart_txbuff = dmatxbuff;
	fnDMA_UARTRxInit();
}

void fnDMA_UartRead(U8 *buf, U16 *pointer, U16 size, U16 *count){
	#if 0
	while(g_dma_uartregs.m_uchRevCount != DMA2_Stream2->NDTR ){
		if(DMA2_Stream2->NDTR != 0){


			if(g_dma_uartregs.m_uchRevCount == 1){
				g_dma_uartregs.m_uchRevCount = DMA_UART_RX_BUFFSIZE;	
			}else{
				g_dma_uartregs.m_uchRevCount --;
			}
			if(g_dma_uartregs.m_uchRevPointer >= DMA_UART_RX_BUFFSIZE){
				g_dma_uartregs.m_uchRevPointer = 0;
			}
			if(*pointer >= size){
				*pointer = 0;
			}
			if(*count < size){
				buf[*pointer] = g_dma_uart_rxbuff[g_dma_uartregs.m_uchRevPointer];
			//	g_dma_uartbuff[g_dma_uartregs.m_uchRevPointer] = 0;
				*pointer += 1;

				if(*pointer >= size){
					*pointer = 0;
				}
				*count += 1;
			}
			g_dma_uartregs.m_uchRevPointer ++;
			if(g_dma_uartregs.m_uchRevPointer >= DMA_UART_RX_BUFFSIZE){
				g_dma_uartregs.m_uchRevPointer = 0;
			}
		}
	}
	#endif
}
void fnDMA_RealTime(void){
	
	if(g_dma_uartregs.m_uchEn){
		if(DMA1_Stream6->NDTR == 0){
		    //  DMA_Cmd(DMA2_Stream7,DISABLE);
			g_dma_uartregs.m_uchEn = FALSE;
		}
	}
}

void fnDMA_GetSendBuff(U8 **sendbuff, U16 *len){
	*sendbuff = g_dma_uart_txbuff;
	*len = DMA_UART_TX_BUFFSIZE - 16;
}
