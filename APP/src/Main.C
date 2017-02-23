/*
********************************************************************************
*  �� �� ����Main.c
*
*  �ļ�������������
*
*  ����оƬ��stm32F103RCT6/101RCT6, FLASH 256K, RAM 48K
*
*  �� �� �ˣ�023
*
*  �� �� �ţ�V1.00 �������߱�ģ�����
*
*  �޸ļ�¼��2012-03-26 11:05:08 ����֧�ֿ�������F38�����ն����ӣ��˿������
*				  �ͳɹ��������ñ�����վIP�Ͷ˿ڵ�¼��ͬʱ֧
*				  �ֶ��Ż��ѣ�����ΪLINKTOZHGC*IP,�˿ں�*������
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
    ����˵����
    ���������
    ���������
    �� �� ֵ��
*******************************************************************************/
int task_main(void)
{
	// InitHardware();   //;Ӳ����ʼ��
	// ZD_RAM_MAK();     //;д������
	// CS_Read();	     //;�����ն˵Ĳ���
	// MMU_FM24_Read();  //;����FM24C16�������������
	// ZD_Stat_Init();   //;��ʼ���ն�״̬
//;    ERC14_Judge(2);	 //;�ն�����  zrt 20130801�ŵ�������������
	while(1) {
		ZD_POW_CHK();	    //;�����紦��
		ZD_HWareRes_CHK();  //;���Ӳ���Ƿ�Ҫ����
		//ZD_CSSAVE_CHK();    //;����ն˲�������
		//USART_Com();	    //;����ͨѶ@like
		//ZD_Port_Com()     //;�ն˶˿�ͨѶ@like
		//Modem_Com();	      //;Gprsģ��ͨѶ @like
		AFNComDeal();	    //;GDW130��Լ������Զ��ͨѶ

		if(RTC_FlagMs) {   //;250msִ��һ��
			RTC_FlagMs = 0;

//			SendFirstByte_UART5();
//			U5DS.TIndex = 1;
//			U5DS.TLong  = 20;


			//ClockTimer();
			//ZD_DJPN();	//;���������ݶ���
			//ZD_SB();      //@like
			//ZD_CB();	//;�ն˳�������@like
		//	ZD_EventRead();
			//ZD_Relay();	//;����ת��
		//	Modem_GDCDLightCtr();
		//	ZD_ERC();	//;�����¼�
			ZD_Task();	//;��������
			if(RTC_FlagSec) { //;ÿ��ִ��һ��
				RTC_FlagSec = 0;
				LinkStat_CHK();
				//Watchdog();
				//Send_Modem_Status();
				//ZD_CB_CHK();	  //;�����������ʱ@like
				//ZD_DJPN_CHK();	  //;�ն˶�����
				//ZD_Mast_Link();	  //;�ն�����վ����
				if(RTC_FlagMin) { //;ÿ����ִ��һ��
					RTC_FlagMin = 0;
					////ZD_Task_CHK();
					//ZDPN_MXT();   //;���յ�����ֵ�뷢��ʱ�����
					//ZDPN_YXT();   //;���յ���Խ��ʱ�����
					//LinkStat_CHK();
				//	Modem_LinkTime();
				}
			}

		}
	}
}



/******************************************************************************
    ����˵����
    ���������
    ���������
    �� �� ֵ��
*******************************************************************************/
static void App_EventCreate (void)
{
	;
}



/******************************************************************************
    ����˵����
    ���������
    ���������
    �� �� ֵ��
*******************************************************************************/
static void App_TaskCreate (void)
{
	/* �������� */
	OSTaskCreateExt((void (*)(void *)) App_Task_Meter,
			(void		*) 0,
			(OS_STK		*)&App_TaskCBStk[APP_CFG_TASK_CB_STK_SIZE - 1],
			(INT8U		 ) APP_CFG_TASK_CB_PRIO,
			(INT16U		 ) APP_CFG_TASK_CB_PRIO,
			(OS_STK		*)&App_TaskCBStk[0],
			(INT32U		 ) APP_CFG_TASK_CB_STK_SIZE,
			(void		*) 0,
			(INT16U		 )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

//	/* GPRSͨ��ģ��������� */
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
//	/* ��վ���ݽ��� */
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
	/* �������㴦�� */
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
//	/* ���ݶ��� */
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
    ����˵����
    ���������
    ���������
    �� �� ֵ��
*******************************************************************************/
void  BSP_IntDisAll (void)
{
	CPU_IntDis();
}



/******************************************************************************
    ����˵����
    ���������
    ���������
    �� �� ֵ��
*******************************************************************************/
unsigned  int  BSP_CPU_ClkFreq (void)
{
	static	RCC_ClocksTypeDef  rcc_clocks;

	RCC_GetClocksFreq(&rcc_clocks);

	return ((unsigned  int)rcc_clocks.HCLK_Frequency);
}



/******************************************************************************
    ����˵����
    ���������
    ���������
    �� �� ֵ��
*******************************************************************************/
INT32U	OS_CPU_SysTickClkFreq (void)
{
	INT32U	freq;

	freq = BSP_CPU_ClkFreq();
	return (freq);
}


/******************************************************************************
    ����˵����
    ���������
    ���������
    �� �� ֵ��
*******************************************************************************/
static void App_TaskStart (void *p_arg)
{
	(void)p_arg;

	InitHardware();	  //;Ӳ����ʼ��

	OS_CPU_SysTickInit();

	ZD_RAM_MAK();	  //;д������

	CS_Read();	  //;�����ն˵Ĳ���

	MMU_FM24_Read();  //;����FM24C16�������������

	ZD_Stat_Init();	  //;��ʼ���ն�״̬

	App_EventCreate();

	App_TaskCreate();

	while (1) {
		task_main();
		OSTimeDlyHMSM(0, 0, 1, 0);
	}
}


/******************************************************************************
    ����˵����
    ���������
    ���������
    �� �� ֵ��
*******************************************************************************/
int main (void)
{
	/* �ر������ж� */
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
