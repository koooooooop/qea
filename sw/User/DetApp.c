#include "DetApp.h"
#include "IO.h"
#include "Timer.h"
#include "switch.h"
#include "adc.h"
#include "UartWiFi.h"
STRDetAppRegType g_detapp_stRegs;
void fnDetApp_Init(void);
void fnDetApp_RealTime(void);

void fnDetApp_Init(void){
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(DET_APP_INDICATOR_CLK, ENABLE);// 打开串口GPIO的时钟
	
	
	GPIO_InitStructure.GPIO_Pin = DET_APP_INDICATOR_PIN_DEF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DET_APP_INDICATOR_PORT, &GPIO_InitStructure);
	SET_IO_OUT_HS(DET_APP_INDICATOR_PORT, DET_APP_INDICATOR_PIN);
	memset(&g_detapp_stRegs, 0, sizeof(g_detapp_stRegs));
	g_detapp_stRegs.m_uchUartBuff = &g_wfua_stSendRegs.m_uchBuff[6];
}
#define DETAPP_AD_REPORTNUM 256
void fnDetApp_HandleADC(U16 advalue){
	U16 i;
	U16 pointer = 0;
	U16 data;
	static U16 msgid = 0;
	g_detapp_stRegs.m_usADCBuff[g_detapp_stRegs.m_usADCount] = advalue;
	g_detapp_stRegs.m_usADCount ++;
	if(g_detapp_stRegs.m_usADCount >= DETAPP_AD_REPORTNUM){
		g_detapp_stRegs.m_uchUartBuff[pointer] = WFUA_CMD_REPORTADC;
		pointer ++;
		for(i = 0; i < DETAPP_AD_REPORTNUM; i++){
			data = g_detapp_stRegs.m_usADCBuff[i];
			g_detapp_stRegs.m_uchUartBuff[pointer] = (U8)(data >> 8);
			pointer ++;
			g_detapp_stRegs.m_uchUartBuff[pointer] = (U8)(data & 0xff);
			pointer ++;
		}
		fnWFUA_SendReq(msgid, pointer);
		msgid++;
		g_detapp_stRegs.m_usADCount = 0;
	}
}
void fnDetApp_ADC(void){
	U16 data;
	while(g_adc_regs.m_usADCCnt != ADC_DMA->NDTR){
		if(ADC_DMA->NDTR != 0){
		 
			if(g_adc_regs.m_usADCCnt == 1){
				g_adc_regs.m_usADCCnt = ADC_DMA_BUFF_SIZE;	
			}else{
				g_adc_regs.m_usADCCnt --;
			}
			if(g_adc_regs.m_usADCPointer >= ADC_DMA_BUFF_SIZE){
				g_adc_regs.m_usADCPointer = 0;
			}

			data = g_adc_samplebuff[g_adc_regs.m_usADCPointer];

			fnDetApp_HandleADC(data);
			g_adc_regs.m_usADCPointer ++;
			if(g_adc_regs.m_usADCPointer >= ADC_DMA_BUFF_SIZE){
				g_adc_regs.m_usADCPointer = 0;
			}
		}
	}
}
void fnDetApp_RealTime(void){
	fnDetApp_ADC();
	if(g_tm_stTimerFlag.Bits.bTimer100ms){
		if(g_detapp_stRegs.m_usTimer > 0){
			g_detapp_stRegs.m_usTimer --;
		}
	}
	if(g_detapp_stRegs.m_uchState == DETAPP_STATE_IDLE){
		if(INPUT_GET_STATE(DET_APP_SW_OFFSET) 
			&& INPUT_GET_TRIGGER(DET_APP_SW_OFFSET)){
				g_detapp_stRegs.m_uchState = DETAPP_STATE_RUN;
				SET_IO_HIGH(DET_APP_INDICATOR_PORT, DET_APP_INDICATOR_PIN);
				g_detapp_stRegs.m_usTimer = 100;
		}
	}
	else if(g_detapp_stRegs.m_uchState == DETAPP_STATE_RUN){
		if(g_tm_stTimerFlag.Bits.bTimer100ms){
			SET_IO_TOGGLE(DET_APP_INDICATOR_PORT, DET_APP_INDICATOR_PIN);
		}
		if(0 == g_detapp_stRegs.m_usTimer){
			SET_IO_LOW(DET_APP_INDICATOR_PORT, DET_APP_INDICATOR_PIN);
			g_detapp_stRegs.m_uchState = DETAPP_STATE_IDLE;
		}
	}
}