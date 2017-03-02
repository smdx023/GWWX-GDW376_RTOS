/******************************************************************************
    版权所有：
    版 本 号: 1.0
    文 件 名: store.c
    生成日期: 
    作    者：李科
    功能说明：数据存储
    其他说明：
    修改记录：
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <ucos_ii.h>
#include "GDW376_CFG.h"


#define PAGESIZE         528  //;每页保存的字节


unsigned char temp[528];

OS_EVENT *SemStore;

int storage_init(void)
{
 	SemStore = OSSemCreate (1);
	if (SemStore == 0)
		return -1;
	return 0; 
}




int storage_write(unsigned long addr, char *data, int size)
{     
        unsigned short page;
        unsigned short index, len;
        unsigned short write_size = 0;
    	INT8U perr;

	OSSemPend (SemStore, 0, &perr);
	if (perr != OS_ERR_NONE)
		return -1;

        do  {          
                page = addr / PAGESIZE;
                index = addr % PAGESIZE;
                  
                memset(temp, 0, 528);
                SPI_FLASH_PageRead(temp, (u16)page);
                
                len = (PAGESIZE - index >= size) ? size: (PAGESIZE - index);               
                memcpy(temp + index, data + write_size, len);
                        
                SPI_FLASH_PageWrite_E((unsigned char*)data, page);
                             
                SPI_FLASH_PageRead(temp, (u16)page);      
                if (memcmp(data + write_size, temp + index, len) != 0) {
                        write_size = -1; 
			break;
		}	
                
		write_size += len;
		
                addr += write_size;
                             
        } while (write_size < size);      
       
	perr = OSSemPost (SemStore);
	if (perr != OS_ERR_NONE)
		return -1;
	
        return write_size;
}


int storage_read(unsigned long addr, char *data, int size)
{
        unsigned short page;
        unsigned short index, len;
        unsigned short read_size = 0;
    
        do  {          
                page = addr / PAGESIZE;
                index = addr % PAGESIZE;
                  
                memset(temp, 0, 528);
                SPI_FLASH_PageRead(temp, (u16)page);
                
                len = (PAGESIZE - index >= size) ? size: (PAGESIZE - index);               
                memcpy(data + read_size, temp + index,  len);
                read_size += len;
                               
                addr += read_size;
                             
        } while (read_size < size);      
                 
        return read_size;
}
