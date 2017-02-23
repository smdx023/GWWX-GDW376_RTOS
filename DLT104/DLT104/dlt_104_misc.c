/******************************************************************************
    版权所有：
    版 本 号: 1.0
    文 件 名: dlt_104_misc.c
    生成日期: 
    作	 者：李科
    功能说明：实现《配电自动化系统应用DL/T634.5104-2009实施细则》
    其他说明：链路测试过程
    修改记录：
*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "dlt_104_lib.h"
#include "dlt_104_port_sys.h"
#include "dlt_104_cfg.h"
#include "dlt_104_port_com.h"

#define TEST_LINK_PRIO 0

//#define CFG_DEBUG
#ifdef CFG_DEBUG
    #define Print(fmt,args...) printf(fmt, ##args)
#else
    #define Print(fmt,args...)
#endif


struct param_process {
	char test_link_step;
	char FCB;   /* 记录接收到的启动站报文的帧计数，用于应答启动站 */
	char FCV;

	char M_FCB; /* 作为启动站，保存*/
	char M_FCV;

	char TYP;
	char SQ;
	char N;
	char SE;
};


static struct param_process arg[MAX_PORT];



static int dlt_104_test_link(unsigned char port, char *rxbuf, int len)
{
	char C1; 
	char TESTFR;
	
	C1 = rxbuf[2];
	
	/* U frame */	
	if (((rxbuf[2] & 0x03) == 0x03) 
	    && ((rxbuf[4] & 0x01) == 0)) {
		
 		TESTFR = (C1 >> 6) & 0x03;	
		if (TESTFR == 1) 			
			return 1;
	}
		
	return 0;
}


static int dlt_104_test_link_ack(unsigned char port, char *txbuf)
{
	char C1 = 0, C2 = 0, C3 = 0, C4 = 0; 
	char STARTDT_cmd = 0, TESTFR_cmd = 2, STOPDT_cmd = 0;

	txbuf[0] = 0x68;
	txbuf[1] = 4;
	C1 = 0x03 | STARTDT_cmd << 2 | TESTFR_cmd << 6 | STOPDT_cmd << 4 | 0x03;
	txbuf[2] = C1;
	txbuf[3] = C2;
	txbuf[4] = C3;
	txbuf[5] = C4;

	return 6;
}



int test_link_process(unsigned char port)
{
	char step = arg[port].test_link_step;
	int ret;

	switch(step) {
	case 0: 
		ret = com_rx(dlt_104_test_link, port, TEST_LINK_PRIO);
		if (ret == 1) {
			step++;
		}
		break;

	case 1: 
		ret = com_tx(dlt_104_test_link_ack, port, TEST_LINK_PRIO);
		if (ret == 1) {
			step = 0;
		}
		break;

	default:
		step = 0;
		break;
	}

	arg[port].test_link_step = step;
	return 0;
}



int test_init_process_init(unsigned char port)
{
	memset((char *)&arg[port], 0, sizeof(arg[port]));
	return 0;
}

