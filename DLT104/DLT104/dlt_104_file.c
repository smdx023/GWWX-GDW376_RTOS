/******************************************************************************
    版权所有：
    版 本 号: 1.0
    文 件 名: dlt_104_file.c
    生成日期: 
    作	 者：李科
    功能说明：实现《配电自动化系统应用DL/T634.5104-2009实施细则》的文件传输
    其他说明：基于104规约被控站的方式来与控制站进行文件传输
    修改记录：
*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "dlt_104_lib.h"
#include "dlt_104_port_sys.h"
#include "dlt_104_port_com.h"


#define SEGMENT_SIZE 225      /* 段数据的最大长度 */


struct file_str {
	char FCB;   /* 记录接收到的启动站报文的帧计数，用于应答启动站 */
	char FCV;

	char M_FCB; /* 作为启动站，保存*/
	char M_FCV;

	char dir_opt;
	char dir_id[4];
	char dir_name_len;
	char dir_name[20];
	char dir_flag;
	char start_time[7];
	char end_time[7];

	unsigned long file_size;
	unsigned long segment_no;
	char finish;

	struct dtl_ovt ovt;
};


static struct file_str process;	   /* 控制文件上传，下载过程所用到的全局变量 */


/******************************************************************************
    功能说明：召唤目录命令 -->
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int file_call_dir_cmd(char *rxbuf, int len)
{
	char TYP, COT;
	char C, DIR, PRM, FCB, FCV, FC;
	unsigned long point_addr;
	char poket_type;
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
	if (TYP != 210) {
		return -1;
	}

	/* 可变长度限定词 */
	index += 1;

	/*传输原因 */
	COT = rxbuf[index++];
	if (COT != 5) {
		return -1;
	}

	index += 2; /* ASDU公用地址 */

	point_addr = rxbuf[index + 1] * 0x100 + rxbuf[index]; /* 信息体地址 */
	index += 2;
	if (point_addr != 0) {
		return -1;
	}

	poket_type = rxbuf[index++]; /* 附加数据包类型 */
	if (poket_type != 2) {
		return -1;
	}

	process.dir_opt = rxbuf[index++]; /* 操作标识 */

	/* 目录标识 */
	memcpy(process.dir_id,	rxbuf + index, 4);
	index += 4;

	process.dir_name_len = rxbuf[index++]; /* 目录名长度 */
	if ((process.dir_name_len < 20) && (process.dir_name_len > 0)) {
		memcpy(process.dir_name, rxbuf + index, process.dir_name_len); /* 目录名 */
		index += process.dir_name_len;
	}

	process.dir_flag = rxbuf[index++]; /* 召唤标志 */

	memcpy(process.start_time, rxbuf + index, 7); /* 查询起始时间 */
	index += 7;

	memcpy(process.end_time, rxbuf + index, 7); /* 查询终止时间 */
	index += 7;

	return 0;
}


/******************************************************************************
    功能说明：召唤目录命令响应 <--
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int file_call_dir_cmd_res(char *txbuf)
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
    功能说明：召唤目录确认 <--
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int file_call_dir_ack(char *txbuf)
{
	char C, DIR, PRM, FCB, FCV, FC;
	char index, i;
	int ret;

	char file_name_len, file_num;
	char file_name[20];
	unsigned long file_size;
	char file_time[7];

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

	txbuf[index++] = 210; /* TI */

	txbuf[index++] = 1; /* VSQ */
	txbuf[index++] = 5; /* COT */

	index += 2; /* ASDU */

	txbuf[index++] = 0; /* 信息体地址 */
	txbuf[index++] = 0;

	txbuf[index++] = 2; /* 附加数据包类型 */

	txbuf[index++] = process.dir_opt; /* 操作标识 */
	txbuf[index++] = 0;  /* 结果描述字 0：成功 1：失败 */

	memcpy(txbuf + index, process.dir_id, 4);  /* 目录ID */
	index += 4;

	txbuf[index++] = 0; /* 后续标志 */

	file_num = dlt_104_get_dir_file_num(process.dir_name);
	txbuf[index++] = file_num; /* 文件数量 */

	for (i = 0; i < file_num; i++) {
		if (i >= 10) {
			break; /* 最多上传10个文件信息 */
		}

		ret = dlt_104_get_dir_info(process.dir_name,
					   i,
					   &file_name_len,
					   file_name,
					   &file_size,
					   file_time);
		if (ret < 0) {
			break;
		}

		txbuf[index++] = file_name_len; /* 文件名称长度 */
		if ((file_name_len > 0) && (file_name_len < 20)) {
			memcpy(txbuf + index, file_name, file_name_len); /* 文件名称 */
			index += file_name_len;
		}

		txbuf[index++] = 0; /* 文件属性 */

		txbuf[index++] = (char)file_size; /* 文件大小 */
		txbuf[index++] = (char)(file_size >> 8);
		txbuf[index++] = (char)(file_size >> 16);
		txbuf[index++] = (char)(file_size >> 24);

		memcpy(txbuf + index, file_time, 7);
		index += 7; /* 文件时间  */
	}

	return index;
}


/******************************************************************************
    功能说明：
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int file_call_dir_ack_res(char *rxbuf, int len)
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
    功能说明：控制方向的文件下载处理
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int process_file_call_dir(void)
{
	static char step;
	int ret;

	switch(step) {
	case 0: /* 召唤目录命令 --> */
		ret = com_rx(file_call_dir_cmd, &process.ovt);
		if (ret == 1) {
			step++;
		}
		break;

	case 1: /* 召唤目录命令响应 <-- */
		ret = com_tx(file_call_dir_cmd_res);
		if (ret == 1) {
			step++;
		}
		break;

	case 2: /* 召唤目录确认 <-- */
		ret = com_tx(file_call_dir_ack);
		if (ret == 1) {
			step++;
		}
		if (ret == 2) {
			step = 0xFF;
		}
		break;

	case 3: /* 召唤目录确认响应 --> */
		ret = com_rx(file_call_dir_ack_res, &process.ovt);
		if (ret == 1) {
			step = 0;
		}
		if (ret == 2) {
			step = 2;
		}
		break;

	default:
		step = 0;
		return -1;
	}

	return step;
}



/******************************************************************************
    功能说明：读文件激活 -->
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int file_upload_activate(char *rxbuf, int len)
{
	char TYP, COT;
	char C, DIR, PRM, FCB, FCV, FC;
	unsigned long point_addr;
	char poket_type;
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
	if (TYP != 210) {
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

	point_addr = rxbuf[index + 1] * 0x100 + rxbuf[index]; /* 信息体地址 */
	index += 2;
	if (point_addr != 0) {
		return -1;
	}

	poket_type = rxbuf[index++]; /* 附加数据包类型 */
	if (poket_type != 2) {
		return -1;
	}

	process.dir_opt = rxbuf[index++]; /* 操作标识 */
	if (process.dir_opt != 3) {
		return -1;
	}


	process.dir_name_len = rxbuf[index++]; /* 文件名长度 */
	if ((process.dir_name_len < 20) && (process.dir_name_len > 0)) {
		memcpy(process.dir_name, rxbuf + index, process.dir_name_len); /* 文件名 */
		index += process.dir_name_len;
	}

	return 0;
}



/******************************************************************************
    功能说明：读文件激活响应 <--
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int file_upload_activate_res(char *txbuf)
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
    功能说明：读文件激活确认 <--
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int file_upload_activate_ack(char *txbuf)
{
	char C, DIR, PRM, FCB, FCV, FC;
	char index;
	int ret;

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

	txbuf[index++] = 210; /* TI */

	txbuf[index++] = 1; /* VSQ */
	txbuf[index++] = 7; /* COT */

	index += 2; /* ASDU */

	txbuf[index++] = 0; /* 信息体地址 */
	txbuf[index++] = 0;

	txbuf[index++] = 2; /* 附加数据包类型 */

	txbuf[index++] = 4;  /* 操作标识 */
	txbuf[index++] = 0;  /* 结果描述字 0：成功 1：失败 */

	txbuf[index++] = process.dir_name_len; /* 文件名称长度 */
	if ((process.dir_name_len > 0)
	    && (process.dir_name_len < 20)) {
		memcpy(txbuf + index, process.dir_name, process.dir_name_len); /* 文件名称 */
		index += process.dir_name_len;
	}

	ret = dlt_104_get_file_info(process.dir_name,
				    process.dir_name_len,
				    &process.file_size,
				    process.dir_id,
				    NULL);
	if (ret < 0) {
		return -1;
	}

	memcpy(txbuf + index, process.dir_id, 4);  /* 文件ID */
	index += 4;

	txbuf[index++] = (char)process.file_size; /* 文件大小 */
	txbuf[index++] = (char)(process.file_size >> 8);
	txbuf[index++] = (char)(process.file_size >> 16);
	txbuf[index++] = (char)(process.file_size >> 24);

	return index;
}



/******************************************************************************
    功能说明：读文件激活确认响应 -->
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int file_upload_activate_ack_res(char *rxbuf, int len)
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
    功能说明：读文件数据传输 <--
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int file_upload_data(char *txbuf)
{
	char C, DIR, PRM, FCB, FCV, FC;
	char index;
	int ret;
	char len;
	char flag = 0;

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

	txbuf[index++] = 210; /* TI */

	txbuf[index++] = 1; /* VSQ */
	txbuf[index++] = 5; /* COT */

	index += 2; /* ASDU */

	txbuf[index++] = 0; /* 信息体地址 */
	txbuf[index++] = 0;

	txbuf[index++] = 2; /* 附加数据包类型 */

	txbuf[index++] = 5;  /* 操作标识 */

	memcpy(txbuf + index, process.dir_id, 4);  /* 文件ID */
	index += 4;

	txbuf[index++] = (char)process.segment_no; /* 段号 */
	txbuf[index++] = (char)(process.segment_no >> 8);
	txbuf[index++] = (char)(process.segment_no >> 16);
	txbuf[index++] = (char)(process.segment_no >> 24);


	ret = dlt_104_read_file_segment(process.dir_name,
					process.dir_name_len,
					process.segment_no,
					txbuf + index + 1,
					&len,
					&flag);
	if (ret < 0) {
		return -1;
	}


	if (flag == 1) {
		txbuf[index++] = 1;  /* 有后续数据 */
		process.finish = 0;
	} else {
		txbuf[index++] = 0;  /* 无后续数据 */
		process.finish = 1;
	}


	process.segment_no++; /* 段号累计 */

	txbuf[index + len] = dlt_104_cs(txbuf + index, len); /* 和校验 */
	index += (len + 1);

	txbuf[index++] = (char)process.file_size; /* 文件大小 */
	txbuf[index++] = (char)(process.file_size >> 8);
	txbuf[index++] = (char)(process.file_size >> 16);
	txbuf[index++] = (char)(process.file_size >> 24);

	return index;
}


/******************************************************************************
    功能说明：读文件数据传输响应 -->
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int file_upload_data_res(char *rxbuf, int len)
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
    功能说明：读文件数据传输确认 -->
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int file_upload_data_ack(char *rxbuf, int len)
{
	char TYP, COT;
	char C, DIR, PRM, FCB, FCV, FC;
	unsigned long point_addr;
	char poket_type;
	int index;
	unsigned long segment_no;
	char dir_id[4];

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
	if (TYP != 210) {
		return -1;
	}

	/* 可变长度限定词 */
	index += 1;

	/*传输原因 */
	COT = rxbuf[index++];
	if (COT != 5) {
		return -1;
	}

	index += 2; /* ASDU公用地址 */

	point_addr = rxbuf[index + 1] * 0x100 + rxbuf[index]; /* 信息体地址 */
	index += 2;
	if (point_addr != 0) {
		return -1;
	}

	poket_type = rxbuf[index++]; /* 附加数据包类型 */
	if (poket_type != 2) {
		return -1;
	}

	process.dir_opt = rxbuf[index++]; /* 操作标识 */
	if (process.dir_opt != 6) {
		return -1;
	}

	memcpy(dir_id, rxbuf + index, 4); /* 文件id*/
	index += 4;

	if  ((process.dir_id[0] != dir_id[0])
	     || (process.dir_id[1] != dir_id[1])
	     || (process.dir_id[2] != dir_id[2])
	     || (process.dir_id[3] != dir_id[3])) {
		return -1;
	}

	segment_no = rxbuf[index + 3] * 0x1000000
		     + rxbuf[index + 2] * 0x10000
		     + rxbuf[index + 1] * 0x100
		     + rxbuf[index]; /* 段号*/
	index += 4;

	if (process.segment_no != segment_no) {
		return -1;
	}

	if (rxbuf[index] != 0) { /* 无后续数据 */
		return -1;
	}

	return 0;
}


/******************************************************************************
    功能说明：读文件数据传输确认 <--
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int file_upload_data_ack_res(char *txbuf)
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
    功能说明：文件上传处理
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int process_file_upload_file(void)
{
	static char step;
	int ret;

	switch(step) {
	case 0: /* 读文件激活 --> */
		ret = com_rx(file_upload_activate, &process.ovt);
		if (ret == 1) {
			step++;
		}
		break;

	case 1: /* 读文件激活响应 <-- */
		ret = com_tx(file_upload_activate_res);
		if (ret == 1) {
			step++;
		}
		break;

	case 2: /* 读文件激活确认 <-- */
		ret = com_tx(file_upload_activate_ack);
		if (ret == 1) {
			step++;
		}
		break;

	case 3: /* 读文件激活确认响应 --> */
		ret = com_rx(file_upload_activate_ack_res, &process.ovt);
		if (ret == 1) {
			step++;
		}
		if (ret == 2) {
			step = 0xFF;
		}
		break;

	case 4: /* 读文件数据传输 <-- */
		ret = com_tx(file_upload_data);
		if (ret == 1) {
			step++;
		}
		break;

	case 5: /* 读文件数据传输响应 --> */
		ret = com_rx(file_upload_data_res, &process.ovt);
		if (ret == 1) {
			if (process.finish == 1) { /* 文件传输完成 */
				process.finish = 0;
				step++;
			} else {
				step = 4;  /* 继续进入读文件数据传输步骤上传文件数据 */
			}
		}
		if (ret == 2) {
			step = 0xFF;
		}
		break;

	case 6: /* 读文件数据传输确认 --> */
		ret = com_rx(file_upload_data_ack, &process.ovt);
		if (ret == 1) {
			step++;
		}
		if (ret == 2) {
			step = 0xFF;
		}
		break;

	case 7: /* 读文件数据传输确认响应 <-- */
		ret = com_tx(file_upload_data_ack_res);
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
    功能说明：写文件激活 -->
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int file_download_activate(char *rxbuf, int len)
{
	char TYP, COT;
	char C, DIR, PRM, FCB, FCV, FC;
	unsigned long point_addr;
	char poket_type;
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
	if (TYP != 210) {
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

	point_addr = rxbuf[index + 1] * 0x100 + rxbuf[index]; /* 信息体地址 */
	index += 2;
	if (point_addr != 0) {
		return -1;
	}

	poket_type = rxbuf[index++]; /* 附加数据包类型 */
	if (poket_type != 2) {
		return -1;
	}

	process.dir_opt = rxbuf[index++]; /* 操作标识 */
	if (process.dir_opt != 7) {
		return -1;
	}

	process.dir_name_len = rxbuf[index++]; /* 文件名长度 */
	if ((process.dir_name_len < 20) && (process.dir_name_len > 0)) {
		memcpy(process.dir_name, rxbuf + index, process.dir_name_len); /* 文件名 */
		index += process.dir_name_len;
	}

	memcpy(process.dir_id, rxbuf + index, 4); /* 文件id */
	index += 4;

	process.file_size = rxbuf[index + 3] * 0x1000000
			    + rxbuf[index + 2] * 0x10000
			    + rxbuf[index + 1] * 0x100
			    + rxbuf[index]; /* 文件大小 */

	return 0;
}


/******************************************************************************
    功能说明：写文件激活响应 <--
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int file_download_activate_res(char *txbuf)
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
    功能说明：写文件激活确认 <--
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int file_download_activate_ack(char *txbuf)
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

	txbuf[index++] = 210; /* TI */

	txbuf[index++] = 1; /* VSQ */
	txbuf[index++] = 7; /* COT */

	index += 2; /* ASDU */

	txbuf[index++] = 0; /* 信息体地址 */
	txbuf[index++] = 0;

	txbuf[index++] = 2; /* 附加数据包类型 */

	txbuf[index++] = 8;  /* 操作标识 */
	txbuf[index++] = 0;  /* 结果描述字 0：成功 1：失败 */

	txbuf[index++] = process.dir_name_len; /* 文件名称长度 */
	if ((process.dir_name_len > 0)
	    && (process.dir_name_len < 20)) {
		memcpy(txbuf + index, process.dir_name, process.dir_name_len); /* 文件名称 */
		index += process.dir_name_len;
	}

	memcpy(txbuf + index, process.dir_id, 4);  /* 文件ID */
	index += 4;

	txbuf[index++] = (char)process.file_size; /* 文件大小 */
	txbuf[index++] = (char)(process.file_size >> 8);
	txbuf[index++] = (char)(process.file_size >> 16);
	txbuf[index++] = (char)(process.file_size >> 24);

	return index;
}


/******************************************************************************
    功能说明：写文件激活确认响应 -->
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int file_download_activate_ack_res(char *rxbuf, int len)
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
    功能说明：写文件数据传输 -->
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int file_download_data(char *rxbuf, int len)
{
	char TYP, COT;
	char C, DIR, PRM, FCB, FCV, FC;
	unsigned long point_addr;
	char poket_type;
	int index;
	unsigned long segment_no;
	char dir_id[4];
	char cs;
	int ret;

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
	if (TYP != 210) {
		return -1;
	}

	/* 可变长度限定词 */
	index += 1;

	/*传输原因 */
	COT = rxbuf[index++];
	if (COT != 5) {
		return -1;
	}

	index += 2; /* ASDU公用地址 */

	point_addr = rxbuf[index + 1] * 0x100 + rxbuf[index]; /* 信息体地址 */
	index += 2;
	if (point_addr != 0) {
		return -1;
	}

	poket_type = rxbuf[index++]; /* 附加数据包类型 */
	if (poket_type != 2) {
		return -1;
	}

	process.dir_opt = rxbuf[index++]; /* 操作标识 */
	if (process.dir_opt != 9) {
		return -1;
	}

	memcpy(dir_id, rxbuf + index, 4); /* 文件id*/
	index += 4;

	if  ((process.dir_id[0] != dir_id[0])
	     || (process.dir_id[1] != dir_id[1])
	     || (process.dir_id[2] != dir_id[2])
	     || (process.dir_id[3] != dir_id[3])) {
		return -1;
	}

	segment_no = rxbuf[index + 3] * 0x1000000
		     + rxbuf[index + 2] * 0x10000
		     + rxbuf[index + 1] * 0x100
		     + rxbuf[index]; /* 段号*/
	index += 4;

	if (process.segment_no != segment_no) {
		return -1;
	}

	if (rxbuf[index++] == 1) { /* 有后续数据 */
		process.finish = 0;
	} else {
		process.finish = 1;
	}

	len = rxbuf[1] - 23; /* 段数据长度 */
	cs = dlt_104_cs(rxbuf + index, len);
	if (cs != rxbuf[index + len + 1]) {
		return -1;
	}

	ret = dlt_104_save_file_segment(process.dir_name,
					process.dir_name_len,
					process.segment_no,
					rxbuf + index,
					len);
	if (ret < 0) {
		return -1;
	}

	process.segment_no++;

	return 0;
}



/******************************************************************************
    功能说明：读文件数据传输响应 <--
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int file_download_data_res(char *txbuf)
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
    功能说明：写文件数据传输确认 <--
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int file_download_data_ack(char *txbuf)
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

	txbuf[index++] = 210; /* TI */

	txbuf[index++] = 1; /* VSQ */
	txbuf[index++] = 5; /* COT */

	index += 2; /* ASDU */

	txbuf[index++] = 0; /* 信息体地址 */
	txbuf[index++] = 0;

	txbuf[index++] = 2; /* 附加数据包类型 */

	txbuf[index++] = 10;  /* 操作标识 */

	memcpy(txbuf + index, process.dir_id, 4);  /* 文件ID */
	index += 4;

	txbuf[index++] = (char)process.segment_no; /* 数据段号 */
	txbuf[index++] = (char)(process.segment_no >> 8);
	txbuf[index++] = (char)(process.segment_no >> 16);
	txbuf[index++] = (char)(process.segment_no >> 24);

	txbuf[index++] = 0;  /* 结果描述字 0：成功 1：失败 */

	return index;
}



/******************************************************************************
    功能说明：写文件数据传输确认 -->
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int file_download_data_ack_res(char *rxbuf, int len)
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
    功能说明：文件下载处理
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int process_file_download_file(void)
{
	static char step;
	int ret;

	switch(step) {
	case 0: /* 写文件激活 --> */
		ret = com_rx(file_download_activate, &process.ovt);
		if (ret == 1) {
			step++;
		}
		break;

	case 1: /* 写文件激活响应 <-- */
		ret = com_tx(file_download_activate_res);
		if (ret == 1) {
			step++;
		}
		break;

	case 2: /* 写文件激活确认 <-- */
		ret = com_tx(file_download_activate_ack);
		if (ret == 1) {
			step++;
		}
		break;

	case 3: /* 写文件激活确认响应 --> */
		ret = com_rx(file_download_activate_ack_res, &process.ovt);
		if (ret == 1) {
			step++;
		}
		if (ret == 2) {
			step = 0xFF;
		}
		break;

	case 4: /* 写文件数据传输 --> */
		ret = com_rx(file_download_data, &process.ovt);
		if (ret == 1) {
			step++;
		}
		if (ret == 2) {
			step = 0xFF;
		}
		break;

	case 5: /* 写文件数据传输响应 <-- */
		ret = com_tx(file_download_data_res);
		if (ret == 1) {
			if (process.finish == 1) { /* 文件传输完成 */
				process.finish = 0;
				step++;
			} else {
				step = 4;  /* 继续进入读文件数据传输步骤上传文件数据 */
			}
		}
		break;

	case 6: /* 写文件数据传输确认 <-- */
		ret = com_tx(file_download_data_ack);
		if (ret == 1) {
			step++;
		}
		break;

	case 7: /* 写文件数据传输确认 --> */
		ret = com_rx(file_download_data_ack_res, &process.ovt);
		if (ret == 1) {
			step = 0;
		}
		if (ret == 2) {
			step = 0xFF;
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
int dlt_104_process_file(void)
{
	int ret;

	/* 召唤目录过程 */
	ret = process_file_call_dir();

	/* 文件上传过程 */
	ret = process_file_upload_file();

	/* 文件下载过程 */
	ret = process_file_download_file();

	return ret;
}


/******************************************************************************
    功能说明：
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int dlt_104_process_file_init(void)
{
	memset((char *)&process, 0, sizeof(process));
	return 0;
}
