#include "stm32f4xx_fsmc.h"
#include "adc.h"	

typedef struct{
	GPIO_TypeDef *port;
	U16 pin;
	U8 channel;
}STRADPortConfig;
#define ADC_TIMER TIM4
const STRADPortConfig ad_port_config[] =
{
	{P_EXTERNAL_0_PORT, P_EXTERNAL_0_PIN, 16},//10},
	{P_EXTERNAL_1_PORT, P_EXTERNAL_1_PIN, 17},//11},
	{P_EXTERNAL_2_PORT, P_EXTERNAL_2_PIN, 13},
	{P_EXTERNAL_3_PORT, P_EXTERNAL_3_PIN, 1},
	{P_EXTERNAL_4_PORT, P_EXTERNAL_4_PIN, 12},
	{P_EXTERNAL_5_PORT, P_EXTERNAL_5_PIN, 9},
	{P_EXTERNAL_6_PORT, P_EXTERNAL_6_PIN, 0},
	{P_EXTERNAL_7_PORT, P_EXTERNAL_7_PIN, 8},
};
adc_regs_typedef g_adc_regs;

signed short *g_adc_samplebuff;
void fnADC_Init(void);
void fnADC_Task(void);
void fnADC_InitDMA(void);
void  fnADC_InitTimer(void);
void fnADC_InitADC(void);

#define Bank1_SRAM1_ADDR    ((U32)(0x60000000))
#define ADC_DMABUFF_RAM_ADDR Bank1_SRAM1_ADDR

void SRAM_WriteUint16(U32 Address, U16 Data)
{
    *(U16*)(Bank1_SRAM1_ADDR + Address) = Data;
}

U16 SRAM_ReadUint16(U32 Address)
{
    return (*(__IO U16*)(Bank1_SRAM1_ADDR + Address));
}

void fnADC_NORSRAMInit(void)
{ 
	
	FSMC_NORSRAMInitTypeDef FSMC_NORSRAMStructure;
	FSMC_NORSRAMTimingInitTypeDef FSMC_ReadNORSRAMTiming;
	FSMC_NORSRAMTimingInitTypeDef FSMC_WriteNORSRAMTiming;

	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
	
	

	
	FSMC_ReadNORSRAMTiming.FSMC_AddressSetupTime = 0x2;
	FSMC_ReadNORSRAMTiming.FSMC_AddressHoldTime = 0x05;
	FSMC_ReadNORSRAMTiming.FSMC_DataSetupTime = 0x02;
	FSMC_ReadNORSRAMTiming.FSMC_BusTurnAroundDuration = 0x00;
	FSMC_ReadNORSRAMTiming.FSMC_CLKDivision = 0x00;
	FSMC_ReadNORSRAMTiming.FSMC_DataLatency = 0x00;
	FSMC_ReadNORSRAMTiming.FSMC_AccessMode = FSMC_AccessMode_A;

	FSMC_WriteNORSRAMTiming.FSMC_AddressSetupTime = 0x02;
	FSMC_WriteNORSRAMTiming.FSMC_AddressHoldTime = 0x05;
	FSMC_WriteNORSRAMTiming.FSMC_DataSetupTime = 0x02;
	
	FSMC_WriteNORSRAMTiming.FSMC_BusTurnAroundDuration = 0x00;
	
	FSMC_WriteNORSRAMTiming.FSMC_CLKDivision = 0x00;
	FSMC_WriteNORSRAMTiming.FSMC_DataLatency = 0x00;
	FSMC_WriteNORSRAMTiming.FSMC_AccessMode = FSMC_AccessMode_A;
	
	FSMC_NORSRAMStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
	FSMC_NORSRAMStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	FSMC_NORSRAMStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;
	FSMC_NORSRAMStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMStructure.FSMC_ReadWriteTimingStruct = &FSMC_ReadNORSRAMTiming;
	FSMC_NORSRAMStructure.FSMC_WriteTimingStruct = &FSMC_WriteNORSRAMTiming;
	FSMC_NORSRAMInit(&FSMC_NORSRAMStructure);
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}


void fnADC_SRAM_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF |  RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_SRAM1, ENABLE);
       
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource7 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource11 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource13 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15 , GPIO_AF_FSMC);

	GPIO_PinAFConfig(GPIOE, GPIO_PinSource0 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource1 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource2 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource3 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource4 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource5 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource6 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);

	
	
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource0 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource1 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource2 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource3, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource4 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource5 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource12, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource13 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource14 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource15, GPIO_AF_FSMC);

	
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource0, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource1 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource2 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource3 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource4 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource5 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource13 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource14 , GPIO_AF_FSMC);

	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1  | GPIO_Pin_4 | GPIO_Pin_5 |GPIO_Pin_7|  GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 |GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 |GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1| GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5  |GPIO_Pin_13| GPIO_Pin_14 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	

   fnADC_NORSRAMInit();
	
}

void fnADC_Init(void){
	volatile U16 val;
	U32 i;

	volatile U16 adstest[1024];
	
	g_adc_samplebuff = (signed short *)ADC_DMABUFF_RAM_ADDR;
	fnADC_SRAM_Configuration();
	fnADC_InitDMA();
	//fnADC_InitADC();
	
	//fnADC_InitTimer();
	for(i = 0; i < 0x10000; i++)
		SRAM_WriteUint16(2 *i, i);
	for(i = 0; i < 1024; i++){
		val = SRAM_ReadUint16(i * 2);
		adstest[i] = val;
	}
	
}
void fnADC_Task(void){
}
U8 fnADC_IsFinish(void){
	U8 res = FALSE;
	if(DMA2_Stream0->NDTR == 0){
		res = TRUE;
	}
	return(res);
}
void fnADC_Start(void){
	ADC_TIMER->CR1 = 0x0000;
	DMA2_Stream0->NDTR = 65535;//ADC_BUFF_SIZE;/*���ݴ��������Ϊ3*/
	DMA2->LIFCR = 0x3F;
	fnADC_InitADC();
	ADC->CCR |= 0x800000;
	DMA_Cmd(DMA2_Stream0,ENABLE);//����DMA����
	
	
	fnADC_InitTimer();
}
void fnADC_InitADC(void)
{
	U8 i;
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//ʹ��GPIOAʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);//ʹ��GPIOAʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);//ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);//ʹ��ADCʱ��

	RCC->APB2RSTR |= RCC_APB2RSTR_ADCRST;

	RCC->APB2RSTR &= ~RCC_APB2RSTR_ADCRST;
	/*��ʼ��ADC1ͨ��3 ��IO��*/
	for(i = 0; i < ADC_CHANNEL_NUM; i++){
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;/*ģ������*/
		GPIO_InitStructure.GPIO_Pin   = ad_port_config[i].pin;/*ͨ��3*/
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;/*����������*/
		GPIO_Init(ad_port_config[i].port, &GPIO_InitStructure);/*��ʼ��*/
	}

	/*ͨ�ÿ��ƼĴ���������*/
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;/*DMAʧ��*/
	ADC_CommonInitStructure.ADC_Mode          = ADC_Mode_Independent;/*����ģʽ*/
	ADC_CommonInitStructure.ADC_Prescaler     = ADC_Prescaler_Div4;/*APB2��4��Ƶ ��84/4=21M*/
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_10Cycles;/*���������׶ε���ʱ5��ʱ��*/
	ADC_CommonInit(&ADC_CommonInitStructure);/*��ʼ��*/
	/*��ʼ��ADC1*/
	ADC_InitStructure.ADC_Resolution  = ADC_Resolution_12b;/*12λģʽ*/
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;/*ɨ��ģʽ*/
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;/*����ת��*/
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T4_CC4;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;/*��ֹ������� ʹ���������*/
	ADC_InitStructure.ADC_DataAlign    = ADC_DataAlign_Right;/*�Ҷ���*/
	ADC_InitStructure.ADC_NbrOfConversion = ADC_CHANNEL_NUM;/*ֻʹ��1ͨ�� ����ͨΪ1*/
	ADC_Init(ADC1,&ADC_InitStructure);/*��ʼ��*/

	ADC_Cmd(ADC1,ENABLE);/*����ת��*/
	for(i = 0; i < ADC_CHANNEL_NUM; i++){
		ADC_RegularChannelConfig(ADC1, ad_port_config[i].channel, i + 1, ADC_SampleTime_56Cycles);/*���ù���ͨ��16 3������ ����ʱ�� */
	}
	///ADC_SoftwareStartConv(ADC1);/*�������ת��*/
	ADC_DMARequestAfterLastTransferCmd(ADC1,ENABLE);//Դ���ݱ仯ʱ����DMA����
	ADC_DMACmd(ADC1,ENABLE);//ʹ��ADC����
}
/****************************************************************************************
*�� �� ��:bsp_InitAdcDMA
*��������:��ʼ��DMA2
*��    ��:��
*�� �� ֵ:��
*****************************************************************************************/

void fnADC_InitDMA(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);/*DMA2��ʱ��ʹ��*/

       RCC->AHB1RSTR |= RCC_AHB1RSTR_DMA2RST;

	RCC->AHB1RSTR &= ~RCC_AHB1RSTR_DMA2RST;
	   
	while(DMA_GetCmdStatus(DMA2_Stream0)!=DISABLE);/*�ȴ�DMA��������*/


	DMA_InitStructure.DMA_Channel = DMA_Channel_0;/*DMAͨ��0*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;/*�����ַ*/
	DMA_InitStructure.DMA_Memory0BaseAddr    = (U32)g_adc_samplebuff;/*��ȡ����ַ*/
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;/*��������赽�ڴ�*/
	DMA_InitStructure.DMA_BufferSize = 65535;//ADC_BUFF_SIZE;/*���ݴ��������Ϊ3*/
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;/*��ַ������*/
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;/*��ַ������*/
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;/*���ݳ��Ȱ���*/
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;/*���ݳ��Ȱ���*/
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;/*�����ȼ�*/
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;/*ѭ��ģʽ*/
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;/*��ֹFIFO*/
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;/*FIFO��ֵ*/
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;/*���δ���*/
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;/*���δ���*/
	DMA_Init(DMA2_Stream0,&DMA_InitStructure);/**/
	DMA_Cmd(DMA2_Stream0,DISABLE);//����DMA����
}

void  fnADC_InitTimer(void)
{

	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;//Timer2 clock enable
	ADC_TIMER->CR1 = TIM_CR1_ARPE;//�Զ�����
	// TIM4->ARR = BS_BIT_PERIOD - 1;//205*0.5=102.5us
	ADC_TIMER->PSC = (SystemCoreClock /8 /1000000  - 1);//fck_psc/(PSC[15:0] + 1)=0.25us
	ADC_TIMER->EGR = 0x0001;//Reload immediate
	ADC_TIMER->ARR = 1560;//1560;//156;//64 samples per 20ms
	ADC_TIMER->CCR4 = 600;
	ADC_TIMER->EGR = 0x0001;//Reload immediate
	ADC_TIMER->CCMR2 = 0x6800;//ʹ��PWM���ܣ��������Զ�����CH3
	ADC_TIMER->CCER= 0x9000;//??PWM??,???????CH3
	ADC_TIMER->CR1 |= 0x0001;
	
}

