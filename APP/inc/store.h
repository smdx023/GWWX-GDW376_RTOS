/******************************************************************************
    版权所有：
    版 本 号: 1.0
    文 件 名: store.h
    生成日期: 
    作    者：李科
    功能说明：数据存储
    其他说明：
    修改记录：
*******************************************************************************/
#ifndef _STORE_
#define _STORE_


extern unsigned short crc16(char *buf, int len);
extern int storage_read(unsigned long add, char *data, int size);
extern int storage_write(unsigned long add, char *data, int size);

#endif /* _STORE_ */
