#include "dac.h"
void dac_init(void);
void dac_setval(U16 value);

void dac_init(void){
	GPIO_InitTypeDef GPIO_InitStructure;			
	RCC->APB1ENR |= RCC_APB1ENR_DACEN;
	RCC->AHB1ENR |= RCC_AHB1Periph_GPIOA;
				
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = DAC_GPIO_PIN;
	GPIO_Init(DAC_GPIO_PORT, &GPIO_InitStructure);

      RCC->AHB1ENR |= RCC_AHB1Periph_GPIOA;

      DAC->DHR12R1 = 0;
      DAC->CR =  DAC_CR_EN1;
	
}
void dac_setval(U16 value){
	DAC->DHR12R1 = value;
}
