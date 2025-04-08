#include "transmit.h"
#include "timer.h"
#include "adc.h"
#include "DMA.h"
#include "rpc.h"
#include "filter.h"
#include "fft.h"
#include "data.h"
#include "baoluo.h"
#include "DMAWiFi.h"
#include "UartWiFi.h"
#include "math.h"
trs_regs_typedef trs_regs;
U8 fnTRS_ReqRawData(void *addr, U16 datatype, U8 chind, U8 chnum, U16 totalbytelen){
	U8 res = FALSE;
	if(trs_regs.state == TRS_STATE_IDLE){
		trs_regs.datatype = datatype;
		trs_regs.addr = addr;
		trs_regs.channel = chind;
		trs_regs.substate = TRS_STATE_TRANSMIT_INIT;
		trs_regs.pointer = 0;
		trs_regs.state = TRS_STATE_TRANSMIT;
		trs_regs.totalbytelen = totalbytelen;
		trs_regs.sendfinish = FALSE;
		res = TRUE;
	}
	return(res);
}
U16 fnTRS_GenPayload(U16 msgid, void *addr, U16 datatype, U8 channelnum, U8 chind, U16 totalbytelen, U16 npos, U8 *sendbuff, U16 buffsize, U16 *sendpointer){
	U16 pointer;
	U16 sendpack,sendbytes;
	U16 data, rlen;
	U16 i;
	U32 off;
	int idata;
	U16 crc = 0;
	FLBLInfoDef *blinfo;
	UFloatTypeDef ftdata;
	float ftvalue;
	static U16 lmsgid =0;
	Complex *complex;
	*sendpointer = 0;
	sendpack = 0;
	sendbytes = 0;
	
	if(datatype == TRS_DATATYPE_AVS){
		sendpack = 3;
		sendbytes = totalbytelen;
		
		rlen = 0;
		if(totalbytelen > npos){
			rlen = totalbytelen - npos;
		}
		if(sendbytes > rlen){
			sendbytes = rlen;
		}
		if(sendbytes == 0){
			sendpack = 0;
		}
	}else{
		sendpack = (buffsize - 22) >> 1;
		sendbytes = sendpack * 2;
		rlen = 0;
		if(totalbytelen > npos){
			rlen = totalbytelen - npos;
		}
		if(sendbytes > rlen){
			sendbytes = rlen;
		}
		sendpack = sendbytes >> 1;
		
	}

	if(sendpack == 0){
		return 0;
	}
	sendbuff[0] = 0;
	if(datatype == TRS_DATATYPE_RAWDATA){
		sendbuff[0] = MESSAGE_PARAMTYPE_IN_DATA_RAW;
	}
	if(datatype == TRS_DATATYPE_AVS){
		sendbuff[0] = MESSAGE_PARAMTYPE_IN_DATA_AVS;
	}
	if(datatype == TRS_DATATYPE_BAOLUO){
		sendbuff[0] = MESSAGE_PARAMTYPE_IN_DATA_BAOLUO;
	}
	if(datatype == TRS_DATATYPE_BAOLUOFFT){
		sendbuff[0] = MESSAGE_PARAMTYPE_IN_DATA_BAOLUOFFT;
	}

	pointer = 3;
	sendbuff[3] = (lmsgid >> 8)&0xff;
	sendbuff[4] = (lmsgid)&0xff;
	lmsgid++;
	
	sendbuff[5] = (datatype >> 8)&0xff;
	sendbuff[6] = (datatype)&0xff;
	sendbuff[7] = chind;
	
	sendbuff[8] = (totalbytelen >> 8)&0xff;
	sendbuff[9] = (totalbytelen)&0xff;
	sendbuff[10] = (npos >> 8)&0xff;
	sendbuff[11] = (npos)&0xff;
	pointer = 12;
	
	
	
	sendbuff[pointer] = (sendbytes >> 8)&0xff;
	sendbuff[pointer + 1] = (sendbytes)&0xff;
	pointer += 2;
	if(datatype == TRS_DATATYPE_RAWDATA){
		for(i = 0; i < sendpack; i++){
			off =  (U32)(i + (npos >> 1)) * channelnum + (U32)chind; 
			data = *((signed short *)addr + off);//simu_data[i +(npos >> 1)];/
			//data = bldet_bl_calmem.bldet_tempbuff[i +(npos >> 1)];
			sendbuff[pointer] = (data>> 8)&0xff;
			sendbuff[pointer + 1] = (data)&0xff;
			crc = crc + data;
			pointer += 2;
			*sendpointer += 2;
		}
	}
	if(datatype == TRS_DATATYPE_AVS){
		blinfo = (FLBLInfoDef *)addr;
		ftdata.val = blinfo->temp;
		idata =  (int)(ftdata.val * 100);
		sendbuff[pointer] =  (idata >> 24) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 16) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 8) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata ) & 0xff;
		pointer ++;
		

		ftdata.val = blinfo->maxa;
		idata =  (int)(ftdata.val * 100);
		sendbuff[pointer] =  (idata >> 24) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 16) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 8) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata ) & 0xff;
		pointer ++;
		
		ftdata.val = blinfo->mina;
		idata =  (int)(ftdata.val * 100);
		sendbuff[pointer] =  (idata >> 24) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 16) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 8) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata ) & 0xff;
		pointer ++;
		
		ftdata.val = blinfo->pa;
		idata =  (int)(ftdata.val * 100);
		sendbuff[pointer] =  (idata >> 24) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 16) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 8) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata ) & 0xff;
		pointer ++;
		
		ftdata.val = blinfo->ra;
		idata =  (int)(ftdata.val * 100);
		sendbuff[pointer] =  (idata >> 24) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 16) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 8) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata ) & 0xff;
		pointer ++;
		
		ftdata.val = blinfo->maxv;
		idata =  (int)(ftdata.val * 100);
		sendbuff[pointer] =  (idata >> 24) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 16) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 8) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata ) & 0xff;
		pointer ++;
		
		ftdata.val = blinfo->minv;
		idata =  (int)(ftdata.val * 100);
		sendbuff[pointer] =  (idata >> 24) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 16) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 8) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata ) & 0xff;
		pointer ++;
		
		ftdata.val = blinfo->pv;
		idata =  (int)(ftdata.val * 100);
		sendbuff[pointer] =  (idata >> 24) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 16) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 8) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata ) & 0xff;
		pointer ++;
		
		ftdata.val = blinfo->rv;
		idata =  (int)(ftdata.val * 100);
		sendbuff[pointer] =  (idata >> 24) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 16) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 8) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata ) & 0xff;
		pointer ++;
		
		ftdata.val = blinfo->maxd;
		idata =  (int)(ftdata.val * 100);
		sendbuff[pointer] =  (idata >> 24) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 16) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 8) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata ) & 0xff;
		pointer ++;
		
		ftdata.val = blinfo->mind;
		idata =  (int)(ftdata.val * 100);
		sendbuff[pointer] =  (idata >> 24) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 16) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 8) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata ) & 0xff;
		pointer ++;
		
		ftdata.val = blinfo->pd;
		idata =  (int)(ftdata.val * 100);
		sendbuff[pointer] =  (idata >> 24) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 16) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 8) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata ) & 0xff;
		pointer ++;
		
		ftdata.val = blinfo->rd;
		idata =  (int)(ftdata.val * 100);
		sendbuff[pointer] =  (idata >> 24) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 16) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 8) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata ) & 0xff;
		pointer ++;
		
		ftdata.val = blinfo->pen;
		idata =  (int)(ftdata.val * 100);
		sendbuff[pointer] =  (idata >> 24) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 16) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 8) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata ) & 0xff;
		pointer ++;
		
		
		ftdata.val = blinfo->ren;
		idata =  (int)(ftdata.val * 100);
		sendbuff[pointer] =  (idata >> 24) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 16) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 8) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata ) & 0xff;
		pointer ++;
		
		ftdata.val = blinfo->pacc;
		idata =  (int)(ftdata.val * 100);
		sendbuff[pointer] =  (idata >> 24) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 16) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 8) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata ) & 0xff;
		pointer ++;
		
		
		ftdata.val = blinfo->racc;
		idata =  (int)(ftdata.val * 100);
		sendbuff[pointer] =  (idata >> 24) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 16) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata >> 8) & 0xff;
		pointer ++;
		sendbuff[pointer] = (idata ) & 0xff;
		pointer ++;
		
		*sendpointer += 3 * 4 * 4 + (1 + 2 + 2)*4;
	}
	if(datatype == TRS_DATATYPE_BAOLUO){
		for(i = 0; i < sendpack; i++){
			off = (U32)(i + (npos >> 1)); 
			ftvalue = *((float *)addr + off);
			data = 65535;
			if(ftvalue < 65535){
				data = (U16)ftvalue;
			}
			crc = crc + data;
			sendbuff[pointer] = (data>> 8)&0xff;
			sendbuff[pointer + 1] = (data)&0xff;
			pointer += 2;
			*sendpointer += 2;
		}
	}
	if(datatype == TRS_DATATYPE_BAOLUOFFT){
		for(i = 0; i < sendpack; i++){
			off = (U32)(i + (npos >> 1)); 
			complex = ((Complex *)addr + off);
			ftvalue = sqrt(complex->real * complex->real + complex->imag * complex->imag);
			ftvalue = ftvalue * 2 / 8192;
			data = 65535;
			if(ftvalue < 65535){ 
				data = (U16)ftvalue;
			}
			crc = crc + data;
			sendbuff[pointer] = (data>> 8)&0xff;
			sendbuff[pointer + 1] = (data)&0xff;
			pointer += 2;
			*sendpointer += 2;
		}
	}
	sendbuff[pointer] = (U8)(crc >> 8);
	sendbuff[pointer + 1] = (U8)(crc & 0xff);
	pointer += 2;
	sendbuff[1] = (pointer >> 8)&0xff;
	sendbuff[2] = (pointer)&0xff;
	return(pointer);
}
void fnTRS_RealTime(void){
	U8 *sendbuff;
	U16 buffsize;
	U16 sendpointer;
	U16 pointer;
	U8 finish;
	U8 success;
	static U16 lmsgid = 0;
	if(g_tm_stTimerFlag.Bits.bTimer16ms){
		if(trs_regs.subtimer > 0){
			trs_regs.subtimer --;
		}
	}
	if(trs_regs.state == TRS_STATE_IDLE){
	}
	else if(trs_regs.state == TRS_STATE_TRANSMIT){
		if(trs_regs.substate == TRS_STATE_TRANSMIT_INIT){
#if(TRS_TRANSMIT_BYWIFI)
			if(fnWFDMA_SendBaoLuoEn()){
#else
			if(fnDMA_UART_IsTXIdle()){
#endif			
#if(TRS_TRANSMIT_BYWIFI)
				fnDMA_GetSendBuff(&sendbuff,  &buffsize);
				sendbuff[0] = 0xFF;
				sendbuff[3] = 0x5A;
				sendbuff[4] = 0xA5;
				
				
				pointer = fnTRS_GenPayload(trs_regs.msgid, trs_regs.addr, trs_regs.datatype, ADC_CHANNEL_NUM,trs_regs.channel, trs_regs.totalbytelen, trs_regs.pointer, &sendbuff[11], buffsize - 4, &sendpointer);
				sendbuff[1] = (U8)(((pointer + 12) >> 8) & 0xff);
				sendbuff[2] = (U8)((pointer + 12) & 0xff);
				sendbuff[5] = 0x00;
				sendbuff[6] = 88;
				
				
				sendbuff[7] = (U8)g_bldet_regs.testid;
				sendbuff[8] = (U8)(lmsgid) & 0xff;
				sendbuff[9] = (U8)((pointer >> 8) & 0xff);
				sendbuff[10] = (U8)(pointer & 0xff);
				
				sendbuff[pointer + 11] = 0x85;
				sendbuff[pointer + 12] = 0x0A;
	#else
				fnDMA_GetSendBuff(&sendbuff,  &buffsize);
				sendbuff[0] = 0x55;
				sendbuff[1] = 0xFF;
				
				
				pointer = fnTRS_GenPayload(trs_regs.msgid, trs_regs.addr, trs_regs.datatype, ADC_CHANNEL_NUM,trs_regs.channel, trs_regs.totalbytelen, trs_regs.pointer, &sendbuff[4], buffsize - 4, &sendpointer);
				
				sendbuff[2] = (U8)((pointer >> 8) & 0xff);
				sendbuff[3] = (U8)(pointer & 0xff);
#endif
				if(0 == pointer){
					trs_regs.sendfinish = TRUE;
					trs_regs.state = TRS_STATE_IDLE;
				}else{
#if(TRS_TRANSMIT_BYWIFI)
					fnWFUA_SendReqEx(lmsgid, &sendbuff[0], pointer + 13);
					lmsgid++;
#else
					fnDMA_UARTTxStart(pointer + 4);
#endif
					trs_regs.alreadysendcount = sendpointer;
					trs_regs.substate = TRS_STATE_TRANSMIT_WAIT;
					trs_regs.subtimer = 10;//ZZY 20211022 5;
					trs_regs.sendsuccess = FALSE;
					trs_regs.trytimes = 0;
				}
			}	
		}else if(trs_regs.substate == TRS_STATE_TRANSMIT_WAIT){
			finish = FALSE;
			success = FALSE;
			if(trs_regs.subtimer == 0){
				trs_regs.trytimes ++;
				if(trs_regs.trytimes >= 1){//2021122 ZZY > 3){
					success = TRUE;
				}
				finish = TRUE;
			}
			if(trs_regs.sendsuccess){
				success = TRUE;
				finish = TRUE;
			}
			if(finish){
				if(success){
					trs_regs.pointer += trs_regs.alreadysendcount;
	
				}
				trs_regs.substate = TRS_STATE_TRANSMIT_INIT;
			}
		}
	}
}
void fnTRS_Init(void){
	memset(&trs_regs, 0, sizeof(trs_regs));
}
U8 fnTRS_IsFinish(void){
	U8 res = FALSE;
	if(trs_regs.sendfinish){
		res = TRUE;
	}
	return(res);
}
