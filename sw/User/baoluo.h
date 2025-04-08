#ifndef _H_BAOLUO_
#define _H_BAOLUO_
#include "define.h"
#include "adc.h"
#include "filter.h"
typedef struct{
	U8 state;
	U16 timer;
	U8 chpointer;
	U16 dtpoinnter;
	U8 substate;
	U8 sendpointer;
	U8 testid;
	FLBLInfoDef blinfo[ADC_CHANNEL_NUM]; 
}bldet_regs_typedef;
extern bldet_regs_typedef g_bldet_regs;

typedef struct{
	float bldet_bl_buff[SAMPLE_DATA_NUM];
	signed short bldet_tempbuff[SAMPLE_DATA_NUM + FILTER_COEF_NUM];
	//signed short bldet_rawdata[SAMPLE_DATA_NUM];
}BLDET_MixMemType;
extern BLDET_MixMemType bldet_bl_calmem;
extern void fnBL_Task(void);
extern void fnBL_Init(void);

#endif
