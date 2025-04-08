#ifndef _H_SWITCH
#define _H_SWITCH
#include "define.h"





#define KEY_EVENT_NULL          0x00
#define KEY_EVENT_3SLONGPRESS   0x01      //3�볤����δ����
#define KEY_EVENT_5SLONGPRESS   0x02       //5�볤����δ����
#define KEY_EVENT_SHORTRELEASE  0x81      //�̰�����0-3�����
#define KEY_EVENT_3SRELEASE     0x82      //3�밴����3-10�����
#define KEY_EVENT_RELEASEMASK   0x80


#define P_SW_1_PORT GPIOE
#define P_SW_1_PIN 4

#define P_SW_2_PORT GPIOE
#define P_SW_2_PIN 3

#define P_SW_3_PORT GPIOE
#define P_SW_3_PIN 2



typedef struct{
	GPIO_TypeDef *port;
	U8 pin;
	U8 debounce;
}STRSWConfig;

typedef struct
{
	U8 m_uchState;
	U8 m_uchTrigger;
	U8 m_uchCount;
	U8 m_uchEvent;
	U8 m_uchStateTime;
	U8 m_uchLevel;
	U8 m_uchLevelTimer;
}STRSW_DataType;
extern STRSW_DataType g_sw_stRegs[XNUM];
extern STRSWConfig g_sw_stConfig[XNUM];
#define INPUT_GET_STATE(var) g_sw_stRegs[var].m_uchState
#define INPUT_GET_TRIGGER(var) g_sw_stRegs[var].m_uchTrigger
extern void fnSW_RealTime(void);
extern void fnSW_Init(void);
extern void fnSW_Clear(void);
#endif
