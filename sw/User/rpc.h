#ifndef _H_RPC_
#define _H_RPC_
#include "define.h"

enum
{
	MESSAGE_PARAMTYPE_R = 0,
	MESSAGE_PARAMTYPE_RUNMODE,
	MESSAGE_PARAMTYPE_INPUT,
	MESSAGE_PARAMTYPE_OUTPUT,
	
	MESSAGE_PARAMTYPE_VARS, //24
	
	MESSAGE_PARAMTYPE_AD,
	MESSAGE_PARAMTYPE_TM,
	MESSAGE_PARAMTYPE_DY,
	
	MESSAGE_PARAMTYPE_STEPNUM, //8
	MESSAGE_PARAMTYPE_INS,
	MESSAGE_PARAMTYPE_RUN,
	MESSAGE_PARAMTYPE_RESULT,
	
	MESSAGE_PARAMTYPE_RT, //12
	MESSAGE_PARAMTYPE_OTHER,
	MESSAGE_PARAMTYPE_VOLATILEVARS,
	MESSAGE_PARAMTYPE_IN_SETSTATE,
	
	MESSAGE_PARAMTYPE_IN_INS, //16
	MESSAGE_PARAMTYPE_IN_RT,
	MESSAGE_PARAMTYPE_IN_RUN,
	MESSAGE_PARAMTYPE_IN_RTC,
	
	MESSAGE_PARAMTYPE_IN_ADDR, //20
	MESSAGE_PARAMTYPE_IN_NAME,
	MESSAGE_PARAMTYPE_IN_MODE,
	MESSAGE_PARAMTYPE_IN_VARS,
	
	MESSAGE_PARAMTYPE_IN_CONPC, //24
	MESSAGE_PARAMTYPE_IN_SYSINFO,
	MESSAGE_PARAMTYPE_IN_DEVNAME,
	MESSAGE_PARAMTYPE_IN_FLASHVAR,
	
	MESSAGE_PARAMTYPE_IN_BINARY,
	MESSAGE_PARAMTYPE_IN_MOTOREN,
	MESSAGE_PARAMTYPE_IN_DEVID,
	MESSAGE_PARAMTYPE_IN_SYSCODE,

	
	MESSAGE_PARAMTYPE_IN_DEVSN, 
	MESSAGE_PARAMTYPE_LORADEVICEID,
	MESSAGE_PARAMTYPE_PAIR,
	MESSAGE_PARAMTYPE_FORCEON,
	
	MESSAGE_PARAMTYPE_IN_MSGID,
	MESSAGE_PARAMTYPE_IN_VARINRANGE,

	MESSAGE_PARAMTYPE_IN_TCPCONFIG,
	MESSAGE_PARAMTYPE_IN_LANSET,
	MESSAGE_PARAMTYPE_IN_MBSET, 
	MESSAGE_PARAMTYPE_IN_DATA_RAW,
	MESSAGE_PARAMTYPE_IN_DATA_AVS,
	MESSAGE_PARAMTYPE_IN_DATA_BAOLUO,
	MESSAGE_PARAMTYPE_IN_DATA_BAOLUOFFT,
};
enum
{
	MESSAGE_METHOD_NAME_GETPROGRAM = 0xA5,
	MESSAGE_METHOD_NAME_GETALL,
	MESSAGE_METHOD_NAME_GETRT,
	MESSAGE_METHOD_NAME_GETSYS,
	MESSAGE_METHOD_NAME_SAVESYS,
	MESSAGE_METHOD_NAME_SAVERT,
	MESSAGE_METHOD_NAME_SAVERUN,
	MESSAGE_METHOD_NAME_DELRT,
	MESSAGE_METHOD_NAME_SAVEVAR,
	MESSAGE_METHOD_NAME_CONPC,
	MESSAGE_METHOD_NAME_PING,
	MESSAGE_METHOD_NAME_RESET,
	MESSAGE_METHOD_NAME_SAVEFLASH,
	MESSAGE_METHOD_NAME_GETSYSINFO,
	MESSAGE_METHOD_NAME_SAVEBINARYCMD,
	MESSAGE_METHOD_NAME_DIAG, 
	MESSAGE_METHOD_NAME_DISCOVERY,
	MESSAGE_METHOD_NAME_TRANSMITDATA
};


#define RPC_STATE_TIME 1200
typedef struct{
	U8 m_uchState;
	U16 m_uchTimer;
	U8 m_uchSendState;
	U16 m_uchSendTimer;
	U16 m_uchLen;
	U8 *m_uaBuff;
	U8 m_uchMethod;
	U16 m_uchMsgID;
	U8 m_uchCMDSource;
}STRRTMRegs;
extern STRRTMRegs g_rtm_stRegs;

extern void fnRPC_RealTime(void);
extern void  fnRPC_Init(void);

extern U8 rpc_getvaluebytype(U8 method, U8 *msg, U32 *value, U16 tlen);
extern U8 *rpc_getpointerbytype(U8 method, U8 *msg, U16 len);


#endif
