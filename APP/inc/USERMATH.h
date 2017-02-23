#ifndef __USERMATH_H
#define __USERMATH_H

#include "stm32f10x.h"

#define false   0x00
#define true    0x01
#define back    0x55
#define ahead   0xaa
#define same    0x00

extern  u8   TimeCompare_6Byte(u8 *T_Adr1, u8 *T_Adr2);
extern  u8   TimeCompare_5Byte(u8 *T_Adr1, u8 *T_Adr2);
extern  u8   TimeCompare_4Byte(u8 *T_Adr1, u8 *T_Adr2);
extern  u8   TimeCompare_3Byte(u8 *T_Adr1, u8 *T_Adr2);
extern  u8   TimeCompare_2Byte(u8 *T_Adr1, u8 *T_Adr2);
extern  u8   TimeCompare_1Byte(u8 *T_Adr1, u8 *T_Adr2);
extern  void U8To1BCD(u8 indata, u8 *outdata);
extern  void U16To2BCD(u16 indata, u8 *outdata);
extern  void U32To3BCD(u32 indata, u8 *outdata);
extern  void U32To4BCD(u32 indata, u8 *outdata);
extern  u8   BCD1ToU8(u8 *indata);
extern  u16  BCD2ToU16(u8 *indata);
extern  s16  BCD2ToS16(u8 *indata);
extern  u32  BCD3ToU32(u8 *indata);
extern  u32  BCD3ToS32(u8 *indata);
extern  u32  BCD4ToU32(u8 *indata);
extern  u8   TIME_CHK(u8 *TimdAddr);
extern  u8   WeekConut(u8 *Date);
extern  u8   Compare_NByte(u8 n,u8 *Byte1,u8 *Byte2);
extern  void MovNByte(u8 n, u8 *Byte1, u8 *Byte2);
extern  void MovN2Byte(u16 n, u8 *Byte1, u8 *Byte2);
extern  void ByteN_Fill(u16 N, u8 *ByteAddr, u8 Date);
extern  void MovNByte_SaP( u8 n,u8 *Byte1,u8 *Byte2 );
extern  void ByteN_Mov(u8 N, u8 *Byte1, u8 *Byte2);
extern  u8   MON_LASTDAY_CHK(u8 *Date);
extern  void Day_Last(u8 *Today, u8 *Yesday); 
extern  void Month_Last(u8 *DateIn, u8 *DateOut);
extern  void Time_CRAdd(u8 *Time, u8 unit, u8 cyc, u8 Rate);
extern  void Time_15Min(u8 *time);
extern s32 Time_distance( u8 * T1_Sec ,u8 * T2_Sec );
extern u16 Cal_CRC16(const u8* data, u32 size);
#endif