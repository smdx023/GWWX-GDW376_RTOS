/******************************************************************************
    版权所有：
    版 本 号: 1.0
    文 件 名: store_param.h
    生成日期: 
    作    者：李科
    功能说明：数据存储
    其他说明：
    修改记录：
*******************************************************************************/
#ifndef _STORE_PARAM
#define _STORE_PARAM


extern int store_param_save(char *name, char *data, char len);
extern int store_param_read(char *name, char *data);



#endif /* _STORE_PARAM */
