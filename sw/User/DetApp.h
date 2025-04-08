#ifndef _H_DETAPP_
#define _H_DETAPP_
#include "define.h"


#define DET_APP_SW_OFFSET 2

#define DET_APP_INDICATOR_PORT    	GPIOF
#define DET_APP_INDICATOR_CLK 	    RCC_AHB1Periph_GPIOF
#define DET_APP_INDICATOR_PIN		10
#define DET_APP_INDICATOR_PIN_DEF       GPIO_Pin_10


typedef struct{
	U8 m_uchState;
	U16 m_usTimer;
}STRDetAppRegType;

#define DETAPP_STATE_IDLE 0
#define DETAPP_STATE_RUN 1

extern STRDetAppRegType g_detapp_stRegs;
extern void fnDetApp_Init(void);
extern void fnDetApp_RealTime(void);
#endif
