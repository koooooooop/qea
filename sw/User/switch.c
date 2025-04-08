#include "Switch.h"
#include "Timer.h"
#include "IO.h"
#define KEY_TIMEBASE ((U8)64)
#define KEY_MAXTIMES   (U8)((U16)12000/KEY_TIMEBASE)    //约12秒,最大计时
#define KEY_TIMETYPENUM 3
const U8 key_cPressTime[KEY_TIMETYPENUM]={(U16)3000/KEY_TIMEBASE,(U16)5000/KEY_TIMEBASE,(U16)10000/KEY_TIMEBASE};

STRSW_DataType g_sw_stRegs[XNUM];

STRSWConfig g_sw_stConfig[XNUM] = 
{
		{P_SW_1_PORT, P_SW_1_PIN, 15}, // KY_NUM0
    {P_SW_2_PORT, P_SW_2_PIN, 15}, // KY_NUM0
    {P_SW_3_PORT, P_SW_3_PIN, 15}, // KY_NUM0
};




void fnSW_RealTime(void);
void fnSW_Init(void);
void fnSW_Clear(void);

void fnSW_Clear(void)
{
	U8 i;
	for(i = 0; i< XNUM; i++)
	{
		g_sw_stRegs[i].m_uchTrigger = FALSE;
		g_sw_stRegs[i].m_uchEvent = KEY_EVENT_NULL;
	}
}	
void fnSW_Init(void)
{
	U8 i;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);// 打开串口GPIO的时钟
	for(i = 0; i< XNUM; i++)  //
	{
      SET_IO_IN_PULLUP(g_sw_stConfig[i].port, g_sw_stConfig[i].pin);
  }
	memset(&g_sw_stRegs, 0, sizeof(g_sw_stRegs));
}
U8 fnSW_GetState(U8 index)
{
	U8 res = FALSE;
	if(index < XNUM)
	{
		if(0 == (GET_IO_STA(g_sw_stConfig[index].port, g_sw_stConfig[index].pin)))
		{
				res = TRUE;
		}
	}
	return(res);
}	

void fnSW_RealTime(void)
{
	U8 i,j, temp;
	U32 level;
	if(g_tm_stTimerFlag.Bits.bTimer2ms)
   {  

			level = 0;
       for(i= 0; i< XNUM; i++)
	    {
					temp = fnSW_GetState(i);
	        if(temp  != g_sw_stRegs[i].m_uchState)                        //i按键状态有变化
	        {
	            g_sw_stRegs[i].m_uchCount++;                                  //去抖计数
	        }
	        else 
	        {
	           g_sw_stRegs[i].m_uchCount = 0;
	        }
	        if (g_sw_stRegs[i].m_uchCount >= g_sw_stConfig[i].debounce)
	        {
	            g_sw_stRegs[i].m_uchState = temp;            //i按键状态取反,状态确定
	            g_sw_stRegs[i].m_uchTrigger = TRUE;         //i按键状态变化标志
	            g_sw_stRegs[i].m_uchCount = 0;
	        }
	    }

	}
	if(g_tm_stTimerFlag.Bits.bTimer64ms)
    {
        for(i = 0; i < XNUM; i++)
        {
        	temp = FALSE;
        	if(g_sw_stRegs[i].m_uchState)
        	{
        		temp =TRUE;
				if(g_sw_stRegs[i].m_uchStateTime <= KEY_MAXTIMES)
                {
                     g_sw_stRegs[i].m_uchStateTime ++;
                }
        		
        	}
			
          
            if(temp)
            {
                for(j = 0; j< KEY_TIMETYPENUM; j++)
                {
                    if(g_sw_stRegs[i].m_uchStateTime == key_cPressTime[j])
                    {
                        g_sw_stRegs[i].m_uchEvent = j + 1;
                        break;
                    }
                }
            }
            else
            {
                if(g_sw_stRegs[i].m_uchStateTime  > 0)
                {
                    temp = KEY_TIMETYPENUM + 1;
                    for(j= 0; j< KEY_TIMETYPENUM; j++)
                    {
                        if(g_sw_stRegs[i].m_uchStateTime <= key_cPressTime[j])
                        {
                            temp = j + 1;
                            break;
                        }
                    }
					g_sw_stRegs[i].m_uchEvent = (0x80 | temp);
                }
                
                g_sw_stRegs[i].m_uchStateTime = 0;  
            }
               
        }
       
    }
}		


