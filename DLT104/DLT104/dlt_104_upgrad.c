/******************************************************************************
    版权所有：
    版 本 号: 1.0
    文 件 名: dlt_104_upgrad.c
    生成日期: 
    作	 者：李科
    功能说明：实现《配电自动化系统应用DL/T634.5104-2009实施细则》的软件升级
    其他说明：基于104规约被控站的方式来与控制站进行软件升级
    修改记录：
*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "dlt_104_lib.h"
#include "dlt_104_port_sys.h"
#include "dlt_104_port_com.h"


struct upgrad_str {
	char FCB;   /* 记录接收到的启动站报文的帧计数，用于应答启动站 */
	char FCV;

	char M_FCB; /* 作为启动站，保存*/
	char M_FCV;

	struct dtl_ovt ovt;
};


static struct upgrad_str process;




/******************************************************************************
    功能说明：升级启动命令 -->
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
static int upgrad_start_cmd(char *rxbuf, int len)
{
	char TYP, COT;
	char C, DIR, PRM, FCB, FCV, FC;
	char SE;
	int index;

	index = 4;
	C = rxbuf[index++];

	DIR = C >> 7;  /* 0: 下行报文; 1上行报文 */
	if (DIR != 0) {
		return -1;
	}

	PRM = C >> 6;
	if (PRM != 1) { /* 0: 从动站; 1:启动站 */
		return -1;
	}

	FCB = C >> 5;
	FCV = C >> 4;
	process.FCB = FCB; /* 记录好帖计数位 */
	process.FCV = FCV;

	FC = C & 0x0F;	   /* 功能码 */
	if (FC != 3) {
		return -1;
	}

	index += 2; /* A */

	TYP = rxbuf[index++];
	if (TYP != 211) {
		return -1;
	}

	/* 可变长度限定词 */
	index += 1;

	/*传输原因 */
	COT = rxbuf[index++];
	if (COT != 6) {
		return -1;
	}

	index += 2; /* ASDU公用地址 */

	index += 2; /* 信息体地址 */

	SE = rxbuf[index++] >> 7; /* S/E  */
	if (SE != 1) {
		return -1;
	}

	return 0;
}


/******************************************************************************
    功能说明：升级启动命令响应 <--
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
static int upgrad_start_cmd_res(char *txbuf)
{
	char C, DIR, PRM, FCB, FCV, FC;

	DIR = 1;  /* 0: 下行报文; 1: 上行报文 */
	PRM = 0;  /* 0: 从动站; 1: 启动站 */

	FCB = process.FCB;
	FCV = process.FCV;
	FC = 0;

	C = (DIR << 7) | (PRM << 6) | (FCB << 5)| (FCV << 4) | FC;
	txbuf[4] = C;

	return 0;
}



/******************************************************************************
    功能说明：升级启动确认 <--
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
static int upgrad_start_ack(char *txbuf)
{
	char C, DIR, PRM, FCB, FCV, FC;
	char index;

	DIR = 1; /* 0: 下行报文; 1: 上行报文 */
	PRM = 1; /* 0: 从动站; 1: 启动站 */

	process.M_FCB &= ~0x01;
	FCB = process.M_FCB;

	FCV = 1;
	FC = 3;

	C = (DIR << 7) | (PRM << 6) | (FCB << 5)| (FCV << 4) | FC;

	index = 4;
	txbuf[index++] = C;  /* C */

	index += 2; /* A */

	txbuf[index++] = 211; /* TI */

	txbuf[index++] = 1; /* VSQ */
	txbuf[index++] = 7; /* COT */

	index += 2; /* ASDU */

	txbuf[index++] = 0; /* 信息体地址 */
	txbuf[index++] = 0;

	txbuf[index++] = 1 << 7; /* S/E = 1 */

	return index;
}



/******************************************************************************
    功能说明：升级启动确认响应 -->
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
static int upgrad_start_ack_res(char *rxbuf, int len)
{
	char C, DIR, PRM, FCB, FCV, FC;

	C = rxbuf[1];

	DIR = C >> 7;  /* 0: 下行报文; 1上行报文 */
	if (DIR != 0) {
		return -1;
	}

	PRM = C >> 6;
	if (PRM != 0) { /* 0: 从动站; 1:启动站 */
		return -1;
	}

	FCB = C >> 5;
	FCV = C >> 4;
	if ((FCB != process.M_FCB) && (FCV != 1)) { /* 记录好帖计数位 */
		return -1;
	}

	FC = C & 0x0F; /* 功能码 */
	if (FC != 0) {
		return -1;
	}

	return 0;
}


/******************************************************************************
    功能说明：
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int process_upgrad_start(void)
{
	static char step;
	int ret;

	switch(step) {
	case 0: /* 升级启动命令 --> */
		ret = com_rx(upgrad_start_cmd, &process.ovt);
		if (ret == 1) {
			step++;
		}
		break;

	case 1: /* 升级启动命令响应 <-- */
		ret = com_tx(upgrad_start_cmd_res);
		if (ret == 1) {
			step++;
		}
		break;

	case 2: /* 升级启动确认 <-- */
		ret = com_tx(upgrad_start_ack);
		if (ret == 1) {
			step++;
		}
		break;

	case 3: /* 升级启动确认响应 --> */
		ret = com_rx(upgrad_start_ack_res, &process.ovt);
		if (ret == 1) {
			step = 0;
		}
		break;

	default:
		step = 0;
		return -1;
	}

	return 0;
}



/******************************************************************************
    功能说明：升级终止命令 -->
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
static int upgrad_cancel_cmd(char *rxbuf, int len)
{
	char TYP, COT;
	char C, DIR, PRM, FCB, FCV, FC;
	char SE;
	int index;

	index = 4;
	C = rxbuf[index++];

	DIR = C >> 7;  /* 0: 下行报文; 1上行报文 */
	if (DIR != 0) {
		return -1;
	}

	PRM = C >> 6;
	if (PRM != 1) { /* 0: 从动站; 1:启动站 */
		return -1;
	}

	FCB = C >> 5;
	FCV = C >> 4;
	process.FCB = FCB; /* 记录好帖计数位 */
	process.FCV = FCV;

	FC = C & 0x0F;	   /* 功能码 */
	if (FC != 3) {
		return -1;
	}

	index += 2; /* A */

	TYP = rxbuf[index++];
	if (TYP != 211) {
		return -1;
	}

	/* 可变长度限定词 */
	index += 1;

	/*传输原因 */
	COT = rxbuf[index++];
	if (COT != 8) {
		return -1;
	}

	index += 2; /* ASDU公用地址 */

	index += 2; /* 信息体地址 */

	SE = rxbuf[index++] >> 7; /* S/E  */
	if (SE != 0) {
		return -1;
	}

	return 0;
}


/******************************************************************************
    功能说明：升级终止命令响应 <--
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
static int upgrad_cancel_cmd_res(char *txbuf)
{
	char C, DIR, PRM, FCB, FCV, FC;

	DIR = 1;  /* 0: 下行报文; 1: 上行报文 */
	PRM = 0;  /* 0: 从动站; 1: 启动站 */

	FCB = process.FCB;
	FCV = process.FCV;
	FC = 0;

	C = (DIR << 7) | (PRM << 6) | (FCB << 5)| (FCV << 4) | FC;
	txbuf[4] = C;

	return 0;
}



/******************************************************************************
    功能说明：升级终止确认 <--
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
static int upgrad_cancel_ack(char *txbuf)
{
	char C, DIR, PRM, FCB, FCV, FC;
	char index;

	DIR = 1; /* 0: 下行报文; 1: 上行报文 */
	PRM = 1; /* 0: 从动站; 1: 启动站 */

	process.M_FCB &= ~0x01;
	FCB = process.M_FCB;

	FCV = 1;
	FC = 3;

	C = (DIR << 7) | (PRM << 6) | (FCB << 5)| (FCV << 4) | FC;

	index = 4;
	txbuf[index++] = C;  /* C */

	index += 2; /* A */

	txbuf[index++] = 211; /* TI */

	txbuf[index++] = 1; /* VSQ */
	txbuf[index++] = 9; /* COT */

	index += 2; /* ASDU */

	txbuf[index++] = 0; /* 信息体地址 */
	txbuf[index++] = 0;

	txbuf[index++] = 0 << 7; /* S/E = 1 */

	return index;
}



/******************************************************************************
    功能说明：升级终止确认响应 -->
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
static int upgrad_cancel_ack_res(char *rxbuf, int len)
{
	char C, DIR, PRM, FCB, FCV, FC;

	C = rxbuf[1];

	DIR = C >> 7;  /* 0: 下行报文; 1上行报文 */
	if (DIR != 0) {
		return -1;
	}

	PRM = C >> 6;
	if (PRM != 0) { /* 0: 从动站; 1:启动站 */
		return -1;
	}

	FCB = C >> 5;
	FCV = C >> 4;
	if ((FCB != process.M_FCB) && (FCV != 1)) { /* 记录好帖计数位 */
		return -1;
	}

	FC = C & 0x0F; /* 功能码 */
	if (FC != 0) {
		return -1;
	}

	return 0;
}



/******************************************************************************
    功能说明：
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int process_upgrad_cancel(void)
{
	static char step;
	int ret;

	switch(step) {
	case 0: /* 升级终止命令 --> */
		ret = com_rx(upgrad_cancel_cmd, &process.ovt);
		if (ret == 1) {
			step++;
		}
		break;

	case 1: /* 升级终止命令响应 <-- */
		ret = com_tx(upgrad_cancel_cmd_res);
		if (ret == 1) {
			step++;
		}
		break;

	case 2: /* 升级终止确认 <-- */
		ret = com_tx(upgrad_cancel_ack);
		if (ret == 1) {
			step++;
		}
		break;

	case 3: /* 升级终止确认响应 --> */
		ret = com_rx(upgrad_cancel_ack_res, &process.ovt);
		if (ret == 1) {
			step = 0;
		}
		break;

	default:
		step = 0;
		return -1;
	}

	return step;
}



/******************************************************************************
    功能说明：升级执行命令 -->
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
static int upgrad_execute_cmd(char *rxbuf, int len)
{
	char TYP, COT;
	char C, DIR, PRM, FCB, FCV, FC;
	char SE;
	int index;

	index = 4;
	C = rxbuf[index++];

	DIR = C >> 7;  /* 0: 下行报文; 1上行报文 */
	if (DIR != 0) {
		return -1;
	}

	PRM = C >> 6;
	if (PRM != 1) { /* 0: 从动站; 1:启动站 */
		return -1;
	}

	FCB = C >> 5;
	FCV = C >> 4;
	process.FCB = FCB; /* 记录好帖计数位 */
	process.FCV = FCV;

	FC = C & 0x0F;	   /* 功能码 */
	if (FC != 3) {
		return -1;
	}

	index += 2; /* A */

	TYP = rxbuf[index++];
	if (TYP != 211) {
		return -1;
	}

	/* 可变长度限定词 */
	index += 1;

	/*传输原因 */
	COT = rxbuf[index++];
	if (COT != 6) {
		return -1;
	}

	index += 2; /* ASDU公用地址 */

	index += 2; /* 信息体地址 */

	SE = rxbuf[index++] >> 7; /* S/E  */
	if (SE != 0) {
		return -1;
	}

	return 0;
}


/******************************************************************************
    功能说明：升级执行命令响应 <--
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
static int upgrad_execute_cmd_res(char *txbuf)
{
	char C, DIR, PRM, FCB, FCV, FC;

	DIR = 1;  /* 0: 下行报文; 1: 上行报文 */
	PRM = 0;  /* 0: 从动站; 1: 启动站 */

	FCB = process.FCB;
	FCV = process.FCV;
	FC = 0;

	C = (DIR << 7) | (PRM << 6) | (FCB << 5)| (FCV << 4) | FC;
	txbuf[4] = C;

	return 0;
}



/******************************************************************************
    功能说明：升级执行确认 <--
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
static int upgrad_execute_ack(char *txbuf)
{
	char C, DIR, PRM, FCB, FCV, FC;
	char index;

	DIR = 1; /* 0: 下行报文; 1: 上行报文 */
	PRM = 1; /* 0: 从动站; 1: 启动站 */

	process.M_FCB &= ~0x01;
	FCB = process.M_FCB;

	FCV = 1;
	FC = 3;

	C = (DIR << 7) | (PRM << 6) | (FCB << 5)| (FCV << 4) | FC;

	index = 4;
	txbuf[index++] = C;  /* C */

	index += 2; /* A */

	txbuf[index++] = 211; /* TI */

	txbuf[index++] = 1; /* VSQ */
	txbuf[index++] = 7; /* COT */

	index += 2; /* ASDU */

	txbuf[index++] = 0; /* 信息体地址 */
	txbuf[index++] = 0;

	txbuf[index++] = 0 << 7; /* S/E = 1 */

	return index;
}



/******************************************************************************
    功能说明：升级执行确认响应 -->
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
static int upgrad_execute_ack_res(char *rxbuf, int len)
{
	char C, DIR, PRM, FCB, FCV, FC;

	C = rxbuf[1];

	DIR = C >> 7;  /* 0: 下行报文; 1上行报文 */
	if (DIR != 0) {
		return -1;
	}

	PRM = C >> 6;
	if (PRM != 0) { /* 0: 从动站; 1:启动站 */
		return -1;
	}

	FCB = C >> 5;
	FCV = C >> 4;
	if ((FCB != process.M_FCB) && (FCV != 1)) { /* 记录好帖计数位 */
		return -1;
	}

	FC = C & 0x0F; /* 功能码 */
	if (FC != 0) {
		return -1;
	}

	return 0;
}



/******************************************************************************
    功能说明：
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int process_upgrad_execute(void)
{
	static char step;
	int ret;

	switch(step) {
	case 0: /* 升级执行命令 --> */
		ret = com_rx(upgrad_execute_cmd, &process.ovt);
		if (ret == 1) {
			step++;
		}
		break;

	case 1: /* 升级执行命令响应 <-- */
		ret = com_tx(upgrad_execute_cmd_res);
		if (ret == 1) {
			step++;
		}
		break;

	case 2: /* 升级执行确认 <-- */
		ret = com_tx(upgrad_execute_ack);
		if (ret == 1) {
			step++;
		}
		break;

	case 3: /* 升级执行确认响应 --> */
		ret = com_rx(upgrad_execute_ack_res, &process.ovt);
		if (ret == 1) {
			step = 0;
		}
		break;

	default:
		step = 0;
		return -1;
	}

	return step;
}


/******************************************************************************
    功能说明：
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int dlt_104_process_upgrad(void)
{
	int ret;

	/* 启动升级命令过程 */
	ret = process_upgrad_start();
	if (ret < 0) {
		return -1;
	}

	/* 撤销升级命令过程 */
	ret = process_upgrad_cancel();
	if (ret < 0) {
		return -1;
	}

	/* 执行（升级/结束）命令过程 */
	ret = process_upgrad_execute();
	if (ret < 0) {
		return -1;
	}

	return 0;
}


/******************************************************************************
    功能说明：
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int dlt_104_process_upgrad_init(void)
{
	memset((char *)&process, 0, sizeof(process));
	return 0;
}