/*
********************************************************************************
*  文 件 名：
*
*  文件描述：
*
*  所用芯片：
*
*  创 建 人：
*
*  版 本 号：
*
*  修改记录：
********************************************************************************
*/
#ifndef __Function_CFG_H
#define __Function_CFG_H

//;看门狗开关
//#define IWDGON

//;此配置为USB迷你串口配置开关 ;关闭开关，端口都为推挽输出
//#define JTDK_USART1_ON  //;串口监听端口1
#define Local_USART1_ON //;本地串口是否关闭

//;GPRS无数据或无连接计时
#define LINKTIME

//;#define ZS  //;调试记录GPRS拨号信息开关
//;IAP程序
//#define IAP

//;#define PORT2USART  //;不需要端口2

//;定义单片机型号
#define STM32F10XXC


//;版本号
#define VERSION        '1','.','2','6'
#define VERSION_DATE   0x10, 0x01, 0x15

#define  PNMAXNUM    1     //;最大支持测量点数目
#if PNMAXNUM >10
    #error "too many PNMAXNUM"
#endif

#define GPRSUSARTCTR   U2DS
#define GPRSREVBUF     AllBuf.Rx2_2
#define GPRSSEDBUF     AllBuf.Tx2_1
//;#define U2BUFSIZE      550
#define U2TXBUFSIZE      1024
#define U2RXBUFSIZE      512
#define U2FRAMEDIST    15

#define INFUSARTCTR    U3DS
#define INFREVBUF      AllBuf.Rx3_2
#define INFSEDBUF      AllBuf.Tx3_1
#define U3BUFSIZE      512
#define U3FRAMEDIST    25

/*
#define PORT1USARTCTR  U4DS
#define PORT1REVBUF    AllBuf.Rx4_2
#define PORT1SEDBUF    AllBuf.Tx4_1
#define U4BUFSIZE      256
#define U4FRAMEDIST    15

#define PORT2USARTCTR  U5DS
#define PORT2REVBUF    AllBuf.Rx5_2
#define PORT2SEDBUF    AllBuf.Tx5_1
#define U5BUFSIZE      256
#define U5FRAMEDIST    15
*/

#define PORT1USARTCTR   U5DS            //;新版国网表 串口互调 串口5为抄表端口 20141111
#define PORT1REVBUF    AllBuf.Rx5_2
#define PORT1SEDBUF    AllBuf.Tx5_1
#define U4BUFSIZE      256
#define U4FRAMEDIST    15

#define PORT2USARTCTR  U4DS
#define PORT2REVBUF    AllBuf.Rx4_2
#define PORT2SEDBUF    AllBuf.Tx4_1
#define U5BUFSIZE      256
#define U5FRAMEDIST    15

//;#define PORT1USARTCTR  U5DS
//;#define PORT1REVBUF    AllBuf.Rx5_2
//;#define PORT1SEDBUF    AllBuf.Tx5_1
//;#define U5BUFSIZE      256
//;#define U5FRAMEDIST    15
//;
//;#define PORT2USARTCTR  U4DS
//;#define PORT2REVBUF    AllBuf.Rx4_2
//;#define PORT2SEDBUF    AllBuf.Tx4_1
//;#define U4BUFSIZE      256
//;#define U4FRAMEDIST    15

#define LOCREVBUF     AllBuf.Rx1_2
#define LOCSEDBUF     AllBuf.Tx1_1
#define LOCUSARTCTR   U1DS
//#define U1BUFSIZE      550
#define U1BUFSIZE      1024
#define U1FRAMEDIST    15

#endif
