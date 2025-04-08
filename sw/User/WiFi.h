#ifndef _H_WIFI_
#define _H_WIFI_
#include "define.h"
#define WIFI_CONAPS_MAXLEN 512
#define WIFI_CONNECT_STATE_NOAP_DISCONNECT 0
#define WIFI_CONNECT_STATE_NOAP_CONNECT 1
#define WIFI_CONNECT_STATE_AP_DISCONNECT 2
#define WIFI_CONNECT_STATE_AP_CONNECT 3

typedef struct{

	U8 m_uchQueryTimer;
	U8 m_uchDeadTimer;
	U8 m_uchStartUpTimer;
}STRWiFiType;
extern STRWiFiType g_wifi_stRegs;

extern void fnWF_Init(void);
extern void fnWF_RealTime(void);
extern void fnWF_ResetDeadTime(void);
extern void fnWF_ResetQueryTime(void);

#endif

