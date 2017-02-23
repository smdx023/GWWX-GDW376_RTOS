/*----------------------------------------------------------------------------*
 |  控制测量点数据冻结
 *----------------------------------------------------------------------------*/
#include "GDW376_CFG.h"


/* 
********************************************************************************
* 函 数 名：MVTD_Res
* 功    能：每日过零点时 当日最值，累计时间统计数据清零
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void MVTD_Res(u8 Pn)
{
    u32 *Addr32;
    struct PointDataStr *PnD;

    Addr32 = (u32*)(*(PnDTAddr + Pn));
    PnD = (struct PointDataStr*)(*(Addr32 + _PnD));

    if(DDLDJPn.Status != 'F')    //还没冻结，需要记录
    {
      MovNByte(25, PnD->Z9010_D, PnD->R9010); //;将当前电能示值做为零点电能示值
      MovNByte(25, PnD->Z9020_D, PnD->R9020);
      MovNByte(20, PnD->Z9110_D, PnD->R9110);
      MovNByte(20, PnD->Z9120_D, PnD->R9120);  
     }
     PEZD_Save(Pn, &TBCD.Day);   //;保存当日零点电能
     ZDPN_DME(Pn);               //;立刻计算一次日电量

    //;by zrt 20130924  只有在过零点才计算电量
    ByteN_Fill(32, PnD->DF25.ZPMax  , 0x00); 
    ByteN_Fill(24, PnD->DF26.ZPMN   , 0x00);
    ByteN_Fill(66, PnD->DF27.AVUUMin, 0x00);
    ByteN_Fill(14, PnD->DF28.IImbMin, 0x00);
    ByteN_Fill(38, PnD->DF29.AIUUMin, 0x00);
    ByteN_Fill(4,  PnD->DF30.SUUMin , 0x00);
    ByteN_Fill(10, PnD->DF31.Vmax , 0x00);
    ByteN_Fill(4,  PnD->DF49.PowerT , 0x00);
    ByteN_Fill(4,  PnD->DF50.MonthCutNum , 0x00);
    ByteN_Fill(4,  (u8*)&ZDS.F10.Flux_Day , 0x00);      //;日流量清零
    ByteN_Fill(11,  PnD->DF209.MeterPowerStat , 0x00); 
}


/* 
********************************************************************************
* 函 数 名：MVTM_Res
* 功    能：每月过零点时 当月最值，累计时间数据清零
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void MVTM_Res(u8 Pn)
{
    u32 *Addr32;
    struct PointDataStr *PnD;

    Addr32 = (u32*)(*(PnDTAddr + Pn));
    PnD = (struct PointDataStr*)(*(Addr32 + _PnD));

    MovNByte(14, PnD->DB212_M, PnD->DB212);  //; by zrt 20130815 
    MovNByte(14, PnD->DB217_M, PnD->DB217);
    if(MDLDJPn.Status != 'F') 
    {
      MovNByte(25, PnD->Z9010_M, PnD->R9010);
      MovNByte(25, PnD->Z9020_M, PnD->R9020);
      MovNByte(20, PnD->Z9110_M, PnD->R9110);
      MovNByte(20, PnD->Z9120_M, PnD->R9120); //;将当前电能示值做为零点电能示值
    }
      PEZM_Save(Pn, &TBCD.Month); //;保存当月零点电能
      ZDPN_DME(Pn);               //;立刻计算一次月电量

    //;by zrt 20130924   
    ByteN_Fill(32, PnD->DF33.ZPMax  , 0x00); 
    ByteN_Fill(24, PnD->DF34.ZPMN   , 0x00);
    ByteN_Fill(66, PnD->DF35.AVUUMin, 0x00);
    ByteN_Fill(16, PnD->DF36.IImbMin, 0x00);
    ByteN_Fill(38, PnD->DF37.AIUUMin, 0x00);
    ByteN_Fill(4,  PnD->DF38.SUUMin , 0x00);
    ByteN_Fill(12, PnD->DF39.Vmax , 0x00);
    ByteN_Fill(4,  PnD->DF51.PowerT , 0x00);
    ByteN_Fill(4,  PnD->DF52.MonthCutNum , 0x00);
    ByteN_Fill(4,  (u8*)&ZDS.F10.Flux_Month , 0x00);     //;月流量清零
    ByteN_Fill(14,  PnD->DF213.ProgramNum , 0x00);
    ByteN_Fill(14,  PnD->DF214.CLKChangeNum , 0x00);
}



/* 
********************************************************************************
* 函 数 名：HDJ_Pre
* 功    能：初始化小时冻结参数Td_h
* 参    数：
* 返    回： 
* 说    明：这儿初始化冻结间隔为15分钟，暂不支持分钟冻结
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void HDJ_Pre(u8 Pn)
{
    u8 i;
    u16 temp;
    u8 *adr8;
    u32 *Addr32;
    struct PointDataStr *PnD;

    Addr32 = (u32*)(*(PnDTAddr + Pn));
    PnD = (struct PointDataStr*)(*(Addr32 + _PnD));

    for(i=0; i<28; i++)//;将27个小时冻结标识的冻结密度初始化为15分钟
    {
        if(i == 15) continue;
        temp = PnHDJ_Adr[i];
        adr8 = (u8*)(PnD->Start + temp); //;首地址+偏移
        *(adr8 + 1) = 0x01;
        *(adr8 + 0) = 0x00; 
    }
}


/* 
********************************************************************************
* 函 数 名：HDJ_Move_T2
* 功    能：将两字节小时冻结数据写入对应的数据标识内
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void HDJ_Move_T2(struct DJ_B2 *NDJ, u8 Date_DJ[])
{
    u8 Td, Min_Hex;
    u8 index;
    u8 Hour_Bcd;

    Min_Hex = THEX.Min;
    Hour_Bcd = TBCD.Hour;
	
    if(Min_Hex == 0)
    {
        Min_Hex = 60;
    }
//by zrt 20130719 添加上一整点的小时时间	
     if(Hour_Bcd == 0) 
	 	NDJ->Td_h[0] = 0x23;
     else if(Hour_Bcd == 0x10 || Hour_Bcd == 0x20)
	 	NDJ->Td_h[0] = Hour_Bcd -7;
     else 
	 	NDJ->Td_h[0] = Hour_Bcd- 1;	

    Td = NDJ->Td_h[1];
    if(Td == 1)
    {
        if((Min_Hex % 15) == 0)
        {
            index = Min_Hex / 15;
        }
        else
        {
            index = 0;
        }
    }
    else if(Td == 2)
    {
        if ((Min_Hex % 30) == 0)
        {
            index = Min_Hex / 30;
        }
        else
        {
            index = 0;
        }
    }
    else if(Td == 3)
    {
        index = Min_Hex / 60;
    }
    else
    {
        index = 0;
    }
    if(index != 0)
    {
        index--;
        NDJ->Data[index][0] = Date_DJ[0];
        NDJ->Data[index][1] = Date_DJ[1];
    }
}

/* 
********************************************************************************
* 函 数 名：HDJ_Move_T3
* 功    能：将三字节小时冻结数据写入对应的数据标识内
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void HDJ_Move_T3(struct DJ_B3 *NDJ, u8 Date_DJ[])
{
    u8 Td, Min_Hex;
    u8 index;
    u8 Hour_Bcd;

    Min_Hex = THEX.Min;
    Hour_Bcd = TBCD.Hour;
	
    if(Min_Hex == 0)
    {
        Min_Hex = 60;
    }
//by zrt 20130719 添加上一整点的小时时间	
     if(Hour_Bcd == 0) 
	 	NDJ->Td_h[0] = 0x23;
     else if(Hour_Bcd == 0x10 || Hour_Bcd == 0x20)
	 	NDJ->Td_h[0] = Hour_Bcd -7;
     else 
	 	NDJ->Td_h[0] = Hour_Bcd- 1;	
	
    Td = NDJ->Td_h[1];
    if(Td == 1)
    {
        if((Min_Hex % 15) == 0)
        {
            index = Min_Hex / 15;
        }
        else
        {
            index = 0;
        }
    }
    else if(Td == 2)
    {
        if((Min_Hex % 30) == 0)
        {
            index = Min_Hex / 30;
        }
        else
        {
            index = 0;
        }
    }
    else if(Td == 3)
    {
        index = Min_Hex / 60;
    }
    else
    {
        index = 0;
    }
    if(index != 0)
    {
        index--;
        NDJ->Data[index][0] = Date_DJ[0];
        NDJ->Data[index][1] = Date_DJ[1];
        NDJ->Data[index][2] = Date_DJ[2];
    }
}


/* 
********************************************************************************
* 函 数 名：HDJ_Move_T4
* 功    能：将四字节小时冻结数据写入对应的数据标识内
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void HDJ_Move_T4(struct DJ_B4 *NDJ, u8 *Date_DJ)
{
    u8 Td, Min_Hex;
    u8 index;
    u8 Hour_Bcd;

    Min_Hex = THEX.Min;
    Hour_Bcd = TBCD.Hour;
	
    if(Min_Hex == 0)
    {
        Min_Hex = 60;
    }
//by zrt 20130719 添加上一整点的小时时间	
     if(Hour_Bcd == 0) 
	 	NDJ->Td_h[0] = 0x23;
     else if(Hour_Bcd == 0x10 || Hour_Bcd == 0x20)
	 	NDJ->Td_h[0] = Hour_Bcd -7;
     else 
	 	NDJ->Td_h[0] = Hour_Bcd- 1;	
	
    Td = NDJ->Td_h[1];
    if(Td == 1)
    {
        if((Min_Hex % 15) == 0)
        {
            index = Min_Hex / 15;
        }
        else
        {
            index = 0;
        }
    }
    else if(Td == 2)
    {
        if((Min_Hex % 30) == 0)
        {
            index = Min_Hex / 30;
        }
        else
        {
            index = 0;
        }
    }
    else if(Td == 3)
    {
        index = Min_Hex / 60;
    }
    else
    {
        index = 0;
    }
    if(index != 0)
    {
        index--;
        NDJ->Data[index][0] = *(Date_DJ + 0);
        NDJ->Data[index][1] = *(Date_DJ + 1);
        NDJ->Data[index][2] = *(Date_DJ + 2);
        NDJ->Data[index][3] = *(Date_DJ + 3);
    }
}


/* 
********************************************************************************
* 函 数 名：HDJ_Res_T2
* 功    能：小时冻结2字节数据写入0xEE
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void HDJ_Res_T2(struct DJ_B2 *NDJ)
{
    u8 i;
    NDJ->Td_h[0] &= 0xC0;
    NDJ->Td_h[0] += TBCD.Hour;
    for(i=0; i<4; i++)
    {
        NDJ->Data[i][0] = 0xEE;
        NDJ->Data[i][1] = 0xEE;
    }
}

/* 
********************************************************************************
* 函 数 名：HDJ_Res_T3
* 功    能：小时冻结3字节数据写入0xEE
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void HDJ_Res_T3(struct DJ_B3 *NDJ)
{
    u8 i;
    NDJ->Td_h[0] &= 0xC0;
    NDJ->Td_h[0] += TBCD.Hour;
    for(i=0; i<4; i++)
    {
        NDJ->Data[i][0] = 0xEE;
        NDJ->Data[i][1] = 0xEE;
        NDJ->Data[i][2] = 0xEE;
    }
}

/* 
********************************************************************************
* 函 数 名：HDJ_Res_T4
* 功    能：小时冻结4字节数据写入0xEE
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void HDJ_Res_T4(struct DJ_B4 *NDJ)
{
    u8 i;
    NDJ->Td_h[0] &= 0xC0;
    NDJ->Td_h[0] += TBCD.Hour;
    for(i=0; i<4; i++)
    {
        NDJ->Data[i][0] = 0xEE;
        NDJ->Data[i][1] = 0xEE;
        NDJ->Data[i][2] = 0xEE;
        NDJ->Data[i][3] = 0xEE;
    }
}

/* 
********************************************************************************
* 函 数 名：HDJ_Reset
* 功    能：小时冻结数据复位
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void HDJ_Reset(u8 Pn)
{
    u32 *Addr32;
    struct PointDataStr *PnD;

    Addr32 = (u32*)(*(PnDTAddr + Pn));
    PnD = (struct PointDataStr*)(*(Addr32 + _PnD));

    HDJ_Res_T3(&PnD->NDJ_B630);
    HDJ_Res_T3(&PnD->NDJ_B631);
    HDJ_Res_T3(&PnD->NDJ_B632);
    HDJ_Res_T3(&PnD->NDJ_B633);
    HDJ_Res_T3(&PnD->NDJ_B640);
    HDJ_Res_T3(&PnD->NDJ_B641);
    HDJ_Res_T3(&PnD->NDJ_B642);
    HDJ_Res_T3(&PnD->NDJ_B643);
    HDJ_Res_T2(&PnD->NDJ_B611);
    HDJ_Res_T2(&PnD->NDJ_B612);
    HDJ_Res_T2(&PnD->NDJ_B613);
    HDJ_Res_T3(&PnD->NDJ_B621);
    HDJ_Res_T3(&PnD->NDJ_B622);
    HDJ_Res_T3(&PnD->NDJ_B623);
    HDJ_Res_T3(&PnD->NDJ_B6E5);
    HDJ_Res_T4(&PnD->NDJ_9010_1);
    HDJ_Res_T4(&PnD->NDJ_9110_1);
    HDJ_Res_T4(&PnD->NDJ_9020_1);
    HDJ_Res_T4(&PnD->NDJ_9120_1);
    HDJ_Res_T4(&PnD->NDJ_9010);
    HDJ_Res_T4(&PnD->NDJ_9110);
    HDJ_Res_T4(&PnD->NDJ_9020);
    HDJ_Res_T4(&PnD->NDJ_9120);
    HDJ_Res_T2(&PnD->NDJ_B650);
    HDJ_Res_T2(&PnD->NDJ_B651);
    HDJ_Res_T2(&PnD->NDJ_B652);
    HDJ_Res_T2(&PnD->NDJ_B653);
}


/* 
********************************************************************************
* 函 数 名：HDJ_Program
* 功    能：小时冻结控制
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void HDJ_Program(u8 Pn)
{
    u32 *Addr32;
    struct PointDataStr *PnD;

    Addr32 = (u32*)(*(PnDTAddr + Pn));
    PnD = (struct PointDataStr*)(*(Addr32 + _PnD));
    if((PnD->CB_Time[0] != 0) || (PnD->CB_Time[1] != 0) || (PnD->CB_Time[2] != 0) ||
        (PnD->CB_Time[3] != 0) || (PnD->CB_Time[4] != 0)) //;当前数据存在时才能冻结
    {
        HDJ_Move_T3(&PnD->NDJ_B630, PnD->RB630);
        HDJ_Move_T3(&PnD->NDJ_B631, PnD->RB631);
        HDJ_Move_T3(&PnD->NDJ_B632, PnD->RB632);
        HDJ_Move_T3(&PnD->NDJ_B633, PnD->RB633);
        HDJ_Move_T3(&PnD->NDJ_B640, PnD->RB640);
        HDJ_Move_T3(&PnD->NDJ_B641, PnD->RB641);
        HDJ_Move_T3(&PnD->NDJ_B642, PnD->RB642);
        HDJ_Move_T3(&PnD->NDJ_B643, PnD->RB643);

        HDJ_Move_T2(&PnD->NDJ_B611, PnD->RB611);
        HDJ_Move_T2(&PnD->NDJ_B612, PnD->RB612);
        HDJ_Move_T2(&PnD->NDJ_B613, PnD->RB613);
        HDJ_Move_T3(&PnD->NDJ_B621, PnD->RB621);
        HDJ_Move_T3(&PnD->NDJ_B622, PnD->RB622);
        HDJ_Move_T3(&PnD->NDJ_B623, PnD->RB623);
        HDJ_Move_T3(&PnD->NDJ_B6E5, PnD->RB6E5);

        HDJ_Move_T4(&PnD->NDJ_9010, PnD->R9010_D); //;冻结电能日电量
        HDJ_Move_T4(&PnD->NDJ_9110, PnD->R9110_D);
        HDJ_Move_T4(&PnD->NDJ_9020, PnD->R9020_D);
        HDJ_Move_T4(&PnD->NDJ_9120, PnD->R9120_D);

        HDJ_Move_T4(&PnD->NDJ_9010_1, PnD->R9010 + 1); //;冻结电能示值
        HDJ_Move_T4(&PnD->NDJ_9110_1, PnD->R9110);
        HDJ_Move_T4(&PnD->NDJ_9020_1, PnD->R9020 + 1);
        HDJ_Move_T4(&PnD->NDJ_9120_1, PnD->R9120);

        HDJ_Move_T2(&PnD->NDJ_B650, PnD->RB650);
        HDJ_Move_T2(&PnD->NDJ_B651, PnD->RB651);
        HDJ_Move_T2(&PnD->NDJ_B652, PnD->RB652);
        HDJ_Move_T2(&PnD->NDJ_B653, PnD->RB653);
        
        if(TBCD.Min == 0x00) //;发生小时切换时
        {
            EW.MoveByte = 'M';
            MovNByte(56, PnD->ODJ_B630.Td_h,   PnD->NDJ_B630.Td_h);    //;总,A,B,C有功功率
            MovNByte(56, PnD->ODJ_B640.Td_h,   PnD->NDJ_B640.Td_h);    //;总,A,B,C无功功率
            MovNByte(30, PnD->ODJ_B611.Td_h,   PnD->NDJ_B611.Td_h);    //;A,B,C相电压
//;by zrt 20130719            MovNByte(40, PnD->ODJ_B621.Td_h,   PnD->NDJ_B621.Td_h);    //;A,B,C相电流以及中线电流
            MovNByte(56, PnD->ODJ_B621.Td_h,   PnD->NDJ_B621.Td_h);    //;A,B,C相电流以及中线电流
            MovNByte(72, PnD->ODJ_9010.Td_h,   PnD->NDJ_9010.Td_h);    //;小时冻结日电能量
            MovNByte(72, PnD->ODJ_9010_1.Td_h, PnD->NDJ_9010_1.Td_h);  //;小时冻结总电能示值
            MovNByte(40, PnD->ODJ_B650.Td_h,   PnD->NDJ_B650.Td_h);    //;总,A,B,C相功率因数
            EW.MoveByte = 0;
            HDJ_Reset(Pn); //;重新开始一个小时的小时冻结前将小时冻结数据复位为没有该数据
        }
    }
}

/* 
********************************************************************************
* 函 数 名：
* 功    能：终端日冻结处理函数
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void DDJ_Ctrl(void)
{
    if(DDJPn.Status == 'E')
    {
        u8 Date[3];
        Day_Last(&TBCD.Day, Date);      //;计算出上一日的日期         
        DDJ_Save(DDJPn.Pn,  Date);      //;日冻结
        MVTD_Res(DDJPn.Pn);             //;日统计清零
        DDJPn.Status = 'S';             //;冻结完成          
    }
    
    if(DDJPn.Status == 'S')
    {
        DDJPn.Pn++;
        DDJPn.Status = 'E';
        if(DDJPn.Pn >= PNMAXNUM)
        {
            DDJPn.Pn = 0;
            DDJPn.Status = 0; //;所有测量点冻结完成
        }
    }
}


/* 
********************************************************************************
* 函 数 名：BDJ_Ctrl
* 功    能：终端抄表日冻结
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void BDJ_Ctrl(void)
{
    if(BDJPn.Status == 'E')
    {
        BDJ_Save(BDJPn.Pn, &TBCD.Day);
        BDJPn.Status = 'S';//;冻结完成
    }
   
    if(BDJPn.Status == 'S')
    {
        BDJPn.Pn++;
        BDJPn.Status = 'E';
        if(BDJPn.Pn >= PNMAXNUM)
        {
            BDJPn.Pn = 0;
            BDJPn.Status = 0;
        }
    }
}



/* 
********************************************************************************
* 函 数 名：MDJ_Ctrl
* 功    能：月冻结处理函数
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void MDJ_Ctrl(void)
{
    if(MDJPn.Status == 'E')
    {
        u8 Date[2];        
        Month_Last(&TBCD.Month, Date);    //;计算出上一月的月份  
        MDJ_Save(MDJPn.Pn, Date);         //;月冻结
        MVTM_Res(DDJPn.Pn);               //;月统计清零  
        MDJPn.Status = 'S';//;冻结完成
    }
   
    if(MDJPn.Status == 'S')
    {
        MDJPn.Pn++;
        MDJPn.Status = 'E';
        if(MDJPn.Pn >= PNMAXNUM)
        {
            MDJPn.Pn = 0;
            MDJPn.Status = 0;
        }
    }
}


/* 
********************************************************************************
* 函 数 名：QDJ_Ctrl
* 功    能：在0分，15分，30分，45分对测量点进行曲线冻结
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void QDJ_Ctrl(void)
{
    if(QDJPn.Status == 'E')
    {
        QDJPn.Status = 'S';//;冻结完成
        CDJ_Save(QDJPn.Pn, &TBCD.Min); //;曲线冻结
        HDJ_Program(QDJPn.Pn); //;小时冻结
        HDJ_Save(QDJPn.Pn); //;小时冻结保存
        MVT_Save(QDJPn.Pn); //;保存一次统计时间      
//;        PEZD_Save(QDJPn.Pn, &TBCD.Min);    by zrt 20130907
        PEZD_Save(QDJPn.Pn, &TBCD.Day);
    }
    
    if(QDJPn.Status == 'S')
    {
        QDJPn.Pn++;
        QDJPn.Status = 'E';
        if(QDJPn.Pn >= PNMAXNUM)
        {
            QDJPn.Pn = 0;
            QDJPn.Status = 0;
        }
    }
}


/* 
********************************************************************************
* 函 数 名：ZD_DJPN_CHK
* 功    能：冻结使能检测
* 参    数：
* 返    回： 
* 说    明：每秒执行一次
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
/*
void ZD_DJPN_CHK(void)
{
    if(((THEX.Min%15)==0) && (TBCD.Sec==0x00))  //;曲线冻结
    {
        if(QDJPn.Status == 0)
        {
            QDJPn.Pn = 0; //;从第一个测量点开始冻结
            QDJPn.Status = 'E'; //;冻结开始
        }
    }    
    
    if((TBCD.Hour==0x00) && (TBCD.Min==0x00) && (TBCD.Sec==0x00)) //;当天的零点，日冻结
    {
        if(DDJPn.Status == 0)
        {
            DDJPn.Pn = 0; //;从第一个测量点开始冻结
            DDJPn.Status = 'E'; //;冻结开始
        }
    }
        
    if((TBCD.Day==0x01) && (TBCD.Hour==0x00) && 
       (TBCD.Min==0x00) && (TBCD.Sec==0x00)) //;当月的开始
    {
        if(MDJPn.Status == 0) 
        {
            MDJPn.Pn = 0; //;从第一个测量点开始冻结
            MDJPn.Status = 'E'; //;冻结开始
        }
    }
    
    ZD_CBD_CHK();
}
*/
void ZD_DJPN_CHK(void)
{
	static struct
	{
		u8 ChkDDJ	:1;
		u8 AddDDJ	:1;
		u8 DDJcnt	:2;
		u8 ChkMDJ	:1;
		u8 AddMDJ	:1;
		u8 MDJcnt	:2;
	} AddDJ;	//;补冻结标记
	if ((TBCD.Sec == 0x00) &&(TBCD.Day !=0x00))  //;每分钟检测一次
	{
		if(!AddDJ.ChkDDJ) //;还没检测日冻结
		{
			u8 Date[3];
			u8 node;
			AddDJ.ChkDDJ = true;
			Day_Last(&TBCD.Day, Date);      //;计算出上一日的日期
			node = DDJ_FTP(1,Date);
			if (node == 0xFF)  //;今天0点的数据还未冻结
			{
				AddDJ.AddDDJ = true;
//				AddDJ.DDJcnt = 1;
				AddDJ.DDJcnt = 0;				
             			DDLDJPn.Status = 'T'; //;添加一个标志位'T'当前做零点by zrt 20130930							
			}
			else     //;今天0点已冻结
			{
				AddDJ.AddDDJ = false;
				AddDJ.DDJcnt = 0;
				DDLDJPn.Status = 'F'; //;添加一个标志位'F'当前不做零点by zrt 20130930				
			}
		}
		if (AddDJ.AddDDJ) //;需要补冻结数据
		{
			if (AddDJ.DDJcnt  != 0)
			{
				AddDJ.DDJcnt --;
			}
			else if(DDJPn.Status == 0)  //;最少总共等待1分钟，保证抄回基表数据
			{
				AddDJ.AddDDJ = false;
				DDJPn.Pn = 0; //;从第一个测量点开始冻结
				DDJPn.Status = 'E'; //;冻结开始
			}
		}
		if(!AddDJ.ChkMDJ) //;还没检测月冻结
		{
			u8 Date[2];
			u8 node;
			AddDJ.ChkMDJ = true;
			Month_Last(&TBCD.Month, Date);    //;计算出上一月的月份 
			node = MDJ_FTP(1,Date);
			if (node == 0xFF)  //;今天0点的数据还未冻结
			{
				AddDJ.AddMDJ = true;
//				AddDJ.MDJcnt = 1;
				AddDJ.MDJcnt = 0;				
             			MDLDJPn.Status = 'T'; //;添加一个标志位'T'当前做零点by zrt 20130930						
			}
			else
			{
				AddDJ.AddMDJ = false;
				AddDJ.MDJcnt = 0;
				MDLDJPn.Status = 'F'; //;添加一个标志位'F'当前不做零点by zrt 20130930				
			}
		}
		if (AddDJ.AddMDJ) //;需要补冻结数据
		{
			if (AddDJ.MDJcnt  != 0)
			{
				AddDJ.MDJcnt --;
			}
			else if(MDJPn.Status == 0)  //;最少总共等待1分钟，保证抄回基表数据
			{
				AddDJ.AddMDJ = false;
				MDJPn.Pn = 0; //;从第一个测量点开始冻结
				MDJPn.Status = 'E'; //;冻结开始		
			}
		}
	}
	if(((THEX.Min%15)==0) && (TBCD.Sec==0x00))  //;曲线冻结
	{
		if(QDJPn.Status == 0)
		{
			QDJPn.Pn = 0; //;从第一个测量点开始冻结
			QDJPn.Status = 'E'; //;冻结开始
		}
	}    
	
	if((TBCD.Hour==0x00) && (TBCD.Min==0x00) && (TBCD.Sec==0x00) && (TBCD.Day!=0x00)) //;当天的零点，日冻结
	{
		if(DDJPn.Status == 0)
		{
			DDJPn.Pn = 0; //;从第一个测量点开始冻结
			DDJPn.Status = 'E'; //;冻结开始
           		DDLDJPn.Status = 'Y'; //;添加一个标志位'T'当前做零点by zrt 20130930				
		}
	}
	    
	if((TBCD.Day==0x01) && (TBCD.Hour==0x00) && 
	   (TBCD.Min==0x00) && (TBCD.Sec==0x00)) //;当月的开始
	{
		if(MDJPn.Status == 0) 
		{
			MDJPn.Pn = 0; //;从第一个测量点开始冻结
			MDJPn.Status = 'E'; //;冻结开始
      MDLDJPn.Status = 'Y'; //;添加一个标志位'T'当前做零点by zrt 20130930				
		}
	}

	ZD_CBD_CHK();
}

/* 
********************************************************************************
* 函 数 名：ZD_DJPN
* 功    能：测量点冻结控制
* 参    数：每250ms执行一次
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void ZD_DJPN(void)
{
    QDJ_Ctrl(); //;曲线冻结
    DDJ_Ctrl(); //;日冻结
    BDJ_Ctrl(); //;抄表日冻结
    MDJ_Ctrl(); //;月冻结 
}


