#ifndef _H_FFT_
#define _H_FFT_
#include "define.h"
typedef struct
{	
	float real;
	float imag;
}Complex;

#define FFT_BUFF_SIZE 8192
#define FFT_BUFF_SIZE_BITS 13
extern Complex g_fft_stCalBuff[FFT_BUFF_SIZE];
extern void fnFFT_Cal(Complex *xin, U16 N, U16 M);
extern void fnFFT_GetComp(U8 k, Complex *comp);
extern void fnFFT_FFT(float *adval, U16 adsize);
extern void fnFFT_CalMag(Complex *fftv, U16 fftsize);
#endif
