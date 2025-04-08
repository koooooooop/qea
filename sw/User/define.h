#ifndef _H_DEFINE_
#define _H_DEFINE_
#include "stm32f4xx.h"
#include "string.h"
#include <stdint.h>

typedef unsigned long U32;
typedef unsigned char U8;
typedef unsigned short U16;
#define TRUE 1
#define FALSE 0
#define HIGH 1
#define LOW 0

#define DEVTYPE (U16)1001
#define SWVER 1211
#define HWVER 0x300
#define XNUM 3
#define YNUM 8
#define SOFTWARE_MAGICNUM_NUM 12
#define SOFTWARE_MAGICNUM_OFF_HEADER 0
#define SOFTWARE_MAGICNUM_OFF_DEVTYPE 4

#define SOFTWARE_MAGICNUM_OFF_SVERH 5
#define SOFTWARE_MAGICNUM_OFF_SVERL 6
#define SOFTWARE_MAGICNUM_OFF_HVERH 7
#define SOFTWARE_MAGICNUM_OFF_HVERL 8
#define SOFTWARE_MAGICNUM_OFF_XNUM 9
#define SOFTWARE_MAGICNUM_OFF_YNUM 10

extern const U8 g_mn_uchSoftwareMagicNum[SOFTWARE_MAGICNUM_NUM];

typedef union{
	 U8 hexs[4];
	 float val;
	 U32 u32data;
}UFloatTypeDef;
/* 开关全局中断的宏 */
#define ENABLE_INT()	__enable_irq() 	/* 使能全局中断 */
#define DISABLE_INT()	__disable_irq()	/* 禁止全局中断 */
#endif
