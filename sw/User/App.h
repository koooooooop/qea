#ifndef _H_APP_
#define _H_APP_
#include "define.h"



#define LED1_GPIO_PORT    	GPIOF
#define LED1_GPIO_CLK 	    RCC_AHB1Periph_GPIOF
#define LED1_GPIO_PIN		9
#define LED1_GPIO_PIN_DEF       GPIO_Pin_9

extern void app_init(void);
extern void app_task(void);
extern void fnAP_GetID(char *msg, U16 *pointer);
#endif
