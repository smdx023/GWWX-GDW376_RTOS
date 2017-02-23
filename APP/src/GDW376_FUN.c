/*
********************************************************************************
*  文 件 名：GDW376_FUN.c   
*
*  文件描述：GDW376相关功能函数 
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
#include "GDW376_MMU.h"


/*------------------------------------------------------------------------------
||  F1~F10初始化参数__CS1
------------------------------------------------------------------------------*/
B1 InP0F1[ 6]  = {50,2,20,0x30,0,0x05};
/*B1 InP0F3[28]  = {112,91 ,151,  2, 0x63, 0x1B,
                 192,168,1  ,  6, 0xe1, 0x10,
                 'C','M','N','E',  'T', 0   ,
                 0  ,0  ,0  ,  0, 0   , 0   ,0, 0, 0, 0}; */
                 
B1 InP0F3[28]  = {112,91 ,151,  2, 0xB1, 0x1F,
                 192,168,1  ,  6, 0xe1, 0x10,
                 'C','M','N','E',  'T', 0   ,
                 0  ,0  ,0  ,  0, 0   , 0   ,0, 0, 0, 0};                 
//B1 InP0F8[8 ]  = {0x13,0x84,0x03,0x03,0x0A,0xFF,0xFF,0xFF};       by zrt 20130822 
B1 InP0F8[8 ]  = {0x11,0x78,0x00,0x03,0x0A,0xFF,0xFF,0xFF}; 
/*B1 InP0F9[16]  = {0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF,
               0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF, 0x0FF};
*/
B1 InP0F9[16]  = {0x85, 0x32, 0x99, 0x17, 0x00, 0x00, 0x00, 0x00,
               0x85, 0x32, 0x99, 0x17, 0x00, 0x00, 0x00, 0x00};

/*B1 InP0F10[272]= {0x01, 0x00,
                  1,0, 1,0, 1, 30, 0,0,0,0,0,0, 0,0,0,0,0,0, 4, 0x09, 0,0,0,0,0,0, 0x11,
                  2,0, 2,0, 1, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 4, 0x09, 0,0,0,0,0,0, 0x11,
                  3,0, 3,0, 1, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 4, 0x09, 0,0,0,0,0,0, 0x11,
                  4,0, 4,0, 1, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 4, 0x09, 0,0,0,0,0,0, 0x11,
                  5,0, 5,0, 1, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 4, 0x09, 0,0,0,0,0,0, 0x11,
                  6,0, 6,0, 1, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 4, 0x09, 0,0,0,0,0,0, 0x11,
                  7,0, 7,0, 1, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 4, 0x09, 0,0,0,0,0,0, 0x11,
                  8,0, 8,0, 1, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 4, 0x09, 0,0,0,0,0,0, 0x11,
                  9,0, 9,0, 1, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 4, 0x09, 0,0,0,0,0,0, 0x11,
                 10,0,10,0, 1, 1, 0,0,0,0,0,0, 0,0,0,0,0,0, 4, 0x09, 0,0,0,0,0,0, 0x11};
*/
B1 InP0F10[29]= {0x01, 0x00,
                  1,0, 1,0, 1, 30, 0,0,0,0,0,0, 0,0,0,0,0,0, 4, 0x09, 0,0,0,0,0,0, 0x11}; //;--20141226

/*------------------------------------------------------------------------------
||  终端参数F33~F39，F57~F61的初始化参数
------------------------------------------------------------------------------*/
/*
B1 InP0F33[61]={2, 
                0x01, 0x00,0x00, 0x00,0x00,0x00,0x08, 0x00,0x00, 0x03, 0x00,0x00,0x01,
                0x02,
                0x00,0x07,0x00,0x12, 0x00,0x14,0x00,0x18, 0x00,0x00,0x00,0x00, 
                0x00,0x00,0x00,0x00,
                0x01, 0x00,0x00, 0x00,0x00,0x00,0x08, 0x00,0x00, 0x03, 0x00,0x00,0x01,
                0x02,
                0x00,0x07,0x00,0x12, 0x00,0x14,0x00,0x18, 0x00,0x00,0x00,0x00, 
                0x00,0x00,0x00,0x00};
*/
B1 InP0F33[61]={2, 
                0x01, 0x00,0x00, 0x01,0x00,0x00,0x00, 0x00,0x00, 0x03, 0x00,0x00,0x01,
                0x00,
                0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 
                0x00,0x00,0x00,0x00,
                0x01, 0x00,0x00, 0x01,0x00,0x00,0x00, 0x00,0x00, 0x03, 0x00,0x00,0x01,
                0x00,
                0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 
                0x00,0x00,0x00,0x00};
B1 InP0F57[3]={0,0,0};
B1 InP0F59[4]={0,0,0,0};

/*------------------------------------------------------------------------------
||  测量点参数F25~F31，F73~F76的初始化参数
------------------------------------------------------------------------------*/
/*
B1 InP1F25[ 11]={0x64,0,  0x0A,0, 0x10,0x00, 0x40, 0,0,0x60, 1};


B1 InP1F26[57]={0x00,0x12, 0x00,0x08, 0x00,0x08,
                0x00,0x13, 0x02, 0x00,0x08,
                0x00,0x06, 0x02, 0x00,0x08,
                0x00,0x00,0x10, 0x02, 0x00,0x08,
                0x00,0x00,0x08, 0x02, 0x00,0x08,
                0x00,0x00,0x02, 0x02, 0x00,0x08,
                0x00,0x00,0x80, 0x02, 0x00,0x08,
                0x00,0x00,0x70, 0x02, 0x00,0x08,
                0x00,0x05, 0x02, 0x00,0x08,
                0x00,0x05, 0x02, 0x00,0x08,
                0x04};
*/

B1 InP1F25[ 11]={0x01 , 0 , 0x01 , 0 , 0x00 , 0x22 , 0x15 , 0x00 , 0x99 , 0x00 , 0x02 };

B1 InP1F26[57]={
0x20,  0x24,  0x80 , 0x19 , 0x15 , 0x17 , 
0x40 , 0x26 , 0x05 , 0x00 , 0x09 , 
0x60 , 0x17 , 0x05 , 0x00 , 0x09 , 
0x50 , 0x22 , 0x00 , 0x05 , 0x00 , 0x09, 
0x00 , 0x15 , 0x00 , 0x05 , 0x00 , 0x09 , 
0x75 , 0x03 , 0x00 , 0x05 , 0x00 , 0x09 , 
0x70 , 0x28 , 0x01 , 0x05 , 0x00 , 0x09 , 
0x00 , 0x99 , 0x00 , 0x05 , 0x00 , 0x09 , 
0x00 , 0x02 , 0x05 , 0x00 , 0x09 , 
0x00 , 0x02 , 0x05 , 0x00 , 0x09 , 
0x05 };

B1 InP1F28[ 4]={0x00,0x08,0x00,0x09};



/*扩展组12初始化@1*/
B1 InP0F89[ 4]={0x01,0x11,0x01,0x00};
B1 InP0F90[ 3]={0x05,0x05,01};

B1 InP1F97[16]={
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};


//;终端状态参数
B1 ZDSF3[31] = {0xFF, 0xFF, 0x7F, 0x7F, 0xFF, 0xFF, 0x01, 0x1F, 0x0F, 0x0F,
                0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00
               };

B1 ZDSF4[1] = {0x05};   

/*短信头尾内容*/
B2 User_Top[10] = {0x5C0A,0x656C,0x7684,0x7528,0x6237,0xFF1A}; //;尊敬的用户：
B2 User_End[10] = {0x8BF7,0x60A8,0x53CA,0x65F6,0x5904,0x7406}; //;请您及时查看与处理
B2 Admi_Top[10] = {0x5C0A,0x656C,0x7684,0x7BA1,0x7406,0x5458,0xFF1A}; //;尊敬的管理员：
B2 Admi_End[10] = {0x8BF7,0x60A8,0x53CA,0x65F6,0x5904,0x7406}; //;请您及时查看与处理

/*测量点x，x = 1 ~ 10*/
B2 Point_Name[10][10] = {\
{0x6D4B,0x91CF,0x70B9,0x0031}, //;测量点1 
{0x6D4B,0x91CF,0x70B9,0x0032}, //;测量点2
{0x6D4B,0x91CF,0x70B9,0x0033}, //;测量点3
{0x6D4B,0x91CF,0x70B9,0x0034}, //;测量点4
{0x6D4B,0x91CF,0x70B9,0x0035}, //;测量点5
{0x6D4B,0x91CF,0x70B9,0x0036}, //;测量点6
{0x6D4B,0x91CF,0x70B9,0x0037}, //;测量点7
{0x6D4B,0x91CF,0x70B9,0x0038}, //;测量点8
{0x6D4B,0x91CF,0x70B9,0x0039}, //;测量点9
{0x6D4B,0x91CF,0x70B9,0x0031,0x0030},//;测量点10
};

B1 DaysOfMonth[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};


/* 
********************************************************************************
* 函 数 名：ZD_RAM_MAK
* 功    能：写入Ram数据区的标志，用于检查Ram错误
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZD_RAM_MAK(void)
{
    EW.Start[0]  = 'S';     
    EW.End[0] = 'E';
    
    AllBuf.Start[0] = 'S';  
    AllBuf.End[0] = 'E';
    
    AllBuf.St_Spi[0] = 'S'; 
    AllBuf.En_Spi[0] = 'E';
    
    PMD1.Start[0] = 'S';    
    PMD1.End[0] = 'E';
    
//;    PMD2.Start[0] = 'S';    
//;    PMD2.End[0] = 'E';
    
    P1D.Start[0] = 'S';     
    P1D.End[0] = 'E';
    
//;    P2D.Start[0] = 'S';     
//;    P2D.End[0] = 'E';
//;    
//;    P3D.Start[0] = 'S';     
//;    P3D.End[0] = 'E';
//;    
//;    P4D.Start[0] = 'S';     
//;    P4D.End[0] = 'E';
//;    
//;    P5D.Start[0] = 'S';     
//;    P5D.End[0] = 'E';
//;    
//;    P6D.Start[0] = 'S';     
//;    P6D.End[0] = 'E';
//;    
//;    P7D.Start[0] = 'S';     
//;    P7D.End[0] = 'E';
//;    
//;    P8D.Start[0] = 'S';     
//;    P8D.End[0] = 'E';
//;    
//;    P9D.Start[0] = 'S';     
//;    P9D.End[0] = 'E';
//;    
//;    P10D.Start[0] = 'S';    
//;    P10D.End[0] = 'E';
}


/* 
********************************************************************************
* 函 数 名：ZD_RAM_CHK
* 功    能：若RAM标志错误，则复位
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZD_RAM_CHK(void)
{
    u8 Error = false;
    
    if((EW.Start[0] != 'S') || (EW.End[0] != 'E')) 
    {
        Error = true;
    }
    if((P1D.Start[0]!= 'S') || (P1D.End[0]!= 'E'))  
    {
        Error = true;
    }
//;    if ((P2D.Start[0]!= 'S') || (P2D.End[0]!= 'E')) 
//;    {
//;        Error = true;
//;    }
//;    if ((P3D.Start[0]!= 'S') || (P3D.End[0]!= 'E'))  
//;    {
//;        Error = true;
//;    }
//;    if ((P4D.Start[0]!= 'S') || (P4D.End[0]!= 'E')) 
//;    {
//;        Error = true;
//;    }
//;    if ((P5D.Start[0]!= 'S') || (P5D.End[0]!= 'E')) 
//;    {
//;        Error = true;
//;    }
//;    if ((P6D.Start[0]!= 'S') || (P6D.End[0]!= 'E')) 
//;    {
//;        Error = true;
//;    }
//;    if ((P7D.Start[0]!= 'S') || (P7D.End[0]!= 'E'))  
//;    {
//;        Error = true;
//;    }
//;    if ((P8D.Start[0]!= 'S') || (P8D.End[0]!= 'E')) 
//;    {
//;        Error = true;
//;    }
//;    if ((P9D.Start[0]!= 'S') || (P9D.End[0]!= 'E'))  
//;    {
//;        Error = true;
//;    }
//;    if ((P10D.Start[0]!= 'S')|| (P10D.End[0]!='E')) 
//;    {
//;        Error = true;
//;    }
    if((AllBuf.Start[0]!='S') || (AllBuf.End[0] != 'E')) 
    { 
        Error = true;
    }
    if((AllBuf.St_Spi[0] != 'S') || (AllBuf.En_Spi[0] != 'E')) 
    {
        Error = true;
    }
    
    if(Error == true)
    {
        HWareC.EnRes[0] = 'E';
        HWareC.EnRes[1] = 'R';
    }
}


/* 
********************************************************************************
* 函 数 名：ZD_HardWareReset
* 功    能：硬件复位
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZD_HardWareReset(void)
{
     HWareC.EnRes[0] ='E';  //;硬件初始化复位
     HWareC.EnRes[1] ='R';
}


/* 
********************************************************************************
* 函 数 名：ZD_HWareRes_CHK
* 功    能：检测终端是否要硬件复位
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*
void ZD_HWareRes_CHK(void) 
{
    ZD_RAM_CHK();
    if((HWareC.EnRes[0] == 'E') || (HWareC.EnRes[1] == 'R')) //;收到复位命令
    {   
        HWareC.EnRes[0] = 0; 
        HWareC.EnRes[1] = 0;
        while(1)
        {
             ; //;等待看门狗复位
        }
    }
}
*/
void ZD_HWareRes_CHK(void) 
{
	ZD_RAM_CHK();
	//;远程发送复位命令，终端返回应答帧 ---20141224
	/*if((!GprsGyC.Status) &&(HWareC.EnRes[0] == 'E') && (HWareC.EnRes[1] == 'R')) //;收到复位命令
	{   
		HWareC.EnRes[0] = 0; 
		HWareC.EnRes[1] = 0;
		while(1)
		{
			; //;等待看门狗复位
		}
	}
	*/
	//;20141225 发送复位帧，回应答帧再硬件初始化
	if((HWareC.EnRes[0] == 'E') && (HWareC.EnRes[1] == 'R')\
		  &&(!GprsGyC.Status)&&(!GPRSUSARTCTR.SedOrd)\
    	&&(!LocGyC.Status)&&(!InfGyC.Status)\
    	&&(!INFUSARTCTR.SedOrd)&&(!LOCUSARTCTR.SedOrd)\
    	) //;收到复位命令
    {   
        HWareC.EnRes[0] = 0; 
        HWareC.EnRes[1] = 0;
        while(1)
        {
             ; //;等待看门狗复位
        }
    }
	if(HWareC.FactoryInit == 'E')  //;恢复出厂设置
	{
		HWareC.FactoryInit = 'S';
		GprsC.ReConnect = true;
	}
	else if (HWareC.FactoryInit == 'S')
	{
		if((!GprsGyC.Status)&&(!GprsC.ReConnect)&&(EW.CSB1 == 0)\
			&&(!GPRSUSARTCTR.SedOrd)&&(EW.CSB2 == 0)\
			&& (EW.CSB3 == 0)&& (EW.CSB4 == 0) && (EW.CSB5 == 0)\
			&& (EW.CSB6 == 0)&& (EW.CSB7 == 0) && (EW.CSB8 == 0)) //;发送完应答报文和保存完参数
		{
			HWareC.FactoryInit = 0;//;
			while(1)
			{
				; //;等待看门狗复位 
			}
		}
	}
	
	if((GprsC.Re_Connect_states == 'E')\
		   &&(!GprsGyC.Status)&&(!GPRSUSARTCTR.SedOrd)\
		  &&(!LocGyC.Status)&&(!InfGyC.Status)\
    	&&(!INFUSARTCTR.SedOrd)&&(!LOCUSARTCTR.SedOrd))
	{
		GprsC.Re_Connect_states = 0;
		ERCUp.Status = 0;         //;关闭事件上送
		GprsC.ReConnect = true;   //;等待GPRS或者其他通信发送完毕再进行重新连接GPRS
	}
}


/* 
********************************************************************************
* 函 数 名：CalcCRC
* 功    能：CRC循环冗余码认证
* 参    数：InData-待认证的数据的地址；InPW-附加的信息域地址
* 返    回：true-认证成功或不需要认证；false-认证失败
* 说    明：算法对从帧格式控制域C开始，至应用层第一个数据单元标识DT2 结束，
*           共12个字节进行认证。
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
u8 CalcCRC(char *InData, char *InPW)
{
    u8 i, j, data, Plan;
    u16 CRC16, PW, CRC1 = 0;

    Plan = ZDP.F5.Code;  //;认证方案
    if(Plan == 0)
    {
        return true;     //;不认证
    }
    else if(Plan == 1)   //;方案1
    {
        CRC16 = ZDP.F5.Para[1];  //;密钥
        CRC16 <<= 8;
        CRC16 += ZDP.F5.Para[0];
        PW = *(InPW + 1);
        PW <<= 8;
        PW += *(InPW + 0);
        for(i = 0; i < 12; i++)
        {
            data = *(InData + i);

            for(j = 0; j < 8; j++)
            {
                if((data ^ CRC1) & 0x0001)
                {
                    CRC1 = ( CRC1 >> 1 ) ^ CRC16;
                }
                else
                {
                    CRC1 >>= 1;
                }
                data >>= 1;
            }
        }
        if(CRC1 == PW)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}


/* 
********************************************************************************
* 函 数 名：ZD_Pn_Find
* 功    能：查找终端测量点号是否存在
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：2011-09-08 09:33:10
********************************************************************************
*/
s16 ZD_Pn_Find(u16 Pn)
{
    u16 i;
    u16 temp;

    if(Pn == 0) 
    {
        return 0;
    }
    
    for(i=0; i<PNMAXNUM; i++)
    {
        temp = *((u16*)ZDP.F10.Meter[i].Point); //;F10电能表序号i的参数地址
        if(temp == Pn)
        {
            return i;
        }
    }
	return -1;   //;测量点抄其他也有数据，修改此指令只支持对应测量点数据20141225
//    return 0xFFFF;
}


/* 
********************************************************************************
* 函 数 名：初始化终端状态
* 功    能：ZD_Stat_Init
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：2011-09-16 16:14:41 增加GPRS参数初始化
*           2011-09-20 17:29:54  增加ZDS参数初始化
********************************************************************************
*/
void ZD_Stat_Init(void)
{   
    GprsLC.CHKLink = true; //;若虚拟专网工作方式为时段在线，则上电需检测在线。
    
    EW.MoveByte = 'M';
    MovNByte(6, ZDS.F3, (u8*)ZDSF3);
    MovNByte(1, ZDS.F4, (u8*)ZDSF4);
    
    ByteN_Fill(50, &ZDS.F5.InSign, 0x00);
    ByteN_Fill(64, &ZDS.F6.TelCutStat, 0x00);
    ByteN_Fill(8, ZDS.F8, 0x00);
    ByteN_Fill(2, ZDS.F9, 0x00);
    ByteN_Fill(37, &ZDS.F11.PortN, 0x00);
    
    EW.MoveByte = 0;   
    FluxLimValue = *((u32*)ZDP.F36);
    if(0x00 == FluxLimValue)
    {
        FluxLimValue = 0xFFFFFFFF;  //;流量不限制，则给最大值
    }
   
    if((SofewareOldVer[0] == 0) && (SofewareOldVer[1] == 0) && 
       (SofewareOldVer[2] == 0) && (SofewareOldVer[3] == 0))
    {
        SofewareOldVer[0] = ZDCFG_F1[12];
        SofewareOldVer[1] = ZDCFG_F1[13];
        SofewareOldVer[2] = ZDCFG_F1[14];
        SofewareOldVer[3] = ZDCFG_F1[15];
        SaveData_FM24(4, Version, Version + E2R_ByteNum, (u8*)ZDCFG_F1 + 12);
    }

    else
    {
        if((SofewareOldVer[0] != ZDCFG_F1[12]) || 
           (SofewareOldVer[1] != ZDCFG_F1[13]) || 
           (SofewareOldVer[2] != ZDCFG_F1[14]) || 
           (SofewareOldVer[3] != ZDCFG_F1[15]))
        {
            SaveData_FM24(4, Version, Version + E2R_ByteNum, (u8*)ZDCFG_F1 + 12);
            ERC1_Judge(2); //;软件版本号变更
        }
    }
}


/* 
********************************************************************************
* 函 数 名：Pn_DataInit
* 功    能：初始化测量点数据区
* 参    数：Pn-测量点
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void Pn_DataInit(u8 Pn)
{
    u16 E2R_PnDDJ, E2R_PnBDJ, E2R_PnMDJ, E2R_PnQDJ;
    u32 *Addr32;
    struct DJ_Control *PnDDJ,*PnBDJ, *PnMDJ;
    struct QX_Control *PnQDJ;
    struct PointDataStr* PnD;
    
    Addr32 = (u32*)(*(PnDTAddr + Pn));
    PnD = (struct PointDataStr*)(*(Addr32 + _PnD));
    
    ByteN_Fill(PNDSIZE, PnD->CB_Time, 0x00);  //;清除测量点数据
    
    PnDDJ = (struct DJ_Control*)(*(Addr32 + _PnDDJ));
    PnBDJ = (struct DJ_Control*)(*(Addr32 + _PnBDJ));
    PnMDJ = (struct DJ_Control*)(*(Addr32 + _PnMDJ));
    PnQDJ = (struct QX_Control*)(*(Addr32 + _PnQDJ));
    E2R_PnDDJ = (u16)(*(Addr32 + _E2R_PnDDJ));
    E2R_PnBDJ = (u16)(*(Addr32 + _E2R_PnBDJ));
    E2R_PnMDJ = (u16)(*(Addr32 + _E2R_PnMDJ));
    E2R_PnQDJ = (u16)(*(Addr32 + _E2R_PnQDJ));
    PnDDJ->Index = 0;  PnDDJ->StInd = 0;   //;日冻结数据结初始化
    PnBDJ->Index = 0;  PnBDJ->StInd = 0;   //;抄表日冻结数据结初始化
    PnMDJ->Index = 0;  PnMDJ->StInd = 0;   //;月冻结数据结初始化
    PnQDJ->Index = 0;  PnQDJ->StInd = 0;   //;曲线冻结数据结初始化
    SaveData_FM24(1, E2R_PnDDJ, E2R_PnDDJ + E2R_ByteNum, &PnDDJ->Index);
    SaveData_FM24(2, E2R_PnBDJ, E2R_PnBDJ + E2R_ByteNum, &PnBDJ->Index);
    SaveData_FM24(2, E2R_PnMDJ, E2R_PnMDJ + E2R_ByteNum, &PnMDJ->Index);
    SaveData_FM24(4, E2R_PnQDJ, E2R_PnQDJ + E2R_ByteNum, (u8*)(&PnQDJ->Index));
}


/* 
********************************************************************************
* 函 数 名：ZD_DatArea_Init
* 功    能：初始化终端数据区
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：2011-09-20 18:37:15
********************************************************************************
*/
void ZD_DataInit(void)
{
    u8 i;

    ERC1.Index = 0;  //;重要事件数据结初始化
    ERC1.StInd = 0;  
    ERC2.Index = 0;  //;一般事件数据结初始化
    ERC2.StInd = 0;  
    ZDS.F7[0] = 0;   
    ZDS.F7[1] = ERCUp.UpIndex = 0;
    ZDS.F10.Flux_Day = 0x00000000;
    ZDS.F10.Flux_Month = 0x00000000;
    SaveData_FM24(4, E2R_ERC1, E2R_ERC1 + E2R_ByteNum, (u8*)(&ERC1.Index));
    SaveData_FM24(4, E2R_ERC2, E2R_ERC2 + E2R_ByteNum, (u8*)(&ERC2.Index));
    SaveData_FM24(1, E2R_EC1, E2R_EC1 + E2R_ByteNum, ZDS.F7); //;保存事件计数器
    SaveData_FM24(1, E2R_EC2, E2R_EC2 + E2R_ByteNum, ZDS.F7+1); //;保存事件计数器
    SaveData_FM24(1, E2R_ERCUP, E2R_ERCUP + E2R_ByteNum, (u8*)&ERCUp.UpIndex); //;保存事件计数器
    
    for(i = 0; i < PNMAXNUM; i++)
    {
        Pn_DataInit(i);
    }
}


/* 
********************************************************************************
* 函 数 名：ZD_CS1_Init
* 功    能：终端F1~F10参数初始化
* 参    数：type == ture--全部初始化，type==false--除通信参数以外全部初始化
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZD_CS1_Init(u8 type)
{
    EW.MoveByte = 'M';
    MovNByte(6, &ZDP.F1.RTS, (u8*)InP0F1);
    
    ByteN_Fill(33, &ZDP.F2.En_Num, 0x00);
    
    if(type == true)
    {
        MovNByte(28, ZDP.F3.IP1, (u8*)InP0F3);
    }
    
    ByteN_Fill(16, ZDP.F4.Tel_Code, 0x00);
    ByteN_Fill(3,  &ZDP.F5.Code   , 0x00);
    ByteN_Fill(16, ZDP.F6.Addr1   , 0x00);
    ByteN_Fill(64, ZDP.F7.ZDIP     , 0x00);
    
    MovNByte(8   , &ZDP.F8.WorkMode , (u8*)InP0F8);
    MovNByte(16  , ZDP.F9.Enable    , (u8*)InP0F9);
    MovN2Byte(29, ZDP.F10.MeterNum, (u8*)InP0F10); //;--20141226
    
    EW.MoveByte = 0;
    EW.CSB1 = 0xC1;
}

/* 
********************************************************************************
* 函 数 名：ZD_CS2_Init
* 功    能：参数2区所保存的参数F11~F23的初始化
* 参    数：无
* 返    回：无 
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZD_CS2_Init(void)
{
    EW.MoveByte = 'M';
    ByteN_Fill(CS2NUM, &ZDP.F11.PuNum, 0x00);
    EW.MoveByte = 0;
    EW.CSB2 = 0xC2;
}

/* 
********************************************************************************
* 函 数 名：ZD_CS3_Init
* 功    能：终端参数F33~F39，F57~F61的初始化
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZD_CS3_Init(void)
{
    EW.MoveByte = 'M';
    MovNByte(61, &ZDP.F33.PortN, (u8*)InP0F33);
    MovNByte(3, ZDP.F57, (u8*)InP0F57);
    MovNByte(4, &ZDP.F59.EnergyOver, (u8*)InP0F59);
    
    ByteN_Fill(13 , &ZDP.F34.PortN, 0x00);
    ByteN_Fill(11 , &ZDP.F35.UsrN , 0x00);
    ByteN_Fill(4  , ZDP.F36       , 0x00);
    ByteN_Fill(19 , &ZDP.F37.Port, 0x00);
    ByteN_Fill(138, &ZDP.F38.UsrBIndex, 0x00);
    ByteN_Fill(138, &ZDP.F39.UsrBIndex, 0x00);
    
    ByteN_Fill(1, ZDP.F58, 0x00);
    ByteN_Fill(44, ZDP.F60.XBU_High, 0x00);
    ByteN_Fill(1, ZDP.F61, 0x00);
    
    EW.MoveByte = 0;
    EW.CSB3 = 0xC3;
}

/* 
********************************************************************************
* 函 数 名：ZD_CS4_Init
* 功    能：测量点参数F25~F31、F73~F76参数初始化
* 参    数：Pn-测量点号
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZD_CS4_Init(u16 Pn)
{
    
    u32 *Addr32;
    struct PoPStr  *PnP;

    Addr32 = (u32*)(*(PnDTAddr + Pn));
    PnP = (struct PoPStr*)(*(Addr32 + _PnP));
    EW.MoveByte = 'M';
    MovNByte(11, (u8*)(&PnP->F25.UMul), (u8*)InP1F25);
    MovNByte(57, PnP->F26.UHigh, (u8*)InP1F26);
    MovNByte(4,  PnP->F28.Cos1, (u8*)InP1F28);
    
    ByteN_Fill(24 , PnP->F27.RA, 0x00);
    ByteN_Fill(12 , PnP->F29, 0x00);
    ByteN_Fill(1 ,  PnP->F30, 0x00);
    ByteN_Fill(25 , &PnP->F31.NodeN, 0x00);
    ByteN_Fill(48 , &PnP->F73[0].Sign, 0x00);
    ByteN_Fill(10 , PnP->F74.PowerFactor, 0x00);
    ByteN_Fill(16 , PnP->F75.UOut, 0x00);
    ByteN_Fill(1 ,  PnP->F76, 0x00);
    EW.MoveByte = 0;
//    EW.CSB4 |= (0x0001 << (Pn -1)); //;记录需要保存的测量点参数区
    EW.CSB4 |= (0x0001 << Pn );   //;原来在Pn等于0时没有保存参数
}

/* 
********************************************************************************
* 函 数 名：ZD_CS5_Init
* 功    能：任务参数初始化为0
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZD_CS5_Init(void)
{
    EW.MoveByte = 'M';
    ByteN_Fill(CS5NUM , &TaskP.F65[0].Cyc, 0x00);
    EW.CSB5 = 0xC5;
}

/* 
********************************************************************************
* 函 数 名：ZD_CS6_Init
* 功    能：任务参数及开关初始化
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZD_CS6_Init(void)
{
    EW.MoveByte = 'M';
    ByteN_Fill(CS6NUM - 16, &TaskP.F66[0].Cyc, 0x00);//;初始化任务，1,2类数据各8个任务
    ByteN_Fill(8, TaskP.F67, 0xAA); 
    ByteN_Fill(8, TaskP.F68, 0xAA);  //;初始化没设置好---20141226
    EW.CSB6 = 0xC6;
}

/* 
********************************************************************************
* 函 数 名：ZD_CS7_Init
* 功    能：总加组、模拟量、控制量参数初始化
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZD_CS7_Init(void)
{
    EW.MoveByte = 'M';
    ByteN_Fill(CS7NUM_GRP, &GRPP.F41.Flag, 0x00);
    ByteN_Fill(CS7NUM_DCP, DCPP.F81.ValueS, 0x00);
    ByteN_Fill(CS7NUM_CTR, F49, 0x00);
    EW.CSB7 = 0xC7;
}

/* 
********************************************************************************
* 函 数 名：ZD_CS8_Init
* 功    能：扩展参数初始化
* 参    数：type == ture--全部初始化，type==false--除通信参数以外全部初始化
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZD_CS8_Init(u8 type)
{
    EW.MoveByte = 'M';
    if(type == true)
    {
        MovNByte(4,  ZDPKZ.F89.QuXian,(u8*)InP0F89);
    }
    MovNByte(16, ZDPKZ.F97.UserSend,(u8*)InP1F97);
    MovNByte(20, ZDPKZ.F98,(u8*)User_Top);
    MovNByte(20, ZDPKZ.F99,(u8*)User_End);
    MovNByte(20, ZDPKZ.F100,(u8*)Admi_Top);
    MovNByte(20, ZDPKZ.F101,(u8*)Admi_End);
    EW.MoveByte = 0; 
    EW.CSB8 = 0xC8;
}

/* 
********************************************************************************
* 函 数 名：ZD_CS9_Init
* 功    能：任务参数初始化为0
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZD_CS9_Init(void)
{
    EW.MoveByte = 'M';
    ByteN_Fill(CS9NUM , &TaskP.F65[4].Cyc, 0x00);
    EW.CSB9 = 0xC9;
}

/* 
********************************************************************************
* 函 数 名：ZD_CS10_Init
* 功    能：任务参数初始化为0
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZD_CS10_Init(void)
{
    EW.MoveByte = 'M';
    ByteN_Fill(CS10NUM , &TaskP.F66[4].Cyc, 0x00);
    EW.CSB10 = 0xCA;
}

/* 
********************************************************************************
* 函 数 名：ZD_CS_DatArea_Init
* 功    能：参数及全体数据初始化
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZD_CSDataInit(void)
{
    u8 i;

    ERC1_Judge(1);
    ZD_CS1_Init(true); //;初始化参数区1
    ZD_CS2_Init(); //;初始化参数区2
    ZD_CS3_Init(); //;初始化参数区4
    ZD_CS5_Init(); //;4个任务1类数据初始化参数区5
    ZD_CS6_Init(); //;4个任务2类数据初始化参数区6
    ZD_CS7_Init(); //;初始化参数区5
    ZD_CS8_Init(true); //;初始化参数区6
    ZD_CS9_Init(); //;新增4个任务1类数据初始化参数区9
    ZD_CS10_Init(); //;初始化参数区10
    ZD_DataInit();//;初始化终端数据区
    
    for(i=0; i<PNMAXNUM; i++)
    {
        ZD_CS4_Init(i);  //;初始化测量点参数区3
    }

    for(i = 0; i < TASK1NUM; i++)
    {
        Task_Init(i+1, 65);//;初始化1类任务
    }
    for(i = 0; i < TASK2NUM; i++)
    {
        Task_Init(i+1, 66);//;初始化2类任务
    }
}

/* 
********************************************************************************
* 函 数 名：ZD_CS_DatArea_Init
* 功    能：参数及全体数据初始化(通信参数除外)
* 参    数：无
* 返    回：无 
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZD_NCCSDataInit(void)
{
    u8 i;

    ERC1_Judge(1);
    ZD_CS1_Init(false); //;初始化参数区1
    ZD_CS2_Init(); //;初始化参数区2
    ZD_CS3_Init(); //;初始化参数区4
    ZD_CS5_Init(); //;4个任务初始化参数区5
    ZD_CS6_Init(); //;初始化参数区6
    ZD_CS7_Init(); //;初始化参数区5
    ZD_CS8_Init(false); //;初始化参数区6
    ZD_CS9_Init(); //;初始化参数区9
    ZD_CS10_Init(); //;初始化参数区10
    ZD_DataInit();//;初始化终端数据区
    
    for(i=0; i<PNMAXNUM; i++)
    {
        ZD_CS4_Init(i);  //;初始化测量点参数区3
    }

    for(i = 0; i < TASK1NUM; i++)
    {
        Task_Init(i+1, 65);//;初始化1类任务
    }
    for(i = 0; i < TASK2NUM; i++)
    {
        Task_Init(i+1, 66);//;初始化2类任务
    }
}


/* 
********************************************************************************
* 函 数 名：ZD_Time_Set
* 功    能：终端设置时间
* 参    数：TimeAddr-新时间地址
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZD_Time_Set(u8 *TimeAddr)
{
    u8 W1 = 0, W2 = 0;
    u8 preyear, premonth, preday, prehour;
    TBCD.Sec = *(TimeAddr + 0);
    TBCD.Min = *(TimeAddr + 1);
    TBCD.Hour = *(TimeAddr + 2);
    TBCD.Day = *(TimeAddr + 3);
    TBCD.Month = *(TimeAddr + 4) & 0x1F;
    TBCD.Year = *(TimeAddr + 5);
    W1 = TBCD.Month >> 5;
    W2 = WeekConut(TimeAddr + 3); //;算出星期   
    if (W1 = W2)
    {
        Week = W1;
    }
    else
    {
        Week = W2;
    }
    
    preyear = THEX.Year;
    premonth = THEX.Month;
    preday   = THEX.Day;
    prehour  = THEX.Hour;
    
    THEX.Year = BCD1ToU8(&TBCD.Year);
    THEX.Month = BCD1ToU8(&TBCD.Month);
    THEX.Day = BCD1ToU8(&TBCD.Day);
    THEX.Hour = BCD1ToU8(&TBCD.Hour);
    THEX.Min = BCD1ToU8(&TBCD.Min);
    THEX.Sec = BCD1ToU8(&TBCD.Sec);
    
    if(preyear != THEX.Year)
    {
        TimeChgFlag = 1;
    }
    else if(premonth != THEX.Month)
    {
        TimeChgFlag = 2;
    }
    else if(preday  != THEX.Day)
    {
        TimeChgFlag = 3;
    }
    else if(prehour != THEX.Hour)
    {
        TimeChgFlag = 4;
    }
    //EPSON8025_SAVE(0xE0, 0x20); //;24小时制
    //EPSON8025_SAVE(0x60, TBCD.Year); //;年
    //EPSON8025_SAVE(0x50, TBCD.Month); //;月
    //EPSON8025_SAVE(0x40, TBCD.Day); //;日
    //EPSON8025_SAVE(0x30, Week); //;星期
    //EPSON8025_SAVE(0x20, TBCD.Hour); //;小时
    //EPSON8025_SAVE(0x10, TBCD.Min); //;分钟
    //EPSON8025_SAVE(0x00, TBCD.Sec); //;秒  
}


///*----------------------------------------------------------------------------*
// |   终端时间读取
// *----------------------------------------------------------------------------*/
//void ZD_Time_Read(void)
//{
//    u8 Time[7];
//
//    EPSON8025_READ(Time);    
//    if (TIME_CHK(Time) == true) //;读出的时间正确
//    {
//        ZDD.DayTime[0] = TBCD.Sec = Time[0];
//        ZDD.DayTime[1] = TBCD.Min = Time[1];
//        ZDD.DayTime[2] = TBCD.Hour = Time[2];
//        ZDD.DayTime[3] = TBCD.Day = Time[4];
//        ZDD.DayTime[4] = TBCD.Month = Time[5];
//        ZDD.DayTime[5] = TBCD.Year = Time[6];
//        Week = Time[3];
//        ZDD.DayTime[4] |= (Week << 5);
//        THEX.Year = BCD1ToU8(&TBCD.Year);
//        THEX.Month = BCD1ToU8(&TBCD.Month);
//        THEX.Day = BCD1ToU8(&TBCD.Day);
//        THEX.Hour = BCD1ToU8(&TBCD.Hour);
//        THEX.Min = BCD1ToU8(&TBCD.Min);
//        THEX.Sec = BCD1ToU8(&TBCD.Sec);
//        if (Old_Sec != TBCD.Sec)
//        {
//            Old_Sec = TBCD.Sec;
//            RTC_FlagSec = true; //;每秒执行一次
//        }
//        if (Old_Min != TBCD.Min) 
//        {
//            Old_Min = TBCD.Min;
//            RTC_FlagMin = true;  //;每分钟执行一次
//        }
//    }
//    else
//    {
//        u8 In8025Time[6] = { 0x00, 0x00, 0x00, 0x01, 0x01, 0x00};  //;时钟芯片初始化时间,秒.分.时.日.月.年
//        
//        ZD_Time_Set((u8*)In8025Time);  //;初始化时间
//    }
//}


/* 
********************************************************************************
* 函 数 名：ClockTimer
* 功    能：无时钟芯片时调用此函数实现时钟功能
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：2011-10-13 16:49:46 修改当Num025s_WT>8的时候秒跳转快的问题
********************************************************************************
*/
void ClockTimer(void)
{   
    static u8 Old_Sec, Old_Min;
    u8 Nsec;
	  	  
	  if(Num025s_WT >= 4)
	  { 
	      Nsec = Num025s_WT / 4;
	      Num025s_WT -= Nsec * 4;
	  	  if(THEX.Sec >= 59)  //;分翻转
	  	  {
	  	  	  THEX.Sec = 0;
	  	  	  if(THEX.Min >= 59)  //;小时翻转
	  	  	  {
	  	  	  	  THEX.Min = 0;
	  	  	  	  if(THEX.Hour >= 23)  //;天翻转
	  	  	  	  {
	  	  	  	  	  THEX.Hour = 0;	  	  	 	  
	  	  	  	  	  if(THEX.Day >= DaysOfMonth[THEX.Month])  //;判断是否发生月翻转
	  	  	  	  	  {
	  	  	  	  	  	  if(THEX.Month == 2)  //;二月份的特殊情况
	  	  	  	  	  	  {
	  	  	  	  	  	  	  if((THEX.Year % 4) == 0x00)  //;闰年 --@2
	  	  	  	  	  	  	  {
	  	  	  	  	  	  	  	  if(THEX.Day >= 29 )
	  	  	  	  	  	  	  	  {
	  	  	  	  	  	  	  	  	  THEX.Day = 1;
	  	  	  	  	  	  	  	  }
	  	  	  	  	  	  	  	  else   
	  	  	  	  	  	  	  	  {
	  	  	  	  	  	  	  	  	  THEX.Day++;
	  	  	  	  	  	  	  	  }
	  	  	  	  	  	  	  }
	  	  	  	  	  	  	  else
	  	  	  	  	  	  	  {
	  	  	  	  	  	  	  	  THEX.Day = 1;
	  	  	  	  	  	  	  }
	  	  	  	  	  	  }
	  	  	  	  	  	  else
	  	  	  	  	  	  {
	  	  	  	  	  	  	  THEX.Day = 1;
	  	  	  	  	  	  }
	  	  	  	  	  	  
	  	  	  	  	  	  if(THEX.Day == 1) //;发生月翻转
	  	  	  	  	  	  {	  	  	  	  	  	      	  	  	  	  	  	      
	  	  	  	  	  	      if(THEX.Month >= 12) //;发生年翻转
	  	  	  	  	  	      {	  	  	  	  	  	          
	  	  	  	  	  	          THEX.Month = 1;
	  	  	  	  	  	          THEX.Year++;
	  	  	  	  	  	      }
	  	  	  	  	  	      else
	  	  	  	  	  	      {
	  	  	  	  	  	          THEX.Month++;
	  	  	  	  	  	      }
	  	  	  	  	  	  }
	  	  	  	  	  }
	  	  	  	  	  else
	  	  	  	  	  {
	  	  	  	  	  	  THEX.Day++;
	  	  	  	  	  }
	  	  	  	  }
	  	  	  	  else
	  	  	  	  {
	  	  	  	  	  THEX.Hour++;
	  	  	  	  }
	  	  	  }
	  	  	  else
	  	  	  {
	  	  	      THEX.Min++;
	  	  	  }
	  	  }
	  	  else
	  	  {
	  	  	  THEX.Sec += Nsec;
	  	  }
	  }
	  
	  ZDS.F2[0] = TBCD.Sec   = (THEX.Sec/10)*16  + THEX.Sec%10;
	  ZDS.F2[1] = TBCD.Min   = (THEX.Min/10)*16  + THEX.Min%10;
	  ZDS.F2[2] = TBCD.Hour  = (THEX.Hour/10)*16 + THEX.Hour%10;
	  ZDS.F2[3] = TBCD.Day   = (THEX.Day/10)*16  + THEX.Day%10;
	  ZDS.F2[4] = TBCD.Month = (THEX.Month/10)*16+ THEX.Month%10;
	  ZDS.F2[5] = TBCD.Year  = (THEX.Year/10)*16 + THEX.Year%10;
    ZDS.F2[4] |= WeekConut(&TBCD.Day)<<5;    //;by zrt 20130620 增加终端日历时钟显示星期	  
	  if(Old_Sec != TBCD.Sec)
    {
        Old_Sec = TBCD.Sec;
        RTC_FlagSec = true; //;每秒执行一次
    }
    if(Old_Min != TBCD.Min) 
    {
        Old_Min = TBCD.Min;
        RTC_FlagMin = true;  //;每分钟执行一次
    }
}

/* 
********************************************************************************
* 函 数 名：CheckTime
* 功    能：对时及处理函数
* 参    数：无
* 返    回：无
* 说    明：用于系统时间与抄回基表时钟的时间对比
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void CheckTime(void)
{
    s32 long1;
    
    if((TBCD.Year==PMD1.RC010[3]) && (TBCD.Month==PMD1.RC010[2]) 
       && (TBCD.Day==PMD1.RC010[1]))
    {       
//        long1=Time_distance(&TBCD.Sec, (u8*)PMD1.RC011 + 1);20140116
        long1=Time_distance(&TBCD.Sec, (u8*)PMD1.RC011 );		
        if((long1 > -5) && (long1 < 5))//;5秒钟以内不校时
        {
            return;
        }
    }

    //GprsLC.CHKLink = true; //;重新检测上线。
    GprsLC.CHKLink = false; //;重新检测上线。 20150105 (关闭重新检测上线)
    Week = PMD1.RC010[0];
    ZDS.F2[3]=TBCD.Day = PMD1.RC010[1];
    ZDS.F2[4]=TBCD.Month = PMD1.RC010[2];
    ZDS.F2[5]=TBCD.Year = PMD1.RC010[3];
    ZDS.F2[0]=TBCD.Sec = PMD1.RC011[0];
    ZDS.F2[1]=TBCD.Min = PMD1.RC011[1];
    ZDS.F2[2]=TBCD.Hour =PMD1.RC011[2];
    THEX.Year=(TBCD.Year&0x0f)+(TBCD.Year>>4)*10;
    THEX.Month=(TBCD.Month&0x0f)+(TBCD.Month>>4)*10;
    THEX.Day=(TBCD.Day&0x0f)+(TBCD.Day>>4)*10;
    THEX.Hour=(TBCD.Hour&0x0f)+(TBCD.Hour>>4)*10;
    THEX.Min=(TBCD.Min&0x0f)+(TBCD.Min>>4)*10;
    THEX.Sec=(TBCD.Sec&0x0f)+(TBCD.Sec>>4)*10;
    if(Old_Sec != TBCD.Sec)
    {
        Old_Sec = TBCD.Sec;
        RTC_FlagSec = true; //;每秒执行一次
    }
    if(Old_Min != TBCD.Min) 
    {
        Old_Min = TBCD.Min;
        RTC_FlagMin = true;  //;每分钟执行一次
    }        
}

/* 
********************************************************************************
* 函 数 名：ZD_Relay
* 功    能：数据转发处理
* 参    数：无
* 返    回：无
* 说    明：每250ms执行一次
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZD_Relay(void)
{
    static u8 Sts1, Mark1;
#ifdef PORT2USART     
    static u8 Sts2, Mark2;
#endif    
    
    if(RelayC.Status == 'E') //;将中继数据从终端端口发出
    {                
        if((RelayC.Port == 1) && ((Port1.Status == 0) || (Port1.Status == 'F')))
        {
            Sts1 = Port1.Status;
            Mark1 = Port1.Mark;
            Port1.Status = 'E';  //;端口开始发送允许
            Port1.Mark = 'R';    //;进入中继发送标志
            /*暂不支持修改波特率*/
//;            if(Port1.BPS != RelayC.BPS)
//;            {
//;                Port1.BPS = RelayC.BPS;
//;                UARTx_BaudRate_Set(RelayC.BPS, 3);
//;            }
            Port1.Buffer = PORT1SEDBUF;  //;指向端口1的缓存区
            RelayC.Status = 'S'; //;中继数据发送完成
            Port1.Len = RelayC.Len;  //;写入要发送的长度
            EW.MoveByte = 'M';
            MovNByte(RelayC.Len, (u8*)Port1.Buffer, (u8*)RelayC.Data); //;将数据写入端口1的缓存区
            EW.MoveByte = 0;
        }
#ifdef PORT2USART         
        if((RelayC.Port == 2) && (Port2.Status == 0)) 
        {
            Sts2 = Port2.Status;
            Mark2 = Port2.Mark;
            Port2.Status = 'E';  //;端口开始发送允许
            Port2.Mark = 'R';    //;进入中继发送标志
            Port2.Buffer = PORT2SEDBUF; //;将数据写入端口2的缓存区
            Port2.Len = RelayC.Len;  //要发送的长度
            RelayC.Status = 'S'; //;中继数据发送完成
            EW.MoveByte = 'M';
            MovNByte(RelayC.Len, (u8*)Port2.Buffer, (u8*)RelayC.Data); //;将数据写入端口2的缓存区
            EW.MoveByte = 0;
        }
#endif        
    }   

    if(RelayC.Status == 'S') //;中继数据发送完成，正在等待中继返回
    {
        if((RelayC.Port == 1) && (Port1.Status =='R') && (Port1.Mark == 'R')) //;端口1接收到中继数据
        {
            Port1.Status = Sts1; //;释放端口1
            Port1.Mark = Mark1;
            EW.MoveByte = 'M';
            MovNByte(Port1.Len, (u8*)RelayC.Data, (u8*)Port1.Buffer); //;端口1接收的数据写入中继数据区
            EW.MoveByte = 0;
            RelayC.Len = Port1.Len; //;中继应答数据的长度
            RelayC.Status = right; //;中继接收正确
        }
#ifdef PORT2USART         
        if((RelayC.Port == 2) && (Port2.Status =='R') && (Port2.Mark == 'R')) //;端口2接收到中继数据
        {
            Port2.Status = Sts2; //;释放端口2
            Port2.Mark = Mark2;
            EW.MoveByte = 'M';
            MovNByte(Port2.Len, (u8*)RelayC.Data, (u8*)Port2.Buffer); //;端口1接收的数据写入中继数据区
            EW.MoveByte = 0;
            RelayC.Len = Port2.Len; //;中继应答数据的长度
            RelayC.Status = right; //;中继接收正确
        }
#endif        
    }
    
    if(RelayC.Status != 0) //;正在处理中继数据中
    {
        if(RelayC.OvTime > 0)
        {
            RelayC.OvTime--;
            if(RelayC.OvTime == 0) 
            {
                RelayC.Status = error; //;中继超时
                if((RelayC.Port == 1) && (Port1.Status != 0) && (Port1.Mark == 'R')) //;端口1被中继占用
                {
                    Port1.Status = Sts1; //;释放端口1
                    Port1.Mark = Mark1;
                }
#ifdef PORT2USART                 
                if((RelayC.Port == 2) && (Port2.Status != 0) && (Port2.Mark == 'R')) //;端口2被中继占用
                {
                    Port2.Status = Sts2; //;释放端口2
                    Port2.Mark = Mark2;
                }
#endif                 
            }
        }
    }
}


/* 
********************************************************************************
* 函 数 名：ZD_POW_OFF
* 功    能：关闭电源
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZD_POW_OFF(void)
{
    if(Modem_OFF_CHK() == false) //;当前模块启动
    {
        Modem_GDCDOFF();
        Modem_OFF(); //;关闭模块       
    }
    while(1)
    {
        if(RTC_FlagMs)     //;250ms执行一次
        {
            ;
        }
    }
}


/* 
********************************************************************************
* 函 数 名：ZD_POW_CHK
* 功    能：掉电处理程序
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZD_POW_CHK(void)
{
    u8 PowLostTime = 0;
//;    u8 saveflag = true;
//;    u8 upflag = true;
    
    if(Power_Modem == _48V)
    {        
			      USART_Cmd(USART1, DISABLE); 
			      USART_Cmd(USART3, DISABLE);           
//;            ERC14_Judge(1);   //;终端停电      
            ERCSave.Pn = 0x00; //;保存告警数据
            if(GprsC.State & _HadLogin)
            {
                GprsC.LoginOut = 'E';
            }
            while (1)
            { 
                USART_Com();        //;串口通讯
                Modem_Com();        //;Gprs模块通讯
                AFNComDeal();       //;GDW130规约本地与远程通讯
                if (RTC_FlagMs)     //;250ms执行一次
                {
                    RTC_FlagMs = 0;
                    ClockTimer();
//;                    if(saveflag) //;告警保存事件处理
//;                    {
//;                        if(ERC_Deal_Save())
//;                        {
//;                            saveflag = false;
//;                        }
//;                    }                    
//;                    else
//;                    {
//;                        if(!(GprsC.State & _HadLogin))
//;                        {
//;                            ZD_POW_OFF();
//;                        }
//;                        else if(upflag)
//;                        {                           
//;                            if(ZD_ERC())       //;处理事件
//;                            {
//;                                upflag = false;
//;                                if(GprsC.LoginOut == 0)
//;                                {
//;                                    GprsC.LoginOut = 'E';
//;                                }
//;                            }
//;                        }
//;                    }
                    
                    if(GprsC.LoginOut == 'R')
                    {
                        ZD_POW_OFF();
                    }
                    
                    if (RTC_FlagSec)//;每秒执行一次
                    {
                        RTC_FlagSec = 0;
                        PowLostTime ++;
                        Watchdog();       //;喂看门狗
                        ATComdSetNoRec();     //;向GPRS模块发送AT 命令无回应处理  
                        while(PowLostTime >=30)
                        {
                            ZD_POW_OFF();
                        }
                    }
                }
            }
    }
}


 /* 
********************************************************************************
* 函 数 名：Master_HB
* 功    能：主站处理心跳
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：2011-06-21 13:51:35  修改心跳报文无应答重播为1分钟
********************************************************************************
*/
void Master_HB(void)
{
    if((ZDP.F8.WorkMode & 0x03) == 0x02) //;不发心跳
    {
        return;
    }
    
    if (GprsC.State & _HadLogin)
    {
        if (GprsC.HBStatus == idle)
        {
            if (GprsC.HBTime == 0)
            {
                if (ZDP.F1.HBeat != 0)
                {
                    GprsC.HBTime = ZDP.F1.HBeat;
                    //GprsC.HBTime *= 60;
                    GprsC.HBTime *= 45;
                }
            }
            else if (GprsC.HBTime > 0)
            {
                GprsC.HBTime--;                                                                                         //;发送心跳倒计时
                if (GprsC.HBTime == 0)
                {
                    GprsC.HBStatus = 'E';
                }
            }
        }

        if (GprsC.HBStatus == 'S') 
        {
//;            u16 Time;

            GprsC.HBOVT++; 
//;            Time = ZDP.F1.HBeat;
//;            Time *= 60;
//;            Time /= 2;
            if (GprsC.HBOVT >= 60) //;心跳超时
            {
//;                GprsC.HBStatus = 'E';
//;               GprsC.HBStatus = 0; //;--@2
//;               GprsC.HBTime = 1;  //;
                GprsC.HBOVT = 0;
                GprsC.HBNumSend++;
                if(GprsC.HBNumSend < 3)
                {
                     GprsC.HBStatus = 'E';
                }
                else
                {
                    if (GprsC.ATStatus == idle)
                    {
                        GprsC.HBNumSend = 0x00;
                        GprsC.HBStatus = 0;
                        GprsC.ATStatus = 'E';                //;可能是假连接，主动断开连接
                        GprsC.SendAT_Wait1s = 5;
                        GprsC.SendAT_Step = 17;
                    }
                    else
                    {
                        GprsC.HBOVT = 60;                 //;等待ATStatus空闲
                    }
                }
            }
        }
        else if(GprsC.HBStatus == 'E') //;超时未发送
        {
            GprsC.HBOVT++;
            if(GprsC.HBOVT >= 120)
            {
                GprsC.HBStatus = idle;
                GprsC.HBOVT = 0;
            }
        }
        if (GprsC.HBStatus == 'R')
        {
            GprsC.HBStatus = idle;
            GprsC.HBOVT = 0;
        }
    }
    else
    {
        GprsC.HBStatus = idle;
        GprsC.HBTime = 0;
        GprsC.HBOVT = 0;
    }
}


/* 
********************************************************************************
* 函 数 名：Master_Login
* 功    能：主站登陆控制,3次登陆不成功，主动断开连接
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void Master_Login(void)
{
    if(GprsC.LinkState == LINK0)
    {
        if ((GprsC.State & _Connect) && (!(GprsC.State & _HadLogin))) //;还未登陆
        {
            if (GprsC.LoginIn == idle)
            {
                GprsC.LoginIn = 'E';
            }
        
            if (GprsC.LoginIn == 'S')                                                                                       //;登陆报文无回应计时
            {
                GprsC.LoginTimeOut++;
                if (GprsC.LoginTimeOut > 30)                                                              //;在30秒的时间内没有接收到主站对登录报文的应答
                {
                    GprsC.LoginTimeOut = 0;
                    GprsC.LoginNumSend++;
                    if (GprsC.LoginNumSend < 3)        //;最多重发登录报文3次
                    {
                        GprsC.LoginIn = idle;          //;需要再次发送登录报文
                    }
                    //;else if (GprsC.LoginNumSend >= 3)
			              else
			              {
                        if (GprsC.ATStatus == idle)
                        {
                            GprsC.ATStatus = 'E';       //;可能登错主站或通讯不正常，主动断开连接
                            GprsC.SendAT_Wait1s = 5;
                            GprsC.SendAT_Step = 17;
                        }
                        else
                        {
                            GprsC.LoginNumSend = 3;     //;等待ATStatus空闲
                        }
                    }
                }
            }
        
            if(GprsC.LoginIn == 'R') //;收到主站对登录报文的正确应答
            {
                GprsC.LoginIn = idle;
                GprsC.LoginTimeOut = 0;
                GprsC.LoginNumSend = 0;
                GprsC.NLogonMin = 0x00;
                GprsC.State |= _HadLogin;              //;当前处于登陆状态
                GprsC.Status |= _ContHost;              //;表示当前已经寻找到主站
                GprsC.Status &= ~_FindHost;            //;表示寻找主站结束
                Modem_GDCDON();                         //;打开网络灯
            }
        }    
        else
        {
            GprsC.LoginIn = idle;
            GprsC.LoginTimeOut = 0;
            GprsC.LoginNumSend = 0;
        }
    }
    else
    {
        if ((GprsC.State1 & _Connect) && (!(GprsC.State1 & _HadLogin))) //;还未登陆
        {
            if (GprsC.LoginIn == idle)
            {
                GprsC.LoginIn = 'E';
            }
        
            if (GprsC.LoginIn == 'S')                                                                                       //;登陆报文无回应计时
            {
                GprsC.LoginTimeOut++;
                if (GprsC.LoginTimeOut > 30)                                                              //;在30秒的时间内没有接收到主站对登录报文的应答
                {
                    GprsC.LoginTimeOut = 0;
                    GprsC.LoginNumSend++;
                     if (GprsC.LoginNumSend < 3)        //;最多重发登录报文3次
                    {
                        GprsC.LoginIn = idle;          //;需要再次发送登录报文
                    }
                   // else if (GprsC.LoginNumSend == 3)
			else
			{
//;                        if (GprsC.ATStatus == idle)
//;                        {
//;                            GprsC.ATStatus = 'E';       //;可能登错主站或通讯不正常，主动断开连接
//;                            GprsC.SendAT_Wait1s = 5;
//;                            GprsC.SendAT_Step = 17;
//;                        }
//;                        else
//;                        {
//;                            GprsC.LoginNumSend = 3;     //;等待ATStatus空闲
//;                        }
                    }
                }
            }
        
            if (GprsC.LoginIn == 'R') //;收到主站对登录报文的正确应答
            {
                GprsC.LoginIn = idle;
                GprsC.LoginTimeOut = 0;
                GprsC.LoginNumSend = 0;
                GprsC.State1 |= _HadLogin;  //;当前处于登陆状态
                GprsC.LinkState = LINK0;
            }
        }
        else
        {
            GprsC.LoginIn = idle;
            GprsC.LoginTimeOut = 0;
            GprsC.LoginNumSend= 0;
        }
    }
}

/* 
********************************************************************************
* 函 数 名：LinkStat_CHK
* 功    能：终端连接主站检测。
* 参    数：无
* 返    回：无 
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void LinkStat_CHK(void)
{
    u8 bit, byte, type;
    
    type = ZDP.F8.WorkMode & 0x03;
    
    if(type <= 0x01)
    {
        GprsLC.LinkEnable = true;
    }
    else if(type == 0x02) //;被动激活模式
    {
        if(GprsLC.PassLinkOrd)
        {
            GprsLC.LinkEnable = true;
        }
        else
        {
            GprsLC.LinkEnable = false;
        }
    }
    else if(type == 0x03) //;时段在线模式
    {    
        if((THEX.Min==0x00) || GprsLC.CHKLink)
        {
            GprsLC.CHKLink = false;
/*        20130609 zrt
            bit = THEX.Hour & 0x07;  //;找出当前小时所在时区表第几位
            byte = THEX.Hour & 0xF1; //;找出当前小时所在时区表第几字节
*/        
            bit = THEX.Hour & 0x07;  //;找出当前小时所在时区表第几位
            byte = THEX.Hour /8;      //;找出当前小时所在时区表第几字节
            if(ZDP.F8.AllowTimeF[byte] & (0x01<<bit))
            {
                GprsLC.LinkEnable = true;
            }
            else
            {
                GprsLC.LinkEnable = false;
            }
        
        }
        
    }
    
    if(GprsLC.Link2Switch) //;备用连接
    {
        GprsLC.LinkEnable = true;
    }
}


/* 
********************************************************************************
* 函 数 名：Master_Link
* 功    能：控制主站和备用主站的连接和检测
* 参    数：无
* 返    回：无
* 说    明：ZDP.F62.Type:==1 永久在线模式;==2被动激活模式 ; Interval 重拨间隔
* 抛出异常：
* 作    者：023
* 修改记录：2011-06-09 16:22:59
*           2012-01-31 14:39:35 增加重拨状态的信号检测
********************************************************************************
*/
void Master_Link(void)
{
    u8 type;
    
    type = ZDP.F8.WorkMode & 0x03;
    
    if(GprsC.State & _SetUp)
    {
        if(!(GprsC.State & _Connect))
        {
            if((type==1) || (type==3) || ((type==2)&&(GprsLC.PassLinkOrd))) //;永久在线模式
            {
                if(!(GprsC.State & _Redial))
                {
                    if(!(GprsC.State & _Link))     //;还未发起连接
                    {
                        if(ZDS.F10.Flux_Month < FluxLimValue)
                        {
                            if(GprsC.ATStatus == idle)
                            {
                                GprsC.ATStatus = 'E';      //;清除状态重拨
                                GprsC.ATSOVT = 0;
                                GprsC.SendAT_Wait1s = 5;
                                GprsC.SendAT_Step = 1;      //;开始发AT命令建立TCP连接
                                GprsC.IP = ZDP.F3.IP1;
                                GprsC.LinkState = LINK0;
                                GprsC.Port = *((u16*)ZDP.F3.Port1);
                                GprsC.APN = ZDP.F3.APN;
                                GprsC.State |= _Link;
                                GprsC.Status = 0; 
                            }
                        }
                    }
                }
                else
                {
                    if(type == 2) //;被动激活
                    {
                        if(GprsC.MinReCon == 0x00)
                        {
                            GprsLC.PassLinkNum--;
                            if(GprsLC.PassLinkNum >= 0)
                            {
                                GprsC.MinReCon = 10;
                            }
                            else  //;达到最大重拨次数
                            {
                                GprsLC.PassLinkOrd = false; //;不再重拨
                                GprsC.State &= (~_Redial); //;退出重拨状态
                                GprsLC.LinkEnable = false;
                            }
                        }
                        else
                        {
                            GprsC.MinReCon--;
                            if(GprsC.MinReCon == 0x00)
                            {
                                GprsC.State &= (~_Redial); //;退出重拨状态                            
                            }
                        }
                    }
                    else
                    {
                        if(GprsC.MinReCon == 0)
                        {
                            if(*((u16*)ZDP.F8.ActRedailSec) != 0)
                            {
                                GprsC.MinReCon = *((u16*)ZDP.F8.ActRedailSec);
                            }
                            else
                            {
                            	  GprsC.MinReCon = 10 ;  //;10秒后重连接20131227
                            }
                        }
                        else if(GprsC.MinReCon > 0)
                        {
                            GprsC.MinReCon--;
                            {
                                if(GprsC.MinReCon == 0)
                                {
                                    #ifdef ZS
                                        RecordErr(7);
                                    #endif
                                    GprsC.State &= (~_Redial); //;退出重拨状态
                                }
                            }
                        }
                    }
                }
            }           
        }       
        
        if(GprsC.ReConnect == true) //;重新连接
        {
            #ifdef ZS
                RecordErr(8);
            #endif
            GprsC.ReConnect = false;
            Modem_GDCDOFF();                 //;关掉网络灯
            GprsC.State &= (~_Connect);      //;主动断开连接成功
            GprsC.State &= (~_HadLogin);
            GprsC.State &= (~_Link);   //;退出连接状态
            GprsC.State &= (~_Redial); //;退出重拨状态
            GprsC.Status = 0x00;       //;清除状态
        }
        
        //;备用主站发起连接 
        if (GprsLC.Link2Switch == true)
        {
            if(!(GprsC.State1 & _Connect))//;备用主站断开            
            {
                if(!(GprsC.State1 & _Link)) 
                {
                    if(GprsC.ATStatus == idle)
                    {
                         GprsC.LinkState = LINK1;
                         GprsC.ATStatus = 'E';  
                         GprsC.State1 |= _Link;
                         if(GprsC.State & _HadLogin)
                         //;主站为连接状态说明可以直接发起连接
                         {
                            
                             GprsC.SendAT_Wait1s = 2;
                             GprsC.SendAT_Step = 14;
                         }
                         else
                         {
                             GprsC.SendAT_Wait1s = 5;
                             GprsC.SendAT_Step = 1;
                         }
                    }
                }
            }
        }
    }
    if(TBCD.Sec == 0x40) //;检测更新信号
    {
        if(GprsC.LinkState == LINK0)
        {
            if((GprsC.State & _Connect) || (GprsC.State & _Redial))
            {
                if(GprsC.ATStatus == idle)
                {
                
                    GprsC.ATStatus = 'E';
                    GprsC.SendAT_Step = 4;
                    GprsC.SendAT_Wait1s = 1;
                }
            }
        }
    }
}


/* 
********************************************************************************
* 函 数 名：ZD_Mast_Link
* 功    能：处理GPRS模块,每秒处理一次
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZD_Mast_Link(void)
{
    Modem_ON_CHK();       //;  控制和检测GPRS模块启动                                                                                                                     //;检测GPRS模块是否成功开机
    Master_Login();       //;  控制终端登陆主站                                                                                                                        //;主站登陆控制
    Master_HB();          //;  控制终端向主站发送心跳                                                       //;处理心跳                                                                                                             //;发起主站连接
    Master_Link();        //;  控制GPRS拨号
    ATComdSetNoRec();     //;  向GPRS模块发送AT 命令无回应处理                                                          //;AT命令计时
}

/* 
********************************************************************************
* 函 数 名：TimeChangeDeal
* 功    能：时间改变需要做的处理
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：2011-09-29 20:14:30
********************************************************************************
*/
void TimeChangeDeal(void)
{
    u8 i;
    u32 *Addr32;
    struct PointDataStr *PnD;
    
    if(TimeChgFlag != 0x00)
    {
        for(i = 0; i < PNMAXNUM; i++)
        {
            Addr32 = (u32*)(*(PnDTAddr + i));
            PnD = (struct PointDataStr*)(*(Addr32 + _PnD));
            
            switch(TimeChgFlag)
            {
                case 4:
                case 3:
                    ByteN_Fill(228, PnD->DF33.ZPMax, 0x00);
                    ZDS.F10.Flux_Month = 0;
                case 2:
                    ByteN_Fill(207, PnD->DF25.ZPMax, 0x00);
                    ZDS.F10.Flux_Day = 0;
                case 1:
                    ByteN_Fill(366, PnD->ODJ_B630.Td_h, 0x00);
                    break;
                default:
                    break;   
            }
        }
        TimeChgFlag = 0x00;
    }
}

