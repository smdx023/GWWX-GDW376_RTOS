
/*
 * task_gdw_376.h
 * likejshy@126.com
 * 2017-01-06
 */
 
#ifndef _TASK_GDW_376_H
#define _TASK_GDW_376_H


struct PtlMsgStr
{
	char *buff;
	int len;
};


extern OS_EVENT *PtlMboxFd;
int wlan_data_pend(char *buff, int size, int ovt);
int wlan_data_post(char *buff, int len);
extern void App_Task_Ptl(void *p_arg);
 
 
#endif

