#ifndef _H_FILTER_
#define _H_FILTER_
#include "define.h"
#include "fft.h"
#define SAMPLE_DATA_NUM 8192
#define FILTER_COEF_NUM 49
typedef struct{
	float temp;
	float mina;
	float maxa;
	float ra;
	float pa;
	float minv;
	float maxv;
	float rv;
	float pv;
	float mind;
	float maxd;
	float rd;
	float pd;
	float ren;
	float pen;
	float racc;
	float pacc;
}FLBLInfoDef;
extern void fnFL_FilterAll(signed short *data, signed short *tempbuff, U8 channelnum);

extern void fnFL_GetSample(float *chdata, signed short *data, U8 channelnum, U8 chind);
extern void fnFL_CalABS(float *data);
extern float fnFL_CalTC(float *data, U32 samplerate,U32 samplenum);
extern void fnFL_CalBLEx(float *data, U32 samplerate, U32 samplenum, float tc);
extern void fnFL_CalACC(float *acc, int samplenum, int fs, FLBLInfoDef *blinfo);
#endif
