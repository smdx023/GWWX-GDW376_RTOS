/*
 * task_freeze.c
 * likejshy@126.com
 * 2017-01-08
 */

#include <string.h>
#include <stdio.h>
#include <ucos_ii.h>
#include "BSP_CFG.h"
#include "Serial.h"
#include "GDW376_CFG.h"


//#define CFG_DEBUG
#ifdef CFG_DEBUG
#define Print(fmt,args...) printf(fmt, ##args)
#else
#define Print(fmt,args...)
#endif



/* 当前系统时间的分获取 */
static int freeze_get_min_time(unsigned char *minute)
{
	*minute = THEX.Min;
	return 0;
}


/* 获取需要冻结的测量点数量 */
static int freeze_get_pn_num(unsigned char *pn_num)
{
	*pn_num = PNMAXNUM;
	return 0;
}



/* 判断系统曲线冻结的时间是否到来 */
static int freeze_curve_time_up(unsigned char minute)
{
	static unsigned old_minute = 0xFF;

	/* 每分钟执行一次 */
	if (minute != old_minute) {
		old_minute = minute;

		/* 当前分钟为15的倍数时为曲线冻结时间到来
		 * 即在每小时的:0分, 15分，30分，45分进行
		 * 曲线冻结
		 */
		if (minute % 15 == 0)
			return 1;
	}

	return 0;
}



/* 曲线冻结测量点数据 */
static int freeze_curve_data_save(unsigned char pn)
{
	CDJ_Save(pn, &TBCD.Min); //;曲线冻结
	HDJ_Program(pn); //;小时冻结
	HDJ_Save(pn); //;小时冻结保存
	MVT_Save(pn); //;保存一次统计时间
	PEZD_Save(pn, &TBCD.Day);
	return 0;
}



static int freeze_curve_data(void)
{
	unsigned char minute;
	unsigned char num, i;
	int ret;

	/* 当前系统时间的分获取 */
	ret = freeze_get_min_time(&minute);
	if (ret < 0)
		return -1;

	/* 获取需要冻结的测量点数量 */
	ret = freeze_get_pn_num(&num);
	if (ret < 0)
		return -1;

	/* 判断系统曲线冻结的时间是否到来 */
	ret = freeze_curve_time_up(minute);
	if (ret == 1) {

		/* 对所有的测量点进行曲线冻结 */
		for (i = 0; i < num; i++) {

			/* 曲线冻结测量点数据 */
			ret = freeze_curve_data_save(i);
			if (ret < 0)
				return -1;
		}
	}

	return 0;
}


/* 当前系统时间的分，时获取 */
static int freeze_day_get_time(unsigned char *minute, unsigned char *hour)
{
	*minute = THEX.Min;
	*hour = THEX.Hour;
	return 0;
}


/* 判断日冻结时间是否到来 */
static int freeze_day_time_up(unsigned char minute, unsigned char hour)
{
	static unsigned old_minute = 0xFF;

	/* 每分钟判断一次 */
	if (minute != old_minute) {
		old_minute = minute;

		/* 每天零点时为日冻结时间到来 */
		if ((minute == 0) && (hour == 0))
			return 1;
	}

	return 0;
}


/* 日冻结测量点数据 */
static int freeze_day_data_save(unsigned char pn)
{
	u8 Date[3];

	Day_Last(&TBCD.Day, Date);//;计算出上一日的日期
	DDJ_Save(pn,  Date);	  //;日冻结
	MVTD_Res(pn);		  //;日统计清零;
	return 0;
}


/* 日冻结数据 */
static int freeze_day_data(void)
{
	unsigned char minute, hour;
	unsigned char num, i;
	int ret;

	/* 当前系统时间的分，时获取 */
	ret = freeze_day_get_time(&minute, &hour);
	if (ret < 0)
		return -1;

	/* 获取需要冻结的测量点数量 */
	ret = freeze_get_pn_num(&num);
	if (ret < 0)
		return -1;

	/* 判断日冻结时间是否到来 */
	ret = freeze_day_time_up(minute, hour);
	if (ret == 1) {

		/* 对所有的测量点进行日冻结 */
		for (i = 0; i < num; i++) {

			/* 日冻结测量点数据 */
			ret = freeze_day_data_save(i);
			if (ret < 0)
				return -1;
		}
	}

	return 0;
}


/* 当前系统时间的分，时，日获取 */
static int freeze_month_get_time(unsigned char *minute, unsigned char *hour,
			  unsigned char *day)
{
	*minute = THEX.Min;
	*hour = THEX.Hour;
	*day = THEX.Day;
	return 0;
}


/* 判断月冻结时间是否到来 */
static int freeze_month_time_up(unsigned char minute, unsigned char hour,
			 unsigned char day)
{
	static unsigned old_minute = 0xFF;

	/* 每分钟判断一次 */
	if (minute != old_minute) {
		old_minute = minute;

		/* 每月月初进行月数据冻结 */
		if (day == 1 && hour == 0 && minute == 0)
			return 1;
	}

	return 0;
}


/* 月冻结数据 */
static int freeze_month_data_save(unsigned char pn)
{
	u8 Date[2];

	Month_Last(&TBCD.Month, Date);//;计算出上一月的月份
	MDJ_Save(pn, Date); //;月冻结
	MVTM_Res(pn);  //;月统计清零
	return 0;
}


/* 月冻结数据处理 */
static int freeze_month_data(void)
{
	unsigned char minute, hour, month;
	unsigned char num, i;
	int ret;

	/* 获取当前系统时间的分，时，月 */
	ret = freeze_month_get_time(&minute, &hour, &month);
	if (ret < 0)
		return -1;

	/* 获取需要冻结的测量点数量 */
	ret = freeze_get_pn_num(&num);
	if (ret < 0)
		return -1;

	/* 当前月冻结的时间是否来到 */
	ret = freeze_month_time_up(minute, hour, month);
	if (ret == 1) {

		/* 对所有的测量点数据进行冻结 */
		for (i = 0; i < num; i++) {

			/* 冻结月数据 */
			ret = freeze_month_data_save(i);
			if (ret < 0)
				return -1;
		}
	}

	return 0;
}


/* 获取系统当前时间的BCD码，用于抄表日的判断 */
static int freeze_coller_day_get_time(unsigned char *minute, unsigned char *hour,
			       unsigned char *day)
{
	*minute = TBCD.Min;
	*hour = TBCD.Hour;
	*day = TBCD.Day;
	return 0;
}


/* 判断抄表日是否到来 */
static int freeze_coller_day_time_up(unsigned char minute, unsigned char hour,
			      unsigned char day)
{
	static unsigned old_minute = 0xFF;
	u32 ch1 = 0, ch2 = 1;
	u8 i, Day, Hour, Min;

	if (minute != old_minute) {
		old_minute = minute;

		Min  = ZDP.F33.PortP[0].RTime[0];
		Hour = ZDP.F33.PortP[0].RTime[1];

		/* 获取当前系统的日期*/
		Day = TBCD.Day;

		/* 计算出抄表日 */
		ch1 |= ZDP.F33.PortP[0].RDay[3];
		ch1 <<= 8;
		ch1 |= ZDP.F33.PortP[0].RDay[2];
		ch1 <<= 8;
		ch1 |= ZDP.F33.PortP[0].RDay[1];
		ch1 <<= 8;
		ch1 |= ZDP.F33.PortP[0].RDay[0];

		for(i = 1; i < 32; i++) {
			if(ch1 & ch2) {
				if (day == i
				    && hour == Hour
				    && minute == Min) {
					return 1;
				}
			}

			ch2 <<= 1;
		}
	}

	return 0;
}


/* 冻结测量点数据 */
static int freeze_coller_day_data_save(unsigned char pn)
{
	u8 Date[2];

	Month_Last(&TBCD.Month, Date);//;计算出上一月的月份
	MDJ_Save(pn, Date);	    //;月冻结
	MVTM_Res(pn);		    //;月统计清零
	return 0;
}



/* 冻结抄表日数据 */
static int freeze_coller_day_data(void)
{
	unsigned char minute, hour, day;
	unsigned char num, i;
	int ret;

	/* 获取系统当前时间的分，时，日 */
	ret = freeze_coller_day_get_time(&minute, &hour, &day);
	if (ret < 0)
		return -1;

	/* 获取测量点数量 */
	ret = freeze_get_pn_num(&num);
	if (ret < 0)
		return -1;

	/* 抄表日是否到来 */
	ret = freeze_coller_day_time_up(minute, hour, day);
	if (ret == 1) {

		/* 对所有的测量点数据进行冻结 */
		for (i = 0; i < num; i++) {
			ret = freeze_coller_day_data_save(i);
			if (ret < 0)
				return -1;
		}
	}

	return 0;
}


/* 冻结数据任务处理 */
void App_Task_Freeze (void *p_arg)
{
	int ret;

	while (1) {
		/* 冻结曲线数据 */
		ret = freeze_curve_data();
		if (ret < 0) {
			Print("freeze_curve_data error!\r\n");
		}

		/* 冻结日数据 */
		ret = freeze_day_data();
		if (ret < 0) {
			Print("freeze_day_data error!\r\n");
		}

		/* 冻结月数据 */
		ret = freeze_month_data();
		if (ret < 0) {
			Print("freeze_month_data error!\r\n");
		}

		/* 冻结抄表日数据 */
		ret = freeze_coller_day_data();
		if (ret < 0) {
			Print("freeze_coller_meter_day_data error!\r\n");
		}

		OSTimeDlyHMSM(0, 0, 1, 0);
	}
}


