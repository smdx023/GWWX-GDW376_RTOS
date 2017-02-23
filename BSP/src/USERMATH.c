/*----------------------------------------------------------------------------*
 |  文件：  USERMATH.c
 |  说明：  用于运算的函数
 *----------------------------------------------------------------------------*
 | 修改记录：
 |
 |
 *----------------------------------------------------------------------------*/

#include "BSP_CFG.h"

const u16 CRC_TABLE[256] ={
0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 0x8108, 0x9129, 
0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 0x1231, 0x0210, 0x3273, 0x2252, 
0x52b5, 0x4294, 0x72f7, 0x62d6, 0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 
0xf3ff, 0xe3de, 0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485, 
0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d, 0x3653, 0x2672, 
0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719, 0x8738,
0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 
0x2802, 0x3823, 0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b, 
0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc, 
0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a, 0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 
0x2c22, 0x3c03, 0x0c60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 
0x8d68, 0x9d49, 0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70, 
0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78, 0x9188, 0x81a9, 
0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0x00a1, 0x30c2, 0x20e3, 
0x5004, 0x4025, 0x7046, 0x6067, 0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 
0xe37f, 0xf35e, 0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256, 
0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d, 0x34e2, 0x24c3,
0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 0xa7db, 0xb7fa, 0x8799, 0x97b8, 
0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 
0x4615, 0x5634, 0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab, 
0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3, 0xcb7d, 0xdb5c, 
0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 0x4a75, 0x5a54, 0x6a37, 0x7a16, 
0x0af1, 0x1ad0, 0x2ab3, 0x3a92, 0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 
0x9de8, 0x8dc9, 0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1, 
0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 
0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0};

/*----------------------------------------------------------------------------*
 |  时间比较：秒分时日月年 
 *----------------------------------------------------------------------------*/
u8 TimeCompare_6Byte(u8 *T_Adr1, u8 *T_Adr2)
{
    u8 ch1, ch2;
    u32 long1, long2;
    
    ch1 = *(T_Adr1 + 5);
    ch2 = *(T_Adr2 + 5);
    if (ch1 > ch2)
    {
        return back;
    }
    else if (ch1 < ch2)
    {
        return ahead;
    }
    else //;年数相同
    {
        ch1 = *(T_Adr1 + 4);
        ch2 = *(T_Adr2 + 4);
        
        if (ch1 > ch2)
        {
            return back;
        }
        else if (ch1 < ch2)
        {
            return ahead;
        }
        else   //;月份相同
        {
            long1 = *(T_Adr1 + 3) *0x1000000 + *(T_Adr1 + 2) *0x10000 + *(T_Adr1 + 1) *0x100 + *(T_Adr1 + 0);
            long2 = *(T_Adr2 + 3) *0x1000000 + *(T_Adr2 + 2) *0x10000 + *(T_Adr2 + 1) *0x100 + *(T_Adr2 + 0);
            if (long1 > long2)
            {
                return back;
            }
            else if (long1 < long2)
            {
                return ahead;
            }
            else
            {
                return same;
            }
        }
    }
}


/*----------------------------------------------------------------------------*
 |  时间比较：分时日月年
 *----------------------------------------------------------------------------*/
u8 TimeCompare_5Byte(u8 *T_Adr1, u8 *T_Adr2)
{
    u8 ch1, ch2;
    u32 long1, long2;
    ch1 = *(T_Adr1 + 4);
    ch2 = *(T_Adr2 + 4);
    if (ch1 > ch2)
    {
        return back;
    }
    else if (ch1 < ch2)
    {
        return ahead;
    }
    else  //;年数相同
    {
        long1 = *(T_Adr1 + 3) *0x1000000 + *(T_Adr1 + 2) *0x10000 + *(T_Adr1 + 1) *0x100 + *(T_Adr1 + 0);
        long2 = *(T_Adr2 + 3) *0x1000000 + *(T_Adr2 + 2) *0x10000 + *(T_Adr2 + 1) *0x100 + *(T_Adr2 + 0);
        if (long1 > long2)
        {
            return back;
        }
        else if (long1 < long2)
        {
            return ahead;
        }
        else
        {
            return same;
        }
    }
}


/*----------------------------------------------------------------------------*
 |  时间比较：时日月年
 *----------------------------------------------------------------------------*/
u8 TimeCompare_4Byte(u8 *T_Adr1, u8 *T_Adr2)
{
    u8 ch1, ch2;
    u32 long1, long2;
    ch1 = *(T_Adr1 + 3);
    ch2 = *(T_Adr2 + 3);
    if (ch1 > ch2)
    {
        return back;
    }
    else if (ch1 < ch2)
    {
        return ahead;
    }
    else  //;年数相同
    {
        long1 = *(T_Adr1 + 2) *0x10000 + *(T_Adr1 + 1) *0x100 + *(T_Adr1 + 0);
        long2 = *(T_Adr2 + 2) *0x10000 + *(T_Adr2 + 1) *0x100 + *(T_Adr2 + 0);
        if (long1 > long2)
        {
            return back;
        }
        else if (long1 < long2)
        {
            return ahead;
        }
        else
        {
            return same;
        }
    }
}


/*----------------------------------------------------------------------------*
 |  时间比较：日月年
 *----------------------------------------------------------------------------*/
u8 TimeCompare_3Byte(u8 *T_Adr1, u8 *T_Adr2)
{
    u32 long1, long2;
    long1 = *(T_Adr1 + 2) *0x10000 + *(T_Adr1 + 1) *0x100 + *(T_Adr1 + 0);
    long2 = *(T_Adr2 + 2) *0x10000 + *(T_Adr2 + 1) *0x100 + *(T_Adr2 + 0);
    if (long1 > long2)
    {
        return back;
    }
    else if (long1 < long2)
    {
        return ahead;
    }
    else
    {
        return same;
    }
}


/*----------------------------------------------------------------------------*
 |  时间比较：月年
 *----------------------------------------------------------------------------*/
u8 TimeCompare_2Byte(u8 *T_Adr1, u8 *T_Adr2)
{
    u16 long1, long2;
    long1 = *(T_Adr1 + 1) *0x100 + *(T_Adr1 + 0);
    long2 = *(T_Adr2 + 1) *0x100 + *(T_Adr2 + 0);
    if (long1 > long2)
    {
        return back;
    }
    else if (long1 < long2)
    {
        return ahead;
    }
    else
    {
        return same;
    }
}


/*****************************************************************************
名称  ：Time_distance
功能  ：对给出的两个时间，求出差值
******************************************************************************/
s32 Time_distance( u8 * T1_Sec ,u8 * T2_Sec )
{
    s32 t1,t2,temp;
    //;------把 时分秒全部转换成秒的累计
    t1=(*(T1_Sec+2)/16)*10+*(T1_Sec+2)%16;
    t1*=60;   //;小时数转换成分钟数
    temp=(*(T1_Sec+1)/16)*10+*(T1_Sec+1)%16;
    t1+=temp;
    t1*=60;   //;分钟数转换成秒数
    temp=(*T1_Sec/16)*10+*T1_Sec%16;
    t1+=temp;  //;把时分秒转换成 对 00:00:00 起的秒数
    //;------把 时分秒全部转换成秒的累计
    t2=(*(T2_Sec+2)/16)*10+*(T2_Sec+2)%16;
    t2*=60;
    temp=(*(T2_Sec+1)/16)*10+*(T2_Sec+1)%16;
    t2+=temp;
    t2*=60;
    temp=(*T2_Sec/16)*10+*T2_Sec%16;
    t2+=temp;
    temp=t1-t2;  //;求出两个时间的差值  ;相差多少秒
    return(temp);
}

/*----------------------------------------------------------------------------*
 |  时间比较：年
 *----------------------------------------------------------------------------*/
u8 TimeCompare_1Byte(u8 *T_Adr1, u8 *T_Adr2)
{
    u16 long1, long2;
    long1 =  *(T_Adr1 + 0);
    long2 =  *(T_Adr2 + 0);
    if (long1 > long2)
    {
        return back;
    }
    else if (long1 < long2)
    {
        return ahead;
    }
    else
    {
        return same;
    }
}


/*--------------------------------------------------------------------------
   将一个8位的二进制转换成1字节的BCD码
 ---------------------------------------------------------------------------*/
void U8To1BCD(u8 indata, u8 *outdata)
{
    *outdata = indata % 10+(indata / 10) *16;
}


/*--------------------------------------------------------------------------
   将一个16位的二进制转换成2字节的BCD码
 ---------------------------------------------------------------------------*/
void U16To2BCD(u16 indata, u8 *outdata)
{
    u8 temp;
    temp = indata % 100;
    *outdata = temp % 10+(temp / 10) *16;
    temp = (indata / 100) % 100;
    *(outdata + 1) = temp % 10 + (temp / 10) *16;
}


/*--------------------------------------------------------------------------
   将一个32位的二进制转换成4字节的BCD码
 ---------------------------------------------------------------------------*/
void U32To4BCD(u32 indata, u8 *outdata)
{
    u8 temp;
    temp = indata % 100;
    *outdata = temp % 10+(temp / 10) *16;
    indata = indata / 100;
    temp = indata % 100;
    *(outdata + 1) = temp % 10+(temp / 10) *16;
    indata = indata / 100;
    temp = indata % 100;
    *(outdata + 2) = temp % 10+(temp / 10) *16;
    indata = indata / 100;
    temp = indata % 100;
    *(outdata + 3) = temp % 10+(temp / 10) *16;
}


/*--------------------------------------------------------------------------
  将一个32位的二进制转换成3字节的BCD码
 ---------------------------------------------------------------------------*/
void U32To3BCD(u32 indata, u8 *outdata)
{
    u8 temp;
    temp = indata % 100;
    *outdata = temp % 10+(temp / 10) *16;
    indata = indata / 100;
    temp = indata % 100;
    *(outdata + 1) = temp % 10+(temp / 10) *16;
    indata = indata / 100;
    temp = indata % 100;
    *(outdata + 2) = temp % 10+(temp / 10) *16;
}



/*--------------------------------------------------------------------------
  将1个字节的BCD码转换成U8
 ---------------------------------------------------------------------------*/
u8 BCD1ToU8(u8 *indata)
{
    u8 ch;
    ch = (*indata) % 16 + ((*indata) / 16) *10;
    return ch;
}


/*--------------------------------------------------------------------------
  将2个字节的BCD码转换成U16
 ---------------------------------------------------------------------------*/
u16 BCD2ToU16(u8 *indata)
{
    u8 ch;
    u16 temp;
    ch = *(indata + 1) % 16+*(indata + 1) / 16 * 10;
    temp = ch;
    temp *= 100;
    ch = (*indata) % 16+(*indata) / 16 * 10;
    temp += ch;
    return temp;
}


s16 BCD2ToS16(u8 *indata)
{
	u8 ch;
	s16 temp;
	if (*(indata + 1)  & 0x80)//;为负数
	{
		ch = *(indata + 1) - 0x80;
		temp = ch % 16 + ch / 16 * 10;
		temp *= 100;
		ch = (*indata) % 16+(*indata) / 16 * 10;
		temp += ch; 
		temp = -temp;
		
	}
	else
	{
		ch = *(indata + 1) % 16+*(indata + 1) / 16 * 10;
		temp = ch;
		temp *= 100;
		ch = (*indata) % 16+(*indata) / 16 * 10;
		temp += ch;
	}
	return temp;
}

/*--------------------------------------------------------------------------
  将3个字节的BCD码转换成U32
 ---------------------------------------------------------------------------*/
u32 BCD3ToU32(u8 *indata)
{
    u8 ch;
    u32 temp;
    ch = *(indata + 2) % 16+*(indata + 2) / 16 * 10;
    temp = ch;
    temp *= 100;
    ch = *(indata + 1) % 16+*(indata + 1) / 16 * 10;
    temp += ch;
    temp *= 100;
    ch = *(indata + 0) % 16+*(indata + 0) / 16 * 10;
    temp += ch;
    return temp;
}

/*--------------------------------------------------------------------------
  将3个字节的BCD码转换成U32
  去掉符号位用于比较by zrt 20130918
 ---------------------------------------------------------------------------*/
u32 BCD3ToS32(u8 * indata)
{
    u8 ch;
    u32 temp;
    if(*(indata + 2) & 0x80)
    {
        ch = *(indata + 2) - 0x80;
	 temp = ch % 16 + ch / 16 * 10;
        temp *= 100;
        ch = *(indata + 1) % 16+*(indata + 1) / 16 * 10;
        temp += ch;
        temp *= 100;
        ch = *(indata + 0) % 16+*(indata + 0) / 16 * 10;
        temp += ch;	
    }
    else
    {
        ch = *(indata + 2) % 16+*(indata + 2) / 16 * 10;
        temp = ch;
        temp *= 100;
        ch = *(indata + 1) % 16+*(indata + 1) / 16 * 10;
        temp += ch;
        temp *= 100;
        ch = *(indata + 0) % 16+*(indata + 0) / 16 * 10;
        temp += ch;
    }
    return temp;
}

/*--------------------------------------------------------------------------
  将4个字节的BCD码转换成U32
 ---------------------------------------------------------------------------*/
u32 BCD4ToU32(u8 *indata)
{
    u8 ch;
    u32 temp;
    ch = *(indata + 3) % 16+*(indata + 3) / 16 * 10;
    temp = ch;
    temp *= 100;
    ch = *(indata + 2) % 16+*(indata + 2) / 16 * 10;
    temp += ch;
    temp *= 100;
    ch = *(indata + 1) % 16+*(indata + 1) / 16 * 10;
    temp += ch;
    temp *= 100;
    ch = *(indata + 0) % 16+*(indata + 0) / 16 * 10;
    temp += ch;
    return temp;
}



/*----------------------------------------------------------------------------*
 |   验证的时间的正确性
 *----------------------------------------------------------------------------*/
u8 TIME_CHK(u8 *TimdAddr)
{
    u8 Y, Mon, D, W, H, Min, S;

    Y = *(TimdAddr + 6);
    if (Y > 0x99)
    {
        return false; //;年不正确
    }

    Mon = *(TimdAddr + 5);
    if (Mon > 0x12)
    {
        return false; //;月不正确
    }

    D = *(TimdAddr + 4);
    if (D > 0x31)
    {
        return false; //;日不正确
    }

    W = *(TimdAddr + 3);
    if (W > 0x06)
    {
        return false; //;星期不正确
    }

    H = *(TimdAddr + 2);
    if (H > 0x24)
    {
        return false; //;时不正确
    }

    Min = *(TimdAddr + 1);
    if (Min > 0x59)
    {
        return false; //;分不正确
    }

    S = *(TimdAddr + 0);
    if (S > 0x59)
    {
        return false; //;秒不正确
    }
    return true;
}



/*----------------------------------------------------------------------------*
 | 说明：根据日期算出星期
 |        蔡勒公式:W = [C/4] - 2C + y + [y/4] + [13 * (M+1) / 5] + d - 1
 |        C是世纪数减一，y是年份后两位，M是月份，d是日数。
 |        1月和2月要按上一年的13月和14月来算，这时C和y均按上一年取值。
 | 输入：日期的BCD码的地址
 | 返回：星期的BCD码
 *----------------------------------------------------------------------------*/
u8 WeekConut(u8 *Date)
{
    u8 Y, M, d;
    s16 W;

    Y = BCD1ToU8(Date + 2);
    M = *(Date + 1);
    M &= 0x1F;
    M = BCD1ToU8(&M);
    d = BCD1ToU8(Date + 0);
    if (M == 1)
    {
        M = 13;
        if (Y > 0)
        {
            Y = Y - 1;
        }
        else if (Y == 0)
        {
            Y = 99;
        }
    }
    else if (M == 2)
    {
        M = 14;
        if (Y > 0)
        {
            Y = Y - 1;
        }
        else if (Y == 0)
        {
            Y = 99;
        }
    }
    W = Y + Y / 4+13 *(M + 1) / 5+d - 1+5-40;

    W = (W % 7+7) % 7;

    return W;
}


/*----------------------------------------------------------------------------*
 |  比较N个字节是否全部相等，全部相等返回 true ，否则返回 false
 *----------------------------------------------------------------------------*/
u8 Compare_NByte(u8 n, u8 *Byte1, u8 *Byte2)
{
    u8 i;
    for (i = 0; i < n; i++)
    {
        if (*(Byte1 + i) != *(Byte2 + i))
        {
            return false;
        }
    }
    return true;
}


/*----------------------------------------------------------------------------*
 |  将N个字节从 地址 Byte2 移动到地址 Byte1
 *----------------------------------------------------------------------------*/
void MovNByte(u8 n, u8 *Byte1, u8 *Byte2)
{
    u8 i;
    for (i = 0; i < n; i++)
    {
        //if (EW.MoveByte != 'M')
        //{
        //    return ;
        //}
        *(Byte1 + i) = *(Byte2 + i);
    }
}


/*----------------------------------------------------------------------------*
 |  将N个字节用Date填充
 *----------------------------------------------------------------------------*/
void ByteN_Fill(u16 N, u8 *ByteAddr, u8 Date)
{
    u16 i;
    
    for (i = 0; i < N; i++)
    {
        *(ByteAddr + i) = Date;
    }
}


/*----------------------------------------------------------------------------*
 |  将N个字节从 地址 Byte2 移动到地址 Byte1 不加允许字
 *----------------------------------------------------------------------------*/
void ByteN_Mov(u8 N, u8 *Byte1, u8 *Byte2)
{
    u8 i;
    
    for (i = 0; i < N; i++)
    {
        *(Byte1 + i) = *(Byte2 + i);
    }
}


/*----------------------------------------------------------------------------*
 | 将N个字节从 地址 Byte2 移动到地址 Byte1
 *----------------------------------------------------------------------------*/
void MovN2Byte(u16 n, u8 *Byte1, u8 *Byte2)
{
    u16 i;
    
    for (i = 0; i < n; i++)
    {
        //if (EW.MoveByte != 'M')
        //{
        //    return ;
        //}
        *(Byte1 + i) = *(Byte2 + i);
    }
}


/*----------------------------------------------------------------------------*
 | 将N个字节从 地址 Byte2 移动到地址 Byte1 ,需要安全保护
 *----------------------------------------------------------------------------*/
void MovNByte_SaP(u8 n, u8 *Byte1, u8 *Byte2)
{
    u8 i;
    for (i = 0; i < n; i++)
    {
        //if ((SW.Para[0] != 'P') || (SW.Para[1] != 'a'))
        //{
        //    return ;
        //}
        *(Byte1 + i) = *(Byte2 + i);
    }
}





/*----------------------------------------------------------------------------*
 | 根据输入日期计算出昨天的日期
 *----------------------------------------------------------------------------*/
void Day_Last(u8 *Today, u8 *Yesday)
{
    u8 day, mon, year;
    u8 DaysOfMonth[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};    
    
    day = BCD1ToU8(Today + 0);    
    if (day > 1)  
    {
        day -= 1;
        U8To1BCD(day, Yesday + 0);
        *(Yesday + 1) = *(Today + 1);
        *(Yesday + 2) = *(Today + 2);
    }  
    else if (day == 1)  
    {
        mon = BCD1ToU8(Today + 1);  
        if (mon == 1) 
        {
            *(Yesday + 0) = 0x31;
            *(Yesday + 1) = 0x12;
            year = BCD1ToU8(Today + 2);
            if (year == 0)
            {
                year = 99;
            }  
            else  
            {
                year--;
            }
			U8To1BCD(year, Yesday + 2); 
        }  
        else if ((mon >= 2) && (mon <= 12))  
        {
            mon = BCD1ToU8(Today + 1);  
            mon--;
            if (mon == 2) 
            {
                year = BCD1ToU8(Today + 2);
                if ((year % 4) == 0)  
                {
                    *(Yesday + 0) = 0x29;
                }  
                else  
                {
                    *(Yesday + 0) = 0x28;
                }
            }  
            else  
            {
                U8To1BCD(DaysOfMonth[mon], Yesday + 0);    
            }
            U8To1BCD(mon, Yesday + 1);
            *(Yesday + 2) = *(Today + 2);
        }
    }
}


/*----------------------------------------------------------------------------*
 |  根据输入日期找出上个月的月份
 *----------------------------------------------------------------------------*/
void Month_Last(u8 *DateIn, u8 *DateOut)
{
    u8 mon, year;
    
    mon = BCD1ToU8(DateIn + 0);
    if (mon > 1)
    {
       mon -= 1;
       U16To2BCD(mon, DateOut + 0);
       *(DateOut + 1) = *(DateIn + 1);
    }
    else
    {
        *(DateOut + 0) = 0x12;
        year = BCD1ToU8(DateIn + 1);
        if (year > 0)
        {
            year -= 1;
            U16To2BCD(year, DateOut + 1);
        }
        else
        {
            *(DateOut + 1) = 0x99;
        }
    }
}


/*----------------------------------------------------------------------------*
 |  检测输入的日期是不是当月的最后一天
 *----------------------------------------------------------------------------*/
u8 MON_LASTDAY_CHK(u8 *Date)
{
    u8 Day, Mon, Year;
    
    Day  = *(Date + 0);
    Mon  = *(Date + 1);
    Year = *(Date + 2);
    
    if ((Mon == 1) || (Mon == 3) || (Mon == 5) ||(Mon == 7) || 
        (Mon == 8) || (Mon == 10)||(Mon == 12)) //;大月
    {
        if (Day == 31) 
        {
            return true;
        }
    }
    else if ((Mon == 4)||(Mon == 6)||(Mon == 9)||(Mon == 11))
    {
        if (Day == 30) 
        {
            return true;
        }
    }
    else if (Mon == 2)
    {
        if ((Year % 4) == 0x00)
        {
            if (Day == 29)
            {
                return true;
            }
        }
        else
        {
            if (Day == 28) 
            {
                return true;
            }
        }
    }
    return false;
}


/*----------------------------------------------------------------------------*
 |  时间加一个周期 Time = Time + 周期(Cyc) * 倍率(R) 
 *----------------------------------------------------------------------------*/
void Time_CRAdd(u8 *Time, u8 unit, u8 cyc, u8 Rate)
{
    u8 Min = 0, Hour = 0, Day = 0, Month = 0, Year = 0;   
    u8 DayAdd, MonAdd;
    u8 Month_Day[13] ={0, 31, 0, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (Rate == 0)
    {
        Rate = 1; //;防错
    }
    
    cyc *= Rate;
    
    if (unit == 0)                                          //;加分钟
    {
        Min = BCD1ToU8(Time + 0) + cyc;
        U8To1BCD(Min % 60, Time + 0); 
        if ((Min / 60) > 0)                                 //;分钟溢出
        {
            unit = 1;
            cyc = 0;
            Hour = Min / 60;
        }
    }    
    if (unit == 1)                                          //;加小时
    {
        Hour = BCD1ToU8(Time + 1) + Hour + cyc;
        U8To1BCD(Hour % 24, Time + 1);  
        if ((Hour / 24) > 0)                                //;小时溢出
        {
            unit = 2;
            cyc = 0;
            Day = Hour  / 24;
        }
    }    
    
    if (unit == 2)                                          //;加天
    {
        DayAdd = Day + cyc;
        
        while(DayAdd)
        {
            Day = BCD1ToU8(Time + 2) + 1;            
            if (BCD1ToU8(Time + 3) == 2)  
            {
                if ((BCD1ToU8(Time + 4) % 4) == 0)          //;闰年
                {
                    if (Day > 29)
                    {
                       Day = 1; 
                       *(Time + 3) = 0x03; 
                    }             
                     U8To1BCD(Day, Time + 2);
                }
                else                                            //;非闰年
                {
                    if (Day > 28)
                    {
                       Day = 1;
                       *(Time + 3) = 0x03; 
                    }
                    U8To1BCD(Day, Time + 2);  
                }
            }
            else if ((BCD1ToU8(Time + 3) >= 1) && (BCD1ToU8(Time + 3) <= 12))
            {
                if (Day > Month_Day[BCD1ToU8(Time + 3)])
                {
                   Day = 1;
                   Month = BCD1ToU8(Time + 3);
                   Month += 1;
                   if (Month > 12)
                   {
                       Month = 1; 
                       Year = BCD1ToU8(Time + 4);
                       Year += 1;
                       if (Year > 99)
                       {
                           Year = 0;
                       }
                       U8To1BCD(Year, Time + 4); 
                   }
                   U8To1BCD(Month, Time + 3); 
                }
                U8To1BCD(Day, Time + 2);  
            }
            DayAdd--;
        }
    }    
    
    if (unit == 3)                                          //;加月
    {
        MonAdd = cyc;
        while(MonAdd)
        {
            Month = BCD1ToU8(Time + 3) + 1;
            if (Month > 12)
            {
                Month = 1; 
                Year = BCD1ToU8(Time + 4);
                Year += 1;
                if (Year > 99)
                {
                    Year = 0;
                }
                U8To1BCD(Year, Time + 4); 
            }
            U8To1BCD(Month, Time + 3); 
            MonAdd--;
        }
    }
}


/*----------------------------------------------------------------------------*
 |   处理时间为15分钟的倍数
 *----------------------------------------------------------------------------*/
void Time_15Min(u8 *time)
{
    s8 temp = 0;
    temp = ((*time) / 16) * 10 + (*time) % 16; //;BCD码转换成16进制
    
    if ((temp >= 0) && (temp <= 15))
    {
         *time = 0x00;
    }
    if ((temp > 15) && (temp <= 30))
    {
         *time = 0x15;
    }
    if ((temp > 30) && (temp <= 45))
    {
         *time = 0x30;
    }
    if ((temp > 45) && (temp <= 60))
    {
         *time = 0x45;
    }
}

/* 
********************************************************************************
* 函 数 名：Cal_CRC16
* 功    能：CRC校验
* 参    数：data ：数据
*           size ：长度
* 返    回：CRC校验值
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
u16 Cal_CRC16(const u8* data, u32 size)
{
    uint8_t crc_H8;
    u32 crc = 0;
    
    while(size--)
    {
        crc_H8= (uint8_t)(crc >> 8);
        crc = crc << 8;
        crc = crc ^ CRC_TABLE[crc_H8 ^ *data];
        data++;
    }
    return crc&0xffffu;
}


