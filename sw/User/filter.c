#include "Adc.h"
#include "IO.h" 
#include "Timer.h" 
#include "math.h"
#include "filter.h"
#include "baoluo.h"
#include "data.h"

const signed short filter_coef[FILTER_COEF_NUM]={
	13,
	6,
	4,
	33,
	1,
	44,
	41,
	4,
	103,
	-25,
	46,
	52,
	-163,
	88,
	-260,
	-225,
	-75,
	-740,
	-70,
	-715,
	-892,
	218,
	-2267,
	1257,
	7027,
	1257,
	-2267,
	218,
	-892,
	-715,
	-70,
	-740,
	-75,
	-225,
	-260,
	88,
	-163,
	52,
	46,
	-25,
	103,
	4,
	41,
	44,
	1,
	33,
	4,
	6,
	13
};
signed short fnFL_GetValue(signed short *data, int adind, U8 channelnum, U8 chind){
	signed short res = 0;
	U32 ind;
	ind = (U32)channelnum * adind;
	ind += (U32)chind;
	res = data[ind];
	//res = simu_data[adind];
	return(res);
}

void fnFL_SetValue(signed short *data, int adind, U8 channelnum, U8 chind, signed short value){
	U32 ind;
	ind = (U32)channelnum * adind;
	ind += (U32)chind;
	//data[adind] = value;
       data[ind] = value;

}
#define  _DIGITALFILTER FALSE
void fnFL_FilterEX(signed short *data, signed short *tempbuff, U8 channelnum, U8 chind){
	int n,m,i;
	int k;
	signed long long sumv;
	int avgv;
	int wdata;
	n = SAMPLE_DATA_NUM;
	m = FILTER_COEF_NUM;
	sumv = 0;
	for(i = 0; i < n; i++){
		//fnFL_SetValue(data, i, channelnum, chind, simu_data[i]);
		tempbuff[i] = fnFL_GetValue(data, i, channelnum, chind);
		sumv += tempbuff[i];
	}
	sumv = sumv / n;
	avgv = (int)sumv;
	for(i = 0; i< n; i++){
		wdata = tempbuff[i];
		wdata = wdata - avgv;
		tempbuff[i] = wdata;
	}
}
void fnFL_Filter(signed short *data, signed short *tempbuff, U8 channelnum, U8 chind){
	U16 n, k, nm, kmax, i,m;
	int wdata;
	signed short uidata;
	n = SAMPLE_DATA_NUM + FILTER_COEF_NUM;
	m = FILTER_COEF_NUM;
	for(i = 0; i < SAMPLE_DATA_NUM; i++){
		//fnFL_SetValue(data, i, channelnum, chind, simu_data[i]);
		tempbuff[i] = fnFL_GetValue(data, i, channelnum, chind) - 2048;//simu_data[i] - 2048;//
		
		//ad * 3300 /4096/210*980 210mV/g, unit:*100m/s^2
	}
#if(_DIGITALFILTER)
	for(i = SAMPLE_DATA_NUM; i< SAMPLE_DATA_NUM + FILTER_COEF_NUM; i++){
		tempbuff[i] = 0;
	}
	for(i = 0; i< n; i++){
		wdata = 0;
		nm = m -2;
		kmax = i;
		if(nm > kmax){
			nm = kmax;
		}
		for(k = 0; k < nm; k++){
			wdata = wdata + (int)filter_coef[k] * tempbuff[i-k] / 10000;//fnFL_GetValue(data, i - k, channelnum, chind); //(U32)filter_coef[k]* simu_data[i-k];//
			//wdata = wdata + (U32)filter_coef[k]* simu_data[i-k];//
		}
		tempbuff[i] = wdata;

	}
#endif	
	for(i = 0; i< SAMPLE_DATA_NUM; i++){
		fnFL_SetValue(data, i, channelnum, chind, (signed short)tempbuff[i]);
	}
}

void fnFL_FilterAll(signed short *data, signed short *tempbuff, U8 channelnum){
	U8 i;
	for(i = 0; i< channelnum; i++){
		fnFL_Filter(data, tempbuff, channelnum, i);
	}
}



void fnFL_GetSample(float *chdata, signed short *data, U8 channelnum, U8 chind){
	int i;
	float wdata;
	signed short uidata;
	for(i = 0; i < SAMPLE_DATA_NUM; i++){
		//chdata[i] = (float)fnFL_GetValue(data, i, channelnum, chind);//simu_data[i];//
		wdata = (float)fnFL_GetValue(data, i, channelnum, chind);
		wdata = wdata * 402.01;//37598;
		wdata = wdata / 10000;
		chdata[i] = wdata;
	}
}

void fnFL_CalABS(float *data){
	int i;
	float fdata;
	for(i = 0; i< SAMPLE_DATA_NUM; i++){
		fdata = data[i];
		if(fdata < 0){
			fdata  = 0 - fdata;
		}
		data[i] = fdata;
	}
}
static U8 tpointer =0;
static float tbuff[256];
void fnFL_CalACC(float *acc, int samplenum, int fs, FLBLInfoDef *blinfo){
	float ra,rv,rd;
	float df;
	float sumd,sumv,suma;
	float maxa, mina, peaka;
	int hn;
	int n;
	float w;
	float cd,ca,cv;
	int i;
	float wmax, wfa;
	int wi;
	int fi,ti;
	
	wmax = 0;
	wfa =0;
	wi = 0;
	n = samplenum;
	sumd = 0;
	sumv = 0;
	suma = 0;
	maxa = 0;
	mina = 1000000;
	
	for(i = 0; i < n;  i++){
		 //acc[i] = 
		if(maxa < acc[i]){
			maxa = acc[i];
			wi = i;
		
			
		}
		if(mina > acc[i])
			mina = acc[i];
		/*hanning window*/
		w = (float)acc[i] * 0.5  * (1+cos(2* 3.1415926535 * i / n));
	
		/*hanning window*/
	//	acc[i] = w;
		//acc[i] = acc[i]* 0.5 * (1 + math.cos(2 * np.pi*i/ n))
	}
	fnFFT_FFT(acc, n);
	df = (float)fs / samplenum;
	hn = n / 2;
	w = (float)50/df;
	fi = (int)w;
	for(i = fi; i < (n - fi); i++){
		
		 if(i == 0){
        w = 0;
		}else if(i < hn){
				w = (float)1 / (2 * 3.1415926535 * i * df); 
		}else if(i < (n - 1)){
				w = (float)1 / (2 * 3.1415926535 * (i + 1 - 2 * hn) * df);
		}else{
				w = 0;
		}
		ca = ((float)g_fft_stCalBuff[i].real * g_fft_stCalBuff[i].real + (float)g_fft_stCalBuff[i].imag * g_fft_stCalBuff[i].imag);
		//ca = ca * 1.633 * 1.633;
		//cv = (float)ca * w * w;
		cv = (float)ca * w * w;//* 1.633 * 1.633; //hanning window;
		cd = (float)cv * w * w;
		if(i > 0){
			suma += ca;
			sumv += cv;
			sumd += cd;
		
			if(ca > wfa){
				wfa = ca;
				wmax = w;
				wi = i;
			}
		}
	}
	ra = sqrt(suma) / n;
	rv = sqrt(sumv) / n;
	rd = sqrt(sumd* 2 * 2 * 2) / n;//peak-to-peak
	blinfo->maxa = maxa;
	blinfo->mina = mina;
	blinfo->pa = ra * 1.414;//(maxa - mina)/2 / 100;//peak
	blinfo->ra = ra;
	blinfo->maxv = 0;
	blinfo->minv = 0;
	blinfo->rv = rv * 1000;//rms
	tbuff[tpointer] = blinfo->rv;
	tpointer ++;
	//if(blinfo->rv > 24){
		///blinfo->rv  = 24;
	//}
	blinfo->maxd = 0;
	blinfo->mind = 0;
	blinfo->rd = rd * 1000000;
	
}/*
  memo2.Lines.Add('???,???:'+format('%4f', [maxa]) + '???:'+format('%4f', [mina]) + '????:'+format('%4f', [racc]));
  memo2.Lines.Add('??,???:'+format('%4f', [maxv]) + '???:'+format('%4f', [minv]) + '????:'+format('%4f', [rv]));
  //memo2.Lines.Add('???,???:'+format('%4f', [maxa]) + '???:'+format('%4f', [mina]) + '????:'+format('%4f', [racc]));
  memo2.Lines.Add('??,???:'+format('%4f', [maxd]) + '???:'+format('%4f', [mind]) + '????:'+format('%4f', [rd]));
*/
#define DTIME_ARR_SIZE 256
#define DTIME_SORT_SIZE 128

int dtimearr[DTIME_ARR_SIZE];
int dtimesort[DTIME_SORT_SIZE][3];


float fnFL_CalTC(float *data, U32 samplerate,U32 samplenum){
	float ta, ts, sumb, sumu, avgsum, nowv,absv;
	int cb, cu, i, j, lasttime, ntime, deltime, fn, avgcount, avgn, dtimen, dsortn, maxn, maxtn;
	U8 bcheck, inrange;
	float res = 0;
	cb = 0;
	cu = 0;
	fn = 5;
	avgn = samplerate / 100;
	dtimen = 0;
	bcheck = FALSE;
	avgsum = 0;
	for(i = 0; i < samplenum; i++){
		
     		avgsum = avgsum + data[i];
		if(i >= avgn){
			avgsum = avgsum - data[i - avgn];
			nowv = avgsum / avgn;
			if((data[i - 1] < nowv) && (data[i] > nowv)) {
				ntime = i;
				if(bcheck == TRUE){
					deltime = ntime - lasttime;
					if(dtimen < DTIME_ARR_SIZE){
						dtimearr[dtimen] = deltime;
						dtimen++;
					}
					
				}
				lasttime = ntime;
				bcheck = TRUE;
			}
		}
		if(avgcount < avgn){
			avgcount ++;
		}
	}
	dsortn = 0;
	for(i = 0; i < dtimen; i++){
		inrange = FALSE;
		for(j = 0; j < dsortn; j++){
				absv = dtimearr[i] - dtimesort[j][0];
				if(absv < 0){
					absv = 0 - absv;
				}
			  if(absv < 5){
			  	dtimesort[j][2] = dtimesort[j][2] + dtimearr[i];
					dtimesort[j][1]++;
					inrange = TRUE;
					break;
			  }
        			
		}
		if(inrange == FALSE) {
			if(dsortn < DTIME_SORT_SIZE){
				dtimesort[dsortn][0] = dtimearr[i];
				dtimesort[dsortn][1] = 1;
				dtimesort[dsortn][2] = dtimearr[i];
				dsortn++;
			}
		}
	}
	maxn = 0;
	maxtn = 0;
	for(j = 0; j < dsortn; j ++){
	
		if(dtimesort[j][1] > maxn){
			maxn = dtimesort[j][1];
			fn = dtimesort[j][2];
		  fn += (dtimesort[j][1] >> 1);
			maxtn = fn / dtimesort[j][1];;
		}

	}
	res = (float)maxtn / samplerate;
 	return(res);
}
void fnFL_CalBLEx(float *data, U32 samplerate, U32 samplenum, float tc){
	
	float ts;
	int n;
	int i;
	float ec;
	ts = 1/(float)samplerate;
  	n= samplenum;
	ec = exp(-ts/tc);
	for(i = 1; i < n; i++){
		if(data[i] > data[i - 1]){
			
		}else{
			data[i] = (data[i - 1] - data[i]) * ec  + data[i];
		}
			
	}
}




