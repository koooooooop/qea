#include "Adc.h"
#include "define.h"
#include "IO.h" 
#include "Timer.h" 
#include "math.h"
#include "filter.h"
#include "baoluo.h"
#include "fft.h"
#include "transmit.h"
#include "data.h"
#define BLDET_STATE_IDLE 0
#define BLDET_STATE_PROCESS 1
#define BLDET_STATE_TRANSMIT 2
#define BLDET_STATE_FINISH 3

bldet_regs_typedef g_bldet_regs;

BLDET_MixMemType bldet_bl_calmem;
void fnBL_Init(void){
	memset(&g_bldet_regs, 0, sizeof(g_bldet_regs));
	fnTRS_Init();
}
#define BL_SENDPOINTER_NUM 4
#define BL_SAMPLE_RATE 2560
void fnBL_Task(void){
	U16 i;
	float tc;
	int sumad;
	unsigned short *flashaddr;
	return;
	if(g_bldet_regs.state == BLDET_STATE_IDLE){
		if(g_bldet_regs.substate == 0){
			g_bldet_regs.testid ++;
			fnADC_Start(); 
			g_bldet_regs.substate = 1;
		}else {

	
	if(fnADC_IsFinish()){
				sumad = 0;
				for(i = 0; i < 4000; i++){
					flashaddr = (unsigned short *)(0x60000004 + 0x10 * i);
					sumad += *flashaddr;
				}
 				g_bldet_regs.chpointer = 0;
				g_bldet_regs.dtpoinnter = 0;
				fnFL_FilterAll(g_adc_samplebuff, bldet_bl_calmem.bldet_tempbuff,  ADC_CHANNEL_NUM);
				g_bldet_regs.state = BLDET_STATE_PROCESS;
			} 
		}
	}
	else if(g_bldet_regs.state == BLDET_STATE_PROCESS){
		if(g_bldet_regs.chpointer  >= ADC_CHANNEL_NUM){
			g_bldet_regs.state = BLDET_STATE_IDLE;
		}else{
			fnFL_GetSample(bldet_bl_calmem.bldet_bl_buff, g_adc_samplebuff, ADC_CHANNEL_NUM, g_bldet_regs.chpointer);
			
			fnFL_CalACC(bldet_bl_calmem.bldet_bl_buff,  SAMPLE_DATA_NUM, BL_SAMPLE_RATE, &g_bldet_regs.blinfo[g_bldet_regs.chpointer]);
			tc = fnFL_CalTC(bldet_bl_calmem.bldet_bl_buff, BL_SAMPLE_RATE, SAMPLE_DATA_NUM);
			fnFL_CalABS(bldet_bl_calmem.bldet_bl_buff);
			fnFL_CalBLEx(bldet_bl_calmem.bldet_bl_buff, BL_SAMPLE_RATE, SAMPLE_DATA_NUM,  tc);

			fnFFT_FFT(bldet_bl_calmem.bldet_bl_buff, SAMPLE_DATA_NUM);

			fnFFT_CalMag(g_fft_stCalBuff, SAMPLE_DATA_NUM);
			g_bldet_regs.state = BLDET_STATE_TRANSMIT;
			g_bldet_regs.substate = 0;
			g_bldet_regs.sendpointer = 0;
		}
	}else if(g_bldet_regs.state == BLDET_STATE_TRANSMIT){
			if(0 == g_bldet_regs.substate){
		   	if(g_bldet_regs.sendpointer  == 3){
					if(fnTRS_ReqRawData((void *)&g_bldet_regs.blinfo[g_bldet_regs.chpointer], TRS_DATATYPE_AVS, g_bldet_regs.chpointer, ADC_CHANNEL_NUM, 17 * 4)){
					g_bldet_regs.substate = 1;
				}
				
			}else if(g_bldet_regs.sendpointer  == 0){
				if(fnTRS_ReqRawData((void *)g_adc_samplebuff, TRS_DATATYPE_RAWDATA, g_bldet_regs.chpointer, ADC_CHANNEL_NUM, 2 * SAMPLE_DATA_NUM)){
					g_bldet_regs.substate = 1;
				}
			}
			else if(g_bldet_regs.sendpointer  == 1){
				if(fnTRS_ReqRawData((void *)bldet_bl_calmem.bldet_bl_buff,  TRS_DATATYPE_BAOLUO, g_bldet_regs.chpointer, ADC_CHANNEL_NUM, 2 * SAMPLE_DATA_NUM)){
					g_bldet_regs.substate = 1;
				}
			}else if(g_bldet_regs.sendpointer  == 2){
				if(fnTRS_ReqRawData((void *)g_fft_stCalBuff,  TRS_DATATYPE_BAOLUOFFT, g_bldet_regs.chpointer, ADC_CHANNEL_NUM, 2 * SAMPLE_DATA_NUM)){
					g_bldet_regs.substate = 1;
				}
			}else{
				g_bldet_regs.state = BLDET_STATE_FINISH;
			} 
	       	}
		else if(1 == g_bldet_regs.substate){
			if(fnTRS_IsFinish()){
				g_bldet_regs.sendpointer ++;
				if(g_bldet_regs.sendpointer >= BL_SENDPOINTER_NUM){
					g_bldet_regs.state = BLDET_STATE_FINISH;
				}else{
					g_bldet_regs.substate  = 0;
				}
			}
		}
	}
	else if(g_bldet_regs.state == BLDET_STATE_FINISH){
		g_bldet_regs.chpointer ++;
		if(g_bldet_regs.chpointer  >= 1){//ADC_CHANNEL_NUM){
			g_bldet_regs.substate = 0;  
			g_bldet_regs.state = BLDET_STATE_IDLE;
		}else{
			g_bldet_regs.state = BLDET_STATE_PROCESS;
		}
	}
	fnTRS_RealTime();
}







