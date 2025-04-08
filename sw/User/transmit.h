#ifndef _H_TRANSMIT_
#define _H_TRANSMIT_
#include "define.h"
#define TRS_TRANSMIT_BYWIFI 1
#define TRS_DATATYPE_RAWDATA 0
#define TRS_DATATYPE_AVS 1
#define TRS_DATATYPE_BAOLUO 2
#define TRS_DATATYPE_BAOLUOFFT 3

#define TRS_STATE_IDLE 0
#define TRS_STATE_TRANSMIT 1
	#define TRS_STATE_TRANSMIT_INIT 0
	#define TRS_STATE_TRANSMIT_SEND 1
	#define TRS_STATE_TRANSMIT_WAIT 2
typedef struct{
	U16 datatype;
	U16 len;
	void *addr;
	U16 pointer;
	U8 state;
	U16 timer;
	U8 substate;
	U16 subtimer;
	U8 channel;
	U16 totalbytelen;
	U16 alreadysendcount;
	U8 sendsuccess;
	U8 trytimes;
	U16 msgid;
	U8 sendfinish;
}trs_regs_typedef;
extern trs_regs_typedef trs_regs;
extern U8 fnTRS_ReqRawData(void *addr, U16 datatype, U8 chind, U8 chnum, U16 totalbytelen);
extern void fnTRS_Init(void);
extern void fnTRS_RealTime(void);
extern U8 fnTRS_IsFinish(void);
#endif
