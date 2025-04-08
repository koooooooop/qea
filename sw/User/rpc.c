#include "rpc.h"
#include "Timer.h"

STRRTMRegs g_rtm_stRegs;
void fnRPC_RealTim(void);
void fnRPC_Init(void);

#define MESSAGE_OFFSET_METHOD 8
#define MESSAGE_OFFSET_DATA 9 //55FF 4 bytes msgid, 2byte length

U16 fnRPC_GetSys(U8 method, U8 *msg);

U16 fnRPC_SaveBinary(U8 method, U8 *msg);




typedef struct
{
	const U8 method;
	U16 (*fun)(U8 method, U8 *msg);
}RPCMethodType;
#define RPC_FUN_NUM 2
const RPCMethodType rpcmethods[RPC_FUN_NUM] =
{
	{MESSAGE_METHOD_NAME_GETSYS,fnRPC_GetSys},
	{MESSAGE_METHOD_NAME_SAVEBINARYCMD,fnRPC_SaveBinary},
};

U8 rpc_getvaluebytype(U8 method, U8 *msg, U32 *value, U16 tlen)
{
	U8 res = FALSE;
	U16 nextparam;
	U16 paramlen;
	U8 paramtype;
	U8 len,i;
	*value = 0;
	nextparam = 0;
	while((nextparam + 2) < tlen)
	{
		paramtype = msg[nextparam];
		paramlen = (U16)msg[nextparam + 1] << 8;
		paramlen |= (U16)msg[nextparam + 2];
		if(((nextparam + paramlen + 1) > tlen)
			|| (paramlen < 1))
		{
			break;
		}
		else
		{
			if(paramtype == method)
			{
				len = 0;
				if(paramlen >= 3)
				{
					len = paramlen - 3;
					
				}
				for(i = 0; i< len; i++)
				{
					*value = *value << 8;
					*value |= (U32)msg[nextparam + 3 + i];
				}
				res = TRUE;
				break;
			}
		}
		nextparam += paramlen;
	}

	return(res);
	
}

U8 *rpc_getpointerbytype(U8 method, U8 *msg, U16 len)
{
	U16 nextparam;
	U16 paramlen;
	U8 paramtype;
	U8 *p = NULL;
	nextparam = 0;
	while((nextparam + 2) < len)
	{
		paramtype = msg[nextparam];
		paramlen = (U16)msg[nextparam + 1] << 8;
		paramlen |= (U16)msg[nextparam + 2];
		if(((nextparam + paramlen + 1) > len)
			|| (paramlen < 1))
		{
			break;
		}
		else
		{
			if(paramtype == method)
			{
				p = &msg[nextparam];
				break;
			}
		}
		nextparam += paramlen;
	}

	return(p);
	
}




U16 fnRPC_GetParam_SYSINFO(U8 *buff)
{
	U16 pointer;
	
	buff[0] = MESSAGE_PARAMTYPE_IN_SYSINFO;
	pointer = 3;

	buff[pointer] = (U8)(SWVER >> 8);
	pointer++;

	buff[pointer] = (U8)(SWVER);
	pointer++;

	buff[pointer] = (U8)(HWVER >> 8);
	pointer++;

	buff[pointer] = (U8)(HWVER);
	pointer++;


	

	buff[pointer] = g_mn_uchSoftwareMagicNum[SOFTWARE_MAGICNUM_OFF_SVERH];
	pointer++;

	buff[pointer] = g_mn_uchSoftwareMagicNum[SOFTWARE_MAGICNUM_OFF_SVERL];
	pointer++;

	buff[pointer] = g_mn_uchSoftwareMagicNum[SOFTWARE_MAGICNUM_OFF_HVERH];
	pointer++;

	buff[pointer] = g_mn_uchSoftwareMagicNum[SOFTWARE_MAGICNUM_OFF_HVERL];
	pointer++;

	buff[pointer] = g_mn_uchSoftwareMagicNum[SOFTWARE_MAGICNUM_OFF_DEVTYPE];
	pointer++;
	
	buff[1] = (U8)(pointer >> 8);
    	buff[2] = (U8)pointer;
	return(pointer);
	
}

U16 fnRPC_GetParam_DevName(U8 *buff)
{
	U16 pointer;
	buff[0] = MESSAGE_PARAMTYPE_IN_DEVNAME;
	pointer = 3;

	//fnAP_GetName((char *)&buff[pointer], &pointer);
	
	buff[1] = (U8)(pointer >> 8);
  	buff[2] = (U8)pointer;
	return(pointer);
	
}

U16 fnRPC_GetSys(U8 method, U8 *msg)
{
	U16 pointer = 0;
	
       pointer += fnRPC_GetParam_SYSINFO(&g_rtm_stRegs.m_uaBuff[pointer]);
	pointer += fnRPC_GetParam_DevName(&g_rtm_stRegs.m_uaBuff[pointer]);

	return(pointer);
}



U8 fnRPC_SaveBinary_Dispatch(U32 msgid, U32 len, U8 *ins, U8 *ack, U16 *acklen){
	U8 res = FALSE;

	return(res);
	
}
U16 fnRPC_SaveBinary(U8 method, U8 *msg)
{
	U16 pointer = 0;
	return(pointer);
}


#define RTM_SENDSTATE_IDLE 0
#define RTM_SENDSTATE_SEND 1
U8 fnRPC_ResetServer(void)
{
	U8 res = FALSE;
	return(res);
}

U8 fnRPC_SendDiagCmd(U8 *buff, U8 len){
	U8 res = FALSE;

	return(res);
}
U8 fnRPC_PingServer(void)
{
	U8 res = FALSE;

	return(res);
}


void fnRPC_ReqSend(U8 msgid, U16 len)
{	
}
U8 testrtm = 0;
void fnRPC_Comm(void)
{
	U8 i, method;
	U16 pointer, len;
	U16 msgid; 
	U8 *buff;
	U8 reccmd;
	if(RTM_SENDSTATE_IDLE == g_rtm_stRegs.m_uchSendState)
	{
		/*g_rtm_stRegs.m_uchCMDSource = RTM_CMD_SOURCE_UART;
		reccmd = FALSE;
	
		if(g_tcpc_stRegs.m_uchReceived)
		{ 
			if(g_tcpc_stRegs.m_uchProtocolType == TCPC_PROTOCOL_TYPE_CR){
				g_rtm_stRegs.m_uchCMDSource = RTM_CMD_SOURCE_TCPC;
		    	g_rtm_stRegs.m_uaBuff = &g_tcpc_stRegs.sendreqbuff[6];
				buff = g_tcpc_stRegs.m_uchRecRawBuff;
				reccmd = TRUE;
			}
		}
		if(reccmd){
			
			msgid = (U16)buff[0] << 8;
			msgid |= (U16)buff[1];
			g_rtm_stRegs.m_uchMsgID = msgid;
			if(0 == (msgid & 0x8000))
			{
				method = buff[4];
				g_rtm_stRegs.m_uchMethod = method;
				
				len = (U16)buff[2] << 8;
				len |= (U16)buff[3];
				g_rtm_stRegs.m_uchLen = len;
				for(i = 0; i < RPC_FUN_NUM; i++)
				{
					if(method == rpcmethods[i].method)
					{
						pointer = rpcmethods[i].fun(method, (U8 *)&buff[5]);
						if(pointer > 0)
						{
							fnRPC_ReqSend(msgid, pointer);
							g_rtm_stRegs.m_uchSendState = RTM_SENDSTATE_SEND;
							
						}
						break;
					}
				}
				if(method == MESSAGE_METHOD_NAME_DIAG)
				{
						fnTest_WFCallBack(buff);
				}
			}

			///g_tcpc_stRegs.m_uchReceived = FALSE;
			
		}
		*/
	}
	else if(RTM_SENDSTATE_SEND == g_rtm_stRegs.m_uchSendState)
	{
		g_rtm_stRegs.m_uchSendState = RTM_SENDSTATE_IDLE;
	}
	
}

void fnRPC_RealTime(void)
{
	fnRPC_Comm();
	if(g_tm_stTimerFlag.Bits.bTimer100ms){
		if(g_rtm_stRegs.m_uchTimer)
			g_rtm_stRegs.m_uchTimer --;
	}
}
void fnRPC_Init(void){
	memset(&g_rtm_stRegs, 0, sizeof(g_rtm_stRegs));
}

U8 fnRPC_isDiag(U8 cmd){
	U8 res = FALSE;
	 if(cmd == MESSAGE_METHOD_NAME_DIAG){
		 res = TRUE;
	 }
	 return(res);
}




