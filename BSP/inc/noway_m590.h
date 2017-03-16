

/* 
 * noway m590 
 * likejshy@126.com
 * 2017-01-01
 */


#ifndef  _HUAWEI_M590_H
#define  _HUAWEI_M590_H



/* 命令 */
enum M590CMD {
    M590_COMINT,
    M590_POWERON,
    M590_POWEROFF,
    M590_TURNON,
    M590_TURNOFF,
    M590_RESET,
    M590_INIT,
    M590_ATTEST,
    M590_GETCSQ,
    M590_TCPLINK,
    M590_TCPTX,
    M590_TCPRX,
    M590_TCPCHK,
    M590_TCPCOM,
    M590_TCPCLOSD,
    M590_UARTTX,
    M590_UARTRX,
};


struct m590com {       /* M590通信结构 */
    char *dmarxbuf;     /* 模块AT命令DMA接收缓存 */
    int dmarxbufsize;   /* 模块AT命令DMA接收缓存大小 */
};


struct m590info {      /* M590模块初始化信息 */
    char bandrate[7];   /* 通讯波特率 */
    char software_v[20];/* 模块软件版本号*/
    char imei[16];      /* IMEI号 */
    char imsi[16];      /* SIM卡的IMSI号*/
    char simcpin[15];   /* SIM卡PIN鉴权 */
    char netreg[2];     /* 网络注册状况 */
    char opt[20];       /* 运营商 */
    char csq[1];        /* 信号值 */
};


struct m590tcp {       /* M590模块TCP连接参数 */
    char csq[1];        /* 信号值 */
    char gprsstat[2];   /* 附着GPRS网络状态 */
    char apn[17];       /* APN接入点 */
    char linkstat[2];   /* 模块internet连接状态 */
    char localip[16];   /* 本地IP */
    char severip[16];   /* 服务端IP */
    unsigned short severport;  /* 服务端port */
    char tcpstat[2];    /* 模块TCP连接状态 */
    char linkflag;      /* 连接标志*/
    int socket;
};


struct m590csq { 
    char csqval;        /* 信号值 */
};


struct m590tcptx {     /* TCP发送数据结构 */
    char *txbuf;        /* TCP数据发送数据缓存 */
    int txlen;          /* TCP数据发送数据长度 */
};


struct m590tcprx {     /* TCP接收数据结构 */
    char *rxbuf;        /* TCP接收数据缓存 */
    int rxsize;         /* 需要接收长度 */
    int rxlen;          /* 实际接收长度 */
};


struct m590tcpchk {    /* TCP链路检测 */
    char linkflag;      /* 连接标志*/
};


struct m590uartrx {    /* 串口接收数据结构 */
    char *rxbuf;        /* 串口接收数据缓存 */
    int rxsize;         /* 串口接收缓存区长度 */
    int rxlen;          /* 串口实际接收长度 */
};


struct m590uarttx {    /* 串口发送数据结构 */
    char *txbuf;        /* 串口数据发送数据缓存 */
    int txsize;         /* 串口发送缓存区长度 */
    int txlen;          /* 串口数据发送数据长度 */
};


struct m590str
{
    char *dmarxbuf;             /* 保存dam接收缓存区，由应用来决定 */
    int dmarxbufsize;           /* 保存dam接收缓存区大小，由应用来决定 */
    struct m590tcpchk tcpchk;
    struct m590tcptx tcptx;
    struct m590tcprx tcprx;
    unsigned char mutex;    /* 模块操作权信号量 */
    unsigned char prio;     /* 当前正在操作模块的任务 */
};


extern struct dev_ops m590_ops;
int m590_init(void);
int m590_config(struct m590info *info);
int m590_tcp_link(struct m590tcp *tcp);
int m590_read(char *rxbuf, int size, int ovt);
int m590_write(char *wbuf, int wlen);
int m590_tcp_listen(struct m590tcp *tcp);
int m590_server_write(char *wbuf, int wlen);
int m590_server_read(char *rxbuf, int size, int ovt);


#endif /* _HUAWEI_M590_H */



