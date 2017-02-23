/*
********************************************************************************
*  文 件 名：
*
*  文件描述：
*
*  所用芯片：
*
*  创 建 人：023
*
*  版 本 号：
*
*  修改记录：
********************************************************************************
*/

#include "BSP_CFG.h"

struct SafeWord SW;
struct EnableWordStr EW;
u8 RTC_FlagMs, Num025s_WT, RTC_FlagSec, RTC_FlagMin, RTC_ms, RTC_sec, RTC_min;
u8 Yester_Day,Yester_Month,Yester_Year, Old_Sec, Old_Min, Week;  //;前一天的日期
struct Time_Type TBCD, THEX;  //;BCD码格式和HEX格式的日期时间
u8 Enable_Save;//;为I2C.c文件分配的变量
u8 OldStatus_GprsC,OldSign_GprsC;  //;GPRS的之前的状态
u8 Power_Modem;//;
struct HardWareControl   HWareC;     //;用于硬件控制
struct USARTDataStream   U1DS, U4DS, U5DS, U2DS, U3DS;   //;Uart1和Uart2 的数据流的控制参数
struct ModemContrl  GprsC, GprsCErr;
struct ModemLinkCtrl GprsLC;
struct Buf_Type AllBuf;  //;缓冲区
u16 ReadFlash_Enable;
u16 ADCConvertedValue[2];   
struct BattyCharge  BattyC; //;电池充电控制  
u8 NUMPOWER;
