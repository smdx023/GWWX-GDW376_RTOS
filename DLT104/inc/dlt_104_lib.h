/******************************************************************************
    版权所有：
    版 本 号: 1.0
    文 件 名: dlt_104_common.h
    生成日期: 2016.09.20
    作    者：李科
    功能说明：实现《配电自动化系统应用DL/T634.5104-2002实施细则》的通用模块
    其他说明：
    修改记录：
*******************************************************************************/
#ifndef _PTL_104_LIB
#define _PTL_104_LIB

#include "dlt_104_cfg.h"

#define BIT0(x) (x & 0x01)
#define BIT1(x) ((x >> 1) & 0x01)
#define BIT2(x) ((x >> 2) & 0x01)
#define BIT3(x) ((x >> 3) & 0x01)
#define BIT4(x) ((x >> 4) & 0x01)
#define BIT5(x) ((x >> 5) & 0x01)
#define BIT6(x) ((x >> 6) & 0x01)
#define BIT7(x) ((x >> 7) & 0x01)


struct dlt_lib {
	char status;
	unsigned int time_cout_s;
	unsigned int ovt_time_s;
	unsigned short NR;
	unsigned short NS;
	unsigned short asdu;
};

extern struct dlt_lib  dlt_lib_arg[MAX_PORT];
extern unsigned char dlt_104_cs(char *buf, int len);
extern int dlt_104_frame_read(unsigned char port, char *frame, char *rxbuf, int *rxlen);
extern void dlt_104_apci_pack(unsigned char port, char *buf, int len, char type);
extern int dlt_104_fix_frame_pack(char *buf, unsigned short link_addr);
extern void print_frame(char port, char *buff, int len, char TXRX);
extern char dlt_104_frame_type(char *frame, int len);
extern int dlt_104_lib_init(unsigned char port);

#endif /* _PTL_104_ */
