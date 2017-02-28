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

//#include "w25q64.h"


int storage_write(unsigned long addr, char *data, int size)
{   
    //SPI_Flash_Write((unsigned char*)data, addr,(u16)size);    
    return 0;
}


int storage_read(unsigned long addr, char *data, int size)
{
    //SPI_Flash_Read((unsigned char*)data, addr, (u16)size);   
    return size;
}