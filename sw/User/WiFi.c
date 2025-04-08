#include "WiFi.h"
#include "RTManage.h"
#include "IO.h"
#include "timer.h"
STRWiFiType g_wifi_stRegs;
#define WF_HEARTBEAT_TIME 10
#define WF_DEAD_TIME 60
#define WF_DEAD_TIME 60

#define WF_STARTUP_TIME 30

#define WF_RST_GPIO_PORT    	GPIOA
#define WF_RST_GPIO_CLK 	    RCC_AHB1Periph_GPIOA
#define WF_RST_GPIO_PIN		8
#define WF_RST_GPIO_PIN_DEF       GPIO_Pin_8

#define WF_PROM_GPIO_PORT    	GPIOC
#define WF_PROM_GPIO_CLK 	    RCC_AHB1Periph_GPIOC
#define WF_PROM_GPIO_PIN		9
#define WF_PROM_GPIO_PIN_DEF       GPIO_Pin_9

#define WF_SHUTDOWN_GPIO_PORT    	GPIOA
#define WF_SHUTDOWN_GPIO_CLK 	    RCC_AHB1Periph_GPIOA
#define WF_SHUTDOWN_GPIO_PIN		8
#define WF_SHUTDOWN_GPIO_PIN_DEF       GPIO_Pin_8

static U8 wf_rst_state = FALSE;
void fnWF_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	memset(&g_wifi_stRegs, 0, sizeof(g_wifi_stRegs));
	g_wifi_stRegs.m_uchQueryTimer = WF_HEARTBEAT_TIME;
	g_wifi_stRegs.m_uchDeadTimer = WF_DEAD_TIME;
	g_wifi_stRegs.m_uchStartUpTimer = WF_STARTUP_TIME;

	
		
	RCC_AHB1PeriphClockCmd(WF_RST_GPIO_CLK, ENABLE);// 打开串口GPIO的时钟
	
	
	GPIO_InitStructure.GPIO_Pin = WF_RST_GPIO_PIN_DEF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(WF_RST_GPIO_PORT, &GPIO_InitStructure);
	SET_IO_OUT_HS(WF_RST_GPIO_PORT, WF_RST_GPIO_PIN);
	SET_IO_HIGH(WF_RST_GPIO_PORT, WF_RST_GPIO_PIN);
	
	RCC_AHB1PeriphClockCmd(WF_PROM_GPIO_CLK, ENABLE);// 打开串口GPIO的时钟
	
	
	GPIO_InitStructure.GPIO_Pin = WF_PROM_GPIO_PIN_DEF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(WF_PROM_GPIO_PORT, &GPIO_InitStructure);
	SET_IO_OUT_HS(WF_PROM_GPIO_PORT, WF_PROM_GPIO_PIN);
	SET_IO_HIGH(WF_PROM_GPIO_PORT, WF_PROM_GPIO_PIN);
}
#define WFR_STATE_IDLE 0
#define WFR_STATE_HIGH 1
	#define WFR_TIMER_HIGH 10

#define WFR_STATE_PRELOW 2
#define WFR_STATE_LOW 3
	#define WFR_TIMER_LOW 30
#define WFR_STATE_RELEASE 4
	#define WFR_TIMER_RELEASE 50
#define WFR_STATE_RESET 5

void fnWF_Reset(void)
{	
	static U8 timer = 0;
	U8 period = 10;
	static U8 setperiod = 10;
	if(g_tm_stTimerFlag.Bits.bTimer1s){
		if(g_wifi_stRegs.m_uchStartUpTimer > 0){
			g_wifi_stRegs.m_uchStartUpTimer --;
		}
	}
	if(g_wifi_stRegs.m_uchStartUpTimer == 0){
		if(0)//(g_sw_stRegs[P_SW_RESET_OFFSET].m_uchEvent == KEY_EVENT_SHORTRELEASE)
		{
			wf_rst_state = TRUE;
		}
		else if(0)//(g_sw_stRegs[P_SW_RESET_OFFSET].m_uchEvent == KEY_EVENT_3SLONGPRESS)
		{
			fnRTM_ResetServer();
			timer = 100;
			setperiod = 1;
		}
	}else{
		if(0)//g_sw_stRegs[P_SW_RESET_OFFSET].m_uchEvent == KEY_EVENT_3SLONGPRESS)
		{
			timer = 100;
			setperiod = 3;
			
		}
	}
	if(g_tm_stTimerFlag.Bits.bTimer100ms) 
	{
		if(timer > 0)
		{
			period = setperiod;
			timer --;
		}else{
			if(g_wifi_stRegs.m_uchStartUpTimer > 0){
				period = 25;
			}
		}
	}

}
void fnWF_RealTime(void){
	fnWF_Reset();
	if(wf_rst_state)
	{
		if(g_tm_stTimerFlag.Bits.bTimer1s){
			g_wifi_stRegs.m_uchDeadTimer++;
		}
		if(g_wifi_stRegs.m_uchDeadTimer < 2)
		{
			//SET_IO_LOW(WF_RST_PORT, WF_RST_PIN);
		}
		else
		{
			wf_rst_state = FALSE;
			//SET_IO_HIGH(WF_RST_PORT, WF_RST_PIN);
			g_wifi_stRegs.m_uchDeadTimer = WF_DEAD_TIME;
			g_wifi_stRegs.m_uchQueryTimer = WF_HEARTBEAT_TIME;
		}
	}
	else
	{
		if(g_tm_stTimerFlag.Bits.bTimer1s)
		{
			if(g_wifi_stRegs.m_uchQueryTimer)
			{
				g_wifi_stRegs.m_uchQueryTimer --;
			}
			if(g_wifi_stRegs.m_uchDeadTimer) 
			{
				g_wifi_stRegs.m_uchDeadTimer --;
				if(0 == g_wifi_stRegs.m_uchDeadTimer)
				{
					wf_rst_state = TRUE;
				}
			}
			if(0 == g_wifi_stRegs.m_uchQueryTimer)
			{
				if(fnRTM_PingServer())
				{
					g_wifi_stRegs.m_uchQueryTimer = WF_HEARTBEAT_TIME;
				}
			}	
		}
	}
}

void fnWF_ResetDeadTime(void)
{
	g_wifi_stRegs.m_uchDeadTimer = WF_DEAD_TIME;
}
void fnWF_ResetQueryTime(void)
{
	g_wifi_stRegs.m_uchQueryTimer = WF_HEARTBEAT_TIME;
}

