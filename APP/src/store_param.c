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

#include "store.h"
//#include "lib.h"
#include "GDW376_MMU.h"
#include "printf.h"


#define CFG_DEBUG

#ifdef CFG_DEBUG
#define print(fmt,args...) kprintf(fmt, ##args)
#else
#define print(fmt,args...)
#endif



/* 参数保存地址 */
#define STA_ADDR_STORE_PARAM   ((DLT104_PARAM_START) * PAGESIZE) //0x0FF5D0
#define END_ADDR_STORE_PARAM   ((DLT104_PARAM_END) * PAGESIZE)   //0x100A70


#define PARAM_MAX_NUM          100
#define PARAM_MAX_SIZE         64


/* 参数信息 */
struct stroe_param_map
{
    char *name;
    unsigned long addr;
    char size;
};

#pragma pack(1)
struct stroe_param
{
    char len;
    unsigned short cs;
    char data[PARAM_MAX_SIZE];    
};
#pragma pack()


/* 参数存储映射表 */
const struct stroe_param_map param_map[PARAM_MAX_NUM] = 
{
    /* 终端固有参数信息体地址分配表 */
    {"0x8001",  0x0FF5D0L,  16}, /* 终端类型 */
    {"0x8002",  0x0FF5E0L,  16}, /* 终端操作系统 */
    {"0x8003",  0x0FF5F0L,  16}, /* 终端制造商 */
    {"0x8004",  0x0FF600L,  16}, /* 终端硬件版本 */
    {"0x8005",  0x0FF610L,  16}, /* 终端软件版本 */
    {"0x8006",  0x0FF620L,  16}, /* 终端软件版本校验码 */   
    {"0x8007",  0x0FF630L,  16}, /* 终端通信规约类型 */
    {"0x8008",  0x0FF640L,  16}, /* 终端出厂型号 */    
    {"0x8009",  0x0FF650L,  16}, /* 终端ID号 */  
    {"0x800A",  0x0FF660L,  16}, /* 终端网卡MAC地址 */  
    
     /* 终端运行参数信息体地址 */
    {"0x8026",  0x0FF670L,  16}, /* PT一次额定 */
    {"0x8027",  0x0FF680L,  16}, /* PT二次额定 */  

     /* 终端动作定值信息体地址 */
    {"0x8824",  0x0FF690L,  16}, /* X时间定值 */
    {"0x8825",  0x0FF6A0L,  16}, /* Y时间定值 */
    {"0x8826",  0x0FF6B0L,  16}, /* C时间定值 */
    {"0x8827",  0x0FF6C0L,  16}, /* S时间定值 */
};


/* 104规约参数 */
struct dlt_param
{
	unsigned char tag;
	unsigned char len;
        unsigned char val[64];  
};



/* 参数信息 */
struct param_default_tab
{
    char *name;
    unsigned char len;
    struct dlt_param data;
};



/* 参数的出厂默认值 */
const struct param_default_tab param_default[PARAM_MAX_NUM] = 
{
	{"0x8001",   7,  {4,  5,   "GYDLB"} },  	/* 终端类型 */
	{"0x8002",   4,  {4,  7,   "uCOS-II"} },  	/* 终端操作系统 */
};


extern u16 Cal_CRC16(const u8* data, u32 size);



static int __store_param_save(char *name, char *data, char len)
{
    struct stroe_param param = {0}; 
    unsigned long addr;
    char size;
    int ret; 
    int i;
    
    if (len > PARAM_MAX_SIZE)
    {
        return -1;
    }
    
    for (i = 0; i < PARAM_MAX_NUM; i++)
    {
        if (strcmp(name, param_map[i].name) == 0)
        {
            size = param_map[i].size;
            addr = param_map[i].addr;           
            if (len > size)
            {
                return -1;
            } 
            
            param.len = len;
            memcpy(param.data, data, len);
            param.cs = Cal_CRC16((unsigned char *)param.data, len);
            
            len += 3; 
            ret = storage_write(addr, (char *)&param, len);
            if (ret < 0)
            {
                return -1;
            }
            
            return 0;
        }
    }
    
    return -1;
}


static int __store_param_read(char *name, char *data)
{
    struct stroe_param param = {0}; 
    unsigned long addr;
    unsigned short cs;
    char size;
    int len = 0; 
    int i;
    
    for (i = 0; i < PARAM_MAX_NUM; i++)
    {
        if (strcmp(name, param_map[i].name) == 0)
        {
            size = param_map[i].size;
            addr = param_map[i].addr;
            
            len = storage_read(addr, (char *)&param, size);
            if ((len > 0) && (len <= size))
            {
                cs = Cal_CRC16((unsigned char *)param.data, param.len); 
                if ((cs == param.cs) && (cs != 0))               
                {           
                    len = param.len;
                    memcpy(data, param.data,len); 
                    return len;
                }
            }
            
            break;
        }
    }
    
    return 0;
}



int store_param_save(char *name, char *data, unsigned char data_len)
{	
	int ret;
	char temp[64] = {0};
	unsigned char len;

	ret = __store_param_save(name, data, data_len);
	if  (ret < 0)
		return -1;

	len = __store_param_read(name, temp);
	if (len == data_len) {
		if (memcmp(data, temp, data_len) == 0)
			return 0;	
	}
	
	return -1;
}




static int store_param_default(char *name)
{
	unsigned char len;
	char *pdata;
   	int ret; 
   	int i;
        
    	for (i = 0; i < PARAM_MAX_NUM; i++) {
        	if (strcmp(name, param_default[i].name) == 0) {
			
			len = param_default[i].len;
			pdata = (char *)&param_default[i].data;
			
			ret = __store_param_save(name, pdata, len);
			if (ret == 0)
				return 0;
		}
        }
    
    	return -1;
}



///* 读取终端的配置参数 */
//int store_param_config(char *name, char *data)
//{
//	unsigned char len;
//	char *pdata;
//   	int i;
//        
//    	for (i = 0; i < PARAM_MAX_NUM; i++) {
//        	if (strcmp(name, param_config[i].name) == 0) {
//			
//			len = param_config[i].len;
//			pdata = (char *)param_config[i].data;
//			
//			memcpy(data, pdata, len);
//			return len;
//		}
//        }
//    
//    	return 0;
//}



int store_param_read(char *name, char *data)
{
	int len = 0;
	int ret;
	
	len = __store_param_read(name, data);
	if (len == 0) {
		ret = store_param_default(name);
		if (ret == 0)
			len = __store_param_read(name, data);	
	}

	return len;
}



//int test(void)
//{
//	char data[20] = {0};
//	
//	print("STA_ADDR_STORE_PARAM = %X\r\n",STA_ADDR_STORE_PARAM);
//	print("STA_ADDR_STORE_PARAM = %X\r\n",END_ADDR_STORE_PARAM);
//	
//	store_param_save("server_ip", "192.168.1.1", 13);
//	store_param_read("server_ip", data);
//	
//}

