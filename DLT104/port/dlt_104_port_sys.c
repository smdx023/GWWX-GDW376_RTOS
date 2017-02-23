/******************************************************************************
    版权所有：
    版 本 号: 1.0
    文 件 名: dlt_104_port_sys.c
    生成日期: 
    作	 者：李科
    功能说明：实现《配电自动化系统应用DL/T634.5104-2009实施细则》与系统数据层接口
    其他说明：
    修改记录：
*******************************************************************************/
#include <stdio.h>
#include <string.h>

//#include "file_dir.h"
#define CFG_DEBUG

#ifdef CFG_DEBUG
    #define Print(fmt,args...) printf(fmt, ##args)
#else
    #define Print(fmt,args...)
#endif

char sys_cp56time2a[7];
 
/******************************************************************************
    功能说明：
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
char dlt_104_get_dir_file_num(char *dir_name)
{
	//char num;

	//num = file_dir_num_get(dir_name, 0);

	return 0;
}



/******************************************************************************
    功能说明：
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int dlt_104_get_dir_info(char *dir_name,
			 char file_index,
			 char *file_name_len,
			 char *file_name,
			 unsigned long *file_size,
			 char *file_time)
{
//	struct file_info info = {0};
//	int ret;
//
//	/* 获取目录下的所有文件信息 */
//	ret = file_dir_info(dir_name, file_index, &info);
//	if (ret == 1) {
//		*file_name_len = info.name_len;
//		memcpy(file_name, info.name, info.name_len);
//		*file_size = info.size;
//		memcpy(file_time, info.time, 7);
//		return 1;
//	}

	return 0;
}


/******************************************************************************
    功能说明：
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int dlt_104_get_file_info(char *file_name,
			  char file_name_len,
			  unsigned long *file_size,
			  char *id,
			  char *file_time)
{
//	struct file_info info = {0};
//	int ret;
//
//	ret = file_dir_file_info(file_name, &info);
//	if (ret == 0) {
//		*file_size = info.size;
//		memcpy(id, info.id, 4);
//		memcpy(file_time, info.time, 7);
//		return 1;
//	}

	return 0;
}



/******************************************************************************
    功能说明：
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int dlt_104_read_file_segment(char *file_name,
			      unsigned long file_len,
			      unsigned long segment_index,
			      char *segment,
			      char *segment_len,
			      char *flag)
{
//	int len;
//
//	len = file_dir_file_segment_read(file_name, segment_index, segment, flag);
//	if (len > 0) {
//		*segment_len = len;
//		return 1;
//	}

	return 0;
}




/******************************************************************************
    功能说明：
    输入参数：
    输出参数：
    返 回 值：
*******************************************************************************/
int dlt_104_save_file_segment(char *file_name,
			      unsigned long file_len,
			      unsigned long segment_index,
			      char *segment,
			      char segment_len)
{
//	int ret;
//
//	ret = file_dir_file_segment_write(file_name, segment_index,
//					  segment, segment_len);
//	if (ret == 0) {
//		return 0;
//	}

	return -1;
}



char dlt_104_get_param(char *txbuf, unsigned char *point_num_out, 
		char *asdu_data, unsigned char *adsu_len, 
		unsigned char point_num_in)
{
	return 0;
}




int dlt_104_set_param(char *data, unsigned char num, char *type)
{
	return 0;
}


int dlt_104_get_elect(char *data, unsigned char SQ, unsigned char *num_out, unsigned char sub_num)
{
	return 0;
}



int user_data_read(char TI, char SQ, char *txbuf, unsigned char *num)
{
	//unsigned char len;
	
	/* 单点信息 */
	if (TI == 1)
	{
		if (SQ == 1) {
			txbuf[0] = 0x01;
			txbuf[1] = 0x00;
			txbuf[2] = 0x00;
			txbuf[3] = 0x01;
			*num = 1;
			return 4;
		}	
	}	

	/* 测量值，归一化值*/
	if (TI == 9)
	{
		if (SQ == 1) {
			txbuf[0] = 0x01;
			txbuf[1] = 0x40;
			txbuf[2] = 0x00;

			txbuf[3] = 0x01;
			txbuf[4] = 0x00;
			
			txbuf[5] = 0x00; /* QS */	
			*num = 1;
			return 6;
		}	
	}
		
	return 0;
}



int dlt_104_cp56time2a_read(char *cp56time2a)
{
	memcpy(cp56time2a, sys_cp56time2a, 7);
	return 0;
}

int dlt_104_cp56time2a_write(char *cp56time2a)
{
	memcpy(sys_cp56time2a, cp56time2a, 7);
	return 0;
}



struct soe_yx
{
	unsigned short point_add;
	unsigned char yx_data;
};


int user_data_YX_change(unsigned char *num, char *buff, 
		unsigned char TI, unsigned char SQ)
{
		


	return 0;
}




int user_data_YX_clear(void)
{
	return 0;
}


int user_data_YC_change(unsigned char *num, char *buff, unsigned char TI, unsigned char SQ)
{
	return 0;
}


