/******************************************************************************
    版权所有：
    版 本 号: 1.0
    文 件 名: dlt_104_elect.c
    生成日期: 
    作	 者： 李科
    功能说明：实现《配电自动化系统应用DL/T634.5104-2009实施细则》
    其他说明：电能量召唤过程
    修改记录：
*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "dlt_104_lib.h"
#include "dlt_104_port_sys.h"
#include "dlt_104_port_com.h"
#include "dlt_104_cfg.h"





//#define CFG_DEBUG

#ifdef CFG_DEBUG
    #define Print(fmt,args...) printf(fmt, ##args)
#else
    #define Print(fmt,args...)
#endif



struct param_process {

	char TYP;
	char SQ;
	char N;
	char SE;
	
	char num;

	char elect_step;
};


static struct param_process arg[MAX_PORT];



/* 68 0E 04 00 04 00 65 81 06 00 01 00 00 00 00 05 
   68 0E 00 00 00 00 65 81 06 00 01 00 00 00 00 05 */
static int dlt_104_elect_call(unsigned char port, char *rxbuf, int len)
{
	char TYP, VSQ, ASDU, COT, QCC;
	struct dlt_lib  *arg = &dlt_lib_arg[port];
	unsigned int point_addr = 0;
	
	Print("unpack elect_call\r\n");	

	TYP = rxbuf[6]; /* TI */
	if (TYP != 101) {
		Print("unpack elect_call TI error\r\n");		
		return 0;
	}

	/* 可变长度限定词 */
	VSQ = (rxbuf[7] >> 7) & 0x01;
	if (VSQ != 1) {
		Print("unpack elect_call VSQ error\r\n");	
		return 0;
	}

	/*传输原因 */
	COT = rxbuf[9] * 0x100 + rxbuf[8];
	if (COT != 6) {
		Print("unpack elect_call COT error\r\n");	
		return 0;
	}
	
	/* ASDU 公共地址 */
	ASDU = rxbuf[11] * 0x100 + rxbuf[10];
	if (ASDU != arg->asdu) { 
		Print("unpack elect_call asdu error\r\n");	
		return 0;
	}

	/* 信息对象地址  */
	point_addr = rxbuf[14] * 0x10000 + rxbuf[13] * 0x100 + rxbuf[12];
	if (point_addr != 0) { 
		Print("unpack elect_call point addr error\r\n");	
		return 0;
	}
	
	QCC = rxbuf[15];
	if(QCC == 5)		
		return 1;

	return 0;
}


static int dlt_104_elect_call_ack(unsigned char port, char *txbuf)
{
	struct dlt_lib  *arg = &dlt_lib_arg[port];
	
	txbuf[0] = 0x68;
	txbuf[1] = 14;
			
	txbuf[2] = 0; /*控制域C */
	txbuf[3] = 0;
	txbuf[4] = 0;
	txbuf[5] = 0;

	txbuf[6] = 101; /* TI */
	txbuf[7] = 1;  /* VSQ */	
	
	txbuf[8] = 7;  /* 传送原因COT 激活确认 */
	txbuf[9] = 0;
 	
	txbuf[10] = (unsigned char)arg->asdu; /* ASDU公共地址 */
	txbuf[11] = (unsigned char)(arg->asdu >> 8);
	
	txbuf[12] = 0; /* 信息对象地址 */
	txbuf[13] = 0;
	txbuf[14] = 0;
	
	txbuf[15] = 5;
	
	return 16;
}


static int dlt_104_elect_call_data(unsigned char port, char *txbuf)
{
	struct dlt_lib  *lib_arg = &dlt_lib_arg[port];
	//struct param_process *pro_arg = &arg[port];
	unsigned char len, num = 0, SQ = 1;

	len = dlt_104_get_elect(txbuf + 12, SQ, &num);

	txbuf[0] = 0x68;
	txbuf[1] = 10 + len;
			
	txbuf[2] = 0; /*控制域C */
	txbuf[3] = 0;
	txbuf[4] = 0;
	txbuf[5] = 0;

	txbuf[6] = 206;  /* TI */
	txbuf[7] = (1 << 7) | num;  /* VSQ */	
	
	txbuf[8] = 37;  /* 传送原因COT 响应电能量总召唤 */
	txbuf[9] = 0;
 	
	txbuf[10] = (unsigned char)lib_arg->asdu; /* ASDU公共地址 */
	txbuf[11] = (unsigned char)(lib_arg->asdu >> 8);
	
	return len + 12;
}


static int dlt_104_elect_call_end(unsigned char port, char *txbuf)
{
	struct dlt_lib  *arg = &dlt_lib_arg[port];
	
	txbuf[0] = 0x68;
	txbuf[1] = 14;
			
	txbuf[2] = 0; /*控制域C */
	txbuf[3] = 0;
	txbuf[4] = 0;
	txbuf[5] = 0;

	txbuf[6] = 101; /* TI */
	txbuf[7] = 1;  /* VSQ */	
	
	txbuf[8] = 10;  /* 传送原因COT 激活终止 */
	txbuf[9] = 0;
 	
	txbuf[10] = (unsigned char)arg->asdu; /* ASDU公共地址 */
	txbuf[11] = (unsigned char)(arg->asdu >> 8);
	
	txbuf[12] = 0; /* 信息对象地址 */
	txbuf[13] = 0;
	txbuf[14] = 0;
	txbuf[15] = 5;

	return 16;
}


int elect_process(unsigned char port)
{
	char step = arg[port].elect_step;
	int ret;

	switch(step) {
	case 0: 
		ret = com_rx(dlt_104_elect_call, port, ELECT_PRIO);
		if (ret == 1)
			step++;			
		break;

	case 1: 
		ret = com_tx(dlt_104_elect_call_ack, port, ELECT_PRIO);
		if (ret == 1)
			step++;
		break;
	
	case 2: 
		ret = com_tx(dlt_104_elect_call_data, port, ELECT_PRIO);
		if (ret == 1) /* the all elect data frames send end */
			step++;
		break;
	
	case 3: 
		ret = com_tx(dlt_104_elect_call_end, port, ELECT_PRIO);
		if (ret == 1)
			step = 0;
		break;

	default:
		step = 0;
		break;
	}

	arg[port].elect_step = step;
	return step;
}


int elect_process_init(unsigned char port)
{
	memset((char *)&arg[port], 0, sizeof(arg[port]));
	return 0;
}


