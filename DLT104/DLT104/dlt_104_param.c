/******************************************************************************
    版权所有：
    版 本 号: 1.0
    文 件 名: dlt_104_param.c
    生成日期: 
    作	 者：李科
    功能说明：实现《配电自动化系统应用DL/T634.5104-2009实施细则》的装载参数
    其他说明：基于104规约被控站的方式来与控制站进行装载参数
    修改记录：
*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "dlt_104_lib.h"
#include "dlt_104_port_sys.h"
#include "dlt_104_port_com.h"
#include "dlt_104_cfg.h"


#define AREA_READ_PRIO 4
#define AREA_SWITCH_PRIO 5
#define PARAM_SET_PRIO 6
#define PARAM_READ_PRIO 7


#define CFG_DEBUG


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

	char COT;
	char CR;
	char num;
	char SE;

	char asdu_data[128];
	unsigned char asdu_len;

	char area_read_step;
	char param_read_step;
	char area_switch_step;
	char param_set_step;
};


static struct param_process arg[MAX_PORT];



static int dlt_104_param_fixed_area_read(unsigned char port, char *rxbuf, int len)
{
	char TYP, VSQ, ASDU, COT;
	struct dlt_lib  *arg = &dlt_lib_arg[port];
	unsigned int point_addr = 0;
	
	Print("unpack fixed_area_read read\r\n");	

	TYP = rxbuf[6]; /* TI */
	if (TYP != 201) {
		Print("unpack fixed_area_read TI error\r\n");		
		return 0;
	}

	/* 可变长度限定词 */
	VSQ = rxbuf[7];
	if (VSQ != 1) {
		Print("unpack fixed_area_read VSQ error\r\n");	
		return 0;
	}

	/*传输原因 */
	COT = rxbuf[9] * 0x100 + rxbuf[8];
	if (COT != 6) {
		Print("unpack fixed_area_read COT error\r\n");	
		return 0;
	}
	
	/* ASDU 公共地址 */
	ASDU = rxbuf[11] * 0x100 + rxbuf[10];
	if (ASDU != arg->asdu) { 
		Print("unpack fixed_area_read asdu error\r\n");	
		return 0;
	}

	/* 信息对象地址  */
	point_addr = rxbuf[14] * 0x10000 + rxbuf[13] * 0x100 + rxbuf[12];
	if (point_addr != 0) { 
		Print("unpack fixed_area_read point addr error\r\n");	
		return 0;
	}
			
	return 1;
}


static int dlt_104_param_fixed_area_read_ack(unsigned char port, char *txbuf)
{
	struct dlt_lib  *arg = &dlt_lib_arg[port];
	
	txbuf[0] = 0x68;
	txbuf[1] = 24;
			
	txbuf[2] = 0; /*控制域C */
	txbuf[3] = 0;
	txbuf[4] = 0;
	txbuf[5] = 0;

	txbuf[6] = 201; /* TI */
	txbuf[7] = 1;  /* VSQ */	
	
	txbuf[8] = 7;  /* 传送原因COT 激活确认 */
	txbuf[9] = 0;
 	
	txbuf[10] = (unsigned char)arg->asdu; /* ASDU公共地址 */
	txbuf[11] = (unsigned char)(arg->asdu >> 8);
	
	txbuf[12] = 0; /* 信息对象地址 */
	txbuf[13] = 0;
	txbuf[14] = 0;

	txbuf[15] = 1; /* 当前定值区区号SN1 */
	txbuf[16] = 0;

	txbuf[17] = 1; /* 终端支持的最小定值区号SN2 */
	txbuf[18] = 0;
	
	txbuf[19] = 1;
	txbuf[20] = 0; /* 终端支持的最大定值区号SN3 */
	txbuf[21] = 0;
	txbuf[22] = 0; 
	txbuf[23] = 0;
	txbuf[24] = 0; 
	txbuf[25] = 0;

	return 26;
}


int param_fixed_area_read_process(unsigned char port)
{
	char step = arg[port].area_read_step;
	int ret;

	switch(step) {
	case 0: 		
		ret = com_rx(dlt_104_param_fixed_area_read, port, AREA_READ_PRIO);
		if (ret == 1)
			step++;			
		break;

	case 1:
		ret = com_tx(dlt_104_param_fixed_area_read_ack, port, AREA_READ_PRIO);
		if (ret == 1)
			step = 0;
		break;
		
	default:
		step = 0;
		break;
	}

	arg[port].area_read_step = step;
	return step;
}


static int dlt_104_param_read(unsigned char port, char *rxbuf, int len)
{
	char TYP, VSQ, ASDU, COT;
	struct dlt_lib  *lib_arg = &dlt_lib_arg[port];
 	struct param_process *pro_arg = &arg[port];
	
	Print("unpack param_read read\r\n");	

	TYP = rxbuf[6]; /* TI */
	if (TYP != 202) {
		Print("unpack param_read TI error\r\n");		
		return 0;
	}

	/* 可变长度限定词 */
	VSQ = rxbuf[7];
	pro_arg->num = VSQ & 0x07; /* 对应信息元素 */

	/*传输原因 */
	COT = rxbuf[9] * 0x100 + rxbuf[8];
	if (COT != 6) {
		Print("unpack param_read COT error\r\n");	
		return 0;
	}
	
	/* ASDU 公共地址 */
	ASDU = rxbuf[11] * 0x100 + rxbuf[10];
	if (ASDU != lib_arg->asdu) { 
		Print("unpack param_read asdu error\r\n");	
		return 0;
	}

	pro_arg->asdu_len = rxbuf[1] - 10;
	memcpy(pro_arg->asdu_data, rxbuf + 12, pro_arg->asdu_len);	
	
	return 1;
}


static int dlt_104_param_read_ack(unsigned char port, char *txbuf)
{
	struct dlt_lib  *lib_arg = &dlt_lib_arg[port];
	struct param_process *pro_arg = &arg[port];
	unsigned char len, num;

	len = dlt_104_get_param(txbuf + 12, &num, pro_arg->asdu_data,
				 &pro_arg->asdu_len, pro_arg->num);
	if (len <= 0)
		return 0;

	txbuf[0] = 0x68;
	txbuf[1] = 10 + len;
			
	txbuf[2] = 0; /*控制域C */
	txbuf[3] = 0;
	txbuf[4] = 0;
	txbuf[5] = 0;

	txbuf[6] = 202; /* TI */
	txbuf[7] = num;  /* VSQ */	
	
	txbuf[8] = 7;  /* 传送原因COT 激活确认 */
	txbuf[9] = 0;
 	
	txbuf[10] = (unsigned char)lib_arg->asdu; /* ASDU公共地址 */
	txbuf[11] = (unsigned char)(lib_arg->asdu >> 8);
	
	return len + 12;
}



int param_read_process(unsigned char port)
{
	char step = arg[port].param_read_step;
	int ret;

	switch(step) {
	case 0: 		
		ret = com_rx(dlt_104_param_read, port, PARAM_READ_PRIO);
		if (ret == 1)
			step++;			
		break;

	case 1:
		ret = com_tx(dlt_104_param_read_ack, port, PARAM_READ_PRIO);
		if (ret == 0)
			step = 0;
		break;
		
	default:
		step = 0;
		break;
	}

	arg[port].param_read_step = step;
	return step;
}


static int dlt_104_param_fixed_area_switch(unsigned char port, char *rxbuf, int len)
{
	char TYP, VSQ, ASDU, COT;
	struct dlt_lib  *lib_arg = &dlt_lib_arg[port];
 	struct param_process *pro_arg = &arg[port];
	unsigned int point_addr = 0;
	

	Print("unpack fixed_area_switch read\r\n");	

	TYP = rxbuf[6]; /* TI */
	if (TYP != 200) {
		Print("unpack fixed_area_switch TI error\r\n");		
		return 0;
	}

	/* 可变长度限定词 */
	VSQ = rxbuf[7];
	if (VSQ != 1) {
		Print("unpack fixed_area_switch VSQ error\r\n");	
		return 0;
	}

	/*传输原因 */
	COT = rxbuf[9] * 0x100 + rxbuf[8];
	if (COT != 6) {
		Print("unpack fixed_area_switch COT error\r\n");	
		return 0;
	}
	
	/* ASDU 公共地址 */
	ASDU = rxbuf[11] * 0x100 + rxbuf[10];
	if (ASDU != lib_arg->asdu) { 
		Print("unpack fixed_area_switch asdu error\r\n");	
		return 0;
	}

	/* 信息对象地址  */
	point_addr = rxbuf[14] * 0x10000 + rxbuf[13] * 0x100 + rxbuf[12];
	if (point_addr != 0) { 
		Print("unpack fixed_area_switch point addr error\r\n");	
		return 0;
	}
	
	/* 定值区号SN */
	pro_arg->asdu_data[0] = rxbuf[15];
	pro_arg->asdu_data[1] = rxbuf[16];
		
	return 1;
}


static int dlt_104_param_fixed_area_switch_ack(unsigned char port, char *txbuf)
{
	struct dlt_lib  *lib_arg = &dlt_lib_arg[port];
	struct param_process *pro_arg = &arg[port];

	txbuf[0] = 0x68;
	txbuf[1] = 24;
			
	txbuf[2] = 0; /*控制域C */
	txbuf[3] = 0;
	txbuf[4] = 0;
	txbuf[5] = 0;

	txbuf[6] = 200; /* TI */
	txbuf[7] = 1;  /* VSQ */	
	
	txbuf[8] = 7;  /* 传送原因COT 激活确认 */
	txbuf[9] = 0;
 	
	txbuf[10] = (unsigned char)lib_arg->asdu; /* ASDU公共地址 */
	txbuf[11] = (unsigned char)(lib_arg->asdu >> 8);
	
	txbuf[12] = 0; /* 信息对象地址 */
	txbuf[13] = 0;
	txbuf[14] = 0;

	txbuf[15] = pro_arg->asdu_data[0]; /* 当前定值区区号SN */
	txbuf[16] = pro_arg->asdu_data[1];
	
	return 17;
}



int param_fixed_area_switch_process(unsigned char port)
{
	char step = arg[port].area_switch_step;
	int ret;

	switch(step) {
	case 0: 		
		ret = com_rx(dlt_104_param_fixed_area_switch, port, AREA_SWITCH_PRIO);
		if (ret == 1)
			step++;			
		break;

	case 1:
		ret = com_tx(dlt_104_param_fixed_area_switch_ack, port, AREA_SWITCH_PRIO);
		if (ret == 1)
			step = 0;
		break;
		
	default:
		step = 0;
		break;
	}

	arg[port].area_switch_step = step;
	return step;
}


static int dlt_104_param_set(unsigned char port, char *rxbuf, int len)
{
	char TYP, VSQ, ASDU, COT;
	struct dlt_lib  *lib_arg = &dlt_lib_arg[port];
 	struct param_process *pro_arg = &arg[port];
	unsigned char SE = 0, num = 0, CR = 0;
	int ret;
	
	Print("unpack param_pro_set\r\n");	

	TYP = rxbuf[6]; /* TI */
	if (TYP != 203) {
		Print("unpack param_pro_set TI error\r\n");		
		return 0;
	}

	/* 可变长度限定词 */
	VSQ = rxbuf[7];
	num = VSQ & 0x07; /* 对应信息元素 */

	/*传输原因 */
	COT = rxbuf[9] * 0x100 + rxbuf[8];
	
	/* ASDU 公共地址 */
	ASDU = rxbuf[11] * 0x100 + rxbuf[10];
	if (ASDU != lib_arg->asdu) { 
		Print("unpack param_pro_set asdu error\r\n");	
		return 0;
	}

	SE = (rxbuf[14] >> 7) & 0x01;
	CR = (rxbuf[14] >> 6) & 0x01;
		
	/* save for ack */
	pro_arg->asdu_len = rxbuf[1] - 10;
	memcpy(pro_arg->asdu_data, rxbuf + 12, pro_arg->asdu_len);
	pro_arg->SE = SE;
	pro_arg->COT = COT;
	pro_arg->CR = CR;
	pro_arg->num = num;

	/* pro set */
	if ((COT == 6) && (SE == 1) && (CR == 0)) {				
		ret = dlt_104_set_param(rxbuf + 12, num, "pro_set");
		if (ret == 0) 	
			return 1;
	}

	/* stop set */
	if ((COT == 8) && (SE == 0) && (CR == 1)) {
		ret = dlt_104_set_param(rxbuf + 12, num, "stop_set");
		if (ret == 0)
			return 1;
	}

	/*  set */
	if ((COT == 6) && (SE == 0) && (CR == 0)) {
		ret = dlt_104_set_param(rxbuf + 12, num, "set");
		if (ret == 0)
			return 1;
	}

	return 0;
}


static int dlt_104_param_set_ack(unsigned char port, char *txbuf)
{
	struct dlt_lib  *lib_arg = &dlt_lib_arg[port];
	struct param_process *pro_arg = &arg[port];
	unsigned char len;
	unsigned char SE = 0, COT = 0, CR = 0;

	SE = pro_arg->SE;
	COT = pro_arg->COT;
	CR = pro_arg->CR;

	/* pro set */
	if ((COT == 6) && (SE == 1) && (CR == 0)) {				
		COT = 7;
		SE = 1;
		CR = 0;
	}

	/* stop set */
	if ((COT == 8) && (SE == 0) && (CR == 1)) {				
		COT = 9;
		SE = 0;
		CR = 1;
	}

	/* set */
	if ((COT == 6) && (SE == 0) && (CR == 0)) {				
		COT = 7;
		SE = 0;
		CR = 0;
	}

	len = pro_arg->asdu_len;
	memcpy(txbuf + 12, pro_arg->asdu_data, pro_arg->asdu_len);

	txbuf[0] = 0x68;
	txbuf[1] = 10 + len;
			
	txbuf[2] = 0; /*控制域C */
	txbuf[3] = 0;
	txbuf[4] = 0;
	txbuf[5] = 0;

	txbuf[6] = 203; /* TI */
	txbuf[7] = pro_arg->num;  /* VSQ */	
	
	txbuf[8] = COT;  /* 传送原因COT 激活确认 */
	txbuf[9] = 0;
 	
	txbuf[10] = (unsigned char)lib_arg->asdu; /* ASDU公共地址 */
	txbuf[11] = (unsigned char)(lib_arg->asdu >> 8);	

	txbuf[14] = SE << 7 | CR << 6;

	return len + 12;
}



int param_set_process(unsigned char port)
{
	char step = arg[port].param_set_step;
	int ret;

	switch(step) {
	case 0: 		
		ret = com_rx(dlt_104_param_set, port, PARAM_SET_PRIO);
		if (ret == 1)
			step++;			
		break;

	case 1:
		ret = com_tx(dlt_104_param_set_ack, port, PARAM_SET_PRIO);
		if (ret == 0)
			step = 0;
		break;
		
	default:
		step = 0;
		break;
	}

	arg[port].param_set_step = step;
	return step;
}



int param_process_init(unsigned char port)
{
	memset((char *)&arg[port], 0, sizeof(arg[port]));
	return 0;
}
