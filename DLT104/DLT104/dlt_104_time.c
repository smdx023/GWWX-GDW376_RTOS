/******************************************************************************
    版权所有：
    版 本 号: 1.0
    文 件 名: dlt_104_time.c
    生成日期: 
    作	 者： 李科
    功能说明：实现《配电自动化系统应用DL/T634.5104-2009实施细则》
    其他说明：时钟同步过程
    修改记录：
*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "dlt_104_lib.h"
#include "dlt_104_port_sys.h"
#include "dlt_104_port_com.h"
#include "dlt_104_cfg.h"


#define PRIO 2


//#define CFG_DEBUG

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

	char syn_step;
	char read_step;
	

};


static struct param_process arg[MAX_PORT];


static int dlt_104_time_syn(unsigned char port, char *rxbuf, int len)
{
	char TYP, VSQ, ASDU, COT;
	struct dlt_lib  *arg = &dlt_lib_arg[port];
	unsigned int point_addr = 0;
	
	Print("unpack time syn cmd\r\n");	

	TYP = rxbuf[6]; /* TI */
	if (TYP != 103) {
		Print("unpack time syn TI error\r\n");		
		return 0;
	}

	/* 可变长度限定词 */
	VSQ = rxbuf[7];
	if (VSQ != 1) {
		Print("unpack time syn VSQ error\r\n");	
		return 0;
	}

	/*传输原因 */
	COT = rxbuf[9] * 0x100 + rxbuf[8];
	if (COT != 6) {
		Print("unpack time syn COT error\r\n");	
		return 0;
	}
	
	/* ASDU 公共地址 */
	ASDU = rxbuf[11] * 0x100 + rxbuf[10];
	if (ASDU != arg->asdu) { 
		Print("unpack time syn asdu error\r\n");	
		return 0;
	}

	/* 信息对象地址  */
	point_addr = rxbuf[14] * 0x10000 + rxbuf[13] * 0x100 + rxbuf[12];
	if (point_addr != 0) { 
		Print("unpack time syn point addr error\r\n");	
		return 0;
	}

	dlt_104_cp56time2a_write(rxbuf + 15);
			
	return 1;
}



static int dlt_104_time_syn_ack(unsigned char port, char *txbuf)
{
	struct dlt_lib  *arg = &dlt_lib_arg[port];
	
	txbuf[0] = 0x68;
	txbuf[1] = 20;
			
	txbuf[2] = 0; /*控制域C */
	txbuf[3] = 0;
	txbuf[4] = 0;
	txbuf[5] = 0;

	txbuf[6] = 103; /* TI */
	txbuf[7] = 1;  /* VSQ */	
	
	txbuf[8] = 7;  /* 传送原因COT 激活确认 */
	txbuf[9] = 0;
 	
	txbuf[10] = (unsigned char)arg->asdu; /* ASDU公共地址 */
	txbuf[11] = (unsigned char)(arg->asdu >> 8);
	
	txbuf[12] = 0; /* 信息对象地址 */
	txbuf[13] = 0;
	txbuf[14] = 0;

	dlt_104_cp56time2a_read(txbuf + 15); /* cp56time2a  7 bytes */

	return 22;
}



int time_syn_process(unsigned char port)
{
	char step = arg[port].syn_step;
	int ret;

	switch(step) {
	case 0: /* time syn --> */
		Print("time syn\r\n");
		ret = com_rx(dlt_104_time_syn, port, PRIO);
		if (ret == 1)
			step++;			
		break;

	case 1: /* time syn ack <-- */
		ret = com_tx(dlt_104_time_syn_ack, port, PRIO);
		if (ret == 1)
			step = 0;
		break;
		
	default:
		step = 0;
		break;
	}

	arg[port].syn_step = step;
	return step;
}


static int dlt_104_time_read(unsigned char port, char *rxbuf, int len)
{
	char TYP, VSQ, ASDU, COT;
	struct dlt_lib  *arg = &dlt_lib_arg[port];
	unsigned int point_addr = 0;
	
	Print("unpack time read\r\n");	

	TYP = rxbuf[6]; /* TI */
	if (TYP != 103) {
		Print("unpack time read TI error\r\n");		
		return 0;
	}

	/* 可变长度限定词 */
	VSQ = rxbuf[7];
	if (VSQ != 1) {
		Print("unpack time read VSQ error\r\n");	
		return 0;
	}

	/*传输原因 */
	COT = rxbuf[9] * 0x100 + rxbuf[8];
	if (COT != 5) {
		Print("unpack time read COT error\r\n");	
		return 0;
	}
	
	/* ASDU 公共地址 */
	ASDU = rxbuf[11] * 0x100 + rxbuf[10];
	if (ASDU != arg->asdu) { 
		Print("unpack time read asdu error\r\n");	
		return 0;
	}

	/* 信息对象地址  */
	point_addr = rxbuf[14] * 0x10000 + rxbuf[13] * 0x100 + rxbuf[12];
	if (point_addr != 0) { 
		Print("unpack time read point addr error\r\n");	
		return 0;
	}
			
	return 1;
}


static int dlt_104_time_read_ack(unsigned char port, char *txbuf)
{
	struct dlt_lib  *arg = &dlt_lib_arg[port];
	
	txbuf[0] = 0x68;
	txbuf[1] = 20;
			
	txbuf[2] = 0; /*控制域C */
	txbuf[3] = 0;
	txbuf[4] = 0;
	txbuf[5] = 0;

	txbuf[6] = 103; /* TI */
	txbuf[7] = 1;  /* VSQ */	
	
	txbuf[8] = 5;  /* 传送原因COT 激活确认 */
	txbuf[9] = 0;
 	
	txbuf[10] = (unsigned char)arg->asdu; /* ASDU公共地址 */
	txbuf[11] = (unsigned char)(arg->asdu >> 8);
	
	txbuf[12] = 0; /* 信息对象地址 */
	txbuf[13] = 0;
	txbuf[14] = 0;

	dlt_104_cp56time2a_read(txbuf + 15); /* cp56time2a  7 bytes */

	return 22;
}



int time_read_process(unsigned char port)
{
	char step = arg[port].read_step;
	int ret;

	switch(step) {
	case 0: /* time read --> */
		ret = com_rx(dlt_104_time_read, port, PRIO);
		if (ret == 1)
			step++;			
		break;

	case 1: /* time read ack <-- */
		ret = com_tx(dlt_104_time_read_ack, port, PRIO);
		if (ret == 1)
			step = 0;
		break;
		
	default:
		step = 0;
		break;
	}

	arg[port].read_step = step;
	return step;
}


int time_process_init(unsigned char port)
{
	memset((char *)&arg[port], 0, sizeof(arg[port]));
	return 0;
}


