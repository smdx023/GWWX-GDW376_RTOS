/*
********************************************************************************
*  文 件 名：Main.c
*
*  文件描述：主函数
*
*  所用芯片：stm32F103RCT6/101RCT6, FLASH 256K, RAM 48K
*
*  创 建 人：023
*
*  版 本 号：V1.00 国网无线表模块程序。
*
*  修改记录：2012-03-26 11:05:08 增加支持控制命令F38激活终端连接，此控制命令发
*				  送成功后，则利用备用主站IP和端口登录。同时支
*				  持短信唤醒，命令为LINKTOZHGC*IP,端口号*，例：
*				   LINKTOZHGC*112.091.151.002,7107*
********************************************************************************
*/

#include  <ucos_ii.h>
#include "GDW376_CFG.h"
#include "stdio.h"
#include "task_wlan.h"
#include "task_ptl.h"
#include "task_meter.h"
#include "task_freeze.h"
#include "task_dat_pro.h"



#define CONFIG_PRINT


#ifdef CONFIG_PRINT
#define print(fmt,args...)  printf(fmt, ##args)
#else
#define print(fmt,args...)
#endif

void  CPU_IntDis (void);

#define	 APP_CFG_TASK_START_STK_SIZE	     512
#define	 APP_CFG_TASK_CB_STK_SIZE	     512
#define	 APP_CFG_TASK_GPRS_STK_SIZE	     1024
#define	 APP_CFG_TASK_GDW376_STK_SIZE	     1024
#define	 APP_CFG_TASK_DATPRO_STK_SIZE	     512
#define	 APP_CFG_TASK_FRE_STK_SIZE	     512


#define	 APP_CFG_TASK_START_PRIO	     9
#define	 APP_CFG_TASK_CB_PRIO		     4
#define	 APP_CFG_TASK_GPRS_PRIO		     5
#define	 APP_CFG_TASK_GDW376_PRIO	     6
#define	 APP_CFG_TASK_DATPRO_PRIO	     7
#define	 APP_CFG_TASK_FRE_PRIO		     8


static	OS_STK	  App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE];
static	OS_STK	  App_TaskCBStk[APP_CFG_TASK_CB_STK_SIZE];
static	OS_STK	  App_Task_Gprs_Stk[APP_CFG_TASK_GPRS_STK_SIZE];
static	OS_STK	  App_Task_GDW376_Stk[APP_CFG_TASK_GDW376_STK_SIZE];
static	OS_STK	  App_Task_DatPro_Stk[APP_CFG_TASK_DATPRO_STK_SIZE];
static	OS_STK	  App_Task_Fre_Stk[APP_CFG_TASK_FRE_STK_SIZE];


/******************************************************************************
    功能说明：
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int task_main(void)
{
	// InitHardware();   //;硬件初始化
	// ZD_RAM_MAK();     //;写入检测字
	// CS_Read();	     //;读出终端的参数
	// MMU_FM24_Read();  //;读出FM24C16保存的配置数据
	// ZD_Stat_Init();   //;初始化终端状态
//;    ERC14_Judge(2);	 //;终端来电  zrt 20130801放到抄表函数里面
	while(1) {
		ZD_POW_CHK();	    //;检测掉电处理
		ZD_HWareRes_CHK();  //;检测硬件是否要重启
		//ZD_CSSAVE_CHK();    //;检测终端参数保存
		//USART_Com();	    //;串口通讯@like
		//ZD_Port_Com()     //;终端端口通讯@like
		//Modem_Com();	      //;Gprs模块通讯 @like
		AFNComDeal();	    //;GDW130规约本地与远程通讯

		if(RTC_FlagMs) {   //;250ms执行一次
			RTC_FlagMs = 0;

//			SendFirstByte_UART5();
//			U5DS.TIndex = 1;
//			U5DS.TLong  = 20;


			//ClockTimer();
			//ZD_DJPN();	//;测量点数据冻结
			//ZD_SB();      //@like
			//ZD_CB();	//;终端抄表处理@like
		//	ZD_EventRead();
			//ZD_Relay();	//;数据转发
		//	Modem_GDCDLightCtr();
		//	ZD_ERC();	//;处理事件
			ZD_Task();	//;任务上送
			if(RTC_FlagSec) { //;每秒执行一次
				RTC_FlagSec = 0;
				LinkStat_CHK();
				//Watchdog();
				//Send_Modem_Status();
				//ZD_CB_CHK();	  //;抄表间隔倒计时@like
				//ZD_DJPN_CHK();	  //;终端冻结检测
				//ZD_Mast_Link();	  //;终端与主站连接
				if(RTC_FlagMin) { //;每分钟执行一次
					RTC_FlagMin = 0;
					////ZD_Task_CHK();
					//ZDPN_MXT();   //;当日当月最值与发生时间计算
					//ZDPN_YXT();   //;当日当月越限时间计算
					//LinkStat_CHK();
				//	Modem_LinkTime();
				}
			}

		}
	}
}



/******************************************************************************
    功能说明：
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
static void App_EventCreate (void)
{
	;
}



/******************************************************************************
    功能说明：
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
static void App_TaskCreate (void)
{
	/* 抄表任务 */
	OSTaskCreateExt((void (*)(void *)) App_Task_Meter,
			(void		*) 0,
			(OS_STK		*)&App_TaskCBStk[APP_CFG_TASK_CB_STK_SIZE - 1],
			(INT8U		 ) APP_CFG_TASK_CB_PRIO,
			(INT16U		 ) APP_CFG_TASK_CB_PRIO,
			(OS_STK		*)&App_TaskCBStk[0],
			(INT32U		 ) APP_CFG_TASK_CB_STK_SIZE,
			(void		*) 0,
			(INT16U		 )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

//	/* GPRS通信模块管理任务 */
//	OSTaskCreateExt((void (*)(void *)) App_Task_Wlan,
//			(void		*) 0,
//			(OS_STK		*)&App_Task_Gprs_Stk[APP_CFG_TASK_GPRS_STK_SIZE - 1],
//			(INT8U		 ) APP_CFG_TASK_GPRS_PRIO,
//			(INT16U		 ) APP_CFG_TASK_GPRS_PRIO,
//			(OS_STK		*)&App_Task_Gprs_Stk[0],
//			(INT32U		 ) APP_CFG_TASK_GPRS_STK_SIZE,
//			(void		*) 0,
//			(INT8U		 )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
//
//	/* 主站数据交互 */
//	OSTaskCreateExt((void (*)(void *)) App_Task_Ptl,
//			(void		*) 0,
//			(OS_STK		*)&App_Task_GDW376_Stk[APP_CFG_TASK_GDW376_STK_SIZE - 1],
//			(INT8U		 ) APP_CFG_TASK_GDW376_PRIO,
//			(INT16U		 ) APP_CFG_TASK_GDW376_PRIO,
//			(OS_STK		*)&App_Task_GDW376_Stk[0],
//			(INT32U		 ) APP_CFG_TASK_GDW376_STK_SIZE,
//			(void		*) 0,
//			(INT8U		 )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
//
//
	/* 数据运算处理 */
	OSTaskCreateExt((void (*)(void *)) App_Task_DatPro,
			(void		*) 0,
			(OS_STK		*)&App_Task_DatPro_Stk[APP_CFG_TASK_DATPRO_STK_SIZE - 1],
			(INT8U		 ) APP_CFG_TASK_DATPRO_PRIO,
			(INT16U		 ) APP_CFG_TASK_DATPRO_PRIO,
			(OS_STK		*)&App_Task_DatPro_Stk[0],
			(INT32U		 ) APP_CFG_TASK_DATPRO_STK_SIZE,
			(void		*) 0,
			(INT8U		 )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
//
//
//	/* 数据冻结 */
//	OSTaskCreateExt((void (*)(void *)) App_Task_Freeze,
//			(void		*) 0,
//			(OS_STK		*)&App_Task_Fre_Stk[APP_CFG_TASK_FRE_STK_SIZE - 1],
//			(INT8U		 ) APP_CFG_TASK_FRE_PRIO,
//			(INT16U		 ) APP_CFG_TASK_FRE_PRIO,
//			(OS_STK		*)&App_Task_Fre_Stk[0],
//			(INT32U		 ) APP_CFG_TASK_FRE_STK_SIZE,
//			(void		*) 0,
//			(INT8U		 )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));


}



/******************************************************************************
    功能说明：
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
void  BSP_IntDisAll (void)
{
	CPU_IntDis();
}



/******************************************************************************
    功能说明：
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
unsigned  int  BSP_CPU_ClkFreq (void)
{
	static	RCC_ClocksTypeDef  rcc_clocks;

	RCC_GetClocksFreq(&rcc_clocks);

	return ((unsigned  int)rcc_clocks.HCLK_Frequency);
}



/******************************************************************************
    功能说明：
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
INT32U	OS_CPU_SysTickClkFreq (void)
{
	INT32U	freq;

	freq = BSP_CPU_ClkFreq();
	return (freq);
}


/******************************************************************************
    功能说明：
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
static void App_TaskStart (void *p_arg)
{
	(void)p_arg;

	InitHardware();	  //;硬件初始化

	OS_CPU_SysTickInit();

	ZD_RAM_MAK();	  //;写入检测字

	CS_Read();	  //;读出终端的参数

	MMU_FM24_Read();  //;读出FM24C16保存的配置数据

	ZD_Stat_Init();	  //;初始化终端状态

	App_EventCreate();

	App_TaskCreate();

	while (1) {
		task_main();
		OSTimeDlyHMSM(0, 0, 1, 0);
	}
}


/******************************************************************************
    功能说明：
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int main (void)
{
	/* 关闭所有中断 */
	BSP_IntDisAll();

	OSInit();

	OSTaskCreateExt((void (*)(void *)) App_TaskStart,
			(void		*) 0,
			(OS_STK		*)&App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE - 1],
			(INT8U		 ) APP_CFG_TASK_START_PRIO,
			(INT16U		 ) APP_CFG_TASK_START_PRIO,
			(OS_STK		*)&App_TaskStartStk[0],
			(INT32U		 ) APP_CFG_TASK_START_STK_SIZE,
			(void		*) 0,
			(INT16U		 )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

	OSStart();
}

