#ifndef _H_ADC_
#define _H_ADC_
#include "define.h"
#include "filter.h"


#define P_EXTERNAL_0_CLK		RCC_AHB1Periph_GPIOA
#define P_EXTERNAL_0_PORT	GPIOA
#define P_EXTERNAL_0_PIN		GPIO_Pin_1
#define P_EXTERNAL_0_SOURCE  GPIO_PinSource1

#define P_EXTERNAL_1_CLK		RCC_AHB1Periph_GPIOC
#define P_EXTERNAL_1_PORT	GPIOC
#define P_EXTERNAL_1_PIN		GPIO_Pin_3
#define P_EXTERNAL_1_SOURCE  GPIO_PinSource3

#define P_EXTERNAL_2_CLK		RCC_AHB1Periph_GPIOC
#define P_EXTERNAL_2_PORT	GPIOC
#define P_EXTERNAL_2_PIN		GPIO_Pin_2
#define P_EXTERNAL_2_SOURCE  GPIO_PinSource2

#define P_EXTERNAL_3_CLK		RCC_AHB1Periph_GPIOC
#define P_EXTERNAL_3_PORT	GPIOC
#define P_EXTERNAL_3_PIN		GPIO_Pin_1
#define P_EXTERNAL_3_SOURCE  GPIO_PinSource1

#define P_EXTERNAL_4_CLK		RCC_AHB1Periph_GPIOC
#define P_EXTERNAL_4_PORT	GPIOC
#define P_EXTERNAL_4_PIN		GPIO_Pin_0
#define P_EXTERNAL_4_SOURCE  GPIO_PinSource0

#define P_EXTERNAL_5_CLK		RCC_AHB1Periph_GPIOB
#define P_EXTERNAL_5_PORT	        GPIOB
#define P_EXTERNAL_5_PIN		GPIO_Pin_0
#define P_EXTERNAL_5_SOURCE    GPIO_PinSource0

#define P_EXTERNAL_6_CLK		RCC_AHB1Periph_GPIOB
#define P_EXTERNAL_6_PORT	       GPIOB
#define P_EXTERNAL_6_PIN		GPIO_Pin_1
#define P_EXTERNAL_6_SOURCE    GPIO_PinSource1

#define P_EXTERNAL_7_CLK		 RCC_AHB1Periph_GPIOA
#define P_EXTERNAL_7_PORT	        GPIOA
#define P_EXTERNAL_7_PIN		GPIO_Pin_0
#define P_EXTERNAL_7_SOURCE    GPIO_PinSource0

#define ADC_CHANNEL_NUM 8
#define ADC_BUFF_SIZE (U32)ADC_CHANNEL_NUM * (SAMPLE_DATA_NUM + FILTER_COEF_NUM)
extern signed short *g_adc_samplebuff;
typedef struct{
	U8 data;
}adc_regs_typedef;
extern adc_regs_typedef g_adc_regs;
extern void fnADC_Init(void);
extern void fnADC_Task(void);
extern U8 fnADC_IsFinish(void);
extern void fnADC_Start(void);
#endif
