/*
********************************************************************************
*  文 件 名：GDW376_ERC.C
*
*  文件描述：GDW376.1规定的异常事件的记录预处理
*
*  所用芯片：
*
*  创 建 人：
*
*  版 本 号：
*
*  修改记录：1.如果主动上送未成功，事件又一次发生，则不发送上一次事件
********************************************************************************
*/
#include "GDW376_CFG.h"
#include "GDW376_ERC_SMS.h"
#include "GDW376_MMU.h"

u8 UALostTMin,UBLostTMin,UCLostTMin;


 /* 
********************************************************************************
* 函 数 名：ERC1 数据初始化和版本变更记录
* 功    能：
* 参    数：Type = 1 参数及数据区初始化
*           Type = 2 终端版本变更
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void ERC1_Judge(u8 Type)
{
    ERCP0.ERC1.Status[0] = 0x01;
    
    ERCP0.ERC1.OTime[0] = TBCD.Min;
    ERCP0.ERC1.OTime[1] = TBCD.Hour;
    ERCP0.ERC1.OTime[2] = TBCD.Day;
    ERCP0.ERC1.OTime[3] = TBCD.Month;
    ERCP0.ERC1.OTime[4] = TBCD.Year;
    
    ERCP0.ERC1.Mark[0] = Type;

    ERCP0.ERC1.Data[0] = SofewareOldVer[0];
    ERCP0.ERC1.Data[1] = SofewareOldVer[1];
    ERCP0.ERC1.Data[2] = SofewareOldVer[2];
    ERCP0.ERC1.Data[3] = SofewareOldVer[3];

    ERCP0.ERC1.Data[4] = ZDCFG_F1[12];   //;终端软件版本号
    ERCP0.ERC1.Data[5] = ZDCFG_F1[13];
    ERCP0.ERC1.Data[6] = ZDCFG_F1[14];
    ERCP0.ERC1.Data[7] = ZDCFG_F1[15];
}


 /* 
********************************************************************************
* 函 数 名：ERC17_Judge
* 功    能：根据瞬时数据判断是否产生电压，电流不平衡度越限事件ERC17
* 参    数：
* 返    回： 
* 说    明
* 抛出异常：
* 作    者：
* 修改记录：2011-09-26 09:26:45
********************************************************************************
*/
void ERC17_Judge(struct PointDataStr *PnD, struct ERCPnData *ERCPn)
{   
    u16 Upn, Ipn;
    
    
    ERCPn->ERC17.BPH_OM = 0; //;先清除异常标志
    ERCPn->ERC17.BPH_RM = 0;

    if(((ERCPn->ERC17.Status_O & 0x01) != 0x01) && 
        ((ERCPn->ERC17.Status_N & 0x01) == 0x01))
    {
        ERCPn->ERC17.BPH_OM |= 0x01;  //;电压不平衡发生
    }
    else if(((ERCPn->ERC17.Status_O & 0x01) == 0x01) && 
            ((ERCPn->ERC17.Status_N & 0x01) != 0x01))
    {
        ERCPn->ERC17.BPH_RM |= 0x01; //;不平衡结束
    }
    
    if(((ERCPn->ERC17.Status_O & 0x02) != 0x02) && 
       ((ERCPn->ERC17.Status_N & 0x02) == 0x02))
    {
        ERCPn->ERC17.BPH_OM |= 0x02; //;电流不平衡开始
    }
    else if(((ERCPn->ERC17.Status_O & 0x02) == 0x02) && 
            ((ERCPn->ERC17.Status_N & 0x02) != 0x02))
    {
        ERCPn->ERC17.BPH_RM |= 0x02; //;结束
    }
    
    if((ERCPn->ERC17.BPH_OM) || (ERCPn->ERC17.BPH_RM))
    {
        if (ERCPn->ERC17.BPH_OM)
        {
            ERCPn->ERC17.Status[0] = 0x01;       //;不平衡事件发生
        }
        
        if (ERCPn->ERC17.BPH_RM)
        {
            ERCPn->ERC17.Status[1] = 0x01;       //;不平衡事件恢复
        }
        
        Upn = ERCPn->ERC17.Upn;
        Ipn = ERCPn->ERC17.Ipn;
        ERCPn->ERC17.Data[1]  = (Upn / 1000) *16+((Upn % 1000) / 100);
        Upn %= 100;
        ERCPn->ERC17.Data[0] = (Upn / 10) *16+(Upn % 10); //;电压不平衡度
        ERCPn->ERC17.Data[3]  = (Ipn / 1000) *16+((Ipn % 1000) / 100);
        Ipn %= 100;
        ERCPn->ERC17.Data[2]  = (Ipn / 10) *16+(Ipn % 10); //;电流不平衡度
        ERCPn->ERC17.Data[4]  = PnD->RB611[0];//;Ua
        ERCPn->ERC17.Data[5]  = PnD->RB611[1]; 
        ERCPn->ERC17.Data[6]  = PnD->RB612[0];//;Ub
        ERCPn->ERC17.Data[7]  = PnD->RB612[1]; 
        ERCPn->ERC17.Data[8]  = PnD->RB613[0];//;Uc
        ERCPn->ERC17.Data[9]  = PnD->RB613[1]; 
        ERCPn->ERC17.Data[10] = PnD->RB621[0]; //;Ia
        ERCPn->ERC17.Data[11] = PnD->RB621[1]; 
        ERCPn->ERC17.Data[12] = PnD->RB621[2];
        ERCPn->ERC17.Data[13] = PnD->RB622[0]; //;Ib
        ERCPn->ERC17.Data[14] = PnD->RB622[1]; 
        ERCPn->ERC17.Data[15] = PnD->RB622[2]; 
        ERCPn->ERC17.Data[16] = PnD->RB623[0]; //;Ic
        ERCPn->ERC17.Data[17] = PnD->RB623[1]; 
        ERCPn->ERC17.Data[18] = PnD->RB623[2];
    }
    
    ERCPn->ERC17.Status_O = ERCPn->ERC17.Status_N;
}



/*----------------------------------------------------------------------------*
 |  ERC21-4: 终端故障记录
 *----------------------------------------------------------------------------*/
void ERC21_4Judge(u8 Occur)
{     
    if (Occur == 1)
    {
        if ((Port1.PnErrNum + Port2.PnErrNum) == PNMAXNUM)
        {
            if (ERCP0.ERC21.State != 0x01)
            { 
                ERCP0.ERC21.State = 0x01;
                ERCP0.ERC21.Status[0] = 0x01; //;终端故障
                ERCP0.ERC21.OTime[0] = TBCD.Min;
                ERCP0.ERC21.OTime[1] = TBCD.Hour;
                ERCP0.ERC21.OTime[2] = TBCD.Day;
                ERCP0.ERC21.OTime[3] = TBCD.Month;
                ERCP0.ERC21.OTime[4] = TBCD.Year;
                ERCP0.ERC21.Data[0]  = 4;
            }
        }
    }
    else if (Occur == 2)
    {
        ERCP0.ERC21.State = 0;  
    }
}


/* 
********************************************************************************
* 函 数 名：ERC24_Judge
* 功    能：电压越限事件
* 参    数：
* 返    回： 
* 说    明：u16 PnSt->DYYX_N : xxxx xxxx xxxx xxxx
*           0~3  bit  表示：A相越上上限，上限， 下下限，下限
*           4~7  bit  表示：B相越上上限，上限， 下下限，下限
*           8~11 bit  表示：C相越上上限，上限， 下下限，下限
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void ERC24_Judge(struct PointDataStr *PnD, struct ERCPnData *ERCPn)
{
    
    ERCPn->ERC24.YSSXOM = 0;
    ERCPn->ERC24.YSSXRM = 0;
    ERCPn->ERC24.YXXXOM = 0;
    ERCPn->ERC24.YXXXRM = 0;
    
    /*------------------- 越限事件发生与恢复 ------------------------------*/
    if(((ERCPn->ERC24.Status_O & 0x01) != 0x01) && ((ERCPn->ERC24.Status_N  & 0x01) == 0x01))
    {
        ERCPn->ERC24.YSSXOM |= 0x01; //;A相电压越上上限发生
    }
    else if(((ERCPn->ERC24.Status_O & 0x01) == 0x01) && ((ERCPn->ERC24.Status_N  & 0x01) != 0x01))
    {
        ERCPn->ERC24.YSSXRM |= 0x01; //;A相电压越上上限恢复
    }
    if(((ERCPn->ERC24.Status_O & 0x02) != 0x02) && ((ERCPn->ERC24.Status_N  & 0x02) == 0x02))
    {
        ERCPn->ERC24.YXXXOM |= 0x01;   //;A相电压越下下限发生
    }
    else if(((ERCPn->ERC24.Status_O & 0x02) == 0x02) && ((ERCPn->ERC24.Status_N & 0x02) != 0x02))
    {
        ERCPn->ERC24.YXXXRM |= 0x01;   //;A相电压越下下限恢复
    }


    if(((ERCPn->ERC24.Status_O & 0x04) != 0x04) && ((ERCPn->ERC24.Status_N & 0x04) == 0x04))
    {
        ERCPn->ERC24.YSSXOM |= 0x02; //;B相电压越上上限发生
    }
    else if(((ERCPn->ERC24.Status_O &0x04) == 0x04) && ((ERCPn->ERC24.Status_N &0x04) != 0x04))
    {
        ERCPn->ERC24.YSSXRM |= 0x02; //;B相电压越上上限恢复
    }
    if(((ERCPn->ERC24.Status_O & 0x08) != 0x08) && ((ERCPn->ERC24.Status_N & 0x08) == 0x08))
    {
        ERCPn->ERC24.YXXXOM |= 0x02; //;B相电压越下下限发生
    }
    else if(((ERCPn->ERC24.Status_O & 0x08) == 0x08) && ((ERCPn->ERC24.Status_N & 0x08) != 0x08))
    {
        ERCPn->ERC24.YXXXRM |= 0x02; //;B相电压越下下限恢复
    }

    if(((ERCPn->ERC24.Status_O & 0x10) != 0x10) && ((ERCPn->ERC24.Status_N & 0x10) == 0x10))
    {
        ERCPn->ERC24.YSSXOM |= 0x04; //;C相电压越上上限发生
    }
    else if(((ERCPn->ERC24.Status_O & 0x10) == 0x10) && ((ERCPn->ERC24.Status_N & 0x10) != 0x10))
    {
        ERCPn->ERC24.YSSXRM |= 0x04; //;C相电压越上上限恢复
    }
    if(((ERCPn->ERC24.Status_O & 0x20) != 0x20) && ((ERCPn->ERC24.Status_N & 0x20) == 0x20))
    {
        ERCPn->ERC24.YXXXOM |= 0x04; //;C相电压越下下限发生
    }
    else if(((ERCPn->ERC24.Status_O & 0x20) == 0x20) && ((ERCPn->ERC24.Status_N & 0x20) != 0x20))
    {
        ERCPn->ERC24.YXXXRM |= 0x04;  //;C相电压越下下限恢复
    }
    
    if(ERCPn->ERC24.YSSXOM || ERCPn->ERC24.YSSXRM || ERCPn->ERC24.YXXXOM || ERCPn->ERC24.YXXXRM)
    {
        if (ERCPn->ERC24.YSSXOM)
        {
            ERCPn->ERC24.Status[0] = 0x01;
        }
        if (ERCPn->ERC24.YSSXRM)
        {
            ERCPn->ERC24.Status[1] = 0x01;
        }
        if (ERCPn->ERC24.YXXXOM)
        {
            ERCPn->ERC24.Status[2] = 0x01;
        }
        if (ERCPn->ERC24.YXXXRM)
        {
            ERCPn->ERC24.Status[3] = 0x01;
        }
        ERCPn->ERC24.Data[0] = PnD->RB611[0];
        ERCPn->ERC24.Data[1] = PnD->RB611[1]; //;Ua
        ERCPn->ERC24.Data[2] = PnD->RB612[0];
        ERCPn->ERC24.Data[3] = PnD->RB612[1]; //;Ub
        ERCPn->ERC24.Data[4] = PnD->RB613[0];
        ERCPn->ERC24.Data[5] = PnD->RB613[1]; //;Uc
    }
    
    ERCPn->ERC24.Status_O = ERCPn->ERC24.Status_N;
}


/* 
********************************************************************************
* 函 数 名：ERC25_Judge
* 功    能：电流越限记录
* 参    数：P1P: 测量点参数地址
*           P1D: 测量点数据地址
*           ERCNPn： 测量点Pn中ERC25的分类事件
* 返    回： 
* 说    明：u8 ERCPn->ERC25.Status_N :  xxxx xxxx
*           0，1 bit ：A相越上限，上上限  2，3 bit ：B相越上限，上上限
*           4，5 bit ：C相越上限，上上限  6,   bit : 零序电流越上限
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void ERC25_Judge(struct PointDataStr *PnD, struct ERCPnData *ERCPn)
{

    ERCPn->ERC25.YSSXOM = 0;
    ERCPn->ERC25.YSSXRM = 0;
    ERCPn->ERC25.YSXOM = 0;
    ERCPn->ERC25.YSXRM = 0;
    
    /*--------------------------- 越限事件发生与恢复 ------------------------*/
    if(((ERCPn->ERC25.Status_O & 0x01) != 0x01) && ((ERCPn->ERC25.Status_N & 0x01) == 0x01))
    {
        ERCPn->ERC25.YSSXOM |= 0x01; //;A相电流越上上限发生
    }
    else if(((ERCPn->ERC25.Status_O & 0x01) == 0x01) && ((ERCPn->ERC25.Status_N & 0x01) != 0x01))
    {
        ERCPn->ERC25.YSSXRM |= 0x01;  //;A相电流越上上限恢复
    }

    if(((ERCPn->ERC25.Status_O &0x02) != 0x02) && ((ERCPn->ERC25.Status_N & 0x02) == 0x02))
    {
        ERCPn->ERC25.YSXOM |= 0x01;  //;A相电流越上限发生
    }
    else if(((ERCPn->ERC25.Status_O & 0x02) == 0x02) && ((ERCPn->ERC25.Status_N & 0x02) != 0x02))
    {
        ERCPn->ERC25.YSXRM |= 0x01;  //;A相电流越上限恢复
    }


    if(((ERCPn->ERC25.Status_O & 0x04) != 0x04) && ((ERCPn->ERC25.Status_N & 0x04) == 0x04))
    {
        ERCPn->ERC25.YSSXOM |= 0x02;   //;B相电流越上上限发生
    }
    else if(((ERCPn->ERC25.Status_O & 0x04) == 0x04) && ((ERCPn->ERC25.Status_N & 0x04) != 0x04))
    {
        ERCPn->ERC25.YSSXRM |= 0x02; //;B相电流越上上限恢复
    }
    if(((ERCPn->ERC25.Status_O & 0x08) != 0x08) && ((ERCPn->ERC25.Status_N & 0x08) == 0x08))
    {
        ERCPn->ERC25.YSXOM |= 0x02; //;B相电流越上限发生
    }
    else if(((ERCPn->ERC25.Status_O & 0x08) == 0x08) && ((ERCPn->ERC25.Status_N & 0x08) != 0x08))
    {
        ERCPn->ERC25.YSXRM |= 0x02;  //;B相电流越上限恢复
    }


    if(((ERCPn->ERC25.Status_O & 0x10) != 0x10) && ((ERCPn->ERC25.Status_N & 0x10) == 0x10))
    {
        ERCPn->ERC25.YSSXOM |= 0x04; //;C相电流越上上限发生
    }
    else if(((ERCPn->ERC25.Status_O & 0x10) == 0x10) && ((ERCPn->ERC25.Status_N & 0x10) != 0x10))
    {
        ERCPn->ERC25.YSSXRM |= 0x04; //;C相电流越上上限恢复
    }
    if(((ERCPn->ERC25.Status_O & 0x20) != 0x20) && ((ERCPn->ERC25.Status_N & 0x20) == 0x20))
    {
        ERCPn->ERC25.YSXOM |= 0x04; //;C相电流越上限发生
    }
    else if(((ERCPn->ERC25.Status_O & 0x20) == 0x20) && ((ERCPn->ERC25.Status_N & 0x20) != 0x20))
    {
        ERCPn->ERC25.YSXRM |= 0x04;  //;C相电流越上限恢复
    }

    if((ERCPn->ERC25.YSSXOM || ERCPn->ERC25.YSSXRM || ERCPn->ERC25.YSXOM) || (ERCPn->ERC25.YSXRM))
    {
        if (ERCPn->ERC25.YSSXOM) //;zrt_20130614
        {
            ERCPn->ERC25.Status[0] = 0x01;
        }
        if (ERCPn->ERC25.YSSXRM)
        {
            ERCPn->ERC25.Status[1] = 0x01;
        }
        if (ERCPn->ERC25.YSXOM)
        {
            ERCPn->ERC25.Status[2] = 0x01;
        }
        if (ERCPn->ERC25.YSXRM)
        {
            ERCPn->ERC25.Status[3] = 0x01;
        }  //;修正电流越限不上送的问题
        ERCPn->ERC25.Data[0] = PnD->RB621[0];//;Ia
        ERCPn->ERC25.Data[1] = PnD->RB621[1];
        ERCPn->ERC25.Data[2] = PnD->RB621[2]; 
        ERCPn->ERC25.Data[3] = PnD->RB622[0];//;Ib
        ERCPn->ERC25.Data[4] = PnD->RB622[1]; 
        ERCPn->ERC25.Data[5] = PnD->RB622[2];
        ERCPn->ERC25.Data[6] = PnD->RB623[0];//;Ic
        ERCPn->ERC25.Data[7] = PnD->RB623[1]; 
        ERCPn->ERC25.Data[8] = PnD->RB623[2];
    }
    
    ERCPn->ERC25.Status_O = ERCPn->ERC25.Status_N;
}


/* 
********************************************************************************
* 函 数 名：ERC26_Judge
* 功    能：视在功率越限记录
* 参    数：u8 SYX_N  xxx xxx
*           0 bit ：越上限 1 bit :越上上限
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void ERC26_Judge(struct PoPStr  *PnP, struct PointDataStr *PnD, struct ERCPnData *ERCPn)
{

    if(((ERCPn->ERC26.Status_O & 0x01) != 0x01) && ((ERCPn->ERC26.Status_N & 0x01) == 0x01))
    {
        ERCPn->ERC26.Status[0] |= 0x01; //;越上限发生
    }
    else if(((ERCPn->ERC26.Status_O & 0x01) == 0x01) && ((ERCPn->ERC26.Status_N & 0x01) != 0x01))
    {
        ERCPn->ERC26.Status[1] |= 0x01;  //;越上限恢复
    }
    
    if(((ERCPn->ERC26.Status_O & 0x02) != 0x02) && ((ERCPn->ERC26.Status_N & 0x02) == 0x02))
    {
        ERCPn->ERC26.Status[2] |= 0x01; //;越上上限发生
    }
    else if(((ERCPn->ERC26.Status_O & 0x02) == 0x02) && ((ERCPn->ERC26.Status_N & 0x02) != 0x02))
    {
        ERCPn->ERC26.Status[3] |= 0x01; //;越上上限恢复
    }
    
    if(ERCPn->ERC26.Status[0] || ERCPn->ERC26.Status[1] || ERCPn->ERC26.Status[2] || ERCPn->ERC26.Status[3])
    {
        ERCPn->ERC26.Data[0] = PnD->RB660[0];
        ERCPn->ERC26.Data[1] = PnD->RB660[1];
        ERCPn->ERC26.Data[2] = PnD->RB660[2]; //;发生时的视在功率
        
        if((ERCPn->ERC26.Status[0]) || (ERCPn->ERC26.Status[1]))
        {
            ERCPn->ERC26.Data[3] = PnP->F26.SH[0];
            ERCPn->ERC26.Data[4] = PnP->F26.SH[1];
            ERCPn->ERC26.Data[5] = PnP->F26.SH[2];
        }
        else
        {
            ERCPn->ERC26.Data[3] = PnP->F26.SHH[0];
            ERCPn->ERC26.Data[4] = PnP->F26.SHH[1];
            ERCPn->ERC26.Data[5] = PnP->F26.SHH[2];
        }
    }
    
    ERCPn->ERC26.Status_O = ERCPn->ERC26.Status_N;
}


/* 
********************************************************************************
* 函 数 名：ERC13_Judge
* 功    能：ERC13电表故障信息
* 参    数：
* 返    回： 
* 说    明：停电次数与电池欠压待做(已做by zrt 20130904)
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void ERC13_Judge(struct PointDataStr *PnD, struct ERCPnData *ERCPn)
{
    if(ERCPn->ERC13.Status[0] == 0x01) //;上次结果未处理，不做判断
    {
        return;
    }
    ERCPn->ERC13.Mark[0] = 0;
    
    if((PnD->OB212[0]!=PnD->RB212[0]) || (PnD->OB212[1]!=PnD->RB212[1]) ||
       (PnD->OB212[2]!=PnD->RB212[2]))
    {
        ERCPn->ERC13.Mark[0] |= 0x01; //; 编程次数变更
        MovNByte(3, P1D.OB212, P1D.RB212); //;编程总次数
    }

    if((PnD->OB213[0]!=PnD->RB213[0]) || (PnD->OB213[1]!=PnD->RB213[1]) ||
       (PnD->OB213[2]!=PnD->RB213[2]))
    {
        ERCPn->ERC13.Mark[0] |= 0x01; //; 最大需量清零次数变更
        MovNByte(3, PnD->OB213, PnD->RB213); //;需量清零次数
    }

    if((PnD->OB310[0] != PnD->RB310[0]) || (PnD->OB310[1] != PnD->RB310[1]))
    {
        ERCPn->ERC13.Mark[0] |= 0x02; //; 断相次数变更
        MovNByte(2, PnD->OB310, PnD->RB310); //;断相次数
       
    }

    if((PnD->OB710[0] != PnD->RB710[0]) || (PnD->OB710[1] != PnD->RB710[1]) ||
       (PnD->OB710[2] != PnD->RB710[2]))
    {
        ERCPn->ERC13.Mark[0] |= 0x04; //; 失压次数变更
        PnD->OB710[0] = PnD->RB710[0]; //;失压次数
        PnD->OB710[1] = PnD->RB710[1];
        PnD->OB710[2] = PnD->RB710[2];
    }

    if((PnD->OB222[0] != PnD->RB222[0]) || (PnD->OB222[1] != PnD->RB222[1]) ||
       (PnD->OB222[2] != PnD->RB222[2]))
    {
        ERCPn->ERC13.Mark[0] |= 0x08; //;掉电次数变更
        PnD->OB222[0] = PnD->RB222[0]; //;掉电次数
        PnD->OB222[1] = PnD->RB222[1];
        PnD->OB222[2] = PnD->RB222[2];
    }

    if((PnD->RC027[0])&0x08)
    {
        ERCPn->ERC13.Mark[0] |= 0x10; //;电表电池欠压
    }	

    if(ERCPn->ERC13.Mark[0])
    {
        ERCPn->ERC13.Status[0] = 0x01;  //; 电能表参数变更
    }
}


/* 
********************************************************************************
* 函 数 名：ERC14_Judge
* 功    能：终端停电/上电事件
* 参    数：Type = 1停电 ，=2上电
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************

void ERC14_Judge(u8 Type)
{
    if(Type == 1)
    {
        ERCP0.ERC14.Status[1] |= 0x01; //;电能表停电
        ERCP0.ERC14.PTOTime[0] = TBCD.Min;
        ERCP0.ERC14.PTOTime[1] = TBCD.Hour;
        ERCP0.ERC14.PTOTime[2] = TBCD.Day;
        ERCP0.ERC14.PTOTime[3] = TBCD.Month;
        ERCP0.ERC14.PTOTime[4] = TBCD.Year; //;停电时间
    }
    else
    {
        ERCP0.ERC14.Status[0] |= 0x01; //;电能表来电
        ERCP0.ERC14.PTOTime[0] = 0xEE;
        ERCP0.ERC14.PTOTime[1] = 0xEE;
        ERCP0.ERC14.PTOTime[2] = 0xEE;
        ERCP0.ERC14.PTOTime[3] = 0xEE;
        ERCP0.ERC14.PTOTime[4] = 0xEE;
        ERCP0.ERC14.PLOTime[0] = TBCD.Min;
        ERCP0.ERC14.PLOTime[1] = TBCD.Hour;
        ERCP0.ERC14.PLOTime[2] = TBCD.Day;
        ERCP0.ERC14.PLOTime[3] = TBCD.Month;
        ERCP0.ERC14.PLOTime[4] = TBCD.Year; //;上电时间
    }
}
*/
void ERC14_Judge(u8 *Byte1, u8 Type)
{
    if(Type == 1)
    {
        ERCP0.ERC14.Status[1] |= 0x01; //;电能表停电
        ERCP0.ERC14.PTOTime[0] = TBCD.Min;
        ERCP0.ERC14.PTOTime[1] = TBCD.Hour;
        ERCP0.ERC14.PTOTime[2] = TBCD.Day;
        ERCP0.ERC14.PTOTime[3] = TBCD.Month;
        ERCP0.ERC14.PTOTime[4] = TBCD.Year; //;停电时间 
        SaveData_FM24(5, E2R_PowD, E2R_PowD + E2R_ByteNum, ERCP0.ERC14.PTOTime); //;保存停电        
    }
    else
    {
        ERCP0.ERC14.Status[0] |= 0x01; //;电能表来电      
        ERCP0.ERC14.PTOTime[0] = *Byte1;
        ERCP0.ERC14.PTOTime[1] = *(Byte1+1);
        ERCP0.ERC14.PTOTime[2] = *(Byte1+2);
        ERCP0.ERC14.PTOTime[3] = *(Byte1+3);
        ERCP0.ERC14.PTOTime[4] = *(Byte1+4); //;停电时间         
        ERCP0.ERC14.PLOTime[0] = TBCD.Min;
        ERCP0.ERC14.PLOTime[1] = TBCD.Hour;
        ERCP0.ERC14.PLOTime[2] = TBCD.Day;
        ERCP0.ERC14.PLOTime[3] = TBCD.Month;
        ERCP0.ERC14.PLOTime[4] = TBCD.Year; //;上电时间
    }
}

/* 
********************************************************************************
* 函 数 名：ERC8_Judge
* 功    能：电表能参数变更
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void ERC8_Judge(struct PointDataStr *PnD, struct ERCPnData *ERCPn)
{
//;    u8  i;
    
    if(ERCPn->ERC8.Status[0] == 0x01) //;上次事件未处理
    {
        return;
    } 
    ERCPn->ERC8.Mark[0] = 0;

    if((PnD->OB224[0] != PnD->RB224[0]) || (PnD->OB224[1] != PnD->RB224[1]) ||
       (PnD->OB224[2] != PnD->RB224[2]))
    {
        ERCPn->ERC8.Mark[0] |= 0x01; //;费率时段表编程发生
        PnD->OB224[0] = PnD->RB224[0]; //;次数
        PnD->OB224[1] = PnD->RB224[1];
        PnD->OB224[2] = PnD->RB224[2];
    }	
    if((PnD->OB21A[0] != PnD->RB21A[0]) || (PnD->OB21A[1] != PnD->RB21A[1]) || 
       (PnD->OB21A[2] != PnD->RB21A[2]) || (PnD->OB21A[3] != PnD->RB21A[3]) || 
       (PnD->OB21A[4] != PnD->RB21A[4]) || (PnD->OB21A[5] != PnD->RB21A[5]))
    {
        ERCPn->ERC8.Mark[0] |= 0x02; //;电能表编程时间更改
        MovNByte(6, PnD->OB21A, PnD->RB21A); //;最近第一次编程时间
    }

    if((PnD->OC117[0] != PnD->RC117[0])||(PnD->OC117[1] != PnD->RC117[1]) ||
       (PnD->OC118[0] != PnD->RC118[0])||(PnD->OC118[1] != PnD->RC118[1]) ||
       (PnD->OC119[0] != PnD->RC119[0])||(PnD->OC119[1] != PnD->RC119[1]))
    {
        ERCPn->ERC8.Mark[0] |= 0x04; //;电能表抄表日更改
        PnD->OC117[0] = PnD->RC117[0]; //;自动抄表日期
        PnD->OC117[1] = PnD->RC117[1];
        PnD->OC118[0] = PnD->RC118[0]; //;自动抄表日期
        PnD->OC118[1] = PnD->RC118[1];
        PnD->OC119[0] = PnD->RC119[0]; //;自动抄表日期
        PnD->OC119[1] = PnD->RC119[1];
    }

    if((PnD->OC030[0] != PnD->RC030[0]) || (PnD->OC030[1] != PnD->RC030[1])
        || (PnD->OC030[2] != PnD->RC030[2]) || (PnD->OC031[0] != PnD->RC031[0])
        || (PnD->OC031[1] != PnD->RC031[1]) || (PnD->OC031[2] != PnD->RC031[2]))
    {
        ERCPn->ERC8.Mark[0] |= 0x08; //;电能表脉冲常数更改
        PnD->OC030[0] = PnD->RC030[0]; //;电表常数(有功)
        PnD->OC030[1] = PnD->RC030[1];
        PnD->OC030[2] = PnD->RC030[2];
        PnD->OC031[0] = PnD->RC031[0]; //;电表常数(无功)
        PnD->OC031[1] = PnD->RC031[1];
        PnD->OC031[2] = PnD->RC031[2];
    }

//;    for(i = 0; i < 24; i++)
//;    {
//;        if(*(PnD->OC331 + i) != *(PnD->RC331 + i))
//;        {
//;            ERCPn->ERC8.Mark[0] |= 0x01; //;电能表时段或费率更改
//;            MovNByte(24,P1D.OC331,P1D.RC331);//;当前时段表起始时间及费率号
//;            break;
//;        }
//;    }

    if((PnD->OC040[0] != PnD->RC040[0]) || (PnD->OC040[1] != PnD->RC040[1]) || 
       (PnD->OC040[2] != PnD->RC040[2]) || (PnD->OC041[0] != PnD->RC041[0]) ||
       (PnD->OC041[1] != PnD->RC041[1]) || (PnD->OC041[2] != PnD->RC041[2]))
    {
        ERCPn->ERC8.Mark[0] |= 0x10; //;电能表互感器倍率更改
        PnD->OC040[0] = PnD->RC040[0]; //;电压倍率
        PnD->OC040[1] = PnD->RC040[1];
        PnD->OC040[2] = PnD->RC040[2];
        PnD->OC041[0] = PnD->RC041[0]; //;电流的倍率
        PnD->OC041[1] = PnD->RC041[1];
        PnD->OC041[2] = PnD->RC041[2];
    }

     if((PnD->OB213[0]!=PnD->RB213[0]) || (PnD->OB213[1]!=PnD->RB213[1]) ||
       (PnD->OB213[2]!=PnD->RB213[2]))
    {
        ERCPn->ERC8.Mark[0] |= 0x20; //;电能表最大需量清零
        //;Read_MeterM_Order(); //;读上月数据
        MovNByte(3, PnD->OB213, PnD->RB213);    //;by zrt 20130910        
    }

    if(ERCPn->ERC8.Mark[0])
    {
        ERCPn->ERC8.Status[0] = 0x01; 
    }
}

/* 
********************************************************************************
* 函 数 名：ERC10_Judge
* 功    能：根据瞬时数据判断是否产生电压回路异常事件,每分钟判断一次
* 参    数：
* 返    回： 
* 说    明：断相: 电压小于电压断相门限,且电流小于额定电流的5%时
*         失压：电压小于断相门限且电流大于额定电流的5%时时间大于连续失压时间限值
*         E10位意义：Bit 1:断相发生 Bit 2:断相恢复
*         Bit 3:失压发生 Bit 4 失压恢复
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void ERC10_Judge(struct PoPStr *PnP, struct PointDataStr *PnD, struct ERCPnData *ERCPn)
{
    u16 Ua, Ub, Uc, UBreak, IBLZZ;   
    u32 Ia, Ib, Ic; 

    
    Ua = BCD2ToU16(PnD->RB611);
    Uc = BCD2ToU16(PnD->RB613);
    UBreak = BCD2ToU16(PnP->F26.UBreak);
    Ia = BCD3ToU32(PnD->RB621) * 10;                  //;I放大1000倍
    Ic = BCD3ToU32(PnD->RB623) * 10;
    IBLZZ = (u16)BCD1ToU8(&P1P.F25.In);
    IBLZZ = IBLZZ * 50;      //;测量点断相和失压电流阈值也放大1000倍5%


    if((Ua < UBreak) && (Ia > IBLZZ) )
    {
        UALostTMin++;
        if(UALostTMin > PnP->F26.ULostT)
        {
            UALostTMin = 0;
            ERCPn->ERC10.Status_N |= 0x10;             //;A相失压
        }
    }
    else
    {
        UALostTMin = 0;
        ERCPn->ERC10.Status_N &= (~0x10);             //;A相失压恢复
    }
    if((Ua < UBreak) && (Ia < IBLZZ))
    {
        ERCPn->ERC10.Status_N |= 0x01;              //;A相断相
    }                                                                           
    else                                                                        
    {                                                                           
        ERCPn->ERC10.Status_N &= (~0x01);          //;A相断相恢复
    }

    if(PnP->F25.Type == 2)
    {
        Ub = BCD2ToU16(PnD->RB612);
        Ib = BCD3ToU32(PnD->RB622);
        
        if((Ub < UBreak) && (Ib < IBLZZ))
        {
            ERCPn->ERC10.Status_N  |= 0x02;       //;B相断相
        }                                                                       
        else                                                                    
        {                                                                       
            ERCPn->ERC10.Status_N  &= (~0x02);    //;B相断相恢复
        }
        if((Ub < UBreak) && (Ib > IBLZZ) )
        {
            UBLostTMin++;
            if(UBLostTMin > PnP->F26.ULostT)
            {
                UBLostTMin = 0;
                ERCPn->ERC10.Status_N |= 0x20;             //;B相失压
            }
        }
        else
        {
            UBLostTMin = 0;
            ERCPn->ERC10.Status_N &= (~0x20);             //;B相失压恢复
        }		
    }

    if((Uc < UBreak) && (Ic > IBLZZ) )
    {
        UCLostTMin++;
	 if(UCLostTMin > PnP->F26.ULostT)
	 {
            UCLostTMin = 0;	 
            ERCPn->ERC10.Status_N |= 0x40;             //;C相失压
	 }
    }
    else
    {
        UCLostTMin = 0;    
        ERCPn->ERC10.Status_N &= (~0x40);             //;C相失压恢复
    }
    if((Uc < UBreak) && (Ic < IBLZZ))
    {
        ERCPn->ERC10.Status_N |= 0x04;           //;C相断相
    }                                                                           
    else                                                                        
    {                                                                           
        ERCPn->ERC10.Status_N &= (~0x04);       //;C相断相恢复
    }

    ERCPn->ERC10.DXOM = 0;
    ERCPn->ERC10.DXRM = 0;
    ERCPn->ERC10.SYOM = 0;
    ERCPn->ERC10.SYRM = 0;	
    
    if(((ERCPn->ERC10.Status_O & 0x01)!= 0x01) && ((ERCPn->ERC10.Status_N & 0x01) == 0x01))      
    {
        ERCPn->ERC10.DXOM |= 0x01;              //;A相断相事件发生
    }
    else if(((ERCPn->ERC10.Status_O & 0x01) == 0x01) && ((ERCPn->ERC10.Status_N & 0x01) != 0x01))
    {
        ERCPn->ERC10.DXRM |= 0x01;             //;A相断相事件恢复
    }
    if(((ERCPn->ERC10.Status_O & 0x10)!= 0x10) && ((ERCPn->ERC10.Status_N & 0x10) == 0x10))      
    {
        ERCPn->ERC10.SYOM |= 0x01;              //;A相失压事件发生
    }
    else if(((ERCPn->ERC10.Status_O & 0x10) == 0x10) && ((ERCPn->ERC10.Status_N & 0x10) != 0x10))
    {
        ERCPn->ERC10.SYRM |= 0x01;             //;A相失压事件恢复
    }	

    if(PnP->F25.Type == 2)
    {
        if(((ERCPn->ERC10.Status_O & 0x02)!= 0x02) && ((ERCPn->ERC10.Status_N & 0x02) == 0x02))
        {
            ERCPn->ERC10.DXOM |= 0x02;
        }
        else if(((ERCPn->ERC10.Status_O& 0x02) == 0x02) && ((ERCPn->ERC10.Status_N & 0x02) != 0x02))
        {
            ERCPn->ERC10.DXRM |= 0x02;
        }
        if(((ERCPn->ERC10.Status_O & 0x20)!= 0x20) && ((ERCPn->ERC10.Status_N & 0x20) == 0x20))
        {
            ERCPn->ERC10.SYOM |= 0x02;
        }
        else if(((ERCPn->ERC10.Status_O& 0x20) == 0x20) && ((ERCPn->ERC10.Status_N & 0x20) != 0x20))
        {
            ERCPn->ERC10.SYRM |= 0x02;
        }		
    }

    if(((ERCPn->ERC10.Status_O & 0x04)!= 0x04) && ((ERCPn->ERC10.Status_N & 0x04) == 0x04))
    {
        ERCPn->ERC10.DXOM |= 0x04;
    }
    else if(((ERCPn->ERC10.Status_O & 0x04) == 0x04) && ((ERCPn->ERC10.Status_N & 0x04) != 0x04))
    {
        ERCPn->ERC10.DXRM |= 0x04;
    }
    if(((ERCPn->ERC10.Status_O & 0x40)!= 0x40) && ((ERCPn->ERC10.Status_N & 0x40) == 0x40))
    {
        ERCPn->ERC10.SYOM |= 0x04;
    }
    else if(((ERCPn->ERC10.Status_O & 0x40) == 0x40) && ((ERCPn->ERC10.Status_N & 0x40) != 0x40))
    {
        ERCPn->ERC10.SYRM |= 0x04;
    }	

    if(ERCPn->ERC10.DXOM || ERCPn->ERC10.DXRM ||ERCPn->ERC10.SYOM || ERCPn->ERC10.SYRM)
    {        
        if (ERCPn->ERC10.DXOM)
        {
            ERCPn->ERC10.Status[0] = 0x01;
        }
        if (ERCPn->ERC10.DXRM)
        {
            ERCPn->ERC10.Status[1] = 0x01;
        }
        if (ERCPn->ERC10.SYOM)
        {
            ERCPn->ERC10.Status[2] = 0x01;
        }
        if (ERCPn->ERC10.SYRM)
        {
            ERCPn->ERC10.Status[3] = 0x01;
        }		
        ERCPn->ERC10.Data[0]  = PnD->RB611[0];//;Ua
        ERCPn->ERC10.Data[1]  = PnD->RB611[1];             
        ERCPn->ERC10.Data[2]  = PnD->RB612[0];      //;Ub                              
        ERCPn->ERC10.Data[3]  = PnD->RB612[1];            
        ERCPn->ERC10.Data[4]  = PnD->RB613[0];      //;Uc                             
        ERCPn->ERC10.Data[5]  = PnD->RB613[1];             
        ERCPn->ERC10.Data[6]  = PnD->RB621[0];      //;Ia                        
        ERCPn->ERC10.Data[7]  = PnD->RB621[1];  
        ERCPn->ERC10.Data[8]  = PnD->RB621[2];            
        ERCPn->ERC10.Data[9]  = PnD->RB622[0];      //;Ib                              
        ERCPn->ERC10.Data[10] = PnD->RB622[1];   
        ERCPn->ERC10.Data[11] = PnD->RB622[2];         
        ERCPn->ERC10.Data[12] = PnD->RB623[0];      //;Ic                            
        ERCPn->ERC10.Data[13] = PnD->RB623[1];  
        ERCPn->ERC10.Data[14] = PnD->RB623[2];           
        ERCPn->ERC10.Data[15] = PnD->R9010[0];
        ERCPn->ERC10.Data[16] = PnD->R9010[1];
        ERCPn->ERC10.Data[17] = PnD->R9010[2];
        ERCPn->ERC10.Data[18] = PnD->R9010[3];
        ERCPn->ERC10.Data[19] = PnD->R9010[4];             //;正向有功总电能;
    }

    ERCPn->ERC10.Status_O = ERCPn->ERC10.Status_N;
}


/*----------------------------------------------------------------------------*
 |  根据发生的事件配置好要发送的短信内容
 *----------------------------------------------------------------------------*/
u16 ERC_SMS_Up(char *Out_Buf)
{
//;    u8   ch1,Len;
//;    u16  j = 0, *addr;
//;    u32  *Addr32;
//;    struct  PoPStr  *PnP;
//;    struct  ERCPnData  *ERCPn;
//;    
//;    Addr32 = (u32*)(*(PnDTAddr + SMS.Pn));
//;    PnP = (struct PoPStr*)(*(Addr32 + _PnP));
//;    ERCPn = (struct ERCPnData*)(*(Addr32 + _ERCPn));
//;    
//;    Len  = SMS_CodeData(Out_Buf, ZDP.F4.SMS_Code, SMS.PhoneNo);                 //;接收人的手机号码
//;    j += Len;
//;    ch1 = j;
//;
//;    j += 2;
//;    if (SMS.Receiver == 'A')                                                    //;短信头
//;    {
//;        addr = (u16*)ZDPKZ.F100;   
//;    }
//;    else if (SMS.Receiver == 'U')
//;    {
//;        addr = (u16*)ZDPKZ.F98;
//;    }
//;    Len = SMS_DataMove(10, Out_Buf + j, addr);
//;    j += Len;
//;
//;    Len = SMS_DataMove(10, Out_Buf + j, (u16*)PnP->F31);                        //;测量点名称
//;    j += Len;
//;
//;    switch (SMS.ERCNo)                                                          //;事件的内容
//;    {
//;        case 8:
//;        {
//;            if (ERCPn->ERC8.Mark[0] & 0x01)
//;            {
//;                Len = SMS_DataMove(6, Out_Buf + j, (u16*)ERC8MS + 3);           //;费率时段变化
//;                j += Len;
//;            }
//;            if (ERCPn->ERC8.Mark[0] & 0x02)
//;            {
//;                Len = SMS_DataMove(6, Out_Buf + j, (u16*)ERC8MS + 9);           //;编程时间更改
//;                j += Len;
//;            }
//;            if (ERCPn->ERC8.Mark[0] & 0x04)
//;            {
//;                Len = SMS_DataMove(5, Out_Buf + j, (u16*)ERC8MS + 14);          //;抄表日更改
//;                j += Len;
//;            }
//;            if (ERCPn->ERC8.Mark[0] & 0x08)
//;            {
//;                Len = SMS_DataMove(6, Out_Buf + j, (u16*)ERC8MS + 20);          //;脉冲常数更改
//;                j += Len;
//;            }
//;            if (ERCPn->ERC8.Mark[0] & 0x10)
//;            {
//;                Len = SMS_DataMove(7, Out_Buf + j, (u16*)ERC8MS + 27);          //;互感器倍率更改
//;                j += Len;
//;            }
//;            if (ERCPn->ERC8.Mark[0] & 0x20)
//;            {
//;                Len = SMS_DataMove(6, Out_Buf + j, (u16*)ERC8MS + 33);          //;最大需量清零
//;                j += Len;
//;            }
//;            break;
//;        }
//;        case 10:
//;        {
//;            if (ERCPn->ERC10.DXOM)
//;            {
//;                if (ERCPn->ERC10.DXOM & 0x01)  
//;                {
//;                    Len = SMS_DataMove(1, Out_Buf + j, (u16*)PhaseMS);          //;   A
//;                    j += Len;
//;                }
//;                
//;                if (ERCPn->ERC10.DXOM & 0x02)
//;                {
//;                    Len = SMS_DataMove(1, Out_Buf + j, (u16*)PhaseMS + 1);      //;   B
//;                    j += Len;
//;                }            
//;                
//;                if (ERCPn->ERC10.DXOM & 0x04)
//;                {
//;                    Len = SMS_DataMove(1, Out_Buf + j, (u16*)PhaseMS + 2);      //;   C
//;                    j += Len;
//;                } 
//;                Len = SMS_DataMove(1, Out_Buf + j, (u16*)PhaseMS + 3);          //;   相
//;                j += Len;
//;                Len = SMS_DataMove(2, Out_Buf + j, (u16*)ERC10MS);              //;   断相
//;                j += Len;      
//;            }
//;            else if (ERCPn->ERC10.SYOM)
//;            {
//;                if (ERCPn->ERC10.SYOM & 0x01)  
//;                {
//;                    Len = SMS_DataMove(1, Out_Buf + j, (u16*)PhaseMS);          //;   A
//;                    j += Len;
//;                }                
//;                if (ERCPn->ERC10.SYOM & 0x02)
//;                {
//;                    Len = SMS_DataMove(1, Out_Buf + j, (u16*)PhaseMS + 1);      //;   B
//;                    j += Len;
//;                }                
//;                if (ERCPn->ERC10.SYOM & 0x04)
//;                {
//;                    Len = SMS_DataMove(1, Out_Buf + j, (u16*)PhaseMS + 2);      //;   C
//;                    j += Len;
//;                } 
//;                Len = SMS_DataMove(1, Out_Buf + j, (u16*)PhaseMS + 3);          //;   相
//;                j += Len;
//;                Len = SMS_DataMove(2, Out_Buf + j, (u16*)ERC10MS + 2);          //;   失压
//;                j += Len;
//;            }
//;            break;
//;        }
//;        case 12:
//;        {
//;            Len = SMS_DataMove(7, Out_Buf + j, (u16*)ERC12MS);                  //;电能表时间超差
//;            j += Len;
//;            break;
//;        }
//;        case 17:
//;        {
//;            if (ERCPn->ERC17.BPH_OM & 0x01)
//;            {
//;                Len = SMS_DataMove(7, Out_Buf + j, (u16*)ERC17MS);              //;电压不平衡越限
//;                j += Len;
//;            }
//;            if (ERCPn->ERC17.BPH_OM & 0x02)
//;            {
//;                Len = SMS_DataMove(7, Out_Buf + j, (u16*)ERC17MS + 7);          //;电流不平衡越限
//;                j += Len;
//;            }
//;            break;
//;        }
//;        case 24:
//;        {
//;            if (ERCPn->ERC24.YSSXOM)
//;            {
//;                if (ERCPn->ERC24.YSSXOM & 0x01)  
//;                {
//;                    Len = SMS_DataMove(1, Out_Buf + j, (u16*)PhaseMS);          //;   A
//;                    j += Len;
//;                }                
//;                if (ERCPn->ERC24.YSSXOM & 0x02)
//;                {
//;                    Len = SMS_DataMove(1, Out_Buf + j, (u16*)PhaseMS + 1);      //;   B
//;                    j += Len;
//;                }                
//;                if (ERCPn->ERC24.YSSXOM & 0x04)
//;                {
//;                    Len = SMS_DataMove(1, Out_Buf + j, (u16*)PhaseMS + 2);      //;   C
//;                    j += Len;
//;                } 
//;                Len = SMS_DataMove(1, Out_Buf + j, (u16*)PhaseMS + 3);          //;   相
//;                j += Len;
//;                Len = SMS_DataMove(6, Out_Buf + j, (u16*)ERC24MS);              //;   电压越上上限
//;                j += Len;
//;            }
//;            else if (ERCPn->ERC24.YXXXOM)
//;            {
//;                if (ERCPn->ERC24.YXXXOM & 0x01)  
//;                {
//;                    Len = SMS_DataMove(1, Out_Buf + j, (u16*)PhaseMS);          //;   A
//;                    j += Len;
//;                }                
//;                if (ERCPn->ERC24.YXXXOM & 0x02)
//;                {
//;                    Len = SMS_DataMove(1, Out_Buf + j, (u16*)PhaseMS + 1);      //;   B
//;                    j += Len;
//;                }                
//;                if (ERCPn->ERC24.YXXXOM & 0x04)
//;                {
//;                    Len = SMS_DataMove(1, Out_Buf + j, (u16*)PhaseMS + 2);      //;   C
//;                    j += Len;
//;                } 
//;                Len = SMS_DataMove(1, Out_Buf + j, (u16*)PhaseMS + 3);          //;   相
//;                j += Len;
//;                Len = SMS_DataMove(6, Out_Buf + j, (u16*)ERC24MS + 6);          //;   电压越下下限
//;                j += Len;
//;            }
//;            break;
//;        }
//;        case 25:
//;        {
//;            if (ERCPn->ERC25.YSSXOM)
//;            {
//;                if (ERCPn->ERC25.YSSXOM & 0x01)  
//;                {
//;                    Len = SMS_DataMove(1, Out_Buf + j, (u16*)PhaseMS);          //;   A
//;                    j += Len;
//;                }                
//;                if (ERCPn->ERC25.YSSXOM & 0x02)
//;                {
//;                    Len = SMS_DataMove(1, Out_Buf + j, (u16*)PhaseMS + 1);      //;   B
//;                    j += Len;
//;                }                
//;                if (ERCPn->ERC25.YSSXOM & 0x04)
//;                {
//;                    Len = SMS_DataMove(1, Out_Buf + j, (u16*)PhaseMS + 2);      //;   C
//;                    j += Len;
//;                } 
//;                Len = SMS_DataMove(1, Out_Buf + j, (u16*)PhaseMS + 3);          //;   相
//;                j += Len;
//;                Len = SMS_DataMove(6, Out_Buf + j, (u16*)ERC25MS + 5);          //;电流越上上限
//;                j += Len;
//;            }
//;            else if (ERCPn->ERC25.YSXOM)
//;            {
//;                if (ERCPn->ERC25.YSXOM & 0x01)  
//;                {
//;                    Len = SMS_DataMove(1, Out_Buf + j, (u16*)PhaseMS);          //;   A
//;                    j += Len;
//;                }                
//;                if (ERCPn->ERC25.YSXOM & 0x02)
//;                {
//;                    Len = SMS_DataMove(1, Out_Buf + j, (u16*)PhaseMS + 1);      //;   B
//;                    j += Len;
//;                }                
//;                if (ERCPn->ERC25.YSXOM & 0x04)
//;                {
//;                    Len = SMS_DataMove(1, Out_Buf + j, (u16*)PhaseMS + 2);      //;   C
//;                    j += Len;
//;                } 
//;                Len = SMS_DataMove(1, Out_Buf + j, (u16*)PhaseMS + 3);          //;   相
//;                Len = SMS_DataMove(5, Out_Buf + j, (u16*)ERC25MS);              //;电流越上限
//;                j += Len;
//;            }
//;            break;
//;        }
//;        case 26:
//;        {
//;            if (ERCPn->ERC26.Status[0] & 0x01)
//;            {
//;                 Len = SMS_DataMove(7, Out_Buf + j, (u16*)ERC26MS);             //;视在功率越上限
//;                 j += Len;
//;            }
//;            else if (ERCPn->ERC26.Status[2] & 0x01)
//;            {
//;                Len = SMS_DataMove(8, Out_Buf + j, (u16*)ERC26MS + 7);          //;视在功率越上上限
//;                j += Len;
//;            }
//;            break;
//;        }
//;        case 27:
//;        {
//;            Len = SMS_DataMove(7, Out_Buf + j, (u16*)ERC27MS);                  //;电能表示度下降
//;            j += Len;
//;            break;
//;        }
//;        case 29:
//;        {
//;            Len = SMS_DataMove(7, Out_Buf + j, (u16*)ERC29MS);                  //;电能表飞走
//;            j += Len;
//;            break;
//;        }
//;        case 30:                                                                //;电能表停走
//;        {
//;            Len = SMS_DataMove(5, Out_Buf + j, (u16*)ERC30MS);
//;            j += Len;
//;            break;
//;        }
//;        //;case 31:
//;        //;{
//;        //;    if (AlarmF.ERC31[7] & 0x80)
//;        //;    {
//;        //;        Len = SMS_DataMove(2, Out_Buf + j, (u16*)OccRecMS);             //;发生
//;        //;    }
//;        //;    else
//;        //;    {
//;        //;        Len = SMS_DataMove(2, Out_Buf + j, (u16*)OccRecMS + 2);         //;恢复
//;        //;    }
//;        //;    j += Len;
//;        //;    Len = SMS_DataMove(3, Out_Buf + j, (u16*)ERC31MS);                  //;电能表
//;        //;    j += Len;
//;        //;
//;        //;    if (AlarmF.ERC31[8] & 0x01)
//;        //;    {
//;        //;        Len = SMS_DataMove(2, Out_Buf + j, (u16*)PhaseMS);
//;        //;        j += Len;
//;        //;    }
//;        //;    if (AlarmF.ERC31[8] & 0x02)
//;        //;    {
//;        //;        Len = SMS_DataMove(2, Out_Buf + j, (u16*)PhaseMS + 2);
//;        //;        j += Len;
//;        //;    }
//;        //;    if (AlarmF.ERC31[8] & 0x04)
//;        //;    {
//;        //;        Len = SMS_DataMove(2, Out_Buf + j, (u16*)PhaseMS + 4);
//;        //;        j += Len;
//;        //;    }
//;        //;    Len = SMS_DataMove(2, Out_Buf + j, (u16*)ERC31MS + 3);              //;失压
//;        //;    j += Len;
//;        //;    break;
//;        //;}
//;        //;case 32:                                                                //;电能表失流
//;        //;{
//;        //;    if (AlarmF.ERC32[7] & 0x80)
//;        //;    {
//;        //;        Len = SMS_DataMove(2, Out_Buf + j, (u16*)OccRecMS);             //;发生
//;        //;    }
//;        //;    else
//;        //;    {
//;        //;        Len = SMS_DataMove(2, Out_Buf + j, (u16*)OccRecMS + 2);         //;恢复
//;        //;    }
//;        //;    j += Len;
//;        //;
//;        //;    Len = SMS_DataMove(3, Out_Buf + j, (u16*)ERC32MS);
//;        //;    j += Len;
//;        //;
//;        //;    if (AlarmF.ERC32[8] & 0x01)
//;        //;    {
//;        //;        Len = SMS_DataMove(2, Out_Buf + j, (u16*)PhaseMS);
//;        //;        j += Len;
//;        //;    }
//;        //;    if (AlarmF.ERC32[8] & 0x02)
//;        //;    {
//;        //;        Len = SMS_DataMove(2, Out_Buf + j, (u16*)PhaseMS + 2);
//;        //;        j += Len;
//;        //;    }
//;        //;    if (AlarmF.ERC32[8] & 0x04)
//;        //;    {
//;        //;        Len = SMS_DataMove(2, Out_Buf + j, (u16*)PhaseMS + 4);
//;        //;        j += Len;
//;        //;    }
//;        //;    Len = SMS_DataMove(2, Out_Buf + j, (u16*)ERC32MS + 3);
//;        //;    j += Len;
//;        //;    break;
//;        //;}
//;        //;case 33:                                                                //;电能表断相
//;        //;{
//;        //;    if (AlarmF.ERC33[7] &0x80)
//;        //;    {
//;        //;        Len = SMS_DataMove(2, Out_Buf + j, (u16*)OccRecMS);             //;发生
//;        //;        j += Len;
//;        //;    }
//;        //;    else
//;        //;    {
//;        //;        Len = SMS_DataMove(2, Out_Buf + j, (u16*)OccRecMS + 2);         //;恢复
//;        //;        j += Len;
//;        //;    }
//;        //;    Len = SMS_DataMove(3, Out_Buf + j, (u16*)ERC33MS);
//;        //;    j += Len;
//;        //;
//;        //;    if (AlarmF.ERC33[8] & 0x01)
//;        //;    {
//;        //;        Len = SMS_DataMove(2, Out_Buf + j, (u16*)PhaseMS);
//;        //;        j += Len;
//;        //;    }
//;        //;    if (AlarmF.ERC33[8] & 0x02)
//;        //;    {
//;        //;        Len = SMS_DataMove(2, Out_Buf + j, (u16*)PhaseMS + 2);
//;        //;        j += Len;
//;        //;    }
//;        //;    if (AlarmF.ERC33[8] & 0x04)
//;        //;    {
//;        //;        Len = SMS_DataMove(2, Out_Buf + j, (u16*)PhaseMS + 4);
//;        //;        j += Len;
//;        //;    }
//;        //;    Len = SMS_DataMove(2, Out_Buf + j, (u16*)ERC33MS + 3);
//;        //;    j += Len;
//;        //;    break;
//;        //;}
//;        case 34: //;电能表485抄表故障
//;        {
//;            Len = SMS_DataMove(10, Out_Buf + j, (u16*)ERC34MS);
//;            j += Len;
//;            break;
//;        }
//;        case 35://;电能表硬件故障
//;        {
//;            Len = SMS_DataMove(7, Out_Buf + j, (u16*)ERC35MS);
//;            j += Len;
//;            break;
//;        }
//;        case 36://;电能表剩余金额小于告警金额
//;        {
//;            Len = SMS_DataMove(13, Out_Buf + j, (u16*)ERC36MS);
//;            j += Len;
//;            break;
//;        }
//;        case 37://;电能表剩余金额小于0
//;        {
//;            Len = SMS_DataMove(10, Out_Buf + j, (u16*)ERC37MS);
//;            j += Len;
//;            break;
//;        }
//;        case 38://;电能表剩余金额小于透支金额
//;        {
//;            Len = SMS_DataMove(13, Out_Buf + j, (u16*)ERC38MS);
//;            j += Len;
//;            break;
//;        }
//;        case 39://;电能表跳闸
//;        {
//;            Len = SMS_DataMove(5, Out_Buf + j, (u16*)ERC39MS);
//;            j += Len;
//;            break;
//;        }
//;        case 40: //;电能表购电成功
//;        {
//;            Len = SMS_DataMove(7, Out_Buf + j, (u16*)ERC40MS);
//;            j += Len;
//;            break;
//;        }
//;        case 41://;电能表允许合闸
//;        {
//;            Len = SMS_DataMove(7, Out_Buf + j, (u16*)ERC41MS);
//;            j += Len;
//;            break;
//;        }
//;        default:
//;        {
//;            break;
//;        }
//;    }
//;
//;    if (SMS.Receiver == 'A')                                                    //;写入短信尾
//;    {
//;        addr = (u16*)ZDPKZ.F101;  
//;    }
//;    else if (SMS.Receiver == 'U')
//;    {
//;        addr = (u16*)ZDPKZ.F99; 
//;    }
//;    Len = SMS_DataMove(10, Out_Buf + j, addr);
//;    j += Len;
//;
//;    *(Out_Buf + ch1 + 0) = SMS_HexToASC((( (j - 2 - ch1) / 2) & 0xF0) >> 4);
//;    *(Out_Buf + ch1 + 1) = SMS_HexToASC((((j - 2 - ch1) / 2) & 0x0F));
//;
//;    SMS.Len = ((j - 2 - ch1) / 2) + 13;
//;
//;    *(Out_Buf + j++) = 0x1A;              //;  ctrl+Z
//;    return j;                             //;一共写入了多少个Ascii码
return 0;
}

 /* 
********************************************************************************
* 函 数 名：ERC_DTF
* 功    能：
* 参    数：
* 返    回： 
* 说    明：DT=1：记录; DT=2：上送; DT=3: 发送短信给管理员; DT=4: 发送短信给用户
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
u8 ERC_DTF(u8 ERCNo, u8 Status, u8 DT)
{
   u8  *DTAdr;
   u32  Adr[5] = {0,(u32)(ZDP.F9.Enable),(u32)(ZDP.F9.Rating),
                    (u32)(ZDPKZ.F97.AdmiSend),(u32)ZDPKZ.F97.UserSend};
    
   DTAdr = (u8 *)(Adr[DT]);

   if(Status & 0x01) 
   {
       if(DTAdr[(ERCNo - 1) / 8] & (0x01 << ((ERCNo - 1) % 8)))  //;需要处理
       {
           if(!(Status & (0x01 << DT)))  
           {
               return true;  //;还未处理
           }
       }
   }
   
   return false;
}



/* 
********************************************************************************
* 函 数 名：ERC_DTS
* 功    能：事件处理完成后标志和状态的清零
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
u8 ERC_DTS(u8 ERCNo, u8 *Status, u8 DT)
{
   u8  *DTAdr;
   u32  Adr[5] = {0,(u32)(ZDP.F9.Enable),(u32)(ZDP.F9.Rating),(u32)(ZDPKZ.F97.AdmiSend),
                    (u32)ZDPKZ.F97.UserSend};
    
   DTAdr = (u8 *)(Adr[DT]);
   
   if(*Status & 0x01) 
   {
       if(DTAdr[(ERCNo - 1) / 8] & (0x01 << ((ERCNo - 1) % 8)))       //;需要处理
       {
           if(!(*Status & (0x01 << DT)))  
           {
               *(Status + 0) |= (0x01 << DT);                          //;当前状态处理完成
               
#ifdef ERC_SMS 
               
               if((*(Status + 0) & 0x1F) == 0x1F)                     //;该事件完成有所有的处理
               {
                   *(Status + 0) = 0;                                  //;清除全部事件标志            
               }
             
#else
               
              // if((*(Status + 0) & 0x07) == 0x07)                     //;该事件完成有所有的处理
              if((*(Status + 0) & 0x03) == 0x03)                     //;该事件完成有所有的处理
               {
                   *(Status + 0) = 0;                                  //;清除全部事件标志            
               }
             
#endif 
               
               return true;
           }
       }
   }
   return false;
}



/*----------------------------------------------------------------------------*
 |  找出发生的事件
 *----------------------------------------------------------------------------*/
u8 ERC_Find(u8 Pn, u8 DT, u8 *status)
{
	  *status = 0x00;
    if (Pn == 0)                                                                //;找出终端事件
    {
        if (ERC_DTF(1,  ERCP0.ERC1.Status[0], DT) == true)  return 1;
        if (ERC_DTF(3,  ERCP0.ERC3.Status[0], DT) == true)  return 3; 
        if (ERC_DTF(14, ERCP0.ERC14.Status[0],DT) == true)  
        {
        	return 14;
        }  
        if (ERC_DTF(14, ERCP0.ERC14.Status[1],DT) == true)
        {
        	  *status = 0x01;
        	  return 14; 
        }
        if (ERC_DTF(20, ERCP0.ERC20.Status[0],DT) == true)  return 20; 
        if (ERC_DTF(21, ERCP0.ERC21.Status[0],DT) == true)  return 21; 
    } 
    else if (Pn <= 1)                                                          //;找出测量点事件
    {           
        u32     *Addr32;
        struct  ERCPnData  *ERCPn;
        
        Addr32 = (u32*)(*(PnDTAddr + Pn - 1));
        ERCPn = (struct ERCPnData*)(*(Addr32 + _ERCPn));
        
        if (ERC_DTF(8,  ERCPn->ERC8.Status[0], DT) == true) return 8;
                           
        if (ERC_DTF(10, ERCPn->ERC10.Status[0], DT) == true) 
        {
        	
        	return 10;
        }
        if (ERC_DTF(10, ERCPn->ERC10.Status[1], DT) == true) 
        {
        	*status = 0x01;
        	return 10;
        }
        if (ERC_DTF(10, ERCPn->ERC10.Status[2], DT) == true) 
        {
        	*status = 0x02;
        	return 10;
        }
        if (ERC_DTF(10, ERCPn->ERC10.Status[3], DT) == true)
        {
        	*status = 0x03;
        	return 10;
        }
                           
        if (ERC_DTF(13, ERCPn->ERC13.Status[0], DT) == true) return 13;     
                           
        if (ERC_DTF(17, ERCPn->ERC17.Status[0], DT) == true) return 17; 
        if (ERC_DTF(17, ERCPn->ERC17.Status[1], DT) == true) 
        {
        	*status = 0x01;
        	return 17;
        } 
                           
        if (ERC_DTF(24, ERCPn->ERC24.Status[0], DT) == true) return 24; 
        if (ERC_DTF(24, ERCPn->ERC24.Status[1], DT) == true) 
        {
        	*status = 0x01;
        	return 24;
        } 
        if (ERC_DTF(24, ERCPn->ERC24.Status[2], DT) == true) 
        {
        	*status = 0x02;
        	return 24;
        }  
        if (ERC_DTF(24, ERCPn->ERC24.Status[3], DT) == true) 
        {
        	*status = 0x03;
        	return 24;
        }  
                           
        if (ERC_DTF(25, ERCPn->ERC25.Status[0], DT) == true) return 25; 
        if (ERC_DTF(25, ERCPn->ERC25.Status[1], DT) == true) 
        {
        	*status = 0x01;
        	return 25;
        }  
        if (ERC_DTF(25, ERCPn->ERC25.Status[2], DT) == true)
        {
        	*status = 0x02;
        	return 25;
        }   
        if (ERC_DTF(25, ERCPn->ERC25.Status[3], DT) == true) 
        {
        	*status = 0x03;
        	return 25;
        }  
                           
        if (ERC_DTF(26, ERCPn->ERC26.Status[0], DT) == true) return 26; 
        if (ERC_DTF(26, ERCPn->ERC26.Status[1], DT) == true) 
        {
        	*status = 0x01;
        	return 26;
        }   
        if (ERC_DTF(26, ERCPn->ERC26.Status[2], DT) == true) 
        {
        	*status = 0x02;
        	return 26;
        }   
        if (ERC_DTF(26, ERCPn->ERC26.Status[3], DT) == true) 
        {
        	*status = 0x03;
        	return 26;
        }   
                           
        if (ERC_DTF(27, ERCPn->ERC27.Status[0], DT) == true) return 27;
        if (ERC_DTF(29, ERCPn->ERC29.Status[0], DT) == true) return 29;
    }
    
    return 0xFF;
}


void ERC_Finish(u8 Pn, u8 ERCNo, u8 DT)
{
    u32     *Addr32;
    struct  ERCPnData  *ERCPn;
    
    if (Pn > 0)                                                          //;找出测量点事件
    {           
        Addr32 = (u32*)(*(PnDTAddr + Pn - 1));
        ERCPn = (struct ERCPnData*)(*(Addr32 + _ERCPn));
    }

    switch (ERCNo)
    {
        case 1:  ERC_DTS(1, ERCP0.ERC1.Status, DT);   break;
        case 3:  ERC_DTS(3, ERCP0.ERC3.Status, DT);   break;
        case 8:  ERC_DTS(8, ERCPn->ERC8.Status, DT);  break;
        case 10: 
        {
            if (ERC_DTS(10, ERCPn->ERC10.Status + 0, DT) == true) return; 
            if (ERC_DTS(10, ERCPn->ERC10.Status + 1, DT) == true) return; 
            if (ERC_DTS(10, ERCPn->ERC10.Status + 2, DT) == true) return;
            if (ERC_DTS(10, ERCPn->ERC10.Status + 3, DT) == true) return; 
            break;
        }
        case 13: ERC_DTS(13, ERCPn->ERC13.Status, DT); break;
        case 17: 
        {
            if (ERC_DTS(17, ERCPn->ERC17.Status + 0, DT) == true) return; 
            if (ERC_DTS(17, ERCPn->ERC17.Status + 1, DT) == true) return; 
            break;
        }
        case 14: 
        {
            if (ERC_DTS(14,ERCP0.ERC14.Status + 0, DT) == true) return;
            if (ERC_DTS(14,ERCP0.ERC14.Status + 1, DT) == true) return;
            break;
        }
        case 20: ERC_DTS(20,ERCP0.ERC20.Status, DT); break;
        case 21: ERC_DTS(21,ERCP0.ERC21.Status, DT); break;
        case 24:
        {
            if (ERC_DTS(24, ERCPn->ERC24.Status + 0, DT) == true) return; 
            if (ERC_DTS(24, ERCPn->ERC24.Status + 1, DT) == true) return; 
            if (ERC_DTS(24, ERCPn->ERC24.Status + 2, DT) == true) return;
            if (ERC_DTS(24, ERCPn->ERC24.Status + 3, DT) == true) return; 
            break;
        }
        case 25:
        {
            if (ERC_DTS(25, ERCPn->ERC25.Status + 0, DT) == true) return; 
            if (ERC_DTS(25, ERCPn->ERC25.Status + 1, DT) == true) return; 
            if (ERC_DTS(25, ERCPn->ERC25.Status + 2, DT) == true) return;
            if (ERC_DTS(25, ERCPn->ERC25.Status + 3, DT) == true) return; 
            break;
        }
        case 26:
        {
            if (ERC_DTS(26, ERCPn->ERC26.Status + 0, DT) == true) return; 
            if (ERC_DTS(26, ERCPn->ERC26.Status + 1, DT) == true) return; 
            if (ERC_DTS(26, ERCPn->ERC26.Status + 2, DT) == true) return;
            if (ERC_DTS(26, ERCPn->ERC26.Status + 3, DT) == true) return; 
            break;
        }
        case 27: ERC_DTS(27,ERCPn->ERC27.Status, DT); break;
        case 29: ERC_DTS(29,ERCPn->ERC29.Status, DT); break;
        default:  break;
    }
}

/* 
********************************************************************************
* 函 数 名：ERC_DataPre
* 功    能：组织事件数据结构
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
u8 ERC_DataPre(u16 Pn, u8 ERCNo, char *OutData)
{
    u8 j = 0;
    u32     *Addr32;
    struct  ERCPnData  *ERCPn;
    
    if(Pn > 0)                                                          //;找出测量点事件
    {           
        Addr32 = (u32*)(*(PnDTAddr + Pn - 1));
        ERCPn = (struct ERCPnData*)(*(Addr32 + _ERCPn));
    }
    
    switch(ERCNo)
    {
        case 1:
        {
            *(OutData + j++) = 1;
            *(OutData + j++) = 14;
            EW.MoveByte = 'M'; 
            MovNByte(14, (u8*)OutData + j, ERCP0.ERC1.OTime);
            EW.MoveByte = 0;  
            j += 14;
            break;
        }
        case 3:
        {
            u8 Le;
            *(OutData + j++) = 3;
            *(OutData + j++) = Le = ERCP0.ERC3.Num * 4 + 6;
            EW.MoveByte = 'M'; 
            MovNByte(Le, (u8*)OutData + j, ERCP0.ERC3.OTime);
            EW.MoveByte = 0;  
            j += Le;
            break;
        }
        case 8:
        {
            *(OutData + j++) = 8;
            *(OutData + j++) = 8;
            *(OutData + j++) = ERCPn->OTime[0];
            *(OutData + j++) = ERCPn->OTime[1];
            *(OutData + j++) = ERCPn->OTime[2];
            *(OutData + j++) = ERCPn->OTime[3];
            *(OutData + j++) = ERCPn->OTime[4]; 
            *(OutData + j++) = (u8)ERCPn->Pn;
            *(OutData + j++) = (u8)(ERCPn->Pn >> 8);
            *(OutData + j++) = ERCPn->ERC8.Mark[0];
            j += 7;
            break;
        }
        case 10:
        {
            *(OutData + j++) = 10;
            *(OutData + j++) = 28;
            *(OutData + j++) = ERCPn->OTime[0];
            *(OutData + j++) = ERCPn->OTime[1];
            *(OutData + j++) = ERCPn->OTime[2];
            *(OutData + j++) = ERCPn->OTime[3];
            *(OutData + j++) = ERCPn->OTime[4]; 
            *(OutData + j++) = (u8)ERCPn->Pn;
            *(OutData + j) = (u8)(ERCPn->Pn >> 8);
            if (ERCPn->ERC10.Status[0] & 0x01)
            {
                *(OutData + j++) |= 0x80;
                *(OutData + j) = ERCPn->ERC10.DXOM;
                *(OutData + j++) |= 0x40;
            }
            else if (ERCPn->ERC10.Status[1] & 0x01) 
            {
                *(OutData + j++) &= (~0x80);
                *(OutData + j) = ERCPn->ERC10.DXRM;
                *(OutData + j++) |= 0x40;
            }
            else if (ERCPn->ERC10.Status[2] & 0x01) 
            {
                *(OutData + j++) |= 0x80;
                *(OutData + j) = ERCPn->ERC10.SYOM;
                *(OutData + j++) |= 0x80;
            }
            else if (ERCPn->ERC10.Status[3] & 0x01) 
            {
                *(OutData + j++) &= (~0x80);
                *(OutData + j) = ERCPn->ERC10.SYRM;
                *(OutData + j++) |= 0x80;
            }
            EW.MoveByte = 'M'; 
            MovNByte(20, (u8*)OutData + j, ERCPn->ERC10.Data);
            EW.MoveByte = 0;  
            j += 20;
            break;
        }
        case 13:
        {
            *(OutData + j++) = 13;
            *(OutData + j++) = 8;
            *(OutData + j++) = ERCPn->OTime[0];
            *(OutData + j++) = ERCPn->OTime[1];
            *(OutData + j++) = ERCPn->OTime[2];
            *(OutData + j++) = ERCPn->OTime[3];
            *(OutData + j++) = ERCPn->OTime[4]; 
            *(OutData + j++) = (u8)ERCPn->Pn;
            *(OutData + j++) = (u8)(ERCPn->Pn >> 8);
            *(OutData + j++) = ERCPn->ERC13.Mark[0];
            break;
        }
        case 14:
        {
            *(OutData + j++) = 14;
            *(OutData + j++) = 10;
            EW.MoveByte = 'M'; 
            MovNByte(10, (u8*)OutData + j, ERCP0.ERC14.PTOTime);
            EW.MoveByte = 0; 
            j += 10;
            break;
        }
        case 17:
        {
            *(OutData + j++) = 17;
            *(OutData + j++) = 27;
            *(OutData + j++) = ERCPn->OTime[0];
            *(OutData + j++) = ERCPn->OTime[1];
            *(OutData + j++) = ERCPn->OTime[2];
            *(OutData + j++) = ERCPn->OTime[3];
            *(OutData + j++) = ERCPn->OTime[4]; 
            *(OutData + j++) = (u8)ERCPn->Pn;
            *(OutData + j) = (u8)(ERCPn->Pn >> 8);
            if(ERCPn->ERC17.Status[0] & 0x01)
            {
                *(OutData + j++) |= 0x80;
                *(OutData + j++) = ERCPn->ERC17.BPH_OM;
            }
            else if (ERCPn->ERC17.Status[1] & 0x01) 
            {
                *(OutData + j++) &= (~0x80);
                *(OutData + j++) = ERCPn->ERC17.BPH_RM;
            }
            EW.MoveByte = 'M'; 
            MovNByte(19, (u8*)OutData + j, ERCPn->ERC17.Data);
            EW.MoveByte = 0;  
            j += 19;
            break; 
        }
        case 20:
        {
            *(OutData + j++) = 20;
            *(OutData + j++) = 22;
            EW.MoveByte = 'M'; 
            MovNByte(22, (u8*)OutData + j, ERCP0.ERC20.OTime);
            EW.MoveByte = 0; 
            j += 22;
            break;  
        }
        case 21:
        {
            *(OutData + j++) = 21;
            *(OutData + j++) = 6;
            EW.MoveByte = 'M'; 
            MovNByte(6, (u8*)OutData + j, ERCP0.ERC21.OTime);
            EW.MoveByte = 0;
            j += 6;
            break;  
        }
        case 24:
        {
            *(OutData + j++) = 24;
            *(OutData + j++) = 14;
            *(OutData + j++) = ERCPn->OTime[0];
            *(OutData + j++) = ERCPn->OTime[1];
            *(OutData + j++) = ERCPn->OTime[2];
            *(OutData + j++) = ERCPn->OTime[3];
            *(OutData + j++) = ERCPn->OTime[4]; 
            *(OutData + j++) = (u8)ERCPn->Pn;
            *(OutData + j) = (u8)(ERCPn->Pn >> 8);
            if (ERCPn->ERC24.Status[0] & 0x01)
            {
                *(OutData + j++) |= 0x80;
                *(OutData + j) = ERCPn->ERC24.YSSXOM;
                *(OutData + j++) |= 0x40;
            }
            else if (ERCPn->ERC24.Status[1] & 0x01) 
            {
                *(OutData + j++) &= (~0x80);
                *(OutData + j) = ERCPn->ERC24.YSSXRM;
                *(OutData + j++) |= 0x40;
            }
            else if (ERCPn->ERC24.Status[2] & 0x01) 
            {
                *(OutData + j++) |= 0x80;
                *(OutData + j) = ERCPn->ERC24.YXXXOM;
                *(OutData + j++) |= 0x80;
            }
            else if (ERCPn->ERC24.Status[3] & 0x01) 
            {
                *(OutData + j++) &= (~0x80);
                *(OutData + j) = ERCPn->ERC24.YXXXRM;
                *(OutData + j++) |= 0x80;
            }
            EW.MoveByte = 'M'; 
            MovNByte(6, (u8*)OutData + j, ERCPn->ERC24.Data);
            EW.MoveByte = 0;  
            j += 6;
            break;
            
        }
        case 25:
        {
            *(OutData + j++) = 25;
            *(OutData + j++) = 17;
            *(OutData + j++) = ERCPn->OTime[0];
            *(OutData + j++) = ERCPn->OTime[1];
            *(OutData + j++) = ERCPn->OTime[2];
            *(OutData + j++) = ERCPn->OTime[3];
            *(OutData + j++) = ERCPn->OTime[4]; 
            *(OutData + j++) = (u8)ERCPn->Pn;
            *(OutData + j) = (u8)(ERCPn->Pn >> 8);
            if (ERCPn->ERC25.Status[0] & 0x01)
            {
                *(OutData + j++) |= 0x80;
                *(OutData + j) = ERCPn->ERC25.YSSXOM;
                *(OutData + j++) |= 0x40;
            }
            else if (ERCPn->ERC25.Status[1] & 0x01) 
            {
                *(OutData + j++) &= (~0x80);
                *(OutData + j) = ERCPn->ERC25.YSSXRM;
                *(OutData + j++) |= 0x40;
            }
            else if (ERCPn->ERC25.Status[2] & 0x01) 
            {
                *(OutData + j++) |= 0x80;
                *(OutData + j) = ERCPn->ERC25.YSXOM;
                *(OutData + j++) |= 0x80;
            }
            else if (ERCPn->ERC25.Status[3] & 0x01) 
            {
                *(OutData + j++) &= (~0x80);
                *(OutData + j) = ERCPn->ERC25.YSXRM;
                *(OutData + j++) |= 0x80;
            }
            EW.MoveByte = 'M'; 
            MovNByte(9, (u8*)OutData + j, ERCPn->ERC25.Data);
            EW.MoveByte = 0;  
            j += 9;
            break;
        }
        case 26:
        {
            *(OutData + j++) = 26;
            *(OutData + j++) = 14;
            *(OutData + j++) = ERCPn->OTime[0];
            *(OutData + j++) = ERCPn->OTime[1];
            *(OutData + j++) = ERCPn->OTime[2];
            *(OutData + j++) = ERCPn->OTime[3];
            *(OutData + j++) = ERCPn->OTime[4]; 
            *(OutData + j++) = (u8)ERCPn->Pn;
            *(OutData + j) = (u8)(ERCPn->Pn >> 8);
            if (ERCPn->ERC26.Status[0] & 0x01)
            {
                *(OutData + j++) |= 0x80;          //;起止标识
//;                *(OutData + j++) = 0x40;        越限标识0x80为上限，0x40为上上限 by zrt 20130906
                *(OutData + j++) = 0x80;				
            }
            else if (ERCPn->ERC26.Status[1] & 0x01) 
            {
                *(OutData + j++) &= (~0x80);
//;                *(OutData + j++) = 0x40;
                *(OutData + j++) = 0x80;				
            }
            else if (ERCPn->ERC26.Status[2] & 0x01) 
            {
                *(OutData + j++) |= 0x80;
//;                *(OutData + j++) = 0x80;
                *(OutData + j++) = 0x40;				
            }
            else if (ERCPn->ERC26.Status[3] & 0x01) 
            {
                *(OutData + j++) &= (~0x80);
//;                *(OutData + j++) = 0x80;
                *(OutData + j++) = 0x40;				
            }
            EW.MoveByte = 'M'; 
            MovNByte(6, (u8*)OutData + j, ERCPn->ERC26.Data);
            EW.MoveByte = 0;  
            j += 6;
            break;
        }
        case 27:
        {
            *(OutData + j++) = 27;
            *(OutData + j++) = 17;
            *(OutData + j++) = ERCPn->OTime[0];
            *(OutData + j++) = ERCPn->OTime[1];
            *(OutData + j++) = ERCPn->OTime[2];
            *(OutData + j++) = ERCPn->OTime[3];
            *(OutData + j++) = ERCPn->OTime[4]; 
            *(OutData + j++) = (u8)ERCPn->Pn;
            *(OutData + j) = (u8)(ERCPn->Pn >> 8);
            *(OutData + j++) |= 0x80; 			//;by zrt 20130924
            EW.MoveByte = 'M'; 
            MovNByte(10, (u8*)OutData + j, ERCPn->ERC27.Data);
            EW.MoveByte = 0;  
            j += 10;
            break;   
        }
        case 29:
        {
            *(OutData + j++) = 29;
            *(OutData + j++) = 18;
            *(OutData + j++) = ERCPn->OTime[0];
            *(OutData + j++) = ERCPn->OTime[1];
            *(OutData + j++) = ERCPn->OTime[2];
            *(OutData + j++) = ERCPn->OTime[3];
            *(OutData + j++) = ERCPn->OTime[4]; 
            *(OutData + j++) = (u8)ERCPn->Pn;
            *(OutData + j) = (u8)(ERCPn->Pn >> 8);
            *(OutData + j++) |= 0x80; 			//;by zrt 20130924			
            EW.MoveByte = 'M'; 
            MovNByte(11, (u8*)OutData + j, ERCPn->ERC29.Data);
            EW.MoveByte = 0;  
            j += 11;
            break;   
        }
        default:
        {   
            break;
        }
    }
    
    return j;
}


/*----------------------------------------------------------------------------*
 |  事件计数器加1
 *----------------------------------------------------------------------------*/
void ERC_Add(u8 ERC, u8 status)
{
    u8 ch1, ch2;

    if (ERC <= 8)
    {
        ch1 = ZDP.F9.Rating[0];
    }
    else if (ERC <= 16)
    {
        ch1 = ZDP.F9.Rating[1];
        ERC = ERC - 8;
    }
    else if (ERC <= 24)
    {
        ch1 = ZDP.F9.Rating[2];
        ERC = ERC - 16;
    }
    else if (ERC <= 32)
    {
        ch1 = ZDP.F9.Rating[3];
        ERC = ERC - 24;
    }
    ch2 = 0x01;
    ch2 <<= ERC - 1;
    if(ch1 &ch2)
    {

        ZDS.F7[0]++; //;重要事件加1
       
        if (ZDS.F7[0] != ERC1.Index % ERCReadNum) //;防错
        {
            ZDS.F7[0] = ERC1.Index % ERCReadNum;
        }
        SaveData_FM24(1, E2R_EC1, E2R_EC1 + E2R_ByteNum, ZDS.F7); //;保存事件计数器
    }
    else
    {
        ZDS.F7[1]++; //;一般事件加1
        if (ZDS.F7[1] != ERC2.Index % ERCReadNum)
        {
            ZDS.F7[1] = ERC2.Index % ERCReadNum;
        }
        SaveData_FM24(1, E2R_EC2, E2R_EC2 + E2R_ByteNum, ZDS.F7 + 1); //;保存事件计数器
    }
}


/* 
********************************************************************************
* 函 数 名：ERC_Deal_Save
* 功    能：保存事件
* 参    数：
* 返    回：true--事件处理完毕。false--还有事件需要处理。
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：2011-06-09 10:48:18
********************************************************************************
*/
void ERC_Deal_Save(void)
{
    u8 R, Len, ERCNo,i;
    u8 status;
//;    char ERCData[100];	
    char ERCData[100]={0};   //by zrt 20130907,
    
    for(i = 0; i <= PNMAXNUM; i++ )
    {
        ERCNo = ERC_Find(i, 1, &status); //;找出要记录的事件
        if(ERCNo != 0xFF)
        {
            Len = ERC_DataPre(i, ERCNo, ERCData); //;配置好发生事件的数据
            R = ZDP.F9.Rating[(ERCNo -1) / 8] & (0x01 << ((ERCNo -1) % 8));  //;事件重要性等级标志
            R = R && 1;  //;R = 1时表示事件重要，R = 0为一般事件
            ERC_Save(Len, ERCData, R);  //;保存事件
            ERC_Add(ERCNo, status);   //;事件计数器加1
            ERC_Finish(i, ERCNo, 1); //;事件完成
        }
    }
}


/*----------------------------------------------------------------------------*
 |  处理事件短信发送
 |  return:  true 该事件处理完成  false 该事件没有处理完成
 *----------------------------------------------------------------------------*/
void ERC_Deal_SMS(void)
{        
  #ifdef  ERC_SMS    
    u8 ERCNo;
    u8 status;
    u32 *Addr32;
    struct PoPStr  *PnP;
    
    if (!(GprsC.State & _SetUp))
    {
        return; //;GPRS模块未启动
    }
    
    if (SMS.Status == idle)
    {
        if (SMS.Receiver == 0)
        {
            SMS.Receiver = 'A';
        }
        
        if (SMS.Receiver == 'A')
        {
            ERCNo = ERC_Find(SMS.Pn + 1, 3, &status);                                        //;查找测量点事件
            if (ERCNo != 0xFF)                                                      //;当前有事件发生且要发送给管理员
            {
                SMS.ERCNo = ERCNo;
                SMS.Status = 'F';                                                   //;处理状态为‘找到’
                SMS.Index = 0;
            }
        }
        else if (SMS.Receiver == 'U')
        {
            ERCNo = ERC_Find(SMS.Pn + 1, 4, &status);                                    //;查找事件
            if (ERCNo != 0xFF)                                                  //;当前有事件发生且要发送给用户
            {
                SMS.ERCNo = ERCNo;
                SMS.Status = 'F';                                               //;状态为‘找到’
                SMS.Index = 0;
            }
        }
    }
    
    if (SMS.Status == idle)                                                     //;当前测量点没找到事件
    {
        SMS.Pn++;                                                               //;指向下一个测量点
        SMS.ERCNo = 0;
        SMS.Index = 0;
        SMS.Receiver = 0;
        if ((SMS.Pn + 1) >= PnMaxNum)
        {
            SMS.Pn = 0;
            PLC.ERC |= 0x04;                                                    //;短信发送完成
        }
    }
    
    if (SMS.Status == 'F')                                                      //;找出接收者的手机号码
    {
        Addr32 = (u32*)(*(PnDTAddr + SMS.Pn));
        PnP = (struct PoPStr*)(*(Addr32 + _PnP));
        if (SMS.Receiver == 'A')
        {    
            if (SMS_NumCheck(PnP->F32.Admi_MobNo1 + SMS.Index * 8) == true) 
            {
                EW.MoveByte = 'M';
                MovNByte (8, SMS.PhoneNo, PnP->F32.Admi_MobNo1 + SMS.Index * 8);
                EW.MoveByte = 0;
                SMS.Status = 'E';                                               //;找到接收者的号码，进入‘发送’状态
            }
        }
        else if (SMS.Receiver == 'U')
        {
            if (SMS_NumCheck(PnP->F32.User_MobNo1 + SMS.Index * 8) == true) 
            {
                EW.MoveByte = 'M';
                MovNByte (8, SMS.PhoneNo, PnP->F32.User_MobNo1 + SMS.Index * 8);
                EW.MoveByte = 0;
                SMS.Status = 'E';                                               //;找到接收者的号码，进入‘发送’状态
            }
        }
    }
    
    if (SMS.Status == 'E')  
    {
        PLC.ERC &= (~0x04); //;未处理完
    }
    
    if ((SMS.Status == 'S') || (SMS.Status == 'F'))                             //;发送完成或者没找到手机号码
    {
        SMS.Index++;                                                            //;指向下一个号码地址
        if (SMS.Index > 4)
        {
            SMS.Index = 0;
            SMS.Status = 0;
            if (SMS.Receiver == 'A')
            {
                ERC_Finish(SMS.Pn + 1, SMS.ERCNo, 3);
                SMS.Receiver = 'U';                                             //;接收者为用户
            }
            else if (SMS.Receiver == 'U')
            {
                ERC_Finish(SMS.Pn + 1, SMS.ERCNo, 4);
                SMS.Receiver = 0;                    
            }
        }
        else
        {
            SMS.Status = 'F';                                                  //;继续找出接收人的号码
        }
    }
  
  #endif
}


/*----------------------------------------------------------------------------*
 |  控制事件的上送
 *----------------------------------------------------------------------------*/
u8 ERC_Deal_Up(void)
{          
//;  u8 status;
//;  u8 num;
    
    if((ERCUp.Status == 'L') || (ERCUp.Status == 'G'))
    {
#ifdef LocalUpERC  
    ;
#else 
    if(!(GprsC.State & _HadLogin))
    {
        ERCUp.Status = 0;
        return true; //;GPRS模块未登陆主站
    }
#endif 
        ERCUp.OVT++;
        if(ERCUp.OVT > 30)  
        { 
            ERCUp.OVT = 0;                                //;超时
            ERCUp.Status = 'S';
        }
    }
    
    if(ERCUp.Status == 'S')  //;发送完成
    {
        if(ZDP.F1.CON & 0x04)  //;如果主动上报重要事件需要确认
        {
            u16 OVT, temp;                                                                
            u8  RNum;
            
            temp = *((u16*)ZDP.F1.OVT_RNum);
            RNum = (temp & 0x3000) >> 12; //;最大重发次数
            if (RNum != 0)  //;重发次数不为零
            {
                OVT = temp & 0xFFF; //;允许主站响应超时时间
                if (OVT == 0)
                {
                    OVT = 1;
                }
                //;OVT *= 4;
                ERCUp.WatiAns++;
                if (ERCUp.WatiAns > OVT)
                {
                    ERCUp.WatiAns = 0x00;
                    ERCUp.SendNum++;
                    if (ERCUp.SendNum < RNum)
                    {
                        ERCUp.Status = 0;
                        ERCUp.WatiAns = 0;
                        #ifdef LocalUpERC 
                        ERCUp.Status = 'L';  //;通过本地通道上送                        
                        #else 
                        ERCUp.Status = 'G';  //;通过远程通道上送
                        #endif 
                        ERCUp.ReSedFlag = true;
                    }
                    else
                    {
                        ERCUp.SendNum = 0x00;                      
                        ERCUp.Status = 'R';
                    }           
                }
            }
            else
            {
                ERCUp.Status = 'R';
            }
        }
        else
        {
            ERCUp.Status = 'R';
        }
    }
    
    if(ERCUp.Status == 'R') //;收到正确应答
    {
        ERCUp.Status = 0;
        ERCUp.WatiAns = 0;
        ERCUp.UpIndex = ERCUp.SendERCNum;
        SaveData_FM24(1, E2R_ERCUP, E2R_ERCUP + E2R_ByteNum, &ERCUp.UpIndex); //;保存事件计数器
    }
    
    if (ERCUp.Status == idle) //;当前没有测量点事件处理
    {
#ifdef LocalUpERC  
    ;
#else 
    if(!(GprsC.State & _HadLogin))
    {
        ERCUp.Status = 0;
        return true; //;GPRS模块未登陆主站
    }
#endif         
        if(ERCUp.UpIndex != ZDS.F7[0])
        {
//;            ERCUp.ERCNo = num; //;找出要上送的事件
//;            ERCUp.PmStat = status;
          #ifdef LocalUpERC 
            ERCUp.Status = 'L';  //;通过本地通道上送
          #else 
            ERCUp.Status = 'G';  //;通过远程通道上送
          #endif 
          ERCUp.ReSedFlag = false;
        }
        else
        {
            return true;
        }
    }   
    
    return false;
}




/* 
********************************************************************************
* 函 数 名：ERCPn_ParaJudge
* 功    能：ERC8、ERC13判断
* 参    数：测量点号
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void ERCPn_ParaJudge(u16 index)
{
    u32     *Addr32;
    struct  PointDataStr *PnD;
    struct  ERCPnData  *ERCPn;
    
    if(index > 0)
    {
        index -= 1;
    }
    Addr32 = (u32*)(*(PnDTAddr + index));
    PnD = (struct PointDataStr*)(*(Addr32 + _PnD));
    ERCPn = (struct ERCPnData*)(*(Addr32 + _ERCPn));
   
    ERCPn->Pn = *((u16*)ZDP.F10.Meter[index].Point);
    ERCPn->OTime[0] = TBCD.Min;   
    ERCPn->OTime[1] = TBCD.Hour;  
    ERCPn->OTime[2] = TBCD.Day;   
    ERCPn->OTime[3] = TBCD.Month; 
    ERCPn->OTime[4] = TBCD.Year;  
                                  
    ERC8_Judge (PnD, ERCPn);
    ERC13_Judge(PnD, ERCPn);
}

/* 
********************************************************************************
* 函 数 名：ERCPn_EnergyJudge
* 功    能：ERC27、ERC29
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void ERCPn_EnergyJudge(u16 index)
{
    u8  FLY;
    u32 *Addr32;
    u32 NEP,OEP;
    struct  PointDataStr *PnD;
    struct  ERCPnData  *ERCPn;
    
  
    Addr32 = (u32*)(*(PnDTAddr + index));
    PnD = (struct PointDataStr*)(*(Addr32 + _PnD));
    ERCPn = (struct ERCPnData*)(*(Addr32 + _ERCPn));
   
    ERCPn->Pn = *((u16*)ZDP.F10.Meter[index].Point);
    ERCPn->OTime[0] = TBCD.Min;   
    ERCPn->OTime[1] = TBCD.Hour;  
    ERCPn->OTime[2] = TBCD.Day;   
    ERCPn->OTime[3] = TBCD.Month; 
    ERCPn->OTime[4] = TBCD.Year;  

    FLY = BCD1ToU8(&ZDP.F59.MeterFly); //;电能飞走闸值
    NEP = BCD4ToU32(PnD->R9010 + 1);  //;当前有功总电量示值
    OEP = BCD4ToU32(PnD->O9010 + 1);  //;上一分钟有功总电量示值
    
    if(NEP < OEP) //;电能表示度下降
    {
        if(ERCPn->ERC27.Status[0] == 0x01) //;上次事件未处理完
        {
            return;
        }
        ERCPn->ERC27.Status[0] = 0x01;        //; 电能表示度下降
        ERCPn->ERC27.Data[0] = PnD->O9010[0]; //;下降后前向有功电能示值
        ERCPn->ERC27.Data[1] = PnD->O9010[1];
        ERCPn->ERC27.Data[2] = PnD->O9010[2];
        ERCPn->ERC27.Data[3] = PnD->O9010[3];
        ERCPn->ERC27.Data[4] = PnD->O9010[4];
        ERCPn->ERC27.Data[5] = PnD->R9010[0]; //;下降后正向有功电能示值
        ERCPn->ERC27.Data[6] = PnD->R9010[1];
        ERCPn->ERC27.Data[7] = PnD->R9010[2];
        ERCPn->ERC27.Data[8] = PnD->R9010[3];
        ERCPn->ERC27.Data[9] = PnD->R9010[4];
        
        
    }
    else if(NEP > OEP)  //;电能表飞走
    {
        if(FLY != 0)
        {
            if((NEP - OEP) > FLY)
            {
                if(ERCPn->ERC29.Status[0] == 0x01) //;上次事件未处理完
                {
                    return;
                }
                ERCPn->ERC29.Status[0] = 0x01;
                ERCPn->ERC29.Data[0] = PnD->O9010[0];//;飞走后前向有功电能示值
                ERCPn->ERC29.Data[1] = PnD->O9010[1];
                ERCPn->ERC29.Data[2] = PnD->O9010[2];
                ERCPn->ERC29.Data[3] = PnD->O9010[3];
                ERCPn->ERC29.Data[4] = PnD->O9010[4];
                
                ERCPn->ERC29.Data[5] = PnD->R9010[0];//;飞走后正向有功电能示值
                ERCPn->ERC29.Data[6] = PnD->R9010[1];
                ERCPn->ERC29.Data[7] = PnD->R9010[2];
                ERCPn->ERC29.Data[8] = PnD->R9010[3];
                ERCPn->ERC29.Data[9] = PnD->R9010[4];
                ERCPn->ERC29.Data[10] =  ZDP.F59.MeterFly;
            }
        }
    }
    
    PnD->O9010[0] = PnD->R9010[0]; //;保存上一次当前正向有功总电能
    PnD->O9010[1] = PnD->R9010[1];
    PnD->O9010[2] = PnD->R9010[2];
    PnD->O9010[3] = PnD->R9010[3];
    PnD->O9010[4] = PnD->R9010[4];
}

/*----------------------------------------------------------------------------*
 |  每250ms处理一次事件
 *----------------------------------------------------------------------------*/
u8 ZD_ERC(void)
{
    u8 ch1;
    ERC_Deal_Save();   
    ch1 = ERC_Deal_Up(); 
    ERC_Deal_SMS();
    return ch1;
}
