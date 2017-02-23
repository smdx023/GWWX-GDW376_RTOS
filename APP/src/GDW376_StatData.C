/*
********************************************************************************
*  文 件 名：GDW376_StatData.C
*
*  文件描述：电压电流等统计数据的判断和处理
*
*  所用芯片：
*
*  创 建 人：
*
*  版 本 号：
*
*  修改记录：2011-09-26 14:16:56 
********************************************************************************
*/

#include "GDW376_CFG.h"

//;电压超上限、超下限、超上上限、超下下限状态
u8 UHStateA, ULStateA, UHHStateA, ULLStateA;
u8 UHStateB, ULStateB, UHHStateB, ULLStateB;
u8 UHStateC, ULStateC, UHHStateC, ULLStateC;
//;发生延时时间
u8 UHDLYMinSA, ULDLYMinSA, UHHDLYMinSA, ULLDLYMinSA;
u8 UHDLYMinSB, ULDLYMinSB, UHHDLYMinSB, ULLDLYMinSB;
u8 UHDLYMinSC, ULDLYMinSC, UHHDLYMinSC, ULLDLYMinSC;

//;结束延时时间
u8 UHDLYMinEA, ULDLYMinEA, UHHDLYMinEA, ULLDLYMinEA;
u8 UHDLYMinEB, ULDLYMinEB, UHHDLYMinEB, ULLDLYMinEB;
u8 UHDLYMinEC, ULDLYMinEC, UHHDLYMinEC, ULLDLYMinEC;

//;相电流超上限、超上上限、零序电流超上限
u8 IHStateA, IHHStateA, I0HState;
u8 IHStateB, IHHStateB;
u8 IHStateC, IHHStateC;
//;发生延时时间
u8 IHDLYMinSA, IHHDLYMinSA, I0HDLYMinS;
u8 IHDLYMinSB, IHHDLYMinSB;
u8 IHDLYMinSC, IHHDLYMinSC;

//;结束延时时间
u8 IHDLYMinEA, IHHDLYMinEA, I0HDLYMinE;
u8 IHDLYMinEB, IHHDLYMinEB;
u8 IHDLYMinEC, IHHDLYMinEC;

//;视在功率超上限、超上上限
u8 SHState, SHHState;

//;发生延时时间
u8 SHDLYMinS, SHHDLYMinS;

//;结束延时时间
u8 SHDLYMinE, SHHDLYMinE;

//;电压不平衡、电流不平衡延时时间累计
u8 UImbDLYMinS, UImbDLYMinE, IImbDLYMinS, IImbDLYMinE;


/* 
********************************************************************************
* 函 数 名：VolStatData
* 功    能：电压合格、过压、欠压等事件的判断及统计
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void VolStatData(struct PoPStr  *PnP, struct PointDataStr *PnD, struct ERCPnData *ERCPn)
{
    u16 Ua, Ub, Uc, UHH, UHigh, ULL, ULow;
    s16 rate, UHH_End, ULL_End;	
    
    Ua = BCD2ToU16(PnD->RB611);
    Ub = BCD2ToU16(PnD->RB612);
    Uc = BCD2ToU16(PnD->RB613);
       
    UHH = BCD2ToU16(PnP->F26.UHH);       //;电压上上限
    UHigh = BCD2ToU16(PnP->F26.UHigh);   //;电压上限
    ULL = BCD2ToU16(PnP->F26.ULL);       //;电压下下限
    ULow = BCD2ToU16(PnP->F26.ULow);     //;电压下限

/*
    UHH_End = BCD2ToU16(PnP->F26.GY_Ratio) * UHH / 1000;
    ULL_End = BCD2ToU16(PnP->F26.QY_Ratio) * ULL / 1000;  //;应比ULL大
*/

    rate = BCD2ToS16(PnP->F26.GY_Ratio); //;比例的1000倍
    if(rate < 0) rate = rate + 1000;			 //;需要进行计算的比例
    UHH_End = UHH * rate / 1000;
    rate = BCD2ToS16(PnP->F26.QY_Ratio); //;比例的1000倍
    if(rate < 0) rate = rate + 1000;			 //;需要进行计算的比例
    ULL_End = ULL * rate / 1000;
    
    
    //;A相电压越上上限事件判断及统计
    if(Ua > UHH)
    {
        if(UHHStateA)
        {
            (*((u16*)PnD->DF27.AVUUMin))++;  //;越上上限时间统计
            (*((u16*)PnD->DF35.AVUUMin))++;
        }
        else
        {
            UHHDLYMinSA++;
            if(UHHDLYMinSA >= PnP->F26.GY_DLY) //;超过判定延时时间
            {
                UHHDLYMinSA = 0;
                UHHStateA = true;  //;电压越上上限发生
                ERCPn->ERC24.Status_N |= 0x01;    //;越上上限状态发生
            }
        }
    }
    else if(Ua <= UHH_End) //;越上上限恢复
    {
        if(UHHStateA)
        {
            UHHDLYMinEA++;
            if(UHHDLYMinEA >= PnP->F26.GY_DLY) //;超过判定延时时间
            {
                UHHDLYMinEA = 0;
                UHHStateA = false;  //;电压越上上限结束
                ERCPn->ERC24.Status_N &= (~0x01);
            }
        }       
    }
    else
    {
        if(UHHStateA)
        {
            (*((u16*)PnD->DF27.AVUUMin))++;  //;越上上限时间统计
            (*((u16*)PnD->DF35.AVUUMin))++;
        }
    }
    
    //;A相电压越下下限事件判断及统计
    if((Ua < ULL) && (Ua > 0))
    {
        if(ULLStateA)
        {
            (*((u16*)PnD->DF27.AVLLMin))++;  //;越下下限时间统计
            (*((u16*)PnD->DF35.AVLLMin))++;
        }
        else
        {
            ULLDLYMinSA++;
            if(ULLDLYMinSA >= PnP->F26.QY_DLY) //;超过判定延时时间
            {
                ULLDLYMinSA = 0;
                ULLStateA = true;  //;电压越下下限发生
                ERCPn->ERC24.Status_N |= 0x02;    //;越下下限状态发生
            }
        }
    }
    else if(Ua >= ULL_End) //;越下下限恢复
    {
        if(ULLStateA)
        {
            ULLDLYMinEA++;
            if(ULLDLYMinEA >= PnP->F26.QY_DLY) //;超过判定延时时间
            {
                ULLDLYMinEA = 0;
                ULLStateA = false;  //;电压越下下限恢复
                ERCPn->ERC24.Status_N &= (~0x02); //;越下下限状态结束
            }
        }       
    }
    else
    {
        if(ULLStateA)
        {
            (*((u16*)PnD->DF27.AVLLMin))++;  //;越下下限时间统计
            (*((u16*)PnD->DF35.AVLLMin))++;
        }
    }
    
    //;A相电压越上限
    if((Ua <= UHH) && (Ua > UHigh))
    {
        if(UHStateA)
        {
            (*((u16*)PnD->DF27.AVUMin))++;
            (*((u16*)PnD->DF35.AVUMin))++;   //;越上限时间统计
        }
        else
        {
            UHDLYMinSA++;
            if(UHDLYMinSA >= PnP->F26.GY_DLY) //;超过判定延时时间
            {
                UHDLYMinSA = 0;
                UHStateA = true;  //;电压越上限发生
            }
        }
        
    }
    else
    {
        if(UHStateA)
        {
            UHDLYMinEA++;
            if(UHDLYMinEA >= PnP->F26.GY_DLY) //;超过判定延时时间
            {
                UHDLYMinEA = 0;
                UHStateA = false;  //;电压越上限恢复
            }
        }
    }     
    
    //;A相电压越下限  
    if((Ua <= UHigh) && (Ua >= ULow))
    {
        if(ULStateA)
        {
            (*((u16*)PnD->DF27.AVHGMin))++;  //;电压合格时间统计
            (*((u16*)PnD->DF35.AVHGMin))++;
        }
        else
        {
            ULDLYMinSA++;
            if(ULDLYMinSA >= PnP->F26.QY_DLY) //;超过判定延时时间
            {
                ULDLYMinSA = 0;
                ULStateA = true;  //;电压越下限发生
            }
        }
    }
    else
    {
        if(ULStateA)
        {
            ULDLYMinEA++;
            if(ULDLYMinEA >= PnP->F26.QY_DLY) //;超过判定延时时间
            {
                ULDLYMinEA = 0;
                ULStateA = false;  //;电压越下限恢复
            }
        }
    }
    
    //;A电压合格率判断
    if((Ua < ULow) && (Ua >= ULL))
    {
        (*((u16*)PnD->DF27.AVLMin))++;   //;越下限时间统计
        (*((u16*)PnD->DF35.AVLMin))++;
    }   
    
    //;C相电压越上上限事件判断及统计
    if(Uc > UHH)
    {
        if(UHHStateC)
        {
            (*((u16*)PnD->DF27.CVUUMin))++;  //;越上上限时间统计
            (*((u16*)PnD->DF35.CVUUMin))++;
        }
        else
        {
            UHHDLYMinSC++;
            if(UHHDLYMinSC >= PnP->F26.GY_DLY) //;超过判定延时时间
            {
                UHHDLYMinSC = 0;
                UHHStateC = true;  //;电压越上上限发生
                ERCPn->ERC24.Status_N|= 0x10;     //;越上上限状态发生
            }
        }
    }
    else if(Uc <= UHH_End) //;越上上限恢复
    {
        if(UHHStateC)
        {
            UHHDLYMinEC++;
            if(UHHDLYMinEC >= PnP->F26.GY_DLY) //;超过判定延时时间
            {
                UHHDLYMinEC = 0;
                UHHStateC = false;  //;电压越上上限发生
                ERCPn->ERC24.Status_N &= (~0x10);  //;越上上限状态结束
            }
        }       
    }
    else
    {
        if(UHHStateC)
        {
            (*((u16*)PnD->DF27.CVUUMin))++;  //;越上上限时间统计
            (*((u16*)PnD->DF35.CVUUMin))++;
        }
    }
    
    //;C相电压越下下限事件判断及统计
    if((Uc < ULL) && (Uc > 0))
    {
        if(ULLStateC)
        {
            (*((u16*)PnD->DF27.CVLLMin))++;  //;越下下限时间统计
            (*((u16*)PnD->DF35.CVLLMin))++;
        }
        else
        {
            ULLDLYMinSC++;
            if(ULLDLYMinSC >= PnP->F26.QY_DLY) //;超过判定延时时间
            {
                ULLDLYMinSC = 0;
                ULLStateC = true;  //;电压越下下限发生
                ERCPn->ERC24.Status_N |= 0x20; //;越下下限状态发生
            }
        }
    }
    else if(Uc >= ULL_End) //;越下下限恢复
    {
        if(ULLStateC)
        {
            ULLDLYMinEC++;
            if(ULLDLYMinEC >= PnP->F26.QY_DLY) //;超过判定延时时间
            {
                ULLDLYMinEC = 0;
                ULLStateC = false;  //;电压越下下限恢复
                ERCPn->ERC24.Status_N &= (~0x20);  //;越下下限状态结束
            }
        }       
    }
    else
    {
        if(ULLStateC)
        {
            (*((u16*)PnD->DF27.CVLLMin))++;  //;越下下限时间统计
            (*((u16*)PnD->DF35.CVLLMin))++;
        }
    }
    
    //;C相电压越上限
    if((Uc <= UHH) && (Uc > UHigh))
    {
        if(UHStateC)
        {
            (*((u16*)PnD->DF27.CVUMin))++;
            (*((u16*)PnD->DF35.CVUMin))++;   //;越上限时间统计
        }
        else
        {
            UHDLYMinSC++;
            if(UHDLYMinSC >= PnP->F26.GY_DLY) //;超过判定延时时间
            {
                UHDLYMinSC = 0;
                UHStateC = true;  //;电压越上限发生
            }
        }
        
    }
    else
    {
        if(UHStateC)
        {
            UHDLYMinEC++;
            if(UHDLYMinEC >= PnP->F26.GY_DLY) //;超过判定延时时间
            {
                UHDLYMinEC = 0;
                UHStateC = false;  //;电压越上限恢复
            }
        }
    }     
    
    //;C相电压越下限  
    if((Uc <= UHigh) && (Uc >= ULow))
    {
        if(ULStateC)
        {
            (*((u16*)PnD->DF27.CVHGMin))++;  //;电压合格时间统计
            (*((u16*)PnD->DF35.CVHGMin))++;
        }
        else
        {
            ULDLYMinSC++;
            if(ULDLYMinSC >= PnP->F26.QY_DLY) //;超过判定延时时间
            {
                ULDLYMinSC = 0;
                ULStateC = true;  //;电压越下限发生
            }
        }
    }
    else
    {
        if(ULStateC)
        {
            ULDLYMinEC++;
            if(ULDLYMinEC >= PnP->F26.QY_DLY) //;超过判定延时时间
            {
                ULDLYMinEC = 0;
                ULStateC = false;  //;电压越下限恢复
            }
        }
    }
    
    //;C电压合格率判断
    if((Uc < ULow) && (Uc >= ULL))
    {
        (*((u16*)PnD->DF27.CVLMin))++;   //;越下限时间统计
        (*((u16*)PnD->DF35.CVLMin))++;
    }   
    
  
    if (PnP->F25.Type == 2)
    {
        //;B相电压越上上限事件判断及统计
        if(Ub > UHH)
        {
            if(UHHStateB)
            {
                (*((u16*)PnD->DF27.BVUUMin))++;  //;越上上限时间统计
                (*((u16*)PnD->DF35.BVUUMin))++;
            }
            else
            {
                UHHDLYMinSB++;
                if(UHHDLYMinSB >= PnP->F26.GY_DLY) //;超过判定延时时间
                {
                    UHHDLYMinSB = 0;
                    UHHStateB = true;  //;电压越上上限发生
                    ERCPn->ERC24.Status_N |= 0x04;   //;越上上限状态发生
                }
            }
        }
        else if(Ub <= UHH_End) //;越上上限恢复
        {
            if(UHHStateB)
            {
                UHHDLYMinEB++;
                if(UHHDLYMinEB >= PnP->F26.GY_DLY) //;超过判定延时时间
                {
                    UHHDLYMinEB = 0;
                    UHHStateB = false;  //;电压越上上限发生
                    ERCPn->ERC24.Status_N &= (~0x04);//;越上上限状态结束
                }
            }       
        }
        else
        {
            if(UHHStateB)
            {
                (*((u16*)PnD->DF27.BVUUMin))++;  //;越上上限时间统计
                (*((u16*)PnD->DF35.BVUUMin))++;
            }
        }
        
        //;B相电压越下下限事件判断及统计
        if((Ub < ULL) && (Ub > 0))
        {
            if(ULLStateB)
            {
                (*((u16*)PnD->DF27.BVLLMin))++;  //;越下下限时间统计
                (*((u16*)PnD->DF35.BVLLMin))++;
            }
            else
            {
                ULLDLYMinSB++;
                if(ULLDLYMinSB >= PnP->F26.QY_DLY) //;超过判定延时时间
                {
                    ULLDLYMinSB = 0;
                    ULLStateB = true;  //;电压越下下限发生
                    ERCPn->ERC24.Status_N |= 0x08;   //;越下下限状态发生
                }
            }
        }
        else if(Ub >= ULL_End) //;越下下限恢复
        {
            if(ULLStateB)
            {
                ULLDLYMinEB++;
                if(ULLDLYMinEB >= PnP->F26.QY_DLY) //;超过判定延时时间
                {
                    ULLDLYMinEB = 0;
                    ULLStateB = false;  //;电压越下下限恢复
                    ERCPn->ERC24.Status_N &= (~0x08);//;越下下限状态结束
                }
            }       
        }
        else
        {
            if(ULLStateB)
            {
                (*((u16*)PnD->DF27.BVLLMin))++;  //;越下下限时间统计
                (*((u16*)PnD->DF35.BVLLMin))++;
            }
        }
        
        //;B相电压越上限
        if((Ub <= UHH) && (Ub > UHigh))
        {
            if(UHStateB)
            {
                (*((u16*)PnD->DF27.BVUMin))++;
                (*((u16*)PnD->DF35.BVUMin))++;   //;越上限时间统计
            }
            else
            {
                UHDLYMinSB++;
                if(UHDLYMinSB >= PnP->F26.GY_DLY) //;超过判定延时时间
                {
                    UHDLYMinSB = 0;
                    UHStateB = true;  //;电压越上限发生
                }
            }
            
        }
        else
        {
            if(UHStateB)
            {
                UHDLYMinEB++;
                if(UHDLYMinEB >= PnP->F26.GY_DLY) //;超过判定延时时间
                {
                    UHDLYMinEB = 0;
                    UHStateB = false;  //;电压越上限恢复
                }
            }
        }     
        
        //;B相电压越下限  
        if((Ub <= UHigh) && (Ub >= ULow))
        {
            if(ULStateB)
            {
                (*((u16*)PnD->DF27.BVHGMin))++;  //;电压合格时间统计
                (*((u16*)PnD->DF35.BVHGMin))++;
            }
            else
            {
                ULDLYMinSB++;
                if(ULDLYMinSB >= PnP->F26.QY_DLY) //;超过判定延时时间
                {
                    ULDLYMinSB = 0;
                    ULStateB = true;  //;电压越下限发生
                }
            }
        }
        else
        {
            if(ULStateB)
            {
                ULDLYMinEB++;
                if(ULDLYMinEB >= PnP->F26.QY_DLY) //;超过判定延时时间
                {
                    ULDLYMinEB = 0;
                    ULStateB = false;  //;电压越下限恢复
                }
            }
        }
        
        //;B电压合格率判断
        if((Ub < ULow) && (Ub >= ULL))
        {
            (*((u16*)PnD->DF27.BVLMin))++;   //;越下限时间统计
            (*((u16*)PnD->DF35.BVLMin))++;
        }   
    }
}


/* 
********************************************************************************
* 函 数 名：CurStatData
* 功    能：电流过限等事件的判断及统计
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void CurStatData(struct PoPStr  *PnP, struct PointDataStr *PnD, struct ERCPnData *ERCPn)
{
    u32 Ia, Ib, Ic, I0;
    u32 IHH, IH, I0H;
//    u32 IHH_End, IH_End, I0H_End;
    s16 rate,IHH_End, IH_End, I0H_End;
//电流只比较绝对值    

    Ia = BCD3ToS32(PnD->RB621);
    Ic = BCD3ToS32(PnD->RB623);
    I0 = BCD3ToS32(PnD->RB6E5);

    IHH = BCD3ToS32(PnP->F26.IHH);       //;相电流上上限
    IH = BCD3ToS32(PnP->F26.IH);         //;相电流上限
    I0H = BCD3ToS32(PnP->F26.I0High); //;零序电流上限
    
    rate = BCD2ToS16(PnP->F26.GL_Ratio);
    if (rate < 0) rate = rate + 1000;
    IHH_End =  IHH * rate / 1000;
    rate = BCD2ToS16(PnP->F26.GBL_Ratio);
    if(rate < 0) rate = rate + 1000;
    IH_End =  IH * rate  / 1000;
    rate = BCD2ToS16(PnP->F26.G0L_Ratio);
    if(rate <0 ) rate = rate + 1000;	
    I0H_End = I0H * rate  / 1000;


/*
    Ia = BCD3ToU32(PnD->RB621);
    Ic = BCD3ToU32(PnD->RB623);
    I0 = BCD3ToU32(PnD->RB6E5);

    IHH = BCD3ToU32(PnP->F26.IHH);       //;相电流上上限
    IH = BCD3ToU32(PnP->F26.IH);         //;相电流上限
    I0H = BCD3ToU32(PnP->F26.I0High); //;零序电流上限
    
   
    IHH_End = BCD2ToU16(PnP->F26.GL_Ratio) * IHH / 1000;
    IH_End = BCD2ToU16(PnP->F26.GBL_Ratio) * IH / 1000;
    I0H_End = BCD2ToU16(PnP->F26.G0L_Ratio) * I0H / 1000;
*/
  
    
    /*A相电流越上上限时间统计*/
    if(Ia > IHH)
    {
        if(IHHStateA)
        {
            (*((u16*)PnD->DF29.AIUUMin))++;  //;电流越上上限时间统计
            (*((u16*)PnD->DF37.AIUUMin))++;
        }
        else
        {
            IHHDLYMinSA++;
            if(IHHDLYMinSA >= PnP->F26.GL_DLY) //;超过判定延时时间
            {
                IHHDLYMinSA = 0x00;
                IHHStateA = true;
                ERCPn->ERC25.Status_N |= 0x01;    //;越上上限状态发生
            }
        }
    }
    else if(Ia <= IHH_End)
    {
        if(IHHStateA)
        {
            IHHDLYMinEA++;
            if(IHHDLYMinEA >= PnP->F26.GL_DLY) //;超过判定延时时间
            {
                IHHDLYMinEA = 0x00;
                IHHStateA = false;
                ERCPn->ERC25.Status_N &= (~0x01); //;越上上限状态结束
            }
        }       
    }
    else
    {
        if(IHHStateA)
        {
            (*((u16*)PnD->DF29.AIUUMin))++;  //;电流越上上限时间统计
            (*((u16*)PnD->DF37.AIUUMin))++;
        }
    }
    
    /*A相电流越上限时间统计*/
    if((Ia>IH) && (Ia <= IHH))
    {
        if(IHStateA)
        {
            (*((u16*)PnD->DF29.AIUMin))++;  //;电流越上上限时间统计
            (*((u16*)PnD->DF37.AIUMin))++;
        }
        else
        {
            IHDLYMinSA++;
            if(IHDLYMinSA >= PnP->F26.GBL_DLY) //;超过判定延时时间
            {
                IHDLYMinSA = 0x00;
                IHStateA = true;
                ERCPn->ERC25.Status_N |= 0x02;    //;越上限状态发生
            }
        }
    }
    else if(Ia <= IH_End)
    {
        if(IHStateA)
        {
            IHDLYMinEA++;
            if(IHDLYMinEA >= PnP->F26.GBL_DLY) //;超过判定延时时间
            {
                IHDLYMinEA = 0x00;
                IHStateA = false;
                ERCPn->ERC25.Status_N &= (~0x02); //;越上限状态结束
            }
        }       
    }
    else
    {
        if(IHStateA)
        {
            (*((u16*)PnD->DF29.AIUMin))++;  //;电流越上限时间统计
            (*((u16*)PnD->DF37.AIUMin))++;
        }
    }
    
    /*C相电流越上上限时间统计*/
    if(Ic > IHH)
    {
        if(IHHStateC)
        {
            (*((u16*)PnD->DF29.CIUUMin))++;  //;电流越上上限时间统计
            (*((u16*)PnD->DF37.CIUUMin))++;
        }
        else
        {
            IHHDLYMinSC++;
            if(IHHDLYMinSC >= PnP->F26.GL_DLY) //;超过判定延时时间
            {
                IHHDLYMinSC = 0x00;
                IHHStateC = true;
                ERCPn->ERC25.Status_N |= 0x10;    //;越上上限状态发生
            }
        }
    }
    else if(Ic <= IHH_End)
    {
        if(IHHStateC)
        {
            IHHDLYMinEC++;
            if(IHHDLYMinEC >= PnP->F26.GL_DLY) //;超过判定延时时间
            {
                IHHDLYMinEC = 0x00;
                IHHStateC = false;
                ERCPn->ERC25.Status_N &= (~0x10); //;越上上限状态结束
            }
        }       
    }
    else
    {
        if(IHHStateC)
        {
            (*((u16*)PnD->DF29.CIUUMin))++;  //;电流越上上限时间统计
            (*((u16*)PnD->DF37.CIUUMin))++;
        }
    }
    
    /*C相电流越上限时间统计*/
    if((Ic <= IHH) && (Ic > IH))
    {
        if(IHStateC)
        {
            (*((u16*)PnD->DF29.CIUMin))++;   //;电流越上限时间统计
            (*((u16*)PnD->DF37.CIUMin))++;
        }
        else
        {
            IHDLYMinSC++;
            if(IHDLYMinSC >= PnP->F26.GBL_DLY) //;超过判定延时时间
            {
                IHDLYMinSC = 0x00;
                IHStateC = true;
                ERCPn->ERC25.Status_N |= 0x20;    //;越上限状态发生
            }
        }
    }
    else if(Ic <= IH_End)
    {
        if(IHStateC)
        {
            IHDLYMinEC++;
            if(IHDLYMinEC >= PnP->F26.GBL_DLY) //;超过判定延时时间
            {
                IHDLYMinEC = 0x00;
                IHStateC = false;
                ERCPn->ERC25.Status_N &= (~0x20); //;越上限状态结束
            }
        }       
    }
    else
    {
        if(IHStateC)
        {
            (*((u16*)PnD->DF29.CIUMin))++;   //;电流越上限时间统计
            (*((u16*)PnD->DF37.CIUMin))++;
        }
    }
    
    /*零序电流越上限时间统计*/
    if(I0 > I0H)
    {
        if(I0HState)
        {
            (*((u16*)PnD->DF29.I0UMin))++;   //;零序电流越上限时间统计
            (*((u16*)PnD->DF37.I0UMin))++;
        }
        else
        {
            I0HDLYMinS++;
            if(I0HDLYMinS >= PnP->F26.G0L_DLY) //;超过判定延时时间
            {
                I0HDLYMinS = 0x00;
                I0HState= true;
            }
        }
    }
    else if(I0 <= I0H_End)
    {
        if(I0HState)
        {
            I0HDLYMinE++;
            if(I0HDLYMinE >= PnP->F26.GBL_DLY) //;超过判定延时时间
            {
                I0HDLYMinE = 0x00;
                I0HState = false;
            }
        }       
    }
    else
    {
        if(I0HState)
        {
            (*((u16*)PnD->DF29.I0UMin))++;   //;零序电流越上限时间统计
            (*((u16*)PnD->DF37.I0UMin))++;
        }
    }

    if(PnP->F25.Type == 2)
    {
        Ib = BCD3ToS32(PnD->RB622);

        /*B相电流越上上限时间统计*/
        if(Ib > IHH)
        {
            if(IHHStateB)
            {
                (*((u16*)PnD->DF29.BIUUMin))++;  //;电流越上上限时间统计
                (*((u16*)PnD->DF37.BIUUMin))++;
            }
            else
            {
                IHHDLYMinSB++;
                if(IHHDLYMinSB >= PnP->F26.GL_DLY) //;超过判定延时时间
                {
                    IHHDLYMinSB = 0x00;
                    IHHStateB = true;
                    ERCPn->ERC25.Status_N |= 0x04;    //;越上上限状态发生
                }
            }
        }
        else if(Ib <= IHH_End)
        {
            if(IHHStateB)
            {
                IHHDLYMinEB++;
                if(IHHDLYMinEB >= PnP->F26.GL_DLY) //;超过判定延时时间
                {
                    IHHDLYMinEB = 0x00;
                    IHHStateB = false;
                    ERCPn->ERC25.Status_N &= (~0x04); //;越上上限状态结束
                }
            }       
        }
        else
        {
            if(IHHStateB)
            {
                (*((u16*)PnD->DF29.BIUUMin))++;  //;电流越上上限时间统计
                (*((u16*)PnD->DF37.BIUUMin))++;
            }
        }
        
        /*B相电流越上限时间统计*/
        if((Ib <= IHH) && (Ib > IH))
        {
            if(IHStateB)
            {
                (*((u16*)PnD->DF29.BIUMin))++;   //;电流越上限时间统计
                (*((u16*)PnD->DF37.BIUMin))++;
            }
            else
            {
                IHDLYMinSB++;
                if(IHDLYMinSB >= PnP->F26.GBL_DLY) //;超过判定延时时间
                {
                    IHDLYMinSB = 0x00;
                    IHStateB = true;
                    ERCPn->ERC25.Status_N |= 0x08;    //;越上限状态发生
                }
            }
        }
        else if(Ib <= IH_End)
        {
            if(IHStateB)
            {
                IHDLYMinEB++;
                if(IHDLYMinEB >= PnP->F26.GBL_DLY) //;超过判定延时时间
                {
                    IHDLYMinEB = 0x00;
                    IHStateB = false;
                    ERCPn->ERC25.Status_N &= (~0x08); //;越上限状态结束
                }
            }       
        }
        else
        {
            if(IHStateB)
            {
                (*((u16*)PnD->DF29.BIUMin))++;   //;电流越上限时间统计
                (*((u16*)PnD->DF37.BIUMin))++;
            }
        }
    }

}

/* 
********************************************************************************
* 函 数 名：CurStatData
* 功    能：视在功率过限等事件的判断及统计
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void SStatData(struct PoPStr  *PnP, struct PointDataStr *PnD, struct ERCPnData *ERCPn)
{
    u32 S, SHH, SH;
    s16 rate, SHH_End, SH_End;
/*    
     S = BCD3ToU32(PnD->RB660);           //;当前视在功率
     SHH = BCD3ToU32(PnP->F26.SHH);       //;视在功率上上限
     SH = BCD3ToU32(PnP->F26.SH);         //;视在功率上限

     SHH_End = BCD2ToU16(PnP->F26.SHH_Ratio) * SHH / 1000;
     SH_End  = BCD2ToU16(PnP->F26.SH_Ratio) * SH/ 1000;
*/
     S = BCD3ToS32(PnD->RB660);           //;当前视在功率
     SHH = BCD3ToU32(PnP->F26.SHH);       //;视在功率上上限
     SH = BCD3ToU32(PnP->F26.SH);         //;视在功率上限

    rate = BCD2ToS16(PnP->F26.SHH_Ratio);
    if(rate < 0) rate = rate + 1000;
    SHH_End = SHH * rate /1000;    
    rate = BCD2ToS16(PnP->F26.SH_Ratio);
    if(rate < 0) rate = rate + 1000;
    SH_End = SH * rate /1000;  
   
    
    
    /*视在功率越上上限时间统计*/
    if(S > SHH)
    {
        if(SHHState)
        {
            (*((u16*)PnD->DF30.SUUMin))++; //;视在功率越上上限时间统计
            (*((u16*)PnD->DF38.SUUMin))++;
        }
        else
        {
            SHHDLYMinS++;
            if(SHHDLYMinS >= PnP->F26.SHH_DLY)
            {
                SHHDLYMinS = 0x00;
                SHHState = true;
                ERCPn->ERC26.Status_N |= 0x02;
            }
        }
    }
    else if(S < SHH_End)
    {
        if(SHHState)
        {
            SHHDLYMinE++;
            if(SHHDLYMinE >= PnP->F26.SHH_DLY)
            {
                SHHDLYMinE = 0x00;
                SHHState = false;
                ERCPn->ERC26.Status_N &= (~0x02);
            }
        }
    }
    else
    {
        if(SHHState)
        {
            (*((u16*)PnD->DF30.SUUMin))++; //;视在功率越上上限时间统计
            (*((u16*)PnD->DF38.SUUMin))++;
        }
    }
    
    /*视在功率越上限时间统计*/
    if((S > SH) && (S<=SHH))
    {
        if(SHState)
        {
            (*((u16*)PnD->DF30.SUMin))++;   //;视在功率越上限时间统计
            (*((u16*)PnD->DF38.SUMin))++;
        }
        else
        {
            SHDLYMinS++;
            if(SHDLYMinS >= PnP->F26.SH_DLY)
            {
                SHDLYMinS = 0x00;
                SHState = true;
                ERCPn->ERC26.Status_N |= 0x01;
            }
        }
    }
    else if(S < SH_End)
    {
        if(SHState)
        {
            SHDLYMinE++;
            if(SHDLYMinE >= PnP->F26.SH_DLY)
            {
                SHDLYMinE = 0x00;
                SHState = false;
                ERCPn->ERC26.Status_N &= (~0x01);
            }
        }
    }
    else
    {
        if(SHState)
        {
            (*((u16*)PnD->DF30.SUMin))++;   //;视在功率越上限时间统计
            (*((u16*)PnD->DF38.SUMin))++;
        }
    }        
}

/* 
********************************************************************************
* 函 数 名：CurStatData
* 功    能：电压电流不平衡等事件的判断及统计
* 参    数：
* 返    回： 
* 说    明：：电压，电流不平衡：  BPH(max) > BPH(XZ)
*           avr = (a + b+ C)/3   三相平均值
*           BPH(max) = [(|a/b/c - avr|)/avr]*100%  当前不平衡度最大值
*           BPH(XZ) 不平衡度限值
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
/*
void IMBStatData(struct PoPStr  *PnP, struct PointDataStr *PnD, struct ERCPnData *ERCPn)
{
    //;UImb是电压不平衡限值， Upn是电压当前不平衡度
    u16 Ua, Ub = 0, Uc, Uave, UImb, Upn = 0, UImb_End, Ipn = 0; 
    u32 Ia, Ib = 0, Ic, Iave, IImb , IImb_End;
    u32 temp;
    s16 rate;


    Ua = BCD2ToU16(PnD->RB611);
    Uc = BCD2ToU16(PnD->RB613);
    Ia = BCD3ToU32(PnD->RB621);
    Ic = BCD3ToU32(PnD->RB623);

    UImb = BCD2ToU16(PnP->F26.UImb); //;电压不平衡限值 (%) 放大了10倍
    IImb = BCD2ToU16(PnP->F26.IImb); //;电流不平衡限值 (%)
    
    UImb_End = BCD2ToU16(PnP->F26.UImb_Ratio) * UImb / 1000;
    IImb_End = BCD2ToU16(PnP->F26.IImb_Ratio) * IImb / 1000;

    rate = BCD2ToS16(PnP->F26.UImb_Ratio);
    rate = rate + 1000 ;
    UImb_End =UImb * rate  / 1000;
    rate = BCD2ToS16(PnP->F26.IImb_Ratio);
    rate = rate + 1000 ;
    IImb_End =IImb * rate  / 1000;

    if(PnP->F25.Type == 2)
    {
        Ub = BCD2ToU16(PnD->RB612);
        Ib = BCD3ToS32(PnD->RB622);
        Uave = (Ua + Ub + Uc) / 3;
        Iave = (Ia + Ib + Ic) / 3; //;平均值
    }
    else if(PnP->F25.Type == 1)
    {
        Uave = (Ua + Uc) / 2;
        Iave = (Ia + Ic) / 2; //;平均值
    }

    if(Uave != 0)
    {
        (Ua >=Uave) ? (Ua -= Uave):(Ua = Uave - Ua); //;a相与平均值的差
        if(PnP->F25.Type == 2)
        {
            (Ub >= Uave)?(Ub -= Uave):(Ub = Uave - Ub);//;b相与平均值的差
           
        }
        (Uc >= Uave)?(Uc -= Uave):(Uc = Uave - Uc);//;c相与平均值的差
        
        if(PnP->F25.Type == 2)
        {
            if(Ua < Ub)
            {
                Ua = Ub;
            }
        }
        if(Ua < Uc)
        {
            Ua = Uc; //;把最大的差写到Ua
        }
        
        temp = Ua * 1000; //;乘100得出百分数，再乘10
        Upn = temp / Uave; //;求出最大不平衡值
        
        if(Upn > UImb) 
        {
            if(ERCPn->ERC17.Status_N & 0x01)
            {
                (*((u16*)PnD->DF28.VImbMin))++;  //;电压不平衡时间统计
                (*((u16*)PnD->DF36.VImbMin))++;
	             if( BCD2ToU16(PnD->DF28.UImbV) < Upn)
	             	{
		          U16To2BCD(Upn, PnD->DF28.UImbV);    //;记录发生时的日最值及时间by zrt 20130917
		          PnD->DF28.UImbT[0]=TBCD.Min;
		          PnD->DF28.UImbT[1]=TBCD.Hour;
		          PnD->DF28.UImbT[2]=TBCD.Day;
	             	}
	             if( BCD2ToU16(PnD->DF36.UImbV) < Upn)
	             	{
		          U16To2BCD(Upn, PnD->DF36.UImbV);     //;记录发生时的月最值及时间by zrt 20130917
		          PnD->DF36.UImbT[0]=TBCD.Min;
		          PnD->DF36.UImbT[1]=TBCD.Hour;
		          PnD->DF36.UImbT[2]=TBCD.Day;
			   PnD->DF36.UImbT[3]=TBCD.Month;
	             	}				
            }
            else
            {
                UImbDLYMinS++;
                if(UImbDLYMinS >= PnP->F26.UImb_DLY)
                {
                    UImbDLYMinS = 0x00;
                    ERCPn->ERC17.Status_N |= 0x01; //;电压不平衡发生
                    ERCPn->ERC17.Upn = Upn;				 
                }
            }            
        }
        else if(Upn < UImb_End)
        {
            if(ERCPn->ERC17.Status_N & 0x01)
            {
                UImbDLYMinE++;
                if(UImbDLYMinE >= PnP->F26.UImb_DLY)
                {
                    UImbDLYMinE = 0x00;
                    ERCPn->ERC17.Status_N &= (~0x01);//;不平衡状态恢复
                    ERCPn->ERC17.Upn = Upn;
                }
            }
        }
        else
        {
            if(ERCPn->ERC17.Status_N & 0x01)
            {
                (*((u16*)PnD->DF28.VImbMin))++;  //;电压不平衡时间统计
                (*((u16*)PnD->DF36.VImbMin))++;
            }
        }
    }
    else
    {
        ERCPn->ERC17.Status_N &= (~0x01);//;不平衡状态恢复
    }

    if(Iave != 0)
    {
        if (Ia >= Iave)
        {
            Ia -= Iave;
        }
        else
        {
            Ia = Iave - Ia;  //;a相与平均值的差
        }
        if (PnP->F25.Type == 2)
        {
            if (Ib >= Iave)
            {
                Ib -= Iave;
            }
            else
            {
                Ib = Iave - Ib; //;b相与平均值的差
            }
        }
        if (Ic >= Iave)
        {
            Ic -= Iave;
        }
        else
        {
            Ic = Iave - Ic;  //;c相与平均值的差
        }
        if (PnP->F25.Type == 2)
        {
            if (Ia < Ib)
            {
                Ia = Ib;
            }
        }
        if (Ia < Ic)
        {
            Ia = Ic;  //;把最大的差写到Ia
        }
       
        temp = Ia * 1000;
        Ipn = temp / Iave; //;求出最大不平衡值
        if(Ipn > IImb) //;不平衡超过开始阀值
        {
            if(ERCPn->ERC17.Status_N & 0x02)
            {
                (*((u16*)PnD->DF28.IImbMin))++;  //;电流不平衡时间统计
                (*((u16*)PnD->DF36.IImbMin))++;
	             if( BCD2ToU16(PnD->DF28.IImbV) < Ipn)
	             	{
		          U16To2BCD(Ipn, PnD->DF28.IImbV);    //;记录发生时的日最值及时间by zrt 20130917
		          PnD->DF28.IImbT[0]=TBCD.Min;
		          PnD->DF28.IImbT[1]=TBCD.Hour;
		          PnD->DF28.IImbT[2]=TBCD.Day;
	             	}
	             if( BCD2ToU16(PnD->DF36.IImbV) < Ipn)
	             	{
		          U16To2BCD(Ipn, PnD->DF36.IImbV);    //;记录发生时的月最值及时间by zrt 20130917
		          PnD->DF36.IImbT[0]=TBCD.Min;
		          PnD->DF36.IImbT[1]=TBCD.Hour;
		          PnD->DF36.IImbT[2]=TBCD.Day;
			   PnD->DF36.IImbT[3]=TBCD.Month;
	             	}					
            }
            else
            {
                IImbDLYMinS++;
                if(IImbDLYMinS  >= PnP->F26.IImb_DLY)
                {
                    IImbDLYMinS = 0x00;
                    ERCPn->ERC17.Status_N |= 0x02; //;电流不平衡状态发生
                    ERCPn->ERC17.Ipn = Ipn;				
                }
            }   
        }
        else if(Ipn < IImb_End)
        {
            if(ERCPn->ERC17.Status_N & 0x02)
            {
                IImbDLYMinE++;
                if(IImbDLYMinE  >= PnP->F26.IImb_DLY)
                {
                    IImbDLYMinE = 0x00;
                    ERCPn->ERC17.Status_N &= (~0x02); //;电流不平衡状态结束
                    ERCPn->ERC17.Ipn = Ipn;
                }
            }
        }
        else
        {
            if(ERCPn->ERC17.Status_N & 0x02)
            {
                (*((u16*)PnD->DF28.IImbMin))++;  //;电流不平衡时间统计
                (*((u16*)PnD->DF36.IImbMin))++;
            }
        }
    }
}
*/

void IMBStatData(struct PoPStr  *PnP, struct PointDataStr *PnD, struct ERCPnData *ERCPn)
{
    //;UImb是电压不平衡限值， Upn是电压当前不平衡度
/*    u16 Ua, Ub = 0, Uc, Uave, UImb, Upn = 0, UImb_End, Ipn = 0; 
    u32 Ia, Ib = 0, Ic, Iave, IImb , IImb_End;
    u32 temp;
    s16 rate;
*/
    u16 Ua, Ub = 0, Uc, Uave, Upn = 0, Ipn = 0; 
    u32 Ia, Ib = 0, Ic, Iave ;
    u32 temp,temp_max,temp_min;
    s16 rate,UImb,IImb, UImb_End, IImb_End;	

    Ua = BCD2ToU16(PnD->RB611);
    Uc = BCD2ToU16(PnD->RB613);
    Ia = BCD3ToS32(PnD->RB621);
    Ic = BCD3ToS32(PnD->RB623);

    UImb = BCD2ToS16(PnP->F26.UImb); //;电压不平衡限值 (%) 放大了10倍
    IImb = BCD2ToS16(PnP->F26.IImb); //;电流不平衡限值 (%)	

    rate = BCD2ToS16(PnP->F26.UImb_Ratio);
    if(rate < 0 ) rate = rate + 1000 ;
    UImb_End =UImb * rate  / 1000;
    rate = BCD2ToS16(PnP->F26.IImb_Ratio);
    if(rate < 0 ) rate = rate + 1000 ;
    IImb_End =IImb * rate  / 1000;	

/*   
    Ua = BCD2ToU16(PnD->RB611);
    Uc = BCD2ToU16(PnD->RB613);
    Ia = BCD3ToU32(PnD->RB621);
    Ic = BCD3ToU32(PnD->RB623);

    UImb = BCD2ToU16(PnP->F26.UImb); //;电压不平衡限值 (%) 放大了10倍
    IImb = BCD2ToU16(PnP->F26.IImb); //;电流不平衡限值 (%)
    
    UImb_End = BCD2ToU16(PnP->F26.UImb_Ratio) * UImb / 1000;
    IImb_End = BCD2ToU16(PnP->F26.IImb_Ratio) * IImb / 1000;
*/
/*
    rate = BCD2ToS16(PnP->F26.UImb_Ratio);
    rate = rate + 1000 ;
    UImb_End =UImb * rate  / 1000;
    rate = BCD2ToS16(PnP->F26.IImb_Ratio);
    rate = rate + 1000 ;
    IImb_End =IImb * rate  / 1000;
*/

    if(PnP->F25.Type == 2)
    {
        Ub = BCD2ToU16(PnD->RB612);
        Ib = BCD3ToS32(PnD->RB622);
        Uave = (Ua + Ub + Uc) / 3;
        Iave = (Ia + Ib + Ic) / 3; //;平均值
    }
    else if(PnP->F25.Type == 1)
    {
        Uave = (Ua + Uc) / 2;
        Iave = (Ia + Ic) / 2; //;平均值
    }

    if(Uave != 0)
    {
 /*   不是用平均值算最大不平衡值by zrt 20130924
        (Ua >=Uave) ? (Ua -= Uave):(Ua = Uave - Ua); //;a相与平均值的差
        if(PnP->F25.Type == 2)
        {
            (Ub >= Uave)?(Ub -= Uave):(Ub = Uave - Ub);//;b相与平均值的差
           
        }
        (Uc >= Uave)?(Uc -= Uave):(Uc = Uave - Uc);//;c相与平均值的差
  
        if(PnP->F25.Type == 2)
        {
            if(Ua < Ub)
            {
                Ua = Ub;
            }
        }
        if(Ua < Uc)
        {
            Ua = Uc; //;把最大的差写到Ua
        }
        temp = Ua * 1000; //;乘100得出百分数，再乘10
        Upn = temp / Uave; //;求出最大不平衡值        
*/
        if(PnP->F25.Type == 2)
        {
	     temp_max = (Ub >= Uc) ?Ub:Uc;
	     temp_max = (temp_max >= Ua)? temp_max:Ua;
	     temp_min = (Ub <= Uc) ?Ub:Uc;
	     temp_min = (temp_min <= Ua)? temp_min:Ua;
        }
        else if(PnP->F25.Type == 1)
        {
	     temp_max = (Ua >= Uc) ?Ua:Uc;
	     temp_min = (Ua <= Uc) ?Ua:Uc;
        }
        temp = (temp_max - temp_min)*1000;
        Upn = temp /temp_max;                         //;求出最大不平衡值by zrt 20130924
        
        if(Upn > UImb) 
        {
            if(ERCPn->ERC17.Status_N & 0x01)
            {
                (*((u16*)PnD->DF28.VImbMin))++;  //;电压不平衡时间统计
                (*((u16*)PnD->DF36.VImbMin))++;
	             if( BCD2ToU16(PnD->DF28.UImbV) < Upn)
	             	{
		          U16To2BCD(Upn, PnD->DF28.UImbV);    //;记录发生时的日最值及时间by zrt 20130917
		          PnD->DF28.UImbT[0]=TBCD.Min;
		          PnD->DF28.UImbT[1]=TBCD.Hour;
		          PnD->DF28.UImbT[2]=TBCD.Day;
	             	}
	             if( BCD2ToU16(PnD->DF36.UImbV) < Upn)
	             	{
		          U16To2BCD(Upn, PnD->DF36.UImbV);     //;记录发生时的月最值及时间by zrt 20130917
		          PnD->DF36.UImbT[0]=TBCD.Min;
		          PnD->DF36.UImbT[1]=TBCD.Hour;
		          PnD->DF36.UImbT[2]=TBCD.Day;
			   PnD->DF36.UImbT[3]=TBCD.Month;
	             	}				
            }
            else
            {
                UImbDLYMinS++;
                if(UImbDLYMinS >= PnP->F26.UImb_DLY)
                {
                    UImbDLYMinS = 0x00;
                    ERCPn->ERC17.Status_N |= 0x01; //;电压不平衡发生
                    ERCPn->ERC17.Upn = Upn;				 
                }
            }            
        }
        else if(Upn < UImb_End)
        {
            if(ERCPn->ERC17.Status_N & 0x01)
            {
                UImbDLYMinE++;
                if(UImbDLYMinE >= PnP->F26.UImb_DLY)
                {
                    UImbDLYMinE = 0x00;
                    ERCPn->ERC17.Status_N &= (~0x01);//;不平衡状态恢复
                    ERCPn->ERC17.Upn = Upn;
                }
            }
        }
        else
        {
            if(ERCPn->ERC17.Status_N & 0x01)
            {
                (*((u16*)PnD->DF28.VImbMin))++;  //;电压不平衡时间统计
                (*((u16*)PnD->DF36.VImbMin))++;
            }
        }
    }
    else
    {
        ERCPn->ERC17.Status_N &= (~0x01);//;不平衡状态恢复
    }

    if(Iave != 0)
    {

/*
        if (Ia >= Iave)
        {
            Ia -= Iave;
        }
        else
        {
            Ia = Iave - Ia;  //;a相与平均值的差
        }
        if (PnP->F25.Type == 2)
        {
            if (Ib >= Iave)
            {
                Ib -= Iave;
            }
            else
            {
                Ib = Iave - Ib; //;b相与平均值的差
            }
        }
        if (Ic >= Iave)
        {
            Ic -= Iave;
        }
        else
        {
            Ic = Iave - Ic;  //;c相与平均值的差
        }
        if (PnP->F25.Type == 2)
        {
            if (Ia < Ib)
            {
                Ia = Ib;
            }
        }
        if (Ia < Ic)
        {
            Ia = Ic;  //;把最大的差写到Ia
        }
       
        temp = Ia * 1000;
        Ipn = temp / Iave; //;求出最大不平衡值
*/

        if(PnP->F25.Type == 2)
        {
	     temp_max = (Ib >= Ic) ?Ib:Ic;
	     temp_max = (temp_max >= Ia)? temp_max:Ia;
	     temp_min = (Ib <= Ic) ?Ib:Ic;
	     temp_min = (temp_min <= Ia)? temp_min:Ia;
        }
        else if(PnP->F25.Type == 1)
        {
	     temp_max = (Ia>= Ic) ?Ia:Ic;
	     temp_min = (Ia <= Ic) ?Ia:Ic;
        }
        temp = (temp_max - temp_min)*1000;
        Ipn = temp /temp_max;                      //;求出最大不平衡值by zrt 20130924
		
        if(Ipn > IImb) //;不平衡超过开始阀值
        {
            if(ERCPn->ERC17.Status_N & 0x02)
            {
                (*((u16*)PnD->DF28.IImbMin))++;  //;电流不平衡时间统计
                (*((u16*)PnD->DF36.IImbMin))++;
	             if( BCD2ToU16(PnD->DF28.IImbV) < Ipn)
	             	{
		          U16To2BCD(Ipn, PnD->DF28.IImbV);    //;记录发生时的日最值及时间by zrt 20130917
		          PnD->DF28.IImbT[0]=TBCD.Min;
		          PnD->DF28.IImbT[1]=TBCD.Hour;
		          PnD->DF28.IImbT[2]=TBCD.Day;
	             	}
	             if( BCD2ToU16(PnD->DF36.IImbV) < Ipn)
	             	{
		          U16To2BCD(Ipn, PnD->DF36.IImbV);    //;记录发生时的月最值及时间by zrt 20130917
		          PnD->DF36.IImbT[0]=TBCD.Min;
		          PnD->DF36.IImbT[1]=TBCD.Hour;
		          PnD->DF36.IImbT[2]=TBCD.Day;
			   PnD->DF36.IImbT[3]=TBCD.Month;
	             	}					
            }
            else
            {
                IImbDLYMinS++;
                if(IImbDLYMinS  >= PnP->F26.IImb_DLY)
                {
                    IImbDLYMinS = 0x00;
                    ERCPn->ERC17.Status_N |= 0x02; //;电流不平衡状态发生
                    ERCPn->ERC17.Ipn = Ipn;				
                }
            }   
        }
        else if(Ipn < IImb_End)
        {
            if(ERCPn->ERC17.Status_N & 0x02)
            {
                IImbDLYMinE++;
                if(IImbDLYMinE  >= PnP->F26.IImb_DLY)
                {
                    IImbDLYMinE = 0x00;
                    ERCPn->ERC17.Status_N &= (~0x02); //;电流不平衡状态结束
                    ERCPn->ERC17.Ipn = Ipn;
                }
            }
        }
        else
        {
            if(ERCPn->ERC17.Status_N & 0x02)
            {
                (*((u16*)PnD->DF28.IImbMin))++;  //;电流不平衡时间统计
                (*((u16*)PnD->DF36.IImbMin))++;
            }
        }
    }
}

/* 
********************************************************************************
* 函 数 名：ZDPN_YXT
* 功    能：电压，电流，电流不平衡，视在功率越限当日累计时间与当月累计时间
* 参    数：
* 返    回： 
* 说    明：每分钟统计一次
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void ZDPN_YXT(void)
{
    u8 i;
    u32 *Addr32;

    struct PoPStr  *PnP;
    struct PointDataStr *PnD;
    struct  ERCPnData  *ERCPn;

    for(i=0; i<PNMAXNUM; i++)
    {
        Addr32 = (u32*)(*(PnDTAddr + i));
        PnD = (struct PointDataStr*)(*(Addr32 + _PnD));
        PnP = (struct PoPStr*)(*(Addr32 + _PnP));
        ERCPn = (struct ERCPnData*)(*(Addr32 + _ERCPn));

        /*当前数据存在时才开始累计越限时间*/
        if((PnD->CB_Time[0]!=0) || (PnD->CB_Time[1]!=0) || (PnD->CB_Time[2]!=0) ||
            (PnD->CB_Time[3]!=0) || (PnD->CB_Time[4]!=0))
        {          
            ERCPn->Pn = *((u16*)ZDP.F10.Meter[i].Point);
            ERCPn->OTime[0] = TBCD.Min;   
            ERCPn->OTime[1] = TBCD.Hour;  
            ERCPn->OTime[2] = TBCD.Day;   
            ERCPn->OTime[3] = TBCD.Month; 
            ERCPn->OTime[4] = TBCD.Year;  
            VolStatData(PnP, PnD, ERCPn);
            CurStatData(PnP, PnD, ERCPn);
            SStatData(PnP, PnD, ERCPn);
            IMBStatData(PnP, PnD, ERCPn); 
            ERC17_Judge(PnD, ERCPn);    //;压流不平衡越限事件   
            ERC24_Judge(PnD, ERCPn);    //;电压越限记录
            ERC25_Judge(PnD, ERCPn);    //;电流越限记录
            ERC26_Judge(PnP, PnD, ERCPn);//;视在功率越限记录
            ERC10_Judge(PnP, PnD, ERCPn);//;电压回路异常记录

        }
    }
}


/*用于比较电压，统计出最大值与最小值，并记录发生的相应时间*/
void MXT_CAL_U(u8 *NowVal, u8 *MaxVal, u8 *MinVal, u8 *MaxT, u8 *MinT)
{
    u16 Now, Max, Min;

    Now = BCD2ToU16(NowVal);
    Max = BCD2ToU16(MaxVal);
    Min = BCD2ToU16(MinVal);

    if (Now != 0)       /*当前电压不为零时才开始判断*/
    {
        if (Now > Max)          /*判断是不是最大值*/
        {
            U16To2BCD(Now, MaxVal);
            *(MaxT + 0) = TBCD.Min;
            *(MaxT + 1) = TBCD.Hour;
            *(MaxT + 2) = TBCD.Day;
        }
        else                       /*如果不是最大值，再判断是不是最小值*/
        {
            if (Min != 0)
            {
                if (Now < Min)                    /*最小值存在时与最小值比较*/
                {
                    U16To2BCD(Now, MinVal);
                    *(MinT + 0) = TBCD.Min;
                    *(MinT + 1) = TBCD.Hour;
                    *(MinT + 2) = TBCD.Day;
                }
            }
            else                               /*最小值不存在时将当前值作为最小值*/
            {
                U16To2BCD(Now, MinVal);
                *(MinT + 0) = TBCD.Min;
                *(MinT + 1) = TBCD.Hour;
                *(MinT + 2) = TBCD.Day;
            }
        }
    }
}


/*用于比较电流，统计出最大值，并记录发生的相应时间*/
void MXT_CAL_I(u8 *NowVal, u8 *MaxVal, u8 *MaxT)
{
    u8 pow[3];
    u32 Now,Max;

    pow[0] = *(NowVal + 0);
    pow[1] = *(NowVal + 1);
    pow[2] = *(NowVal + 2);
    pow[2] &= 0x7F;                 //;去掉符号by zrt 20131022
    Now = BCD3ToU32(pow);
    pow[0] = *(MaxVal + 0);
    pow[1] = *(MaxVal + 1);
    pow[2] = *(MaxVal + 2);
    pow[2] &= 0x7F;	
    Max = BCD3ToU32(pow);

    if(Now > Max)
    {
        U32To3BCD(Now, MaxVal);
        *(MaxT + 0) = TBCD.Min;
        *(MaxT + 1) = TBCD.Hour;
        *(MaxT + 2) = TBCD.Day;
    }
}



/*用于比较功率绝对值大小*/
void MXT_CAL_P(u8 *NowVal, u8 *MaxVal, u8 *MaxT, u16 *Pow0_T)
{
    u8 pow[3];
    u32 Now1,Max1;

    pow[0] = *(NowVal + 0);
    pow[1] = *(NowVal + 1);
    pow[2] = *(NowVal + 2);
    pow[2] &= 0x7F;                 //;去掉符号位
    Now1 = BCD3ToU32(pow);
    pow[0] = *(MaxVal + 0);
    pow[1] = *(MaxVal + 1);
    pow[2] = *(MaxVal + 2);
    pow[2] &= 0x7F;
    Max1 = BCD3ToU32(pow);

    if (Now1 > Max1)
    {
        U32To3BCD(Now1, MaxVal);
        *(MaxT + 0) = TBCD.Min;
        *(MaxT + 1) = TBCD.Hour;
        *(MaxT + 2) = TBCD.Day;
    }
    if (!Now1)
    {
        (*Pow0_T)++;
    }
}


/*----------------------------------------------------------------------------*
 |  每分钟计算一次当日当月电压最值与发生时间
 *----------------------------------------------------------------------------*/
void ZDPN_MXT(void)
{
    u8 i;
    u16 AAver,BAver,CAver;
    u32 *Addr32;
    struct PointDataStr *PnD;

    for(i = 0; i < PNMAXNUM; i++)
    {
        Addr32 = (u32*)(*(PnDTAddr + i));
        PnD = (struct PointDataStr*)(*(Addr32 + _PnD));

        /*当前数据存在时才开始算最值与发生时间*/
        if ((PnD->CB_Time[0] != 0) || (PnD->CB_Time[1] != 0) || (PnD->CB_Time[2] != 0) ||
            (PnD->CB_Time[3] != 0) || (PnD->CB_Time[4] != 0))
        {
            /*当日电压最值与发生时间*/
            MXT_CAL_U(PnD->RB611, PnD->DF27.AVMax, PnD->DF27.AVMini, PnD->DF27.AVMax_T, PnD->DF27.AVMini_T);
            MXT_CAL_U(PnD->RB612, PnD->DF27.BVMax, PnD->DF27.BVMini, PnD->DF27.BVMax_T, PnD->DF27.BVMini_T);
            MXT_CAL_U(PnD->RB613, PnD->DF27.CVMax, PnD->DF27.CVMini, PnD->DF27.CVMax_T, PnD->DF27.CVMini_T);

            /*当日平均电压*/
            AAver = (BCD2ToU16(PnD->DF27.AVMax) + BCD2ToU16(PnD->DF27.AVMini)) / 2;
            U16To2BCD(AAver, PnD->DF27.AAver);
            BAver = (BCD2ToU16(PnD->DF27.BVMax) + BCD2ToU16(PnD->DF27.BVMini)) / 2;
            U16To2BCD(BAver, PnD->DF27.BAver);
            CAver = (BCD2ToU16(PnD->DF27.CVMax) + BCD2ToU16(PnD->DF27.CVMini)) / 2;
            U16To2BCD(CAver, PnD->DF27.CAver);

            /*当月电压最值与发生时间*/
            MXT_CAL_U(PnD->RB611, PnD->DF35.AVMax, PnD->DF35.AVMini, PnD->DF35.AVMax_T, PnD->DF35.AVMini_T);
            MXT_CAL_U(PnD->RB612, PnD->DF35.BVMax, PnD->DF35.BVMini, PnD->DF35.BVMax_T, PnD->DF35.BVMini_T);
            MXT_CAL_U(PnD->RB613, PnD->DF35.CVMax, PnD->DF35.CVMini, PnD->DF35.CVMax_T, PnD->DF35.CVMini_T);
            /*当月平均电压*/
            AAver = (BCD2ToU16(PnD->DF35.AVMax) + BCD2ToU16(PnD->DF35.AVMini)) / 2;
            U16To2BCD(AAver, PnD->DF35.AAver);
            BAver = (BCD2ToU16(PnD->DF35.BVMax) + BCD2ToU16(PnD->DF35.BVMini)) / 2;
//            U16To2BCD(BAver, PnD->DF27.BAver);  by zrt 20130916
            U16To2BCD(BAver, PnD->DF35.BAver);			
            CAver = (BCD2ToU16(PnD->DF35.CVMax) + BCD2ToU16(PnD->DF35.CVMini)) / 2;
            U16To2BCD(CAver, PnD->DF35.CAver);

            /*当日电流最值与发生时间*/
            MXT_CAL_I(PnD->RB621, PnD->DF29.AIMax, PnD->DF29.AIMax_T);
            MXT_CAL_I(PnD->RB622, PnD->DF29.BIMax, PnD->DF29.BIMax_T);
            MXT_CAL_I(PnD->RB623, PnD->DF29.CIMax, PnD->DF29.CIMax_T);
            MXT_CAL_I(PnD->RB6E5, PnD->DF29.I0Max, PnD->DF29.I0Max_T);
            /*当月电流最值与发生时间*/
            MXT_CAL_I(PnD->RB621, PnD->DF37.AIMax, PnD->DF37.AIMax_T);
            MXT_CAL_I(PnD->RB622, PnD->DF37.BIMax, PnD->DF37.BIMax_T);
            MXT_CAL_I(PnD->RB623, PnD->DF37.CIMax, PnD->DF37.CIMax_T);
            MXT_CAL_I(PnD->RB6E5, PnD->DF37.I0Max, PnD->DF37.I0Max_T);

            /*当日功率最值与发生时间*/
            MXT_CAL_P(PnD->RB630, PnD->DF25.ZPMax, PnD->DF25.ZPMax_T, (u16*)PnD->DF25.ZP0Min);
            MXT_CAL_P(PnD->RB631, PnD->DF25.APMax, PnD->DF25.APMax_T, (u16*)PnD->DF25.AP0Min);
            MXT_CAL_P(PnD->RB632, PnD->DF25.BPMax, PnD->DF25.BPMax_T, (u16*)PnD->DF25.BP0Min);
            MXT_CAL_P(PnD->RB633, PnD->DF25.CPMax, PnD->DF25.CPMax_T, (u16*)PnD->DF25.CP0Min);
            /*当月功率最值与发生时间*/
            MXT_CAL_P(PnD->RB630, PnD->DF33.ZPMax, PnD->DF33.ZPMax_T,(u16*)PnD->DF33.ZP0Min);
            MXT_CAL_P(PnD->RB631, PnD->DF33.APMax, PnD->DF33.APMax_T,(u16*)PnD->DF33.AP0Min);
            MXT_CAL_P(PnD->RB632, PnD->DF33.BPMax, PnD->DF33.BPMax_T,(u16*)PnD->DF33.BP0Min);
            MXT_CAL_P(PnD->RB633, PnD->DF33.CPMax, PnD->DF33.CPMax_T,(u16*)PnD->DF33.CP0Min);
        }
    }
}

