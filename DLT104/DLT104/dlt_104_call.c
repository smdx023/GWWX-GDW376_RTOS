/******************************************************************************
    版权所有：
    版 本 号: 1.0
    文 件 名: dlt_104_call.c
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




//#define CFG_DEBUG

#ifdef CFG_DEBUG
    #define Print(fmt,args...) printf(fmt, ##args)
#else
    #define Print(fmt,args...)
#endif


struct param_process {
	char step;
	char FCB;   /* 记录接收到的启动站报文的帧计数，用于应答启动站 */
	char FCV;

	char M_FCB; /* 作为启动站，保存*/
	char M_FCV;

	char TYP;
	char SQ;
	char N;
	char SE;
	
	char no;
};

static struct param_process arg[MAX_PORT];


static int dlt_101_call_cmd(unsigned char port, char *rxbuf, int len)
{
	char TYP, VSQ, ASDU, COT, QOI;
	struct dlt_lib  *arg = &dlt_lib_arg[port];
	unsigned int point_addr = 0;
	
	Print("unpack call cmd\r\n");	

	TYP = rxbuf[6]; /* TI */
	if (TYP != 100) {
		Print("unpack call TI error\r\n");		
		return 0;
	}

	/* 可变长度限定词 */
	VSQ = rxbuf[7];
	if (VSQ != 1) {
		Print("unpack call VSQ error\r\n");	
		return 0;
	}

	/*传输原因 */
	COT = rxbuf[9] * 0x100 + rxbuf[8];
	if (COT != 6) {
		Print("unpack call COT error\r\n");	
		return 0;
	}
	
	/* ASDU 公共地址 */
	ASDU = rxbuf[11] * 0x100 + rxbuf[10];
	if (ASDU != arg->asdu) { 
		Print("unpack call asdu error\r\n");	
		return 0;
	}

	/* 信息对象地址  */
	point_addr = rxbuf[14] * 0x10000 + rxbuf[13] * 0x100 + rxbuf[12];
	if (point_addr != 0) { 
		Print("unpack call point addr error\r\n");	
		return 0;
	}


	QOI = rxbuf[15];
	if (QOI == 20) { 
		Print("unpack call point right\r\n");	
		return 1;
	}
			
	return 0;
}



static int dlt_101_call_ack(unsigned char port, char *txbuf)
{
	struct dlt_lib  *arg = &dlt_lib_arg[port];
	
	txbuf[0] = 0x68;
	txbuf[1] = 14;
			
	txbuf[2] = 0; /*控制域C */
	txbuf[3] = 0;
	txbuf[4] = 0;
	txbuf[5] = 0;

	txbuf[6] = 100; /* TI */
	txbuf[7] = 1;  /* VSQ */	
	
	txbuf[8] = 7;  /* 传送原因COT 激活确认 */
	txbuf[9] = 0;
 	
	txbuf[10] = (unsigned char)arg->asdu; /* ASDU公共地址 */
	txbuf[11] = (unsigned char)(arg->asdu >> 8);
	
	txbuf[12] = 0; /* 信息对象地址 */
	txbuf[13] = 0;
	txbuf[14] = 0;

	txbuf[15] = 20; /* 初始化原因COI */

	return 16;
}




static int dlt_101_call_user_data_YX(unsigned char port, char *txbuf)
{
	struct dlt_lib  *arg = &dlt_lib_arg[port];
	unsigned char VSQ = 0, SQ = 1, num = 0, TI = 0, len = 0;

	txbuf[0] = 0x68;
			
	txbuf[2] = 0; /*控制域C */
	txbuf[3] = 0;
	txbuf[4] = 0;
	txbuf[5] = 0;

	txbuf[6] = 1; /* TI 单点信息 */
	TI = 1;
	
	txbuf[8] = 5;  /* 传送原因COT 被请求*/
	txbuf[9] = 0;
 	
	txbuf[10] = (unsigned char)arg->asdu; /* ASDU公共地址 */
	txbuf[11] = (unsigned char)(arg->asdu >> 8);
	
	/* read user data, yaoce, yaoxin */
	len = user_data_read_YX(TI, SQ, txbuf + 12, &num);
	VSQ = (SQ << 7) | num;
	txbuf[7] = VSQ;  /* VSQ */
	
	txbuf[1] = len + 10;

	return len + 12;
}



static int dlt_101_call_user_data_YC(unsigned char port, char *txbuf)
{
	struct dlt_lib  *arg = &dlt_lib_arg[port];
	unsigned char VSQ = 0, SQ = 1, num = 0, TI = 0, len = 0;

	txbuf[0] = 0x68;
			
	txbuf[2] = 0; /*控制域C */
	txbuf[3] = 0;
	txbuf[4] = 0;
	txbuf[5] = 0;

	txbuf[6] = 13; /* TI 测量值，IEEE STD745 短浮点数 */
	TI = 9;
	
	txbuf[8] = 5;  /* 传送原因COT 被请求*/
	txbuf[9] = 0;
 	
	txbuf[10] = (unsigned char)arg->asdu; /* ASDU公共地址 */
	txbuf[11] = (unsigned char)(arg->asdu >> 8);
	
	/* read user data, yaoce, yaoxin */
	len = user_data_read_YC(TI, SQ, txbuf + 12, &num);
	VSQ = (SQ << 7) | num;
	txbuf[7] = VSQ;  /* VSQ */
	
	txbuf[1] = len + 10;

	return len + 12;
}



static int dlt_101_call_end(unsigned char port, char *txbuf)
{
	struct dlt_lib  *arg = &dlt_lib_arg[port];
	
	txbuf[0] = 0x68;
	txbuf[1] = 14;
			
	txbuf[2] = 0; /*控制域C */
	txbuf[3] = 0;
	txbuf[4] = 0;
	txbuf[5] = 0;

	txbuf[6] = 100; /* TI */
	txbuf[7] = 1;  /* VSQ */	
	
	txbuf[8] = 10;  /* 传送原因COT 激活终止*/
	txbuf[9] = 0;
 	
	txbuf[10] = (unsigned char)arg->asdu; /* ASDU公共地址 */
	txbuf[11] = (unsigned char)(arg->asdu >> 8);
	
	txbuf[12] = 0; /* 信息对象地址 */
	txbuf[13] = 0;
	txbuf[14] = 0;

	txbuf[15] = 20; /* 召唤限定词QOI */

	return 16;
}



int call_process(unsigned char port)
{
	char step = arg[port].step;
	int ret;

	switch(step) {
	case 0: /* call cmd  --> */
		Print("call cmd -->\r\n");	
		ret = com_rx(dlt_101_call_cmd, port, CALL_PRIO);
		if (ret == 1)
			step++;			
		break;

	case 1: /* call ack <-- */
		Print("call ack <--\r\n");
		ret = com_tx(dlt_101_call_ack, port, CALL_PRIO);
		if (ret == 1)
			step++;
		break;

	case 2: /* call user data YX <-- */
		ret = com_tx(dlt_101_call_user_data_YX, port, CALL_PRIO);
		if (ret == 1) /* have user data to send */
			step++;
		break;

	case 3: /* call user data YC <-- */
		ret = com_tx(dlt_101_call_user_data_YC, port, CALL_PRIO);
		if (ret == 1) /* have user data to send */
			step++;
		break;

	case 4: /* call end <-- */
		ret = com_tx(dlt_101_call_end, port, CALL_PRIO);
		if (ret == 1) 
			step = 0;
		break;
		
	default:
		step = 0;
	}

	arg[port].step = step;
	return step;
}


int call_process_init(unsigned char port)
{
	memset((char *)&arg[port], 0, sizeof(arg[port]));
	return 0;
}


