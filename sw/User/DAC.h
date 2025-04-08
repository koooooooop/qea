#ifndef _H_DAC_
#define _H_DAC_
#include "define.h"

#define DAC_GPIO_PORT    	GPIOA
#define DAC_GPIO_CLK 	      RCC_AHB1Periph_GPIOA
#define DAC_GPIO_PIN		GPIO_Pin_4
extern void dac_init(void);
extern void dac_setval(U16 value);
#endif
