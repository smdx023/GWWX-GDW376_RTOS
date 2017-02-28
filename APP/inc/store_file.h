
//store files in exflash
//likejshy@126.com
//2016-12-16

#ifndef _STORE_FILE_H
#define _STORE_FILE_H


extern int store_file_creat(char *file_name);
extern int store_file_write(int file_id, char *buff, int len);
extern int store_file_close(int file_id);
extern int store_file_open(int *file_id, char *file_name, unsigned long *file_size);
extern int store_file_read(int file_id, unsigned long offset, char *buff, int size);
extern int store_file_clear(int file_id);

#endif