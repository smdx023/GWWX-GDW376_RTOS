
/*
 * task_wlan.c
 * likejshy@126.com
 * 2017-01-08
 */

#include <string.h>
#include <stdio.h>
#include <ucos_ii.h>

#include "noway_m590.h"
#include "GDW376_CFG.h"
#include "task_wlan.h"
#include "task_ptl.h"


/* WLAN缓存区大小 */
#define WLAN_BUFF_SIZE 512

OS_EVENT *WlanMboxFd;

struct wlan_info
{
	struct m590info cfg;
	struct m590tcp  tcp;
	char link_status;
} ;


static struct wlan_info wlan;


int wlan_info_link_status(void)
{
	return 	wlan.link_status;
}




static int wlan_init(void)
{
	WlanMboxFd = OSMboxCreate (NULL);
	if (WlanMboxFd != 0)
		return 0;	
	return -1;
}


static int wlan_tcp_data_post(char *buff, int len)
{
	struct WlanMsgStr WlanMsg;
	struct PtlMsgStr *PtlMsg = 0;
	INT8U ret, perr;
	unsigned short ovt_ms;
	
	/* post message to ptl task */
	WlanMsg.buff = buff;
	WlanMsg.len = len;		
	ovt_ms = 1000;
	do {
		ret = OSMboxPost (WlanMboxFd, &WlanMsg);
		ovt_ms--;
		if (ovt_ms == 0)
			return -1;		
		OSTimeDly(1);
	} while (ret != OS_ERR_NONE);
	
	/* wait until the message is unpacked  by ptl task */
	PtlMsg = OSMboxPend (PtlMboxFd, 1000, &perr);
	if ((perr == OS_ERR_NONE) && (PtlMsg != 0))
		return 0;
	
	return -1;
}


static int wlan_tcp_data_pend(char *buff, int size, int ovt)
{
	struct WlanMsgStr WlanMsg;
	struct PtlMsgStr *PtlMsg;	
	int len;
	INT8U ret, perr;
	
	PtlMsg = OSMboxPend (PtlMboxFd, ovt, &perr);
	if (perr != OS_ERR_NONE) 
		return -1;
	
	if ((PtlMsg != 0) && (PtlMsg->len > 0)) {			
		len = PtlMsg->len;
		if (len > size)
			len = size;
		memcpy(buff, PtlMsg->buff, len);	
		WlanMsg.buff = 0;
		WlanMsg.len = 0;		

		ret = OSMboxPost (WlanMboxFd, &WlanMsg);
		if (ret == OS_ERR_NONE)
			return len;
	} 
	
	return 0;	
}



static int wlan_tcp_client(struct wlan_info *info)
{
        char buff[WLAN_BUFF_SIZE] = {0};
	int len = 0;
        int ret;
	int time = 0;	
	 	
	/* 模块初始化 */
        ret = m590_init();
        if (ret < 0) {
                return -1;
        }

        ret = m590_config(&info->cfg);
        if (ret < 0) {
                return -1;
        }
      
	sprintf(info->tcp.severip, "%d.%d.%d.%d", ZDP.F3.IP1[0], ZDP.F3.IP1[1],
		ZDP.F3.IP1[2],ZDP.F3.IP1[3]);
	info->tcp.severport = ZDP.F3.Port1[1] * 0x100 + ZDP.F3.Port1[1];
	memcpy(info->tcp.apn, ZDP.F3.APN, 16);
	
	strcpy(info->tcp.severip, "122.228.19.57\0");
	info->tcp.severport = 34299;
	strcpy(info->tcp.apn, "CMNET\0");
	
	/* 建立TCP连接 */
        ret = m590_tcp_link(&info->tcp);
        if (ret < 0) {
                return -1;
        }
	
	info->link_status = 1;
	
        while (1) {                		
		/* 从服务器接收TCP数据 */
                len = m590_read(buff, WLAN_BUFF_SIZE, 100);
       		if (len > 0) {			
			/* 当收到服务器数据后，超时清零 */
                        time = 0;									
			/* 将接收到的数据发送规约任务进行数据解析 */
			wlan_tcp_data_post(buff, len);			
			memset(buff, 0, WLAN_BUFF_SIZE);
		} else if (len == 0) {
            		/*
                	 * TCP没有收到服务器数据后，超时计时累加
                	 * 超时大于定值就认为TCP链路断开了，需要
                	 * 重新发起连接
                	 */
                	time++;
                	if (time > 1000) {
                	        time = 0;
                	        info->link_status = 0;
				return -1;
                	}		
		}  else {
			info->link_status = 0;
			return -1;
		}	
		
		/* 将从规约任务接收到的数据采用tcp发送 */
		len = wlan_tcp_data_pend(buff, WLAN_BUFF_SIZE, 100);
		if (len > 0) {
			/* 发送TCP数据 */
               		ret = m590_write(buff, len);
			if (ret < 0) {
				info->link_status = 0;
               			return -1;
			}
               	}
        }
}


void App_Task_Wlan(void *p_arg)
{
        char mode = 0;
	
	wlan_init();
	
	for ( ;; ) {
                /* diff app mode */
		switch (mode) {
                case 0:
                        wlan_tcp_client(&wlan);
                        break;
                case 1:
                        //wlan_tcp_server(&wlan);
                        break;
                case 2:
                        //wlan_masege(&wlan);
                        break;			
                default:
                        break;
                }
		
		OSTimeDlyHMSM(0, 0, 0, 1);	
        }
}



