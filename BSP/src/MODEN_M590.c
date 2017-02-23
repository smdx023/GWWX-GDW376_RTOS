/*
********************************************************************************
*  文 件 名：MODEN_M590.C
*
*  文件描述：有方GPRS模块M590驱动程序  
*
*  所用芯片：
*
*  创 建 人：
*
*  版 本 号：
*
*  修改记录：1.增加模块短信接收功能，用来唤醒备用主站的连接，短信唤醒命令是:
*               LINKTOZHGC*IP,端口号*;例如:LINKTOZHGC*112.091.151.002,7011*
*            2.增加在被动激活模式下短信激活主站的连接。短信激活命令：
*               *LINKTOMASTER* 2011-09-16 16:49:11
*            3 更新贴片M590 R2   20140704
*              更改开机时序，低电平延时2.4S
*              +CPIN 指令删除ok 之前的\r\n 
*              +CREG 指令删除ok 之前的\r\n   20140704
* 
********************************************************************************
*/
#include "BSP_CFG.h"
#include "GDW376_CFG.h"
//;extern struct ZDStatStr ZDS;      //;终端状态数据

/*----------------------------------------------------------------------------*
 |  打开模块电源
 *----------------------------------------------------------------------------*/
void Modem_PowON(void)
{
    _ModemPowerOn;
    _ModemPowerOn;
    _ModemPowerOn;
}


/*----------------------------------------------------------------------------*
 |  关闭模块电源
 *----------------------------------------------------------------------------*/
void Modem_PowOFF(void)
{
    _ModemPowerOff;
    _ModemPowerOff;
    _ModemPowerOff;
}

/*----------------------------------------------------------------------------*
 |   打开模块网络灯
 *----------------------------------------------------------------------------*/
void Modem_GDCDON(void)
{
    LOGONLIGHTON;
    LOGONLIGHTON;
}


/*----------------------------------------------------------------------------*
 |   关闭模块网络灯
 *----------------------------------------------------------------------------*/
void Modem_GDCDOFF(void)
{
    LOGONLIGHTOFF;
    LOGONLIGHTOFF;
}


/*----------------------------------------------------------------------------*
 |  启动模块
 *----------------------------------------------------------------------------*/
void Modem_ON(void)
{
    if(_GPRS_Modem_Run == _Modem_OFF)
    {
        /*
        _ModemONOFF_High;                                                                                                       //;先使模块ON/OFF引脚保持最少200ms的高电平
        Delay_nms(220);
        _ModemONOFF_Low;                                                                                                        //;M590要拉低最少300ms
        Delay_nms(500);
        _ModemONOFF_High;  
        */   
        _ModemONOFF_Low;  //;20140704                                                                                                       //;M590要拉低最少300ms
        Delay_nms(220);
        Watchdog();
        Delay_nms(220);
        Watchdog();
        Delay_nms(220);
        Watchdog();
        Delay_nms(220);
        Watchdog();
        Delay_nms(220);
        _ModemONOFF_High;                                                                                                  //;给模块的ON/OFF信号为高
    }
}

/*----------------------------------------------------------------------------*
 |  检测模块是否启动成功
 *----------------------------------------------------------------------------*/
void Modem_ON_CHK(void)
{
    static u8 On_Off_Sec;
/*    
    if(!Modem_OFF_CHK())     //;如果模块启动
    {
        if(GprsLC.LinkEnable == true)
        {
            On_Off_Sec = 0;
            if (!(GprsC.State & _SetUp))
            {
                GprsC.State = _SetUp;                                                                            //;写入模块启动标志
            }
        }
        else
        {
            On_Off_Sec = 0x00;
            if(!((GprsC.SendAT_Step == 20) || (GprsC.SendAT_Step == 99)))
            {                  
                Modem_OFF();//;关闭模块电源
            }
        }
    }
 */
 /*检测时段在线模块是否需要关闭20130613  zrt*/
     if(!Modem_OFF_CHK())     //;如果模块启动
    {
        if(GprsLC.LinkEnable == true)
        {
            On_Off_Sec = 0;
            if (!(GprsC.State & _SetUp))
            {
                GprsC.State = _SetUp;                                                                            //;写入模块启动标志
            }
        }
        else
        {
            On_Off_Sec = 0x00;
            if (GprsC.State & _HadLogin)  //;已经登录
            {
                GprsC.ReConnect = true;
                
            }
            else if((!GprsGyC.Status) &&(!GprsC.ReConnect)) //;已经断开连接和上送完报文
            {
            	if(!((GprsC.SendAT_Step == 20) || (GprsC.SendAT_Step == 99)))
                {                  
                    Modem_OFF();//;关闭模块电源
                }
            }
        }
    }
    else
    {
        if(GprsLC.LinkEnable == true)
        {
            On_Off_Sec++;                                                        //;模块关机计时
            if(On_Off_Sec == 2)
            {
                ByteN_Fill(sizeof(struct ModemContrl), (u8*)&GprsC, 0x00);
            }
            else if(On_Off_Sec == 6)
            {
                Modem_PowOFF();               //;10秒后关闭模块电源
            }
            else if(On_Off_Sec == 12)
            {
                Modem_PowON();               //;20秒后打开模块电源
            }
            else if(On_Off_Sec == 18)         //;30秒后模块开机
            {
                On_Off_Sec = 0;
                Modem_ON();
            }
        }
    }
}


/* 
********************************************************************************
* 函 数 名：检测模块启动
* 功    能：
* 参    数：true--关闭。false--启动
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：2011-06-09 09:51:50
********************************************************************************
*/
u8 Modem_OFF_CHK(void)
{
    if(_GPRS_Modem_Run == _Modem_OFF)  //;如果模块未启动
    {
        Delay_nms(20);
        if (_GPRS_Modem_Run == _Modem_OFF)
        {
            return true;
        }
    }
    if(_GPRS_Modem_Run == _Modem_ON)
    {
        Delay_nms(20);
        if (_GPRS_Modem_Run == _Modem_ON)
        {
            return false;
        }
    }
    return false;
}


/*----------------------------------------------------------------------------*
 |  强制关闭模块
 *----------------------------------------------------------------------------*/
void Modem_OFF(void)
{
    if (_GPRS_Modem_Run == _Modem_ON)                                                                                       //;如果模块启动
    {
        GprsC.ATStatus = 'E';
        GprsC.SendAT_Wait1s = 2;
        GprsC.SendAT_Step = 20;
        Modem_GDCDOFF(); //;关闭 网络灯
        GprsC.Status = 0x00;
        GprsC.State = 0x00;
        GprsC.Sign = 0x00;
    }
    else
    {
        Modem_PowOFF();
    }
}


/*----------------------------------------------------------------------------*
 |  强制重启模块
 *----------------------------------------------------------------------------*/
void Modem_OFFON(void)
{
    Modem_OFF();
    Modem_ON();
}


/*----------------------------------------------------------------------------*
 |  向模块发送AT命令
 *----------------------------------------------------------------------------*/
void Modem_AT_Send(char *str1)
{
    u8 i;

    if (GPRSUSARTCTR.SedOrd == idle) //;当前串口没有正在发命令
    {
        GprsC.ATStatus = 'S';
        GprsC.ATLen = strlen(str1);
        for (i = 0; i < GprsC.ATLen; i++)
        {
            GprsC.ATStr[i] = *(str1 + i);
        }
    
        for (; i < 50; i++)
        {
            GprsC.ATStr[i] = 0;
        }
    
        GPRSUSARTCTR.SedOrd = 'E';
        GPRSUSARTCTR.BufAdr = GprsC.ATStr; //;指向要发送的数据缓存
        GPRSUSARTCTR.TLong = GprsC.ATLen;  //;要发送的长度
    }
}


/*----------------------------------------------------------------------------*
 |  控制数据通过TCP协议发送出去
 *----------------------------------------------------------------------------*/
u8 Modem_TCP_Send(void)
{
    if(GprsC.ATStatus == idle)
    {
        if(GprsC.LinkState == LINK0)
        {
            if(GprsC.State & _Connect)
            {
                GprsC.ATStatus = 'E';
                GprsC.SendAT_Wait1s = 1;
                GprsC.SendAT_Step = 21;
            }
        }
        else if(GprsC.LinkState == LINK1)
        {
            if(GprsC.State1 & _Connect)
            {
                GprsC.ATStatus = 'E';
                GprsC.SendAT_Wait1s = 1;
                GprsC.SendAT_Step = 21;
            }
        }
    }

    if(GprsC.ATStatus == 'T') //;缓冲区数据准备发送
    {
        if (GPRSUSARTCTR.SedOrd == 0)
        {
            GPRSUSARTCTR.SedOrd = 'E';
            GprsC.ATStatus = 'S';
            GPRSUSARTCTR.TLong = GprsC.TCPLen;//;写入TCP要发送数据的长度
            GPRSSEDBUF[GPRSUSARTCTR.TLong] = 0x0d; //;为发送数据后加回车
            GPRSUSARTCTR.TLong++; //;发送长度
            GPRSUSARTCTR.BufAdr = GPRSSEDBUF;  //;发送的缓存地址
        }
    }

    if (GprsC.ATStatus == 'O')
    {
        GprsC.ATStatus = idle;
        if(GprsC.LinkState == LINK1)
        {
            if(GprsC.State1 & _HadLogin)
            {
                GprsC.LinkState = LINK0;
            }
        }
        return 0xFF; //;发送完成 
    }
    else if(GprsC.TCPFail == true) //;发送失败
    {
        GprsC.ATStatus = idle;
        GprsC.TCPFail = false; 
        if(GprsC.LinkState == LINK1)
        {
            if(GprsC.State1 & _HadLogin)
            {
                GprsC.LinkState = LINK0;
            }
        }
        return 0xDF;      
    }
    return 0xEF;
}

/*----------------------------------------------------------------------------*
 |  控制数据通过短信发送出去
 *----------------------------------------------------------------------------*/
u8 Modem_SMS_Send(void)
{
    if (GprsC.ATStatus == idle)
    {
        if (GprsC.SMSPer == true)
        {
            GprsC.ATStatus = 'E';
            GprsC.SendAT_Wait1s = 2;
            GprsC.SendAT_Step = 22;
        }
    }

    if(GprsC.ATStatus == 'M')
    {
        if(GPRSUSARTCTR.SedOrd == 0)
        {
            GPRSUSARTCTR.SedOrd = 'E';
            GprsC.ATStatus = 'S'; 
            GPRSUSARTCTR.TLong = GprsC.SMSLen; //;发送长度
            GPRSUSARTCTR.BufAdr = GPRSSEDBUF;  //;发送的缓存地址
        }
    }

    if (GprsC.ATStatus == 'O')
    {
        GprsC.ATStatus = idle;
        return true; //;发送完成
    }

    return false;
}


/*----------------------------------------------------------------------------*
 |  收到短信的AT指令：n+CMT:
 *----------------------------------------------------------------------------*/
void Modem_SMS_Rece(void)
{  
    char *string;
    char Order[10];
    u16 i;
    
    string = strstr(GPRSUSARTCTR.BufAdr, "L"); //;寻找首次出现L的位置
    if(string == NULL) return;     
    for (i = 0; i < 10; i++)
    {
        Order[i] = *(string + i); //;得到命令字
    }
    
    if(strcmp(Order, "LINKTOZHGC") == 0)
    {
        GprsLC.Link2Switch = true; //;允许备用主站的连接
        GprsLC.IP1_long = 0;
        
        string = strstr(GPRSUSARTCTR.BufAdr, "*"); //;寻找首次出现*的位置
        if(string == NULL) return;
        for (i = 0; i < 21; i++)   //;*IP,端口号*  中两个*号中间的最大位数为21位
        {
            if (*(string + i + 1) == 0x2A)  //;0x2A是字符*的ASCII码
            {
                break; //;末尾遇到*退出
            }
    
            GprsLC.IP1[i] = *(string + i + 1);
            GprsLC.IP1_long++;
        }
    }     
    else if(strcmp(Order, "*LINKTOMASTER*") == 0)   //;被动激活短信
    {
        if((ZDP.F8.WorkMode&0x03) == 0x02) //;被动激活模式
        {
            GprsLC.PassLinkOrd = true;
        }
    } 
}

/*----------------------------------------------------------------------------*
 |  发送命令AT，查看模块是否与单片机通讯正常
 *----------------------------------------------------------------------------*/
void AT01Comd_AT(void)
{
    if(GprsC.ATStatus == 'E')   
    {
        Modem_AT_Send("AT\r\0"); //;发送Send
    }   
    else if(strstr(GprsC.ATStr, "AT\r\r\nOK\r\n") != NULL)                                          //;收到ATOK后为波特率正常
    {
        GprsC.ATStatus = 'E'; 
        GprsC.SendAT_Wait1s = 1;
        GprsC.SendAT_Step = 3;                                              //;当前波特率可以正常通信不再需要设置
    }
    else if(strstr(GprsC.ATStr, "\r\nOK\r\n") != NULL)                                          //;收到ATOK后为波特率正常
    {
        GprsC.ATStatus = 'E'; 
        GprsC.SendAT_Wait1s = 1;
        GprsC.SendAT_Step = 3;                                              //;当前波特率可以正常通信不再需要设置
    } 
}


/*----------------------------------------------------------------------------*
 |  设置模块的波特率为9600
 *----------------------------------------------------------------------------*/
void AT02Comd_IPR(void)
{
    if(GprsC.ATStatus == 'E')   
    {
        Modem_AT_Send("AT+IPR=9600\r\0");
    }      
    else if(strstr(GprsC.ATStr, "AT+IPR=9600\r") != NULL)
    {
        ;                                          //;继续接收                                                                  //;返回两条命令，接到第二条才算设置成功
    }
    else if(strstr(GprsC.ATStr, "\r\nOK\r\n") != NULL)                    //;波特率修改成功
    {
        GprsC.ATStatus = 'E'; 
        GprsC.SendAT_Wait1s = 2;
        GprsC.SendAT_Step = 1;
        USART2_BaudRateSet(9600);                                         //;再设回与模块通信的串口波特率
    }
}


/*----------------------------------------------------------------------------*
 |  设置模块的回显
 *----------------------------------------------------------------------------*/
void AT03Comd_ATE0(void)
{
    if(GprsC.ATStatus == 'E')   
    {
        Modem_AT_Send("ATE0\r\0");
    }   
    else if(strstr(GprsC.ATStr, "ATE0\r\r\nOK\r\n") != NULL)
    {
        GprsC.ATStatus = 'E';
        GprsC.SendAT_Wait1s = 1;
        GprsC.SendAT_Step = 4;
    }
    else if(strstr(GprsC.ATStr, "\r\nOK\r\n") != NULL)
    {
        GprsC.ATStatus = 'E';
        GprsC.SendAT_Wait1s = 1;
        GprsC.SendAT_Step = 4;
    }
}



/*----------------------------------------------------------------------------*
 |  检测SIM卡是否在位
 *----------------------------------------------------------------------------*/
void AT04Comd_CPIN(void)
{
    if(GprsC.ATStatus == 'E')   
    {
        Modem_AT_Send("AT+CPIN?\r\0");
    }       
    /*else if(strstr(GprsC.ATStr, "\r\n+CPIN: READY\r\n\r\nOK\r\n") != NULL)     //;有sim卡，且不用输入PIN码*/
    else if(strstr(GprsC.ATStr, "\r\n+CPIN: READY\r\nOK\r\n") != NULL)     //;有sim卡，且不用输入PIN码 20140704
    {
        GprsC.ATStatus = 'E';
        GprsC.SendAT_Wait1s = 1;
        GprsC.SendAT_Step = 5;         //;下一步检测模块信号
        GprsC.Status &= (~_NoCard);
        GprsC.CPINErrNum = 0;
    }
    else if(strstr(GprsC.ATStr, "\r\nERROR\r\n") != NULL)                 //;SIM不存在
    {
        GprsC.ATStatus = 'E';
        GprsC.CPINErrNum++;
        if(GprsC.CPINErrNum < 30)
        {
            GprsC.SendAT_Wait1s = 2;
            GprsC.SendAT_Step = 4;
            GprsC.Status = _NoCard;
            Modem_GDCDOFF();      //;关掉网络灯
            GprsC.State = 0;   
            GprsC.Sign = 0;
        }
        else
        {   
            GprsC.CPINErrNum = 0;
            Modem_OFF();     
        }
    }
}


/* 
********************************************************************************
* 函 数 名：AT05Comd_CSQ
* 功    能：检测模块信号CSQ
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：2012-01-31 14:46:49 增加重拨状态的处理
********************************************************************************
*/
void AT05Comd_CSQ(void)
{
    u8 ch1, ch2;
    
    if(GprsC.ATStatus == 'E')   
    {
        Modem_AT_Send("AT+CSQ\r\0");  //;读信号的强度
    }       
    else if(strstr(GprsC.ATStr, "\r\n+CSQ: ") != NULL)
    {  
         //;NEW
        GprsC.SignR = 'R'; //;读取信号成功
        GprsC.ATStatus = 'E';
        ch1 = *(GprsC.ATStr + 8);
        ch2 = ch1 - 0x30;
        ch1 = *(GprsC.ATStr + 9);
        if (ch1 == ',')
        {
            ch1 = ch2;
        }
        else if ((ch1 >= 0x30) && (ch1 <= 0x39))
        {
            ch1 -= 0x30;
            ch1 += (ch2 * 10);
        }
        else
        {
            ch1 = ch2;
        }
        if (ch1 == 99)
        {
            ch1 = 0;
        }

        GprsC.Sign = ch1;

        if(GprsC.State & _Link)          //;当前状态为发起连接(Link)
        {
            //if((GprsC.Sign > 10) && (GprsC.Sign <= 31))                    //;信号强度在大于10，小于31时才可以正常收发数据
            if((GprsC.Sign > 1) && (GprsC.Sign <= 35))  //;qiu20140416
            {
                GprsC.SendAT_Wait1s = 1;
                GprsC.SendAT_Step = 6;
                GprsC.CSQErrNum = 0;
            }
            else
            {
                GprsC.CSQErrNum++;
                if (GprsC.CSQErrNum < 30)
                {
                    GprsC.SendAT_Wait1s = 2;
                    GprsC.SendAT_Step = 5;
                }
                else
                {
                    GprsC.SendAT_Wait1s = 2;
                    GprsC.SendAT_Step = 20;                                  //;重启模块
                    GprsC.CSQErrNum = 0;
                }
            }
        }
        else       //;当前连接状态
        {
            //if((GprsC.Sign > 10) && (GprsC.Sign <= 31))
            if((GprsC.Sign > 1) && (GprsC.Sign <= 35))  //;qiu20140416
            {                 
                /*if(GprsC.State & _Connect)  
                {      
                    GprsC.SendAT_Wait1s = 1;       
                    GprsC.SendAT_Step = 15;       //;查看链路1
                }
                else //;重拨状态
                {
                    GprsC.ATStatus = 'R';
                }
                */
                GprsC.ATStatus = 'R';
            }
            else
            {
                GprsC.CSQErrNum++;
                if (GprsC.CSQErrNum < 30)
                {
                    GprsC.SendAT_Wait1s = 2;
                    GprsC.SendAT_Step = 5;
                }
                else
                {
                    Modem_GDCDOFF();                       //;关掉网络灯
                    GprsC.State = 0;                       //;主动断开连接成功                
                    GprsC.Status = 0;
                    GprsC.SendAT_Wait1s = 2;
                    GprsC.SendAT_Step = 20;                //;重启模块
                    GprsC.CSQErrNum = 0;
                }
            }
        }
    }
}


/*----------------------------------------------------------------------------*
 |  检测模块是否注册CREG
 *----------------------------------------------------------------------------*/
void AT06Comd_CREG(void)
{
    if(GprsC.ATStatus == 'E')  
    {
        Modem_AT_Send("at+creg?\r\0");
    }   
    /*else if((strstr(GprsC.ATStr, "\r\n+CREG: 0,1\r\n\r\nOK\r\n")!= NULL) ||
             (strstr(GprsC.ATStr,"\r\n+CREG: 0,5\r\n\r\nOK\r\n") != NULL))      //;已经注册为本地状态或漫游的状态
    */
    else if((strstr(GprsC.ATStr, "\r\n+CREG: 0,1\r\nOK\r\n")!= NULL) ||
             (strstr(GprsC.ATStr,"\r\n+CREG: 0,5\r\nOK\r\n") != NULL))      //;已经注册为本地状态或漫游的状态 20140704
    {
        GprsC.ATStatus = 'E';
        GprsC.SendAT_Wait1s = 1;
        GprsC.SendAT_Step = 7;
        GprsC.Status |= _FindHost;                                           //;正在寻找主站
        GprsC.CREGErrNum = 0;
    }
    /*
    else if((strstr(GprsC.ATStr,"\r\n+CREG: 0,0\r\n\r\nOK\r\n") != NULL) ||
             (strstr(GprsC.ATStr,"\r\n+CREG: 0,2\r\n\r\nOK\r\n") != NULL) ||
             (strstr(GprsC.ATStr,"\r\n+CREG: 0,3\r\n\r\nOK\r\n") != NULL) ||
             (strstr(GprsC.ATStr,"\r\n+CREG: 0,4\r\n\r\nOK\r\n") != NULL)) //;还在寻找注册中
    */
    else if((strstr(GprsC.ATStr,"\r\n+CREG: 0,0\r\nOK\r\n") != NULL) ||
             (strstr(GprsC.ATStr,"\r\n+CREG: 0,2\r\nOK\r\n") != NULL) ||
             (strstr(GprsC.ATStr,"\r\n+CREG: 0,3\r\nOK\r\n") != NULL) ||
             (strstr(GprsC.ATStr,"\r\n+CREG: 0,4\r\nOK\r\n") != NULL)) //;还在寻找注册中 20140704
    {

        GprsC.ATStatus = 'E';
        GprsC.CREGErrNum++;
        if (GprsC.CREGErrNum < 30)
        {
            GprsC.SendAT_Wait1s = 2;
            GprsC.SendAT_Step = 5;                   //;读信号
            GprsC.Status |= _FindHost;              //;正在寻找主站
        }
        else                   //;附着GPRS失败,信号不好，或无效卡
        {
            GprsC.CREGErrNum = 0;
            Modem_OFF();
        }
    }
}


 /* 
********************************************************************************
* 函 数 名：
* 功    能：
* 参    数：
* 返    回： 
* 说    明：AT+CMGF=1为文本模式 =0为PDU模式
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void AT07Comd_CMGF(void)
{    
    if(GprsC.ATStatus == 'E')  
    {
        Modem_AT_Send("AT+CMGF=1\r\0");
    }    
    else if(strstr(GprsC.ATStr, "\r\nOK\r\n") != NULL)                          //;发短信设为文本模式
    {
        GprsC.ATStatus = 'E';
        GprsC.SendAT_Wait1s = 1;
        GprsC.SendAT_Step = 8;
    }
}


/*----------------------------------------------------------------------------*
 |  设置TE字符集为UCS2
 *----------------------------------------------------------------------------*/
void AT08Comd_UCS2(void)
{
    if(GprsC.ATStatus == 'E')  
    {
           //; Modem_AT_Send("AT+CSCS=\"UCS2\"\r\0");
       Modem_AT_Send("AT+CSCS=\"GSM\"\r\0");
    }  
    else if(strstr(GprsC.ATStr, "\r\nOK\r\n") != NULL)                          //;设置TE字符集为GSM
    {
        GprsC.ATStatus = 'E';
        GprsC.SendAT_Wait1s = 1;
        GprsC.SendAT_Step = 9;
    }
}


/*----------------------------------------------------------------------------*
 |  新短信内容直接显示而不存贮在SIM卡
 *----------------------------------------------------------------------------*/
void AT09Comd_CNMI(void)
{   
    if(GprsC.ATStatus == 'E')  
    {
        Modem_AT_Send("AT+CNMI=2,2,0,0,0\r\0");
    }     
    else if(strstr(GprsC.ATStr, "\r\nOK\r\n") != NULL)
    {
        GprsC.ATStatus = 'E';
        GprsC.SMSPer = true;                                                                                                //;完成发送短信的配置
        GprsC.SendAT_Wait1s = 1;
        GprsC.SendAT_Step = 10;
    }
}


/*----------------------------------------------------------------------------*
 |  使用内部协议栈
 *----------------------------------------------------------------------------*/
void AT10Comd_XISP(void)
{  
    if(GprsC.ATStatus == 'E')  
    {
        Modem_AT_Send("AT+XISP=0\r\0");
    }    
    else if(strstr(GprsC.ATStr, "\r\nOK\r\n") != NULL)
    {
        GprsC.ATStatus = 'E';
        GprsC.SendAT_Wait1s = 1;
        GprsC.SendAT_Step = 11;
    }
}


/*----------------------------------------------------------------------------*
 |  设置APN：AT + CGDCONT = 1, "IP", "APN"
 *----------------------------------------------------------------------------*/
void AT11Comd_CGDCONT(char *APN)
{
    if(GprsC.ATStatus == 'E')
    {                         
        strcpy(GprsC.ATStr, "AT+CGDCONT=1,\"IP\",\"\0");
        strcat(GprsC.ATStr, APN);
        strcat(GprsC.ATStr, "\"\r\0");
        Modem_AT_Send(GprsC.ATStr);
    }    
    else if(strstr(GprsC.ATStr, "\r\nOK\r\n") != NULL)
    {
        GprsC.ATStatus = 'E';
        GprsC.SendAT_Wait1s = 1;
        GprsC.SendAT_Step = 12;
    }
}


/*----------------------------------------------------------------------------*
 |  建立PPP连接
 *----------------------------------------------------------------------------*/
void AT12Comd_XIIC(void)
{
    if(GprsC.ATStatus == 'E')
    {                         
        Modem_AT_Send("AT+xiic=1\r\0");
    }    
    else if (strstr(GprsC.ATStr, "\r\nOK\r\n") != NULL)
    {
        GprsC.ATStatus = 'E';
        GprsC.SendAT_Wait1s = 1;
        GprsC.SendAT_Step = 13;
    }
}


/*----------------------------------------------------------------------------*
 |  查询PPP链路状态
 *----------------------------------------------------------------------------*/
void AT13Comd_XIIC(void)
{
    if(GprsC.ATStatus == 'E')
    {                         
        Modem_AT_Send("AT+xiic?\r\0");
    }    
    else if (strstr(GprsC.ATStr, "\r\n+XIIC:    1,") != NULL)                    //;PPP连接建立成功
    {
        GprsC.ATStatus = 'E';
        GprsC.SendAT_Wait1s = 1;
        GprsC.SendAT_Step = 14;
        GprsC.XIICErrNum = 0;
    }
    else if (strstr(GprsC.ATStr, "\r\n+XIIC:    0, 0.0.0.0") != NULL)       //;PPP连接建立失败,可能是APN设错，或者欠费了，或者信号不好
    {
        GprsC.ATStatus = 'E';
        GprsC.XIICErrNum++;
        if (GprsC.XIICErrNum < 30)
        {
            GprsC.SendAT_Step = 13;
            GprsC.SendAT_Wait1s = 2;
        }
        else
        {
            GprsC.XIICErrNum = 0;
             //;重启模块
            Modem_OFF();
        }
    }
}

/* 
********************************************************************************
* 函 数 名：NumIpToChar
* 功    能：将数值IP和端口转换为字符并保存在指定缓冲区。
* 参    数：Buf--保存转换后的字符；IP--IP存放缓冲；Port--端口号。
* 返    回：转换后字符长度
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：2012-03-26 10:25:18
********************************************************************************
*/
u8 NumIpToChar(char *Buf, u8 *IP, u16 Port)
{
    u8  j;
    u8  len = 0;
    u8  ch1 = 0;
    u8  ch2 = 0;
    u16 sh;
    
    for(j = 0; j < 4; j++)    //;主站IP
    {
        ch1 = *(IP + j) / 100; //;百位
        if(ch1 != 0)
        {
            ch2 = 1;
            *(Buf + len++) = ch1 + '0';
        }
        ch1 = (*(IP + j) % 100) / 10; //;十位
        if((ch2 != 0) || (ch1 != 0))
        {
            *(Buf + len++) = ch1 + '0';
        }
        ch1 = *(IP + j) % 10; //;个位
        *(Buf + len++) = ch1 + '0';
        ch2 = 0;
        *(Buf + len++) = '.';
    }
    len--;
    *(Buf + len++) = ',';
    
    sh = Port; //;端口号
    ch1 = sh / 10000;
    if(ch1 != 0)
    {
        ch2 = 1; //;高位不为0
        *(Buf + len++) = ch1 + '0';
    }
    ch1 = (sh % 10000) / 1000;
    if((ch2 != 0) || (ch1 != 0))
    {
        ch2 = 1;
        *(Buf + len++) = ch1 + '0';
    }
    ch1 = (sh % 1000) / 100;
    if((ch2 != 0) || (ch1 != 0))
    {
        ch2 = 1;
        *(Buf + len++) = ch1 + '0';
    }
    ch1 = (sh % 100) / 10;
    if((ch2 != 0) || (ch1 != 0))
    {
        ch2 = 1;
        *(Buf + len++) = ch1 + '0';
    }
    ch1 = sh % 10;
    *(Buf + len++) = ch1 + '0';
    
    return len;
}



/*----------------------------------------------------------------------------*
 |  建立TCP连接，AT指令：AT + TCPSETUP = 0, IP, Port
 *----------------------------------------------------------------------------*/
void AT14Comd_TCPSETUP(u8 Link, u8 *IP, u16 Port)
{
     u8  i = 0, j;
     
    if(GprsC.ATStatus == 'E')
    {                                
        strcpy(GprsC.ATStr, "AT+TCPSETUP=");
        i += 12;
        *(GprsC.ATStr + i++) = Link + 0x30;
        *(GprsC.ATStr + i++) = ',';
        if(GprsC.LinkState == LINK0) 
        {           
            i += NumIpToChar(GprsC.ATStr + i, IP, Port);       
            *(GprsC.ATStr + i++) = '\r';
            *(GprsC.ATStr + i++) = '\0';
            
            if(ZDS.F10.Flux_Month < FluxLimValue)
            {
                Modem_AT_Send(GprsC.ATStr);
                if (GprsC.ATStatus == 'S')
                {
                /*为什么加500个字节???泰瑞捷没有加，所以去掉，by zrt 130705
                    ZDS.F10.Flux_Day += 500;
                    ZDS.F10.Flux_Month += 500;
                */
                }
            }
            else
            {
                #ifdef ZS
                RecordErr(1);
                #endif
                GprsC.ATStatus = idle;
                GprsC.State &= (~_Link); //;退出连接状态
                GprsC.State |= _Redial;  //;进入重拨状态
            }
        }
        else
        {
            for(j = 0; j < GprsLC.IP1_long; j++)
            {
                *(GprsC.ATStr + i++) = GprsLC.IP1[j];
            }
            *(GprsC.ATStr + i++) = '\r';
            *(GprsC.ATStr + i++) = '\0';
            Modem_AT_Send(GprsC.ATStr);
        }
                
    }       
    else if(strstr(GprsC.ATStr, "\r\nOK\r\n") != NULL)    //;并不代表已经实现连接
    {
        ;              //;继续接收,
    }
    else if(strstr(GprsC.ATStr, "\r\n+TCPSETUP:0,OK\r\n") != NULL)             //;模块与主站已经实现连接
    {
        GprsC.ATStatus = 'R';
        GprsC.State &= (~_Link);                                 //;退出连接状态
        GprsC.State |= _Connect;                                //;表示网当前与主站连接成功
        GprsC.Status &= ~_FindHFalst;
        GprsC.Status &= ~_FindHost;                            //;表示寻找主站结束
        GprsC.Status |= _ContHost;                            //;@11 寻找主站失败
       
    }
    else if(strstr(GprsC.ATStr, "\r\n+TCPSETUP:0,FAIL\r\n")!= NULL)            //;连接失败，可能是主站没打开或网络不通
    {
        #ifdef ZS
                RecordErr(2);
        #endif
        GprsC.ATStatus = 'R';
        GprsC.State &= (~_Link);                                                //;退出连接状态
        GprsC.State |= _Redial;                                                 //;进入重拨状态
        GprsC.Status &= ~_FindHost;                                             //;表示寻找主站结束
        GprsC.Status |= _FindHFalst;                                            //;@11 寻找主站失败
    }
    else if(strstr(GprsC.ATStr, "\r\n+TCPSETUP:Error n\r\n") != NULL)
    {
             //;重启模块
            Modem_OFF();
    }
    else if(strstr(GprsC.ATStr, "\r\n+TCPSETUP:1,OK\r\n") != NULL)             //;模块与主站已经实现连接
    {
        GprsC.ATStatus = 'R';
        GprsLC.Link2Switch = false; //;不再允许备用连接
        GprsC.State1 &= (~_Link);                                                //;退出连接状态
        GprsC.State1 |= _Connect;                                                //;表示网当前与主站连接成功
    }
    else if (strstr(GprsC.ATStr, "\r\n+TCPSETUP:1,FAIL\r\n")!= NULL)            //;连接失败，可能是主站没打开或网络不通
    {
        GprsC.ATStatus = 'R';
        GprsLC.Link2Switch = false; //;不再允许备用连接
        GprsC.State1 = 0;       
        GprsC.LinkState = LINK0;                                         //;退出连接状态
       
    }
}

/*----------------------------------------------------------------------------*
 |  检测模块TCP链路是否正常IPSTATUS
 *----------------------------------------------------------------------------*/
void AT15Comd_IPSTATUS(u8 Link)
{
    if (GprsC.ATStatus == 'E')
    {                         
         u8 i = 0;        
         strcpy(GprsC.ATStr, "AT+IPSTATUS=");
         i += 12;
         *(GprsC.ATStr + i++) = Link + 0x30;
         *(GprsC.ATStr + i++) = '\r';
         *(GprsC.ATStr + i++) = '\0';
         Modem_AT_Send(GprsC.ATStr);
    }   
    else if (strstr(GprsC.ATStr, "\r\n+IPSTATUS:0,CONNECT") != NULL)             //;链路1处于连接状态
    {
        GprsC.ATStatus = 'R';
    }
    else if (strstr(GprsC.ATStr, "\r\n+IPSTATUS:0,DISCONNECT") != NULL)     //;链路1处于断开状态
    {
        #ifdef ZS
                RecordErr(3);
        #endif
        GprsC.ATStatus = 'R';
        Modem_GDCDOFF();                                                     //;关掉网络灯
        GprsC.State &= ~_Connect;
        GprsC.State &= (~_HadLogin);
        GprsC.State &= (~_Link); //;退出连接状态
        GprsC.State |= _Redial;  //;进入重拨状态
        GprsC.Status &= ~_ContHost;
        GprsC.Status |= _FindHFalst;  
    }
     else if (strstr(GprsC.ATStr, "\r\n+IPSTATUS:1,CONNECT") != NULL)             //;链路1处于连接状态
    {
        GprsC.ATStatus = 'E';
        GprsC.SendAT_Wait1s = 2;
        GprsC.SendAT_Step = 17;
    }
    else if (strstr(GprsC.ATStr, "\r\n+IPSTATUS:1,DISCONNECT") != NULL)     //;链路1处于断开状态
    {
        GprsC.ATStatus = 'R';                                                    //;关掉网络灯
        GprsC.State1 = 0x00;
        GprsC.LinkState = LINK0;
      
    }
}


/*----------------------------------------------------------------------------*
 |  强制断开模块TCP链路TCPCLOSE
 *----------------------------------------------------------------------------*/
void AT17Comd_TCPCLOSE(u8 Link)
{
    if (GprsC.ATStatus == 'E')
    {                         
        u8 i = 0;       
        strcpy(GprsC.ATStr,"AT+TCPCLOSE=");
        i += 12;
        *(GprsC.ATStr + i++) = Link + 0x30;
        *(GprsC.ATStr + i++) = '\r';
        *(GprsC.ATStr + i++) = '\0';
        Modem_AT_Send(GprsC.ATStr);
    }    
    else if (strstr(GprsC.ATStr, "\r\n+TCPCLOSE:0,OK\r\n") != NULL)              //;链路1成功断开
    {
        #ifdef ZS
                RecordErr(4);
        #endif
        GprsC.ATStatus = 'R';
        Modem_GDCDOFF();                                                     //;关掉网络灯
        GprsC.State &= (~_Connect);                                           //;主动断开连接成功
        GprsC.State &= (~_HadLogin);
        GprsC.State &= (~_Link); //;退出连接状态
        GprsC.State |= _Redial;  //;进入重拨状态
        GprsC.Status = 0;
    }
    else if(strstr(GprsC.ATStr, "\r\n+TCPCLOSE:1,OK\r\n") != NULL)              //;链路1成功断开
    {
        GprsC.ATStatus = 'R';       
        GprsC.State1 = 0x00;
        GprsC.LinkState = LINK0;
    }
}


/*----------------------------------------------------------------------------*
 |  软关机
 *----------------------------------------------------------------------------*/
void AT20Comd_CPWROFF(void)
{   
    if(GprsC.ATStatus == 'E')
    {
        Modem_AT_Send("AT+CPWROFF\r\0");                                     //;软关机
    }    
    else if(strstr(GprsC.ATStr, "\r\nOK\r\n") != NULL)
    {
         GprsC.ATStatus = 'R'; 
    }
    else if(strstr(GprsC.ATStr, "\r\nCME ERROR:<error>\r\n") != NULL) //;错误
    {
        GprsC.ATStatus = 'R';  
        Modem_PowOFF();
    }
}


/*----------------------------------------------------------------------------*
 |  AT指令：AT + TCPSEND = 0,XXX;
 *----------------------------------------------------------------------------*/
void AT21Comd_TCPSEND(u8 Link, u16 Len)
{
    if (GprsC.ATStatus == 'E')
    {
        u8 i = 0;       
        strcpy(GprsC.ATStr, "AT+TCPSEND=");
        i += 11;
        *(GprsC.ATStr + i++) = Link + 0x30;
        *(GprsC.ATStr + i++) = ',';
        *(GprsC.ATStr + i++) = Len / 100 + 0x30;                                //;写入需要通过TCP协议发送的数据的长度
        *(GprsC.ATStr + i++) = (Len % 100) / 10 + 0x30;
        *(GprsC.ATStr + i++) = (Len % 100) % 10 + 0x30;
        *(GprsC.ATStr + i++) = '\r';
        *(GprsC.ATStr + i++) = '\0';
        Modem_AT_Send(GprsC.ATStr);
    }   
    else if (strstr(GprsC.ATStr, "\r\n>") != NULL)
    {
        GprsC.ATStatus = 'T';                                               //;可以通过TCP协议发送数据
    }
    else if (strstr(GprsC.ATStr, "\r\n+TCPSEND:0") != NULL)
    {
        GprsC.ATStatus = 'O';                                               //;通过链路1TCP协议发送数据完成
    }
     else if(strstr(GprsC.ATStr, "\r\n+TCPSEND:1") != NULL)
    {
        GprsC.ATStatus = 'O';                                               //;通过链路1TCP协议发送数据完成
    }
    else if(strstr(GprsC.ATStr, "\r\n+TCPSEND:Error"))  //;链路尚未建立，可能处于假链接状态
    {
        GprsC.TCPFail = true; 
        if(GprsC.LinkState == LINK0)
        {
            if(GprsC.State & _HadLogin)   
            {
                #ifdef ZS
                RecordErr(5);
                #endif
                Modem_GDCDOFF();                                                     //;关掉网络灯
                GprsC.State &= (~_Connect);                                           //;主动断开连接成功
                GprsC.State &= (~_HadLogin);
                GprsC.State &= (~_Link); //;退出连接状态
                GprsC.State |= _Redial;  //;进入重拨状态
                GprsC.Status = 0x00;      //;清除状态
            }   
        }
        
    }
}


/*----------------------------------------------------------------------------*
 |  AT指令：AT + CMGS = XXX;
 *----------------------------------------------------------------------------*/
void AT22Comd_CMGS(u16 Len)
{
    if (GprsC.ATStatus == 'E')
    {
        u8 i = 0;    
        strcpy(GprsC.ATStr,"AT+CMGS=");
        i += 8;
        *(GprsC.ATStr + i++) = Len / 100 + 0x30;                                //;写入需要通过短信发送的数据的长度
        *(GprsC.ATStr + i++) = (Len % 100) / 10 + 0x30;
        *(GprsC.ATStr + i++) = (Len % 100) % 10 + 0x30;
        *(GprsC.ATStr + i++) = '\r';
        *(GprsC.ATStr + i++) = '\0';
        Modem_AT_Send(GprsC.ATStr);
        GprsC.CMGSNum = 0;
    }    
    else if (strstr(GprsC.ATStr, "\r\n>") != NULL)
    {
        GprsC.CMGSNum++;
        if (GprsC.CMGSNum == 1)
        {
           GprsC.ATStatus = 'M';                                               //;可以发送短信数据
        }
        else if (GprsC.CMGSNum == 2)
        {
           GprsC.ATStatus = 'R'; 
        }
    }
    else if (strstr(GprsC.ATStr, "+CMGS:") != NULL)
    {
        GprsC.CMGSNum = 0;
        GprsC.ATStatus = 'O';   //;Over                                            //;发送短信成功
    }
}


/*----------------------------------------------------------------------------*
 |  向模块发生完AT指令以后，等待若时间以后没有接收到应答
 *----------------------------------------------------------------------------*/
void ATComdSetNoRec(void)
{
    static u8 OldStep, firstsign = true;
    
    if(OldStep != GprsC.SendAT_Step) //;清除重发标志位
    {
        OldStep = GprsC.SendAT_Step;
        GprsC.ATAns_Wait1s = 0;
        GprsC.ReSendAT_Num = 0;
    }
    
    if(GprsC.ATStatus == 'E')
    {
        if (GprsC.SendAT_Wait1s > 0)
        {
            GprsC.SendAT_Wait1s--;
        }
    }

    else if(GprsC.ATStatus == 'S')
    {
        GprsC.ATAns_Wait1s++;
        if(GprsC.ATAns_Wait1s > 15)
        {
            GprsC.ATAns_Wait1s = 0;
            GprsC.ReSendAT_Num++;
            if(GprsC.ReSendAT_Num < 2)
            {
                if(GprsC.SendAT_Step == 1)
                {
                    if(firstsign)
                    {
                        firstsign = false;
                        USART2_BaudRateSet(115200); //;第一次与模块通信要将单片机的波特率设为115200;
                        GprsC.ATStatus = 'E';
                        GprsC.SendAT_Wait1s = 3;
                        GprsC.SendAT_Step = 2;    //;模块没AT的返回，需要到STEP2去设模块的波特率
                    }
                    else
                    {
                        Modem_PowOFF();
                    }
                }
                else if(GprsC.SendAT_Step == 2)
                {
                    USART2_BaudRateSet(9600); //;第一次与模块通信要将单片机的波特率设为115200;
                    GprsC.ATStatus = 'E';
                    GprsC.SendAT_Wait1s = 3;
                    GprsC.SendAT_Step = 1;    //;模块没AT的返回，需要到STEP2去设模块的波特率
                }
                else if(GprsC.SendAT_Step == 20) //;软关机无应答则断电
                {
                    Modem_PowOFF();
                }
                else
                {
                    GprsC.ATStatus = 'E';
                    GprsC.SendAT_Wait1s = 3;   //;重发
                }
            }
            else
            {
                if(GprsC.SendAT_Step == 21)
                {
                    GprsC.TCPFail = true;
                }
                if(GprsC.LinkState == LINK1) //;若备用主站连接
                {
                    GprsLC.Link2Switch = false; //;不再允许备用连接
                    GprsC.State1 = 0x00; 
                    GprsC.LinkState = LINK0;
                }             
                Modem_OFF();        //;关闭模块                
            }
        }
    }
    
       
    if(GprsC.ATStatus != idle)  //;AT状态防错
    {
        GprsC.ATSOVT++;
        if(GprsC.ATSOVT > 240)
        {
            GprsC.ATStatus = idle; //;状态复位
            Modem_PowOFF();
        }
    }
    else
    {
        GprsC.ATSOVT = 0;
    }
}



/*----------------------------------------------------------------------------*
 |  模块主动发送的AT命令后，根据模块回复的AT命令进行处理
 *----------------------------------------------------------------------------*/
void Modem_AT_Deal(void)
{    
    
    switch (GprsC.SendAT_Step)
    {
        case 1:
        {
            AT01Comd_AT();  //; 查看模块是否启动成功
            break;
        }
        case 2:
        {
            AT02Comd_IPR();//;重设波特率
            break;
        }
        case 3:
        {
            AT03Comd_ATE0(); //;关闭回显
            break;
        }
        case 4:
        {
            AT04Comd_CPIN(); //;检测sim卡的状态
            break;
        }
        case 5:
        {
            AT05Comd_CSQ();  //;检测模块信号强度
            break;
        }
        case 6:
        {
            AT06Comd_CREG(); //;检查GSM网络注册情况
            break;
        }
        case 7:
        {
            AT07Comd_CMGF(); //;发短信设为PDU模式
            break;
        }
        case 8:
        {
            AT08Comd_UCS2();  //;设置TE字符集为UCS2
            break;
        }
        case 9:
        {
            AT09Comd_CNMI(); //;新短信内容直接显示而不存贮在SIM卡
            break;
        }
        case 10:
        {
            AT10Comd_XISP();  //;使用内部协议栈
            break;
        }
        case 11:
        {
            AT11Comd_CGDCONT(GprsC.APN);
            break;
        }
        case 12:
        {
            AT12Comd_XIIC(); //; 建立PPP 连接
            break;
        }
        case 13:
        {
            AT13Comd_XIIC(); //;查询PPP链路状态
            break;
        }
        case 14:
        {
            AT14Comd_TCPSETUP(GprsC.LinkState, GprsC.IP, GprsC.Port);//;链路1建立TCP连接
            break;
        }
        case 15:
        {
            AT15Comd_IPSTATUS(GprsC.LinkState);   //;查看链路1连接是否正常
            break;
        }
        case 17:
        {
            AT17Comd_TCPCLOSE(GprsC.LinkState); //;假连接要强制断开链路1
            break;
        }
        case 20:
        {
            AT20Comd_CPWROFF(); //;软关机
            break;
        }
        case 21:
        {
            AT21Comd_TCPSEND(GprsC.LinkState, GprsC.TCPLen); //;AT+TCPSEND=0,XXX
            break;
        }
        case 22:
        {
            AT22Comd_CMGS(GprsC.SMSLen); //;AT + CMGS = XXX
            break;
        }      
        default:
        {
            GprsC.ATStatus =0;
            GprsC.ReConnect =true;						
            break;
        }
    }  
}


/*----------------------------------------------------------------------------*
 |  GPRS模块通讯
 *----------------------------------------------------------------------------*/
void Modem_Com(void)
{
     u16 i;
    
    if((GprsC.ATStatus == 'E')&&(GprsC.SendAT_Wait1s == 0)) //;有数据需要发送
    {
         Modem_AT_Deal();
    }

    if(GPRSUSARTCTR.RecOrd == true)
    {
        GPRSUSARTCTR.RecOrd = false;
        for (i = 0; i < GPRSUSARTCTR.RecLen; i++)  //;只接收前面最多50个字节
        {
            if (i == 50)  break;
            GprsC.ATStr[i] =  *(GPRSUSARTCTR.BufAdr + i);
        }
        
        GprsC.ATLen = i;
        
        for (; i < 50; i++)
        {
            GprsC.ATStr[i] = 0;
        }
        
        if(GprsC.ATStatus == 'S')
        {
            Modem_AT_Deal();
        }
        else if(strstr(GprsC.ATStr, "+TCPRECV:0,") != NULL)   //;收到主站发来的数据
        {
            TCPDATALIGHTON(); //;数据灯亮
            GprsC.DataLightMs = 200;
            GprsC.NDataMin0 = 0x0000; //;接收到数据，TCP无数据计时清零
            GprsC.LinkState = LINK0;
            GprsC.TCPRec = true;  //;收到TCP数据
            GprsC.TCPBuf = GPRSUSARTCTR.BufAdr;
            GprsC.TCPLen = GPRSUSARTCTR.RecLen - 11;
            GPRSUSARTCTR.RecLen = 0;
        }
        else if(strstr(GprsC.ATStr, "+TCPRECV:1,") != NULL)   //;收到主站发来的数据
        {
            GprsC.LinkState = LINK1;
            GprsC.NDataMin1 = 0x00;
            GprsC.TCPRec = true;  //;收到TCP数据
            GprsC.TCPBuf = GPRSUSARTCTR.BufAdr;
            GprsC.TCPLen = GPRSUSARTCTR.RecLen - 11;
            GPRSUSARTCTR.RecLen = 0;
        }
        else if(strstr(GprsC.ATStr, "\r\n+TCPCLOSE:0,Link Closed\r\n") != NULL)     //;网络连接已经断开
        {
            #ifdef ZS
                RecordErr(6);
        #endif
            GprsC.ATStatus = 'R';
            Modem_GDCDOFF();                                                     //;关掉网络灯
            GprsC.State &= (~_Connect);                                          //;表示网络当前为断开状态
            GprsC.State &= (~_HadLogin);                                         //;当前不处于登陆的状态
            GprsC.State &= (~_Link); //;退出连接状态
            GprsC.State |= _Redial;  //;进入重拨状态
            GprsC.Status = 0;
        }
        else if(strstr(GprsC.ATStr, "\r\n+TCPCLOSE:1,Link Closed\r\n") != NULL)     //;网络连接已经断开
        {
            GprsC.State1 = 0x00;
        }
        else if(strstr(GprsC.ATStr, "\r\n+CMT:") != NULL)
        {
            Modem_SMS_Rece();   //;收到短信的处理
        }

  
        for(i = 0; i < GprsC.ATLen; i++)
        {
            GprsC.ATStr[i] = 0; //;清除AT命令准备发送
        }
        GprsC.ATLen = 0;
        
        if(GprsC.ATStatus == 'R')  //;接收正确
        {
            GprsC.ATStatus = idle;
        }
    }
}


/*检查电话号码是否存在*/
u8 SMS_NumCheck(u8 *NumAddr)
{
    if ((*(NumAddr + 0) == 0) && (*(NumAddr + 1) == 0) &&
        (*(NumAddr + 2) == 0) && (*(NumAddr + 3) == 0) &&
        (*(NumAddr + 4) == 0) && (*(NumAddr + 5) == 0) &&
        (*(NumAddr + 6) == 0) && (*(NumAddr + 7) == 0))
    {
        return false;
    }
    else
    {
        return true;
    }
}


/*----------------------------------------------------------------------------*
 |  功能    : 配置好短信的中心号码与接收号码
 |  out_buf : 写入发送数据的地址
 |  SMS_Code: 短信中心号码的地址
 |  Tel_Code: 短信用户手机号码
 |  return  : 写入发送数据的长度
 *----------------------------------------------------------------------------*/
u8 SMS_CodeData(char *Out_Buf, u8 *SMS_Code, u8 *Tel_Code)
{
    u8 j = 0;

    *(Out_Buf + j++) = '0';
    *(Out_Buf + j++) = '8';
    *(Out_Buf + j++) = '9';
    *(Out_Buf + j++) = '1';
    *(Out_Buf + j++) = '6';
    *(Out_Buf + j++) = '8';

    *(Out_Buf + j++) = *(SMS_Code + 3) / 0x10 + 0x30;                           //;短信中心号码
    *(Out_Buf + j++) = *(SMS_Code + 2) % 0x10 + 0x30;
    *(Out_Buf + j++) = *(SMS_Code + 4) / 0x10 + 0x30;
    *(Out_Buf + j++) = *(SMS_Code + 3) % 0x10 + 0x30;
    *(Out_Buf + j++) = *(SMS_Code + 5) / 0x10 + 0x30;
    *(Out_Buf + j++) = *(SMS_Code + 4) % 0x10 + 0x30;
    *(Out_Buf + j++) = *(SMS_Code + 6) / 0x10 + 0x30;
    *(Out_Buf + j++) = *(SMS_Code + 5) % 0x10 + 0x30;
    *(Out_Buf + j++) = *(SMS_Code + 7) / 0x10 + 0x30;
    *(Out_Buf + j++) = *(SMS_Code + 6) % 0x10 + 0x30;
    *(Out_Buf + j++) = 'F';
    *(Out_Buf + j++) = *(SMS_Code + 7) % 0x10 + 0x30;
    *(Out_Buf + j++) = '0';
    *(Out_Buf + j++) = '1';
    *(Out_Buf + j++) = '0';
    *(Out_Buf + j++) = '0';

    *(Out_Buf + j++) = '0';                                                     //;对方号码长度转十六进制
    *(Out_Buf + j++) = 'B';
    *(Out_Buf + j++) = '8';
    *(Out_Buf + j++) = '1';

    *(Out_Buf + j++) = *(Tel_Code + 3) / 0x10 + 0x30;                           //;写入短信接收者的号码
    *(Out_Buf + j++) = *(Tel_Code + 2) % 0x10 + 0x30;
    *(Out_Buf + j++) = *(Tel_Code + 4) / 0x10 + 0x30;
    *(Out_Buf + j++) = *(Tel_Code + 3) % 0x10 + 0x30;
    *(Out_Buf + j++) = *(Tel_Code + 5) / 0x10 + 0x30;
    *(Out_Buf + j++) = *(Tel_Code + 4) % 0x10 + 0x30;
    *(Out_Buf + j++) = *(Tel_Code + 6) / 0x10 + 0x30;
    *(Out_Buf + j++) = *(Tel_Code + 5) % 0x10 + 0x30;
    *(Out_Buf + j++) = *(Tel_Code + 7) / 0x10 + 0x30;
    *(Out_Buf + j++) = *(Tel_Code + 6) % 0x10 + 0x30;
    *(Out_Buf + j++) = 'F';
    *(Out_Buf + j++) = *(Tel_Code + 7) % 0x10 + 0x30;
    *(Out_Buf + j++) = '0';
    *(Out_Buf + j++) = '0';
    *(Out_Buf + j++) = '0';
    *(Out_Buf + j++) = '8';

    return j;
}


/*----------------------------------------------------------------------------*
 |  Hex编码转换成Ascii编码
 *----------------------------------------------------------------------------*/
char SMS_HexToASC(u8 HEX)
{
    if (HEX <= 9)
    {
        HEX += 0x30;
    }
    else if ((HEX >= 0x0A) && (HEX <= 0x0F))
    {
        HEX += 0x37;
    }
    else
    {
        HEX = 0xFF;
    }
    return HEX;
}


/*----------------------------------------------------------------------------*
 |  拆分Unicode编码为4个Ascii编码
 *----------------------------------------------------------------------------*/
u8 SMS_DataChange(char *ASC, u16 Unicode)
{
    char ASCII[4];

    ASCII[3] = SMS_HexToASC((u8)(Unicode & 0x000F));
    ASCII[2] = SMS_HexToASC((u8)((Unicode & 0x00F0) >> 4));
    ASCII[1] = SMS_HexToASC((u8)((Unicode & 0x0F00) >> 8));
    ASCII[0] = SMS_HexToASC((u8)((Unicode & 0xF000) >> 12));

    if ((ASCII[0] != 0x30) || (ASCII[1] != 0x30) || (ASCII[2] != 0x30) || (ASCII[3] != 0x30))
    {
        *(ASC + 0) = ASCII[0];
        *(ASC + 1) = ASCII[1];
        *(ASC + 2) = ASCII[2];
        *(ASC + 3) = ASCII[3];
        return 4;
    }

    return 0;
}


/*----------------------------------------------------------------------------*
 |  功能    ：取出汉字的编码并转换成Ascii字符写入发送地址
 |  Len     ：汉字个数，
 |  InAddr  ：汉字编码存放的地址
 |  OutAddr ：汉字发送的地址
 |  return  ：实际写入OutAddr地址的Ascii编码的个数
 *----------------------------------------------------------------------------*/
u8 SMS_DataMove(u8 Len, char *OutAddr, u16 *InAddr)
{
    u8 i, j = 0, L;

    for (i = 0; i < Len; i++)
    {
        L = SMS_DataChange(OutAddr + j, *(InAddr + i));
        if (L == 0)
        {
            break;
        }
        else
        {
            j += L;
        }
    }
    return j;
}

/*******************************************************************************
 * 函数名称:   Link2Timing
 * 功能    :   备用主站登陆后开始计时，如果10分种内无抄表数据，主动断开
 * 输入    :
 * 输出    :
 * 返回    :
 *******************************************************************************/
void Modem_LinkTime(void)
{
    u8 type;
    
    type = ZDP.F8.WorkMode & 0x03;
    
    if((type <= 0x01) || ((type == 0x03) && GprsLC.LinkEnable))
    {
        GprsC.NDataMin0++;
        if(GprsC.NDataMin0 > 180)  //;三个小时无TCP数据通信
        {
            GprsC.NLogonMin = 0;
            GprsC.NDataMin0 = 0;
            Modem_PowOFF(); //;无条件重启模块
        }
        if(!(GprsC.State & _HadLogin))
        {
            GprsC.NLogonMin++;
            if(GprsC.NLogonMin > 180)
            {
                GprsC.NDataMin0 = 0;
                GprsC.NLogonMin = 0;
                Modem_PowOFF(); //;无条件重启模块
            }
        }
    }
    else if(type == 0x02)
    {   
        if(GprsLC.PassLinkOrd) 
        {
            if(!(GprsC.State & _HadLogin))//;
            {
                GprsC.NLogonMin++;
                if(GprsC.NLogonMin > 5)
                {
                    GprsC.NDataMin0 = 0;
                    GprsC.NLogonMin = 0;
                    Modem_PowOFF(); //;无条件重启模块
                }
            }
            else  //;处于登录状态
            {
                GprsC.NDataMin0++;
                if(GprsC.NDataMin0 > ZDP.F8.NoComBreakMin)  //;三个小时无TCP数据通信
                {
                    GprsC.NLogonMin = 0;
                    GprsC.NDataMin0 = 0;
                    GprsLC.PassLinkOrd = false; //;不再重拨
                    Modem_OFF(); //;关闭模块模块
                }
            }
        }
    }
    
    
    if(GprsC.State1 & _HadLogin)
    {
        GprsC.NDataMin1++;
        if(GprsC.NDataMin1 >= ZDP.F8.NoComBreakMin)//;在于10分钟自动断开链路2       
        {
            if(GprsC.ATStatus  == idle)
            {
                GprsC.NDataMin1 = 0;
                GprsC.LinkState = LINK1;
                GprsC.ATStatus = 'E';
                GprsC.SendAT_Wait1s = 2;
                GprsC.State1 = 0x00;  //;清除状态
                GprsC.SendAT_Step = 15; //;查看链路2
            }
            else
            {
                GprsC.NDataMin1 = ZDP.F8.NoComBreakMin; //;等待LinkState空闲
            }
        }
    }
    else if(GprsC.State1 & _Link) 
    {
        GprsC.NDataMin1++;
        if(GprsC.NDataMin1 >= 5) //;5分钟连接不成功，清空状态字
        {
            GprsC.NDataMin1 = 0x00;
            GprsLC.Link2Switch = false; //;不再允许备用连接
            GprsC.State1 = 0x00; 
            GprsC.LinkState = LINK0;
        }
    }
}

#ifdef ZS
/* 
********************************************************************************
* 函 数 名：RecordErr
* 功    能：重启时调用
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void RecordErr(u8 sign)
{
    u8 *addr1, *addr2;
    u8 len;
    u8 i;
    
    GprsC.RailSign = sign;
    len = sizeof(struct ModemContrl);
    addr1 = (u8*)&GprsC;
    addr2 = (u8*)&GprsCErr;
    for(i = 0; i < len; i++)
    {
        *addr2 = *addr1;
        addr1++;
        addr2++;
    }
}
#endif


/* 
********************************************************************************
* 函 数 名：Modem_GDCDLightCtr
* 功    能：没有登录且不是重拨状态，网络灯1秒闪烁一次
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
void Modem_GDCDLightCtr(void)
{
    static u8 N250ms;   

    if(!(GprsC.State & _HadLogin))
    {
        if(GprsC.Sign > 10)
        {
            N250ms++;        
            if(N250ms == 2)
            {
                Modem_GDCDON();
            }
            else if(N250ms >=4)
            {
                N250ms = 0;
                Modem_GDCDOFF();
            }
        }
        else
        {
            Modem_GDCDOFF();
        }
    }
}


