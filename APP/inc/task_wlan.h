
/*
 * task_wlan.h
 * likejshy@126.com
 * 2017-01-06
 */
 
 #ifndef _TASK_WLAN_H
 #define _TASK_WLAN_H


#define WLAN_MSG_SIZE 1024


struct WlanMsgStr
{
	char *buff;
	int len;
};


extern OS_EVENT *WlanMboxFd;
extern void App_Task_Wlan(void *p_arg);
 
 
 #endif