#ifndef _H_RTMANAGE_
#define _H_RTMANAGE_
#include "define.h"
#define RTM_STATE_TIME 1200
typedef struct{
	U8 m_uchState;
	U16 m_uchTimer;
	U8 m_uchSendState;
	U16 m_uchSendTimer;
	U16 m_uchLen;
	U8 *m_uaBuff;
	U8 m_uchMethod;
}STRRTMRegs;
extern STRRTMRegs g_rtm_stRegs;

extern void fnRTM_RealTime(void);
extern void fnRTM_Init(void);
extern U8 fnRTM_IsSYS(void);
extern U8 fnRTM_IsIdle(void);
extern U8 fnRTM_ResetServer(void);
extern U8 fnRTM_PingServer(void);
extern U8 fnRTM_SendDiagCmd(U8 *buff, U8 len);
extern U8 fnRTM_isDiag(U8 cmd);
#endif
