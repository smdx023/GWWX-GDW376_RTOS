/******************************************************************************
    版权所有：
    版 本 号: 1.0
    文 件 名: dlt_104_common.c
    生成日期: 
    作	 者：李科
    功能说明：实现《配电自动化系统应用DL/T634.5104-2009实施细则》的通用模块
    其他说明：
    修改记录：
*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "dlt_104_cfg.h"
#include "dlt_104_lib.h"
#include "dlt_104_cfg.h"

#define CFG_DEBUG

#ifdef CFG_DEBUG
    #define Print(fmt,args...) printf(fmt, ##args)
#else
    #define Print(fmt,args...)
#endif


struct dlt_lib  dlt_lib_arg[MAX_PORT];



unsigned char dlt_104_cs(char *buf, int len)
{
	unsigned char  cs = 0;
	unsigned char i;

	for (i = 0; i < len; i++) {
		cs += buf[i];
	}

	return cs;
}


char dlt_104_frame_type(char *frame, int len)
{
	unsigned char L;

	L = frame[1];
	
	/* U frame */
	if (((frame[2] & 0x03) == 0x03) 
	    && ((frame[4] & 0x01) == 0)
                   && (L == 4)) {
		return 'U';
	}

	/* S frame */
	if (((frame[2] & 0x01) == 1) 
	    && ((frame[4] & 0x01) == 0) 
	    && (L == 4)) {
		return 'S';
	}

	/* I frame */
	if (((frame[2] & 0x01) == 0) 
	      && ((frame[4] & 0x01) == 0) 
                     && (L > 4)) {
		return 'I';
	}

	return '?';
}



void print_frame(char port, char *buff, int len, char RT)
{
	int i;
	char type;
		
	Print("\r\n%CX", RT);	
	
	type = dlt_104_frame_type(buff, len);	
	Print("[%03d]%C: ", len, type);
	
	for (i = 0; i < len; i++)
		Print("%02X ", (unsigned char)buff[i]);
	
	Print("[");		
	if (type == 'U') {
		Print("TESTFR = %d, ", (unsigned char)(buff[2] >> 6) & 0x03);
		Print("STOPDT = %d, ", (unsigned char)(buff[2] >> 4) & 0x03);
		Print("STARTDT = %d",   (unsigned char)(buff[2] >> 2) & 0x03);
	}

	else if (type == 'S') {
		Print("NR = %d", (unsigned char)(buff[5] * 0x100 + buff[4]) >> 1);
	}
	else if (type == 'I') {
		Print("NS = %d, ", (unsigned char)(buff[3] * 0x100 + buff[2]) >> 1);
		Print("NR = %d, ", (unsigned char)(buff[5] * 0x100 + buff[4]) >> 1);
	
		Print("TI = %d, ", (unsigned char)buff[6]);
		Print("VSQ = %d, ", (unsigned char)buff[7]);	
		Print("COT = %d, ", (unsigned char)buff[9] * 0x100 + buff[8]);
		Print("ASUD = %d, ", (unsigned char)buff[11] * 0x100 + buff[10]);
		Print("POINT ADDR = %d, ", (unsigned char)buff[14] * 0x10000 + buff[13] * 0x100 + buff[12]);
		
		Print("ASDU[%d]: ", len - 15);
		for (i = 0; i < len - 15; i++)
			Print("%02X ",(unsigned char)buff[15 + i]);
	} else {
		Print("error frame!!!");
	}

	Print("]\r\n");
}



int dlt_104_frame_read(unsigned char port, char *frame, char *rxbuf, int *rxlen)
{
	int i;
	int len = *rxlen;
	char L;
	char *head = 0;
	char type;
	struct dlt_lib *arg = &dlt_lib_arg[port];
	unsigned char NS = 0;

	/* 找104报文头 */
	for (i = 0; i < len; i++) {
		/* 找出缓存区的最先收到的一帧来处理 */
		if (rxbuf[i] == 0x68) {
			L = rxbuf[i + 1];			
			if ((L <= len) && (L >= 4)) {
				head = rxbuf + i;
				break;
			}
		}			
	}		
	
	/* find apdu */	
	if (head != 0) {
		/* U frame */
		type = dlt_104_frame_type(head, len);
		if (type == 'U') {
#ifdef CFG_DEBUG			
			print_frame(0, head, 6, 'R');
#endif		
			memcpy(frame, head, 6);			
			len = *rxlen - 6;
		
			memcpy(rxbuf, head + 6, len);
			*rxlen = len;			
			return 6;		    
		}

		/* S frame */
		type = dlt_104_frame_type(head, len);
		if (type == 'S') {
#ifdef CFG_DEBUG			
			print_frame(0, head, 6, 'R');
#endif			
			memcpy(frame, head + 6, 6);			
			len = *rxlen - 6;
			
			memcpy(rxbuf, head, len);
			*rxlen = len;			
			return 6;			 

 		}
			
		/* I frame */
		else if (type == 'I') {
#ifdef CFG_DEBUG			
			print_frame(0, head, L + 2, 'R');
#endif			
			/* get frame send index */
			NS = (head[3] * 0x100 + head[2]) << 1;
			if (NS >= arg->NR) {			
				arg->NR++;
				Print("right frame\r\n");
				memcpy(frame, head, L + 2);			
				len = *rxlen - L - 2;
	
				memcpy(rxbuf, head + L + 2, len);
				*rxlen = len;			
				return L + 2;	
			}
 			
			/* repetitive frame */
			else if (NS < arg->NR) {
				Print("repetitive frame\r\n");
				len = *rxlen - L - 2;
	
				memcpy(rxbuf, head + L + 2, len);
				*rxlen = len;			
				return 0;
			}  
 	    
		}						    
	}
				
				
	return 0;
}



void dlt_104_apci_pack(unsigned char port, char *buf, int len, char type)
{
	struct dlt_lib *arg = &dlt_lib_arg[port];
	unsigned short NR, NS;
	
	if (type == 'S') {
		NR = arg->NR << 1;
		buf[4] |= (unsigned char)NR;
		buf[5] |= (unsigned char)(NR >> 8);
		arg->NS++; 
	}
	
	if (type == 'I') {
		NS = arg->NS << 1;
		buf[2] |= (unsigned char)NS;
		buf[3] |= (unsigned char)(NS >> 8);	

		NR = arg->NR << 1;
		buf[4] |= (unsigned char)NR;
		buf[5] |= (unsigned char)(NR >> 8);
		arg->NS++; 
	}	
}





/* 
 * t1规定发送方发送一个I格式报文或U格式报文后，必须在t1的时间内得到接收方的认可，
 * 否则发送方认为TCP连接出现问题并应重新建立连接。
 */
int dlt_104_t1_ovt(void)
{
	return 0;
}


int dlt_104_lib_init(unsigned char port)
{
	memset((char *)&dlt_lib_arg[port], 0, sizeof(dlt_lib_arg[port]));
	dlt_lib_arg[port].asdu = 1;	
	return 0;
}
