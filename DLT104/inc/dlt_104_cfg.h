
/*
 * dlt_104_cfg.h
 * config for dlt_104
 * likejshy@126.com
 * 2017-01-16
 */

#ifndef _CFG_DLT_104_H
#define _CFG_DLT_104_H

#define CFG_SYS_UCOS
//#define CFG_SYS_LINUX

#define PORT_COM  0
#define PORT_WLAN 1
#define PORT_ETH  1

#define MAX_PORT 2

#define MAX_PROCESS   20

/* 所有104过程优先级 */
#define INIT_PRIO 	0
#define CALL_PRIO 	1
#define TIME_PRIO 	2
#define YX_PRIO 	3
#define YC_PRIO 	4
#define AREA_READ_PRIO  5
#define AREA_SWITCH_PRIO 6
#define PARAM_SET_PRIO 	7
#define PARAM_READ_PRIO 8
#define ELECT_PRIO 	9
#define TEST_LINK_PRIO 	10
#define S_FRAME_PRIO 	11


/* 遥信地址 */
//#define 0x0001 


#endif
