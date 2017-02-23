/*
********************************************************************************
*  文 件 名：GDW376_ZDCB.c 
*
*  文件描述：控制终端抄表 f165
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
#include "GDW376_CFG.h"

static u8 GprsSign;

/*检测残端参数F10的表号1的参数当前电表表号*/
u8 MeterNumChk(void *save, void *now)
{
	u8 *p,*q,i;
	p = save;
	q = now;
	for(i=0; i<6;i++)
	{
		if (*p != *q)
		{
			return true;
		}
		else
		{
			p++;
			q++;
		}
	}
	return false;
}
/******************************************************************************
//;  功能：判断数据是否为全为单一指定数值;
//;  返回：true -- 全为指定值;   || false -- 非;
*******************************************************************************/
u8 f_is_sole_data (u8 b8_buf[],u8 b8_byte)
{
  u8 i;
  
  for(i=0;i<b8_byte;i++)
  {
    if( 0x00 != b8_buf[i] )
      return false;
  }
  return true;
}

/* 
********************************************************************************
* 函 数 名：Format6Current
* 功    能：电流格式转换，适应376.1
* 参    数：AddrOut---转换后；AddrIn---被转换
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void Format6Current(u8 *AddrOut, u8 *AddrIn)
{
    *(AddrOut + 0) = 0x00;
    *(AddrOut + 0) |= *(AddrIn + 0) << 4;
    *(AddrOut + 1) = *(AddrIn + 0) >> 4;
    *(AddrOut + 1) |= *(AddrIn + 1) << 4;
    *(AddrOut + 2) = *(AddrIn + 1) >> 4;
    *(AddrOut + 2) &= 0x0F;  
    
    *(AddrOut + 3) = 0x00;
    *(AddrOut + 3) |= *(AddrIn + 2) << 4;
    *(AddrOut + 4) = *(AddrIn + 2) >> 4;
    *(AddrOut + 4) |= *(AddrIn + 3) << 4;
    *(AddrOut + 5) = *(AddrIn + 3) >> 4;
    *(AddrOut + 5) &= 0x0F;  
    
    *(AddrOut + 6) = 0x00;
    *(AddrOut + 6) |= *(AddrIn + 4) << 4;
    *(AddrOut + 7) = *(AddrIn + 4) >> 4;
    *(AddrOut + 7) |= *(AddrIn + 5) << 4;
    *(AddrOut + 8) = *(AddrIn + 5) >> 4;
    *(AddrOut + 8) &= 0x0F;   
}

/* 
********************************************************************************
* 函 数 名：Format7Voltage
* 功    能：三相电压格式转换，适应376.1
* 参    数：AddrOut---转换后；AddrIn---被转换
* 返    回：无 
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void Format7Voltage(u8 *AddrOut, u8 *AddrIn)
{
    *(AddrOut + 0) = (*(AddrIn + 0) & 0x0F) << 4; //;取出个位
    *(AddrOut + 1) = (*(AddrIn + 0) &0xF0) >> 4;  //;取出十位
    *(AddrOut + 1)|= (*(AddrIn + 1) & 0x0F) << 4; //;取出百位
    
    *(AddrOut + 2) = (*(AddrIn + 2) & 0x0F) << 4;
    *(AddrOut + 3) = (*(AddrIn + 2) &0xF0) >> 4; 
    *(AddrOut + 3)|= (*(AddrIn + 3) & 0x0F) << 4;
    
    *(AddrOut + 4) = (*(AddrIn + 4) & 0x0F) << 4;
    *(AddrOut + 5) = (*(AddrIn + 4) &0xF0) >> 4; 
    *(AddrOut + 5)|= (*(AddrIn + 5) & 0x0F) << 4;
}


/* 
********************************************************************************
* 函 数 名：Format9Power
* 功    能：格式转换，有功功率不变，无功功率和电能从两个字节变为三个字节，
*            低位补零
* 参    数：AddrOut---转换后；AddrIn---被转换；Type---==1,有功功率，==2无功
* 返    回：无 
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void Format9Power(u8 *AddrOut, u8 *AddrIn, u8 Type)
{
    if(Type == 1)        //;有功功率
    {
        *(AddrOut + 0) =  *(AddrIn + 0);
        *(AddrOut + 1) =  *(AddrIn + 1);
        *(AddrOut + 2) = *(AddrIn + 2); 
        
        *(AddrOut + 3) =  *(AddrIn + 3);
        *(AddrOut + 4) =  *(AddrIn + 4);
        *(AddrOut + 5) = *(AddrIn + 5); 
        
        *(AddrOut + 6) =  *(AddrIn + 6);
        *(AddrOut + 7) =  *(AddrIn + 7);
        *(AddrOut + 8) = *(AddrIn + 8); 
        
        *(AddrOut + 9) =  *(AddrIn + 9);
        *(AddrOut + 10) =  *(AddrIn + 10);
        *(AddrOut + 11) = *(AddrIn + 11); 
    }
    else if(Type == 2)   //;无功功率
    {
        *(AddrOut + 0) =  0x00;
        *(AddrOut + 1) =  *(AddrIn + 0);
        *(AddrOut + 2) = *(AddrIn + 1);
        
        *(AddrOut + 3) =  0x00;
        *(AddrOut + 4) =  *(AddrIn + 3);
        *(AddrOut + 5) = *(AddrIn + 4);
        
        *(AddrOut + 6) =  0x00;
        *(AddrOut + 7) =  *(AddrIn + 6);
        *(AddrOut + 8) = *(AddrIn + 7);
        
        *(AddrOut + 9) =  0x00;
        *(AddrOut + 10) =  *(AddrIn + 9);
        *(AddrOut + 11) = *(AddrIn + 10);
    }
}


/* 
********************************************************************************
* 函 数 名：Format13Energy
* 功    能：电能格式 xxxxxx.xx -> xxxx.xxxx
* 参    数：AddrOut---转换后；AddrIn---被转换；
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void Format13Energy(u8 *AddrOut, u8 *AddrIn)
{         
    *(AddrOut + 0) = 0x00;
    *(AddrOut + 1) = *(AddrIn + 0);
    *(AddrOut + 2) = *(AddrIn + 1);
    *(AddrOut + 3) = *(AddrIn + 2);
}


/* 
********************************************************************************
* 函 数 名：Format14Energy
* 功    能：数据格式14：电能格式xxxxxx.xxxx
* 参    数：AddrOut---转换后；AddrIn---被转换；
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void Format14Energy(u8 *AddrOut, u8 *AddrIn)
{
    *(AddrOut + 0) = 0x00;
    *(AddrOut + 1) = *(AddrIn + 0);
    *(AddrOut + 2) = *(AddrIn + 1);
    *(AddrOut + 3) = *(AddrIn + 2);
    *(AddrOut + 4) = *(AddrIn + 3);   
}


/* 
********************************************************************************
* 函 数 名：Format14Energy5
* 功    能：同时改变5个电能
* 参    数：AddrOut---转换后；AddrIn---被转换；
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void Format14Energy5(u8 *AddrOut, u8 *AddrIn)
{
    Format14Energy(AddrOut, AddrIn);
    Format14Energy(AddrOut + 5 , AddrIn + 4 );
    Format14Energy(AddrOut + 10, AddrIn + 8 );
    Format14Energy(AddrOut + 15, AddrIn + 12);
    Format14Energy(AddrOut + 20, AddrIn + 16);
}



/* 
********************************************************************************
* 函 数 名：DL645_CHK
* 功    能：检测接收DTL645帧是否正确
* 参    数：Buf_In---接收数据缓冲；Buf_Len---接收长度。
* 返    回：有效数据开始位置。
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
u8 DL645_CHK(char *Buf_In, u8 Buf_Len)
{
    u8 i, sum = 0, len, sign = 0;
  
    while((*(Buf_In + sign) != 0x68) && (sign < Buf_Len))
    {
        sign++;
    }
    
    if((sign + 12) > Buf_Len)
    {
        return 0xFF;
    }
    
    Buf_In = Buf_In + sign;
    
    if (*(Buf_In + 0) != 0x68)
    {
        return 0xFF;
    }

    if (*(Buf_In + 7) != 0x68)
    {
        return 0xFF;
    }

    len = *(Buf_In + 9) + 10;

    if(Buf_Len != (len + 2 + sign))
    {
        return 0xFF;
    }

    for(i = 0; i < len; i++)
    {
        sum += *(Buf_In + i);
    }

    if(sum != *(Buf_In + len))
    {
        return 0xFF;
    }

    if(*(Buf_In + len + 1) != 0x16)
    {
        return 0xFF;
    }

    return sign;
}


/* 
********************************************************************************
* 函 数 名：DME_CAL
* 功    能：计算日月电量及电表操作(zrt20130815)
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*
u8 DME_CAL(u8 *NEP, u8 *OEP, u8 *EP_DM, u8 Type)
{
    u8 EP[4];
    u32 EP_Now, EP_Z, EP_Add;

    if(Type == 1) //;有功
    {
        EP_Z = BCD4ToU32(OEP + 1);    //;零点电能
        EP_Now = BCD4ToU32(NEP + 1);  //;当前电能
    }
    else         //;无功
    {
        EP_Z = BCD4ToU32(OEP);
        EP_Now = BCD4ToU32(NEP);
    }

    if((EP_Now >= EP_Z) && (EP_Z != 0))
    {
        EP_Add =  EP_Now - EP_Z;
        U32To4BCD(EP_Add, EP);
        Format13Energy(EP_DM, EP);
        return false;
    }
    else
    {
        *(OEP + 0) = *(NEP + 0);   //;当前电能作为零点电能
        *(OEP + 1) = *(NEP + 1);
        *(OEP + 2) = *(NEP + 2);
        *(OEP + 3) = *(NEP + 3);
        if(Type == 1)    //;有功
        {
            *(OEP + 4) = *(NEP + 4);
        }
        return true;
    }
}
*/
u8 DME_CAL(u8 *NEP, u8 *OEP, u8 *EP_DM, u8 Type)
{
    u8 EP[4];
    u32 EP_Now, EP_Z, EP_Add;

    if(Type == 1) //;有功
    {
        EP_Z = BCD4ToU32(OEP + 1);    //;零点电能
        EP_Now = BCD4ToU32(NEP + 1);  //;当前电能
    }
    else if(Type == 2 )         //;无功
    {
        EP_Z = BCD4ToU32(OEP);
        EP_Now = BCD4ToU32(NEP);
    }
    else                           //;电能表操作记录
    {
        EP_Z = BCD2ToU16(OEP);
        EP_Now = BCD2ToU16(NEP);
    }

//    if((EP_Now >= EP_Z) && (EP_Z != 0))  by zrt 20131009  清零时，零点电能可为0
    if(EP_Now >= EP_Z)		
    {
        if(Type != 3)
	{
            EP_Add =  EP_Now - EP_Z;
            U32To4BCD(EP_Add, EP);
            Format13Energy(EP_DM, EP);
	}
       else
       {
            EP_Add = EP_Now - EP_Z;
	     U16To2BCD((u16)EP_Add, EP_DM);
       }
	 return false;
    }
    else
    {
        if(Type != 3)
        {
            *(OEP + 0) = *(NEP + 0);   //;当前电能作为零点电能
            *(OEP + 1) = *(NEP + 1);
            *(OEP + 2) = *(NEP + 2);
            *(OEP + 3) = *(NEP + 3);
            if(Type == 1)    //;有功
            {
                *(OEP + 4) = *(NEP + 4);
            }        
            EP_Add =  0;
            U32To4BCD(EP_Add, EP);
            Format13Energy(EP_DM, EP);			
        }
	 else
	 {
	     *(OEP + 0) = *(NEP + 0); 
            *(OEP + 1) = *(NEP + 1);
            EP_Add = 0;
	     U16To2BCD((u16)EP_Add, EP_DM);			
	 }
        return true;
    }
}

/* 
********************************************************************************
* 函 数 名：ZDPN_DME
* 功    能：抄表完成后统计一次当日,当月电能量
* 参    数：Pn---测量点号。
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZDPN_DME(u8 Pn)
{
    u32 *Addr32;
    struct PointDataStr *PnD;

    Addr32 = (u32*)(*(PnDTAddr + Pn));
    PnD = (struct PointDataStr*)(*(Addr32 + _PnD));

    /*当前数据存在*/
    if((PnD->CB_Time[0] != 0) || (PnD->CB_Time[1] != 0) || (PnD->CB_Time[2] != 0) ||
        (PnD->CB_Time[3] != 0) || (PnD->CB_Time[4] != 0))
    {
        /*当日电量是当前电量减当日零点电量*/
        DME_CAL(PnD->R9010, PnD->Z9010_D, PnD->R9010_D, 1);  //;正向有功日电量
        DME_CAL(PnD->R9011, PnD->Z9011_D, PnD->R9011_D, 1);
        DME_CAL(PnD->R9012, PnD->Z9012_D, PnD->R9012_D, 1);
        DME_CAL(PnD->R9013, PnD->Z9013_D, PnD->R9013_D, 1);
        DME_CAL(PnD->R9014, PnD->Z9014_D, PnD->R9014_D, 1);

        DME_CAL(PnD->R9020, PnD->Z9020_D, PnD->R9020_D, 1);  //;反向有功日电量
        DME_CAL(PnD->R9021, PnD->Z9021_D, PnD->R9021_D, 1);
        DME_CAL(PnD->R9022, PnD->Z9022_D, PnD->R9022_D, 1);
        DME_CAL(PnD->R9023, PnD->Z9023_D, PnD->R9023_D, 1);
        DME_CAL(PnD->R9024, PnD->Z9024_D, PnD->R9024_D, 1);

        DME_CAL(PnD->R9110, PnD->Z9110_D, PnD->R9110_D, 2);  //;正向无功日电量
        DME_CAL(PnD->R9111, PnD->Z9111_D, PnD->R9111_D, 2);
        DME_CAL(PnD->R9112, PnD->Z9112_D, PnD->R9112_D, 2);
        DME_CAL(PnD->R9113, PnD->Z9113_D, PnD->R9113_D, 2);
        DME_CAL(PnD->R9114, PnD->Z9114_D, PnD->R9114_D, 2);

        DME_CAL(PnD->R9120, PnD->Z9120_D, PnD->R9120_D, 2);  //;反向无功日电量
        DME_CAL(PnD->R9121, PnD->Z9121_D, PnD->R9121_D, 2);
        DME_CAL(PnD->R9122, PnD->Z9122_D, PnD->R9122_D, 2);
        DME_CAL(PnD->R9123, PnD->Z9123_D, PnD->R9123_D, 2);
        DME_CAL(PnD->R9124, PnD->Z9124_D, PnD->R9124_D, 2);
        
        /*当月电量是当前电量减上月电量*/
        DME_CAL(PnD->R9010, PnD->Z9010_M, PnD->R9010_M, 1);  //;正向有功月电量
        DME_CAL(PnD->R9011, PnD->Z9011_M, PnD->R9011_M, 1);
        DME_CAL(PnD->R9012, PnD->Z9012_M, PnD->R9012_M, 1);
        DME_CAL(PnD->R9013, PnD->Z9013_M, PnD->R9013_M, 1);
        DME_CAL(PnD->R9014, PnD->Z9014_M, PnD->R9014_M, 1);

        DME_CAL(PnD->R9020, PnD->Z9020_M, PnD->R9020_M, 1);  //;反向有功月电量
        DME_CAL(PnD->R9021, PnD->Z9021_M, PnD->R9021_M, 1);
        DME_CAL(PnD->R9022, PnD->Z9022_M, PnD->R9022_M, 1);
        DME_CAL(PnD->R9023, PnD->Z9023_M, PnD->R9023_M, 1);
        DME_CAL(PnD->R9024, PnD->Z9024_M, PnD->R9024_M, 1);

        DME_CAL(PnD->R9110, PnD->Z9110_M, PnD->R9110_M, 2);  //;正向无功月电量
        DME_CAL(PnD->R9111, PnD->Z9111_M, PnD->R9111_M, 2);
        DME_CAL(PnD->R9112, PnD->Z9112_M, PnD->R9112_M, 2);
        DME_CAL(PnD->R9113, PnD->Z9113_M, PnD->R9113_M, 2);
        DME_CAL(PnD->R9114, PnD->Z9114_M, PnD->R9114_M, 2);

        DME_CAL(PnD->R9120, PnD->Z9120_M, PnD->R9120_M, 2);  //;反向无功月电量
        DME_CAL(PnD->R9121, PnD->Z9121_M, PnD->R9121_M, 2);
        DME_CAL(PnD->R9122, PnD->Z9122_M, PnD->R9122_M, 2);
        DME_CAL(PnD->R9123, PnD->Z9123_M, PnD->R9123_M, 2);
        DME_CAL(PnD->R9124, PnD->Z9124_M, PnD->R9124_M, 2);

        DME_CAL(PnD->DB212, PnD->DB212_M, PnD->DF213.ProgramNum, 3);
        DME_CAL(PnD->RB218, PnD->RB218_M, PnD->DF213.LidOpenNum, 3);
        DME_CAL(PnD->DB217, PnD->DB217_M, PnD->DF214.CLKChangeNum, 3);
        DME_CAL(PnD->RB220, PnD->RB220_M, PnD->DF214.PeriodChangeNum, 3);			
    }
}


/*----------------------------------------------------------------------------*
 |  计算出视在功率,用于标准485表
 *----------------------------------------------------------------------------*/
void APow_CAL(u8 index)
{
    u16 Ua, Ub, Uc;
    u32 Ia, Ib, Ic;
    u32 U, I, AP;
    u32 *Addr32;
    struct PointDataStr *PnD;

    Addr32 = (u32*)(*(PnDTAddr + index));
    PnD = (struct PointDataStr*)(*(Addr32 + _PnD));

    Ua = BCD2ToU16(PnD->RB611);
    Ub = BCD2ToU16(PnD->RB612);
    Uc = BCD2ToU16(PnD->RB613);

    Ia = BCD3ToU32(PnD->RB621);
    Ib = BCD3ToU32(PnD->RB622);
    Ic = BCD3ToU32(PnD->RB623);

    U = (u32)Ua;
    I = (u32)Ia;
    AP = U * I;
    U = (u32)Ub;
    I = (u32)Ib;
    AP += U * I;
    U = (u32)Uc;
    I = (u32)Ic;
    AP += U * I;
    AP /= 100;
    U32To3BCD(AP, PnD->RB660);
}

/* 
********************************************************************************
* 函 数 名：AsciiToU16
* 功    能：将6位ASCII值转换为扩大十倍的十进制数值，比如'2''2''0''.''0'转换为
*            220；
* 参    数：
* 返    回： 
* 说    明：用于07规约抄表
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*
u16 AsciiToU16(u8 *ascii)
{
    u8 end, point, i, j;
    u16 temp16, t16;

    end = 0;
    point = 0;
    temp16 = 0;
    t16 = 1;
    for(i = 0; i < 6; i++)
    {
        if(ascii[i] == 0)
        {
            end = i;
            break;
        }
        else if(ascii[i] == '.')
        {
            point = i;                          
        }
    }
    
    if(point != 0)
    {
        j = point; //;多乘以10，扩大十倍适应376格式
    }
    else
    {
        j = end; //;多乘以10，扩大十倍适应376格式
    }
    
    for(i = 0; i < j; i++) 
    {
        t16 *= 10;
    }
    
    for(i = 0; i < end; i++)
    {
        if(point != 0) //;小数点不为0
        {
            if(i == point)
            {
                temp16 += ascii[i+1];   
            }
        }
        temp16 += ascii[i] * t16;
        t16 /= 10;
        
    }
    return temp16; 
}
*/
//by zrt 20130820 
u16 AsciiToU16(u8 *ascii)
{
    u8 end, point, i, j;
    u16 temp16, t16;

    end = 0;
    point = 0;
    temp16 = 0;
    t16 = 1;
    for(i = 0; i < 6; i++)
    {
        if(ascii[5-i] == '.')
        {
            point = i;                          //;找到小数点位置
        }    
        else if(ascii[5-i] > '9'||ascii[5-i] < '0')
        {
            end = i;
            break;
        }
    }
    
    if(point != 0)
    {
        j = point; //;多乘以10，扩大十倍适应376格式
    }
    else
    {
        j = end; //;多乘以10，扩大十倍适应376格式
    }
    
    for(i = 0; i < j; i++) 
    {
        t16 *= 10;
    }
    
    for(i = 0; i < end; i++)
    {
        if(ascii[5-i]>='0' && ascii[5-i]<='9')
        {
            temp16 += (ascii[5-i] -0x30)* t16;
            t16 /= 10;    
	}
        if(point != 0) //;小数点不为0
        {
            if(i == point)
            {
                temp16 += (ascii[5-i-1]-0x30);   
		  break;
            }
        }
        
    }
    return temp16; 
}


/* 
********************************************************************************
* 函 数 名：ZDCB_MovDataDI
* 功    能：将抄表数据标识写入到端口缓存空间
* 参    数：
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZDCB_MovDataDI(char *Order_address, struct MeterData *PMD, struct PortCtrl *Port)
{
    u8 i, L, Len, *Addr8, j, k;
    u32 *Addr32, StrOT;
    u32 ComTab_Head, DI1DI0;
    
    if(Port->GYType == GYType2007)
    {
        if(*(Order_address + 8) != 0x91)
        {
           return;
        }
        
        L = *(Order_address + 9); //;数据长度
        
        for(i = 0; i < L; i++)
        {
            *(Order_address + 10 + i) -= 0x33; //;数据域减去 0x33
        }
        
        DI1DI0 = *((u32*)(Order_address + 10));
        
        if(DI1DI0 == Port->DI1DI0)
        {
            
            ComTab_Head = (u32)DITab_2007;
            Addr32 = (u32*)(ComTab_Head + Port->IndSend * DITab_Num + 4);       
            Len = (u8)(*Addr32 & 0x0000FFFF);        
            StrOT = (u16)(*Addr32 >> 16);        /*在MeterData结构中的偏移*/
            Addr8 = (u8*)(PMD->Start + StrOT);
            EW.MoveByte = 'M';
            MovNByte(Len, Addr8, (u8*)Order_address + 14);
            EW.MoveByte = 0;              
        }
    }
    else if(Port->GYType == GYType1997)
    {
        if(*(Order_address + 8) != 0x81)
        {
           return;
        }
        
        L = *(Order_address + 9); //;数据长度
        
        for(i = 0; i < L; i++)
        {
            *(Order_address + 10 + i) -= 0x33; //;数据域减去 0x33
        }
        
        DI1DI0 = *(Order_address + 11);
        DI1DI0 <<= 8;
        DI1DI0 += *(Order_address + 10);
        
        if(DI1DI0 == Port->DI1DI0)
        {     
            ComTab_Head = (u32)DITab_1997;
            Addr32 = (u32*)(ComTab_Head + Port->IndSend * DITab_Num + 4);       
            Len = (u8)(*Addr32 & 0x0000FFFF);        
            StrOT = (u16)(*Addr32 >> 16);        /*在MeterData结构中的偏移*/
            Addr8 = (u8*)(PMD->Start + StrOT);
            EW.MoveByte = 'M';
            if(Len == 0xF3) //;需量发生时间
            {
                j = 12;
                k = 0;
                for(i = 0; i < 5; i++)
                {
                    MovNByte(4, Addr8 + k, (u8*)Order_address + j);
                    j += 4;
                    k += 8; //;下一个需量时间位置4+1+3
                }
            }
            else if(Len == 0xF5) //;需量值
            {
                j = 12;
                k = 0;
                for(i = 0; i < 5; i++)
                {
                    MovNByte(3, Addr8 + k, (u8*)Order_address + j);
                    j += 3;
                    k += 8; //;下一个需量时间位置5+3
                }
            }
            else if(Len == 0xF1) //;断相次数
            {
                j = 12;
                k = 0;
                for(i = 0; i < 5; i++)
                {
                    MovNByte(2, Addr8 + k, (u8*)Order_address + j);
                    j += 2;
                    k += 3; //;下一个需量时间位置5+3
                }
            }
            else
            {
                MovNByte(Len, Addr8, (u8*)Order_address + 12);
            }
            EW.MoveByte = 0;          
        }
    }
}


/* 
********************************************************************************
* 函 数 名：ZDCB_MovDataPn
* 功    能：表完成后，将抄表数据从端口缓存空间写入测量点空间
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZDCB_MovDataPn(u8 pn, u8 gytype, struct MeterData *PMD)
{
    u8 FirstCB = false, i;
    u16 temp16;
    u32 *Addr32, temp32, t32;
    struct PointDataStr *PnD;
    struct PoPStr *PnP;

    Addr32 = (u32*)(*(PnDTAddr + pn));
    PnD = (struct PointDataStr*)(*(Addr32 + _PnD));
    PnP = (struct PoPStr*)(*(Addr32 + _PnP));

    if ((PnD->CB_Time[0] == 0) && (PnD->CB_Time[1] == 0) &&
        (PnD->CB_Time[2] == 0) && (PnD->CB_Time[3] == 0) &&
        (PnD->CB_Time[4] == 0))
    {
        FirstCB = true; //;首次抄表
    }

    EW.MoveByte = 'M';
    PnD->RC313[0] = 4;
    //;PnD->RC313[0] = PMD->RC313[0];
    PnD->CB_Time[0] = PMD->CB_Time[0];
    PnD->CB_Time[1] = PMD->CB_Time[1];
    PnD->CB_Time[2] = PMD->CB_Time[2];
    PnD->CB_Time[3] = PMD->CB_Time[3];
    PnD->CB_Time[4] = PMD->CB_Time[4];
    /* F25 */
    MovNByte(12, PnD->RB630, PMD->RB630);    //;有功功率        
    MovNByte(8, PnD->RB650, PMD->RB650);     //;功率因素  
    MovNByte(12, PnD->RB660, PMD->RB660);   //;视在功率
    if(gytype == GYType2007)
    {    
        //;F25
        MovNByte(9,PnD->RB621, PMD->RB621);     //;瞬时电流
        MovNByte(6, PnD->RB611, PMD->RB611);    //;瞬时电压
        MovNByte(12, PnD->RB640, PMD->RB640);    //;无功功率
        
        //;F26
        /*次数由3个字节变为两个字节
          时间由6个字节变为四个字节
          并计算出总的次数，时间，起始和结束*/
        MovNByte(2, PnD->RB311, PMD->RB311);       //;A断相次数
        temp16 = BCD2ToU16(PnD->RB311);
        MovNByte(2, PnD->RB312, PMD->RB312);       //;B断相次数
        temp16 += BCD2ToU16(PnD->RB312);
        MovNByte(2, PnD->RB313, PMD->RB313);       //;C断相次数
        temp16 += BCD2ToU16(PnD->RB313);
        U16To2BCD(temp16, PnD->RB310);              //;总断相次数   
                  
        MovNByte(3, PnD->RB321, PMD->RB321);       //;A断相时间
        temp32 = BCD3ToU32(PnD->RB321);   
        MovNByte(3, PnD->RB322, PMD->RB322);       //;B断相时间
        temp32 += BCD3ToU32(PnD->RB322);
        MovNByte(3, PnD->RB323, PMD->RB323);       //;C断相时间
        temp32 += BCD3ToU32(PnD->RB323);
        U32To3BCD(temp32, PnD->RB320);                 //;总断相时间
        
        MovNByte(4, PnD->RB331, PMD->RB331 + 1);       //;A相最近断相起始时刻
        temp32 = *((u32*)PnD->RB331);
        MovNByte(4, PnD->RB332, PMD->RB332 + 1);       //;B相最近断相起始时刻
        t32 = *(u32*)PnD->RB332;
        if(t32 > temp32)
        {
            temp32 = t32;
        }
        MovNByte(4, PnD->RB333, PMD->RB333 + 1);       //;C相最近断相起始时刻
        t32 = *(u32*)PnD->RB333;
        if(t32 > temp32)
        {
            temp32 = t32;
        }
        *(u32*)PnD->RB330 = temp32;                 //;最近断相起始时刻
        
        MovNByte(4, PnD->RB341, PMD->RB341 + 1);       //;A相最近断相结束时刻
        temp32 = *((u32*)PnD->RB341);
        MovNByte(4, PnD->RB342, PMD->RB342 + 1);       //;B相最近断相结束时刻
        t32 = *((u32*)PnD->RB342);
        if(t32 > temp32)
        {
            temp32 = t32;
        }
        MovNByte(4, PnD->RB343, PMD->RB343 + 1);       //;C相最近断相结束时刻
        t32 = *(u32*)PnD->RB343;
        if(t32 > temp32)
        {
            temp32 = t32;
        }
        *(u32*)PnD->RB340 = temp32;                 //;最近断相结束时刻
           
        //;F27
        MovNByte(4, PnD->RB216, PMD->RB216); //;电池工作时间
        MovNByte(3, PnD->RB212, PMD->RB212); //;编程总次数  
        MovNByte(3, PnD->RB214, PMD->RB214); //;电表清零总次数 
        MovNByte(3, PnD->RB213, PMD->RB213); //;需量清零总次数   
        MovNByte(3, PnD->RB215, PMD->RB215); //;事件清零总次数   
        MovNByte(3, PnD->RB217, PMD->RB217); //;校时总次数
        MovNByte(6, PnD->RB21A, PMD->RB21A); //;最近一次编程发生时间 
        MovNByte(6, PnD->RB21C, PMD->RB21C); //;最近一次电表清零发生时间  
        MovNByte(6, PnD->RB21B, PMD->RB21B); //;最近一次需量清零发生时间  
        MovNByte(6, PnD->RB21D, PMD->RB21D); //;最近一次事件清零发生时间
        MovNByte(6, PnD->RB21F, PMD->RB21F + 4); //;最近一次校时发生时间



        
        //;F28 电表运行状态字及变位标志
        MovNByte(14, PnD->RC027, PMD->RC027); //;电表运行状态字
        
        //;F31 当前A、B、C三相正/反向有功电能示值  不支持反向
        Format14Energy(PnD->R30A1, PMD->R30A1); 
        Format14Energy(PnD->R30B1, PMD->R30B1); 
        Format14Energy(PnD->R30C1, PMD->R30C1); 

        Format14Energy(PnD->R30A2, PMD->R30A2); 
        Format14Energy(PnD->R30B2, PMD->R30B2); 
        Format14Energy(PnD->R30C2, PMD->R30C2);    //by zrt 20130909		

        MovNByte(4, PnD->R30A3, PMD->R30A3);
        MovNByte(4, PnD->R30B3, PMD->R30B3);
        MovNByte(4, PnD->R30C3, PMD->R30C3);	

        MovNByte(4, PnD->R30A4, PMD->R30A4);
        MovNByte(4, PnD->R30B4, PMD->R30B4);
        MovNByte(4, PnD->R30C4, PMD->R30C4);		
    
        //;F32 上一结算日A、B、C三相正/反向有功电能示值
        Format14Energy(PnD->R31A1, PMD->R31A1); 
        Format14Energy(PnD->R31B1, PMD->R31B1); 
        Format14Energy(PnD->R31C1, PMD->R31C1); 

        Format14Energy(PnD->R31A2, PMD->R31A2); 
        Format14Energy(PnD->R31B2, PMD->R31B2); 
        Format14Energy(PnD->R31C2, PMD->R31C2);    //by zrt 20130909		

        MovNByte(4, PnD->R31A3, PMD->R31A3);
        MovNByte(4, PnD->R31B3, PMD->R31B3);
        MovNByte(4, PnD->R31C3, PMD->R31C3);	

        MovNByte(4, PnD->R31A4, PMD->R31A4);
        MovNByte(4, PnD->R31B4, PMD->R31B4);
        MovNByte(4, PnD->R31C4, PMD->R31C4);				
        
        //;F49 当前电压、电流相位角 不支持电流相位角
        MovNByte(6,  PnD->RB671, PMD->RB671);
        
         //;参数类 F25
        if(PMD->RC14A[1] == 'S')/*电源接线方式*/
        {
            PnP->F25.Type = 1;  //;三相三线
        }
        else if(PMD->RC14A[1] == 'T')
        {
            PnP->F25.Type = 2;  //;三相四线
        }
        
            
        MovNByte(6,  PnD->RC040, PMD->RC040); //;互感器倍率
        temp16 = (u16)BCD3ToU32(PMD->RC040); //;电压互感器倍率
        PnP->F25.UMul[0] = (u8)temp16;
        PnP->F25.UMul[1] = (u8)(temp16>>8);
        temp16 = (u16)BCD3ToU32(PMD->RC041); //;电流互感器倍率
        PnP->F25.IMul[0] = (u8)temp16;
        PnP->F25.IMul[1] = (u8)(temp16>>8);
        
         //;额定电压by zrt 20130821
/*         
        if((PMD->RC141[0] != PnD->RC141[0]) || (PMD->RC141[1] != PnD->RC141[1]) ||
           (PMD->RC141[2] != PnD->RC141[2]) || (PMD->RC141[3] != PnD->RC141[3]) ||
           (PMD->RC141[4] != PnD->RC141[4]) || (PMD->RC141[5] != PnD->RC141[5]))
        {
*/        
            for(i = 0; i < 6; i++)
            {
                PnD->RC141[i] = PMD->RC141[i];
            }
            temp16 = AsciiToU16((u8*)PMD->RC141);
            U16To2BCD(temp16, PnP->F25.Un);
/*			
        }
        //;额定电流
        if((PMD->RC142[0] != PnD->RC142[0]) || (PMD->RC142[1] != PnD->RC142[1]) ||
           (PMD->RC142[2] != PnD->RC142[2]) || (PMD->RC142[3] != PnD->RC142[3]) ||
           (PMD->RC142[4] != PnD->RC142[4]) || (PMD->RC142[5] != PnD->RC142[5]))
        {
*/        
            for(i = 0; i < 6; i++)
            {
                PnD->RC142[i] = PMD->RC142[i];
            }
            temp16 = AsciiToU16((u8*)PMD->RC142);
            U8To1BCD((u8)temp16, &PnP->F25.In);
			
//        }
		
        //;F165  by zrt 20130812
        MovNByte(2, PnD->DB212, PMD->RB212);        //;总编程次数
        MovNByte(5, PnD->DB21A, PMD->RB21A+1);      //;最近一次编程发生时间
        MovNByte(2, PnD->RB218, PMD->RB218);        //;开端盖总次数                  //;by zrt 20130808
        MovNByte(5, PnD->RB219, PMD->RB219+1);      //;最近一次开端盖发生时间	
        //;F166  by zrt 20130814
        MovNByte(2, PnD->DB217, PMD->RB217);        //;校时总次数
        MovNByte(5, PnD->DB21F, PMD->RB21F+5);      //;最近一次校时发生时间
        MovNByte(2, PnD->RB220, PMD->RB220);        //;时段参数修改次数
        MovNByte(5, PnD->RB221, PMD->RB221+1);	    //;时段参数修改发生时间
	      MovNByte(3, PnD->RB224, PMD->RB224);        //;by zrt 210130910
    }
    else if(gytype == GYType1997)
    {  
        //;F25
        Format6Current(PnD->RB621, PMD->RB621); //;瞬时电流
        Format7Voltage(PnD->RB611, PMD->RB611); //;瞬时电压
        Format9Power(PnD->RB640, PMD->RB640, 2); //;无功功率 
        //;F26
        /*次数由3个字节变为两个字节
          时间由6个字节变为四个字节*/
        MovNByte(2, PnD->RB310, PMD->RB310);       //;总断相次数
        MovNByte(2, PnD->RB311, PMD->RB311);       //;A断相次数
        MovNByte(2, PnD->RB312, PMD->RB312);       //;B断相次数
        MovNByte(2, PnD->RB313, PMD->RB313);       //;C断相次数
           
        MovNByte(12, PnD->RB320, PMD->RB320);      //;断相时间                
        
        MovNByte(4, PnD->RB330, PMD->RB330);       //;最近断相起始时刻
        MovNByte(4, PnD->RB331, PMD->RB331);       //;A相最近断相起始时刻
        MovNByte(4, PnD->RB332, PMD->RB332);       //;B相最近断相起始时刻
        MovNByte(4, PnD->RB333, PMD->RB333);       //;C相最近断相起始时刻
              
        MovNByte(4, PnD->RB341, PMD->RB341);       //;最近断相结束时刻
        MovNByte(4, PnD->RB341, PMD->RB341);       //;A相最近断相结束时刻
        MovNByte(4, PnD->RB342, PMD->RB342);       //;B相最近断相结束时刻
        MovNByte(4, PnD->RB343, PMD->RB343);       //;C相最近断相结束时刻
         
        //;F27
        PMD->RB216[3] = 0x00;
        MovNByte(4, PnD->RB216, PMD->RB216); //;电池工作时间
        MovNByte(3, PnD->RB212, PMD->RB212); //;编程总次数  
        MovNByte(3, PnD->RB214, PMD->RB214); //;电表清零总次数 
        MovNByte(3, PnD->RB213, PMD->RB213); //;需量清零总次数   
        MovNByte(3, PnD->RB215, PMD->RB215); //;事件清零总次数   
        MovNByte(3, PnD->RB217, PMD->RB217); //;校时总次数
        PnD->RB21A[0] = 0x00; 
        MovNByte(5, PnD->RB21A + 1, PMD->RB21A);    //;最近一次编程发生时间 
        PnD->RB21B[0] = 0x00;
        MovNByte(5, PnD->RB21B, PMD->RB21B);        //;最近一次需量清零发生时间  
        ByteN_Fill(6, PnD->RB21C, 0);               //;最近一次电表清零发生时间 
        ByteN_Fill(6, PnD->RB21D, 0);               //;最近一次事件清零发生时间
        ByteN_Fill(6, PnD->RB21F, 0);               //;最近一次校时发生时间   
        
        //;F28 电表运行状态字及变位标志
        MovNByte(1, PnD->RC027, PMD->RC027); //;电表运行状态字
        ByteN_Fill(13, PnD->RC027, 0);       //;电表运行状态字
        
         //;参数类 F25
        if(PMD->RC14A[1] == 0x33)/*电源接线方式*/
        {
            PnP->F25.Type = 1;  //;三相三线
        }
        else if(PMD->RC14A[1] == 0x34)
        {
            PnP->F25.Type = 2;  //;三相四线
        }
        
//;        MovNByte(6,  PnD->RC040, PMD->RC040); //;互感器倍率
//;        temp16 = (u16)BCD3ToU32(PMD->RC040); //;电压互感器倍率
//;        PnP->F25.UMul[0] = (u8)temp16;
//;        PnP->F25.UMul[1] = (u8)(temp16>>8);
//;        temp16 = (u16)BCD3ToU32(PMD->RC041); //;电流互感器倍率
//;        PnP->F25.IMul[0] = (u8)temp16;
//;        PnP->F25.IMul[1] = (u8)(temp16>>8);
         MovNByte(2, PnD->RC141, PMD->RC141 + 1); //;额定电压
         MovNByte(1, PnD->RC142, PMD->RC142 + 1); //;额定电压
         
//;         /* F50 */
//;         MovNByte(2, PnD->RB740, PMD->RB740); //;电表故障及预付费状态字
    }
       
    /* F27 */
    CheckTime();
    PnD->RDayTime[0] = PMD->RC011[0];
    PnD->RDayTime[1] = PMD->RC011[1];
    PnD->RDayTime[2] = PMD->RC011[2];
    PnD->RDayTime[3] = PMD->RC010[1];
    PnD->RDayTime[4] = PMD->RC010[2];
    PnD->RDayTime[4] |= (PMD->RC010[0] << 5);   //;星期
    PnD->RDayTime[5] = PMD->RC010[3];
          
    /* F33 */
    Format14Energy5(PnD->R9010, PMD->R9010);    //;当前正向有功各费率电能
    MovNByte(20, PnD->R9110, PMD->R9110);       //;当前正向无功各费率电能
    MovNByte(40, PnD->R9130, PMD->R9130);       //;当前一四象限无功各费率电能

    /* F34 */
    Format14Energy5(PnD->R9020, PMD->R9020);    //;当前反向有功各费率电能
    MovNByte(20, PnD->R9120, PMD->R9120);       //;当前反向无功各费率电能
    MovNByte(40, PnD->R9150, PMD->R9150);       //;当前二三象限无功各费率电能

    /* F35 发生时间五位变四位*/
    MovNByte(3,  PnD->RA010, PMD->RA010);       //;当前正向有功最大需量
    MovNByte(3,  PnD->RA011, PMD->RA011);
    MovNByte(3,  PnD->RA012, PMD->RA012);
    MovNByte(3,  PnD->RA013, PMD->RA013);
    MovNByte(3,  PnD->RA014, PMD->RA014);
    MovNByte(4,  PnD->RB010, PMD->RB010);       //;当前正向有功最大需量发生时间
    MovNByte(4,  PnD->RB011, PMD->RB011); 
    MovNByte(4,  PnD->RB012, PMD->RB012); 
    MovNByte(4,  PnD->RB013, PMD->RB013); 
    MovNByte(4,  PnD->RB014, PMD->RB014); 
    
    MovNByte(3,  PnD->RA110, PMD->RA110);       //;当前正向无功最大需量
    MovNByte(3,  PnD->RA111, PMD->RA111);
    MovNByte(3,  PnD->RA112, PMD->RA112);
    MovNByte(3,  PnD->RA113, PMD->RA113);
    MovNByte(3,  PnD->RA114, PMD->RA114);
    MovNByte(4,  PnD->RB110, PMD->RB110);       //;当前正向无功最大需量发生时间
    MovNByte(4,  PnD->RB111, PMD->RB111); 
    MovNByte(4,  PnD->RB112, PMD->RB112); 
    MovNByte(4,  PnD->RB113, PMD->RB113); 
    MovNByte(4,  PnD->RB114, PMD->RB114); 

    /* F36 发生时间五位变四位*/
    MovNByte(3,  PnD->RA020, PMD->RA020);       //;当前反向有功最大需量
    MovNByte(3,  PnD->RA021, PMD->RA021);
    MovNByte(3,  PnD->RA022, PMD->RA022);
    MovNByte(3,  PnD->RA023, PMD->RA023);
    MovNByte(3,  PnD->RA024, PMD->RA024);
    MovNByte(4,  PnD->RB020, PMD->RB020);       //;当前反向有功最大需量发生时间
    MovNByte(4,  PnD->RB021, PMD->RB021); 
    MovNByte(4,  PnD->RB022, PMD->RB022); 
    MovNByte(4,  PnD->RB023, PMD->RB023); 
    MovNByte(4,  PnD->RB024, PMD->RB024); 
    
    MovNByte(3,  PnD->RA120, PMD->RA120);       //;当前反向无功最大需量
    MovNByte(3,  PnD->RA121, PMD->RA121);
    MovNByte(3,  PnD->RA122, PMD->RA122);
    MovNByte(3,  PnD->RA123, PMD->RA123);
    MovNByte(3,  PnD->RA124, PMD->RA124);
    MovNByte(4,  PnD->RB120, PMD->RB120);       //;当前反向无功最大需量发生时间
    MovNByte(4,  PnD->RB121, PMD->RB121); 
    MovNByte(4,  PnD->RB122, PMD->RB122); 
    MovNByte(4,  PnD->RB123, PMD->RB123); 
    MovNByte(4,  PnD->RB124, PMD->RB124); 

    /* F37 */
    Format14Energy5(PnD->R9410, PMD->R9410);
    MovNByte(20, PnD->R9510, PMD->R9510);       //;上月正向无功各费率电能
    MovNByte(40, PnD->R9530, PMD->R9530);       //;上月一四象限无功各费率电能

    /* F38 */
    Format14Energy5(PnD->R9420, PMD->R9420);
    MovNByte(20, PnD->R9520, PMD->R9520);       //;上月反向无功各费率电能
    MovNByte(40, PnD->R9550, PMD->R9550);       //;上月二三象限无功各费率电能

   /* F39 发生时间五位变四位*/
    MovNByte(3,  PnD->RA410, PMD->RA410);       //;上一结算日正向有功最大需量
    MovNByte(3,  PnD->RA411, PMD->RA411);
    MovNByte(3,  PnD->RA412, PMD->RA412);
    MovNByte(3,  PnD->RA413, PMD->RA413);
    MovNByte(3,  PnD->RA414, PMD->RA414);
    MovNByte(4,  PnD->RB410, PMD->RB410);       //;上一结算日正向有功最大需量发生时间
    MovNByte(4,  PnD->RB411, PMD->RB411); 
    MovNByte(4,  PnD->RB412, PMD->RB412); 
    MovNByte(4,  PnD->RB413, PMD->RB413); 
    MovNByte(4,  PnD->RB414, PMD->RB414); 
    
    MovNByte(3,  PnD->RA510, PMD->RA510);       //;上一结算日正向无功最大需量
    MovNByte(3,  PnD->RA511, PMD->RA511);
    MovNByte(3,  PnD->RA512, PMD->RA512);
    MovNByte(3,  PnD->RA513, PMD->RA513);
    MovNByte(3,  PnD->RA514, PMD->RA514);
    MovNByte(4,  PnD->RB510, PMD->RB510);       //;上一结算日正向无功最大需量发生时间
    MovNByte(4,  PnD->RB511, PMD->RB511); 
    MovNByte(4,  PnD->RB512, PMD->RB512); 
    MovNByte(4,  PnD->RB513, PMD->RB513); 
    MovNByte(4,  PnD->RB514, PMD->RB514); 

    /* F40 发生时间五位变四位*/
    MovNByte(3,  PnD->RA420, PMD->RA420);       //;上一结算日反向有功最大需量
    MovNByte(3,  PnD->RA421, PMD->RA421);
    MovNByte(3,  PnD->RA422, PMD->RA422);
    MovNByte(3,  PnD->RA423, PMD->RA423);
    MovNByte(3,  PnD->RA424, PMD->RA424);
    MovNByte(4,  PnD->RB420, PMD->RB420);       //;上一结算日反向有功最大需量发生时间
    MovNByte(4,  PnD->RB421, PMD->RB421); 
    MovNByte(4,  PnD->RB422, PMD->RB422); 
    MovNByte(4,  PnD->RB423, PMD->RB423); 
    MovNByte(4,  PnD->RB424, PMD->RB424); 
    
    MovNByte(3,  PnD->RA520, PMD->RA520);       //;上一结算日反向无功最大需量
    MovNByte(3,  PnD->RA521, PMD->RA521);
    MovNByte(3,  PnD->RA522, PMD->RA522);
    MovNByte(3,  PnD->RA523, PMD->RA523);
    MovNByte(3,  PnD->RA524, PMD->RA524);
    MovNByte(4,  PnD->RB520, PMD->RB520);       //;上一结算日反向无功最大需量发生时间
    MovNByte(4,  PnD->RB521, PMD->RB521); 
    MovNByte(4,  PnD->RB522, PMD->RB522); 
    MovNByte(4,  PnD->RB523, PMD->RB523); 
    MovNByte(4,  PnD->RB524, PMD->RB524); 
    
    
    MovNByte(12, PnD->RC030, PMD->RC030); //;电表常数,表号
    MovNByte(6,  PnD->RC117, PMD->RC117); //;自动抄表日
    MovNByte(3,  PnD->RB710, PMD->RB710); //;失压总次数

    if(FirstCB == true)
    {    
        PnD->CB_Time[0] = PMD->RC011[1];
        PnD->CB_Time[1] = PMD->RC011[2];
        PnD->CB_Time[2] = PMD->RC010[1];
        PnD->CB_Time[3] = PMD->RC010[2];
        PnD->CB_Time[4] = PMD->RC010[3];    
/*		
        MovNByte(5, PnD->O9010, PnD->R9010); //;当前正向有功总电能  
        MovNByte(6, PnD->OB212, PnD->RB212); //;编程次数       
        MovNByte(6, PnD->OB21A, PnD->RB21A); //;最近第一次编程时间
        MovNByte(3, PnD->OB213, PnD->RB213); //;最大需量清零次数
        MovNByte(6, PnD->OC117, PnD->RC117); //;自动抄表日期
        MovNByte(6, PnD->OC030, PnD->RC030);  //;电表常数(有无功)
        MovNByte(6, PnD->OC040, PnD->RC040);  //;互感器倍率    
        MovNByte(6, PnD->OB310, PnD->RB310);  //;断相总次数 
        MovNByte(3, PnD->OB710, PMD->RB710); //;失压总次数 
*/
        MovNByte(5, PnD->O9010, PnD->R9010); //;当前正向有功总电能  
        MovNByte(3, PnD->OB212, PnD->RB212); //;编程次数       
        MovNByte(6, PnD->OB21A, PnD->RB21A); //;最近第一次编程时间
        MovNByte(3, PnD->OB213, PnD->RB213); //;最大需量清零次数
        MovNByte(6, PnD->OC117, PnD->RC117); //;自动抄表日期
        MovNByte(6, PnD->OC030, PnD->RC030);  //;电表常数(有无功)
        MovNByte(6, PnD->OC040, PnD->RC040);  //;互感器倍率    
        MovNByte(2, PnD->OB310, PnD->RB310);  //;断相总次数 
        MovNByte(3, PnD->OB710, PnD->RB710); //;失压总次数 

        MovNByte(2, PnD->RC027, PMD->RC027);//;电表电池欠压状态字

        MovNByte(3, PnD->RB222, PMD->RB222);   //;by zrt 20130828
        MovNByte(5, PnD->OB223, PMD->RB223+1);

        MovNByte(3, PnD->OB224, PnD->RB224);  //;by zrt 210130910
	
        ERC14_Judge(PnD->OB223,2);   //;终端来电zrt20130801		
        
        HDJ_Read();       //;读小时冻结数据
        MVT_Read();       //;读出统计的最值，累计时间
        PEZ_Read();       //;读出零点电能为统计日，月电量作准备         
    }
    else
    {    
        ERCPn_EnergyJudge(pn); //;当前测量点完成抄表,可以对其判断事件
        ERCPn_ParaJudge(pn);
    }
    ZDPN_DME(pn);   //;统计当日月电能量
    EW.MoveByte = '0';
}


/* 
********************************************************************************
* 函 数 名：ZDCB_PortInit
* 功    能：终端抄表控制字初始化
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZDCB_PortInit(struct PortCtrl *Port)
{
//;    Port->Status = 0;
//;    Port->Mark = 0;
    Port->Index = 0;
    Port->PnOver = 0;
    Port->Over = 0;
//;    Port->MeterNum[0] = 0;
//;    Port->MeterNum[1] = 0;
//;    Port->MeterNum[2] = 0;
//;    Port->MeterNum[3] = 0;
//;    Port->MeterNum[4] = 0;
//;    Port->MeterNum[5] = 0;
    Port->GYType = 0;
    Port->IndSend = 0;
    Port->IndSave = 0;
//;    Port->DI1DI0 = 0;
    Port->WaitAns = 0;
    Port->ReSNum = 0;
    Port->ErrNum = 0;
    Port->PnErrNum = 0;
}


/*----------------------------------------------------------------------------*
 |  每秒钟判断当前终端抄表日是否到来
 *----------------------------------------------------------------------------*/
/* 
********************************************************************************
* 函 数 名：ZD_CBD_CHK
* 功    能：每秒钟判断当前终端抄表日是否到来
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZD_CBD_CHK(void)
{
    u32 ch1 = 0, ch2 = 1;
    u8 i, Day, Hour, Min;
    
    Min  = ZDP.F33.PortP[0].RTime[0];  
    Hour = ZDP.F33.PortP[0].RTime[1];
    
    if((TBCD.Sec ==0x00) && (TBCD.Min==Min) && (TBCD.Hour==Hour))
    {
        Day = TBCD.Day;
        ch1 |= ZDP.F33.PortP[0].RDay[3];
        ch1 <<= 8;
        ch1 |= ZDP.F33.PortP[0].RDay[2];
        ch1 <<= 8;
        ch1 |= ZDP.F33.PortP[0].RDay[1];
        ch1 <<= 8;
        ch1 |= ZDP.F33.PortP[0].RDay[0];
        Day = (Day / 0x10) * 10 + (Day % 0x10 );
        
        for(i=1; i<32; i++)
        {
            if(ch1 & ch2)
            {
                if(Day == i)
                {
                    BDJPn.Status = 'E'; //;抄表日冻结
                    ZDCB_PortInit(&Port1);
                    ZDCB_PortInit(&Port2);
                    ZDCB.Status = 'E';  //;抄表开始
                    ZDCB.Mark = 'B';    //;抄表日抄表
                }
            }
            ch2 <<= 1;
        }
    }
}



/* 
********************************************************************************
* 函 数 名：ZD_CB_CHK
* 功    能：终端抄表间隔倒计时处理
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void ZD_CB_CHK(void)
{
    if(ZDCB.CouDown > 0)
    {
        ZDCB.CouDown--;
    }
    
    if(ZDCB.CouDown == 0) //;根据抄表抄表间隔抄表
    { 
        if(ZDCB.Status == 0)
        {
            if(ZDP.F33.PortP[0].RTimeDis != 0)
            {           
                ZDCB.CouDown = ZDP.F33.PortP[0].RTimeDis;
                ZDCB.CouDown *= 60;               
            }
            else
            {
                ZDCB.CouDown = 120; //;默认2分钟
            }

            //增加抄表时段控制
			
            ZDCB_PortInit(&Port1);
#ifdef PORT2USART             
            ZDCB_PortInit(&Port2);
#endif             
            ZDCB.Status = 'E';
        }
    }
    
    ZD_CBD_CHK(); //;判断当前终端抄表日是否到来
}

/* 
********************************************************************************
* 函 数 名：ZDCB_PointFind
* 功    能：找出端口对应的测量点序号
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
u8 ZDCB_PointFind(struct PortCtrl *Port)
{
    u8 temp, PortX, BPS;

    if(!(Port->PnOver & (0x0001<<Port->Index)))   //;当前测量点未抄表
    {
        temp = ZDP.F10.Meter[Port->Index].BPS_Port;
        PortX = temp & 0x1F;

        if((PortX == 1) || (PortX == 2))
        {
            BPS = (temp & 0xE0) >> 5;
            Port->GYType = ZDP.F10.Meter[Port->Index].Type; //;当前采用的通信规约类型
            EW.MoveByte = 'M';
            MovNByte(6, Port->MeterNum, ZDP.F10.Meter[Port->Index].Addr); //;写入当前测量点号的表号
            EW.MoveByte = 0;
            if((Port==&Port1) && (PortX==1))
            {   
                if((BPS != 0x03) && (BPS != 0x00))
                {
                    Port->BPS = 0x03;
                    UARTx_BaudRate_Set(0x03, 5);//;与基表通信只支持2400bps --20141229
                }  
                return true;
            }
#ifdef PORT2USART            
            else if((Port==&Port2) && (PortX==2))
            {        
                if(Port->BPS != BPS)
                {     
                    Port->BPS = BPS;
                    UARTx_BaudRate_Set(BPS, 4);
                }
                return true;
            }
#endif            
        }

        Port->PnOver |= (0x0001 << Port->Index);
    }
    return false;
}


/* 
********************************************************************************
* 函 数 名：ZDCB_DIFind
* 功    能：找出抄表的标识
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZDCB_DIFind(struct PortCtrl *Port)
{
    u32 ComTab_Head ;

    if(Port == &Port1)
    {
        if(Port->GYType == GYType2007)
        {
            Port->IndSave = DITab_2007_Num;
            ComTab_Head = (u32)DITab_2007;
            Port->DI1DI0 = *((u32*)(ComTab_Head + Port->IndSend * DITab_Num));
        }
        else if(Port->GYType == GYType1997)
        {
            Port->IndSave = DITab_1997_Num;
            ComTab_Head = (u32)DITab_1997;
            Port->DI1DI0 = *((u16*)(ComTab_Head + Port->IndSend * DITab_Num));
        }
    }
   
}


/* 
********************************************************************************
* 函 数 名：ZDCB_OrderSend
* 功    能：配置向电表发送数据帧
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZDCB_OrderSend(struct PortCtrl *Port)
{
    u8 i, sum = 0, j = 0;

    if(Port == &Port1)
    {
        Port->Buffer = PORT1SEDBUF;
    }
#ifdef PORT2USART
    else if(Port == &Port2)
    {
        Port->Buffer = PORT2SEDBUF;
    }
#endif

    *(Port->Buffer + j++ ) = 0xFE;
    *(Port->Buffer + j++ ) = 0xFE;
    *(Port->Buffer + j++ ) = 0xFE;
    *(Port->Buffer + j++ ) = 0xFE;
    *(Port->Buffer + j++ ) = 0x68;
//;    *(Port->Buffer + 5 ) = Port->MeterNum[0];  //;表号
//;    *(Port->Buffer + 6 ) = Port->MeterNum[1];
//;    *(Port->Buffer + 7 ) = Port->MeterNum[2];
//;    *(Port->Buffer + 8 ) = Port->MeterNum[3];
//;    *(Port->Buffer + 9 ) = Port->MeterNum[4];
//;    *(Port->Buffer + 10) = Port->MeterNum[5];
    *(Port->Buffer + j++ ) = 0xAA;  //;表号
    *(Port->Buffer + j++ ) = 0xAA;
    *(Port->Buffer + j++ ) = 0xAA;
    *(Port->Buffer + j++ ) = 0xAA;
    *(Port->Buffer + j++ ) = 0xAA;
    *(Port->Buffer + j++) = 0xAA;
    *(Port->Buffer + j++) = 0x68;
    
    if(Port->GYType == GYType2007)
    {
        *(Port->Buffer + j++) = 0x11; //;标准规约读基表
        *(Port->Buffer + j++) = 0x04;
        *(Port->Buffer + j++) = (u8)Port->DI1DI0 + 0x33;
        *(Port->Buffer + j++) = (u8)(Port->DI1DI0 >> 8) + 0x33;
        *(Port->Buffer + j++) = (u8)(Port->DI1DI0 >> 16) + 0x33;
        *(Port->Buffer + j++) = (u8)(Port->DI1DI0 >> 24) + 0x33;
    }
    else if(Port->GYType == GYType1997)
    {
        *(Port->Buffer + j++) = 0x01; //;标准规约读基表
        *(Port->Buffer + j++) = 0x02;
        *(Port->Buffer + j++) = (u8)Port->DI1DI0 + 0x33;
        *(Port->Buffer + j++) = (u8)(Port->DI1DI0 >> 8) + 0x33;
    }

    for(i = 4; i < j; i++)
    {
        sum += *(Port->Buffer + i);
    }
    *(Port->Buffer + j++) = sum;
    *(Port->Buffer + j++) = 0x16;
    Port->Len = j;
}


/* 
********************************************************************************
* 函 数 名：ZDSB_OrderSend
* 功    能：配置设表数据
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：2011-07-19 13:40:09
********************************************************************************
*/
void ZDSB_OrderSend(struct PortCtrl *Port)
{
    u8 i, j = 0, sum = 0, n;
    u32 DI;

    if(Port == &Port1)
    {
        Port->Buffer = PORT1SEDBUF;
    }
#ifdef PORT2USART
    else if(Port == &Port2)
    {
        Port->Buffer = PORT2SEDBUF;
    }
#endif
    
    DI = CtrlOrder.DI[CtrlOrder.IndSend % SBNUM];
    
    *(Port->Buffer + j++ ) = 0xFE;
    *(Port->Buffer + j++ ) = 0xFE;
    *(Port->Buffer + j++ ) = 0xFE;
    *(Port->Buffer + j++ ) = 0xFE;
    *(Port->Buffer + j++ ) = 0x68;
//;    *(Port->Buffer + 5 ) = CtrlOrder.MeterNum[0];  //;表号
//;    *(Port->Buffer + 6 ) = CtrlOrder.MeterNum[1];
//;    *(Port->Buffer + 7 ) = CtrlOrder.MeterNum[2];
//;    *(Port->Buffer + 8 ) = CtrlOrder.MeterNum[3];
//;    *(Port->Buffer + 9 ) = CtrlOrder.MeterNum[4];
//;    *(Port->Buffer + 10) = CtrlOrder.MeterNum[5];
    *(Port->Buffer + j++ ) = 0xAA;  //;表号
    *(Port->Buffer + j++ ) = 0xAA;
    *(Port->Buffer + j++ ) = 0xAA;
    *(Port->Buffer + j++ ) = 0xAA;
    *(Port->Buffer + j++ ) = 0xAA;
    *(Port->Buffer + j++) = 0xAA;
    *(Port->Buffer + j++) = 0x68; 
    if(Port->GYType == GYType2007)
    {  
        *(Port->Buffer + j++) = 0x14; //;标准规约设基表
        n = j;
        *(Port->Buffer + j++) = 12; //;标准规约设基表
        *(Port->Buffer + j++) = (u8)DI + 0x33;
        *(Port->Buffer + j++) = (u8)(DI >> 8) + 0x33;
        *(Port->Buffer + j++) = (u8)(DI >> 16) + 0x33;
        *(Port->Buffer + j++) = (u8)(DI >> 24) + 0x33;
        *(Port->Buffer + j++) = 0x33;    //;密码与权限
        *(Port->Buffer + j++) = 0x33;
        *(Port->Buffer + j++) = 0x33;
        *(Port->Buffer + j++) = 0x33;
        *(Port->Buffer + j++) = 0x33;    //;操作者代码
        *(Port->Buffer + j++) = 0x33;
        *(Port->Buffer + j++) = 0x33;
        *(Port->Buffer + j++) = 0x33;
        
        switch(DI)
        {
            case 0x04001301: //;无线状态
            {
                *(Port->Buffer + j++) = GprsSign + 0x33;    //; 信号的大小
                *(Port->Buffer + n) += 1; //;数据个数
            }
            break;
            case 0x04000101: //;日期
            {
                *(Port->Buffer + j++) = (TBCD.Month >> 5) + 0x33;
                *(Port->Buffer + j++) = TBCD.Day + 0x33;
                *(Port->Buffer + j++) = (TBCD.Month & 0x1F)  + 0x33;
                *(Port->Buffer + j++) = TBCD.Year + 0x33;
                *(Port->Buffer + n) += 4; //;数据个数
            }
            break;
            case 0x04000102: //;日期
            {
                *(Port->Buffer + j++) = TBCD.Sec + 0x33 ; //;秒
                *(Port->Buffer + j++) = TBCD.Min  + 0x33; //;分
                *(Port->Buffer + j++) = TBCD.Hour + 0x33; //;时
                *(Port->Buffer + n) += 3; //;数据个数
            }
            break;
            
            default:
            break;
        }
    }
    else if(Port->GYType == GYType1997)
    {
        *(Port->Buffer + j++) = 0x04; //;标准规约设基表
        n = j;
        *(Port->Buffer + j++) = 6; //;标准规约设基表
        *(Port->Buffer + j++) = (u8)DI + 0x33;
        *(Port->Buffer + j++) = (u8)(DI >> 8) + 0x33;
        *(Port->Buffer + j++) = 0x33;    //;密码与权限
        *(Port->Buffer + j++) = 0x33;
        *(Port->Buffer + j++) = 0x33;
        *(Port->Buffer + j++) = 0x33;
        switch((u16)DI)
        {
            case 0xCA00:
            {
                *(Port->Buffer + j++) = GprsSign + 0x33;    //; 信号的大小
                *(Port->Buffer + n) += 1;
            }
            break;
            case 0xC900: //;IP地址1段
            {
                   *(Port->Buffer + n) += 4;                
                   *(Port->Buffer + j++) = ZDP.F3.IP1[0] + 0x33;
                   *(Port->Buffer + j++) = ZDP.F3.IP1[1] + 0x33;
                   *(Port->Buffer + j++) = ZDP.F3.IP1[2] + 0x33;
                   *(Port->Buffer + j++) = ZDP.F3.IP1[3] + 0x33;
               }
               break;
            case 0xC901: //;端口地址1
               {
                   *(Port->Buffer + n) += 2;
                   *(Port->Buffer + j++) = ZDP.F3.Port1[0] + 0x33;
                   *(Port->Buffer + j++) = ZDP.F3.Port1[1] + 0x33;
//;                   *(Port->Buffer + j++) = ZDP.F3.Port1[1] + 0x33;
//;                   *(Port->Buffer + j++) = ZDP.F3.Port1[0] + 0x33;
               }
               break;
//;            case 0xF313: //;设日期 C010
//;               {
//;                   j = 13;
//;                   *(Port->Buffer + j++) = 0x0A;
//;                   *(Port->Buffer + j++) = 0x10 + 0x33;
//;                   *(Port->Buffer + j++) = 0xC0 + 0x33;
//;                   j = 20;
//;                   *(Port->Buffer + j++) = 0x00 + 0x33;
//;                   *(Port->Buffer + j++) = TBCD_bak.Day + 0x33;
//;                   *(Port->Buffer + j++) = TBCD_bak.Month & 0x0F + (TBCD_bak.Month & 0x10) * 10  + 0x33;
//;                   *(Port->Buffer + j++) = TBCD_bak.Year + 0x33;
//;
//;               }
//;               break;
//;            case 0xF314: //;设时间 C011
//;               {
//;                    j = 13;
//;                    *(Port->Buffer + j++) = 0x09;
//;                    *(Port->Buffer + j++)  = 0x11 + 0x33;
//;                    *(Port->Buffer + j++)  = 0xC0 + 0x33;
//;                    j = 20;
//;                    *(Port->Buffer + j++) = TBCD_bak.Sec + 0x33 ; //;秒
//;                    *(Port->Buffer + j++) = TBCD_bak.Min  + 0x33; //;分
//;                    *(Port->Buffer + j++) = TBCD_bak.Hour + 0x33; //;时
//;               }
//;               break;
            case 0xC902: //;IP地址2段
               {
                   *(Port->Buffer + n) += 4;
                   *(Port->Buffer + j++) = ZDP.F3.IP2[0] + 0x33;
                   *(Port->Buffer + j++) = ZDP.F3.IP2[1] + 0x33;
                   *(Port->Buffer + j++) = ZDP.F3.IP2[2] + 0x33;
                   *(Port->Buffer + j++) = ZDP.F3.IP2[3] + 0x33;
               }
               break;
            case 0xC903: //;端口地址2
               {
                   *(Port->Buffer + n) += 2;
                   
                   *(Port->Buffer + j++) = ZDP.F3.Port2[0] + 0x33;
                   *(Port->Buffer + j++) = ZDP.F3.Port2[1] + 0x33;
               }
               break;
            case 0xC904: //; APN
               {
                   *(Port->Buffer + n) += 16;

                   for (i = 0; i < 16; i++)
                   {
                       *(Port->Buffer + j++) = ZDP.F3.APN[i] + 0x33;
                   }
               }
               break;
            case 0xC905:  //;主站手机电话号码
               {
                   *(Port->Buffer + n) += 8;
                  
                   for (i = 0; i < 8; i++)
                   {
                       *(Port->Buffer + j++) = ZDP.F4.Tel_Code[7-i] + 0x33;
                   }
               }
               break;
            case 0xC906:  //;主站短信中心号码
               {
                   *(Port->Buffer + n) += 8;

                   for(i = 0; i < 8; i++)
                   {
                       *(Port->Buffer + j++) = ZDP.F4.SMS_Code[7-i] + 0x33;
                   }
               }
               break;
//;            case 0xC040: //;电压互感器倍率
//;               {
//;                   *(Port->Buffer + j - 7) = 10 ;
//;                  
//;                   *(Port->Buffer + j++) = 0 + 0x33;
//;                   *(Port->Buffer + j++) = P1P.F25.UMul[0] + 0x33; //;PT变比
//;                   *(Port->Buffer + j++) = P1P.F25.UMul[1] + 0x33;
//;                   *(Port->Buffer + j++) = 0 + 0x33;
//;               }
//;               break;
//;            case 0xC041:  //;电流互感器倍率
//;               {
//;                   *(Port->Buffer + j - 7) = 10 ;
//;                  
//;                   *(Port->Buffer + j++) = 0 + 0x33;
//;                   *(Port->Buffer + j++) = P1P.F25.IMul[0] + 0x33; //;CT变比
//;                   *(Port->Buffer + j++) = P1P.F25.IMul[1] + 0x33;
//;                   *(Port->Buffer + j++) = 0 + 0x33;
//;               }
//;               break;
//;            case 0xC141:  //;额定电压
//;               {
//;                   *(Port->Buffer + j - 7) = 9 ;
//;                  
//;                   *(Port->Buffer + j++) = 0 + 0x33;
//;                   *(Port->Buffer + j++) = P1P.F25.Un[0] + 0x33;
//;                   *(Port->Buffer + j++) = P1P.F25.Un[1] + 0x33;
//;                   break;
//;               }
            case 0xC907: //;终端行政区划码
            {
                *(Port->Buffer + n) += 2;
              
                *(Port->Buffer + j++) = ZDPKZ.F89.QuXian[0] + 0x33;
                *(Port->Buffer + j++) = ZDPKZ.F89.QuXian[1] + 0x33;
                break;
            }
            case 0xC908: //;终端地址
            {
                u8 DiZhi[2];

                *(Port->Buffer + n) += 2;
                
                U16To2BCD(*(u16*)ZDPKZ.F89.DiZhi, DiZhi);
                *(Port->Buffer + j++) = DiZhi[0] + 0x33;
                *(Port->Buffer + j++) = DiZhi[1] + 0x33;
                break;
            }
            case 0xC912:
            {
                *(Port->Buffer + n) += 1;              
                *(Port->Buffer + j++) = (ZDP.F1.HBeat / 10) * 16 + (ZDP.F1.HBeat % 10)+ 0x33;
                break;
            }
            case 0xC910:
            {
                *(Port->Buffer + n) += 1;
                *(Port->Buffer + j++) = (ZDP.F8.WorkMode & 0x03) + 0x33;
                break;
            }
            case 0xC911:
            {
                u16 t;
                *(Port->Buffer + n) += 2;
                t = ZDP.F8.ActRedailSec[0] + (ZDP.F8.ActRedailSec[1] << 8);
                t = t / 60;
                U16To2BCD(t, (u8*)Port->Buffer + j);
                *(Port->Buffer + j++) += 0x33;
                *(Port->Buffer + j++) += 0x33;
                break;
            }
            case 0x6601:
            {
                *(Port->Buffer + n) += 1;
                *(Port->Buffer + j++) = CtrlOrder.Order[CtrlOrder.IndSend % SBNUM] + 0x33;
            }
            default:
               break;
        }
    }
    
    
    for(i = 4; i < j; i++)
    {
        sum += *(Port->Buffer + i);
    }
    *(Port->Buffer + j++) = sum;
    *(Port->Buffer + j++) = 0x16;
    Port->Len = j;
}


/* 
********************************************************************************
* 函 数 名：ZDCB_Port
* 功    能：控制端口抄表
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZDCB_Port(struct PortCtrl *Port, struct MeterData *PMD)
{
    u8 Len;
    static u8 comstat = false;
    
    if (Port->Over == false)
    {
        if (Port->Status == idle) //;当前空闲状态
        {
            if (ZDCB_PointFind(Port) == true)
            {
                Port->Status = 'F'; //;找到测量点进行抄表
                if (Port == &Port1)
                {
                    PMD1.CB_Time[0] = TBCD.Min;   
                    PMD1.CB_Time[1] = TBCD.Hour;  
                    PMD1.CB_Time[2] = TBCD.Day;   
                    PMD1.CB_Time[3] = TBCD.Month; 
                    PMD1.CB_Time[4] = TBCD.Year;
                }
                else
                {
                    PMD2.CB_Time[0] = TBCD.Min;   
                    PMD2.CB_Time[1] = TBCD.Hour;  
                    PMD2.CB_Time[2] = TBCD.Day;   
                    PMD2.CB_Time[3] = TBCD.Month; 
                    PMD2.CB_Time[4] = TBCD.Year;  
                }
            }
            else
            {
                Port->Index++;
                if(Port->Index >= *((u16*)ZDP.F10.MeterNum))
                {
                    Port->Index = 0;
                    Port->Over = true; //;端口完成抄表
                }
            }
        }
    }

    if(Port->Status == 'F')
    {
        ZDCB_DIFind(Port); //;找到标识抄表
        ZDCB_OrderSend(Port);
        Port->Status = 'E'; //;端口发送开始
        Port->Mark = 'B';   //;端口在抄表
    }

    if(Port->Mark == 'B') //;抄表或抄表日抄表
    {      
        if(Port->Status == 'S')//;发送状态
        {
            Port->WaitAns++;
            if (Port->WaitAns > 12) //;3秒无回应
            {
                Port->WaitAns = 0;
                Port->ReSNum++;
                if(Port->ReSNum < 2)
                {
                    Port->Status = 'E'; //;重发
                }
                else if(Port->ReSNum < 3)
                {
//;                    if((Port->BPS == 0x06)||(Port->BPS == 0x00))
//;                    {
//;                        Port->BPS = 0x08;
//;                        UARTx_BaudRate_Set(0x08, 3);
//;                    }
//;                    else if(Port->BPS == 0x08)
//;                    {
//;                        Port->BPS = 0x06;
//;                        UARTx_BaudRate_Set(0x06, 3);
//;                    }
                    Port->Status = 'E'; //;重发
                }
                else
                {
                    Port->ReSNum = 0;
                    Port->ErrNum++;               
                    if (Port->ErrNum < 2)
                    {
             //           Port->ErrNum = 0x00;
                        Port->IndSend++;
                        Port->Status = 'F';                                         //;指向下一个标识继续抄
                        if (Port->IndSend >= Port->IndSave)
                        {
                            Port->IndSend = 0;
                            Port->PnOver |= (0x0001 << Port->Index);                //;当前测量点找表完成
                            Port->Status = idle;
                        }
                    }
                    else                                                            //;连续两个标识未抄到跳过当前测量点
                    {
                        Port->Mark = 0;
                        Port->ErrNum = 0;
                        Port->IndSend = 0;
                        Port->PnOver |= (0x0001 << Port->Index); //;当前测量点找表完成
                        Port->Status = idle;
                        Port->PnErrNum++;                       //;累计没有抄到的测量点个数                   
                        ERC21_4Judge(1);                        //;判断是否发生485抄表故障
                        if(comstat)
                        {
                            comstat = false;
                            COMLIGHTOFF();  //;通信失败指示
                        }
                    }
                }
            } 
        }
        
        if(Port->Status == 'R')//;接收到抄表回复
        {
            Port->Mark = 0;
            Port->WaitAns = 0;
            Port->ReSNum = 0;
            Port->ErrNum = 0;
            Len = DL645_CHK(Port->Buffer, Port->Len);
            if (Len != 0xFF) //;串口5收到654帧 
            {
               if (MeterNumChk(ZDP.F10.Meter[0].Addr,Port->Buffer + Len + 1))
               {
                  MovNByte(6, ZDP.F10.Meter[0].Addr, (u8*)(Port->Buffer + Len + 1));
                  EW.CSB1 = 0xC1;
                  if(!f_is_sole_data ((u8*)(Port->Buffer + Len + 1),6))/*表号为全0,则不修改区域码*/
                  {
                  	u8 buf[4];
                  	buf[0] = 0x25;
                  	buf[1] = ZDP.F10.Meter[0].Addr[3];
                  	buf[2] = ZDP.F10.Meter[0].Addr[2];
                  	buf[3] = ZDP.F10.Meter[0].Addr[1];
                  	EW.MoveByte = 'M';
                  	MovNByte(4,  ZDPKZ.F89.QuXian,(u8*)&buf[0]);
                  	EW.MoveByte = 0;
                  	EW.CSB8 = 0xC8;
                  	GprsC.ReConnect = true; //;终端区域码更改，需要重新连接
                  }	
               }            
                if(Port == &Port1)
                {
                    METERDATALIGHTON(); //;数据灯亮
                    PORT1USARTCTR.DataLightMs = 100;
                    if(comstat == false)
                    {
                        comstat = true;
                        COMLIGHTON();  //;通信正常指示
                    }
                }
                
                ZDCB_MovDataDI(Port->Buffer + Len, PMD, Port); //;将从端口接收到的数据写入到PMD区
            }
            Port->IndSend++;
            Port->Status = 'F'; //;指向下一个标识继续抄
            if (Port->IndSend >= Port->IndSave)
            {
                Port->IndSend = 0;
                Port->PnOver |= (0x0001 << Port->Index); //;当前测量点抄表完成
                ZDCB_MovDataPn(Port->Index, Port->GYType, PMD); //;完成抄表后将数据写入当前测量点区
                Port->Status = idle;
            }
            ERC21_4Judge(2);   //;判断485抄表故障是否恢复
        }
    }
}


/* 
********************************************************************************
* 函 数 名：ZD_Port1
* 功    能：处理终端端口1的数据发送与接收
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZD_Port1(void)  
{
    if(PORT1USARTCTR.RecOrd == true) //;串口5接收数据
    {
        PORT1USARTCTR.RecOrd = false; 
        if(Port1.Status == 'S') //;端口1发送数据完成状态,等待接收数据
        {
            Port1.Status = 'R'; //;端口1接收数据成功
            Port1.Buffer =  PORT1REVBUF; //;指向端口1接收数据缓存区
            Port1.Len = PORT1USARTCTR.RecLen; //;端口1接到的长度
            EW.MoveByte = 'M';
            MovNByte(Port1.Len, (u8*)Port1.Buffer, (u8*)PORT1USARTCTR.BufAdr); //;端口1接收的数据写入串口5数据区
            EW.MoveByte = 0;
        }        
    }
    
    if(PORT1USARTCTR.SedOrd == 0) //;串口5空闲
    {
        if(Port1.Status == 'E')//;端口1发送数据允许
        {
            Port1.Status = 'S'; //;端口1发送数据完成状态
            PORT1USARTCTR.SedOrd = 'E';  //;串口5发送命令允许
            PORT1USARTCTR.BufAdr = Port1.Buffer;
            PORT1USARTCTR.TLong = Port1.Len;
        }
    }
}

#ifdef PORT2USART
/* 
********************************************************************************
* 函 数 名：ZD_Port2
* 功    能：处理终端端口2的数据发送与接收
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZD_Port2(void)  
{
    if (PORT2USARTCTR.RecOrd == true) //;串口4接收数据
    {
        PORT2USARTCTR.RecOrd = false; 
        if (Port2.Status == 'S') //;端口2发送数据完成状态,等待接收数据
        {
            Port2.Status = 'R'; //;端口2接收数据成功
            Port2.Buffer =  PORT2REVBUF; //;指向端口2接收数据缓存区
            Port2.Len = PORT2USARTCTR.RecLen; //;端口2接到的长度
            EW.MoveByte = 'M';
            MovNByte(Port2.Len, (u8*)Port2.Buffer, (u8*)PORT2USARTCTR.BufAdr); //;端口1接收的数据写入串口4数据区
            EW.MoveByte = 0;
        }  
    }
    if ((Port2.Status == 'E') && (PORT2USARTCTR.SedOrd == 0))//;端口2发送数据允许且串口4空闲
    {
        Port2.Status = 'S'; //;端口2发送数据完成状态
        PORT2USARTCTR.SedOrd = 'E';  //;串口4发送命令允许
        PORT2USARTCTR.BufAdr = Port2.Buffer;
        PORT2USARTCTR.TLong = Port2.Len;
    }
}
#endif


/* 
********************************************************************************
* 函 数 名：ZD_Port_Com
* 功    能：终端端口通讯处理
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZD_Port_Com(void)  
{
    ZD_Port1();
#ifdef PORT2USART
    ZD_Port2();
#endif
}


/* 
********************************************************************************
* 函 数 名：ZD_CB
* 功    能：终端抄表控制
* 参    数：无
* 返    回：无
* 说    明：每250ms执行一次
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZD_CB(void)
{
    if (ZDCB.Status == 'E') //;终端抄表开始
    {
        ZDCB_Port(&Port1, &PMD1);
#ifdef PORT2USART
        ZDCB_Port(&Port2, &PMD2);
        if((Port1.Over == true) && (Port2.Over == true))
        {
            ZDCB.Status = 'O'; //;抄表完成
        } 
#else
        if((Port1.Over == true))
        {
            ZDCB.Status = 'O'; //;抄表完成
        } 
#endif
        
    }
    
    if(ZDCB.Status == 'O')//;抄表完成后根据不同的抄表类型进行处理
    {
        ZDCB.Status = 0;     
        if(ZDCB.Mark == 'B') //;抄表日抄表
        {           
            if(BDJPn.Status == 0)
            {
                BDJPn.Pn = 0; //;从第一个测量点开始冻结
                BDJPn.Status = 'E'; //;冻结开始
            }
        }
        ZDCB.Mark = 0;
    }
}


/* 
********************************************************************************
* 函 数 名：Send_Modem_Status
* 功    能：根据模块信号状态决定是否向电表发送更新命令
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void Send_Modem_Status(void)
{
    static u8 OldStatus = 0, OldSign = 0, OldGprsSign = 0;
    
    if((OldStatus != GprsC.State) || (OldSign != GprsC.Sign))
    {
        OldStatus = GprsC.State;
        OldSign = GprsC.Sign;
        if(GprsC.Sign >= 22)
        {
            GprsSign = 0x04;
        }
        else if(GprsC.Sign >= 14)
        {
            GprsSign = 0x03;
        }
        else if(GprsC.Sign >=7)
        {
            GprsSign = 0x02;
        }
        else if(GprsC.Sign >= 1)
        {
            GprsSign = 0x01;
        }
        else
        {
            GprsSign = 0x00;
        }
        if(GprsC.State & _HadLogin)
        {
            GprsSign |= 0x80;
        }
        else
        {
            GprsSign &= 0x7F;
        }
        if(GprsSign != OldGprsSign)
        {
            OldGprsSign = GprsSign;
            CtrlOrder.Pn[CtrlOrder.IndSave % SBNUM] = 1;
            if(Port1.GYType == GYType2007)
            {
                CtrlOrder.DI[CtrlOrder.IndSave % SBNUM] = 0x04001301;               
            }
            else if(Port1.GYType == GYType1997)
            {
                CtrlOrder.DI[CtrlOrder.IndSave % SBNUM] = 0xCA00;
            }
            CtrlOrder.IndSave++; 
        }
    }
}

/*设基表时间*/
void Send_TimetoMeter(void)
{
    CtrlOrder.Pn[CtrlOrder.IndSave % SBNUM] = 1;//;日期
    CtrlOrder.DI[CtrlOrder.IndSave % SBNUM] = 0x04000101;
    CtrlOrder.IndSave++; 
    CtrlOrder.Pn[CtrlOrder.IndSave % SBNUM] = 1;//;时间
    CtrlOrder.DI[CtrlOrder.IndSave % SBNUM] = 0x04000102;
    CtrlOrder.IndSave++; 
}

/*设IP与端口 */
void Send_ZDPF3toMeter(void)
{
    CtrlOrder.Pn[CtrlOrder.IndSave % SBNUM] = 1;
    CtrlOrder.DI[CtrlOrder.IndSave % SBNUM] = 0xC900;
    CtrlOrder.IndSave++;
    CtrlOrder.Pn[CtrlOrder.IndSave % SBNUM] = 1;
    CtrlOrder.DI[CtrlOrder.IndSave % SBNUM] = 0xC901;
    CtrlOrder.IndSave++;
    CtrlOrder.Pn[CtrlOrder.IndSave % SBNUM] = 1;
    CtrlOrder.DI[CtrlOrder.IndSave % SBNUM] = 0xC902;
    CtrlOrder.IndSave++;
    CtrlOrder.Pn[CtrlOrder.IndSave % SBNUM] = 1;
    CtrlOrder.DI[CtrlOrder.IndSave % SBNUM] = 0xC903;
    CtrlOrder.IndSave++;
    CtrlOrder.Pn[CtrlOrder.IndSave % SBNUM] = 1;//;APN
    CtrlOrder.DI[CtrlOrder.IndSave % SBNUM] = 0xC904;
    CtrlOrder.IndSave++;
}

/*设心跳周期 */
void Send_ZDPF1toMeter(void)
{
    CtrlOrder.Pn[CtrlOrder.IndSave % SBNUM] = 1;
    CtrlOrder.DI[CtrlOrder.IndSave % SBNUM] = 0xC912;
    CtrlOrder.IndSave++;
}

/*设虚拟专网工作方式 */
void Send_ZDPF8toMeter(void)
{
    CtrlOrder.Pn[CtrlOrder.IndSave % SBNUM] = 1;
    CtrlOrder.DI[CtrlOrder.IndSave % SBNUM] = 0xC910;
    CtrlOrder.IndSave++;
    CtrlOrder.Pn[CtrlOrder.IndSave % SBNUM] = 1;
    CtrlOrder.DI[CtrlOrder.IndSave % SBNUM] = 0xC911;
    CtrlOrder.IndSave++;
}

void Send_ZDPF89toMeter(void)
{
    CtrlOrder.Pn[CtrlOrder.IndSave % SBNUM] = 1;//;行政区划码
    CtrlOrder.DI[CtrlOrder.IndSave % SBNUM] = 0xC907;
    CtrlOrder.IndSave++;
    CtrlOrder.Pn[CtrlOrder.IndSave % SBNUM] = 1;//;终端地址
    CtrlOrder.DI[CtrlOrder.IndSave % SBNUM] = 0xC908;
    CtrlOrder.IndSave++;
}


/* 
********************************************************************************
* 函 数 名：ZD_SB
* 功    能：设表控制程序
* 参    数：
* 返    回： 
* 说    明：每250ms执行一次
* 抛出异常：
* 作    者：
* 修改记录：2012-01-31 14:05:00 增加抄表状态下有设表命令时保存抄表状态字并先进
*                                行设表的功能。
*           2012-10-09 09:40:42  增加防错处理。@1
*           2012-10-12 16:07:00  修改错误。当变量溢出时会出错。@2
********************************************************************************
*/
void ZD_SB(void)
{
    u8 Pn, Len, i;
    u32 DI;
    static u8 Sts, Mark;
    struct ZDPStr_F10_1 *PnF10;
    
    if(CtrlOrder.IndSave > CtrlOrder.IndSend)
    {
        if(CtrlOrder.Status == idle)
        {
            Pn = CtrlOrder.Pn[CtrlOrder.IndSend % SBNUM];
            Pn -= 0x01;   
            PnF10 = ZDP.F10.Meter + Pn;
            CtrlOrder.Port = PnF10->BPS_Port & 0x1F;
            CtrlOrder.BPS  = (PnF10->BPS_Port & 0xE0) >> 5;
            MovNByte(6, CtrlOrder.MeterNum, PnF10->Addr); //;写入当前测量点号的表号
            CtrlOrder.Status = 'E';          
        }
        
        if(CtrlOrder.Status == 'E')
        {
            if(CtrlOrder.Port == 0x01) //;端口1
            {
                if((Port1.Status == idle) || (Port1.Status == 'F'))
                {
                    Sts = Port1.Status; //;保存抄表状态，优先设表
                    Mark = Port1.Mark;
                    
                    CtrlOrder.Status = 'S';
                    Port1.Status = 'E'; //;允许发送，且为设表
                    Port1.Mark = 'C';   //;端口在设表
                    if(Port1.BPS != CtrlOrder.BPS) //;通信速率不符 --20141229
                    {
                        Port1.BPS = 0x03;
                        UARTx_BaudRate_Set(0x03, 5);//;默认2400bps                     
                    }
                   
                    DI = CtrlOrder.DI[CtrlOrder.IndSend % SBNUM];
                    if(DI == 0x0400050E) //;读事件状态
                    {
                        Port1.DI1DI0 = DI;
                        ZDCB_OrderSend(&Port1);
                    }
                    else
                    {
                        ZDSB_OrderSend(&Port1);     
                    }                        
                }
            }
#ifdef PORT2USART             
            else if(CtrlOrder.Port == 0x02) //;端口2
            {
                if(Port2.Status == idle)
                {
                    CtrlOrder.Status = 'S';
                    Port2.Status = 'E'; //;允许发送，且为设表
                    Port2.Mark = 'C';   //;端口在设表
                    if(Port2.BPS != CtrlOrder.BPS) //;通信速率不符
                    {
                        Port2.BPS = CtrlOrder.BPS;
                        UARTx_BaudRate_Set(CtrlOrder.BPS, 4);                       
                    }
                    ZDSB_OrderSend(&Port2);                             
                }
            }
#endif            
            else 
            {                      
                CtrlOrder.Status = idle;  
                Port1.Status = Sts; //;恢复抄表状态 //;@1
                Port1.Mark = Mark;
                Port1.WaitAns = 0;
                Port1.ReSNum = 0;
                Port1.ErrNum = 0;
                CtrlOrder.ErrNum = 0x00;
                CtrlOrder.IndSend++;
            }
        }
        
        if(CtrlOrder.Status == 'S')
        {
            if((Port1.Mark != 'C') && (Port2.Mark != 'C')) //;防错处理
            {
                CtrlOrder.Status = idle;
                return;
            }
            
            if(Port1.Mark == 'C')
            {
                if(Port1.Status == 'S')//;发送状态
                {
                    Port1.WaitAns++;
                    if (Port1.WaitAns > 12) //;3秒无回应
                    {
                        Port1.WaitAns = 0;
                        Port1.ReSNum++;
                        if (Port1.ReSNum < 2)
                        {
                            Port1.Status = 'E'; //;重发
                           
                        }
                        else
                        {
                            ERC21_4Judge(1);   //;判断是否发生485抄表故障
                            Port1.ReSNum = 0;
                            Port1.Status = Sts; //;恢复抄表状态
                            Port1.Mark = Mark;
                            CtrlOrder.Status = idle;  
                            if(CtrlOrder.DI[CtrlOrder.IndSend % SBNUM] == 0x0400050E)
                            {
                                HWareC.EventFlag = false;
                                HWareC.EventRead = false;
                            }
                            CtrlOrder.IndSend++;     
                        }
                    } 
                }    
                else if(Port1.Status == 'R')//;接收状态
                {
                    CtrlOrder.Status = idle;
                    Port1.Status = Sts; //;恢复抄表状态
                    Port1.Mark = Mark;
                    Port1.WaitAns = 0;
                    Port1.ReSNum = 0;
                    Port1.ErrNum = 0;
                    Len = DL645_CHK(Port1.Buffer, Port1.Len);
                    if (Len != 0xFF) //;串口5收到654帧 
                    {
                        if((*(Port1.Buffer + 8 + Len) == 0x84)||(*(Port1.Buffer + 8 + Len) == 0x94))
                        {
                            CtrlOrder.IndSend++;
                            CtrlOrder.ErrNum = 0x00;
                        }
                        else if((*(Port1.Buffer + 8 + Len) == 0x91)) //;读事件状态返回
                        {                            
                            for(i = 0; i < 4; i++)
                            {
                                *(Port1.Buffer + 10 + Len + i) -= 0x33; //;数据域减去 0x33
                            }
                            DI = *((u32*)(Port1.Buffer + 10 + Len));
                            if(DI == 0x0400050E)
                            {
                                
                                HWareC.EventFlag = false;
                                HWareC.EventRead = false;
                            }
                            CtrlOrder.IndSend++;
                            CtrlOrder.ErrNum = 0x00;
                        }
                        else
                        {
                            CtrlOrder.ErrNum++;
                            if(CtrlOrder.ErrNum > 3) //;允许重发三次
                            {
                                if(CtrlOrder.DI[CtrlOrder.IndSend % SBNUM] == 0x0400050E)
                                {
                                    HWareC.EventFlag = false;
                                    HWareC.EventRead = false;
                                }
                                CtrlOrder.ErrNum = 0x00;
                                CtrlOrder.IndSend++;
                            }
                        }
                    }
                    else
                    {
                        CtrlOrder.ErrNum++;
                        if(CtrlOrder.ErrNum > 3) //;允许重发三次
                        {
                            if(CtrlOrder.DI[CtrlOrder.IndSend % SBNUM] == 0x0400050E)
                            {
                                HWareC.EventFlag = false;
                                HWareC.EventRead = false;
                            }
                            CtrlOrder.ErrNum = 0x00;
                            CtrlOrder.IndSend++;
                        }
                    }              
                    ERC21_4Judge(2);   //;判断485抄表故障是否恢复
                }              
            }     
#ifdef PORT2USART           
            if(Port2.Mark == 'C')
            {    
                if(Port2.Status == 'S')//;发送状态
                {
                    Port2.WaitAns++;
                    if (Port2.WaitAns > 12) //;3秒无回应
                    {
                        Port2.WaitAns = 0;
                        Port2.ReSNum++;
                        if (Port2.ReSNum < 2)
                        {
                            Port2.Status = 'E'; //;重发
                          
                        }
                        else
                        {
                            ERC21_4Judge(1);   //;判断是否发生485抄表故障
                            Port2.ReSNum = 0;
                            Port2.Status = idle; //;接收完成 
                            CtrlOrder.Status = idle;                           
                            CtrlOrder.IndSend++;         
                        }
                    } 
                }
                else if(Port2.Status == 'R')//;接收状态
                {
                    CtrlOrder.Status = idle;
                    Port2.Status = idle;
                    Port2.Mark = 0;
                    Port2.WaitAns = 0;
                    Port2.ReSNum = 0;
                    Port2.ErrNum = 0;
                    Len = DL645_CHK(Port2.Buffer, Port2.Len);
                    if (Len != 0xFF) //;串口5收到654帧 
                    {
                        if(*(Port2.Buffer + 8 + Len) == 0x84)
                        {
                            CtrlOrder.IndSend++;
                            CtrlOrder.ErrNum = 0x00;
                        }
                        else
                        {
                            CtrlOrder.ErrNum++;
                            if(CtrlOrder.ErrNum > 3) //;允许重发三次
                            {
                                CtrlOrder.ErrNum = 0x00;
                                CtrlOrder.IndSend++;
                            }
                        }
                    }
                    else
                    {
                        CtrlOrder.ErrNum++;
                        if(CtrlOrder.ErrNum > 3) //;允许重发三次
                        {
                            CtrlOrder.ErrNum = 0x00;
                            CtrlOrder.IndSend++;
                        }
                    }              
                    ERC21_4Judge(2);   //;判断485抄表故障是否恢复
                }
            } 
#endif                     
        }
    }
    else
    {
        CtrlOrder.Status = idle;
        CtrlOrder.IndSave = CtrlOrder.IndSend = 0x00;  //;@2
    }
}

/* 
********************************************************************************
* 函 数 名：ZD_EventRead
* 功    能：终端有事件需要读取
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：2012-03-23 09:05:51
********************************************************************************
*/
void ZD_EventRead(void)
{
    if(HWareC.EventFlag && (!HWareC.EventRead))
    {
        HWareC.EventRead = true;
        
        CtrlOrder.Pn[CtrlOrder.IndSave % SBNUM] = 1;      
        CtrlOrder.DI[CtrlOrder.IndSave % SBNUM] = 0x0400050E;
        CtrlOrder.IndSave++;      
    }
}

