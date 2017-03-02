
/*
 * noway m590
 * likejshy@126.com
 * 2017-01-01
 */

#include <string.h>
#include <stdio.h>
#include <ucos_ii.h>

#include "stm32f10x.h"

#include "Serial.h"
#include "noway_m590.h"


#define DELAYMS(x)		OSTimeDly(x)
#define M590_POWER_ON		GPIO_SetBits(GPIOB, GPIO_Pin_12)
#define M590_POWER_OFF		GPIO_ResetBits(GPIOB, GPIO_Pin_12)

#define M590_TERN_ON_HIGH  	GPIO_ResetBits(GPIOA, GPIO_Pin_8)
#define M590_TERN_ON_LOW    	GPIO_SetBits(GPIOA, GPIO_Pin_8)

#define TXMAXLEN    1024
#define RXMAXLEN    1300


void com_led_flash(void)
{	
	GPIO_ResetBits(GPIOB, GPIO_Pin_8) ; //;网络数据灯开
	OSTimeDly(50);
	GPIO_SetBits(GPIOB, GPIO_Pin_8) ;	
	OSTimeDly(50);
}


void  tcp_link_led_on(void)
{	
	GPIO_ResetBits(GPIOB, GPIO_Pin_9) ; //;网络数据灯开
}

  

void  tcp_link_led_off(void)
{	
	GPIO_SetBits(GPIOB, GPIO_Pin_9) ;	
}


char* memstr(char* full_data, int full_data_len, char* substr)
{
    if (full_data == NULL || full_data_len <= 0 || substr == NULL) {
        return NULL;
    }

    if (*substr == '0') {
        return NULL;
    }

    int sublen = strlen(substr);

    int i;
    char* cur = full_data;
    int last_possible = full_data_len - sublen + 1;
    for (i = 0; i < last_possible; i++) {
        if (*cur == *substr) {
            //assert(full_data_len - i >= sublen);
            if (memcmp(cur, substr, sublen) == 0) {
                //found
                return cur;
            }
        }
        cur++;
    }

    return NULL;
}








/******************************************************************************
    功能说明：接收一条AT命令，
    输入参数：athead：接收AT命令头关键字
	      attail：接收AT命令尾关键字
	      size：接收的最大长度
	      ovt：超时时间, 单位:ms
    输出参数：rxbuf：AT命令地址
    返 回 值：0： 失败；>0：成功
*******************************************************************************/
static int rxatcmd(char *rxbuf, char *athead, char *attail, 
		   int size, int ovt_ms)
{
	int len = 0;
	char *key;

	memset(rxbuf, 0, size);
	
	while (ovt_ms--) {
		len = uart2_receive_packet(rxbuf, size, 250);
		if (len > 0) {
			rxbuf[len] = '\0';
			com_led_flash();
			break;
		}
		
		OSTimeDly(1);
	}
	
	if (len > 0) {
	
		/* want to find the head key word */
		if (athead != 0) {				
			key = memstr(rxbuf, len, athead);
			if (key != NULL) {
				if (key != rxbuf) {
					len -= (key - rxbuf);
					memcpy(rxbuf, key, len);				
				}
				
				/* not want find the tail key word, just return */
				if (attail == NULL)				
					return len;
				
			} else {
				return 0;
			}
		}
	
		/* wnat to find the tail key word */
		if (attail != NULL) {			
			key = memstr(rxbuf + 1, len - 1, attail);
			if (key != NULL)
				return len;
			else
				return 0;
		}
		
		return len;
	}

        return 0;
}


/******************************************************************************
    功能说明：发送一条AT命令，保证连续两条AT命令之间间隔为400ms
    输入参数：atcmd：ATT命令
    输出参数：无
    返 回 值：-1： 失败；0：成功
*******************************************************************************/
static int txatcmd(char *atcmd)
{
	uart2_send_byte(atcmd, strlen(atcmd));
	com_led_flash();
	return 0;
}


/******************************************************************************
    功能说明：AT命令: 设置回显
    输入参数：value：0 ：回显关闭；1：回显开启
    输出参数：无
    返 回 值：-1： 失败；0：成功
*******************************************************************************/
static int at_at_exec(void)
{
	char cmdbuf[64] = {0};

	if (txatcmd("AT\r\n") < 0)
		return -1;
	
	if (rxatcmd(cmdbuf, "OK", 0, 64, 500) <= 0)
		return -1;
	
	return 0;
}


/******************************************************************************
    功能说明：AT命令：关机
    输入参数：无
    输出参数：无
    返 回 值：-1： 失败；0：成功
*******************************************************************************/
static int at_cpwroff_exec(void)
{
	char cmdbuf[64] = {0};

	if (txatcmd("AT+CPWROFF\r\n") < 0)
		return -1;
	
	if (rxatcmd(cmdbuf, "OK", 0, 64, 5000) <= 0)
		return -1;
	
	DELAYMS(5000);
	return 0;
}


/******************************************************************************
    功能说明：ATE  设置回显命令
    输入参数：value：0 ：回显关闭；1：回显开启
    输出参数：无
    返 回 值：-1： 失败；0：成功
*******************************************************************************/
static int at_ate_exec(int value)
{
	char cmdbuf[64] = {0};

	sprintf(cmdbuf, "%s%d\r\n", "ATE", value);
	if (txatcmd(cmdbuf) < 0)
		return -1;
	
	if (rxatcmd(cmdbuf, "OK", 0, 64, 500) <= 0)
		return -1;
	
	return 0;
}


/******************************************************************************
    功能说明：查询产品信息命令
    输入参数：无
    输出参数：无
    返 回 值：-1： 失败；0：成功
*******************************************************************************/
static int at_cgsn_exec(char *imei)
{
	char cmdbuf[64] = {0};

	if (txatcmd("AT+CGSN\r\n") < 0)
		return -1;
	
	if (rxatcmd(cmdbuf, "+CGSN:", "OK", 64, 500) <= 0)
		return -1;
	
	sscanf(cmdbuf, "+CGSN:%sOK", imei);
	
	return 0;
}


/******************************************************************************
    功能说明：设置波特率命令
    输入参数：bandrate：0(自适应波特率)，600 ，1200 ，2400 ，4800，9600，14400，
			19200，28800，38400，57600，115200(默认值) ，230400
    输出参数：无
    返 回 值：-1： 失败；0：成功
*******************************************************************************/
int at_ipr_write(char *bandrate)
{
	char cmdbuf[64] = {0};

	sprintf(cmdbuf, "AT+IPR=%s\r\n", bandrate);
	if (txatcmd(cmdbuf) < 0)
		return -1;
	
	if (rxatcmd(cmdbuf, "OK", 0, 64, 500) <= 0)
		return -1;
	return 0;
}


/******************************************************************************
    功能说明：检查是否存在管理 PIN 码命令
    输入参数：无
    输出参数：code: SIM卡PIN鉴权
    返 回 值：-1： 失败；0：成功
*******************************************************************************/
static int at_cpin_read(char *pin)
{
	char cmdbuf[64] = {0};

	if (txatcmd("AT+CPIN?\r\n") < 0)
		return -1;
	
	if (rxatcmd(cmdbuf, "+CPIN:", "OK\r\n", 64, 500) <= 0)
		return -1;
	
	sscanf(cmdbuf, "+CPIN: %s\r\n", pin);
	
	return 0;
}


/******************************************************************************
    功能说明：查询SIM卡的IMSI
    输入参数：无
    输出参数：code: SIM卡PIN鉴权
    返 回 值：-1： 失败；0：成功
*******************************************************************************/
static int at_ccid_read(char *ccid)
{
	char cmdbuf[64] = {0};

	if (txatcmd("AT+CCID\r\n") < 0)
		return -1;
	
	if (rxatcmd(cmdbuf, "\r\n+CCID:", "\r\n\r\nOK\r\n", 64, 500) <= 0)
		return -1;
	
	sscanf(cmdbuf, "+CCID:%s\r\n\r\nOK\r\n", ccid);
	
	return 0;
}



/******************************************************************************
    功能说明：读软件版本
    输入参数：无
    输出参数：vers: 软件版本
    返 回 值：-1： 失败；0：成功
*******************************************************************************/
static int at_getvers_exec(char *vers)
{
	char cmdbuf[64] = {0};

	if (txatcmd("AT+GETVERS\r\n") < 0)
		return -1;
	
	if (rxatcmd(cmdbuf, "\r\n", "OK\r\n", 64, 500) <= 0)
		return -1;
	
	sscanf(cmdbuf, "\r\n%s\r\n\r\nOK\r\n", vers);
	
	return 0;
}



/******************************************************************************
    功能说明：AT+CSQ 查询信号质量命令
    输入参数：无
    输出参数：csq: 信号值
    返 回 值：-1： 失败；0：成功
*******************************************************************************/
static int at_csq_exec(char *csq)
{
	char cmdbuf[64] = {0};
	int val = 0;

	if (txatcmd("AT+CSQ\r\n") < 0)
		return -1;
	if (rxatcmd(cmdbuf, "+CSQ:", "OK\r\n", 64, 500) <= 0)
		return -1;
	sscanf(cmdbuf, "+CSQ: %d,", &val);
	*csq = (char)val;
	return 0;
}


/******************************************************************************
    功能说明：AT+CREG 查询网络注册状态
    输入参数：无
    输出参数：stat：当前网络注册状态
		    0 ：没有注册，ME现在并没有在搜寻要注册的新的运营商
		    1 ：注册了本地网络
		    2 ：没有注册，但 MS正在搜寻要注册的新的运营商
		    3 ：注册被拒绝
		    4 ：未知原因
		    5 ：注册了漫游网络
    返 回 值：-1：失败；0：成功
*******************************************************************************/
static int at_creg_read(char *stat)
{
	char cmdbuf[64] = {0};

	if (txatcmd("AT+CREG?\r\n") < 0)
		return -1;
	if (rxatcmd(cmdbuf, "+CREG:", "OK\r\n", 64, 500) <=0)
		return -1;
	sscanf(cmdbuf, "+CREG: 0,%s\r\n", stat);
	return 0;
}


/******************************************************************************
    功能说明：查询当前的 GPRS 服务状态
    输入参数：无
    输出参数：无
    返 回 值：-1： 失败；0：成功
*******************************************************************************/
int at_cgatt_read(char *stat)
{
	char cmdbuf[64] = {0};

	if (txatcmd("AT+CGATT?\r\n") < 0)
		return -1;
	if (rxatcmd(cmdbuf, "+CGATT: ", "OK\r\n", 64, 500) <= 0)
		return -1;
	*stat = cmdbuf[8];
	return 0;
}


/******************************************************************************
    功能说明：读取当前已经注册的网络的运营商信息
    输入参数：无
    输出参数：opt:运营商信息
    返 回 值：-1： 失败；0：成功
*******************************************************************************/
int at_cops_read(char *opt)
{
	char cmdbuf[64] = {0};
	int i;

	if (txatcmd("AT+COPS?\r\n") < 0)
		return -1;
	if (rxatcmd(cmdbuf, "+COPS:", "OK\r\n", 64, 500) <= 0)
		return -1;
	for (i = 0; i < 16; i++) {
		if (cmdbuf[12 + i] == '"') {
			opt[i] = '\0';
			break;
		}
		opt[i] = cmdbuf[12 + i];
	}
	return 0;
}



/******************************************************************************
    功能说明：写Internet 服务数据命令, 每包长度限制为1024个字节
    输入参数：id：Profile的标识
	      txbuf：发送数据首地址
	      txlen：发送数据长度
    输出参数：无
    返 回 值：-1： 失败；0：成功
*******************************************************************************/
static int at_tcp_send(const char *id, char *txbuf, int txlen)
{
	char cmdbuf[64] = {0};
	int ret;
	
	if ((txlen > TXMAXLEN) || (txbuf == NULL))
		return -1;
	
	sprintf(cmdbuf, "AT+TCPSEND=%s,%d\r\n", id, txlen);
	if (txatcmd(cmdbuf) < 0)
		return -1;
	
	if (rxatcmd(cmdbuf, "\r\n>", 0, 64, 500) <= 0)
		return -1;

	txbuf[txlen++] = 0x0d;
	
	ret = uart2_send_byte(txbuf, txlen);
	if (ret < 0)
		return -1;
	
	if (rxatcmd(cmdbuf, "OK", 0, 64, 500) <= 0)
		return -1;
	
	return 0;
}


/******************************************************************************
    功能说明：m590模块软关机
    输入参数：无
    输出参数：无
    返 回 值：-1： 失败；0：成功
*******************************************************************************/
static int m590_shutdown(void)
{
	tcp_link_led_off();
	return at_cpwroff_exec();
}


/******************************************************************************
    功能说明：m590模块关电源
    输入参数：无
    输出参数：无
    返 回 值：无
*******************************************************************************/
static int m590_poweroff(void)
{
	M590_POWER_OFF;
	DELAYMS(1000);
	OSTimeDlyHMSM(0, 0, 0, 0);
	return 0;
}


/******************************************************************************
    功能说明：m590模块开电源
    输入参数：无
    输出参数：无
    返 回 值：无
*******************************************************************************/
static int m590_poweron(void)
{
	/* 在打开模块电源前，开机引脚必须设为高电平 */
	M590_TERN_ON_HIGH;
	M590_POWER_ON;
	DELAYMS(1000);
	return 0;
}


/******************************************************************************
    功能说明：m590模块重启
    输入参数：com：端口参数
    输出参数：无
    返 回 值：-1： 失败；0：成功
*******************************************************************************/
int m590_init(void)
{	
	/* 软关机 */
	m590_shutdown();

	/* 关电源 */
	m590_poweroff();

	/* 开电源 */
	m590_poweron();

	/* 模块开机 */
	M590_TERN_ON_HIGH;
	DELAYMS(750);
	M590_TERN_ON_LOW;	
	
	DELAYMS(5000);
	
	/* AT命令测试开机是否成功 */
	if (at_at_exec() < 0)
		return -1;

	return 0;
}


/******************************************************************************
    功能说明：m590模块参数配置
    输入参数：无
    输出参数：info: 模块初始化信息
    返 回 值：-n： 失败；0：成功
*******************************************************************************/
int m590_config(struct m590info *info)
{
	int i;

	/* 关回显 */
	if (at_ate_exec(0))
		return -1;

	/* 查看软件版本等信息 */
	if (at_getvers_exec(info->software_v))
		return -2;	
	
	/* read imei */
	if (at_cgsn_exec(info->imei))
		return -3;
	
	/* 读取SIM的IMSI串号, 检测SIM卡是否就位 */
	for (i = 0; i < 10; i++) {
		if (at_ccid_read(info->imsi) == 0)
			break;
		if (i >= 9)
			return -4;
		DELAYMS(1000);
	}

	/* 检测SIM卡PIN码是否鉴权成功 */
	for (i = 0; i < 10; i++)  {
		memset(info->simcpin, 0, 15);
		if (at_cpin_read(info->simcpin))
			return -4;
		if (strstr(info->simcpin, "READY") != NULL)
			break;
		if (i >= 9)
			return -5;
		DELAYMS(1000);
	}

	/* 检测信号是否正常 */
	for (i = 0; i < 20; i++)  {
		if (at_csq_exec(info->csq) < 0)
			return -6;
		if ((info->csq[0] > 0) && (info->csq[0] <= 31))
			break;
		if (i > 19)
			return -7;
		DELAYMS(2000);
	}

	/* 查找是否注册网络 */
	for (i = 0; i < 20; i++) {
		if (at_creg_read(info->netreg) < 0)
			return -8;
		if ((info->netreg[0] == '1') || (info->netreg[0] == '5'))
			break;
		if (i >= 19)
			return -9;
		DELAYMS(1000);
	}

	return 0;
}



static int at_xisp_write(void)
{
	char cmdbuf[64] = {0};

	if (txatcmd("AT+XISP=0\r\n") < 0)
		return -1;
	
	if (rxatcmd(cmdbuf, "OK", 0, 64, 1000) <= 0)
		return -1;
	
	return 0;
}


static int at_cgdcont(const char *apn)
{
	char cmdbuf[64] = {0};

	sprintf(cmdbuf, "AT+CGDCONT=1,\"IP\",\"%s\"\r\n", apn);
	if (txatcmd(cmdbuf) < 0)
		return -1;
	
	if (rxatcmd(cmdbuf, "OK", 0, 64, 500) <= 0)
		return -1;
	
	return 0;
}


static int at_xiic_write(void)
{
	char cmdbuf[64] = {0};

	if (txatcmd("AT+XIIC=1\r\n") < 0)
		return -1;
	
	if (rxatcmd(cmdbuf, "OK", 0, 64, 500) <= 0)
		return -1;
	
	return 0;
}


static int at_xiic_read(char *stat, char *localip)
{
	char cmdbuf[64] = {0};

	if (txatcmd("AT+XIIC?\r\n") < 0)
		return -1;
	
	if (rxatcmd(cmdbuf, "+XIIC:", 0, 64, 500) <= 0)
		return -1;
	
	*stat = cmdbuf[10];
	
	return 0;
}


static int at_tcpsetup_write(char *ip, unsigned short port)
{
	char cmdbuf[64] = {0};
	char ovt_ms = 60;
	int len;

	sprintf(cmdbuf, "AT+TCPSETUP=0,%s,%d\r\n", ip, port);
	if (txatcmd(cmdbuf) < 0)
		return -1;
	
	while (ovt_ms--) {
		len = rxatcmd(cmdbuf, "\r\n", "\r\n", 64, 1000);
		if (len > 0) {
			if (strstr(cmdbuf, "+TCPSETUP:0,OK") != NULL) {
				tcp_link_led_on();
				return 0;
			}
			if (strstr(cmdbuf, "OK") != NULL)
				continue;
			if (strstr(cmdbuf, "+TCPCLOSE") != NULL)
				return -1;
		}	
	}	
		
	return -1;
}




/******************************************************************************
    功能说明：m590模块采用客户端模块与服务器建立TCP连接
    输入参数：无
    输出参数：tcp: 模块TCP连接信息
    返 回 值：-1： 出错；0：成功
*******************************************************************************/
int m590_tcp_link(struct m590tcp *tcp)
{
	int i;
	int ret;

	/* use self tcp ptl */
	if (at_xisp_write() < 0)
		return -1;
	
	if (at_cgdcont(tcp->apn) < 0)
		return -2;

	if (at_xiic_write() < 0)
		return -3;
	
	/* ppp link */
	for (i = 0; i < 30; i++) {
		if (at_xiic_read(tcp->gprsstat, tcp->localip) < 0)
			return -4;
		if (tcp->gprsstat[0] == '1')
			break;
		if (i >= 29)
			return -5;
		DELAYMS(2000);
	}

	ret = at_tcpsetup_write(tcp->severip, tcp->severport);
	if (ret < 0)
		return -6;
	
	
	return 0;
}


/******************************************************************************
    功能说明：m590模块读取数据
    输入参数：无
    输出参数：无
    返 回 值：无
*******************************************************************************/
int m590_read(char *rxbuf, int size, int ovt)
{
	int len = 0;
	char *index;

	if ((rxbuf == NULL) || (size <= 0))
		return -1;
		
	len = rxatcmd(rxbuf, "\r\n", "\r\n", size, ovt);
	if (len > 0) {			
		if (strstr(rxbuf, "TCPCLOSE") != NULL)
			return -1;
		
		index = strstr(rxbuf, "+TCPRECV:");
		if (index != NULL) {	 
			sscanf(index, "+TCPRECV:0,%d,", &len);
			if (len > size)
				len = size;
		
			index = strstr(index + 11, ",");
			if (index != NULL) {	
				memcpy(rxbuf , index + 1, len);
				return len;
			}
		}
	}
			 
	return 0;
}


/******************************************************************************
    功能说明：m590模块写入数据
    输入参数：无
    输出参数：无
    返 回 值：无
*******************************************************************************/
int m590_write(char *wbuf, int wlen)
{
	if ((wbuf == NULL) || (wlen <= 0))
		return -1;

	if (at_tcp_send("0", wbuf, wlen) < 0)
		return -1;
	
	return 0;
}
