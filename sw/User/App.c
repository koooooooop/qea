#include "App.h"
#include "IO.h"
#include "Timer.h"

void app_init(void);
void app_task(void);

void app_init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(LED1_GPIO_CLK, ENABLE);// 打开串口GPIO的时钟
	
	
	GPIO_InitStructure.GPIO_Pin = LED1_GPIO_PIN_DEF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);
	SET_IO_OUT_HS(LED1_GPIO_PORT, LED1_GPIO_PIN);
	
}
void app_task(void){
	if(g_tm_stTimerFlag.Bits.bTimer1s){
		SET_IO_TOGGLE(LED1_GPIO_PORT, LED1_GPIO_PIN);
	}
	
}
const U8 ap_uchid[]= "ABCDBAOLUO00000012345678";
 void fnAP_GetID(char *msg, U16 *pointer){
	U16 len;

	len = strlen((char *)ap_uchid);
	memcpy(msg, ap_uchid, len);
	*pointer += len;
}

