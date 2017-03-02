/*
 * printf.h
 * likejshy@126.com
 * 2017-03-01
 */

#include <stdarg.h>

#ifndef _PRINTF_H
#define _PRINTF_H

extern int print_init(void);
extern int kprintf(const char *fmt, ...);
extern int bsp_printf(const char *fmt, ...);
#endif
