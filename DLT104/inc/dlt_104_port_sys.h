/******************************************************************************
    版权所有：
    版 本 号: 1.0
    文 件 名: dlt_104_port.h
    生成日期: 
    作    者：李科
    功能说明：实现104规约的文件传输的接口
    其他说明：
    修改记录：
*******************************************************************************/
#ifndef _PTL_104_PORT_
#define _PTL_104_PORT_



extern int dlt_104_cp56time2a_read(char *cp56time2a);
extern int dlt_104_cp56time2a_write(char *cp56time2a);
extern int user_data_YX_change(unsigned char *num, char *buff, 
		unsigned char TI, unsigned char SQ);

int user_data_YX_clear(void);

extern int user_data_YC_change(unsigned char *num, char *buff, unsigned char TI, unsigned char SQ);

extern int user_data_read(char TI, char SQ, char *txbuf, unsigned char *num);
extern char dlt_104_get_dir_file_num(char *dir_name);
extern int dlt_104_get_dir_info(char *dir_name,
                         char index, 
                         char *file_name_len, 
                         char *file_name,
                         unsigned long *file_size,
                         char *file_time);

extern int dlt_104_get_file_info(char *file_name,
                                 char file_name_len,
                                 unsigned long *file_size, 
                                 char *id, 
                                 char *file_time);


extern int dlt_104_read_file_segment(char *file_name, 
                           unsigned long file_len, 
                           unsigned long segment_index,
                           char *segment,
                           char *segment_len,
                           char *flag);
                          

                      
extern int dlt_104_save_file_segment(char *file_name, 
                           unsigned long file_len, 
                           unsigned long segment_index,
                           char *segment,
                           char len);
                          

char dlt_104_get_param(char *txbuf, unsigned char *ponit_num_out, 
		char *asdu_data, unsigned char *adsu_len, unsigned char point_num_in);

int dlt_104_set_param(char *data, unsigned char num, char *type);
int dlt_104_get_elect(char *data, unsigned char SQ, 
			unsigned char *num_out, unsigned char sub_num);

#endif /* _PTL_104_PORT_ */
