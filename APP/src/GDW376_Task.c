/*
********************************************************************************
*  文 件 名：GDW376_Task.c
*
*  文件描述：
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


 /* 
********************************************************************************
* 函 数 名：
* 功    能：找出离输入日期最近的一个抄表日 
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：2011-05-17 21:43:20 修改错误
             2011-06-14 15:19:40 简化程序ch2 <<= Day;
********************************************************************************
*/
u8 T2Task_CBF(u8 *DateIn, u8 *DateOut)
{
    u32 ch1 = 0, ch2 = 1;
    u8 i, Day;
    
    Day = BCD1ToU8(DateIn + 0);    
    ch1 |= ZDP.F33.PortP[0].RDay[3];
    ch1 <<= 8;
    ch1 |= ZDP.F33.PortP[0].RDay[2];
    ch1 <<= 8;
    ch1 |= ZDP.F33.PortP[0].RDay[1];
    ch1 <<= 8;
    ch1 |= ZDP.F33.PortP[0].RDay[0];
    
    for(i = 1; i < 32; i++)
    {      
         if(ch1 & ch2)
         {
             U16To2BCD(i, DateOut + 0);
             if(i > Day)
             {
                if(*(DateIn + 1) == 0x01)
                {
                    *(DateOut + 1) = 12;
                    *(DateOut + 2) = *(DateIn + 2) - 1;
                }
                else
                {
                    *(DateOut + 1) = *(DateIn + 1) - 1;
                    *(DateOut + 2) = *(DateIn + 2);
                }
             }
             else
             {
                *(DateOut + 1) = *(DateIn + 1);
                *(DateOut + 2) = *(DateIn + 2);
            }
             return  true;
         } 
         ch2 <<= 1;  
    }
    return false;
}


/* 
********************************************************************************
* 函 数 名：T2Task_CTF
* 功    能：出离上送时间最近的曲线点的时间
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：2012-03-27 11:21:52
********************************************************************************
*/
void T2Task_CTF(u8 *DateIn, u8 *DateOut)
{
    u8 min;
    
    min = BCD1ToU8(DateIn + 0);    
    if(min < 15)
    {
        *(DateOut + 0) = 0x00;
    }
    else if(min < 30)
    {
        *(DateOut + 0) = 0x15;
    }
    else if(min < 45)
    {
        *(DateOut + 0) = 0x30;
    }
    else if(min < 60)
    {
        *(DateOut + 0) = 0x45;
    }
    *(DateOut + 1) = *(DateIn + 1);
    *(DateOut + 2) = *(DateIn + 2);
    *(DateOut + 3) = *(DateIn + 3); 
    *(DateOut + 4) = *(DateIn + 4);
}


/* 
********************************************************************************
* 函 数 名：T2Task_FnPn
* 功    能：2类任务数据配置
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：2011-06-14 16:51:22
*           2011-09-23 08:58:50 
*           2012-03-05 16:43:34
********************************************************************************
*/
u8 T2Task_FnPn(char *S_Buf, u16 Buf_Ind, u8 Pn, u8 Fn)
{
    u8  readtime, ratenum, len, type, j, temp;
    u8  Date[5];
    u16 Buf_Add = 0;
    u8 Td_m[2];
    u8 Td_d[3]; 
    u32 *adr32;
    struct Task2_Td  *Td;
   
  
    adr32 = (u32*)(PN0DZU + Fn);  //;找出信息类
    temp = *((u8*)adr32 + 3); //;
    type = temp & 0x07; 
    readtime = temp & 0x08;
    ratenum  = temp & 0x10;
    len  = *((u8*)adr32 + 2);
    
    //;终端不支持信息类/信息类无法识别/信息点错误/信息点不存在
    if((type==0) || (Pn==0xFF) || (ZD_Pn_Find(Pn)<0)||
       (Fn == 0xFF) || (Fn>218))
    {
        return false; 
    }
        
    Td = (struct Task2_Td*) (&TaskC.T2.Td1 + TaskC.TaskNO); //;上送时间 
       
    if(type == 1)  //;日冻结数据
    {                                                                                                                                     
        Day_Last(Td->Td_SO + 2, Td_d);    //;计算出前一日的时间$$$$
        ByteN_Mov(3, Date, Td_d);
        Buf_Add = DDJ_Read(S_Buf, Buf_Ind + 3, Pn, Fn, Date);    
        if(Buf_Add != 0xFF)
        {
            if(Compare_NByte(3, Date, Td_d) == true) //;找到与Td_d相等的点
            {
                *(S_Buf + Buf_Ind + 0) = Td_d[0];
                *(S_Buf + Buf_Ind + 1) = Td_d[1];
                *(S_Buf + Buf_Ind + 2) = Td_d[2];
                Buf_Add += 3;
                return Buf_Add;                
            }
          
        }
        else
        {
            *(S_Buf + Buf_Ind++) = Td_d[0];
            *(S_Buf + Buf_Ind++) = Td_d[1];
            *(S_Buf + Buf_Ind++) = Td_d[2];
            
            Buf_Add = 3;
        }
    }
             
    else if(type == 2) //; 抄表日冻结数据
    {
        if(T2Task_CBF(Td->Td_SO + 2, Td_d) == true) //;找出离发送时间最近的抄表日
        {
            ByteN_Mov(3, Date, Td_d);
            Buf_Add = BDJ_Read(S_Buf, Buf_Ind + 3, Pn, Fn, Date);
            if(Buf_Add != 0xFF) 
            {
                if(Compare_NByte(3, Date, Td_d) == true) //;找到与Td_d相等的点
                {
                    *(S_Buf + Buf_Ind + 0) = Td_d[0];
                    *(S_Buf + Buf_Ind + 1) = Td_d[1];
                    *(S_Buf + Buf_Ind + 2) = Td_d[2];
                    Buf_Add += 3;
                    return Buf_Add;
                }
            }  
            else
            {
                *(S_Buf + Buf_Ind++) = Td_d[0];
                *(S_Buf + Buf_Ind++) = Td_d[1];
                *(S_Buf + Buf_Ind++) = Td_d[2];
                Buf_Add = 3;
            }
            
        }
    }
        
    else if(type == 3)  //;月冻结数据
    {     
        Month_Last(Td->Td_SO + 3, Td_m);  //;找出上个月

        ByteN_Mov(2, Date, Td_m);
        Buf_Add = MDJ_Read(S_Buf, Buf_Ind + 2, Pn, Fn, Date); 
        if(Buf_Add != 0xFF)
        {
            if(Compare_NByte(2, Date, Td_m) == true) //;找到与Td_m相等的点
            {
                *(S_Buf + Buf_Ind + 0) = Td_m[0];
                *(S_Buf + Buf_Ind + 1) = Td_m[1];
                Buf_Add += 2;
                return Buf_Add;
            }
        }  
        else
        {
            *(S_Buf + Buf_Ind++) = Td_m[0];
            *(S_Buf + Buf_Ind++) = Td_m[1];
            Buf_Add = 2;
        }  
    }
    
    else if(type == 4)   //;曲线数据
    {    
        T2Task_CTF(Td->Td_SO, Td->Td_C); //;处理时间的分钟为15的整数倍     
        ByteN_Mov(5, Date,  Td->Td_C);
        Buf_Add = QDJ_Read(S_Buf, Buf_Ind + 7, Pn, Fn, Date);          
        if(Buf_Add != 0xFF) //;找到数据
        {
            if(Compare_NByte(5, Date, Td->Td_C) == true) //;找到与Td_C相等的点
            {
                *(S_Buf + Buf_Ind + 0) = Td->Td_C[0];
                *(S_Buf + Buf_Ind + 1) = Td->Td_C[1];
                *(S_Buf + Buf_Ind + 2) = Td->Td_C[2];
                *(S_Buf + Buf_Ind + 3) = Td->Td_C[3];
                *(S_Buf + Buf_Ind + 4) = Td->Td_C[4];
                *(S_Buf + Buf_Ind + 5) = 1; //;冻结密度为15分钟
                *(S_Buf + Buf_Ind + 6) = 1; //;1点
                Buf_Add += 7;
                return Buf_Add;
            }
        }
        else 
        {
            *(S_Buf + Buf_Ind++) = Td->Td_C[0];
            *(S_Buf + Buf_Ind++) = Td->Td_C[1];
            *(S_Buf + Buf_Ind++) = Td->Td_C[2];
            *(S_Buf + Buf_Ind++) = Td->Td_C[3];
            *(S_Buf + Buf_Ind++) = Td->Td_C[4];
            *(S_Buf + Buf_Ind++) = 1; //;冻结密度为15分钟
            *(S_Buf + Buf_Ind++) = 1; //;1点
            Buf_Add = 7;
        }

    }


   //;未从FLASH里边找到数据，则填写0xEE           
    if(readtime)   //;包含抄表日
    {  
        *(S_Buf + Buf_Ind++) = 0xEE;
        *(S_Buf + Buf_Ind++) = 0xEE;
        *(S_Buf + Buf_Ind++) = 0xEE;
        *(S_Buf + Buf_Ind++) = 0xEE;
        *(S_Buf + Buf_Ind++) = 0xEE;
        Buf_Add += 5;
    }
    if(ratenum)   //;包含费率数
    {  
        *(S_Buf + Buf_Ind++) = 0x04;
        Buf_Add += 1;
    }
    for(j=0; j<len; j++)  
    {
        *(S_Buf + Buf_Ind++) = 0xEE;
    }
    Buf_Add += len;
    
    return Buf_Add;  
}

/* 
********************************************************************************
* 函 数 名：Task_Init
* 功    能：初始化任务控制参数任务上送时间
* 参    数：Pn---任务号；Fn---信息类（F65，F66）
* 返    回： 无
* 说    明：
* 抛出异常：
* 作    者：023 
* 修改记录：
********************************************************************************
*/
void Task_Init(u16 Pn, u8 Fn)
{
    u8 i;

    if(Fn == 65)
    {
        if((Pn>0) && (Pn<TASK1NUM))
        {
            for(i=0; i<5; i++)
            {
                *(TaskC.T1.Td1 + (Pn - 1) * 5 + i) = 0;
            }
        }
    }
      
    else if(Fn == 66)
    {
        if((Pn>0) && (Pn<TASK2NUM))
        {
            for(i=0; i<15; i++)
            {
                *(TaskC.T2.Td1.Td_S + (Pn - 1) * 15 + i) = 0;
            }
        }
    }
}


 /* 
********************************************************************************
* 函 数 名：TaskUpFind
* 功    能：找出适合上送的任务
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
u8 TaskUpFind(void)
{
    u8 *Tdn, unit, cyc, Td_c[5], *TdD;
    struct TaskPStr_F65 *TaskPro; 
    
    if(TaskC.TaskTn == '1')
    {
        TaskPro = (struct TaskPStr_F65*)(TaskP.F65 + TaskC.TaskNO);  //;取出任务TaskNO的参数地址    
        Tdn = TaskC.T1.Td1 + TaskC.TaskNO * 5; 
    } 
    else if(TaskC.TaskTn == '2')
    {
        TaskPro = (struct TaskPStr_F65*)(TaskP.F66 + TaskC.TaskNO);  //;取出任务TaskNO的参数地址    
        Tdn = TaskC.T2.Td1.Td_S + TaskC.TaskNO * 15; 
        TdD = TaskC.T2.Td1.Td_SO + TaskC.TaskNO * 15;
    }
    
    if(TaskPro->PnFnNum == 0)
    {
        return false;  
    }
         
    unit = TaskPro->Cyc >> 6;
    cyc = TaskPro->Cyc & 0x3f;    
    
    if(cyc == 0)
    {
        return false;
    }
    
    if((Tdn[0]==0) && (Tdn[1]==0) && (Tdn[2]==0)&& (Tdn[3]==0) && (Tdn[4]==0)) //;发送时间时间未配置
    {
        if((TaskPro->BaseTime[0]==0) && (TaskPro->BaseTime[1]==0) && 
           (TaskPro->BaseTime[2]==0) && (TaskPro->BaseTime[3]==0) && 
           (TaskPro->BaseTime[4]==0) && (TaskPro->BaseTime[5]==0))  //;发送基准时间未配置
        {
            EW.MoveByte = 'M';
            MovNByte(5, Tdn, &TBCD.Min);        
            EW.MoveByte = 0; 
            Time_CRAdd(Tdn, unit, cyc, 1); //;将当前时间加一个周期作为发送的时间
        }
        else
        {
            EW.MoveByte = 'M';
            MovNByte(5, Tdn, TaskPro->BaseTime + 1); //;发送基准时间作为发送的时间
            EW.MoveByte = 0; 
            Tdn[3] &= 0x1F; //;去除星期
        }
    }
    
    //;发送基准时间小于当前时间。
    while(TimeCompare_5Byte(Tdn, &TBCD.Min) == ahead)
    {
        Time_CRAdd(Tdn, unit, cyc, 1); 
        if(TimeCompare_5Byte(Tdn, &TBCD.Min) == ahead)
        {
            if(unit == 0) //;分
            {
                if(TimeCompare_4Byte(Tdn+1, &TBCD.Hour) == ahead)
                {
                    U8To1BCD((BCD1ToU8(Tdn)%cyc), Tdn); //;此法60分钟必是cyc的倍数
                    Tdn[1] = TBCD.Hour;
                    Tdn[2] = TBCD.Day;  
                    Tdn[3] = TBCD.Month;
                    Tdn[4] = TBCD.Year;
                   
                }
            }
            else if(unit == 1) //;时
            {
                if(TimeCompare_3Byte(Tdn+2, &TBCD.Day) == ahead)
                {
                    U8To1BCD((BCD1ToU8(Tdn+1)%cyc),(Tdn+1));
//;                    Tdn[1] = Tdn[1] % cyc; //;此法24必是cyc的倍数
                    Tdn[2] = TBCD.Day;  
                    Tdn[3] = TBCD.Month;
                    Tdn[4] = TBCD.Year;
                    
                }
            }
            else if(unit == 2) //;日
            {
                if(TimeCompare_2Byte(Tdn+3, &TBCD.Month) == ahead)
                {
                    Tdn[2] = 1;  
                    Tdn[3] = TBCD.Month;
                    Tdn[4] = TBCD.Year;
                }
            }
            else if(unit == 3) //;月
            {
                if(TimeCompare_1Byte(Tdn+4, &TBCD.Year) == ahead)
                {
                    Tdn[3] = 1;
                    Tdn[4] = TBCD.Year;
                }
            }
        }
    }

    if(TimeCompare_5Byte(Tdn, &TBCD.Min) == back)
    {
        EW.MoveByte = 'M';
        MovNByte(5, Td_c, &TBCD.Min); //;将当前时间取出来进行判断       
        EW.MoveByte = 0; 
        Time_CRAdd(Td_c, unit, cyc, 1); //;加一个周期
        if(TimeCompare_5Byte(Tdn, Td_c) == back) //;时间向前更改导致上送时间超过当前时间一个周期
        {
            if(TaskC.TaskTn == '1')
            {
                 ByteN_Fill(5, Tdn, 0); //;清除上送时间，等下一次重新配置
            }
            else if(TaskC.TaskTn == '2')
            {
                 ByteN_Fill(15, Tdn, 0);
            }
        }
    }
    
    if(TimeCompare_5Byte(Tdn, &TBCD.Min) == same)
    {
    	  if(TaskC.TaskTn == '2')//;2011-05-16 17:21:34当设置周期为日的时候读取日冻结数据将提前一天。
    	  {
    	  	  ByteN_Mov(5, TdD, Tdn);
    	  }
        Time_CRAdd(Tdn, unit, cyc, 1); 
        return true;
    }
    
    return false;
}


/* 
********************************************************************************
* 函 数 名：Task_Deal_Up
* 功    能：处理需要上送的任务
* 参    数：无
* 返    回：无
* 说    明：250ms执行一次任务上送
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZD_Task(void)
{     
    u8 i;
  
    if(TaskC.Switch == OFF) //;不处理
    {
        return;
    }
    
    if((TaskC.Status=='L') || (TaskC.Status=='G')) //;判断上送是否超时
    {
        TaskC.OVT++;
        if(TaskC.OVT > 30)  
        { 
            TaskC.OVT = 0;      //;超时，则按发送完成处理
            TaskC.Status = 'S';
        }
    }     
     
    if(TaskC.Status == 'S')    //;发送完成，并找出下一个需发送任务
    {
#ifdef LocalUpTask  
    ;
#else 
        if(!(GprsC.State & _HadLogin))
        {
            TaskC.Status = 0;
            TaskC.TaskTn = 0;
            TaskC.TaskNO = 0;
            TaskC.Switch = OFF;
            return; //;GPRS模块未登陆主站
        }
#endif 
        TaskC.Status = 0;                                 //;清除状态
        TaskC.OVT = 0;
        if(TaskC.TaskTn == '2')
        {
            TaskC.T2Over[0] |= (0x01 << TaskC.TaskNO);       //;写入任务处理完成标志
            TaskC.TaskNO++;                                  //;指向一个任务号
            if(TaskC.TaskNO >= TASK2NUM)
            {
                TaskC.TaskTn = 0;
                TaskC.TaskNO = 0;
                TaskC.Switch = OFF;
                return;
            }
        }  
        
        if(TaskC.TaskTn == '1')
        {
            TaskC.T1Over[0] |= (0x01 << TaskC.TaskNO);     //;写入任务处理完成标志
            TaskC.TaskNO++;                                //;指向一个任务号
            if(TaskC.TaskNO >= TASK1NUM)
            {
                TaskC.TaskTn = '2';
                TaskC.TaskNO = 0;
            }
        }
    }
    
    if(TaskC.Status == 0)
    {
 #ifdef LocalUpTask  
    ;
#else 
        if(!(GprsC.State & _HadLogin))
        {
            TaskC.Status = 0;
            TaskC.TaskTn = 0;
            TaskC.TaskNO = 0;
            TaskC.Switch = OFF;
            return; //;GPRS模块未登陆主站
        }
#endif        
       if(TaskC.TaskTn == '1')
       {
            for(i=TaskC.TaskNO; i<TASK1NUM; i++)
            {
                if(TaskP.F67[TaskC.TaskNO] == _TaskON)
                {
                    if(!(TaskC.T1Over[0] & (0x01 << TaskC.TaskNO)))   //;任务未处理
                    {
                        if(TaskUpFind() == true)
                        {
#ifdef LocalUpTask  //;本地调试上送任务
                            TaskC.Status = 'L';        
#else
                            TaskC.Status = 'G';   //;远程上送任务
#endif
                            break;
                        }
                        else
                        {
                            TaskC.T1Over[0] |= (0x01 << TaskC.TaskNO);     //;写入任务处理完成标志
                            TaskC.TaskNO++;                                //;指向一个任务号
                            if(TaskC.TaskNO >= TASK1NUM)
                            {
                                TaskC.TaskTn = '2';
                                TaskC.TaskNO = 0;
                            }   
                        }             
                    }
                }
                else
                {
                    TaskC.T1Over[0] |= (0x01 << TaskC.TaskNO);     //;写入任务处理完成标志
                    TaskC.TaskNO++;                                //;指向一个任务号
                    if (TaskC.TaskNO >= TASK1NUM)
                    {
                        TaskC.TaskTn = '2';
                        TaskC.TaskNO = 0;
                    }
                }
            }
           
       }  
              
       if(TaskC.TaskTn == '2')
       {
            for(i=TaskC.TaskNO; i<TASK2NUM; i++)
            {
                if(TaskP.F68[TaskC.TaskNO] == _TaskON)
                {
                    if(!(TaskC.T2Over[0] & (0x01 << TaskC.TaskNO)))   //;任务未处理
                    {
                        if(TaskUpFind() == true)
                        {
#ifdef LocalUpTask      //;本地调试上送任务
                            TaskC.Status = 'L';        
#else
                            TaskC.Status = 'G';   //;远程上送任务
#endif
                            break;
                        }
                        else
                        {
                            TaskC.T2Over[0] |= (0x01 << TaskC.TaskNO);       //;写入任务处理完成标志
                            TaskC.TaskNO++;                                  //;指向一个任务号
                            if(TaskC.TaskNO >= TASK2NUM)
                            {
                                TaskC.TaskTn = 0;
                                TaskC.TaskNO = 0;
                                TaskC.Switch = OFF;
                                return;
                            }
                        }
                    }
                }     
                else
                {
                    TaskC.T2Over[0] |= (0x01 << TaskC.TaskNO);       //;写入任务处理完成标志
                    TaskC.TaskNO++;                                  //;指向一个任务号
                    if(TaskC.TaskNO >= TASK2NUM)
                    {
                        TaskC.TaskTn = 0;
                        TaskC.TaskNO = 0;
                        TaskC.Switch = OFF;
                        return;
                    }
                }  
            }
        } 
    }         
}
 

/* 
********************************************************************************
* 函 数 名：ZD_Task_CHK
* 功    能：打开任务
* 参    数：无
* 返    回：无
* 说    明：每分钟执行一次
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZD_Task_CHK(void)
{ 
#ifndef LocalUpTask
    if(!(GprsC.State & _HadLogin)) 
    {
        TaskC.Switch = OFF;
        TaskC.Status = 0;
        TaskC.TaskTn = 0;
        TaskC.TaskNO = 0;
        TaskC.T1Over[0] = 0;
        TaskC.T2Over[0] = 0;
        return;
    }
#endif

    if(TaskC.Switch == OFF)                              
    {
        TaskC.Switch = ON;
        TaskC.Status = 0;
        TaskC.TaskTn = '1';
        TaskC.TaskNO = 0;
        TaskC.T1Over[0] = 0;
        TaskC.T2Over[0] = 0;
    }
}            
