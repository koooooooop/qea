#include "DMAWiFi.h"
#include "UartWiFi.h"
#define WFDMA_UART_RX_BUFFSIZE 128
#define WFDMA_UART_TX_BUFFSIZE 1024
U8 g_wfdma_uart_rxbuff[WFDMA_UART_RX_BUFFSIZE];
U8 g_wfdma_uart_txbuff[WFDMA_UART_TX_BUFFSIZE];
typedef struct{
	U8 m_uchSend;
	U8 m_uchEn;
	U16 m_uchRevPointer;
	U16 m_uchRevCount;
	
}STRWFDMAUartRegs;
STRWFDMAUartRegs g_wfdma_uartregs;
void fnWFDMA_UARTRxInit(void)
{

	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);/*DMA2��ʱ��ʹ��*/
	
	   
	///while(DMA_GetCmdStatus(DMA2_Stream2)!=DISABLE);/*�ȴ�DMA��������*/
	DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TCIF4);
	DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TEIF4);
	DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_DMEIF4);
	DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_HTIF4);
	DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_FEIF4);

	DMA_InitStructure.DMA_Channel = DMA_Channel_4;/*DMAͨ��0*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = (U32)&USART1->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr    = (U32)g_wfdma_uart_rxbuff;///g_adc_samplebuff;/*��ȡ����ַ*/
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;/*��������赽�ڴ�*/
	DMA_InitStructure.DMA_BufferSize = WFDMA_UART_RX_BUFFSIZE;/*���ݴ��������Ϊ3*/
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;/*��ַ������*/
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;/*��ַ������*/
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;/*���ݳ��Ȱ���*/
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;/*���ݳ��Ȱ���*/
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;/*�����ȼ�*/
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;/*ѭ��ģʽ*/
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;/*��ֹFIFO*/
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;/*FIFO��ֵ*/
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;/*���δ���*/
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;/*���δ���*/
	DMA_Init(DMA2_Stream2,&DMA_InitStructure);/**/
	DMA_Cmd(DMA2_Stream2,ENABLE);//����DMA����
	
}

void fnWFDMA_UARTTxInit(U16 datasize)
{
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);/*DMA2��ʱ��ʹ��*/

	//RCC->AHB1RSTR |= RCC_AHB1RSTR_DMA2RST;

	//RCC->AHB1RSTR &= ~RCC_AHB1RSTR_DMA2RST;
	while(DMA_GetCmdStatus(DMA2_Stream7)!=DISABLE);/*�ȴ�DMA��������*/
	
	DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TCIF7);
	DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TEIF7);
	DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_DMEIF7);
	DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_HTIF7);
	DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_FEIF7);
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;/*DMAͨ��0*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = (U32)&USART1->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr    = (U32)g_wfdma_uart_txbuff;/*��ȡ����ַ*/
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;/*��������赽�ڴ�*/
	DMA_InitStructure.DMA_BufferSize = datasize;/*���ݴ��������Ϊ3*/
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;/*��ַ������*/
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;/*��ַ������*/
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;/*���ݳ��Ȱ���*/
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;/*���ݳ��Ȱ���*/
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;/*�����ȼ�*/
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;/*ѭ��ģʽ*/
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;/*��ֹFIFO*/
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;/*FIFO��ֵ*/
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;/*���δ���*/
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;/*���δ���*/
	DMA_Init(DMA2_Stream7,&DMA_InitStructure);/**/
	DMA_Cmd(DMA2_Stream7, ENABLE);//����DMA����
	
}

U8 fnWFDMA_UART_IsTXIdle(void){
	U8 res = FALSE;
	if(g_wfdma_uartregs.m_uchEn == FALSE){
		 res = TRUE;
	}
	
	return(res);
}
U8 fnWFDMA_SendBaoLuoEn(void){
	return(wfua_en_flag);
}
U8 fnWFDMA_UARTTxStart(U8 *data, U16 size)
{
	U16 i;
	U8 nv;
	U8 crc = 0;
	U8 res = FALSE;
	if(g_wfdma_uartregs.m_uchEn == FALSE){
		for(i = 0; i < size; i++){
			
			g_wfdma_uart_txbuff[i] = data[i];
			crc += data[i];
		}
		g_wfdma_uart_txbuff[size] = crc;
		fnWFDMA_UARTTxInit(size + 1);
		g_wfdma_uartregs.m_uchEn = TRUE;
		g_wfdma_uartregs.m_uchSend = TRUE;
		res = TRUE;
		
	}else{
		
	}
	return(res);
	
	
}

void fnWFDMA_UARTInit(U8 *dmatxbuff)
{
	memset(&g_wfdma_uartregs, 0, sizeof(g_wfdma_uartregs));
	g_wfdma_uartregs.m_uchRevCount = WFDMA_UART_RX_BUFFSIZE;
	g_wfdma_uartregs.m_uchRevPointer = 0;
	///g_dma_uart_txbuff = dmatxbuff;
	fnWFDMA_UARTRxInit();
}

void fnWFDMA_UartRead(U8 *buf, U16 *pointer, U16 size, U16 *count){
	#if 1
	while(g_wfdma_uartregs.m_uchRevCount != DMA2_Stream2->NDTR ){
	
			if(DMA2_Stream2->NDTR == 0){	 
					break;
		   }
			else{
				if(g_wfdma_uartregs.m_uchRevCount == 1){
					g_wfdma_uartregs.m_uchRevCount = WFDMA_UART_RX_BUFFSIZE;	
				}else{
					g_wfdma_uartregs.m_uchRevCount --;
				}
				if(g_wfdma_uartregs.m_uchRevPointer >= WFDMA_UART_RX_BUFFSIZE){
					g_wfdma_uartregs.m_uchRevPointer = 0;
				}
				if(*pointer >= size){
					*pointer = 0;
				}
				if(*count < size){
					buf[*pointer] = g_wfdma_uart_rxbuff[g_wfdma_uartregs.m_uchRevPointer];
				//	g_dma_uartbuff[g_dma_uartregs.m_uchRevPointer] = 0;
					*pointer += 1;

					if(*pointer >= size){
						*pointer = 0;
					}
					*count += 1;
				}
				g_wfdma_uartregs.m_uchRevPointer ++;
				if(g_wfdma_uartregs.m_uchRevPointer >= WFDMA_UART_RX_BUFFSIZE){
					g_wfdma_uartregs.m_uchRevPointer = 0;
				} 
			}
	}
	#endif
}
void fnWFDMA_RealTime(void){
	
	if(g_wfdma_uartregs.m_uchEn){
		if(DMA2_Stream7->NDTR == 0){
		    //  DMA_Cmd(DMA2_Stream7,DISABLE);
			g_wfdma_uartregs.m_uchEn = FALSE;
		}
	}
}

void fnWFDMA_GetSendBuff(U8 **sendbuff, U16 *len){
	*sendbuff = g_wfdma_uart_txbuff;
	*len = WFDMA_UART_TX_BUFFSIZE;
}
