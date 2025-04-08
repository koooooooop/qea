#include "define.h"
#include "Vars.h"
#include "ADC.h"
#include "temperature.h"
#include "switch.h"
#include "IoControl.h"
#include "timer.h"
#include "RTC.h"
int g_ct_uiVarComp[CT_VAR_NUM];

#define VAR_DEFAULTVALUE_NUM 4
const U16 var_defaultvalue[][2] = {
	{VARS_VOLATILE_OFFSET_MB_ADDR, 1},
	{VARS_VOLATILE_OFFSET_MB_BAUDRATE, 1},
	{VARS_VOLATILE_OFFSET_MB_MASTER, 1},

	{VARS_VOLATILE_OFFSET_MB_SENSORNUM, 0}
};

void fnVARS_SetDefault(void)
{
	U16 i, j;
	U16 defvalue = 0;
	for(i = 0; i< VARS_VOLATILE_NUM; i++)
	{
		defvalue = 0;
		for(j = 0; j<VAR_DEFAULTVALUE_NUM; j++){
				if(var_defaultvalue[j][0] == i){
					defvalue = var_defaultvalue[j][1];
				}
		}
		g_ct_uiVarComp[VARS_VOLATILE_OFFSET + i] = defvalue;
		fnKD_WriteData(KD_DATA_USERAREA_LOW + i,defvalue);
	}
}
void fnVARS_GetMemory(void)
{
	U16 i;
	//memory
	
	for(i = 0; i< VARS_VOLATILE_NUM; i++)
	{
		g_ct_uiVarComp[VARS_VOLATILE_OFFSET + i] = (U32)KD_GET_DATA(KD_DATA_USERAREA_LOW + i);
	}
	
	for(i = 0; i< VARS_BACKUP_NUM; i++)
	{
	    g_ct_uiVarComp[VARS_BACKUP_OFFSET + i] = fnRTC_GetBackUP(i);
	}
	//memory
}

void fnVARS_SetMemory(void)
{
	U16 i;
	//memory
	
	for(i = 0; i< VARS_VOLATILE_NUM; i++)
	{
		fnKD_WriteData(KD_DATA_USERAREA_LOW + i, (U16)g_ct_uiVarComp[VARS_VOLATILE_OFFSET + i]);
	}

	
	//memory
}
void fnVARS_SetBackup(void)
{
	U16 i;
	for(i = 0; i< VARS_BACKUP_NUM; i++)
	{
		fnRTC_SaveBackUP(i, g_ct_uiVarComp[VARS_BACKUP_OFFSET + i]);
	}
}

void fnVARS_GetSystem(void)
{
	U16 i;
	//system
	for(i = 0; i< AD_CHANNEL_NUM; i++)
	{
		g_ct_uiVarComp[VARS_SYSTEM_OFFSET_AD0 + i] = (U32)AD_GET_VAL(i);
	}
	//g_var_stRegs[VARS_ADREAL_LOW] = ps;
	for(i = 0; i< AD_CHANNEL_NUM; i++)
	{
		g_ct_uiVarComp[VARS_SYSTEM_OFFSET_TEMPERUATE + i] = (U32)g_tp_stRegs.m_uiTemp[i];
	}

	//system
}
void fnVARS_Init(void)
{
	memset(&g_ct_uiVarComp, 0, sizeof(g_ct_uiVarComp));
	fnVARS_GetSystem();
	fnVARS_GetMemory();
}
void fnVARS_RealTime(void)
{
	
	
	fnVARS_GetSystem();
	if(g_tm_stTimerFlag.Bits.bTimer1min)
	{
		fnVARS_SetMemory();
		fnVARS_GetMemory();
	}
	fnVARS_SetBackup();

	if(g_tm_stTimerFlag.Bits.bTimer10ms)
	{
		CT_WRITE_VAR(VARS_10MSEVENT, TRUE);
	}
	if(g_tm_stTimerFlag.Bits.bTimer100ms)
	{
		CT_WRITE_VAR(VARS_100MSEVENT, TRUE);
	}
	
	
}
void fnVARS_Save(U32 varid, U32 value)
{
	if(varid < CT_VAR_NUM)
	{
		g_ct_uiVarComp[varid] = value;
	}
	fnVARS_SetMemory();
}


