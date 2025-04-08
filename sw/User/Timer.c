#include "define.h"
#include "Timer.h"
#include "IO.h"

STRTMRegType g_tm_stTimerFlag;
STRTMRegType tm_sTimerFlag;
#define TM_INT_LOAD (SystemCoreClock / 4000)//2000-500us, 4000-250us

void timer_Init(void);
void timer_RealTime(void);
void timer_clear(void);
void timer_init(void)
{
    SysTick->VAL = 0;
	SysTick->LOAD = (TM_INT_LOAD - 1);//48000 for 4MHz, 49152 for 4.096MHZ
    SysTick->CTRL = 0x07;
	memset(&g_tm_stTimerFlag, 0, sizeof(g_tm_stTimerFlag));

	/*
	one system timer for application purpose	
	*/
}
void timer_task(void)
{ 
	static U16 tm_uchTimer16msCount = 0;
	
	if(tm_sTimerFlag.Bits.bTimer2ms)
	{
		SysTick->CTRL = 0x00;
		memcpy(&g_tm_stTimerFlag, &tm_sTimerFlag, sizeof(tm_sTimerFlag));
		memset(&tm_sTimerFlag, 0, sizeof(tm_sTimerFlag));
	    SysTick->CTRL = 0x07;
	}
	if(g_tm_stTimerFlag.Bits.bTimer16ms)
	{
		tm_uchTimer16msCount ++;		
		if(0 == (tm_uchTimer16msCount  & 0x0001))
		{
			g_tm_stTimerFlag.Bits.bTimer32ms = TRUE;
		}
		if(0 == (tm_uchTimer16msCount  & 0x0003))
		{
			g_tm_stTimerFlag.Bits.bTimer64ms = TRUE;
		}
		if(0 == (tm_uchTimer16msCount  & 0x0FFF))//5
		{
			
			g_tm_stTimerFlag.Bits.bTimer1min = TRUE;
		}
			
	}
}
void timer_clear(void)
{
    memset(&g_tm_stTimerFlag, 0, sizeof(g_tm_stTimerFlag));
}


void SysTick_Handler(void)
{
	static U8 tm_uchTimer500usCount = 0;
	static U8 tm_uchTimer2msCount = 0;
	static U8 tm_uchTimer10msCount = 0;
	static U8 tm_uchTimer100mscount = 0;
	static U8 counter250us = 0;
	static U8 counter1s = 0;
	counter250us ++;
	if(counter250us <= 1){
		return;
	}
	counter250us = 0;
	tm_uchTimer500usCount ++;
	tm_uchTimer10msCount++;
	tm_uchTimer100mscount ++;
	if(tm_uchTimer100mscount >= 200)
	{
		tm_sTimerFlag.Bits.bTimer100ms = TRUE;
		tm_uchTimer100mscount = 0;
		counter1s ++;
		if(counter1s >= 10){
			counter1s = 0;
			tm_sTimerFlag.Bits.bTimer1s = TRUE;
		}
	}
	if(tm_uchTimer10msCount >= 20)
	{
		tm_sTimerFlag.Bits.bTimer10ms = TRUE;
		tm_uchTimer10msCount = 0;
	}
    
	if(0 == (tm_uchTimer500usCount & 0x03))
	{
			tm_sTimerFlag.Bits.bTimer2ms = TRUE;
	    tm_uchTimer2msCount ++;   
	    if(!(tm_uchTimer2msCount & 0x03))
	    {
	    	tm_sTimerFlag.Bits.bTimer8ms = TRUE;
	    }
	    if(!(tm_uchTimer2msCount & 0x07))
	    {
	    	tm_sTimerFlag.Bits.bTimer16ms = TRUE;
	    } 
	}

	
}


