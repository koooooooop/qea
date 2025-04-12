#include "define.h"
#include "timer.h"
#include "app.h"
#include "uart.h"
#include "baoluo.h"
#include "adc.h"
#include "UartWiFi.h"
#include "wifi.h"
#include "RTManage.h"
#include "switch.h"
#include "DetApp.h"
#define	APP_FLASH_BASE	0x08000000	// 0x08010000

const U8 g_mn_uchSoftwareMagicNum[SOFTWARE_MAGICNUM_NUM] =
{
	'Z','Z','Y', 'S', 
	(U8)(DEVTYPE >> 8), (U8)DEVTYPE, (U8)(SWVER >> 8), (U8)SWVER,
	(U8)(HWVER >> 8), (U8)HWVER, XNUM, YNUM
};

void sys_init(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
}
int main(void)
{
#if APP_FLASH_BASE != 0x08000000
	SCB->VTOR = APP_FLASH_BASE;
#endif
	
 	sys_init();
	timer_init();
	app_init();
	fnADC_Init();
	fnSW_Init();
	fnUA_Init();
	fnWFUA_Init();
	///fnBL_Init();
	fnWF_Init();
	fnRTM_Init();
	fnDetApp_Init();
	while(1)
	{
		timer_task();
		fnSW_RealTime();
		fnUA_RealTime();
		fnWFUA_RealTime();
		fnADC_Task();
		///fnBL_Task();
	
		app_task();
		fnDetApp_RealTime();
		fnWF_RealTime();
		fnRTM_RealTime();
		fnUA_Clear();
		fnWFUA_Clear();
		fnSW_Clear();
		timer_clear();
		
	}
}
/********************************************************** END OF FILE **************************************************************/

