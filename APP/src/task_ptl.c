
/*
 * task_ptl.c
 * likejshy@126.com
 * 2017-01-08
 */

#include <string.h>
#include <stdio.h>
#include <ucos_ii.h>

#include "GDW376_CFG.h"
#include "task_wlan.h"
#include "task_ptl.h"
#include "dlt_104.h"
#include "dlt_104_port_com.h"
#include "dlt_104_cfg.h"


#define PTL_BUFF_SIZE 512


OS_EVENT *PtlMboxFd;
struct PtlMsgStr PtlMsg;


static int task_ptl_init(void)
{
	PtlMboxFd = OSMboxCreate (NULL);
	if (PtlMboxFd != 0)
		return 0;	
	return -1;
}


int wlan_data_post(char *buff, int len)
{
	struct PtlMsgStr PtlMsg;
	struct WlanMsgStr *WlanMsg = 0;
	INT8U ret, perr;
	unsigned short ovt_ms;
	
	/* post measssge to Wlan task */
	PtlMsg.buff = buff;
	PtlMsg.len = len;		
	ovt_ms = 1000;
	do {
		ret = OSMboxPost (PtlMboxFd, &PtlMsg);
		ovt_ms--;
		if (ovt_ms == 0)
			return -1;		
		OSTimeDly(1);
	} while (ret != OS_ERR_NONE);
	
	/* wait until the message is copy by wlan task */
	WlanMsg = OSMboxPend (WlanMboxFd, 1000, &perr);
	if ((perr == OS_ERR_NONE) && (WlanMsg != 0))
		return 0;
	
	return -1;
}



int wlan_data_pend(char *buff, int size, int ovt)
{
	struct PtlMsgStr PtlMsg;
	struct WlanMsgStr *WlanMsg;	
	int len;
	INT8U ret, perr;
	
	/* pend the tcp data from wlan task */
	WlanMsg = OSMboxPend (WlanMboxFd, ovt, &perr);
	if (perr != OS_ERR_NONE) 
		return -1;
	
	if ((WlanMsg != 0) && (WlanMsg->len > 0)) {			
		len = WlanMsg->len;
		if (len > size)
			len = size;
		memcpy(buff, WlanMsg->buff, len);	
		
		PtlMsg.buff = 0;
		PtlMsg.len = 0;		

		ret = OSMboxPost (PtlMboxFd, &PtlMsg);
		if (ret == OS_ERR_NONE)
			return len;
	} 
	
	return 0;	
}


//这里写了个与wlan任务通信的测试函数
int wlan_comm_test(void)
{
	char buff[PTL_BUFF_SIZE];
	int len;
	int ret;
	
	while (1) {		
		len = wlan_data_pend(buff, PTL_BUFF_SIZE, 1000);
		if (len > 0) {			
			ret  = wlan_data_post(buff, len);
			if (ret < 0) 
				return -1;
		}
	}
}



void App_Task_Ptl(void *p_arg)
{
        task_ptl_init();
	dlt_104_init(PORT_COM);
	
	for ( ;; ) {		
		//ptl_gdw_376_test();
		dlt_104(PORT_COM);
        }
}

