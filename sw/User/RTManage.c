#include "RTManage.h"
#include "Timer.h"
#

#include "WiFi.h"

#include "APP.h"

#include "uartwifi.h"
#include "strs.h"
#define RTM_RECEIVE_BUFFSIZE 600
#define RTM_STATE_IDLE 0x00
#define RTM_STATE_PROGRAM 0x01
#define RTM_STATE_SYS  0x02
#define RTM_VARS_NUM 250
#define RTM_VARS_OFFSET 250

STRRTMRegs g_rtm_stRegs;
typedef struct
{
	U8 m_uchRT;
	U8 m_uchRTPage;
	U8 m_uchTimer;
}STRRTBuffType;
STRRTBuffType g_rtm_stBuff;
void fnRTM_RealTim(void);
void fnRTM_Init(void);
enum
{
	MESSAGE_PARAMTYPE_R = 0,
	MESSAGE_PARAMTYPE_RUNMODE,
	MESSAGE_PARAMTYPE_INPUT,
	MESSAGE_PARAMTYPE_OUTPUT,
	
	MESSAGE_PARAMTYPE_VARS,
	MESSAGE_PARAMTYPE_AD,
	MESSAGE_PARAMTYPE_TM,
	MESSAGE_PARAMTYPE_DY,
	
	MESSAGE_PARAMTYPE_STEPNUM,
	MESSAGE_PARAMTYPE_INS,
	MESSAGE_PARAMTYPE_RUN,
	MESSAGE_PARAMTYPE_RESULT,
	
	MESSAGE_PARAMTYPE_RT,
	MESSAGE_PARAMTYPE_OTHER,
	MESSAGE_PARAMTYPE_VOLATILEVARS,
	MESSAGE_PARAMTYPE_IN_SETSTATE,
	
	MESSAGE_PARAMTYPE_IN_INS,
	MESSAGE_PARAMTYPE_IN_RT,
	MESSAGE_PARAMTYPE_IN_RUN,
	MESSAGE_PARAMTYPE_IN_RTC,
	
	MESSAGE_PARAMTYPE_IN_ADDR,
	MESSAGE_PARAMTYPE_IN_NAME,
	MESSAGE_PARAMTYPE_IN_MODE,
	MESSAGE_PARAMTYPE_IN_VARS,
	
	MESSAGE_PARAMTYPE_IN_CONPC,
	MESSAGE_PARAMTYPE_IN_SYSINFO,
	MESSAGE_PARAMTYPE_IN_DEVNAME,
	MESSAGE_PARAMTYPE_IN_FLASHVAR,
	
	MESSAGE_PARAMTYPE_IN_BINARY,
	MESSAGE_PARAMTYPE_IN_MOTOREN,
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
};

#define MESSAGE_OFFSET_METHOD 8
#define MESSAGE_OFFSET_DATA 9 //55FF 4 bytes msgid, 2byte length

U16 fnRTM_GetProgram(U8 method, U8 *msg);
U16 fnRTM_GetAll(U8 method, U8 *msg);
U16 fnRTM_GetRT(U8 method, U8 *msg);
U16 fnRTM_DelRT(U8 method, U8 *msg);
U16 fnRTM_SaveRT(U8 method, U8 *msg);
U16 fnRTM_SaveRun(U8 method, U8 *msg);
U16 fnRTM_GetSys(U8 method, U8 *msg);
U16 fnRTM_SaveSys(U8 method, U8 *msg);
U16 fnRTM_SaveVar(U8 method, U8 *msg);

U16 fnRTM_ChangeMode(U8 method, U8 *msg);

U16 fnRTM_SaveFlash(U8 method, U8 *msg);

U16 fnRTM_GetSysInfo(U8 method, U8 *msg);

U16 fnRTM_SaveBinary(U8 method, U8 *msg);




typedef struct
{
	const U8 method;
	U16 (*fun)(U8 method, U8 *msg);
}RPCMethodType;
#define RPC_FUN_NUM 13
const RPCMethodType rpcmethods[] =
{
	{MESSAGE_METHOD_NAME_GETPROGRAM,fnRTM_GetProgram}, 
	{MESSAGE_METHOD_NAME_GETALL,fnRTM_GetAll},
	{MESSAGE_METHOD_NAME_GETRT,fnRTM_GetRT},
	{MESSAGE_METHOD_NAME_GETSYS,fnRTM_GetSys},
	
	{MESSAGE_METHOD_NAME_SAVESYS,fnRTM_SaveSys},
	{MESSAGE_METHOD_NAME_SAVEVAR,fnRTM_SaveVar},
	{MESSAGE_METHOD_NAME_SAVERT,fnRTM_SaveRT},
	{MESSAGE_METHOD_NAME_SAVERUN,fnRTM_SaveRun},
	
	{MESSAGE_METHOD_NAME_DELRT,fnRTM_DelRT},
	{MESSAGE_METHOD_NAME_CONPC,fnRTM_ChangeMode},

	{MESSAGE_METHOD_NAME_SAVEFLASH,fnRTM_SaveFlash},
	{MESSAGE_METHOD_NAME_GETSYSINFO,fnRTM_GetSysInfo},

	{MESSAGE_METHOD_NAME_SAVEBINARYCMD,fnRTM_SaveBinary},
};

U8 rpc_getvaluebytype(U8 method, U8 *msg, U32 *value)
{
	U8 res = FALSE;
	U16 nextparam;
	U16 paramlen;
	U8 paramtype;
	U8 len,i;
	*value = 0;
	nextparam = 0;
	while((nextparam + 2) < g_rtm_stRegs.m_uchLen)
	{
		paramtype = msg[nextparam];
		paramlen = (U16)msg[nextparam + 1] << 8;
		paramlen |= (U16)msg[nextparam + 2];
		if(((nextparam + paramlen + 1) > g_rtm_stRegs.m_uchLen)
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

U8 *rpc_getpointerbytype(U8 method, U8 *msg)
{
	U16 nextparam;
	U16 paramlen;
	U8 paramtype;
	U8 *p = NULL;
	nextparam = 0;
	while((nextparam + 2) < g_rtm_stRegs.m_uchLen)
	{
		paramtype = msg[nextparam];
		paramlen = (U16)msg[nextparam + 1] << 8;
		paramlen |= (U16)msg[nextparam + 2];
		if(((nextparam + paramlen + 1) > g_rtm_stRegs.m_uchLen)
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


/*
method 1: get program
params: 1: state & step, 3 * byte array
params 2:runmode,1 byte;
*/
const char g_hp_pm_1[] = {"{\"a\":\"get\",\"t\":\"program\",\"r\":\""};
const char g_hp_pm_2[] = {"\",\"runmode\":"};

const char g_hp_pm_3[] = {""};
U16 fnRTM_GetParam_RoutineInfo(U8 *buff)
{
	U16 pointer;
	U8 data;
	U8 i;
	buff[0] = MESSAGE_PARAMTYPE_R;
	pointer = 3;
	for(i = 0; i< 24; i++)
	{

		
		buff[pointer] = 0;

		data = 0;
		data = data << 4;
		data |= 0;
		
		buff[pointer + 1] = data;
		buff[pointer + 2] =  0;
		pointer += 3;
	}
	buff[1] = (U8)(pointer >> 8);
	buff[2] = (U8)pointer;
	return(pointer);
	
}

U16 fnRTM_GetParam_RunMode(U8 *buff)
{
	U16 pointer;
	buff[0] = 0;
	pointer = 3;
	
	buff[pointer] = 0;
	pointer++;
	
	buff[1] = (U8)(pointer >> 8);
    buff[2] = (U8)pointer;
	return(pointer);
	
}
U16 fnRTM_GetParam_FlashVar(U8 *buff)
{
	U16 pointer, i, value;
	buff[0] = MESSAGE_PARAMTYPE_IN_FLASHVAR;
	pointer = 3;

	for(i = 0; i <= 1; i++)
	{
		buff[pointer] = (U8)(i >> 8);
		pointer++;
		buff[pointer] = (U8)(i &0xff);
		pointer++;
		value = 0;
		buff[pointer] = (U8)(value >> 8);
		pointer++;
		buff[pointer] = (U8)(value);
		pointer++;
	}
	buff[1] = (U8)(pointer >> 8);
    buff[2] = (U8)pointer;
	return(pointer);
	
}

U16 fnRTM_GetParam_Input(U8 *buff)
{
	U16 pointer;
	U16 data;
	buff[0] = MESSAGE_PARAMTYPE_INPUT;
	pointer = 3;
	
	data = 0;
	buff[pointer] = (U8)(data >> 8);
	buff[pointer + 1] = (U8)data;
	pointer +=2;
	
	buff[1] = (U8)(pointer >> 8);
    buff[2] = (U8)pointer;
	return(pointer);
	
}

U16 fnRTM_GetParam_Output(U8 *buff)
{
	U16 pointer;
	U16 data;
	buff[0] = MESSAGE_PARAMTYPE_OUTPUT;
	pointer = 3;
	
	data =  0;
	buff[pointer] = (U8)(data >> 8);
	buff[pointer + 1] = (U8)data;
	pointer +=2;
	
	buff[1] = (U8)(pointer >> 8);
    buff[2] = (U8)pointer;
	return(pointer);
	
}
#define MAXVAR_BUFF 200
U16 fnRTM_GetParam_VARS(U8 *buff)
{
	U16 pointer;
	buff[0] = MESSAGE_PARAMTYPE_VARS;
	pointer = 3;
	
	buff[1] = (U8)(pointer >> 8);
  buff[2] = (U8)pointer;
	return(pointer);
	
}

U16 fnRTM_GetParam_VOLATILEVARS(U8 *buff)
{
	U16 pointer;
	buff[0] = MESSAGE_PARAMTYPE_VOLATILEVARS;
	pointer = 3;
	buff[1] = (U8)(pointer >> 8);
    buff[2] = (U8)pointer;
	return(pointer);
	
}


U16 fnRTM_GetParam_AD(U8 *buff)
{
	U16 pointer;
	U8 i;
	U16 data;
	buff[0] = MESSAGE_PARAMTYPE_AD;
	pointer = 3;
	buff[1] = (U8)(pointer >> 8);
    buff[2] = (U8)pointer;
	return(pointer);
	
}
U16 fnRTM_GetParam_TM(U8 *buff)
{
	U16 pointer;
	U8 i;
	U32 value;
	buff[0] = MESSAGE_PARAMTYPE_TM;
	pointer = 3;

	
	buff[1] = (U8)(pointer >> 8);
    buff[2] = (U8)pointer;
	return(pointer);
	
}

U16 fnRTM_GetParam_DY(U8 *buff)
{
	U16 pointer;
	U8 i;
	U32 value;
	buff[0] = MESSAGE_PARAMTYPE_DY;
	pointer = 3;


	buff[1] = (U8)(pointer >> 8);
    buff[2] = (U8)pointer;
	return(pointer);
	
}

U16 fnRTM_GetParam_STEPNUM(U8 *buff, U8 rid)
{
	U16 pointer;
	buff[0] = MESSAGE_PARAMTYPE_STEPNUM;
	pointer = 3;

	buff[pointer] = 0;
	pointer++;
	
	buff[1] = (U8)(pointer >> 8);
    buff[2] = (U8)pointer;
	return(pointer);
	
}

U16 fnRTM_GetParam_INS(U8 *buff, U8 rid)
{
	U16 pointer;
	buff[0] = MESSAGE_PARAMTYPE_INS;
	pointer = 3;
	buff[1] = (U8)(pointer >> 8);
    buff[2] = (U8)pointer;
	return(pointer);
	
}

U16 fnRTM_GetParam_RUN(U8 *buff, U8 rid)
{
	U16 pointer;
	buff[0] = MESSAGE_PARAMTYPE_RUN;
	pointer = 3;

	buff[pointer] = 0;
	pointer++;

	
	buff[1] = (U8)(pointer >> 8);
  buff[2] = (U8)pointer;
	return(pointer);
	
}

U16 fnRTM_GetParam_RESULT(U8 *buff, U8 result)
{
	U16 pointer;
	buff[0] = MESSAGE_PARAMTYPE_RESULT;
	pointer = 3;

	buff[pointer] = 0;
	pointer++;

	
	buff[1] = (U8)(pointer >> 8);
    buff[2] = (U8)pointer;
	return(pointer);
	
}
U16 fnRTM_GetParam_RT(U8 *buff, U16 rid)
{
	U16 pointer;
	buff[0] = MESSAGE_PARAMTYPE_RT;
	pointer = 3;

	buff[pointer] = (U8)(rid >> 8);
	pointer++;

	buff[pointer] = (U8)rid;
	pointer++;
	
	buff[1] = (U8)(pointer >> 8);
    buff[2] = (U8)pointer;
	return(pointer);
	
}

U16 fnRTM_GetParam_SYSINFO(U8 *buff)
{
	U16 pointer;
	
	buff[0] = MESSAGE_PARAMTYPE_IN_SYSINFO;
	pointer = 3;

	buff[pointer] = 32;
	pointer++;

	buff[pointer] = 8;
	pointer++;

	buff[pointer] = (U8)(500 >> 8);
	pointer++;

	buff[pointer] = (U8)(500);
	pointer++;

	buff[pointer] = (U8)(300 >> 8);
	pointer++;

	buff[pointer] = (U8)(300);
	pointer++;

	

	buff[pointer] = 0;
	pointer++;

	buff[pointer] = 0;
	pointer++;

	buff[pointer] = 0;
	pointer++;

	buff[pointer] = 0;
	pointer++;

	buff[pointer] = 0;
	pointer++;
	
	buff[1] = (U8)(pointer >> 8);
    buff[2] = (U8)pointer;
	return(pointer);
	
}

U16 fnRTM_GetParam_MotorEn(U8 *buff)
{
	U16 pointer;
	U8 motorflag;
	buff[0] = MESSAGE_PARAMTYPE_IN_MOTOREN;
	pointer = 3;

  motorflag = 0;
	buff[pointer] = motorflag;
	pointer ++;
	
	buff[1] = (U8)(pointer >> 8);
  buff[2] = (U8)pointer;
	
	return pointer;
}
U16 fnRTM_GetParam_DevName(U8 *buff)
{
	U16 pointer;
	buff[0] = MESSAGE_PARAMTYPE_IN_DEVNAME;
	pointer = 3;
	
	buff[1] = (U8)(pointer >> 8);
  buff[2] = (U8)pointer;
	return(pointer);
	
}


/*
method 1: get all
params: 1: i, word
params 2: o, word
params 3: v 4 byte array
params 4, ad, 2 byte array
params 5 tm, 5 byte array
params 6, r, 3 byte array
params 7, runmode, 1byte
params 8, dy, 4 byte array
*/


const char g_hp_json_1[] = {"{\"a\":\"get\",\"t\":\"all\",\"i\":"};
const char g_hp_json_2[] = {",\"o\":"};

const char g_hp_json_3[] = {",\"v\":\""};

const char g_hp_json_4[] = {"\",\"ad\":\""};
const char g_hp_json_5[] = {"\",\"tm\":\""};
const char g_hp_json_6[] = {"\",\"r\":\""};
const char g_hp_json_7[] = {"\",\"runmode\":"};
const char g_hp_json_8[] = {",\"dy\":\""};

const char g_hp_json_9[] = {"\""};

U16 fnRTM_GetAll(U8 method, U8 *msg)
{
	U16 pointer = 0;
	g_rtm_stRegs.m_uaBuff[pointer] = method;
	pointer++;
	pointer += fnRTM_GetParam_Input(&g_rtm_stRegs.m_uaBuff[pointer]);
	pointer += fnRTM_GetParam_Output(&g_rtm_stRegs.m_uaBuff[pointer]);
	pointer += fnRTM_GetParam_VARS(&g_rtm_stRegs.m_uaBuff[pointer]);
	pointer += fnRTM_GetParam_AD(&g_rtm_stRegs.m_uaBuff[pointer]);
	pointer += fnRTM_GetParam_TM(&g_rtm_stRegs.m_uaBuff[pointer]);
	pointer += fnRTM_GetParam_RoutineInfo(&g_rtm_stRegs.m_uaBuff[pointer]);
	pointer += fnRTM_GetParam_RunMode(&g_rtm_stRegs.m_uaBuff[pointer]);
	pointer += fnRTM_GetParam_DY(&g_rtm_stRegs.m_uaBuff[pointer]);
	return(pointer);
}
/*
	method, get rt
	request params:
		1-rid: 2bytes
	response params:
		1-stepnum, 2bytes
		2-ins,1 byte array, 1024 lengh
		3:run 1 byte
*/
const char g_hp_getrt_1[] = {"{\"a\":\"get\",\"t\":\"rt\",\"rid\":"};
const char g_hp_getrt_2[] = {",\"stepnum\":"};
const char g_hp_getrt_3[] = {",\"ins\":\""};
const char g_hp_getrt_4[] = {"\",\"run\":"};

U16 fnRTM_GetRT(U8 method, U8 *msg)
{
	U16 pointer = 0;
	U32 lwrt;
	U8 lres;
	U16 rt;
	lres = rpc_getvaluebytype(MESSAGE_PARAMTYPE_IN_RT, msg, &lwrt);
	g_rtm_stRegs.m_uaBuff[pointer] = method;
	pointer++;
	if(lres)
	{
		rt = (U16)lwrt;
		pointer += fnRTM_GetParam_RT(&g_rtm_stRegs.m_uaBuff[pointer], rt);
		pointer += fnRTM_GetParam_STEPNUM(&g_rtm_stRegs.m_uaBuff[pointer], rt);
		pointer += fnRTM_GetParam_INS(&g_rtm_stRegs.m_uaBuff[pointer], rt);
		pointer += fnRTM_GetParam_RUN(&g_rtm_stRegs.m_uaBuff[pointer], rt);
	}
	return(pointer);
	
}

const char g_hp_delrt_1[] = {"{\"a\":\"save\",\"t\":\"delrt\",\"rid\":"};
const char g_hp_delrt_2[] = {",\"result\":"};

U16 fnRTM_DelRT(U8 method, U8 *msg)
{

	U16 pointer = 0;
	return(pointer);
	
}

#define RT_SAVE_PART_DURATION (1000 / 10)
const char g_hp_savert_1[] = {"{\"a\":\"save\",\"t\":\"rt\",\"rid\":"};
const char g_hp_savert_2[] = {",\"result\":"};

U16 fnRTM_SaveRT(U8 method, U8 *msg)
{
	U16 pointer = 0;

	
	return(pointer);
	
}
const char g_hp_saverun_1[] = {"{\"a\":\"save\",\"t\":\"run\",\"rid\":"};
const char g_hp_saverun_2[] = {",\"result\":"};
U16 fnRTM_SaveRun(U8 method, U8 *msg){
	U16 pointer = 0;
	return(pointer);
}
const char g_hp_getsys_1[] = "\"year\":";
const char g_hp_getsys_2[] = ",\"month\":";
const char g_hp_getsys_3[] = ",\"day\":";
const char g_hp_getsys_4[] = ",\"hour\":";
const char g_hp_getsys_5[] = ",\"minute\":";
const char g_hp_getsys_6[] = ",\"second\":";
const char g_hp_getsys_7[] = ",\"name\":\"";
const char g_hp_getsys_8[] = "\",\"addr\":";

const char g_hp_getsys_9[] = {"&vars="};



U16 fnRTM_GetSys(U8 method, U8 *msg)
{
	U16 pointer = 0;
	U16 len;
	g_rtm_stRegs.m_uaBuff[pointer] = method;
	pointer ++;

	g_rtm_stRegs.m_uaBuff[pointer] = MESSAGE_PARAMTYPE_OTHER;
	pointer ++;

	pointer += 2;

	
	memcpy(&g_rtm_stRegs.m_uaBuff[pointer], g_hp_getsys_1, strlen(g_hp_getsys_1));
	pointer += strlen(g_hp_getsys_1);


	pointer = fnSTR_DecToStr((char *)g_rtm_stRegs.m_uaBuff, pointer,0 , 0);

	
	memcpy(&g_rtm_stRegs.m_uaBuff[pointer], g_hp_getsys_2, strlen(g_hp_getsys_2));
	pointer += strlen(g_hp_getsys_2);
	pointer = fnSTR_DecToStr((char *)g_rtm_stRegs.m_uaBuff, pointer,0 , 0);
	
	memcpy(&g_rtm_stRegs.m_uaBuff[pointer], g_hp_getsys_3, strlen(g_hp_getsys_3));
	pointer += strlen(g_hp_getsys_3);
	pointer = fnSTR_DecToStr((char *)g_rtm_stRegs.m_uaBuff, pointer,0 , 0);
	
	memcpy(&g_rtm_stRegs.m_uaBuff[pointer], g_hp_getsys_4, strlen(g_hp_getsys_4));
	pointer += strlen(g_hp_getsys_4);
	pointer = fnSTR_DecToStr((char *)g_rtm_stRegs.m_uaBuff, pointer,0 , 0);
	
	memcpy(&g_rtm_stRegs.m_uaBuff[pointer], g_hp_getsys_5, strlen(g_hp_getsys_5));
	pointer += strlen(g_hp_getsys_5);
	pointer = fnSTR_DecToStr((char *)g_rtm_stRegs.m_uaBuff, pointer,0 , 0);
	
	memcpy(&g_rtm_stRegs.m_uaBuff[pointer], g_hp_getsys_6, strlen(g_hp_getsys_6));
	pointer += strlen(g_hp_getsys_6);
	pointer = fnSTR_DecToStr((char *)g_rtm_stRegs.m_uaBuff, pointer,0 , 0);

	memcpy(&g_rtm_stRegs.m_uaBuff[pointer], g_hp_getsys_7, strlen(g_hp_getsys_7));
	pointer += strlen(g_hp_getsys_7);
	///fnAP_GetName((char *)&g_rtm_stRegs.m_uaBuff[pointer], &pointer);

	memcpy(&g_rtm_stRegs.m_uaBuff[pointer], g_hp_getsys_8, strlen(g_hp_getsys_8));
	pointer += strlen(g_hp_getsys_8);
	pointer = fnSTR_DecToStr((char *)g_rtm_stRegs.m_uaBuff, pointer,0, 0);

	len = pointer -1;
	g_rtm_stRegs.m_uaBuff[2] = (U8)(len >> 8);
	g_rtm_stRegs.m_uaBuff[3] = (U8)(len);
	
	
    pointer += fnRTM_GetParam_VOLATILEVARS(&g_rtm_stRegs.m_uaBuff[pointer]);
	pointer += fnRTM_GetParam_FlashVar(&g_rtm_stRegs.m_uaBuff[pointer]);

	return(pointer);
}

#define RTM_SYS_PARAM_LEN 100
char rtm_param[RTM_SYS_PARAM_LEN];
void fnRTM_SaveXVar(U16 addr, U32 value)
{
	
	
}
U16 fnRTM_SaveVar(U8 method, U8 *msg)
{
	U16 pointer = 0;
	return(pointer);
}


U16 fnRTM_SaveSys(U8 method, U8 *msg)
{

	U16 pointer;

	

	pointer = fnRTM_GetSys(MESSAGE_METHOD_NAME_GETSYS, msg);
	return(pointer);
}
#define CONPC_RESETALL  1
#define CONPC_CHANGEMODE 2
U16 fnRTM_ChangeMode(U8 method, U8 *msg)
{
	U16 pointer = 0;
	U8 result = 0;
	g_rtm_stRegs.m_uaBuff[pointer] = method;
	pointer ++;
	pointer += fnRTM_GetParam_RunMode(&g_rtm_stRegs.m_uaBuff[pointer]);
	pointer += fnRTM_GetParam_RESULT(&g_rtm_stRegs.m_uaBuff[pointer], result);
	return(pointer);
}
U16 fnRTM_SaveFlash(U8 method, U8 *msg)
{
	return 0;
}

U16 fnRTM_GetSysInfo(U8 method, U8 *msg)
{
	U16 pointer = 0;
	g_rtm_stRegs.m_uaBuff[pointer] = method;
	pointer++;
	pointer += fnRTM_GetParam_SYSINFO(&g_rtm_stRegs.m_uaBuff[pointer]);
	pointer += fnRTM_GetParam_DevName(&g_rtm_stRegs.m_uaBuff[pointer]);
	pointer += fnRTM_GetParam_MotorEn(&g_rtm_stRegs.m_uaBuff[pointer]);
	return(pointer);
}
U16 fnRTM_GetParam_Binary(U8 *buff, U8 *binary, U16 len)
{
	U16 pointer = 0;
	return(pointer);
}
U16 fnRTM_SaveBinary(U8 method, U8 *msg)
{
	U16 pointer = 0;
	
	return(pointer);
}

U16 fnRTM_GetProgram(U8 method, U8 *msg)
{
	U16 pointer = 0;
	return(pointer);
}
#define RTM_SENDSTATE_IDLE 0
#define RTM_SENDSTATE_SEND 1
U8 fnRTM_ResetServer(void)
{
	U8 *buff;
	U8 res = FALSE;
	if((FALSE == fnWFUA_IsSend())
		&& (RTM_SENDSTATE_IDLE == g_rtm_stRegs.m_uchSendState))
	{
		buff = (U8 *)&g_wfua_stSendRegs.m_uchBuff[6];
		buff[0] = MESSAGE_METHOD_NAME_RESET;
		fnWFUA_SendReq(0, 1);
		res = TRUE;
	}
	return(res);
}

U8 fnRTM_PingServer(void)
{
	U8 *buff;
	U8 res = FALSE;
	U16 pointer = 0;
	if((FALSE == fnWFUA_IsSend())
		&& (RTM_SENDSTATE_IDLE == g_rtm_stRegs.m_uchSendState))
	{
		buff = (U8 *)&g_wfua_stSendRegs.m_uchBuff[6];
		
		buff[0] = MESSAGE_METHOD_NAME_PING;
		pointer ++;
		
		fnAP_GetID((char *)&buff[pointer], &pointer);

		buff[pointer] = 0;
		pointer++;
		
		fnWFUA_SendReq(0, pointer);
		res = TRUE;
	}
	return(res);
}


void fnRTM_ReqSend(U8 msgid, U16 len)
{
	fnWFUA_SendReq(msgid | 0x8000, len);
}
void fnRTM_Comm(void)
{
	U8 i, method;
	U16 pointer, len;
	U16 msgid; 
	if(RTM_SENDSTATE_IDLE == g_rtm_stRegs.m_uchSendState)
	{
		if(g_wfua_stRecRegs.m_uchReceived)
		{
			msgid = (U16)g_wfua_stRecRegs.m_uchBuff[0] << 8;
			msgid |= (U16)g_wfua_stRecRegs.m_uchBuff[1];
			if((0 != (msgid & 0x8000))
				|| (FALSE == fnWFUA_IsSend()))
			{
				method = g_wfua_stRecRegs.m_uchBuff[4];
				g_rtm_stRegs.m_uchMethod = method;
				
				len = (U16)g_wfua_stRecRegs.m_uchBuff[2] << 8;
				len |= (U16)g_wfua_stRecRegs.m_uchBuff[3];
				g_rtm_stRegs.m_uchLen = len;
				
				for(i = 0; i < RPC_FUN_NUM; i++)
				{
					if(method == rpcmethods[i].method)
					{
						pointer = rpcmethods[i].fun(method, (U8 *)&g_wfua_stRecRegs.m_uchBuff[5]);
						if(pointer > 0)
						{
							fnRTM_ReqSend(msgid, pointer);
							g_rtm_stRegs.m_uchSendState = RTM_SENDSTATE_SEND;
							
						}
						break;
					}
				}
			}
			g_wfua_stRecRegs.m_uchReceived = FALSE;
			
		}
	}
	else if(RTM_SENDSTATE_SEND == g_rtm_stRegs.m_uchSendState)
	{
		if(FALSE == fnWFUA_IsSend())
		{
			g_rtm_stRegs.m_uchSendState = RTM_SENDSTATE_IDLE;
		}
	}
	
}

void fnRTM_RealTime(void)
{
	fnRTM_Comm();
	if(g_tm_stTimerFlag.Bits.bTimer100ms){
		if(g_rtm_stBuff.m_uchTimer)
			g_rtm_stBuff.m_uchTimer --;
		if(g_rtm_stRegs.m_uchTimer)
			g_rtm_stRegs.m_uchTimer --;
	}
}
void fnRTM_Init(void){
	g_rtm_stBuff.m_uchTimer = 0;
	memset(&g_rtm_stRegs, 0, sizeof(g_rtm_stRegs));
	g_rtm_stRegs.m_uaBuff = (U8 *)&g_wfua_stSendRegs.m_uchBuff[6];
}

U8 fnRTM_isDiag(U8 cmd){
		U8 res = FALSE;
	 if(cmd == MESSAGE_METHOD_NAME_DIAG){
		 res = TRUE;
	 }
	 return(res);
}


