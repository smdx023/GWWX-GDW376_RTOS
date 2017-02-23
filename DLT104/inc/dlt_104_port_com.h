/******************************************************************************
    版权所有：
    版 本 号: 1.0
    文 件 名: dlt_104_port_comm.h
    生成日期: 2016.09.20
    作    者：李科
    功能说明：实现《配电自动化系统应用DL/T634.5104-2002实施细则》的通信接口
    其他说明：
    修改记录：
*******************************************************************************/
#ifndef _PTL_104_PORT_COMM_
#define _PTL_104_PORT_COMM_


extern int com_rx(int (*dlt_104_apdu_unpack)(unsigned char port, char *rxbuf, int len), unsigned char port, unsigned char prio);
extern int com_tx(int (*dlt_104_apdu_pack)(unsigned char port, char *txbuf), unsigned char port, unsigned char prio);
extern void dlt_104_receive(unsigned char port);
extern int dlt_104_com_init(unsigned char port);

#endif /* _PTL_104_PORT_COMM_ */
