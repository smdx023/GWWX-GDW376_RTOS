/******************************************************************************
    版权所有：
    版 本 号: 1.0
    文 件 名: dlt_104_soe.c
    生成日期: 
    作	 者：李科
    功能说明：实现《配电自动化系统应用DL/T634.5104-2009实施细则》
    其他说明：
    修改记录：
*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "dlt_104_lib.h"
#include "dlt_104_port_sys.h"
#include "dlt_104_port_com.h"
#include "dlt_104_cfg.h"


#define YX_PRIO 3
#define YC_PRIO 4


#define CFG_DEBUG

#ifdef CFG_DEBUG
    #define Print(fmt,args...) printf(fmt, ##args)
#else
    #define Print(fmt,args...)
#endif



struct param_process {
	char FCB;   /* 记录接收到的启动站报文的帧计数，用于应答启动站 */
	char FCV;

	char M_FCB; /* 作为启动站，保存*/
	char M_FCV;

	char TYP;
	char SQ;
	char N;
	char SE;
	
	char no;

	char YX_step;
	char YC_step;
};


static struct param_process arg[MAX_PORT];


static int dlt_104_soe_YX(unsigned char port, char *txbuf)
{
	struct dlt_lib  *arg = &dlt_lib_arg[port];
	unsigned char num = 0, TI, VSQ = 0, SQ = 1;
	int len;
	
	TI = 30; /* TI 带CP56Time2a时标的单点信息 */
	
	/* YX change? */
	len = user_data_YX_change(&num, txbuf + 12, TI, SQ);
	if (len <= 0)
		return 0;					
	
	VSQ = (SQ << 7) | num;
	txbuf[7] = VSQ;  /* VSQ */

	txbuf[0] = 0x68;
	txbuf[1] = 10 + len;
			
	txbuf[2] = 0; /*控制域C */
	txbuf[3] = 0;
	txbuf[4] = 0;
	txbuf[5] = 0;

	txbuf[6] = TI; /* TI */
	
	txbuf[8] = 3;  /* 传送原因COT 突发（自发） */
	txbuf[9] = 0;
 	
	txbuf[10] = (unsigned char)arg->asdu; /* ASDU公共地址 */
	txbuf[11] = (unsigned char)(arg->asdu >> 8);
	
	return 12 + len;
}



static int dlt_104_soe_YC(unsigned char port, char *txbuf)
{
	struct dlt_lib  *arg = &dlt_lib_arg[port];
	unsigned char num = 0, TI, VSQ = 0, SQ = 1;
	int len;
	
	TI = 9; /* TI 测量值，归一化值 */
	
	/* 配电终端主动周期性的向配电主站发送传输遥测数据的ASDU */
	len = user_data_YC_change(&num, txbuf + 12, TI, SQ);
	if (len <= 0)
		return 0;					
	
	VSQ = (SQ << 7) | num;
	txbuf[7] = VSQ;  /* VSQ */

	txbuf[0] = 0x68;
	txbuf[1] = 10 + len;
			
	txbuf[2] = 0; /*控制域C */
	txbuf[3] = 0;
	txbuf[4] = 0;
	txbuf[5] = 0;

	txbuf[6] = TI; /* TI */
	
	txbuf[8] = 3;  /* 传送原因COT 突发（自发） */
	txbuf[9] = 0;
 	
	txbuf[10] = (unsigned char)arg->asdu; /* ASDU公共地址 */
	txbuf[11] = (unsigned char)(arg->asdu >> 8);
	
	return 12 + len;
}


static int dlt_104_soe_YX_ack(unsigned char port, char *rxbuf, int len)
{
	unsigned char L; 
	
	L = rxbuf[1];
	
	/* S frame */
	if (((rxbuf[2] & 0x01) == 1) 
			&& ((rxbuf[4] & 0x01) == 0) 
			&& (L == 4)) {
		
		user_data_YX_clear();

		return 1;
	}
		
	return 0;
}



int soe_YX_process(unsigned char port)
{
	char step = arg[port].YX_step;
	int ret;

	switch(step) {
	case 0: /* if di changed --> */
		ret = com_tx(dlt_104_soe_YX, port, YX_PRIO);
		if (ret == 1)
			step++;			
		break;

	case 1: /* S frame ack <-- */
		ret = com_rx(dlt_104_soe_YX_ack, port, YX_PRIO);
		if (ret == 1)
			step = 0;			
		break;
	
	default:
		step = 0;
		break;
	}

	arg[port].YX_step = step;
	return step;
}



int soe_YC_process(unsigned char port)
{
	char step = arg[port].YX_step;
	int ret;

	switch(step) {
	case 0: 
		ret = com_tx(dlt_104_soe_YC, port, YC_PRIO);		
		if (ret == 1)
			step = 0;			
		break;
	
	default:
		step = 0;
		break;
	}

	arg[port].YX_step = step;
	return step;
}



int soe_process_init(unsigned char port)
{
	memset((char *)&arg[port], 0, sizeof(arg[port]));
	return 0;
}

