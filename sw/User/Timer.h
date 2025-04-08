#ifndef _H_TIMER
#define _H_TIMER
#include "define.h"
typedef union
{
	U8 BYTES[10];
	struct
	{
		
		U8 bTimer2ms ;
		U8 bTimer4ms ;
		U8 bTimer8ms ;
		U8 bTimer10ms; 
		U8 bTimer16ms ;
		U8 bTimer32ms ;
		U8 bTimer64ms ;
		U8 bTimer100ms ;
		U8 bTimer1s ; 
		U8 bTimer1min; 
	}Bits;
}STRTMRegType;

extern STRTMRegType g_tm_stTimerFlag;
extern void timer_init(void);
extern void timer_task(void);
extern void timer_clear(void);


#endif

