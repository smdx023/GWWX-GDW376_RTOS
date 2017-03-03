/*
 * printf.c
 * likejshy@126.com
 * 2017-03-02
 */

#include <string.h>
#include <stdio.h>
#include <ucos_ii.h>
#include <stdarg.h>
#include "Serial.h"
#include "GDW376_CFG.h"


OS_EVENT *SemPrint;


int print_init(void)
{
	SemPrint = OSSemCreate (1);
	if (SemPrint == 0)
		return -1;
	return 0;
}


/* use when OS runing, for diff task use this fun */
int kprintf(const char *fmt, ...)
{
	va_list args;
	unsigned char length = 0, len = 0;
	char rt_log_buf[128] = {0};
	INT8U perr;

	OSSemPend (SemPrint, 0, &perr);
	if (perr != OS_ERR_NONE)
		return -1;
	
	sprintf(rt_log_buf, "[%d-%d-%d %d:%d:%d]", 
		THEX.Year, THEX.Month, THEX.Day, THEX.Hour, THEX.Min, THEX.Sec);
	
	len = strlen(rt_log_buf);
	
	va_start(args, fmt);
	length = vsnprintf(rt_log_buf + len, sizeof(rt_log_buf) - 1, fmt, args);
	if (length > 128)
		length = 128;

	uart4_send_byte(rt_log_buf, length + len);
	va_end(args);
	
	perr = OSSemPost (SemPrint);
	if (perr != OS_ERR_NONE)
		return -1;

	return 0;	
}


int bsp_printf(const char *fmt, ...)
{
	va_list args;
	unsigned long length;
	char rt_log_buf[128];
	INT8U perr;

	OSSemPend (SemPrint, 0, &perr);
	if (perr != OS_ERR_NONE)
		return -1;
		
	va_start(args, fmt);
	length = vsnprintf(rt_log_buf, sizeof(rt_log_buf) - 1, fmt, args);
	if (length > 128)
		length = 128;

	uart4_send_byte(rt_log_buf, length);
	va_end(args);
	
	perr = OSSemPost (SemPrint);
	if (perr != OS_ERR_NONE)
		return -1;

	return 0;	
}


