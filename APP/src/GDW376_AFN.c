/*
********************************************************************************
*  文 件 名：GDW376_AFN.c
*
*  文件描述：GDW376.1应用功能层处理相关函数  终端地址 F10
*
*  所用芯片：
*
*  创 建 人：023 
*
*  版 本 号：
*    
*  修改记录：2012-03-26 11:15:34 增加控制命令处理F38                                                                           
********************************************************************************
*/
#include "GDW376_CFG.h"
#include "GDW376_MMU.h"

/* 
********************************************************************************
* 函 数 名：Find_Fn
* 功    能：根据DT1和DT2找出对应的Fn
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
/*
u8 Find_Fn(u8 DT1, u8 DT2)
{
    u8 Fn, j;

    if(DT2 <= 30)
    {
        Fn = DT2 * 8;
    }
    else
    {
        return 0xFF; //;非法的情况
    }
    
    if(DT1 == 0x00)
    {
        return 0xFF; //;非法的情况
    }

    for(j = 0; j < 8; j++)
    {
        if(DT1 & 0x01)
        {
            if(DT1 & 0xFE)
            {
                return 0xFF; //;DT1只能有一位为1
            }

            break;
        }
        Fn++;
        DT1 >>= 1;
    }
    Fn++;
    return (Fn);
}
*/
u16 Find_Fn(u8 DT1, u8 DT2)		//;qiu20131120
{
	u16 StartFn = 0;
	u16 RetFn = 0xFFFF;	//;返回的数据，[(有效个数 << 8) + (起始DT)]
	u8 FnNum = 0;
	u8 i;
	
	if(DT1 == 0x00)		//;DT1不允许出现0的情况
	{
		return 0xFFFF;	//;非法的情况
	}
	
	for(i = 1; i < 9; i++)
	{
		if(DT1 & 0x01)	//;有效的标记位
		{
			if (StartFn == 0)	//;还未记录起始Fn
			{
				if(DT2 <= 30)
				{
					StartFn = DT2 * 8;
					StartFn += i;
				}
				else
				{
					return 0xFFFF; //;非法的情况
				}
			}
			FnNum ++;
		}
		DT1 >>= 1;
	}
	RetFn = (FnNum << 8) | (StartFn & 0xff);
	return (RetFn);
}

/* 
********************************************************************************
* 函 数 名：Find_FnI
* 功    能：根据DT1找出对应的Fn在组内的序号
* 参    数：
* 返    回： 0xFF---非法情况
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
u8 Find_FnI(u8 DT1)
{
    u8 FnI = 0, j;

    if(DT1 == 0x00)
    {
        return 0xFF; //;非法的情况
    }
    for(j = 0; j < 8; j++)
    {
        if(DT1 & 0x01)
        {
            if(DT1 & 0xFE)
            {
                return 0xFF; //;非法的情况,DT1只能有一位为1
            }
            break;
        }
        FnI++;
        DT1 >>= 1;
    }
    return (FnI);
}

/* 
********************************************************************************
* 函 数 名：Find_Pn
* 功    能：根据DA1和DA2找出对应的Pn
* 参    数：
* 返    回：0xFFFF---非法情况;0---终端;其他---测量点 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：2011-09-07 20:25:04
********************************************************************************
*/
u16 Find_Pn(u8 DA1, u8 DA2)
{
    u8  j;
    u16 Pn;

    if((DA1 == 0) && (DA2 == 0))  //;终端
    {
        return 0;
    }       
    else if((DA1==0) || (DA2==0))   //;DA1和DA2不能单独为0
    {
        return 0xFFFF;
    }
    
    Pn = (DA2 - 1) * 8;     
    for(j = 0; j < 8; j++)
    {
        if(DA1 & 0x01)
        {
            if(DA1 & 0xFE)
            {
                return 0xFFFF; //;DA1只能有一位为1
            }
            break;
        }
        Pn++;
        DA1 >>= 1;
    }
    Pn++;
    return (Pn);
}

/* 
********************************************************************************
* 函 数 名：Find_DA
* 功    能：根据测量点号计算出DA2，DA1
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：2011-09-07 20:25:02
********************************************************************************
*/
void Find_DA(u16 Pn, char * DA)
{
    u8 da1 = 1, da2 = 1;
    
    if(Pn > 2040)
    {
        return;
    }
    if(Pn == 0x00)  //;终端
    {
        *DA = 0x00;
        *(DA + 1) = 0x00;
        return;
    }
    
    Pn -= 1;    
    da1 <<= (Pn % 8);
    da2 = (Pn / 8) + 1;
    
    *DA = da1;
    *(DA + 1) = da2;
    
}


/* 
********************************************************************************
* 函 数 名：AFN04H_SET
* 功    能：设置完参数后需要处理Flash保存和一些RAM参数的初始化
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：2011-09-07 20:42:57
*           2011-09-29 15:20:53  增加流量限制修改后的处理
********************************************************************************
*/
void AFN04H_SET(u16 Pn, u8 Fn)
{
    s16 index;
  
    if((Fn>=1) && (Fn<=10))
    {
        EW.CSB1 = 0xC1; //;如果参数1区有变动，保存参数1区
        if(Fn == 3) //;GPRS参数改变，需重新连接网络
        {                                              
            GprsC.Re_Connect_states = 'E';
            //GprsC.ReConnect = true;                
        }
        else if(Fn == 8)
        {
            if((ZDP.F8.WorkMode&0x03) == 0x03)
            {
                GprsLC.CHKLink = true; //;需要检测是否是在线时段
            }
        }
    }
    else if((Fn>=10) && (Fn<=23))
    {
        EW.CSB2 = 0xC2; 
    }
    else if(((Fn>=33) && (Fn<=39)) || ((Fn>=57) && (Fn<=61)))
    {
        EW.CSB3 = 0xC3; 
	 if(33 == Fn) //;端口号定为1 by zrt 20131022
	 {
	     ZDP.F33.PortP[0].ComPort = 1;
	     ZDP.F33.PortP[1].ComPort = 1;
	 }
        if(36 == Fn) //;流量限制
        {
            FluxLimValue = *((u32*)ZDP.F36);
            if(0x00 == FluxLimValue)
            {
                FluxLimValue = 0xFFFFFFFF;  //;流量不限制，则给最大值
            } 
        }
    }
    else if(((Fn>=25) && (Fn<=31)) || ((Fn>=73) && (Fn<=76)))
    {
        /*index = ZD_Pn_Find(Pn);
        if(index < 10)
        {
            EW.CSB4 |= (0x0001 << index);          
        }
        */
        index = ZD_Pn_Find(Pn);//;--20141225
        if(index != -1)
        {
            EW.CSB4 |= (0x0001 << index);         
        }
    }
    else if(Fn == 65)
    {
        EW.CSB5 = 0xC5;
        EW.CSB9 = 0xC9;
        Task_Init(Pn, Fn); //;;任务参数改变后要重新配置任务上送参数
    }
    else if((Fn>=66) && (Fn<=68))
    {
        EW.CSB6 = 0xC6;
        EW.CSB10 = 0xCA;
        Task_Init(Pn, Fn);
    }
    else if(((Fn>=41) && (Fn<=48)) || ((Fn>=81) && (Fn<=83)) || (Fn == 49))
    {
        EW.CSB7 = 0xC7;
    }
    else if((Fn>=89) && (Fn<=101))
    {
        EW.CSB8 = 0xC8;
    }
}

/* 
********************************************************************************
* 函 数 名：AFN04H_PreData
* 功    能：为应答AFN=0x04 的命令准备数据
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
u8 AFN04H_PreData(char *S_Buf, u16 *Ind_S_Adr, char *R_Buf, u16 *Ind_R_Adr)
{
    u8 i, SET = false, temp;
    u8 Fn, FnI, DA1, DA2, DT1, DT2;
    u8 *Adr8;
    u8 T, N, L, X, N1;
    u16 Ind_S, Ind_R, Pn, Len, Len1;
    u32 *Adr32;

    Ind_R = *Ind_R_Adr; //;输入缓冲区位置 
    Ind_S = *Ind_S_Adr; //;输出缓冲区位置

    DA1 = *(R_Buf + Ind_R + 0);
    DA2 = *(R_Buf + Ind_R + 1);
    DT1 = *(R_Buf + Ind_R + 2);
    DT2 = *(R_Buf + Ind_R + 3);

    Pn = Find_Pn(DA1, DA2);     //;;计算出Pn--信息点
    FnI = Find_FnI(DT1);        //;;计算出Fn在组内的位置
    Fn = Find_Fn(DT1, DT2);     //;;计算出Fn--信息类

    if((Fn==0xff) || (FnI==0xff) || (DT2>13))
    {
        return 0xFF; //; 终端无法识别该信息类
    }
 
    Adr8 = (u8*)ZDCSZU + Fn * 8;    //;找出对应的参数设置地址
    T =  *Adr8;                 //;存储类型
    N = *(Adr8 + 1);            //;支持的单元数
    L = *(Adr8 + 2);            //;每个单元长度
    X = *(Adr8 + 3);

    if (N == 0)   //;支持设置参数的总数据长度
    {
        Len = L;
    }
    else
    {
        Len = (N * L) + X; //;;多个数据单元
    }
    
    if((Len==0) || ((T!=1) && (T!=2) && (T!=3)))
    {
        return 0xFF;  //;;终端不支持该参数
    }

    *(S_Buf + Ind_S++) = *(R_Buf + Ind_R++); //;;写入标识
    *(S_Buf + Ind_S++)=  *(R_Buf + Ind_R++);
    *(S_Buf + Ind_S++)=  *(R_Buf + Ind_R++);
    *(S_Buf + Ind_S++)=  *(R_Buf + Ind_R++);

    if(T == 1)      //;终端参数
    {
        if(N != 0) //;多个数据单元类型
        {
            N1 = *(R_Buf + Ind_R);  //;读出需要设置的单元数
            if(Fn == 2)
            {
                N1 &= 0x7F; //;F2第一个字节的最高位为允许或禁止转发标志
            }
            Len1 = N1 * L + X; //;主站要求设置参数的长度
        }
        else  //;参数长度固定
        {
            if(Fn == 7)
            {
                Len1 = 21; //;到用户名长度
            }
            else
            {
                Len1 = Len; //;主站要求设置参数的长度
            }
        }
        
        Adr32 = (u32*)(Adr8 + 4);
        
        //;主站要求设置参数的长度大于终端支持的长度,或信息点不支持
        if((Len1>Len) || (Pn!=0) || (*Adr32==0)) 
        {
            *(S_Buf + Ind_S++) = 1; //;设置失败
            *Ind_R_Adr = Ind_R + Len1;
            *Ind_S_Adr = Ind_S;
            return false;
        }
        
        Adr8 = (u8*)(*Adr32);//;指向相应参数地址
        if(10 == Fn)    //;做特殊处理 qiu20140416
        {
            u8 F10_Result = true;
            if (((R_Buf[Ind_R + 6] & 0x7f) == 1)||((R_Buf[Ind_R + 6] & 0x7f) == 0x61))
            {
               F10_Result = true;     //;波特率为默认0或2400bps,端口为1则设置成功，否则设置失败--20141229
            }
            else
            {
            	F10_Result = false;     //;通信口必须为1,波特率必须为2400bps
            }
            
            if (R_Buf[Ind_R + 0] != 1)
            {
                F10_Result = false;     //;测量点总数必须为1
            }
            if (R_Buf[Ind_R + 2] != 1)
            {
                F10_Result = false;     //;交流采样装置序号必须为1
            }
            if (R_Buf[Ind_R + 4] != 1)
            {
                F10_Result = false;     //;测量点号必须为1
            }
            
            if(R_Buf[Ind_R + 7] != GYType2007)
            {
                F10_Result = false;     //;测量点规约必须为07
            }
            if (false == F10_Result)
            {
                *(S_Buf + Ind_S++) = 1; //;设置失败
                *Ind_R_Adr = Ind_R + Len1;
                *Ind_S_Adr = Ind_S;
                return false;
            }
        }
        
        for(i=0; i<Len1; i++) //;把修改的参数放入对应的信息类
        {
            if(false == SET) //;参数为发生变化的时候才去判断
            {
                if(*(Adr8+i) != *(R_Buf+Ind_R))
                {
                    SET = true; //;参数发生改变
                }
            }
            *(Adr8 + i) = *(R_Buf + Ind_R++);
        }
        
        if(Fn == 7)
        {
            temp = *(Adr8 + 20) + Len1; //;用户名长度
            for(i = Len1; i < temp; i++)   //;用户名
            {
                if(false == SET) //;参数为发生变化的时候才去判断
                {
                    if(*(Adr8+i) != *(R_Buf+Ind_R))
                    {
                        SET = true; //;参数发生改变
                    }
                }
                *(Adr8 + i) = *(R_Buf + Ind_R++);
            }
/*
            temp = *(R_Buf + Ind_R++);
            *(Adr8 + 41) = temp;
            temp += 42;
            for(i = 42; i < temp; i++)
            {
                if(false == SET) //;参数为发生变化的时候才去判断
                {
                    if(*(Adr8 + i) != *(R_Buf+Ind_R))
                    {
                        SET = true; //;参数发生改变
                    }
                }
                *(Adr8 + i) = *(R_Buf + Ind_R++);
            }
            *(Adr8 + 62) = *(R_Buf + Ind_R++); //;终端侦听端口
            *(Adr8 + 63) = *(R_Buf + Ind_R++);            
*/
//by zrt 20130618
            int pass_word_len =0;
            pass_word_len = *(Adr8 + temp) = *(R_Buf + Ind_R++);  //;密码长度
            for(i = temp+1; i < temp+1+pass_word_len; i++)
            {
                if(false == SET) //;参数为发生变化的时候才去判断
                {
                    if(*(Adr8 + i) != *(R_Buf+Ind_R))
                    {
                        SET = true; //;参数发生改变
                    }
                }
                *(Adr8 + i) = *(R_Buf + Ind_R++);
            }
            
            *(Adr8 + temp+pass_word_len+1) = *(R_Buf + Ind_R++); //;终端侦听端口
            *(Adr8 + temp+pass_word_len+2) = *(R_Buf + Ind_R++);
            
        }
        
        *(S_Buf + Ind_S++) = 0;//;;设置成功
    }
    else if(T == 2) //;测量点参数
    {
        u16 OffSet;
        u32 *Addr32;
        struct PoPStr  *PnP;

        if(N != 0)
        {
            N1 = *(R_Buf + Ind_R);
            Len1 = N1 * L + X;
        }
        else
        {
            Len1 = Len;
        }
        
        if(Pn == 0)
        {
            Pn = 1; //;防止主站不规范
        }

        Adr32 = (u32*)(Adr8 + 4);
        if((Len >Len) || (ZD_Pn_Find(Pn)<0))  //;--20141225主站要求设置参数的长度大于终端支持的长度，或信息点不支持
        {  
            *(S_Buf + Ind_S++) = 1; //;;设置失败
            *Ind_R_Adr = Ind_R + Len1;
            *Ind_S_Adr = Ind_S;
            return false;
        }

        OffSet = *(Adr32 + 0);
        Addr32 = (u32*)(*(PnDTAddr + ZD_Pn_Find(Pn)));
        PnP = (struct PoPStr*)(*(Addr32 + _PnP));
        Adr8 = PnP->F25.UMul + OffSet;
        
        for(i=0; i<Len1; i++)
        {
            if(false == SET) //;参数发生改变之后不再做比较
            {
                if(*(Adr8+i) != *(R_Buf+Ind_R))
                {
                    SET = true;
                }
            }
            *(Adr8 + i) = *(R_Buf + Ind_R++);
        }
        *(S_Buf + Ind_S++) = 0;
    }
    else if(T == 3)  //;任务参数
    {
        if(Pn == 0)
        {
            Pn = 1;
        }
        
        if((Fn == 67) || (Fn == 68))
        {
            Len = 1; //;;任务启动停止设置 ---@2
            Len1 = 1;
        }
        else 
        {
        		N1 = *(R_Buf + Ind_R + 8);
        		Len1 = N1 * L + 9;
        }
        
        /*if((Len1>Len) || ((Fn==65) && (Pn>TASK1NUM)) ||
            ((Fn ==66) && (Pn>TASK2NUM)))*/
        if((Len1>Len) || ((Fn==65) && (Pn>TASK1NUM)) ||
            ((Fn ==66) && (Pn>TASK2NUM))||((Fn ==67) && (Pn>TASK1NUM))\
            ||((Fn ==68) && (Pn>TASK2NUM)))    //;任务配置最多只有8个,20141230
        {
            //;;主站要求设置参数的长度大于终端支持的长度，或信息点不支持
            *(S_Buf + Ind_S++) = 1; //;;设置失败
            *Ind_R_Adr = Ind_R + Len1;
            *Ind_S_Adr = Ind_S;
            return false;
        }

        Adr32 = (u32*)*(u32*)(Adr8 + 4);
        Adr8 = (u8*)((u8*)Adr32 + (Pn-1) * Len);
        for(i=0; i<Len1; i++)  //;;  --@2
        {
            if(false == SET)
            {
                if(*(Adr8 + i) != *(R_Buf + Ind_R))
                {
                    SET = true;
                }
            }
            *(Adr8 + i) = *(R_Buf + Ind_R++);
        }
        *(S_Buf + Ind_S++) = 0; //;;--@2
    }
 
    if(SET == true)
    {
        if(ERCP0.ERC3.Num < 8)
        {
            ERCP0.ERC3.Data[ERCP0.ERC3.Num][0] = DA1;
            ERCP0.ERC3.Data[ERCP0.ERC3.Num][1] = DA2;
            ERCP0.ERC3.Data[ERCP0.ERC3.Num][2] = DT1;
            ERCP0.ERC3.Data[ERCP0.ERC3.Num][3] = DT2;
            ERCP0.ERC3.Num++;
        }
        else
        {
            ERCP0.ERC3.Num = 0;
        }

        AFN04H_SET(Pn ,Fn);
        
    }

    *Ind_R_Adr = Ind_R;
    *Ind_S_Adr = Ind_S;
    return true;
}


/* 
********************************************************************************
* 函 数 名：GDW_AFN04H
* 功    能：对 AFN=0x04 的帧进行处理   设置参数
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
u16 GDW_AFN04H(char *R_Buf, char *S_Buf)
{
    u16 i, len, Ind_S, Ind_R, Status;
    u8 right_num = 0;
    u8 error_num = 0;
    u8 CRC1 = true, SEQ;
    //s32 long1=0;

    SEQ = *(R_Buf + 13); //;帧序列域
    
    len = *(R_Buf + 1) + (*(R_Buf + 2) << 8);
    len >>= 2;   //;用户数据区的总长度
    len -= 8;    //;数据单元长度=len-1(控制域)-5(地址域)-1(AFN)-1(SEQ)
    len -= 16; //;PW长度
    if(SEQ & 0x80) //;减去TpV
    {
        len -= 6;
    }
    /* //;增加此程序，建三江软件会引起死循环 20141225
    if(SEQ & 0x80)                                   //;带有时间标签
    {
        len -= 6;                                   //;去掉时间标签长度
        if(*(R_Buf+19+len+16)>0)
        {
           long1=Time_distance(&TBCD.Sec, (u8*)(R_Buf + 15 + len+16));   //;tpv允许发送传输延时判断zrt20140107
           if (long1 < -(*(R_Buf + 19 + len+16)*60) || long1 > (*(R_Buf+19+len+16)*60) )
	   {
              return 0;
           }
        }
    }
    */ 		

    if(CRC1 == false)
    {
        return 0;  //;认证失败
    }

    Ind_S = 19;
    ERCP0.ERC3.Num = 0; //;ERC3的初始数据长度为0，当设置完成后仍为6，说明没有参数改变
    for(Ind_R=14; Ind_R<len+14; )
    {        
        Status = AFN04H_PreData(S_Buf, &Ind_S, R_Buf, &Ind_R);
        if(Status == 0xFF) //;;无法识别信息类
        {
            break;
        }
    }

    if(ERCP0.ERC3.Num > 0)
    {
        ERCP0.ERC3.Status[0] = 0x01;      //;有参数发生改变，ERC3发生
        ERCP0.ERC3.OTime[0] = TBCD.Min;
        ERCP0.ERC3.OTime[1] = TBCD.Hour;
        ERCP0.ERC3.OTime[2] = TBCD.Day;
        ERCP0.ERC3.OTime[3] = TBCD.Month;
        ERCP0.ERC3.OTime[4] = TBCD.Year; //;写入参数更新时间
        ERCP0.ERC3.HostAdr = (*(R_Buf + 11) >> 1); //;写入启动站地址
    }

    if(!(SEQ & 0x10)) //;CON--不需要确认
    {
        return 0;
    }
    
    for(i=23; i<=Ind_S; i+=5)
    {
        if(*(S_Buf+i) == 0x00)
        {
            right_num++;
        }
        else if(*(S_Buf+i) == 0x01)
        {
            error_num++;
        }
    }

    if(right_num == 0)  //;全部错误，用全部否认来应答
    {
        Ind_S = 18;
        *(S_Buf + 16) = 0x02; //;AFN
    }
    else if(error_num == 0)  //;全部正确，用全部确认来应答
    {
        Ind_S = 18;
        *(S_Buf + 16) = 0x01; //;AFN
    }
    else //;根据数据单元标识确认和否认
    {
        *(S_Buf + 16) = 0x04; 
        *(S_Buf + 18) = 0x04; //;AFN
    }
    
    *(S_Buf + 0 ) = *(S_Buf + 5) = 0x68;
    *(S_Buf + 6 ) = 0x88;                         //;DIR=1,PRM=0,ACD=0; CID=8
    *(S_Buf + 7 ) = *(R_Buf + 7 );        //;终端地址
    *(S_Buf + 8 ) = *(R_Buf + 8 );
    *(S_Buf + 9 ) = *(R_Buf + 9 );
    *(S_Buf + 10) = *(R_Buf + 10);
    *(S_Buf + 11) = *(R_Buf + 11);
    *(S_Buf + 13) = *(R_Buf + 13) & 0x0f; //;帧序列域SEQ
    *(S_Buf + 13) |= 0x60;                 //;Tpv=0;FIR=1,FIN=1,CON=0;
    *(S_Buf + 12) = 0x00; //;应用层功能码 AFN
    *(S_Buf + 14) = 0x00;
    *(S_Buf + 15) = 0x00; //;p0
    *(S_Buf + 17) = 0x00; //;f3

    if(SEQ & 0x80)//;20131102
	{
		*(S_Buf + Ind_S++) = *(R_Buf + 14 + len + 16); //;16位密码pw
                *(S_Buf + Ind_S++) = *(R_Buf + 15 + len + 16);
                *(S_Buf + Ind_S++) = *(R_Buf + 16 + len + 16);
                *(S_Buf + Ind_S++) = *(R_Buf + 17 + len + 16);
                *(S_Buf + Ind_S++) = *(R_Buf + 18 + len + 16);
		/**(S_Buf + Ind_S++) = TBCD.Sec;
		*(S_Buf + Ind_S++) = TBCD.Min;
		*(S_Buf + Ind_S++) = TBCD.Hour;
		*(S_Buf + Ind_S++) = TBCD.Day;*/
		*(S_Buf + Ind_S++) = *(R_Buf + 19 + len + 16);;
                *(S_Buf + 13) |= 0x80;
	}  
   
    //;数据域长度
    len = Ind_S - 6;
    len *= 4;
    len += 2;  
    *(S_Buf + 1) = *(S_Buf + 3) = (u8)len;
    *(S_Buf + 2) = *(S_Buf + 4) = (u8)(len>>8);
     
    //;校验和
    *(S_Buf + Ind_S) = 0;
    for(i=6; i<Ind_S; i++)
    {
        *(S_Buf + Ind_S) += *(S_Buf + i);
    }
    
    //;结束字节
    *(S_Buf + Ind_S + 1) = 0x16;
    return (Ind_S + 2);
}

/* 
********************************************************************************
* 函 数 名：PreData_AFN09H
* 功    能：为应答AFN=0x09 的命令准备数据
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：2011-09-21 10:07:51
********************************************************************************
*/
u8 PreData_AFN09H(char *S_Buf, u16 *Ind_S_Adr, char *R_Buf, u16 *Ind_R_Adr)
{
    u8 i, Fn, FnI, DA1, DA2, DT1, DT2;
    u8 T, N, L, X, N1, Len;
    u8  *Adr8;
    u16 Ind_R, Ind_S, Pn;
    u32 *Adr32;

    Ind_R = *Ind_R_Adr;
    Ind_S = *Ind_S_Adr;
    
    DA1 = *(R_Buf + Ind_R++);
    DA2 = *(R_Buf + Ind_R++);
    DT1 = *(R_Buf + Ind_R++);
    DT2 = *(R_Buf + Ind_R++);

    Pn = Find_Pn(DA1, DA2); //;计算出Pn--信息点
    FnI = Find_FnI(DT1); //;计算出Fn--信息类
    Fn = Find_Fn(DT1, DT2); //;计算出Fn--信息类

    if((Fn==0xff) || (FnI==0xff) || (DT2!=0))
    {
        *Ind_R_Adr = Ind_R ;
        *Ind_S_Adr = Ind_S;
        return false; //; 终端无法识别该信息类
    }
    
    Adr8 = ((u8*)ZDCFGZU) + Fn * 8;    //;找出该组的首地址
    T =  *Adr8;                 //;存储类型
    N = *(Adr8 + 1);            //;支持的单元数
    L = *(Adr8 + 2);            //;每个单元长度
    X = *(Adr8 + 3);


    Len = L;

    if(Len == 0)
    {
        *Ind_R_Adr = Ind_R ;
        *Ind_S_Adr = Ind_S;
        return false; //;;终端不支持该参数
    }

    if(T == 1) //;终端参数
    {
        Adr32 = (u32*)(Adr8 + 4);
        if((*Adr32==0) || (Pn!=0)) //;;终端不支持该参数
        {
            *Ind_R_Adr = Ind_R ;
            *Ind_S_Adr = Ind_S;
            return false; //;;终端不支持该参数
        }                 
     
            
        *(S_Buf + Ind_S++) = DA1; 
        *(S_Buf + Ind_S++) = DA2; 
        *(S_Buf + Ind_S++) = DT1; 
        *(S_Buf + Ind_S++) = DT2;        
        Adr8 = (u8*)(*Adr32);
        if(N != 0)
        {
            N1 = *(Adr8 + 0);
            Len = N1 * L + X;
        }
        for(i = 0; i < Len; i++)
        {
            *(S_Buf + Ind_S++) = *(Adr8 + i);
        }
    }
    else
    {
        *Ind_R_Adr = Ind_R ;
        *Ind_S_Adr = Ind_S;
        return false; //;;终端不支持该参数
    }
    
    *Ind_R_Adr = Ind_R;
    *Ind_S_Adr = Ind_S;
    return true;
}

/* 
********************************************************************************
* 函 数 名：PreData_AFN0AH
* 功    能：为应答AFN=0x0A 的命令准备数据
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
u8 PreData_AFN0AH(char *S_Buf, u16 *Ind_S_Adr, char *R_Buf, u16 *Ind_R_Adr)
{
    u8 i, Fn, FnI, DA1, DA2, DT1, DT2, temp;
    u8 x8, y8, v8, k, z8, *p;
    u8 T, N, L, X, N1;
    u8  *Adr8;
    u16 Ind_R, Ind_S, Pn, x16, y16, v16, m, n, z16, Len,j;
    u32 *Adr32;

    Ind_R = *Ind_R_Adr;
    Ind_S = *Ind_S_Adr;
    
    DA1 = *(R_Buf + Ind_R++);
    DA2 = *(R_Buf + Ind_R++);
    DT1 = *(R_Buf + Ind_R++);
    DT2 = *(R_Buf + Ind_R++);

    Pn = Find_Pn(DA1, DA2); //;计算出Pn--信息点
    FnI = Find_FnI(DT1); //;计算出Fn--信息类
    Fn = Find_Fn(DT1, DT2); //;计算出Fn--信息类

    if((Fn==0xff) || (FnI==0xff) || (DT2>=13))
    {
        *Ind_R_Adr = Ind_R ;
        *Ind_S_Adr = Ind_S;
        return false; //; 终端无法识别该信息类
    }
    
    Adr8 = (u8*)ZDCSZU + Fn * 8;    //;找出对应的参数设置地址
    T =  *Adr8;                 //;存储类型
    N = *(Adr8 + 1);            //;支持的单元数
    L = *(Adr8 + 2);            //;每个单元长度
    X = *(Adr8 + 3);

    if(N == 0)   //;支持设置参数的总数据长度
    {
        Len = L;
    }
    else
    {
        Len = (N * L) + X;
    }
    
    if(Len == 0)
    {
        *Ind_R_Adr = Ind_R ;
        *Ind_S_Adr = Ind_S;
        return false; //;;终端不支持该参数
    }

    if(T == 1) //;;终端参数
    {
        Adr32 = (u32*)(Adr8 + 4);
        if((*Adr32==0) || (Pn!=0)) //;;终端不支持该参数
        {
            *Ind_R_Adr = Ind_R ;
            *Ind_S_Adr = Ind_S;
            return false; //;;终端不支持该参数
        }  
            
        if(Fn == 10)
        {            
            x16 = *(R_Buf + Ind_R) + ((*(R_Buf + Ind_R + 1)) << 8); //;本次查询数量
            Ind_R += 2;
            
            y16 = *(u16*)ZDP.F10.MeterNum;
            
            if(x16 > y16) //;查询数量大于实际数量
            {
                x16 = y16;
            }
            
          
            z16 = 0;  
            *(S_Buf + Ind_S++) = DA1; 
            *(S_Buf + Ind_S++) = DA2; 
            *(S_Buf + Ind_S++) = DT1; 
            *(S_Buf + Ind_S++) = DT2;   
            Ind_S += 2; //;前两个字节放实际上送的数量个数；
            
            for(m=0; m<x16; m++)
            {
                v16 = *(R_Buf + Ind_R) + ((*(R_Buf + Ind_R + 1)) << 8);
                Ind_R +=2;
                for(n=0; n<y16; n++) //;寻找需查询的对象序号
                {
                    if(*((u16*)ZDP.F10.Meter[n].Index) == v16)
                    {
                        break;
                    }
                }
                
                if(n < y16)  //;查到该序号
                {
                    z16++;
                    Adr8 = ZDP.F10.Meter[n].Index;
                    for(i=0; i<L; i++)
                    {
                        *(S_Buf + Ind_S++) = *(Adr8 + i);
                    }
                }
            }
            
            if(z16 != 0) //;找到z16个需要上送的数量个数
            {
                *(S_Buf + *Ind_S_Adr + 4) = (u8)z16;
                *(S_Buf + *Ind_S_Adr + 5) = (u8)(z16 >> 8);
            }
            else
            {
                Ind_S -= 6; //;不返回此项任何数据
            }
        }
        else if((Fn==11) || (Fn==13) || (Fn==14) || (Fn==38) ||
                (Fn==15) || (Fn==33) || (Fn==34) || (Fn==39))
        {                      
            z8 = 0;  
            *(S_Buf + Ind_S++) = DA1; 
            *(S_Buf + Ind_S++) = DA2; 
            *(S_Buf + Ind_S++) = DT1; 
            *(S_Buf + Ind_S++) = DT2;
            if((Fn==38) || (Fn==39))  //;不去判断用户大类号
            {
                *(S_Buf + Ind_S++) = *(R_Buf + Ind_R++);
            }  
            Ind_S += 1; //;前两个字节放实际上送的数量个数
             
            Adr8 = (u8*)(*Adr32); 
            x8 = *(R_Buf + Ind_R++); //;本次查询数量
            y8 = *Adr8;
            
            if(x8 > y8) //;查询数量大于实际数量
            {
                x8 = y8;
            }           
          
           
            
            for(j=0; j<x8; j++)
            {
                p = Adr8 + 1;
                v8 = *(R_Buf + Ind_R++);
                for(k=0; k<y8; k++, p += L) //;寻找需查询的对象序号
                {
                    if(*(p) == v8)
                    {
                        break;
                    }
                }
                
                if(k < y8)  //;查到该序号
                {
                    z8++;
                    p = Adr8 + 1 + k * L;;
                    for(i=0; i<L; i++)
                    {
                        *(S_Buf + Ind_S++) = *(p + i);
                    }
                }
            }
            
            if(z8 != 0) //;找到z8个需要上送的数量个数
            {
                if((Fn==38) || (Fn==39))  
                {
                    *(S_Buf + *Ind_S_Adr + 5) = z8;;
                }  
                else
                {
                    *(S_Buf+ *Ind_S_Adr + 4) = z8;
                }
            }
            else
            {
                if((Fn==38) || (Fn==39))
                {
                    Ind_S -= 6; //;不返回此项任何数据
                }
                else
                {
                    Ind_S -= 5; //;不返回此项任何数据
                }
            }
        }       
        else
        {
            
            *(S_Buf + Ind_S++) = DA1; 
            *(S_Buf + Ind_S++) = DA2; 
            *(S_Buf + Ind_S++) = DT1; 
            *(S_Buf + Ind_S++) = DT2;        
            Adr8 = (u8*)(*Adr32);
            if(N != 0)
            {
                N1 = *(Adr8 + 0);
                if(Fn == 0x02)
                {
                    N1 &= 0x7F;
                }
                Len = N1 * L + X;
            }
            
            if(Fn == 7)
            {
                Len = 21;
            }
            
            for(i = 0; i < Len; i++)
            {
                *(S_Buf + Ind_S++) = *(Adr8 + i);
            }
            
            if(Fn == 7)
            {
/*
                temp = *(Adr8 + 20) + Len; //;用户名长度
                for(i = Len; i < temp; i++)
                {                
                    *(S_Buf + Ind_S++) = *(Adr8 + i);
                }
                
                temp = *(Adr8 + 41);
                temp += 41;
                for(i = 41; i < temp; i++)
                {
                    *(S_Buf + Ind_S++) = *(Adr8 + i);
                }
*/ 
//by zrt 20130717

                temp = *(Adr8 + 20) + Len;
                for(i = Len; i < temp; i++)   //;用户名
                {
                    *(S_Buf + Ind_S++) = *(Adr8 + i);
                }
                int pass_word_len = 0 ;
                pass_word_len = *(S_Buf + Ind_S++) = *(Adr8 + temp); //;密码长度
                for(i = temp + 1 ; i < temp+1+pass_word_len ; i++)
                {
                    *(S_Buf + Ind_S++) = *(Adr8 + i);
                }
                *(S_Buf + Ind_S++) = *(Adr8 + temp+pass_word_len+1); //;终端侦听端口
                *(S_Buf + Ind_S++) = *(Adr8 + temp+pass_word_len+2);;
                
            }
        }
    }
    else if(T == 2) //;;测量点参数
    {
        u16 OffSet;
        u32 *Addr32;
        struct PoPStr  *PnP;

        Adr32 = (u32*)(Adr8 + 4);
        
        if(ZD_Pn_Find(Pn) < 0) //;;终端不支持该参数
        {
            *Ind_R_Adr = Ind_R ;
            *Ind_S_Adr = Ind_S;
            return false; //;;终端不支持该参数
        }
        
        *(S_Buf + Ind_S++) = DA1; 
        *(S_Buf + Ind_S++) = DA2; 
        *(S_Buf + Ind_S++) = DT1; 
        *(S_Buf + Ind_S++) = DT2; 
         
        //;找出测量点参数所在位置
        OffSet = *(Adr32 + 0);
        Addr32 = (u32*)(*(PnDTAddr + ZD_Pn_Find(Pn)));
        PnP = (struct PoPStr*)(*(Addr32 + _PnP));
        Adr8 = PnP->F25.UMul + OffSet; 
        if(N != 0)
        {
            N1 = *(Adr8 + 0);
            Len = N1 * L + X;
        }
        
        for(i = 0; i < Len ; i++)
        {
            *(S_Buf + Ind_S++) = *(Adr8 + i);
        }
    }
    else if(T == 3) //;任务参数
    {
        
        
        if((Adr32==0) || ((Fn==65) && (Pn>TASK1NUM)) ||
            ((Fn==66) && (Pn>TASK2NUM)) || (Pn==0x00))
        {
            *Ind_R_Adr = Ind_R;
            *Ind_S_Adr = Ind_S;
            return false;
        }
        
        *(S_Buf + Ind_S++) = DA1; 
        *(S_Buf + Ind_S++) = DA2; 
        *(S_Buf + Ind_S++) = DT1; 
        *(S_Buf + Ind_S++) = DT2; 
        
        Adr32 = (u32*)*(u32*)(Adr8 + 4);
        Adr8 = (u8*)((u8*)Adr32 + (Pn-1) * Len);
        if(N != 0)
        {
            N1 = *(Adr8 + 8);
            Len = N1 * L + X;
        }

        for(i = 0; i < Len; i++)
        {
            *(S_Buf + Ind_S++) = *(Adr8 + i);
        }
    }
    else if(T == 9)  //;后台参数(只读)
    {
        u8	FM24TestBz[4];
        u8  TestMark = 0;    //;测试标识字
        *(S_Buf + Ind_S++) = DA1; 
        *(S_Buf + Ind_S++) = DA2; 
        *(S_Buf + Ind_S++) = DT1; 
        *(S_Buf + Ind_S++) = DT2;       
        if(Fn == 53)
        {
            //;写FLASH
            j = 0;
            *(AllBuf.Spi1 + j++) = TestBiaoZhi1;
            *(AllBuf.Spi1 + j++) = TestBiaoZhi2;
            *(AllBuf.Spi1 + j++) = TestBiaoZhi3;
            *(AllBuf.Spi1 + j++) = TestBiaoZhi4;
            for(; j<527; j++)
            {
                *(AllBuf.Spi1 + j) = 0x00;
            }     
            SPI_FLASH_PageWrite_E(AllBuf.Spi1, FLASH_TEST);
            for(j=0; j<527; j++)
            {
                *(AllBuf.Spi1 + j) = 0x00;
            } 			
            //;读FLASH
            ReadFlash_Verify(FLASH_TEST, FLASH_TEST);
            if((*AllBuf.Spi1==TestBiaoZhi1) && (*(AllBuf.Spi1+1)==TestBiaoZhi2) &&
               (*(AllBuf.Spi1+2)==TestBiaoZhi3)&& (*(AllBuf.Spi1+3)==TestBiaoZhi4) )
            {
                TestMark |= 0x01; //;FLASH 读写正确，D0位置1，否则置0
            }	
            else
            {
            	  TestMark |= 0x00;
            }
            //;写FM24
            FM24TestBz[0] = TestBiaoZhi1;
            FM24TestBz[1] = TestBiaoZhi2;
            FM24TestBz[2] = TestBiaoZhi3;
            FM24TestBz[3] = TestBiaoZhi4;
            EW.WFM = 'W';
            WriteNByte_I2C1(4, _FM24_TEST, FM24TestBz);
            EW.WFM = 0;
            for(j=0; j<4;j++)
            {
                FM24TestBz[j] = 0;
            }
            //;读FM24
            EW.RFM = 'R';
            ReadNByte_I2C1(4, _FM24_TEST, FM24TestBz);
            EW.RFM = 0;
            if((FM24TestBz[0]==TestBiaoZhi1) && (FM24TestBz[1]==TestBiaoZhi2) && 
               (FM24TestBz[2]==TestBiaoZhi3) && (FM24TestBz[3]==TestBiaoZhi4))
            {
                TestMark |= (0x01<<1); //;FM24 读写正确，D1位置1，否则置0
            }
            else
            {
            	  TestMark |= 0x00;
            }
            *(S_Buf + Ind_S++) = TestMark;
        }
        else if(Fn == 54) //;读出GPRS信号大小的值
        {
            *(S_Buf + Ind_S++) = GprsC.Sign;
        }         
        else if(Fn == 55) //;F1终端上行通信口通信参数（读FLASH后上送）
        {
            EW.MoveByte = 'M';
            SW.Para[0] = 'P';
            SW.Para[1] = 'a'; //;允许对终端参数和测量点参数进行操作
            
            //;读1区
            ReadFlash_Verify(CSBock1, CSBock1Bak);
            if((*AllBuf.Spi1==CSBiaoZhi1) && (*(AllBuf.Spi1+1)==CSBiaoZhi2) &&
               (*(AllBuf.Spi1+2)==CSBiaoZhi3))
            {
                MovN2Byte(CS1NUM, &ZDP.F1.RTS, AllBuf.Spi1 + 3); //;F1~F10   
                  
                MovN2Byte(6, (u8*)S_Buf + Ind_S, &ZDP.F1.RTS); //;F1
                Ind_S += 6;   
            }
            else
            {
                SW.Para[0] = 0;
                SW.Para[1] = 0;
                EW.MoveByte = 0;
                *Ind_R_Adr = Ind_R ;
                *Ind_S_Adr = Ind_S - 4;
                return false; //;;终端不支持该参数               
            }
            SW.Para[0] = 0;
            SW.Para[1] = 0;
            EW.MoveByte = 0;
        }
        else if(Fn == 56) //;读出事件计数器的值
        {
            //;读出事件计数器EC
            ReadData_FM24(1, E2R_EC1,  E2R_EC1 + E2R_ByteNum,  (u8*) ZDS.F7);    
            ReadData_FM24(1, E2R_EC2,  E2R_EC2 + E2R_ByteNum,  (u8*) ZDS.F7+1);
            
            *(S_Buf + Ind_S++) = ZDS.F7[0];
            *(S_Buf + Ind_S++) = ZDS.F7[1];
            
        }   		
	      else
	      {
             //;任务控制字        
             Adr8 = (u8 *)&TaskC.Switch;
             for(i = 0; i < 87; i++)
             {
                 *(S_Buf + Ind_S++) = *(Adr8 + i);
             }
             //;告警控制字
             *(S_Buf + Ind_S++) = (u8)ERC1.Index;
             *(S_Buf + Ind_S++) = (u8)(ERC1.Index>>8); 
             *(S_Buf + Ind_S++) = (u8)ERC1.StInd;
             *(S_Buf + Ind_S++) = (u8)(ERC1.StInd>>8); 
             *(S_Buf + Ind_S++) = (u8)ERC2.Index;
             *(S_Buf + Ind_S++) = (u8)(ERC2.Index>>8); 
             *(S_Buf + Ind_S++) = (u8)ERC2.StInd;
             *(S_Buf + Ind_S++) = (u8)(ERC2.StInd>>8); 
             //;日冻结控制字
             *(S_Buf + Ind_S++) = P1DDJ.Index;
             *(S_Buf + Ind_S++) = P1DDJ.StInd;
             //;抄表日冻结控制字
             *(S_Buf + Ind_S++) = P1BDJ.Index;
             *(S_Buf + Ind_S++) = P1BDJ.StInd;
             //;月冻结控制字
             *(S_Buf + Ind_S++) = P1MDJ.Index;
             *(S_Buf + Ind_S++) = P1MDJ.StInd;
             //;曲线冻结
             *(S_Buf + Ind_S++) = (u8)P1QDJ.Index;
             *(S_Buf + Ind_S++) = (u8)(P1QDJ.Index>>8); 
             *(S_Buf + Ind_S++) = (u8)P1QDJ.StInd;
             *(S_Buf + Ind_S++) = (u8)(P1QDJ.StInd>>8); 
	      }
	 
    }
    else
    {
        *Ind_R_Adr = Ind_R ;
        *Ind_S_Adr = Ind_S;
        return false; //;;终端不支持该参数
    }
    
    *Ind_R_Adr = Ind_R;
    *Ind_S_Adr = Ind_S;
    return true;
}


/* 
********************************************************************************
* 函 数 名：GDW_AFN0AH
* 功    能：查询参数
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
u16 GDW_AFN090AH(char *R_Buf, char *S_Buf, struct GuiYueCtrl *GYCtrl)
{
    u16 i, len, Ind_S, Ind_R;
    u8 SEQ;
    u8 AFN;
    //s32 long1=0;

    len = *(R_Buf + 1) + (*(R_Buf + 2) << 8);
    len >>= 2; //;用户数据区的总长度
    len -= 8; //;链路用户数据层的总长度,去掉控制域和地址域，再去掉AFN 和 SEQ 两个字节以后的数据长度
    
    SEQ = *(R_Buf + 13);
	
    if(SEQ & 0x80)                                   //;带有时间标签
    {
        len -= 6;                                   //;去掉时间标签长度
    }
    /*
    if(SEQ & 0x80)                                   //;带有时间标签
    {
        len -= 6;                                   //;去掉时间标签长度
        if(*(R_Buf+19+len)>0)
        {
           long1=Time_distance(&TBCD.Sec, (u8*)(R_Buf + 15 + len));   //;tpv允许发送传输延时判断zrt20140107
           if (long1 < -(*(R_Buf + 19 + len)*60) || long1 > (*(R_Buf+19+len)*60) )
	         {
              return 0;
           }
        }
    }
    */
    Ind_R = GYCtrl->Index_Answer;
    
    if(Ind_R == 0)
    {
        Ind_R = 14;
        GYCtrl->ISeq = (*(R_Buf + 13)) &0x0f;
    }
    
    Ind_S = 14;
    AFN = GYCtrl->AFNList[GYCtrl->ListNo];
    if(AFN == 0x0A)
    {
        for(; Ind_R<len+14;)
        {
            PreData_AFN0AH(S_Buf, &Ind_S, R_Buf, &Ind_R);
        }
    }
    else if(AFN == 0x09)
    {
        for(; Ind_R<len+14;)
        {
            PreData_AFN09H(S_Buf, &Ind_S, R_Buf, &Ind_R);
        }
    }
    
    *(S_Buf + 0) = *(S_Buf + 5) = 0x68;
    *(S_Buf + 6) = 0x88;      //;DIR=1,PRM=0,ACD=0; CID=8
    *(S_Buf + 7) = *(R_Buf + 7);
    *(S_Buf + 8) = *(R_Buf + 8);
    *(S_Buf + 9) = *(R_Buf + 9);
    *(S_Buf + 10) = *(R_Buf + 10);
    *(S_Buf + 11) = *(R_Buf + 11);    
    *(S_Buf + 12) = *(R_Buf + 12);
    *(S_Buf + 13) = 0x00; //;Tvp=0

//    *(S_Buf + 13) += (GYCtrl->ISeq &0x0f);
//    GYCtrl->ISeq++; //;发生帧计数
    
    if(GYCtrl->Index_Answer == 0x00) //;第一帧
    {
        *(S_Buf + 13) |= 0x40;
    }
    GYCtrl->Index_Answer = Ind_R; //;当前处理到该请求的第i个字节
    
    if(Ind_R >= len+14) //;最后一帧
    {
        *(S_Buf + 13) |= 0x20;
        GYCtrl->Index_Answer = 0; //;本请求应答已经完成，下一请求需要从开始来应答
        GYCtrl->AFNList[0] = 0;
        GYCtrl->AFNNum = 0;
    }
    else
    {
        GYCtrl->AFNNum++;
        if(GYCtrl->AFNNum < NFRAME)  //;--@2
        {
            GYCtrl->AFNList[0] = 0x0A;
        }
        else
        {  //;分帧次数不能超过 MaxNum_Frame 次
            *(S_Buf + 13) |= 0x20;
            GYCtrl->Index_Answer = 0;
            GYCtrl->AFNList[0] = 0;
            GYCtrl->AFNNum = 0;
        }
    }

    if(Ind_S <= 14)
    //;如果没有要求的数据，则用确认否认帧来全部否认
    {
        *(S_Buf + 12) = 0x00;
        *(S_Buf + 13) = 0x60;
        Ind_S = 14;
        *(S_Buf + Ind_S++) = 0x00;
        *(S_Buf + Ind_S++) = 0x00;
        *(S_Buf + Ind_S++) = 0x02;
        *(S_Buf + Ind_S++) = 0x00;
        GYCtrl->Index_Answer = 0;
        GYCtrl->AFNList[0] = 0;
        GYCtrl->AFNNum = 0;
    }

    *(S_Buf + 13) += (GYCtrl->ISeq &0x0f);
    GYCtrl->ISeq++; //;发生帧计数	

    if(SEQ & 0x80)//;20131102
	{
		*(S_Buf + Ind_S++) = *(R_Buf + 14 + len);
                *(S_Buf + Ind_S++) = *(R_Buf + 15 + len);
                *(S_Buf + Ind_S++) = *(R_Buf + 16 + len);
                *(S_Buf + Ind_S++) = *(R_Buf + 17 + len);
                *(S_Buf + Ind_S++) = *(R_Buf + 18 + len);
		/**(S_Buf + Ind_S++) = TBCD.Sec;
		*(S_Buf + Ind_S++) = TBCD.Min;
		*(S_Buf + Ind_S++) = TBCD.Hour;
		*(S_Buf + Ind_S++) = TBCD.Day;*/
		*(S_Buf + Ind_S++) = *(R_Buf + 19 + len);;
                *(S_Buf + 13) |= 0x80;
	}    	

    len = Ind_S - 6;
    len <<= 2;
    len += 2;
    //;数据域长度
    *(S_Buf + 1) = *(S_Buf + 3) = (u8)len;
    *(S_Buf + 2) = *(S_Buf + 4) = (u8)(len >> 8);
    
    //;校验和
    *(S_Buf + Ind_S) = 0;
    for(i=6; i<Ind_S; i++)
    {
        *(S_Buf + Ind_S) += *(S_Buf + i);
    }
    //;结束字节
    *(S_Buf + Ind_S + 1) = 0x16;
    return (Ind_S + 2);
}



/* 
********************************************************************************
* 函 数 名：GDW_AFN0CH
* 功    能：对请求1类数据的应答处理
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：2011-09-20 20:07:13
********************************************************************************
*/
u16 GDW_AFN0CH(char *R_Buf, char *S_Buf, struct GuiYueCtrl *GYCtrl)
{
    u16 i, len, Ind_S;
    u32 Index_R_S;
    u8 SEQ;    
    //s32 long1=0;

    len = *(R_Buf + 1) + (*(R_Buf + 2) << 8);
    len >>= 2; //;用户数据区的总长度
    len -= 8; //;链路用户数据层的总长度,去掉控制域和地址域，再去掉AFN 和 SEQ 两个字节以后的数据长度
    
    SEQ = *(R_Buf + 13);
    //;去掉附加域时间标签的长度
	
    if(SEQ & 0x80)                                   //;带有时间标签
    {
        len -= 6;                                   //;去掉时间标签长度
    }
    /* //;与建三江连接出现死循环--20141225
    if(SEQ & 0x80)                                   //;带有时间标签
    {
        len -= 6;                                   //;去掉时间标签长度
        if(*(R_Buf+19+len)>0)
        {
           long1=Time_distance(&TBCD.Sec, (u8*)(R_Buf + 15 + len));   //;tpv允许发送传输延时判断zrt20140107
           if (long1 < -(*(R_Buf + 19 + len)*60) || long1 > (*(R_Buf+19+len)*60) )
	   {
              return 0;
           }
        }
    }
     */  
    i = GYCtrl->Index_Answer;
    if(i == 0)
    {
        i = 14;
        GYCtrl->ISeq = (*(R_Buf + 13)) &0x0f; //;第一帧时取出帧序号
    }

    Ind_S = 14;
    for(; i<len+14; )
    {
        Index_R_S = PreData_AFN0CH(Ind_S, i, S_Buf, R_Buf);
        if((Index_R_S&0xffff) < FRAMESIZE)
        {
            Ind_S = (u16)Index_R_S;
            i = Index_R_S >> 16;
        }
        else
        {
            break;
        }
    }
    
    *(S_Buf + 0) = *(S_Buf + 5) = 0x68;
    *(S_Buf + 6) = 0x88; //;DIR=1,PRM=0,ACD=0; CID=8
    *(S_Buf + 7) = *(R_Buf + 7);
    *(S_Buf + 8) = *(R_Buf + 8);
    *(S_Buf + 9) = *(R_Buf + 9);
    *(S_Buf + 10) = *(R_Buf + 10);
    *(S_Buf + 11) = *(R_Buf + 11);
    //;终端地址一样
    *(S_Buf + 12) = *(R_Buf + 12);
    *(S_Buf + 13) = 0x00; //;Tpv=0 //;不带时间标签
//    *(S_Buf + 13) |= (GYCtrl->ISeq &0x0f);   
//    GYCtrl->ISeq++; //;发生帧计数
    
    //;第一帧
    if(GYCtrl->Index_Answer == 0x00)   
    {
        *(S_Buf + 13) |= 0x40;
    }
    GYCtrl->Index_Answer = i; //;当前处理到该请求的第i个字节

    if((i >= len + 14)) //;最后一帧
    {
        *(S_Buf + 13) |= 0x20;
        GYCtrl->Index_Answer = 0; //;本请求应答已经完成，下一请求需要从开始来应答
        GYCtrl->AFNList[0] = 0;
        GYCtrl->AFNNum = 0;
    }
    else
    {
        GYCtrl->AFNNum++;
        if(GYCtrl->AFNNum < NFRAME) //;--@2
        {
             GYCtrl->AFNList[0] = 0x0C;
        }
        else
        {
            *(S_Buf + 13) |= 0x20;  //;分帧次数不能超过MaxNum_Frame次
            GYCtrl->Index_Answer = 0;
            GYCtrl->AFNList[0] = 0;
            GYCtrl->AFNNum = 0;
        }
    }
    
    //;如果没有要求的数据，则用确认否认帧来全部否认
    if(Ind_S <= 14) 
    {
        *(S_Buf + 12) = 0x00;
        *(S_Buf + 13) = 0x60;
        Ind_S = 14;
        *(S_Buf + Ind_S++) = 00;
        *(S_Buf + Ind_S++) = 00;
        *(S_Buf + Ind_S++) = 02;
        *(S_Buf + Ind_S++) = 00;
        GYCtrl->Index_Answer = 0;
        GYCtrl->AFNList[0] = 0;
        GYCtrl->AFNNum = 0;
    }

    *(S_Buf + 13) |= (GYCtrl->ISeq &0x0f);   
    GYCtrl->ISeq++; //;发生帧计数	

    if(SEQ & 0x80)//;20131102
	{
		*(S_Buf + Ind_S++) = *(R_Buf + 14 + len);
                *(S_Buf + Ind_S++) = *(R_Buf + 15 + len);
                *(S_Buf + Ind_S++) = *(R_Buf + 16 + len);
                *(S_Buf + Ind_S++) = *(R_Buf + 17 + len);
                *(S_Buf + Ind_S++) = *(R_Buf + 18 + len);
		/**(S_Buf + Ind_S++) = TBCD.Sec;
		*(S_Buf + Ind_S++) = TBCD.Min;
		*(S_Buf + Ind_S++) = TBCD.Hour;
		*(S_Buf + Ind_S++) = TBCD.Day;*/
		*(S_Buf + Ind_S++) = *(R_Buf + 19 + len);
                *(S_Buf + 13) |= 0x80;
	}   	
    
    len = Ind_S - 6;
    len <<= 2;
    len += 2;
    //;数据域长度
    *(S_Buf + 1) = *(S_Buf + 3) =(u8)len;
    *(S_Buf + 2) = *(S_Buf + 4) = (u8)(len >> 8);
    
    *(S_Buf + Ind_S) = 0; //;校验和
    for(i=6; i<Ind_S; i++)
    {
        *(S_Buf + Ind_S) += *(S_Buf + i);
    }
    
    *(S_Buf + Ind_S + 1) = 0x16; //;结束字节
    return (Ind_S + 2);
}

/* 
********************************************************************************
* 函 数 名：GDW_AFN0CH_Up
* 功    能：第一类任务数据主动上送
* 参    数：
* 返    回： 
* 说    明：不支持分帧上送
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
u16 GDW_AFN0CH_Up(char *S_Buf, struct GuiYueCtrl *GYCtrl)
{
    u16 i, len, Ind_S; 
    u32 Index_R_S;   

    len = TaskP.F65[TaskC.TaskNO].PnFnNum * 4;
    i = 0;
    Ind_S = 14;
    for(; i < len;)
    {
        Index_R_S = PreData_AFN0CH(Ind_S, i, S_Buf, (char*)TaskP.F65[TaskC.TaskNO].PnFn[0]); 
        if((Index_R_S &0xffff) < FRAMESIZE)
        {
            Ind_S = (u16)Index_R_S;
            i = Index_R_S >> 16;
        }
        else
        {
            break;
        }
    }

    if(Ind_S <= 14)
    {
        return (0); //;没有数据
    }
    
    *(S_Buf + 0) = *(S_Buf + 5) = 0x68;
    *(S_Buf + 6) = 0xC4; //;DIR=1;PRM=1,ACD=0 CID=4

    *(S_Buf + 7) = ZDPKZ.F89.QuXian[0];
    *(S_Buf + 8) = ZDPKZ.F89.QuXian[1]; //;区县码
    *(S_Buf + 9) = ZDPKZ.F89.DiZhi[0];
    *(S_Buf + 10) = ZDPKZ.F89.DiZhi[1]; //;终端地址
    *(S_Buf + 11) = 0; //;主站MSA
    *(S_Buf + 12) = 0x0C;
    *(S_Buf + 13) = (GYCtrl->ISeq) &0x0F;
    *(S_Buf + 13) |= 0x60; //;Tpv=0;FIR=1，FIN=1,CON=0
    GYCtrl->ISeq++;


    len = Ind_S - 6;
    len *= 4;
    len += 2;
    //;数据域长度
    *(S_Buf + 1) = *(S_Buf + 3) =(u8)len;
    *(S_Buf + 2) = *(S_Buf + 4) = (u8)(len >> 8);
    //;校验和
    *(S_Buf + Ind_S) = 0;
    for(i=6; i<Ind_S; i++)
    {
        *(S_Buf + Ind_S) += *(S_Buf + i);
    }
    //;结束字节
    *(S_Buf + Ind_S + 1) = 0x16;
    return (Ind_S + 2);
}

/* 
********************************************************************************
* 函 数 名：AFN0DH_DJ
* 功    能：二类数据的读取组织数据
* 参    数：
* 返    回：0xFF--数据发送缓存不足，退出后不再查找下一个信息类。
*           
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：2011-09-23 10:42:18 返回只有当发送缓存不足才不再查找下一信息类
********************************************************************************
*/
u8 AFN0DH_DJ(char *S_Buf, u16 *Ind_S_Adr, char *R_Buf, u16 *Ind_R_Adr)
{
    u8 temp, readtime, ratenum;
    u8 Fn, type, num, j, multiple;
    u8 i, len, Td[7];
    u16  Buf_Add = 0;
    u16 Ind_S, Ind_R, SumLen = 0, Pn, Le;
    u32 *adr32;
    static u8 signnum[5] = {0x00, 0x07, 0x07, 0x06, 0x0B};

    Ind_S = *Ind_S_Adr;
    Ind_R = *Ind_R_Adr;
    
    Pn = Find_Pn(*(R_Buf + Ind_R + 0), *(R_Buf + Ind_R + 1));
    Fn = Find_Fn(*(R_Buf + Ind_R + 2), *(R_Buf + Ind_R + 3));
    
    adr32 = (u32*)(PN0DZU + Fn);  //;找出信息类
    temp = *((u8*)adr32 + 3); //;
    type = temp & 0x07; 
    readtime = temp & 0x08;
    ratenum  = temp & 0x10;
    len  = *((u8*)adr32 + 2);
    
    //;信息类错误无法识别息点错误，或终端不支持该信息
    if((Fn==0xFF) || (Fn>218) || (type==0) || (len==0) ||
       (Pn==0xFF) || (ZD_Pn_Find(Pn) < 0))  
    {                                         //;抄F13数据导致程序复位,---20141225
        *Ind_R_Adr = Ind_R + signnum[type]+4; //;指向下一个信息类继续查找
        return false;                         //;该信息点的数据不存在
    }
     
    if(readtime)   //;抄表时间长度
    {
       SumLen += 5;
    }
    if(ratenum)  //;费率数长度
    {
       SumLen += 1;
    }
    
    if(type == 0x04)
    {
        num = *(R_Buf + Ind_R + 10);
        SumLen += Ind_S + 11 + len * num;
    }
    else
    {
        SumLen += (Ind_S + signnum[type] + len);
    }
    
    if(SumLen > FRAMESIZE)
    {
        return 0xFF;  //;发送缓存区空间不足，退出后不再继续查找
    }
    
    *(S_Buf + Ind_S++) = *(R_Buf + Ind_R++);
    *(S_Buf + Ind_S++) = *(R_Buf + Ind_R++); //;Pn
    *(S_Buf + Ind_S++) = *(R_Buf + Ind_R++);
    *(S_Buf + Ind_S++) = *(R_Buf + Ind_R++); //;Fn

    if(type == 1)  //;日冻结
    {            
        Td[0] = *(R_Buf + Ind_R++);  //;读出日期
        Td[1] = *(R_Buf + Ind_R++);
        Td[2] = *(R_Buf + Ind_R++);
        
        Buf_Add = DDJ_Read(S_Buf, Ind_S + 3, Pn, Fn, Td); //;根据日期查找日冻结数据
        *(S_Buf + Ind_S++) = Td[0];  //;写入实际找到数据的日期
        *(S_Buf + Ind_S++) = Td[1];
        *(S_Buf + Ind_S++) = Td[2];        
        if(Buf_Add==0xFF)    //;Flash中还没有数据，数据不存在
        {
            Ind_S -= 7;          
        }         
        else
        {
            Ind_S += Buf_Add;
        }  
    }
    else if(type == 2)  //; 抄表日
    {
        Td[0] = *(R_Buf + Ind_R++);  //;读出日期
        Td[1] = *(R_Buf + Ind_R++);
        Td[2] = *(R_Buf + Ind_R++);

        Buf_Add = BDJ_Read(S_Buf, Ind_S + 3, Pn, Fn, Td); //;根据日期查找日冻结数据
               
        *(S_Buf + Ind_S++) = Td[0];  //;写入实际找到数据的日期
        *(S_Buf + Ind_S++) = Td[1];
        *(S_Buf + Ind_S++) = Td[2];
        
        //;Flash中还没有数据，数据不存在未找到该时间点的数据
        if(Buf_Add==0xFF)
        {
            Ind_S -= 7;
        }      
        else
        {
            Ind_S += Buf_Add;
        }
    }
    else if(type == 3)  //;月冻结
    {
        Td[0] = *(R_Buf + Ind_R++);  //;读出日期2011-05-17 22:36:59
        Td[1] = *(R_Buf + Ind_R++);

        Buf_Add = MDJ_Read(S_Buf, Ind_S + 2, Pn, Fn, Td); //;根据日期查找日冻结数据
        
        *(S_Buf + Ind_S++) = Td[0];  //;写入实际找到数据的日期
        *(S_Buf + Ind_S++) = Td[1];
        //;Flash中还没有数据，数据不存在,未找到该时间点的数据
        if(Buf_Add == 0xFF)
        {
            Ind_S -= 6;
        }        
        else
        {
            Ind_S += Buf_Add;
        }
    }
/*	
    else if(type == 4) //;曲线数据
    {
        Td[0] = *(R_Buf + Ind_R++); //;Td_c
        Td[1] = *(R_Buf + Ind_R++);
        Td[2] = *(R_Buf + Ind_R++);
        Td[3] = *(R_Buf + Ind_R++);
        Td[4] = *(R_Buf + Ind_R++);
        Td[5] = *(R_Buf + Ind_R++);
        Td[6] = *(R_Buf + Ind_R++);
        
        *(S_Buf + Ind_S + 0) = Td[0];
        *(S_Buf + Ind_S + 1) = Td[1];
        *(S_Buf + Ind_S + 2) = Td[2];
        *(S_Buf + Ind_S + 3) = Td[3];
        *(S_Buf + Ind_S + 4) = Td[4];
        *(S_Buf + Ind_S + 5) = Td[5];
        
        Time_15Min(Td); //;处理时间为15分钟的倍数
        
        if((Td[5] == 0) || (Td[6] == 0))  //;冻结密度和冻结点数为0
        {
            *Ind_S_Adr = Ind_S - 4;
            *Ind_R_Adr = Ind_R;
            return false;
        }
        
        num = 0;
               
        for(j=0; j<Td[6]; j++)
        {
            if(j == 0) //;无第一点数据，则返回否定帧
            {
                if(TimeCompare_5Byte(Td, &TBCD.Min) == back) //;超过了当前时间
                {
                    *Ind_S_Adr = Ind_S - 4;
                    *Ind_R_Adr = Ind_R;
                    return false;
                }
            }
             
            Le = QDJ_Read(S_Buf, Ind_S + 7 + Buf_Add, Pn, Fn, Td);
            //;Flash中还没有数据,Flash中有数据，但是未找到该时间点的数据
            if(Le == 0xFF)
            {
                for(i=0; i<len; i++)
                {
                    *(S_Buf + Ind_S + 7 + Buf_Add + i) = 0xEE;  //;用0xEE填充
                }
                Le = len;
            }
            else
            {
                num += 1; //;实际找到的点数
            }
                      
            Buf_Add += Le;
            multiple = Td[5];
            if(multiple >= 3)
            {
                multiple = 4;
            }
            Time_CRAdd(Td, 0, 15, multiple);
        }
        
        if(num == 0) //;未找到一点数据
        {
            *Ind_S_Adr = Ind_S - 4;
            *Ind_R_Adr = Ind_R;
            return false;
        }
        
        
        *(S_Buf + Ind_S + 6) = Td[6]; //;;写入实际找到的点数
        Buf_Add += 7;
        Ind_S += Buf_Add;
    }
*/
	else if(type == 4) //;曲线数据
	{
		Td[0] = *(R_Buf + Ind_R++); //;Td_c
		Td[1] = *(R_Buf + Ind_R++);
		Td[2] = *(R_Buf + Ind_R++);
		Td[3] = *(R_Buf + Ind_R++);
		Td[4] = *(R_Buf + Ind_R++);
		Td[5] = *(R_Buf + Ind_R++);
		Td[6] = *(R_Buf + Ind_R++);
		if(Td[0] != 0)
		{
			Time_15Min(Td); //;处理时间为15分钟的倍数
			Time_CRAdd(Td, 0, 15, 1);
		}
		
		*(S_Buf + Ind_S + 0) = Td[0];
		*(S_Buf + Ind_S + 1) = Td[1];
		*(S_Buf + Ind_S + 2) = Td[2];
		*(S_Buf + Ind_S + 3) = Td[3];
		*(S_Buf + Ind_S + 4) = Td[4];
		*(S_Buf + Ind_S + 5) = Td[5];
		
		
		
		if((Td[5] == 0) || (Td[6] == 0))  //;冻结密度和冻结点数为0
		{
			*Ind_S_Adr = Ind_S - 4;
			*Ind_R_Adr = Ind_R;
			return false;
		}
		
		num = 0;
		       
		for(j=0; j<Td[6]; j++)
		{
//			if(j == 0) //;无第一点数据，则返回否定帧
//			{
//				if(TimeCompare_5Byte(Td, &TBCD.Min) == back) //;超过了当前时间
//				{
//					*Ind_S_Adr = Ind_S - 4;
//					*Ind_R_Adr = Ind_R;
//					return false;
//				}
//			}
			if(TimeCompare_5Byte(Td, &TBCD.Min) == back) //;超过了当前时间 qiu_20130516
			{
				break;
			}
			Le = QDJ_Read(S_Buf, Ind_S + 7 + Buf_Add, Pn, Fn, Td);
			//;Flash中还没有数据,Flash中有数据，但是未找到该时间点的数据
			if(Le == 0xFF)
			{
				for(i=0; i<len; i++)
				{
					*(S_Buf + Ind_S + 7 + Buf_Add + i) = 0xEE;  //;用0xEE填充
				}
				Le = len;
			}
			else
			{
				num += 1; //;实际找到的点数
			}
			          
			Buf_Add += Le;
			multiple = Td[5];
			if(multiple >= 3)
			{
				multiple = 4;
			}
			Time_CRAdd(Td, 0, 15, multiple);
		}
		
		if(num == 0) //;未找到一点数据
		{
			*Ind_S_Adr = Ind_S - 4;
			*Ind_R_Adr = Ind_R;
			return false;
		}
//		*(S_Buf + Ind_S + 6) = Td[6]; //;;写入实际找到的点数
		*(S_Buf + Ind_S + 6) = num; //;;写入实际找到的点数 qiu_20130516
		Buf_Add += 7;
		Ind_S += Buf_Add;
	}

    *Ind_S_Adr = Ind_S;
    *Ind_R_Adr = Ind_R; //;指向下一个信息类继续查找
    return true; //;该信息类的数据存在
}

/* 
********************************************************************************
* 函 数 名：GDW_AFN0DH_Up
* 功    能：第二类任务主动上送
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
/*
u16 GDW_AFN0DH_Up(char *S_Buf, struct GuiYueCtrl *GYCtrl)
{
    u8  Pn, Fn;
    u16 i, len, Ind_S, Buf_Add = 0;

    *(S_Buf + 0 ) = *(S_Buf + 5) = 0x68;
    *(S_Buf + 6 ) = 0xC4;                              //;DIR=1;PRM=1,ACD=0 CID=4
    *(S_Buf + 7 ) = ZDPKZ.F89.QuXian[0];
    *(S_Buf + 8 ) = ZDPKZ.F89.QuXian[1];               //;区县码
    *(S_Buf + 9 ) = ZDPKZ.F89.DiZhi[0];
    *(S_Buf + 10) = ZDPKZ.F89.DiZhi[1];           //;终端地址
    *(S_Buf + 11) = 0;                                 //;主站MSA
    *(S_Buf + 12) = 0x0d;
    *(S_Buf + 13) = (GYCtrl->ISeq) & 0x0F;
    *(S_Buf + 13) |= 0x60;                             //; Tpv=0; FIR=1;FIN=1,CON=0
    GYCtrl->ISeq++;
    Ind_S = 14;

    for(i=0; i<TaskP.F66[TaskC.TaskNO].PnFnNum; i++)
    {
        Pn = Find_Pn(TaskP.F66[TaskC.TaskNO].PnFn[i][0], TaskP.F66[TaskC.TaskNO].PnFn[i][1]);
        Fn = Find_Fn(TaskP.F66[TaskC.TaskNO].PnFn[i][2], TaskP.F66[TaskC.TaskNO].PnFn[i][3]);
        Buf_Add = T2Task_FnPn(S_Buf, Ind_S + 4, Pn, Fn);
        if (Buf_Add != 0)
        {
            *(S_Buf + Ind_S++) = TaskP.F66[TaskC.TaskNO].PnFn[i][0];
            *(S_Buf + Ind_S++) = TaskP.F66[TaskC.TaskNO].PnFn[i][1];
            *(S_Buf + Ind_S++) = TaskP.F66[TaskC.TaskNO].PnFn[i][2];
            *(S_Buf + Ind_S++) = TaskP.F66[TaskC.TaskNO].PnFn[i][3];
            Ind_S += Buf_Add;
            if(Ind_S >= 500)
            {
                break;
            }
        }
    }

    if(Ind_S <= 14)
    {
        return (0); //;没有数据
    }

    len = Ind_S - 6;
    len *= 4; //;左移两位
    len += 2; //;本规约使用
    //;数据域长度
    *(S_Buf + 1) = *(S_Buf + 3) =(u8)len;
    *(S_Buf + 2) = *(S_Buf + 4) = (u8)(len >> 8);
    //;校验和
    *(S_Buf + Ind_S) = 0;
    for (i = 6; i < Ind_S; i++)
    {
        *(S_Buf + Ind_S) += *(S_Buf + i);
    }
    //;结束字节
    *(S_Buf + Ind_S + 1) = 0x16;
    return (Ind_S + 2);
}
*/
u16 GDW_AFN0DH_Up(char *S_Buf, struct GuiYueCtrl *GYCtrl) 
{
	u8  Pn, Fn, FnNum, temp;
	u16  FnInfo;			//;qiu20131120
	u16 i, j, len, Ind_S, Buf_Add = 0;

	*(S_Buf + 0 ) = *(S_Buf + 5) = 0x68;
	*(S_Buf + 6 ) = 0xC4;                              //;DIR=1;PRM=1,ACD=0 CID=4
  *(S_Buf + 7 ) = ZDPKZ.F89.QuXian[0];
  *(S_Buf + 8 ) = ZDPKZ.F89.QuXian[1];               //;区县码
  *(S_Buf + 9 ) = ZDPKZ.F89.DiZhi[0];
  *(S_Buf + 10) = ZDPKZ.F89.DiZhi[1];           //;终端地址
	*(S_Buf + 11) = 0;                                 //;主站MSA
	*(S_Buf + 12) = 0x0d;
	*(S_Buf + 13) = (GYCtrl->ISeq) & 0x0F;
	*(S_Buf + 13) |= 0x60;                             //; Tpv=0; FIR=1;FIN=1,CON=0
	if (ZDP.F1.CON != 0)			//;qiu20131119	
	{
		*(S_Buf + 13) |= 0x10;		//;需要主站应答
	}
	GYCtrl->ISeq++;
	Ind_S = 14;
	
	for(i=0; i<TaskP.F66[TaskC.TaskNO].PnFnNum; i++)
	{
		Pn = Find_Pn(TaskP.F66[TaskC.TaskNO].PnFn[i][0], TaskP.F66[TaskC.TaskNO].PnFn[i][1]);
		FnInfo = Find_Fn(TaskP.F66[TaskC.TaskNO].PnFn[i][2], TaskP.F66[TaskC.TaskNO].PnFn[i][3]);
		
		Fn = (FnInfo & 0xff) - 1;	//;起始Fn - 1
		FnNum = (FnInfo >> 8) & 0xff;	//;高字节为Fn个数
		for (j = 0; j <8; j++)
		{
			Fn++;
			temp =  (Fn - 1) % 8;		//;需要左移数
			if (TaskP.F66[TaskC.TaskNO].PnFn[i][2] & (1 << temp))		//;当前组内Fn需要处理
			{
				Buf_Add = T2Task_FnPn(S_Buf, Ind_S + 4, Pn, Fn);
				if (Buf_Add != 0)
				{
					*(S_Buf + Ind_S++) = TaskP.F66[TaskC.TaskNO].PnFn[i][0];
					*(S_Buf + Ind_S++) = TaskP.F66[TaskC.TaskNO].PnFn[i][1];
					*(S_Buf + Ind_S++) = 1 << ((Fn-1) % 8);
					*(S_Buf + Ind_S++) = (Fn - 1) / 8;
					Ind_S += Buf_Add;
					if(Ind_S >= 500)
					{
						break;
					}
				}
				FnNum--;
				if (FnNum == 0)
				{
					break;
				}
			}			
		}
	}
	
	if(Ind_S <= 14)
	{
		return (0); //;没有数据
	}
		
	len = Ind_S - 6;
	len *= 4; //;左移两位
	len += 2; //;本规约使用
	//;数据域长度
	*(S_Buf + 1) = *(S_Buf + 3) = (u8)len;
	*(S_Buf + 2) = *(S_Buf + 4) = (u8)(len >> 8);
	//;校验和
	*(S_Buf + Ind_S) = 0;
	for (i = 6; i < Ind_S; i++)
	{
		*(S_Buf + Ind_S) += *(S_Buf + i);
	}
	//;结束字节
	*(S_Buf + Ind_S + 1) = 0x16;
	return (Ind_S + 2);
}


/* 
********************************************************************************
* 函 数 名：GDW_AFN0DH
* 功    能：
* 参    数：
* 返    回： 
* 说    明：查询历史时,如果时间超过存储范围,终端应从返回最近一条开始返回,
*           不应返回否认帧。
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
u16 GDW_AFN0DH(char *R_Buf, char *S_Buf, struct GuiYueCtrl *GYCtrl)
{
    u8  Status, SEQ;
    u16 i, len, Ind_S, Ind_R;
    //s32 long1=0;


    len = *(R_Buf + 1) + (*(R_Buf + 2) << 8);
    len >>= 2;  //;用户数据区的总长度
    len -= 8;   //;链路用户数据层的总长度
    SEQ = *(R_Buf + 13);
	
    if(SEQ & 0x80)                                   //;带有时间标签
    {
        len -= 6;                                   //;去掉时间标签长度
    }
    /* //;建三江软件发送数据帧引起死循环 --20141225
    if(SEQ & 0x80)                                   //;带有时间标签
    {
        len -= 6;                                   //;去掉时间标签长度
        if(*(R_Buf+19+len)>0)
        {
           long1=Time_distance(&TBCD.Sec, (u8*)(R_Buf + 15 + len));   //;tpv允许发送传输延时判断zrt20140107
           if (long1 < -(*(R_Buf + 19 + len)*60) || long1 > (*(R_Buf+19+len)*60) )
	   {
              return 0;
           }
        }
    }
    */

    Ind_R = GYCtrl->Index_Answer;
    if(Ind_R == 0)
    {
        Ind_R = 14;
        GYCtrl->ISeq = (*(R_Buf + 13)) & 0x0f;
    }
    Ind_S = 14;

    for( ; Ind_R<len+14; )
    {
        Status = AFN0DH_DJ(S_Buf, &Ind_S, R_Buf, &Ind_R);
        if(Status == 0xFF) //;;存储数据不存在，返回否定帧
        {
            break;
        }
        else if(Status == 0xFE) //;; 缓存空间不足，需要分帧
        {
            break;
        }
        else if(Status == 0xFD) //;; 信息类无法识别，无法继续查找
        {
            break;
        }
    }

    *(S_Buf + 0 ) = 0x68;
    *(S_Buf + 5 ) = 0x68;
    *(S_Buf + 6 ) = 0x88;
    *(S_Buf + 7 ) = *(R_Buf + 7 );
    *(S_Buf + 8 ) = *(R_Buf + 8 );
    *(S_Buf + 9 ) = *(R_Buf + 9 );
    *(S_Buf + 10) = *(R_Buf + 10);
    *(S_Buf + 11) = *(R_Buf + 11);
    *(S_Buf + 12) = *(R_Buf + 12);
    *(S_Buf + 13) = 0x00;

//    *(S_Buf + 13) |= (GYCtrl->ISeq &0x0f);
//    GYCtrl->ISeq++; //;发生帧计数

    if(GYCtrl->Index_Answer == 0x00)   //;第一帧
    {
        *(S_Buf + 13) |= 0x40;
    }
    GYCtrl->Index_Answer = Ind_R;      //;当前处理到该请求的第i个字节

    if((Ind_R >= len + 14))  //;最后一帧
    {
        *(S_Buf + 13) |= 0x20;
        GYCtrl->Index_Answer = 0; //;本请求应答已经完成，下一请求需要从开始来应答
        GYCtrl->AFNList[0] = 0;
        GYCtrl->AFNNum = 0;
    }
    else
    {
        GYCtrl->AFNNum++;
        if(GYCtrl->AFNNum < NFRAME) //;分帧次数不能超过MaxNum_Frame次
        {
            GYCtrl->AFNList[0] = 0x0D;
        }
        else
        {
            *(S_Buf + 13) |= 0x20;
            GYCtrl->Index_Answer = 0;
            GYCtrl->AFNList[0] = 0;
            GYCtrl->AFNNum = 0;
        }
    }

    if(Ind_S <= 14)   //;如果没有要求的数据，则用确认否认帧来全部否认
    {
        *(S_Buf + 12) = 0x00;
        *(S_Buf + 13) = 0x60;
        *(S_Buf + 13) |= (SEQ &0x0F);	//qiu20131112
        Ind_S = 14;
        *(S_Buf + Ind_S++) = 00;
        *(S_Buf + Ind_S++) = 00;
        *(S_Buf + Ind_S++) = 02;
        *(S_Buf + Ind_S++) = 00;
        GYCtrl->Index_Answer = 0; //;本请求应答已经完成，下一请求需要从开始来应答
        GYCtrl->AFNList[0] = 0;
        GYCtrl->AFNNum = 0;
    }

    *(S_Buf + 13) |= (GYCtrl->ISeq &0x0f);
    GYCtrl->ISeq++; //;发生帧计数	

    if(SEQ & 0x80)//;20131102
	  {
	  	*(S_Buf + Ind_S++) = *(R_Buf + 14 + len);
      *(S_Buf + Ind_S++) = *(R_Buf + 15 + len);
      *(S_Buf + Ind_S++) = *(R_Buf + 16 + len);
      *(S_Buf + Ind_S++) = *(R_Buf + 17 + len);
      *(S_Buf + Ind_S++) = *(R_Buf + 18 + len);
	  	/**(S_Buf + Ind_S++) = TBCD.Sec;
	  	*(S_Buf + Ind_S++) = TBCD.Min;
	  	*(S_Buf + Ind_S++) = TBCD.Hour;
	  	*(S_Buf + Ind_S++) = TBCD.Day;*/
	  	*(S_Buf + Ind_S++) = *(R_Buf + 19 + len);;
      *(S_Buf + 13) |= 0x80;
	  }

    len = Ind_S - 6;
    len *= 4;
    len += 2;      //;数据域长度

    *(S_Buf + 1) = *(S_Buf + 3) = (u8)len;
    *(S_Buf + 2) = *(S_Buf + 4) = (u8)(len >> 8);

    *(S_Buf + Ind_S) = 0;//;校验和
    for(i=6; i<Ind_S; i++)
    {
        *(S_Buf + Ind_S) += *(S_Buf + i);
    }
    *(S_Buf + Ind_S + 1) = 0x16;//;结束字节
    return (Ind_S + 2);
}





/*--------------------------------------------------------------------------
  函数：GDW_AFN0EH
  功能：对 AFN=0x0E 的帧进行处理 读第三类数据
  参数：
 ---------------------------------------------------------------------------*/
u16 GDW_AFN0EH(char *R_Buf, char *S_Buf, struct GuiYueCtrl *GYCtrl)
{
    u8 SEQ;
    u16 i, len, Ind_S;
    u32 Index_R_S;
    //s32 long1=0;
	
    len = *(R_Buf + 1) + *(R_Buf + 2) *256;
    len >>= 2; //;用户数据区的总长度
    len -= 8; //;链路用户数据层的总长度,去掉控制域和地址域，再去掉AFN 和 SEQ 两个字节以后的数据长度

    SEQ = *(R_Buf + 13);
	
    if(SEQ & 0x80)                                   //;带有时间标签
    {
        len -= 6;                                   //;去掉时间标签长度
    }
    /*   //;建三江软件抄读数据 引起死循环 --20141225
    if(SEQ & 0x80)                                   //;带有时间标签
    {
        len -= 6;                                   //;去掉时间标签长度
        if(*(R_Buf+19+len)>0)
        {
           long1=Time_distance(&TBCD.Sec, (u8*)(R_Buf + 15 + len));   //;tpv允许发送传输延时判断zrt20140107
           if (long1 < -(*(R_Buf + 19 + len)*60) || long1 > (*(R_Buf+19+len)*60) )
	         {
              return 0;
           }
        }
    }
     */
    *(S_Buf + 0) = *(S_Buf + 5) = 0x68;
    *(S_Buf + 6) = 0x88; //;DIR=1,PRM=0,ACD=0; CID=8
    *(S_Buf + 7) = *(R_Buf + 7);
    *(S_Buf + 8) = *(R_Buf + 8);
    *(S_Buf + 9) = *(R_Buf + 9);
    *(S_Buf + 10) = *(R_Buf + 10);
    *(S_Buf + 11) = *(R_Buf + 11); //;终端地址也是一样的
    *(S_Buf + 12) = *(R_Buf + 12); //;AFN一样
    *(S_Buf + 13) = 0x00; //;Tvp=0
    Ind_S = i = 14;
    Index_R_S = PreData_AFN0EH(Ind_S, i, S_Buf, R_Buf, GYCtrl);
    Ind_S = (u16)Index_R_S;

    if(GYCtrl->ERC_F == 1)
    //;第一帧
    {
        GYCtrl->ISeq = (*(R_Buf + 13)) &0x0f; //;暂存帧序号
        *(S_Buf + 13) |= 0x40;
    }
    if (GYCtrl->ERC_L == 1)
    //;最后一帧
    {
        *(S_Buf + 13) |= 0x20;
        GYCtrl->ERC_F = 0;
        GYCtrl->ERC_L = 0;
        GYCtrl->AFNList[0] = 0;
        GYCtrl->AFNNum = 0;
    }
    else
    {
        GYCtrl->AFNNum++;
        if(GYCtrl->AFNNum < NFRAME)  //;--@2
        {
           GYCtrl->AFNList[0] = 0x0E; //;事件分帧上送
        }
        else
        {  //;分帧次数不能超过MaxNum_Frame次
            *(S_Buf + 13) |= 0x20;
            GYCtrl->ERC_Index = 0;
            GYCtrl->ERC_F = 0;
            GYCtrl->ERC_L = 0;
            GYCtrl->AFNList[0] =  0;
            GYCtrl->AFNNum = 0;
        }
    }

    if (Ind_S <= 14)
    //;如果没有要求的数据，则用确认否认帧来全部否认
    {
        *(S_Buf + 12) = 0x00;
        Ind_S = 14;
        *(S_Buf + Ind_S++) = 00;
        *(S_Buf + Ind_S++) = 00;
        *(S_Buf + Ind_S++) = 02;
        *(S_Buf + Ind_S++) = 00;
        *(S_Buf + 13) = 0x60;
        GYCtrl->ERC_F = 0;
        GYCtrl->ERC_L = 0;
        GYCtrl->AFNList[0] = 0;
        GYCtrl->AFNNum = 0;
    }

    *(S_Buf + 13) |= (GYCtrl->ISeq &0x0f);
    GYCtrl->ISeq++; //;发生帧计数

    if(SEQ & 0x80)//;20131102
	{
		*(S_Buf + Ind_S++) = *(R_Buf + 14 + len);
                *(S_Buf + Ind_S++) = *(R_Buf + 15 + len);
                *(S_Buf + Ind_S++) = *(R_Buf + 16 + len);
                *(S_Buf + Ind_S++) = *(R_Buf + 17 + len);
                *(S_Buf + Ind_S++) = *(R_Buf + 18 + len);
		/**(S_Buf + Ind_S++) = TBCD.Sec;
		*(S_Buf + Ind_S++) = TBCD.Min;
		*(S_Buf + Ind_S++) = TBCD.Hour;
		*(S_Buf + Ind_S++) = TBCD.Day;*/
		*(S_Buf + Ind_S++) = *(R_Buf + 19 + len);;
                *(S_Buf + 13) |= 0x80;
	}      
	
    len = Ind_S - 6;
    len *= 4;
    len += 2;
    //;------数据域长度
    *(S_Buf + 1) = *(S_Buf + 3) =(u8)len;
    *(S_Buf + 2) = *(S_Buf + 4) = (u8)(len >> 8);
    //;------校验和
    *(S_Buf + Ind_S) = 0;
    for (i = 6; i < Ind_S; i++)
    {
        *(S_Buf + Ind_S) += *(S_Buf + i);
    }
    //;------结束字节
    *(S_Buf + Ind_S + 1) = 0x16;
    return (Ind_S + 2);
}


/* 
********************************************************************************
* 函 数 名：GDW_AFN0EH_Up
* 功    能：主动上报重要事件
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
u16 GDW_AFN0EH_Up(char *S_Buf, struct GuiYueCtrl *GYCtrl)
{

    u16 index_out, i, len, R_Index, Page, Area_ind, Len;
		u8 Pm, Y, k;
		static u8 ISeq;
		
    *(S_Buf + 0 ) = *(S_Buf + 5) = 0x68;
    *(S_Buf + 6 ) = 0xC4;                             //;DIR=1;PRM=1,ACD=0 CID=0
    *(S_Buf + 7 ) = ZDPKZ.F89.QuXian[0];
    *(S_Buf + 8 ) = ZDPKZ.F89.QuXian[1];              //;区县码
    *(S_Buf + 9 ) = ZDPKZ.F89.DiZhi[0];
    *(S_Buf + 10) = ZDPKZ.F89.DiZhi[1];          //;终端地址
    *(S_Buf + 11) = 0;                                //;主站MSA
    *(S_Buf + 12) = 0x0E;                             //;AFN，第3类数据 
    
    if(!ERCUp.ReSedFlag)
    {
        ISeq =  GYCtrl->ISeq;
    }
    *(S_Buf + 13) = ISeq & 0x0F;
    *(S_Buf + 13) |= 0x60;                            //;  Tpv=0;   FIR=1,FIN=1(单帧);
    
    
    GYCtrl->ISeq++;
    Pm = ERCUp.UpIndex;
    if(ZDP.F1.CON & 0x04)                              //;如果主动上报重要事件需要确认
    {
        *(S_Buf + 13) |= 0x10;
    }
    *(S_Buf + 14) = 0x00;                             //;P0
    *(S_Buf + 15) = 0x00;
    *(S_Buf + 16) = 0x01;                             //;F1重要事件
    *(S_Buf + 17) = 0x00;
    *(S_Buf + 18) = ZDS.F7[0];
    *(S_Buf + 19) = ZDS.F7[1];
    *(S_Buf + 20) = Pm;                             //;Pm=0;
    
    
    index_out = 22;
    if(Pm < ZDS.F7[0])
    {
        Y = ZDS.F7[0] - Pm; //;请求的事件个数
    }
    else
    {
        Y = ERCReadNum + ZDS.F7[0] - Pm;
    }
    
    for(i = 0; i < Y; i++)
    {
       /*----控制在保存的264点事件中读取最新的256点----*/
        if(ZDS.F7[0] == 0)
        //;当前有256或0个最新事件发生
        {
             if(ERC1.Index == 0)
             {
                ERCUp.SendERCNum = ZDS.F7[0];
                return 0; //;无事件发生
             }
             R_Index = (ERC1.StInd + Pm) % ERCPointNum; //;以StInd为基准可以读取0~255个最新事件
        }
        else
        //;当前最新发生的事件小于256个
        {
            if(Pm < ZDS.F7[0])
            //;当前需要读取的事件序号小于最新事件个数
            {
                R_Index = (ERC1.Index - ZDS.F7[0] + Pm) % ERCPointNum;  //;以Index倒退ECNum为基准读取新事件
            }
            else
            //;以 StInd 为基准读取旧事件
            {
                if((ERC1.Index - ERC1.StInd) >= ERCReadNum )
                {
                    R_Index = (ERC1.StInd + ERC1.Index % ERCPageNum + Pm - ZDS.F7[0]) % ERCPointNum ;
                }
                else
                {
                    ERCUp.SendERCNum = ZDS.F7[0];
                    return 0; //;无事件发生//;还没有开始覆盖，旧事件不存在
                }
            }
        }
      /*------------------------------------------------*/

        Page = ERC1Home + R_Index / ERCPagePoint; //;事件在Flash中的第几页
        
        Area_ind = R_Index % ERCPagePoint; //;该页的第几个区
        R_Index = Area_ind * ERCAreaNum; //;该页的具体位置
        SPI_FLASH_PageRead(AllBuf.Spi1, Page); //;读出该数据所在页的整页数据
        Len = *(AllBuf.Spi1 + R_Index + 1);
        if (Len < ERCAreaNum) //;事件记录数据长度不会超过 66 个字节     
        {
            if(index_out + Len + 2 < FRAMESIZE)
            //;写入下一个事件不超过最大帧
            {
                *(S_Buf + index_out++) = *(AllBuf.Spi1 + R_Index + 0); //;写入事件代码
                *(S_Buf + index_out++) = Len; //;写入事件长度
                for (k = 0; k < Len; k++)
                {
                    *(S_Buf + index_out++) = *(AllBuf.Spi1 + R_Index + 2 + k); //;读出数据
                }
                Pm++;
                Pm %= ERCPointNum;             
            }
           
        }
        else
        {
            ERCUp.SendERCNum = ZDS.F7[0]; //;最后一帧
            return 0; //;无事件发生; //;事件不存在
        }
    }
    
    *(S_Buf + 21) = Pm;                       //;Pn=1;
    ERCUp.SendERCNum = (Pm % ERCPointNum);
    len = index_out - 6;
    len *= 4;
    len += 2;
    *(S_Buf + 1) = *(S_Buf + 3) =(u8)len;
    *(S_Buf + 2) = *(S_Buf + 4) = (u8)(len >> 8);
    *(S_Buf + index_out) = 0;

    for (i = 6; i < index_out; i++)
    {
        *(S_Buf + index_out) += *(S_Buf + i);       //;校验和
    }

    *(S_Buf + index_out + 1) = 0x16;                  //;结束字节
    return (index_out + 2);
}


/* 
********************************************************************************
* 函 数 名：GDW_AFN0FH
* 功    能：文件传输处理
* 参    数：
* 返    回：失败返回否定帧。
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：2012-05-02 15:31:26
********************************************************************************
*/
u16 GDW_AFN0FH(char *R_Buf, char *S_Buf, struct GuiYueCtrl *GYCtrl)
{
    u16 i, len, frameind, size;
    u16 crc, crctemp,Pn;
    u8 SEQ, filesign, DA1, DA2, DT1, DT2, Fn;
    u32 filesize;
        
    len = *(R_Buf + 1) + (*(R_Buf + 2) << 8);
    len >>= 2;   //;用户数据区的总长度
    len -= 8;    //;数据单元长度=len-1(控制域)-5(地址域)-1(AFN)-1(SEQ)
    len -= 16; //;PW长度
    
    SEQ = *(R_Buf + 13); //;帧序列域
    if(SEQ & 0x80) //;减去TpV
    {
        len -= 6;
    } 
    
    DA1 = *(R_Buf + 14);
    DA2 = *(R_Buf + 15);
    DT1 = *(R_Buf + 16);
    DT2 = *(R_Buf + 17);

    Pn = Find_Pn(DA1, DA2);     //;;计算出Pn--信息点
    Fn = Find_Fn(DT1, DT2);     //;;计算出Fn--信息类

    if((Pn != 0x00) || (Fn != 0x02)) //;只支持自定义文件传输
    {
        goto ErrDeal; //; 终端无法识别该信息类
    }
    
    filesign = *(R_Buf + 18); //;文件标识
    
    if(*(R_Buf + 19) != 'B')
    {
        goto ErrDeal;
    }
    
    filesize =  *(R_Buf + 20) << 24; //;长度
    filesize += *(R_Buf + 21) << 16;
    filesize += *(R_Buf + 22) << 8;
    filesize += *(R_Buf + 23);
    if(filesize > PROG_FLASH_SIZE)
    {
        goto ErrDeal;
    }
    
    frameind =  *(R_Buf + 24) << 8; //;文件序号
    frameind += *(R_Buf + 25);   
    i = *(R_Buf + 26) << 8;
    i += *(R_Buf + 27);
    
    i = ~i;
    if(frameind != i)
    {
        goto ErrDeal;
    }
    
    
     //;本帧长度
    size =  *(R_Buf + 28) << 8;
    size += *(R_Buf + 29);
    if(size > 512)
    {
        goto ErrDeal;
    }
    
    if(SEQ & 0x40) //;首帧
    {
        GYCtrl->FileSign = filesign; 
        
        GYCtrl->FileSize = filesize; //;长度
        
        GYCtrl->FrameIndex = frameind;
        
        if(size != 512)
        {
            goto ErrDeal;
        }
        
        GYCtrl->PageCur = PROG0_PAGE_START;
        GYCtrl->SizeCur = 0x0;
    }
    if(SEQ & 0x20) //;末帧
    {
        if(frameind != 0xFFFF) //;最后一帧。
        {
            goto ErrDeal;
        }
        
    }
    if(!(SEQ & 0x60)) //;中间帧
    {
        if(size != 512)
        {
            goto ErrDeal;
        }
        
        if(GYCtrl->FileSign != filesign)
        {
            goto ErrDeal;
        }
        if(GYCtrl->FileSize != filesize)
        {
            goto ErrDeal;
        }
        if((GYCtrl->FrameIndex) != frameind)
        {
            goto ErrDeal;
        }
         
       
    }
    
    crctemp =  *(R_Buf + 30 + size) << 8;
    crctemp += *(R_Buf + 30 + size + 1);  
    crc = Cal_CRC16((u8 *)R_Buf + 30, size);
    if(crc == crctemp)
    {
        //;写入标识
        AllBuf.Spi1[0] = 0;
        AllBuf.Spi1[1] = 0;
        AllBuf.Spi1[2] = 0;
        AllBuf.Spi1[3] = 0;
        
        //;长度
        *(u32*)(AllBuf.Spi1 + 4) = filesize;
        //;序号
        *(u16*)(AllBuf.Spi1 + 8) = frameind;
        *(u16*)(AllBuf.Spi1 + 10) = ~frameind;
        //;文件内容及CRC内容
        MovN2Byte(size, (u8*)AllBuf.Spi1 + PAGE_HEAD_SIZE, (u8*)R_Buf + 30);
        
        if(frameind == 0xFFFF)
        {
            crc = Cal_CRC16(AllBuf.Spi1 + PAGE_HEAD_SIZE, PAGE_DATA_SIZE);
        }

        *(u16*)(AllBuf.Spi1 + PAGE_HEAD_SIZE + PAGE_DATA_SIZE) = crc;

        
        //;写入FLASH
        SPI_FLASH_PageWrite_E(AllBuf.Spi1, GYCtrl->PageCur++);
//;        
//;        for(i = 0; i < 200; i++)
//;        {
//;            AllBuf.Spi1[i] = 0x00;
//;        }
//;        
//;        SPI_FLASH_PageRead(AllBuf.Spi1, PROG0_PAGE_START);
        
        
        GYCtrl->SizeCur += size;
    }
    else
    {
        goto ErrDeal;
    }
    
    GYCtrl->FrameIndex++;
    if(frameind == 0xFFFF) //;最后一帧
    {     
        if(GYCtrl->SizeCur != GYCtrl->FileSize)
        {
            goto ErrDeal;
        }
        
 //;       SPI_FLASH_PageRead(AllBuf.Spi1, --GYCtrl->PageCur);
        
        
        
        SPI_FLASH_PageRead(AllBuf.Spi1, PROG0_PAGE_START);
        
        AllBuf.Spi1[0] = UPDATASIG0;
        AllBuf.Spi1[1] = UPDATASIG1;
        AllBuf.Spi1[2] = UPDATASIG2;
        AllBuf.Spi1[3] = UPDATASIG3;
        
        //;写入FLASH更新标识
        SPI_FLASH_PageWrite_E(AllBuf.Spi1, PROG0_PAGE_START);
        
        GYCtrl->FileSign = 0;
        GYCtrl->FileSize = 0;
        GYCtrl->FrameIndex = 0;
    }
    *(S_Buf + 16) = 0x01; //;AFN  
    
    *(S_Buf + 0 ) = *(S_Buf + 5) = 0x68;
    *(S_Buf + 6 ) = 0x88;                         //;DIR=1,PRM=0,ACD=0; CID=8
    *(S_Buf + 7 ) = *(R_Buf + 7 );        //;终端地址
    *(S_Buf + 8 ) = *(R_Buf + 8 );
    *(S_Buf + 9 ) = *(R_Buf + 9 );
    *(S_Buf + 10) = *(R_Buf + 10);
    *(S_Buf + 11) = *(R_Buf + 11);
    *(S_Buf + 13) = *(R_Buf + 13) & 0x0f; //;帧序列域SEQ
    *(S_Buf + 13) |= 0x60;                 //;Tpv=0;FIR=1,FIN=1,CON=0;
    *(S_Buf + 12) = 0x00; //;应用层功能码 AFN
    *(S_Buf + 14) = 0x00;
    *(S_Buf + 15) = 0x00; //;p0
    *(S_Buf + 17) = 0x00; //;f3

   
    //;数据域长度
    len = 12;
    len *= 4;
    len += 2;  
    *(S_Buf + 1) = *(S_Buf + 3) = (u8)len;
    *(S_Buf + 2) = *(S_Buf + 4) = (u8)(len>>8);
     
    //;校验和
    *(S_Buf + 18) = 0;
    for(i=6; i<18; i++)
    {
        *(S_Buf + 18) += *(S_Buf + i);
    }
    
    //;结束字节
    *(S_Buf + 19) = 0x16;
    return 20;
    
ErrDeal:
   
    //;            GYCtrl->PageCur--;
//;            GYCtrl->SizeCur -= size;

    *(S_Buf + 16) = 0x02; //;否定帧
 
    *(S_Buf + 0 ) = *(S_Buf + 5) = 0x68;
    *(S_Buf + 6 ) = 0x88;                         //;DIR=1,PRM=0,ACD=0; CID=8
    *(S_Buf + 7 ) = *(R_Buf + 7 );        //;终端地址
    *(S_Buf + 8 ) = *(R_Buf + 8 );
    *(S_Buf + 9 ) = *(R_Buf + 9 );
    *(S_Buf + 10) = *(R_Buf + 10);
    *(S_Buf + 11) = *(R_Buf + 11);
    *(S_Buf + 13) = *(R_Buf + 13) & 0x0f; //;帧序列域SEQ
    *(S_Buf + 13) |= 0x60;                 //;Tpv=0;FIR=1,FIN=1,CON=0;
    *(S_Buf + 12) = 0x00; //;应用层功能码 AFN
    *(S_Buf + 14) = 0x00;
    *(S_Buf + 15) = 0x00; //;p0
    *(S_Buf + 17) = 0x00; //;f3

    //;数据域长度
    len = 12;
    len *= 4;
    len += 2;  
    *(S_Buf + 1) = *(S_Buf + 3) = (u8)len;
    *(S_Buf + 2) = *(S_Buf + 4) = (u8)(len>>8);
     
    //;校验和
    *(S_Buf + 18) = 0;
    for(i=6; i<18; i++)
    {
        *(S_Buf + 18) += *(S_Buf + i);
    }
    
    //;结束字节
    *(S_Buf + 19) = 0x16;
    return 20;

}


/* 
********************************************************************************
* 函 数 名：GDW_AFN10H
* 功    能：处理转发数据命令
* 参    数：R_Buf-接收缓冲区；GYCtrl-规约控制字
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void GDW_AFN10H(char *R_Buf, struct GuiYueCtrl *GYCtrl)
{
    u8  t, DA1, DA2, DT1, DT2, Fn;
    u16 i, Pn;

    if(RelayC.Status != idle)
    {
        return;
    }
    
    if(GYCtrl == &LocGyC)
    {
        RelayC.Channel = 'L';
    }
    else if(GYCtrl == &GprsGyC)
    {
        RelayC.Channel = 'G';
    }
    
    RelayC.Port = *(R_Buf + 18);
    RelayC.BPS  = *(R_Buf + 19) >> 5;
    RelayC.A3   = *(R_Buf + 11);
    RelayC.SEQ  = *(R_Buf + 13);
    
    DA1 = *(R_Buf + 14);
    DA2 = *(R_Buf + 15);
    DT1 = *(R_Buf + 16);
    DT2 = *(R_Buf + 17);

    Pn = Find_Pn(DA1, DA2); //;计算出Pn--信息点
    Fn = Find_Fn(DT1, DT2); //;计算出Fn--信息类

    if((Pn != 0x00) || (Fn != 0x01)) //;暂支持F1信息类
    {
        RelayC.Status = error; //;否定
        return;
    }
    
    //if((RelayC.Port != 1) && (RelayC.Port != 2)) //20150103 关闭端口2允许配置透传
    if(RelayC.Port != 1)
    {
        RelayC.Status = error; //;否定
        return;
    }

    
    t = *(R_Buf + 20);
    if(t & 0x80)
    {
        RelayC.OvTime = (t & 0x7F) * 4; //;OvTime单位为250ms
    }
    else
    {
        RelayC.OvTime = (t & 0x7F) / 25; //;*10ms/250
    }
    
    if(RelayC.OvTime < 4)
    {
        RelayC.OvTime = 4; //;最短是1s
    }
    
    RelayC.Len   = *(u16*)(R_Buf + 22);
    
    if(RelayC.Len > 250)
    {
        return;
    }
    
    for(i = 0; i < RelayC.Len; i++)
    {
        RelayC.Data[i] = *(R_Buf + 24 + i);
    }

    for(i = 0; i < 6; i++)
    {
        RelayC.Tp[i] = *(R_Buf + 24 + RelayC.Len + 16 + i); //20131105
    }	
   
    
    RelayC.Status = 'E'; //;占用中继控制字
}


/* 
********************************************************************************
* 函 数 名：GDW_AFN10H_Up
* 功    能：应答数据转发命令
* 参    数：S-Buf-发送缓冲区；GYCtrl-规约控制字
* 返    回：要发送的数据长度
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
u16  GDW_AFN10H_Up(char *S_Buf, struct GuiYueCtrl *GYCtrl)
{
    u8  i;
    u16 len, Ind_S;

    *(S_Buf + 0) = *(S_Buf + 5) = 0x68;
    *(S_Buf + 6) = 0x88; //;DIR=1,PRM=0,ACD=0; CID=8

    /*终端地址也是一样的*/
    *(S_Buf + 7 ) = ZDPKZ.F89.QuXian[0];
    *(S_Buf + 8 ) = ZDPKZ.F89.QuXian[1];
    *(S_Buf + 9 ) = ZDPKZ.F89.DiZhi[0];
    *(S_Buf + 10) = ZDPKZ.F89.DiZhi[1];
    *(S_Buf + 11) = RelayC.A3;
    *(S_Buf + 13) = RelayC.SEQ & 0x0f; //;帧序列域SEQ
    *(S_Buf + 13) |= 0x60;             //;Tpv=0;FIR=1,FIN=1,CON=0;

    if(RelayC.Status == error)
    {
        *(S_Buf + 12) = 0x00; //;应用层功能码 AFN
        *(S_Buf + 14) = 0x00;  //;P0
        *(S_Buf + 15) = 0x00;
        *(S_Buf + 16) = 0x02;  //;F2全部否认
        *(S_Buf + 17) = 0x00;
        Ind_S = 18;
    }
    else if(RelayC.Status == right)
    {
        *(S_Buf + 12) = 0x10; //;应用层功能码 AFN
        *(S_Buf + 14) = 0x00;  //;P0
        *(S_Buf + 15) = 0x00;
        *(S_Buf + 16) = 0x01;  //;F1
        *(S_Buf + 17) = 0x00;
        *(S_Buf + 18) = RelayC.Port;
        *(S_Buf + 19) = (u8)RelayC.Len;
        *(S_Buf + 20) = (u8)(RelayC.Len >> 8);
        EW.MoveByte = 'M';
        MovNByte(RelayC.Len, (u8*)S_Buf + 21, (u8*)RelayC.Data);
        EW.MoveByte = 0;
//        Ind_S = RelayC.Len + 19;   //;少2个字节  20131106
        Ind_S = RelayC.Len + 21;		
    }

    if(RelayC.SEQ & 0x80)//;20131104
	{
		*(S_Buf + Ind_S++) = RelayC.Tp[0];
                *(S_Buf + Ind_S++) = RelayC.Tp[1];
                *(S_Buf + Ind_S++) = RelayC.Tp[2];
                *(S_Buf + Ind_S++) = RelayC.Tp[3];
                *(S_Buf + Ind_S++) = RelayC.Tp[4];
		/**(S_Buf + Ind_S++) = TBCD.Sec;
		*(S_Buf + Ind_S++) = TBCD.Min;
		*(S_Buf + Ind_S++) = TBCD.Hour;
		*(S_Buf + Ind_S++) = TBCD.Day;*/
		*(S_Buf + Ind_S++) = RelayC.Tp[5];
                *(S_Buf + 13) |= 0x80;
	}     	

    len = Ind_S - 6;
    len <<= 2;
    len += 2;
    //;------数据域长度
    *(S_Buf + 1) = *(S_Buf + 3) =(u8)len;
    *(S_Buf + 2) = *(S_Buf + 4) = (u8)(len >> 8);
    *(S_Buf + Ind_S) = 0;
    //;------校验和
    for(i = 6; i < Ind_S; i++)
    {
        *(S_Buf + Ind_S) += *(S_Buf + i);
    }
    //;------结束字节
    *(S_Buf + Ind_S + 1) = 0x16;
    return (Ind_S + 2);
}


//;/*----------------------------------------------------------------------------*
//; |  收到主站确认命令的处理
//; *----------------------------------------------------------------------------*/
//;void GDW_AFN00H(void)
//;{
//;    if (!(GprsC.State & Login))                                            //;登陆成功后处理心跳
//;    {
//;
//;    }
//;    else
//;    {
//;        if (GprsUp.Status == 'W')                                          //;事件等待应答
//;        {
//;            GprsUp.Status = 'R';                                           //;收到应答
//;        }
//;        if (GprsC.HB_TimeStart == true)
//;        {
//;            GprsC.HB_TimeStart = false;                                     //;心跳回复正常
//;            GprsC.HB_Time = 0;
//;        }
//;    }
//;}


/*----------------------------------------------------------------------------*
 |  收到主站的确认命令
 *----------------------------------------------------------------------------*/
void GDW_AFN00H(struct GuiYueCtrl *GYCtrl)
{
    if(GYCtrl == &GprsGyC)
    {
        if(GprsC.LoginIn == 'S')
        {
            GprsC.LoginIn = 'R';
        }
        else if(GprsC.HBStatus == 'S')
        {
            GprsC.HBStatus = 'R';
        }
        else if(ERCUp.Status == 'S')
        {
            ERCUp.Status = 'R';
        }
        else if(GprsC.LoginOut == 'S')
        {
            GprsC.LoginOut = 'R';
        }
    }
    else if(GYCtrl == &LocGyC)
    {
        if(ERCUp.Status == 'S')
        {
            ERCUp.Status = 'R';
        }
    }
}


 /* 
********************************************************************************
* 函 数 名：GDW_AFN01H
* 功    能：对 AFN=0x01 的帧进行处理   复位命令
* 参    数：
* 返    回： 
* 说    明：响应
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
u16 GDW_AFN01H(char *R_Buf, char *S_Buf)
{
    u16 i, len, Ind_S;
    u8 Fn, Pn, DA1, DA2, DT1, DT2;
    u8 Set = false,SEQ;
    u8 Legalize = true;
    //s32 long1=0;

    len = *(R_Buf + 1) + (*(R_Buf + 2)<<8);
    len >>= 2; //;用户数据区的总长度

    len -= 8; //;链路用户数据层的总长度,去掉控制域和地址域，再去掉AFN 和 SEQ 两个字节以后的数据长度
 
    SEQ = *(R_Buf + 13);
    if(SEQ & 0x80)                                   //;带有时间标签
    {
        len -= 6;                                   //;去掉时间标签长度
    }    
    /*
    if(SEQ & 0x80)                                   //;带有时间标签
    {
        len -= 6;                                   //;去掉时间标签长度
        if(*(R_Buf+19+len)>0)
        {
           long1=Time_distance(&TBCD.Sec, (u8*)(R_Buf + 15 + len));   //;tpv允许发送传输延时判断zrt20140107
           if (long1 < -(*(R_Buf + 19 + len)*60) || long1 > (*(R_Buf+19+len)*60) )
	         {
              return 0;
           }
        }
    } 
    */	
    
    //;Legalize = CalcCRC(R_Buf+6, R_Buf+6+len-2); //;消息认证

    *(S_Buf + 0) = *(S_Buf + 5) = 0x68;
    *(S_Buf + 6) = 0x88; //;DIR=1,PRM=0,ACD=0; CID=8

    /*终端地址也是一样的*/
    *(S_Buf + 7 ) = *(R_Buf + 7 );
    *(S_Buf + 8 ) = *(R_Buf + 8 );
    *(S_Buf + 9 ) = *(R_Buf + 9 );
    *(S_Buf + 10) = *(R_Buf + 10);
    *(S_Buf + 11) = *(R_Buf + 11);

    *(S_Buf + 12) = 0x00; //;应用层功能码 AFN

    *(S_Buf + 13) = *(R_Buf + 13) &0x0f; //;帧序列域SEQ
    *(S_Buf + 13) |= 0x60;                       //;Tpv=0;FIR=1,FIN=1,CON=0;
    
  

    if(Legalize == true)
    {
        DA1 = *(R_Buf + 14);
        DA2 = *(R_Buf + 15);
        DT1 = *(R_Buf + 16);
        DT2 = *(R_Buf + 17);
        Pn = Find_Pn(DA1, DA2); //;计算出Pn--信息点
        Fn = Find_Fn(DT1, DT2); //;计算出Fn--信息类

        if(Pn == 0)
        {
            if(Fn == 1)
            {
                ZD_HardWareReset();
                Set = true;
            }
            else if(Fn == 2)
            {
                ZD_DataInit();        //;全体数据区初始化
                Set = true;
                HWareC.FactoryInit ='E';  //;恢复出厂配置标记				
            }
            else if(Fn == 3)
            {              
                ZD_CSDataInit();        //;参数及全体数据区初始化
                Set = true;
                HWareC.FactoryInit ='E';  //;恢复出厂配置标记
				
            }
            else if(Fn == 4)
            {
                ZD_NCCSDataInit();
                Set = true;
                HWareC.FactoryInit ='E';  //;恢复出厂配置标记
				
            }
        }
    }

    if(Set == true)
    {
        *(S_Buf + 14) =0x00;  //;P0
        *(S_Buf + 15) =0x00;
        *(S_Buf + 16) =0x01;  //;F1全部确认
        *(S_Buf + 17) =0x00;
    }
    else
    {
        *(S_Buf + 14) =0x00;  //;P0
        *(S_Buf + 15) =0x00;
        *(S_Buf + 16) =0x02;  //;F2全部否认
        *(S_Buf + 17) =0x00;
    }

    Ind_S = 18;	

    if(SEQ & 0x80)//;20131104
	{
           *(S_Buf + Ind_S++) = *(R_Buf + 14 + len);
           *(S_Buf + Ind_S++) = *(R_Buf + 15 + len);
           *(S_Buf + Ind_S++) = *(R_Buf + 16 + len);
           *(S_Buf + Ind_S++) = *(R_Buf + 17 + len);
           *(S_Buf + Ind_S++) = *(R_Buf + 18 + len);
           *(S_Buf + Ind_S++) = *(R_Buf + 19 + len);
           *(S_Buf + 13) |= 0x80;
	}        	
    

    len = Ind_S - 6;
    len <<= 2;
    len += 2;  //;本规约用标识
    
    *(S_Buf + 1) = *(S_Buf + 3) =(u8)len; //;数据域长度
    *(S_Buf + 2) = *(S_Buf + 4) = (u8)(len >> 8);

    *(S_Buf + Ind_S) = 0;
    
    //;校验和
    for(i=6; i<Ind_S; i++)
    {
        *(S_Buf + Ind_S) += *(S_Buf + i);
    }
    
    *(S_Buf + Ind_S + 1) = 0x16; //;结束字节
    return (Ind_S + 2);
}

/* 
********************************************************************************
* 函 数 名：GDW_AFN02H
* 功    能：链路接口检测
* 参    数：
* 返    回： 
* 说    明：启动帧
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
u8 GDW_AFN02H(char *R_Buf, struct GuiYueCtrl *GYCtrl)
{
    u8 i;

    *(R_Buf + 0) = 0x68;
    *(R_Buf + 1) = 0x32;
    *(R_Buf + 2) = 0;
    *(R_Buf + 3) = 0x32;
    *(R_Buf + 4) = 0;
    *(R_Buf + 5) = 0x68;
    
    *(R_Buf + 6) = 0xC9;  //;控制域C [c9]--->DIR=1,PRM=1,ACD=0,CID=9
    
    *(R_Buf + 7) = ZDPKZ.F89.QuXian[0];  //;行政区划码
    *(R_Buf + 8) = ZDPKZ.F89.QuXian[1];
    *(R_Buf + 9) = ZDPKZ.F89.DiZhi[0];
    *(R_Buf + 10) = ZDPKZ.F89.DiZhi[1];  //;终端地址
    
    *(R_Buf + 11) = 0;   //;主站地址和组标志[00]--->主站地址(0),组标志(0)
    *(R_Buf + 12) = 0x02;                  //;功能码
    *(R_Buf + 13) = (GYCtrl->ISeq) & 0x0F; //;SEQ
    *(R_Buf + 13) |= 0x70;                 //;帧序列域[70]TpV=0,FIR=1,FIN=1,CON=1
    GYCtrl->ISeq++;
    *(R_Buf + 14) = 0x00;                     //;数据单元标识
    *(R_Buf + 15) = 0x00;
    *(R_Buf + 16) = GYCtrl->AFN02_FN;
    *(R_Buf + 17) = 0x00;
    *(R_Buf + 18) = 0x00;                     //;校验和

    for(i=6; i<18; i++)
    {
        *(R_Buf + 18) += *(R_Buf + i);
    }
    
    *(R_Buf + 19) = 0x16;
    
    return 20;
}

/* 
********************************************************************************
* 函 数 名：PreData_AFN05H
* 功    能：处理控制命令
* 参    数：
* 返    回： u32 --高16位为发送缓冲位置，低16为为接收缓冲位置
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：2011-09-15 20:18:08
********************************************************************************
*/
u32 PreData_AFN05H(u16 Ind_S, u16 Ind_R, char *S_Buf, char *R_Buf)
{
    u8 Fn, FnI, DA1, DA2, DT1, DT2;
    u16 Pn;
    u32 Index_R_S;

    *(S_Buf + Ind_S++) = DA1 = *(R_Buf + Ind_R++);
    *(S_Buf + Ind_S++) = DA2 = *(R_Buf + Ind_R++);
    *(S_Buf + Ind_S++) = DT1 = *(R_Buf + Ind_R++);
    *(S_Buf + Ind_S++) = DT2 = *(R_Buf + Ind_R++);

    Pn = Find_Pn(DA1, DA2); //;计算出Pn--信息点
    FnI = Find_FnI(DT1); //;计算出Fn--信息类
    Fn = Find_Fn(DT1, DT2); //;计算出Fn--信息类

    *(S_Buf + Ind_S) = 1; //;首先假设该参数设置不正确

    if((Pn==0xff) || (Fn==0xff) || (FnI==0xff)) //;Pn或者Fn表示错误    
    {
        Ind_S -= 4;
    }
    else
    {
        /*对时命令*/
        if(Fn == 31)
        {
            ZD_Time_Set((u8*)R_Buf + Ind_R);
            Send_TimetoMeter();
            Ind_R += 6;
            *(S_Buf + Ind_S) = 0;
        }

        /* 激活连接 */
        else if(Fn == 38)
        {
            if((ZDP.F8.WorkMode & 0x03) == 0x02)
            {
                GprsLC.PassLinkOrd = true; //;接收到连接命令
                GprsLC.PassLinkNum = ZDP.F8.RedailNum;
                *(S_Buf + Ind_S) = 0;  //;命令正确
            }
            else
            {       
                if(!GprsLC.Link2Switch)
                {
                    GprsLC.Link2Switch = true;
                    //;将备用主站IP及端口号写入
                    GprsLC.IP1_long = NumIpToChar((char *)GprsLC.IP1, 
                                                  ZDP.F3.IP2, 
                                                  *(u16 *)ZDP.F3.Port2);
                    
                    *(S_Buf + Ind_S) = 0;  //;命令正确
                }         
            }
        }
/*
	 else if(Fn == 39)  //;命令断开连接by zrt20130912
	 {
               u8 i = 0;       
               strcpy(GprsC.ATStr,"AT+TCPCLOSE=");
               i += 12;
               *(GprsC.ATStr + i++) = 0 + 0x30;
               *(GprsC.ATStr + i++) = '\r';
               *(GprsC.ATStr + i++) = '\0';
               Modem_AT_Send(GprsC.ATStr);
               *(S_Buf + Ind_S) = 0;  //;命令正确			   
	 }
*/	 
    }
    Ind_S++;
    Index_R_S = Ind_R << 16;
    Index_R_S += Ind_S;
    return (Index_R_S);
}

/* 
********************************************************************************
* 函 数 名：GDW_AFN05H
* 功    能：控制命令
* 参    数：
* 返    回： 
* 说    明：响应帧
* 抛出异常：
* 作    者：
* 修改记录：2011-09-15 20:14:19
********************************************************************************
*/
u16 GDW_AFN05H(char *R_Buf, char *S_Buf)
{
    u16 i, len, Ind_S;
    u32 Index_R_S;
    u8 right_num = 0;
    u8 error_num = 0;
    u8 Legalize = true;
    u8 SEQ;
    //s32 long1=0;

    SEQ = *(R_Buf + 13); //;帧序列域
    
    len = *(R_Buf + 1) + (*(R_Buf + 2) << 8);
    len >>= 2; //;用户数据区长度
    len -= 8;  //;数据单元长度=len-1(控制域)-5(地址域)-1(AFN)-1(SEQ)
    len -= 16; //;PW长度
    if(SEQ & 0x80) //;减去TpV
    {
        len -= 6;
    }
    /*
    if(SEQ & 0x80)                                   //;带有时间标签
    {
        len -= 6;                                   //;去掉时间标签长度
        if(*(R_Buf+19+len+16)>0)
        {
           long1=Time_distance(&TBCD.Sec, (u8*)(R_Buf + 15 + len+16));   //;tpv允许发送传输延时判断zrt20140107
           if (long1 < -(*(R_Buf + 19 + len+16)*60) || long1 > (*(R_Buf+19+len+16)*60) )
	   {
              return 0;
           }
        }
    }
	  */
    //;Legalize = CalcCRC(R_Buf + 6, R_Buf + 6 + len - 2 ); //;消息认证
    if(Legalize == false)  //;认证失败
    {
        return 0;
    }       
         
    Ind_S = 19;  
    for(i=14; i<len+14; ) //;处理控制命令
    {
        Index_R_S = PreData_AFN05H(Ind_S, i, S_Buf, R_Buf);
        
        if((Index_R_S&0xffff) < FRAMESIZE)
        {
            Ind_S = (u16)Index_R_S;
            i = Index_R_S >> 16;
        }
        else
        {
            break;
        }
    }
    
    if(!(SEQ & 0x10)) //;CON--不需要确认
    {
        return 0;
    }
    
    for(i=23; i<Ind_S; i+=5)
    {
        if(*(S_Buf + i) == 0x00)
        {
            right_num++;
        }
        else if(*(S_Buf + i) == 0x01)
        {
            error_num++;
        }
    }
    
    if(right_num == 0) //;全部错误，用全部否认来应答    
    {
        Ind_S = 18;
        *(S_Buf + 16) = 0x02; //;AFN
    }
    else if(error_num == 0) //;全部正确，用全部确认来应答    
    {
        Ind_S = 18;
        *(S_Buf + 16) = 0x01; //;AFN
    }
    else //;根据数据单元标识确认和否认
    {
        *(S_Buf + 16) = 0x04; 
        *(S_Buf + 18) = 0x05; //;AFN
    }
    
    
    
    *(S_Buf + 0) = *(S_Buf + 5) = 0x68; 
    *(S_Buf + 6) = 0x80; //;DIR=1,PRM=0,ACD=0; CID=0  
    *(S_Buf + 7 ) = *(R_Buf + 7 ); //;终端地址
    *(S_Buf + 8 ) = *(R_Buf + 8 );
    *(S_Buf + 9 ) = *(R_Buf + 9 );
    *(S_Buf + 10) = *(R_Buf + 10);
    *(S_Buf + 11) = *(R_Buf + 11);   
    *(S_Buf + 12) = 0x00; //;应用层功能码 AFN
    *(S_Buf + 13) = *(R_Buf + 13) &0x0f; //;帧序列域SEQ
    *(S_Buf + 13) |= 0x60;               //;Tpv=0;FIR=1,FIN=1,CON=0;
    
    *(S_Buf + 14) = 0x00;//;p0
    *(S_Buf + 15) = 0x00; 
    *(S_Buf + 17) = 0x00; //;F3 按数据标识确认

    if(SEQ & 0x80)//;20131102
	{
		*(S_Buf + Ind_S++) = *(R_Buf + 14 + len + 16);
                *(S_Buf + Ind_S++) = *(R_Buf + 15 + len + 16);
                *(S_Buf + Ind_S++) = *(R_Buf + 16 + len + 16);
                *(S_Buf + Ind_S++) = *(R_Buf + 17 + len + 16);
                *(S_Buf + Ind_S++) = *(R_Buf + 18 + len + 16);
		/**(S_Buf + Ind_S++) = TBCD.Sec;
		*(S_Buf + Ind_S++) = TBCD.Min;
		*(S_Buf + Ind_S++) = TBCD.Hour;
		*(S_Buf + Ind_S++) = TBCD.Day;*/
		*(S_Buf + Ind_S++) = *(R_Buf + 19 + len + 16);;
                *(S_Buf + 13) |= 0x80;
	}  	
    
    //;数据域长度
    len = Ind_S - 6; //;减去长度和标识
    len *= 4;
    len += 2;        //;376标识
    *(S_Buf + 1) = *(S_Buf + 3) = (u8)len;
    *(S_Buf + 2) = *(S_Buf + 4) = (u8)(len >> 8);
    
    //;校验和
    *(S_Buf + Ind_S) = 0;   
    for(i=6; i<Ind_S; i++)
    {
        *(S_Buf + Ind_S) += *(S_Buf + i);
    }
    //;------结束字节
    *(S_Buf + Ind_S + 1) = 0x16;
    return (Ind_S + 2);    
}


/* 
********************************************************************************
* 函 数 名：PreData_AFN0CH
* 功    能：为应答AFN=0x0C 的命令准备数据    读一类数据
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
u32 PreData_AFN0CH(u16 Ind_S, u16 Ind_R, char *S_Buf, char *R_Buf)
{
    u8  i, j;
    u8  Fn, FnI, DA1, DA2, DT1, DT2;  
    u8 T, N, L; 
    u8  *Adr8;
    u16 Pn;
    s16 index;
    u32 *Adr32;
    u32 Index_R_S;
    
    *(S_Buf + Ind_S++) = DA1 = *(R_Buf + Ind_R++);
    *(S_Buf + Ind_S++) = DA2 = *(R_Buf + Ind_R++);
    *(S_Buf + Ind_S++) = DT1 = *(R_Buf + Ind_R++);
    *(S_Buf + Ind_S++) = DT2 = *(R_Buf + Ind_R++);

    Pn = Find_Pn(DA1, DA2); //;计算出Pn--信息点
    FnI = Find_FnI(DT1); //;计算出Fn--信息类
    Fn = Find_Fn(DT1, DT2); //;计算出Fn--信息类

    index = ZD_Pn_Find(Pn);
    if((Pn==0xFFFF) || (Fn==0xFF) || (FnI==0xFF)||(index<0)) //;Pn或者Fn表示错误
    {
        Ind_S -= 4;
        Index_R_S = Ind_R << 16;
        Index_R_S += Ind_S;
        return (Index_R_S);
    }
 
    Adr8 = (u8*)PN0CZU + Fn * 8;
    T =  *Adr8; //;存储类型
    N = *(Adr8 + 1); //;长度
    L = *(Adr8 + 2); //;类型
    
    if(N == 0)  //;清除不存在的标识也能返回否认帧 20141225
    {
        Ind_S -= 4;
        Index_R_S = Ind_R << 16;
        Index_R_S += Ind_S;
        return (Index_R_S);
    }
    Adr32 = (u32*)(Adr8 + 4);
    Adr8 = (u8*)(*Adr32);
    if(T == 1)       //;类型1--对应终端1个参数
    {
        if(Pn != 0)
        {
            Ind_S -= 4; //;错误
        }
        else
        {
            for(i=0; i<N; i++)
            {               
                *(S_Buf + Ind_S++) = *(Adr8 + i);
            }
            
        }
    }
    else if(T == 5)  //;类型5--对应测量点的信息类
    {
        u32    *Addr32;
        u32    Offset = 0;
        struct PointDataStr *PnD;

        Addr32 = (u32*)(*(PnDTAddr + index));
        PnD = (struct PointDataStr*)(*(Addr32 + _PnD));

        Offset = *(Adr32 + 0); //;在结构中的偏移
        if(L & 0x01) //;写入抄表时间
        {
            *(S_Buf + Ind_S++) = PnD->CB_Time[0];
            *(S_Buf + Ind_S++) = PnD->CB_Time[1];
            *(S_Buf + Ind_S++) = PnD->CB_Time[2];
            *(S_Buf + Ind_S++) = PnD->CB_Time[3];
            *(S_Buf + Ind_S++) = PnD->CB_Time[4];
        }
        if(L & 0x02) //;写入费率
        {
            *(S_Buf + Ind_S++) = 4;
        }
        
        if(Fn>=145&&Fn<=152)     //;此范围的Fn格式需要调整 by zrt 20130808
        {
            for(i=0; i<5; i++)
            {
              for(j=0; j<3; j++)
              {
                  *(S_Buf + Ind_S++) = *(PnD->Start + Offset + j + 3*i);
              }
              for(j=0; j<4; j++)
              {
                  *(S_Buf + Ind_S++) = *(PnD->Start + Offset + j + 15 + 4*i);
              }
            }
        }
        else
        {
            for (i=0; i< N; i++)
            {
                *(S_Buf + Ind_S++) = *(PnD->Start + Offset + i);
            }
        }
/*
        for (i=0; i< N; i++)
        {
            *(S_Buf + Ind_S++) = *(PnD->Start + Offset + i);
        }        
*/       
    }
    else if(T == 6)  //;类型6--对应测量点小时冻结参数，读的是上一整点的小时冻结
    {
        u32 *Addr32;
        u32 Offset ;
        struct PointDataStr *PnD;

        Addr32 = (u32*)(*(PnDTAddr + index));
        PnD = (struct PointDataStr*)(*(Addr32 + _PnD));

        Offset = *(Adr32 + 0); //;在结构中的偏移
       
        for(i=0; i<N; i++)
        {
            *(S_Buf + Ind_S++) = *(PnD->Start + Offset + i);
        }

    }
    else
    {
        Ind_S -= 4;
    }

    Index_R_S = Ind_R << 16;
    Index_R_S += Ind_S;
    return (Index_R_S);
}


/*******************************************************************************
 * 函数名称:
 * 功能    :  为应答AFN=0x0E 的命令准备数据
 * 输入    :
 * 输出    :
 * 返回    :
 *******************************************************************************/
u32 PreData_AFN0EH(u16 Ind_S, u16 Ind_R, char *S_Buf, char *R_Buf, struct GuiYueCtrl *GYCtrl)
{
    u16 i, j, Area_ind, Index, StInd, R_Index, Len, Page;
    u8 Fn, Pn, FnI, DA1, DA2, DT1, DT2, ECNum;
    u32 Index_R_S;
    s16 Pn1, Pm1, Y = 0; //;Pn1是请求事件记录结束指针，Pm1是请求事件记录起始指针
    s16 Homepage;

    *(S_Buf + Ind_S++) = DA1 = *(R_Buf + Ind_R++);
    *(S_Buf + Ind_S++) = DA2 = *(R_Buf + Ind_R++);
    *(S_Buf + Ind_S++) = DT1 = *(R_Buf + Ind_R++);
    *(S_Buf + Ind_S++) = DT2 = *(R_Buf + Ind_R++);
    Pn = Find_Pn(DA1, DA2); //;计算出Pn--信息点
    FnI = Find_FnI(DT1);    //;计算出Fn--信息类
    Fn = Find_Fn(DT1, DT2); //;计算出Fn--信息类
    if ((Pn == 0xff) || (Fn == 0xff) || (FnI == 0xff))
    //;Pn或者Fn表示错误
    {
        Ind_R -= 3;
        Ind_S -= 4;
    }
    else if ((Fn == 1) || (Fn == 2))
    {
        *(S_Buf + Ind_S++) = ZDS.F7[0]; //;当前重要计数器
        *(S_Buf + Ind_S++) = ZDS.F7[1]; //;当前一般计数器
        *(S_Buf + Ind_S++) = Pm1 = *(R_Buf + Ind_R++);
        *(S_Buf + Ind_S++) = Pn1 = *(R_Buf + Ind_R++);

        if (Pm1 < Pn1)
        {
            Y = Pn1 - Pm1; //;请求的事件个数
        }
        if (Pm1 > Pn1)
        {
            Y = ERCReadNum + Pn1 - Pm1;
        }
        if (Fn == 1)
        {
            StInd = ERC1.StInd;  //;事件保存的起始位置
            Index = ERC1.Index ; //;事件保存的结束位置
            Homepage = ERC1Home; //;事件在Flash中的起始页
            ECNum = ZDS.F7[0];  //;当前被覆盖的事件个数
        }
        else if (Fn == 2)
        {
            StInd = ERC2.StInd;
            Index = ERC2.Index;
            Homepage = ERC2Home;
            ECNum = ZDS.F7[1];
        }
        /*--- 分帧处理 ---*/
        i = GYCtrl->ERC_Index;
        if (i == 0)
        {
            GYCtrl->ERC_F = 1; //;第一帧
        }
        else
        {
            GYCtrl->ERC_F = 0; //;中间帧
        }
        if(Y == 0)
        {
            GYCtrl->ERC_L = 1; //;最后一帧
        }
        Pm1 += i;

        for (  ; i < Y; i++)
        {
           /*----控制在保存的264点事件中读取最新的256点----*/
            if (ECNum == 0)
            //;当前有256或0个最新事件发生
            {
                 if(Index == 0)
                 {
                     GYCtrl->ERC_L = 1; //;最后一帧
                     break; //;无事件发生
                 }
                 R_Index = (StInd + Pm1) % ERCPointNum; //;以StInd为基准可以读取0~255个最新事件
            }
            else
            //;当前最新发生的事件小于256个
            {
                if( Pm1 < ECNum )
                //;当前需要读取的事件序号小于最新事件个数
                {
                    R_Index = (Index - ECNum + Pm1) % ERCPointNum;  //;以Index倒退ECNum为基准读取新事件
                }
                else
                //;以 StInd 为基准读取旧事件
                {
                    if((Index - StInd) >= ERCReadNum )
                    {
                        R_Index = (StInd + Index % ERCPageNum + Pm1 - ECNum) % ERCPointNum ;
                    }
                    else
                    {
                        GYCtrl->ERC_L = 1; //;最后一帧
                        break;//;还没有开始覆盖，旧事件不存在
                    }
                }
            }
          /*------------------------------------------------*/

            Page = Homepage + R_Index / ERCPagePoint; //;事件在Flash中的第几页
            
            Area_ind = R_Index % ERCPagePoint; //;该页的第几个区
            R_Index = Area_ind * ERCAreaNum; //;该页的具体位置
            SPI_FLASH_PageRead(AllBuf.Spi1, Page); //;读出该数据所在页的整页数据
            Len = *(AllBuf.Spi1 + R_Index + 1);
            if (Len < ERCAreaNum)
            //;事件记录数据长度不会超过 66 个字节
            {
                if(Ind_S + Len + 2 < FRAMESIZE)
                //;写入下一个事件不超过最大帧
                {
                    *(S_Buf + Ind_S++) = *(AllBuf.Spi1 + R_Index + 0); //;写入事件代码
                    *(S_Buf + Ind_S++) = Len; //;写入事件长度
                    for (j = 0; j < Len; j++)
                    {
                        *(S_Buf + Ind_S++) = *(AllBuf.Spi1 + R_Index + 2 + j); //;读出数据
                    }
                    Pm1++;
                    Pm1 %= ERCPointNum;
                    GYCtrl->ERC_Index = 0;
                    GYCtrl->ERC_L = 1; //;最后一帧
                }
                else
                //;需要分帧
                {
                    GYCtrl->ERC_Index = i; //;保存作为下一次发生的开始
                    GYCtrl->ERC_L = 0; //;非最后一帧
                    break;
                }
            }
            else
            {
                GYCtrl->ERC_L = 1; //;最后一帧
                break; //;事件不存在
            }
        }
    }
    else
    {
        Ind_S -= 4;
    }
    Index_R_S = Ind_R << 16;
    Index_R_S += Ind_S;
    return (Index_R_S);
}


/* 
********************************************************************************
* 函 数 名：GDW_AFN_CHK
* 功    能：检查帧报文是否符合376.1规约
* 参    数：R_Buf-接收数据缓冲区
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
u8 GDW_AFN_CHK(char *R_Buf)
{
    u8 sum;
    u16 i, len;

    if((*(R_Buf + 0) != 0x68) || (*(R_Buf + 5) != 0x68))
    {
        return false; //;;帧头不正确
    }

    if((*(R_Buf + 1) != *(R_Buf + 3)) || (*(R_Buf + 2) != *(R_Buf + 4)))
    {
        return false; //;;长度不正确
    }

    if((*(R_Buf + 7) != ZDPKZ.F89.QuXian[0]) || (*(R_Buf + 8) != ZDPKZ.F89.QuXian[1]))
    {
        return false; //;;行政区划码不正确
    }

    if(!(ZDPKZ.F89.DiZhi[0] == *(R_Buf + 9) && ZDPKZ.F89.DiZhi[1] == *(R_Buf + 10)) &&
        !(*(R_Buf + 9) == 0xff && *(R_Buf + 10) == 0xff && (*(R_Buf + 11) & 0x01)))
    {
        return false; //;;终端地址不正确，也不是广播地址
    }

    len = *(R_Buf + 1) + *(R_Buf + 2) * 0x100;
    len >>= 2;

    if(len > 550)
    {
        return false;
    }

    sum = 0;

    for(i = 6; i < len + 6; i++)
    {
        sum += *(R_Buf + i);
    }

    if(sum != *(R_Buf + len + 6))
    {
        return false; //;;校验和不相等
    }

    return true;
}


/* 
********************************************************************************
* 函 数 名：GDW_AFN_CHK_HW
* 功    能：红外通讯帧头检查
* 参    数：R_Buf-接收到的数据
* 返    回：-1-数据帧错误；其他-第一个68出现的位置
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
s8 GDW_AFN_CHK_HW(char *R_Buf, u16 BufLen)
{
    u8 sum;
    u16 i, len;
    s8 sign = 0;
    
    while((*(R_Buf + sign) != 0x68) && (sign < BufLen))
    {
        sign++;
    }
    
    R_Buf += sign;
    if((*(R_Buf + 0) != 0x68) || (*(R_Buf + 5) != 0x68))
    {
        return -1; //;帧头不正确
    }

    if((*(R_Buf + 1) != *(R_Buf + 3)) || (*(R_Buf + 2) != *(R_Buf + 4)))
    {
        return -1; //;长度不正确
    }
    
     len = *(R_Buf + 1) + *(R_Buf + 2) * 0x100;
     len >>= 2;
     if(len > 550)
     {
         return -1;
     }

     sum = 0;
     for(i = 6; i < len + 6; i++)
     {
         sum += *(R_Buf + i);
     }

     if(sum != *(R_Buf + len + 6))
     {
         return -1; //;校验和不相等
     }
    
    if(*(R_Buf + 12) == 0x0A) //;查询参数
    {
        if((*(R_Buf + 14) == 0x00) && (*(R_Buf + 15) == 0x00) &&
           (*(R_Buf + 16) == 0x01) && (*(R_Buf + 17) == 0x0b)) //;查询F89终端地址
        {
            if((*(R_Buf + 7) == 0xAA) && (*(R_Buf + 8) == 0xAA) &&
               (*(R_Buf + 9) == 0xAA) && (*(R_Buf + 10) == 0xAA))
            {     
                return sign;  //; 终端地址为 AAAA AAAA 时可以读出当前的终端地址
            }
        }
    }

    if((*(R_Buf + 7) != ZDPKZ.F89.QuXian[0]) || (*(R_Buf + 8) != ZDPKZ.F89.QuXian[1]))
    {
        return -1; //;行政区划码不正确
    }

    if(!(ZDPKZ.F89.DiZhi[0] == *(R_Buf + 9) && ZDPKZ.F89.DiZhi[1] == *(R_Buf + 10)) &&
        !(*(R_Buf + 9) == 0xff && *(R_Buf + 10) == 0xff && (*(R_Buf + 11) & 0x01)))
    {
        return -1; //;终端地址不正确，也不是广播地址
    }

    return sign;
}


/* 
********************************************************************************
* 函 数 名：AFN_FlagFind
* 功    能：找出要通过串口发送的事件组标志
* 参    数：GYCtrl-规约控制字
* 返    回：0xFF-无需要发送数据；其他-发送时间组标志
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
u8 AFN_FlagFind(struct GuiYueCtrl *GYCtrl)
{
    if(GYCtrl->AFNList[0])
    {
        return  0;
    }

    if(GYCtrl->AFNList[1])
    {
        return  1;
    }

    if(GYCtrl->AFNList[2])
    {
        return  2;
    }

    if(GYCtrl->AFNList[3])
    {
        return  3;
    }

    if(GYCtrl->AFNList[4])
    {
        return  4;
    }

    if(GYCtrl->AFNList[5])
    {
        return  5;
    }

    if(GYCtrl->AFNList[6])
    {
        return  6;
    }

    return  0xFF;
}


/* 
********************************************************************************
* 函 数 名：AFN_DataPre
* 功    能：规约数据的应答上送处理
* 参    数：ListNo-事件标志组；Buf_Rx-接收缓冲区；Buf_Tx-发送缓冲区
*           GYCtrl-规约处理控制字
* 返    回：需发送数据的长度
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
u16 AFN_DataPre(u8 ListNo, char *Buf_Rx, char *Buf_Tx, struct GuiYueCtrl *GYCtrl)
{
    u16 TLong = 0;

    switch(GYCtrl->AFNList[ListNo])
    {
        case 0xEE:
        {
            GDW_AFN00H(GYCtrl);
            break;
        }
        case 0x01: //;复位
        {
            TLong =  GDW_AFN01H(Buf_Rx, Buf_Tx);
            break;
        }
        case 0x02: //;链路口检测
        {
            TLong =  GDW_AFN02H(Buf_Tx, GYCtrl);
            break;
        }
        case 0x04:
        {
            TLong = GDW_AFN04H(Buf_Rx, Buf_Tx);
            break;
        }
        case 0x05:
        {
            TLong = GDW_AFN05H(Buf_Rx, Buf_Tx);
            break;
        }
        case 0x09:
        case 0x0A:
        {
            TLong = GDW_AFN090AH(Buf_Rx, Buf_Tx, GYCtrl);
            break;
        }
        case 0x0B:
        {
            //;EW.TaskAUL_P = 'P';
            //;TLong = Task_AUpLoad_Program(Buf_Tx, GYCtrl);
            //;EW.TaskAUL_P = 0;
            break;
        }
        case 0x0C:
        {
            if((ListNo == 0) || (ListNo == 1))
            {
                TLong = GDW_AFN0CH(Buf_Rx, Buf_Tx, GYCtrl);
            }
            else if(ListNo == 4)
            {
                TLong = GDW_AFN0CH_Up(Buf_Tx, GYCtrl);
            }
            break;
        }
        case 0x0D:
        {
            if((ListNo == 0) || (ListNo == 1))
            {
                TLong = GDW_AFN0DH(Buf_Rx, Buf_Tx, GYCtrl);
            }
            else if(ListNo == 4)
            {
                TLong = GDW_AFN0DH_Up(Buf_Tx, GYCtrl);
            }
            break;
        }
        case 0x0E:
        {
            if((ListNo == 0) || (ListNo == 1))
            {
                TLong = GDW_AFN0EH(Buf_Rx, Buf_Tx, GYCtrl);
            }
            else if(ListNo == 3)
            {
                TLong = GDW_AFN0EH_Up(Buf_Tx, GYCtrl);
            }
            else if(ListNo == 6)
            {
                if(GYCtrl == &GprsGyC)
                {
                    //;;TLong = ERC_SMS_Up(Buf_Tx);
                }
            }
            break;
        }
        /*case 0x0F: //;关闭文件传输--20141225
        {
            TLong = GDW_AFN0FH(Buf_Rx, Buf_Tx, GYCtrl);
            break;    
        }
        */
        case 0x10:
        {
            if(ListNo == 1)
            {
                GDW_AFN10H(Buf_Rx, GYCtrl);              //;数据转发
            }
            else if(ListNo == 2)
            {
                TLong = GDW_AFN10H_Up(Buf_Tx, GYCtrl);
            }
            break;
        }
        default:
        {
            break;
        }
    }

    return  TLong;
}


/* 
********************************************************************************
* 函 数 名：AFN_Gprs_Flag
* 功    能：写入规约相关需要处理的事件标志
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void AFN_Gprs_Flag(void)
{
    if(GprsGyC.RecOrd == 'E')
    {
        GprsGyC.RecOrd = 0;
        if(!GprsGyC.AFNList[1])
        {
            if(*(GPRSREVBUF + 12) == 0)
            {
                GprsGyC.AFNList[1] = 0xEE;
            }
            else
            {
                GprsGyC.AFNList[1] = *(GPRSREVBUF + 12);
            }
        }
    }

    if((RelayC.Status == right) || (RelayC.Status == error))  //;数据转发
    {
        if(RelayC.Channel == 'G')
        {
            if(!GprsGyC.AFNList[2])
            {
                GprsGyC.AFNList[2] = 0x10;
            }
        }
    }

    if(ERCUp.Status == 'G') //;要上送事件
    {

        if(!GprsGyC.AFNList[3])
        {
            GprsGyC.AFNList[3] = 0x0E;
        }
    }

    if(TaskC.Status == 'G')   //;要上送1类任务
    {
        if(!GprsGyC.AFNList[4])
        {
            if(TaskC.TaskTn == '1')
            {
                GprsGyC.AFNList[4] = 0x0C;
            }
            else if(TaskC.TaskTn == '2')
            {
                GprsGyC.AFNList[4] = 0x0D;
            }
        }
    }

    if(GprsC.LoginIn == 'E')  //;登陆
    {
        if (!GprsGyC.AFNList[5])
        {
            GprsGyC.AFNList[5] = 0x02;
            GprsGyC.AFN02_FN = 0x01;
        }
    }
    else if(GprsC.LoginOut == 'E')       //;退出登陆
    {
        if(!GprsGyC.AFNList[5])
        {
            GprsGyC.AFNList[5] = 0x02;
            GprsGyC.AFN02_FN = 0x02;
        }
    }
    else if(GprsC.HBStatus == 'E')    //;心跳
    {
        if(!GprsGyC.AFNList[5])
        {
            GprsGyC.AFNList[5] = 0x02;
            GprsGyC.AFN02_FN = 0x04;
        }
    }

    if(SMS.Status == 'E') //;短信上送事件
    {
        if(!GprsGyC.AFNList[6])
        {
            GprsGyC.AFNList[6] = 0x0E;
        }
    }
}


/* 
********************************************************************************
* 函 数 名：AFN_Gprs_FlagDel
* 功    能：清除要通过串口发送的规约相关数据的APN标志
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void AFN_Gprs_FlagDel(u8 ListNo)
{
    if(ListNo > 0)
    {
        GprsGyC.AFNList[ListNo] = 0;
    }

    if(ListNo == 2)
    {
        if((RelayC.Status == right) || (RelayC.Status == error))  //;数据转发
        {
            if (RelayC.Channel == 'G')
            {
                RelayC.Status = idle;
                RelayC.Channel = 0;
            }
        }
    }

    if(ListNo == 3)
    {
        if(ERCUp.Status == 'G')
        {
            ERCUp.Status = 'S';
        }
    }

    if(ListNo == 4)
    {
        if(TaskC.Status == 'G')
        {
            TaskC.Status = 'S'; //;上送任务完成
        }
    }

    if(ListNo == 5)
    {
        GprsGyC.AFN02_FN = 0;
        if(GprsC.LoginIn == 'E')
        {
            GprsC.LoginIn = 'S';  //;登陆完成
        }
        else if(GprsC.LoginOut == 'E')  //;退出登陆
        {
            GprsC.LoginOut = 'S';   //;登陆完成
        }
        else if(GprsC.HBStatus == 'E')    //;心跳
        {
            GprsC.HBStatus = 'S';
            GprsC.HBOVT = 0;
        }
    }

    if(ListNo == 6)
    {
        if(SMS.Status == 'E')
        {
            SMS.Status = 'S'; //;上送短信完成
        }
    }
}


/* 
********************************************************************************
* 函 数 名：AFN_Gprs_Rec
* 功    能：接收GPRS通道通讯帧
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void AFN_Gprs_Rec(void)
{
     u16 i, len;
     
    if(GprsC.TCPRec == true)
    {
        GprsC.TCPRec = false;

        GprsC.TCPBuf = strstr(GprsC.TCPBuf, "h"); //;;寻找首次出现帧头68的位置

        if(GDW_AFN_CHK(GprsC.TCPBuf) == true)
        {
            len = *(GprsC.TCPBuf + 1) + *(GprsC.TCPBuf + 2) * 0x100;
            len >>= 2;
/*
            ZDS.F10.Flux_Day += len;
            ZDS.F10.Flux_Day += 50;
            ZDS.F10.Flux_Month += len;
            ZDS.F10.Flux_Month += 50; //;;流量统计
by  zrt 20130710
*/
            ZDS.F10.Flux_Day += (len+8);                       //;总数据长度len+6+校验码+结束码
            ZDS.F10.Flux_Month += (len+8);
            if(ZDS.F10.Flux_Month > FluxLimValue)
            {
                return; //;;超日流量
            }

            for(i = 0; i < GprsC.TCPLen; i++)
            {
                *(GPRSREVBUF + i) = *(GprsC.TCPBuf + i);
            }

            GprsGyC.RecOrd = 'E'; //;;收到远程GDW通讯帧
#ifdef JTDK_USART1_ON             
            if(LOCUSARTCTR.RecOrd == false) //;---20141215
            {
              LOCUSARTCTR.TLong = len+8;
              MovNByte(LOCUSARTCTR.TLong, (u8*)&AllBuf.Tx1_1[0],(u8*)&AllBuf.Rx2_2[0]);
            	LOCUSARTCTR.SedOrd = 'E';
              LOCUSARTCTR.BufAdr = LOCSEDBUF;
              USART1_Send();
              if((GprsC.HBStatus == 0)&&( GprsC.LoginIn ==0 ))/*心跳包和登陆时候，不需要延时等待串口发送*/
              {
              	Delay_nms(50);
              	Watchdog(); 
              	Delay_nms(50);
              } 
            }
            else
            {
            	LOCUSARTCTR.SedOrd = 0;
            }
#endif            
        }
    }
}



/* 
********************************************************************************
* 函 数 名：AFN_Gprs_Com
* 功    能：控制规约数据通过GPRS通道上送
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void AFN_Gprs_Com(void)
{
    AFN_Gprs_Rec();
    AFN_Gprs_Flag();

    if(GprsGyC.Status == idle)
    {
        if(GPRSUSARTCTR.SedOrd == 0x00)
        {
            GprsGyC.ListNo = AFN_FlagFind(&GprsGyC); //;找出要发送的APN
            
            if(GprsGyC.ListNo != 0xFF)
            {
                u16  Len;
            
                Len = AFN_DataPre(GprsGyC.ListNo, GPRSREVBUF, GPRSSEDBUF, &GprsGyC);   //;len为整一帧长度
            
                if((Len != 0) && (Len < U2TXBUFSIZE))
                {
                    if(GprsGyC.ListNo == 6)
                    {
                        GprsC.SMSLen = Len; //;;写入发送长度
                        GprsGyC.Status = 'M';  //;发短信
                    }
                    else
                    {
                        if(ZDS.F10.Flux_Month < FluxLimValue)
                        {
/*                        
                            ZDS.F10.Flux_Day += Len;
                            ZDS.F10.Flux_Day += 50;                                                 //; 流量控制
                            ZDS.F10.Flux_Month += Len;
                            ZDS.F10.Flux_Month += 50;                                               //; 月流量统计
by zrt 20130710                            
*/                            
                            ZDS.F10.Flux_Day += Len;
                            ZDS.F10.Flux_Month += Len;
                            GprsC.TCPLen = Len; //;;写入发送长度
                            GprsGyC.Status = 'E';  //;;TCP发送
#ifdef JTDK_USART1_ON
                            if(LOCUSARTCTR.RecOrd == false) //;---20141215
                            {
                            	LOCUSARTCTR.TLong = GprsC.TCPLen;
                            	MovNByte(LOCUSARTCTR.TLong, (u8*)&AllBuf.Tx1_1[0],(u8*)&AllBuf.Tx2_1[0]);
                            }
#endif                            
                            
                        }
                        else
                        {
                            GprsGyC.Status = idle;
                            AFN_Gprs_FlagDel(GprsGyC.ListNo);
                        }
                    }
                }
                else
                {
                    GprsGyC.Status = idle;
                    AFN_Gprs_FlagDel(GprsGyC.ListNo);
                }
            }     
        }    
    }

    if (GprsGyC.Status == 'E')
    {
        if (Modem_TCP_Send() != 0xEF) //;;通过TCP发出
        {
            GprsGyC.Status = idle;
            AFN_Gprs_FlagDel(GprsGyC.ListNo);  //;APN通过TCP发送完成 ---20141215
#ifdef JTDK_USART1_ON            
            if(LOCUSARTCTR.RecOrd == false)    //;GPRS发送完毕后，串口发送数据，串口有接收数据马上终止发送，等待处理串口数据
            {
            	LOCUSARTCTR.BufAdr = LOCSEDBUF;
            	LOCUSARTCTR.SedOrd = 'E';
            }
            else
            {
            	LOCUSARTCTR.SedOrd = 0;
            }
#endif            
        }
    }

    if (GprsGyC.Status == 'M')
    {
        //;if (Modem_SMS_Send() == true) //;;通过SMS发出
        //;{
        //;    U2GY.Status = idle;
        //;    AFN_Gprs_FlagDel(U2GY.ListNo);  //;APN通过短信发送完成
        //;}
    }
}


/* 
********************************************************************************
* 函 数 名：AFN_Local_Flag
* 功    能：写入本地通讯规约标识
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void AFN_Local_Flag(void)
{   
    if(LocGyC.RecOrd == 'E') //;;数据点抄
    {
        LocGyC.RecOrd = 0;
        if(!LocGyC.AFNList[1])
        {
            if(LOCREVBUF[12] == 0)
            {
                LocGyC.AFNList[1] = 0xEE;
            }
            else
            {
                LocGyC.AFNList[1] = LOCREVBUF[12];
            }
        }
    }  

    if ((RelayC.Status == right) || (RelayC.Status == error)) //;数据转发
    {
        if (RelayC.Channel == 'L')
        {
            if (!LocGyC.AFNList[2])
            {
                LocGyC.AFNList[2] = 0x10;
            }
        }
    }

    if (ERCUp.Status == 'L') //;要上送事件
    {
        if (!LocGyC.AFNList[3])
        {
            LocGyC.AFNList[3] = 0x0E;
        }
    }

    if (TaskC.Status == 'L') //;要上送1类任务
    {
        if (!LocGyC.AFNList[4])
        {
            if (TaskC.TaskTn == '1')
            {
                LocGyC.AFNList[4] = 0x0C;
            }
            else if (TaskC.TaskTn == '2')
            {
                LocGyC.AFNList[4] = 0x0D;
            }
        }
    }

    if (InfGyC.RecOrd == 'E') //;;红外通讯帧
    {
        InfGyC.RecOrd = 0;
        if (!InfGyC.AFNList[1])
        {
            if (INFREVBUF[12] == 0)
            {
                InfGyC.AFNList[1] = 0xEE;
            }
            else
            {
                InfGyC.AFNList[1] = INFREVBUF[12];
            }
        }
    }

}


/* 
********************************************************************************
* 函 数 名：AFN_Local_FlagDel
* 功    能：清除事件标志字
* 参    数：GYCtrl-规约处理控制字
* 返    回：无
* 说    明：事件处理完成后调用
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void AFN_Local_FlagDel(struct GuiYueCtrl *GYCtrl)
{
    if(GYCtrl->ListNo > 0)
    {
        GYCtrl->AFNList[GYCtrl->ListNo] = 0;
    }
    
    if(GYCtrl->ListNo == 2)
    {
        if(GYCtrl == &LocGyC)
        {
            if((RelayC.Status == right) || (RelayC.Status == error))  //;数据转发
            {
                if (RelayC.Channel == 'L')
                {
                    RelayC.Status = idle;
                    RelayC.Channel = 0;
                }
            }
        }
    }
    else if(GYCtrl->ListNo == 3)
    {
        if (GYCtrl == &LocGyC)
        {
            if (ERCUp.Status == 'L')
            {
                ERCUp.Status = 'S';
            }
        }
    }
    else if(GYCtrl->ListNo == 4)
    {
        if (GYCtrl == &LocGyC)
        {
            if (TaskC.Status == 'L')
            {
                TaskC.Status = 'S'; //;上送任务完成
            }
        }
    }
}


/* 
********************************************************************************
* 函 数 名：AFN_Local_Rec
* 功    能：接收本地通道通讯帧
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void AFN_Local_Rec(void)
{    
    s8 n;
    u16 i, k;
    
#ifdef Local_USART1_ON    
    if(LOCUSARTCTR.RecOrd == true)
    {            
        LOCUSARTCTR.RecOrd = false;//;--20150105
        if((n = (u16)GDW_AFN_CHK_HW(LOCUSARTCTR.BufAdr, LOCUSARTCTR.RecLen)) != -1)
        {  
            for(i = n, k = 0; i < LOCUSARTCTR.RecLen; i++, k++)
            {
                LOCREVBUF[k] = *(LOCUSARTCTR.BufAdr + i);
            }
            LocGyC.RecOrd = 'E';
            LocGyC.AFNList[1] = 0;
        }
        /*if(GDW_AFN_CHK(LOCUSARTCTR.BufAdr) == true)
        {
            for(i = 0; i < LOCUSARTCTR.RecLen; i++)
            {
                LOCREVBUF[i] = *(LOCUSARTCTR.BufAdr + i);
            }

            LocGyC.RecOrd = 'E';
            LocGyC.AFNList[1] = 0;
        }*/
    }
#endif

    if(INFUSARTCTR.RecOrd == true)
    {
        INFUSARTCTR.RecOrd = false;          
        if((n = (u16)GDW_AFN_CHK_HW(INFUSARTCTR.BufAdr, INFUSARTCTR.RecLen)) != -1)
        {  
            for(i = n, k = 0; i < INFUSARTCTR.RecLen; i++, k++)
            {
                INFREVBUF[k] = *(INFUSARTCTR.BufAdr + i);
            }
    
            InfGyC.RecOrd = 'E';
            InfGyC.AFNList[1] = 0;
        }
    }
}


/* 
********************************************************************************
* 函 数 名：AFN_Local_Com
* 功    能：控制规约数据通过本地通道上送，包括串口调调试与红外通讯
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/ 
void AFN_Local_Com(void)
{
    
    AFN_Local_Rec();
    AFN_Local_Flag();
    
#ifdef Local_USART1_ON
    if(LocGyC.Status == idle) //;;本地串口1通讯
    {
        LocGyC.ListNo = AFN_FlagFind(&LocGyC); //;找出要发送的APN
        if(LocGyC.ListNo != 0xFF)
        {
            u16 Len;
            Len = AFN_DataPre(LocGyC.ListNo, LOCREVBUF, LOCSEDBUF, &LocGyC);
            if((Len != 0) && (Len < FRAMESIZE))
            {
                if(LOCUSARTCTR.SedOrd == 0)
                {
                    LOCUSARTCTR.SedOrd = 'E';
                    LOCUSARTCTR.TLong = Len;
                    LOCUSARTCTR.BufAdr = LOCSEDBUF;
                    LOCUSARTCTR.Mark = 'G';
                }
            }
            //;串口转发数据不成功
            AFN_Local_FlagDel(&LocGyC);   //;--20141229 APN通过本地调试口发送完成
        }
    }
#endif

    if(InfGyC.Status == idle) //; 本地红外通讯
    {
        InfGyC.ListNo = AFN_FlagFind(&InfGyC); //;找出要发送的APN
        if(InfGyC.ListNo != 0xFF)
        {
            u16 Len;
    
            Len = AFN_DataPre(InfGyC.ListNo, INFREVBUF, INFSEDBUF, &InfGyC);
            if((Len != 0) && (Len < 512))
            {
                if(INFUSARTCTR.SedOrd == 0) //;通过串口3发出
                {
                    INFUSARTCTR.SedOrd = 'E';
                    INFUSARTCTR.TLong = Len;
                    INFUSARTCTR.BufAdr = INFSEDBUF;
                }
            }
            AFN_Local_FlagDel(&InfGyC);   //;APN通过本地调试口发送完成
        }
    }
}


/* 
********************************************************************************
* 函 数 名：AFNComDeal
* 功    能：调试口和GPRS通道的GDW376.1规约AFN处理，包括应答，主动上送等。
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void AFNComDeal(void)
{
    AFN_Gprs_Com(); //;远程通讯
    AFN_Local_Com(); //;本地通讯
}
