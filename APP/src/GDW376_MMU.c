/*
********************************************************************************
*  文 件 名：GDW376_MMU.c
*
*  文件描述：Flash参数任务的存储和读取及冻结数据的存储和读取
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

extern B1 DaysOfMonth[13];

/* 
********************************************************************************
* 函 数 名：Buf_Sum
* 功    能：求AllBuf.Spi1缓冲区0~526数据和
* 参    数：无
* 返    回：返回1Byte数据和 
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：2011-09-14 11:31:29
********************************************************************************
*/
u8 FlashBufSum(void)
{
    u8  sum = 0x55;
    u16 j;
    
    for(j = 0; j < 527; j++)
    {
        sum += AllBuf.Spi1[j];
    }
    
    return sum;
}


/* 
********************************************************************************
* 函 数 名：ReadFlash_Verify
* 功    能：Page1数据校验不正确，则读出备份区Page2数据。仍错，则返回Page1.
* 参    数：Page1-要读的页；Page2-对应备份页
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ReadFlash_Verify(u16 Page1, u16 Page2)
{ 
    SPI_FLASH_PageRead(AllBuf.Spi1, Page1); //;读出数据区1的数据
    
    if(FlashBufSum()!= AllBuf.Spi1[527])
    {
        SPI_FLASH_PageRead(AllBuf.Spi1, Page2); //;读出数据区2的数据
        
        if(FlashBufSum() != AllBuf.Spi1[527])
        {
            SPI_FLASH_PageRead(AllBuf.Spi1, Page1); //;读出数据区1的数据
        }
    }
}

/* 
********************************************************************************
* 函 数 名：MMU_FM24_CHK
* 功    能：检测FM24是否第一次上电
* 参    数：无
* 返    回：true--FM24已写入数据；flase--FM24未写入数据
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
u8 MMU_FM24_CHK(void)
{
    u8 ch1, FM24BZ[4];
    
    for(ch1=0; ch1<16; ch1++)  //;连续16次未读到标志，则判断flase
    {
        EW.RFM = 'R';
        ReadNByte_I2C1(4, E2R_BiaoZhi, FM24BZ);
        EW.RFM = 0;
        if((FM24BZ[0]==_FM24_BZ1) && (FM24BZ[1]==_FM24_BZ2) && 
           (FM24BZ[2]==_FM24_BZ3) && (FM24BZ[3]==_FM24_BZ4))
        {
            return true;
        }
    }
    
    return false;
}


/* 
********************************************************************************
* 函 数 名：MMU_FM24_Read
* 功    能：上电时刻读出FM24中保存的数据。
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void MMU_FM24_Read(void)
{
    u8  i, FM24BZ[4];
    u16 E2R_PnDDJ, E2R_PnBDJ, E2R_PnMDJ, E2R_PnQDJ;
    u32 *Addr32;
    struct DJ_Control *PnDDJ,  *PnBDJ,  *PnMDJ;
    struct QX_Control *PnQDJ;

    if(MMU_FM24_CHK() == true)
    {
        ReadData_FM24(4, E2R_ERC1, E2R_ERC1 + E2R_ByteNum, (u8*) &ERC1.Index); //;读出重要事件控制参数
        ReadData_FM24(4, E2R_ERC2, E2R_ERC2 + E2R_ByteNum, (u8*) &ERC2.Index); //;读出一般事件控制参数
        ReadData_FM24(1, E2R_EC1,  E2R_EC1 + E2R_ByteNum,  (u8*) ZDS.F7);    //;读出数据计数器EC
        ReadData_FM24(1, E2R_EC2,  E2R_EC2 + E2R_ByteNum,  (u8*) ZDS.F7+1);
        ReadData_FM24(1, E2R_ERCUP,  E2R_ERCUP + E2R_ByteNum,  (u8*)&ERCUp.UpIndex);//;上送序号
        ReadData_FM24(4, Version,  Version + E2R_ByteNum,  SofewareOldVer); //;读出旧的软件版本号

        for(i=0; i<PNMAXNUM; i++)
        {
            Addr32 = (u32*)(*(PnDTAddr + i));
            PnDDJ = (struct DJ_Control*)(*(Addr32 + _PnDDJ));
            PnBDJ = (struct DJ_Control*)(*(Addr32 + _PnBDJ));
            PnMDJ = (struct DJ_Control*)(*(Addr32 + _PnMDJ));
            PnQDJ = (struct QX_Control*)(*(Addr32 + _PnQDJ));
            E2R_PnDDJ = (u16)(*(Addr32 + _E2R_PnDDJ));
            E2R_PnBDJ = (u16)(*(Addr32 + _E2R_PnBDJ));
            E2R_PnMDJ = (u16)(*(Addr32 + _E2R_PnMDJ));
            E2R_PnQDJ = (u16)(*(Addr32 + _E2R_PnQDJ));
            ReadData_FM24(2, E2R_PnDDJ, E2R_PnDDJ + E2R_ByteNum, &PnDDJ->Index); //;读出日冻结控制参数
            ReadData_FM24(2, E2R_PnBDJ, E2R_PnBDJ + E2R_ByteNum, &PnBDJ->Index); //;读出抄表日冻结控制参数
            ReadData_FM24(2, E2R_PnMDJ, E2R_PnMDJ + E2R_ByteNum, &PnMDJ->Index); //;读出月冻结控制参数
            ReadData_FM24(4, E2R_PnQDJ, E2R_PnQDJ + E2R_ByteNum, (u8*) &PnQDJ->Index); //;读出曲线冻结控制参数
        } 
    }
    else
    {
        FM24BZ[0] = _FM24_BZ1;
        FM24BZ[1] = _FM24_BZ2;
        FM24BZ[2] = _FM24_BZ3;
        FM24BZ[3] = _FM24_BZ4;
        EW.WFM = 'W';
        WriteNByte_I2C1(4, E2R_BiaoZhi, FM24BZ);
        EW.WFM = 0;
    }
}


/* 
********************************************************************************
* 函 数 名：CS_PnSave
* 功    能：测量点参数保存
* 参    数：Pn-测量点号
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void CS_PnSave(u8 Pn)
{
    u16 j = 0;
    u32 *Addr32;
    struct PoPStr  *PnP;

    Addr32 = (u32*)(*(PnDTAddr + Pn));
    PnP = (struct PoPStr*)(*(Addr32 + _PnP));
    *(AllBuf.Spi1 + j++) = CSBiaoZhi1;
    *(AllBuf.Spi1 + j++) = CSBiaoZhi2;
    *(AllBuf.Spi1 + j++) = CSBiaoZhi3;
    MovN2Byte(CS4NUM, AllBuf.Spi1 + j, (u8*)(&PnP->F25.UMul));
    j += CS4NUM;

    for(; j < 527; j++)
    {
        *(AllBuf.Spi1 + j) = 0x00;
    }
    AllBuf.Spi1[527] = FlashBufSum();
    if((SW.Para[0]!='P') || (SW.Para[1]!='a'))
    {
        while (1)
        {
            ;
        }
    }
    SPI_FLASH_PageWrite_E(AllBuf.Spi1, P1CSBock4 + Pn);
    SPI_FLASH_PageWrite_E(AllBuf.Spi1, P1CSBock4Bak + Pn);
}


/* 
********************************************************************************
* 函 数 名：CS_Save
* 功    能：终端参数存入FLASH
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void CS_Save(void)
{
    u8  i;
    u16 j;

    EW.MoveByte = 'M';

    //;保存终端参数F1~F10
    if(EW.CSB1 == 0xC1)
    {
        EW.CSB1 = 0x00;
        j = 0;
        *(AllBuf.Spi1 + j++) = CSBiaoZhi1;
        *(AllBuf.Spi1 + j++) = CSBiaoZhi2;
        *(AllBuf.Spi1 + j++) = CSBiaoZhi3;
        
        MovN2Byte(CS1NUM, AllBuf.Spi1 + j, &ZDP.F1.RTS);//;F1~F10
        j += CS1NUM;
       
        for(; j<527; j++)
        {
            *(AllBuf.Spi1 + j) = 0x00;
        }
        
        //;用以校验
        AllBuf.Spi1[527] = FlashBufSum();
        
        if((SW.Para[0]!='P') || (SW.Para[1]!='a'))
        {
            while (1)
            {
               ; //;等待CPU复位
            }
        }
        
        SPI_FLASH_PageWrite_E(AllBuf.Spi1, CSBock1);  
        SPI_FLASH_PageWrite_E(AllBuf.Spi1, CSBock1Bak);
       
    }

    //;保存终端参数F11~F23
    if(EW.CSB2 == 0xC2)
    {
        EW.CSB2 = 0x00;
        j = 0;
        *(AllBuf.Spi1 + j++) = CSBiaoZhi1;
        *(AllBuf.Spi1 + j++) = CSBiaoZhi2;
        *(AllBuf.Spi1 + j++) = CSBiaoZhi3;
        
        MovN2Byte(CS2NUM, AllBuf.Spi1 + j, &ZDP.F11.PuNum);
        j += CS2NUM;
       
        for(; j < 527; j++)
        {
            *(AllBuf.Spi1 + j) = 0x00;
        }
        
        AllBuf.Spi1[527] = FlashBufSum();
        if((SW.Para[0]!='P') || (SW.Para[1]!='a'))
        {
            while (1)
            {
                ;
            }
        }      
        SPI_FLASH_PageWrite_E(AllBuf.Spi1, CSBock2);
        SPI_FLASH_PageWrite_E(AllBuf.Spi1, CSBock2Bak);
    }

    //;保存终端参数F33~F39，F57~F61
    if(EW.CSB3 == 0xC3)
    {
        EW.CSB3 = 0x00;
        j = 0;
        *(AllBuf.Spi1 + j++) = CSBiaoZhi1;
        *(AllBuf.Spi1 + j++) = CSBiaoZhi2;
        *(AllBuf.Spi1 + j++) = CSBiaoZhi3;
        
        MovN2Byte(CS3NUM, AllBuf.Spi1 + j, &ZDP.F33.PortN);
        j += CS3NUM;
             
        for(; j<527; j++)
        {
            *(AllBuf.Spi1 + j) = 0x00;
        }
        
        AllBuf.Spi1[527] = FlashBufSum();
        if((SW.Para[0]!='P') || (SW.Para[1]!='a'))
        {
            while (1)
            {
                ;
            }
        }        
        SPI_FLASH_PageWrite_E(AllBuf.Spi1, CSBock3);
        SPI_FLASH_PageWrite_E(AllBuf.Spi1, CSBock3Bak);       
    }

    //;测量点参数保存
    for(i=0; i<PNMAXNUM; i++)
    {
        if(EW.CSB4 & (0x0001<<i))
        {
            CS_PnSave(i);
            EW.CSB4 &= (~(0x0001 << i));//;记录完成后清除标志位
        }
    }

   
    //;保存F65任务参数
    if(EW.CSB5 == 0xC5)
    {
        EW.CSB5 = 0x00;
        j = 0;
        *(AllBuf.Spi1 + j++) = CSBiaoZhi1;
        *(AllBuf.Spi1 + j++) = CSBiaoZhi2;
        *(AllBuf.Spi1 + j++) = CSBiaoZhi3;
        MovN2Byte(CS5NUM, AllBuf.Spi1 + j, &TaskP.F65[0].Cyc);
        j += CS5NUM;
       
        for(; j<527; j++)
        {
            *(AllBuf.Spi1 + j) = 0x00;
        }
        
        AllBuf.Spi1[527] = FlashBufSum();
        if((SW.Para[0]!='P') || (SW.Para[1]!='a'))
        {
            while (1)
            {
                ;
            }
        }

        SPI_FLASH_PageWrite_E(AllBuf.Spi1, CSBock5);
        SPI_FLASH_PageWrite_E(AllBuf.Spi1, CSBock5Bak);

    }
    
    //;保存F66，F67，F68，F69参数
    if(EW.CSB6 == 0xC6)
    {
        EW.CSB6 = 0x00;
        j = 0;
        *(AllBuf.Spi1 + j++) = CSBiaoZhi1;
        *(AllBuf.Spi1 + j++) = CSBiaoZhi2;
        *(AllBuf.Spi1 + j++) = CSBiaoZhi3;
        
        MovN2Byte(CS6NUM-16, AllBuf.Spi1 + j, &TaskP.F66[0].Cyc);
        j += CS6NUM-16;
        MovN2Byte(16, AllBuf.Spi1 + j, &TaskP.F67[0]);
        j += 16;
       
        for(; j< 27; j++)
        {
            *(AllBuf.Spi1 + j) = 0x00;
        }
        
        AllBuf.Spi1[527] = FlashBufSum();
        if((SW.Para[0]!='P') || (SW.Para[1]!='a'))
        {
            while (1)
            {
                ;
            }
        }

        SPI_FLASH_PageWrite_E(AllBuf.Spi1, CSBock6);
        SPI_FLASH_PageWrite_E(AllBuf.Spi1, CSBock6Bak);

    }
    
    //;保存F41~F48、F81~F83、F49参数
    if(EW.CSB7 == 0xC7)
    {
        EW.CSB7 = 0x00;
        j = 0;
        *(AllBuf.Spi1 + j++) = CSBiaoZhi1;
        *(AllBuf.Spi1 + j++) = CSBiaoZhi2;
        *(AllBuf.Spi1 + j++) = CSBiaoZhi3;
        
        MovN2Byte(CS7NUM_GRP, AllBuf.Spi1 + j, &GRPP.F41.Flag);
        j += CS7NUM_GRP;
        MovNByte(CS7NUM_DCP, AllBuf.Spi1 + j, DCPP.F81.ValueS);
        j += CS7NUM_DCP;
        MovNByte(CS7NUM_CTR, AllBuf.Spi1 + j, F49);
        j += CS7NUM_CTR;
       
        for(; j<527; j++)
        {
            *(AllBuf.Spi1 + j) = 0x00;
        }
        
        AllBuf.Spi1[527] = FlashBufSum();
        if((SW.Para[0]!='P') || (SW.Para[1]!='a'))
        {
            while(1)
            {
                ;
            }
        }

        SPI_FLASH_PageWrite_E(AllBuf.Spi1, CSBock7);
        SPI_FLASH_PageWrite_E(AllBuf.Spi1, CSBock7Bak);

    }
    
    //;保存终端扩展参数
    if(EW.CSB8 == 0xC8)
    {
        EW.CSB8 = 0x00;
        j = 0;
        *(AllBuf.Spi1 + j++) = CSBiaoZhi1;
        *(AllBuf.Spi1 + j++) = CSBiaoZhi2;
        *(AllBuf.Spi1 + j++) = CSBiaoZhi3;
        
        MovNByte(4, AllBuf.Spi1 + j, ZDPKZ.F89.QuXian);
        j += 4;
        MovNByte(96, AllBuf.Spi1 + j, ZDPKZ.F97.UserSend);
        j += 96;
        
        for(; j<527; j++)
        {
            *(AllBuf.Spi1 + j) = 0x00;
        }
        
        AllBuf.Spi1[527] = FlashBufSum();
        if((SW.Para[0]!='P') || (SW.Para[1]!='a'))
        {
            while(1)
            {
                ;
            }
        }
        SPI_FLASH_PageWrite_E(AllBuf.Spi1, CSBock8);
        SPI_FLASH_PageWrite_E(AllBuf.Spi1, CSBock8Bak);
    }
    
    //;保存F65任务参数
    if(EW.CSB9 == 0xC9)
    {
        EW.CSB9 = 0x00;
        j = 0;
        *(AllBuf.Spi1 + j++) = CSBiaoZhi1;
        *(AllBuf.Spi1 + j++) = CSBiaoZhi2;
        *(AllBuf.Spi1 + j++) = CSBiaoZhi3;
        MovN2Byte(CS9NUM, AllBuf.Spi1 + j, &TaskP.F65[4].Cyc);
        j += CS9NUM;
       
        for(; j<527; j++)
        {
            *(AllBuf.Spi1 + j) = 0x00;
        }
        
        AllBuf.Spi1[527] = FlashBufSum();
        if((SW.Para[0]!='P') || (SW.Para[1]!='a'))
        {
            while (1)
            {
                ;
            }
        }

        SPI_FLASH_PageWrite_E(AllBuf.Spi1, CSBock9);
        SPI_FLASH_PageWrite_E(AllBuf.Spi1, CSBock9Bak);

    }
    
     //;保存F65任务参数
    if(EW.CSB10 == 0xCA)
    {
        EW.CSB10 = 0x00;
        j = 0;
        *(AllBuf.Spi1 + j++) = CSBiaoZhi1;
        *(AllBuf.Spi1 + j++) = CSBiaoZhi2;
        *(AllBuf.Spi1 + j++) = CSBiaoZhi3;
        MovN2Byte(CS10NUM, AllBuf.Spi1 + j, &TaskP.F66[4].Cyc);
        j += CS10NUM;
       
        for(; j<527; j++)
        {
            *(AllBuf.Spi1 + j) = 0x00;
        }
        
        AllBuf.Spi1[527] = FlashBufSum();
        if((SW.Para[0]!='P') || (SW.Para[1]!='a'))
        {
            while (1)
            {
                ;
            }
        }

        SPI_FLASH_PageWrite_E(AllBuf.Spi1, CSBock10);
        SPI_FLASH_PageWrite_E(AllBuf.Spi1, CSBock10Bak);

    }
    EW.MoveByte = 0;
}

/* 
********************************************************************************
* 函 数 名：ZD_CSSAVE_CHK
* 功    能：检测是否需要将终端参数写入FLASH
* 参    数：无
* 返    回：无 
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ZD_CSSAVE_CHK(void)
{
    /*if ((EW.CSB1==0xC1) || (EW.CSB2==0xC2) || ((EW.CSB4>0)&&(EW.CSB4<=0x3FF)) || 
        (EW.CSB3==0xC3) || (EW.CSB5==0xC5) || (EW.CSB6==0xC6)||
        (EW.CSB7==0xC7) || (EW.CSB8==0xC8))  //;判断是否需要保存终端的参数
    */
    //;增加CS9和CS10 也就是多增加的1,2类任务中的各4个需要保存数据--20141226
    if ((EW.CSB1==0xC1) || (EW.CSB2==0xC2) || ((EW.CSB4>0)&&(EW.CSB4<=0x3FF)) || 
        (EW.CSB3==0xC3) || (EW.CSB5==0xC5) || (EW.CSB6==0xC6)||
        (EW.CSB7==0xC7) || (EW.CSB8==0xC8)|| (EW.CSB9==0xC9)|| (EW.CSB10==0xCA))
    {   
        SW.Para[0] = 'P'; SW.Para[1] = 'a';
        CS_Save(); 
        SW.Para[0] = 0;   SW.Para[1] = 0;
    }
}


/* 
********************************************************************************
* 函 数 名：CS_Read
* 功    能：把Flash中保存的终端参数读出
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void CS_Read(void)
{
    u8 i;
    u16 j;
    u32 *Addr32;
    struct PoPStr  *PnP;

    EW.MoveByte = 'M';
    SW.Para[0] = 'P';
    SW.Para[1] = 'a'; //;允许对终端参数和测量点参数进行操作

    //;读1区
    ReadFlash_Verify(CSBock1, CSBock1Bak);
    if((*AllBuf.Spi1==CSBiaoZhi1) && (*(AllBuf.Spi1+1)==CSBiaoZhi2) &&
       (*(AllBuf.Spi1+2)==CSBiaoZhi3))
    {
        j = 3;
        MovN2Byte(CS1NUM, &ZDP.F1.RTS, AllBuf.Spi1 + j); //;F1~F10           
    }
    else
    {
        ZD_CS1_Init(true); //;初始化参数区1
    }

    //;读2区
    ReadFlash_Verify(CSBock2, CSBock2Bak);
    if((*AllBuf.Spi1==CSBiaoZhi1) && (*(AllBuf.Spi1 + 1)==CSBiaoZhi2) && 
       (*(AllBuf.Spi1+2) == CSBiaoZhi3))
    {
        j = 3;
        MovN2Byte(CS2NUM, &ZDP.F11.PuNum, AllBuf.Spi1 + j);//;F11~F23
    }
    else
    {
        ZD_CS2_Init(); //;初始化参数区2
    }

    //;读3区
    ReadFlash_Verify(CSBock3, CSBock3Bak);
    if((*AllBuf.Spi1==CSBiaoZhi1) && (*(AllBuf.Spi1+1)==CSBiaoZhi2) &&
       (*(AllBuf.Spi1+2) == CSBiaoZhi3))
    {
        j = 3;
        MovN2Byte(CS3NUM, &ZDP.F33.PortN, AllBuf.Spi1 + j); //;F33~F39,F57~F61
       
    }
    else
    {
        ZD_CS3_Init(); //;初始化参数区3
    }

    //;读4区
    for(i=0; i<PNMAXNUM; i++)
    {
        Addr32 = (u32*)(*(PnDTAddr + i));
        PnP = (struct PoPStr*)(*(Addr32 + _PnP));

        ReadFlash_Verify(P1CSBock4 + i, P1CSBock4Bak + i);

        if((*AllBuf.Spi1==CSBiaoZhi1) && (*(AllBuf.Spi1+1)==CSBiaoZhi2) && 
           (*(AllBuf.Spi1+2)==CSBiaoZhi3))
        {
            j = 3;
            MovN2Byte(CS4NUM, (u8*)(&PnP->F25.UMul), AllBuf.Spi1 + j); //;F25~F31,F73~F76
        }
        else
        {
            ZD_CS4_Init(i); //;初始化参数区4
        }
    }

   
    //;读5区
    ReadFlash_Verify(CSBock5, CSBock5Bak);
    if ((*AllBuf.Spi1==CSBiaoZhi1) && (*(AllBuf.Spi1+1)==CSBiaoZhi2) && 
        (*(AllBuf.Spi1+2)==CSBiaoZhi3))
    {
        j = 3;
        MovN2Byte(CS5NUM, &TaskP.F65[0].Cyc, AllBuf.Spi1 + j);
    }
    else
    {
        ZD_CS5_Init(); //;初始化参数区5
    }

    //;读6区
    ReadFlash_Verify(CSBock6, CSBock6Bak);
    if ((*AllBuf.Spi1==CSBiaoZhi1) && (*(AllBuf.Spi1+1)==CSBiaoZhi2) && 
        (*(AllBuf.Spi1+2)==CSBiaoZhi3))
    {
        j = 3;
        MovN2Byte(CS6NUM-16, &TaskP.F66[0].Cyc, AllBuf.Spi1 + j);
        j += CS6NUM-16;
        MovN2Byte(16, &TaskP.F67[0], AllBuf.Spi1 + j);
    }
    else
    {
        ZD_CS6_Init(); //;初始化参数区6
    }
    
     //;读7区
    ReadFlash_Verify(CSBock7, CSBock7Bak);
    if ((*AllBuf.Spi1==CSBiaoZhi1) && (*(AllBuf.Spi1+1)==CSBiaoZhi2) && 
    (*(AllBuf.Spi1+2)==CSBiaoZhi3))
    {
        j = 3;
        MovNByte(CS7NUM_GRP, &GRPP.F41.Flag, AllBuf.Spi1 + j); //;F41~F48
        j += CS7NUM_GRP;
        MovNByte(CS7NUM_DCP, DCPP.F81.ValueS, AllBuf.Spi1 + j); //;F81~F83
        j += CS7NUM_DCP;
        MovNByte(CS7NUM_CTR, F49, AllBuf.Spi1 + j); //;F49
        
    }
    else
    {
        ZD_CS7_Init(); //;初始化参数区7
    }
    
    //;读8区
    ReadFlash_Verify(CSBock8, CSBock8Bak);
    if ((*AllBuf.Spi1==CSBiaoZhi1) && (*(AllBuf.Spi1+1)==CSBiaoZhi2) && 
        (*(AllBuf.Spi1+2)==CSBiaoZhi3))
    {
        j = 3;
        MovNByte(4, ZDPKZ.F89.QuXian, AllBuf.Spi1 + j);
        j += 4;
        MovNByte(96,ZDPKZ.F97.UserSend, AllBuf.Spi1 + j);
        j += 96;
    }
    else
    {
        ZD_CS8_Init(true); //;初始化参数区8
    }
    
    
    //;读9区
    ReadFlash_Verify(CSBock9, CSBock9Bak);
    if ((*AllBuf.Spi1==CSBiaoZhi1) && (*(AllBuf.Spi1+1)==CSBiaoZhi2) && 
        (*(AllBuf.Spi1+2)==CSBiaoZhi3))
    {
        j = 3;
        MovN2Byte(CS9NUM, &TaskP.F65[4].Cyc, AllBuf.Spi1 + j);
    }
    else
    {
        ZD_CS9_Init(); //;初始化参数区9
    }
    
    //;读10区
    ReadFlash_Verify(CSBock10, CSBock10Bak);
    if ((*AllBuf.Spi1==CSBiaoZhi1) && (*(AllBuf.Spi1+1)==CSBiaoZhi2) && 
        (*(AllBuf.Spi1+2)==CSBiaoZhi3))
    {
        j = 3;
        MovN2Byte(CS10NUM, &TaskP.F66[4].Cyc, AllBuf.Spi1 + j);
    }
    else
    {
        ZD_CS10_Init(); //;初始化参数区10
    }

    
    SW.Para[0] = 0;
    SW.Para[1] = 0;
    EW.MoveByte = 0;
}


/* 
********************************************************************************
* 函 数 名：DDJ_FTP
* 功    能：根据给出的时间找出冻结数据对应该时间的序列号
* 参    数：Pn-测量点号；Td_d-需要读取冻结数据的时间
* 返    回：0xFF--未找到该时间点的数据 
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
u8 DDJ_FTP(u8 Pn, u8 *Td_d)
{
    u8 PointPage, compare;
    s16 Ind_Low, Ind_High, Ind_Mid,index;
    u8 T_Adr[3];
    u16 temp, page, PnDJHome, DJPointNum;
    u32 *Addr32;
    struct DJ_Control *PnDJ;

    index = ZD_Pn_Find(Pn);//;--20141225
    if(index < 0)          //;测量点不存在
    {
      return 0xFF;
    }
    Addr32 =(u32*)(*(PnDTAddr + index));
    PnDJ = (struct DJ_Control*)(*(Addr32 + _PnDDJ));
    PnDJHome = (u16)(*(Addr32 + _PnDDJHome));
    DJPointNum = DDJPointNum; //;总共存的点数
    PointPage = DDJPointPage; //;每点存页数

    if(PnDJ->Index == PnDJ->StInd)  //;冻结数据还不存在
    {
        return 0xFF;
    }
    
    Ind_Low = PnDJ->StInd;
    Ind_High = PnDJ->Index - 1;
    while(Ind_High >= Ind_Low)
    {
        Ind_Mid = (Ind_Low + Ind_High) / 2;
        temp = Ind_Mid % DJPointNum; //;整个冻结数据保存队列的结束位置
        page = temp * PointPage; //;所在的页码
        page += PnDJHome;
        EW.RFlashNByte = 'R';
        SPI_FLASH_ReadNByte(T_Adr, page, 0, 3); //;读出起始位置的时间
        EW.RFlashNByte = 0;
        
        compare = TimeCompare_3Byte(Td_d, T_Adr);
        if(compare == same)
        {
            return Ind_Mid;
        }
        else if(compare == ahead)
        {
            Ind_High = Ind_Mid - 1;
        }
        else
        {
            Ind_Low = Ind_Mid + 1;
        }
    }

    return 0xFF; //;冻结数据未找到

}


/* 
********************************************************************************
* 函 数 名：BDJ_FTP
* 功    能：根据给出的时间找出冻结数据对应该时间的序列号
* 参    数：Pn-测量点号；Td_d-需要读取冻结数据的时间
* 返    回：0xFF--未找到该时间点的数据 
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
u8 BDJ_FTP(u8 Pn, u8 *Td_d)
{
    u8 PointPage, compare;
    s16 Ind_Low, Ind_High, Ind_Mid, index;
    u8 T_Adr[3];
    u16 temp, page, PnDJHome, DJPointNum;
    u32 *Addr32;
    struct DJ_Control *PnDJ;

    index = ZD_Pn_Find(Pn);
    if( index < 0)  //;--20141225
    	return 0xFF;  //;测量点不存在
    Addr32 =(u32*)(*(PnDTAddr + index));
    PnDJ = (struct DJ_Control*)(*(Addr32 + _PnBDJ));
    PnDJHome = (u16)(*(Addr32 + _PnBDJHome));
    DJPointNum = BDJPointNum;
    PointPage = BDJPointPage;
    if(PnDJ->Index == PnDJ->StInd) //;冻结数据还不存在  
    { 
        return 0xFF;
    }

    Ind_Low = PnDJ->StInd;
    Ind_High = PnDJ->Index - 1;
    while(Ind_High >= Ind_Low)  
    {
        Ind_Mid = (Ind_Low + Ind_High) / 2;
        temp = Ind_Mid % DJPointNum; //;整个冻结数据保存队列的结束位置
        page = temp * PointPage; //;所在的页码
        page += PnDJHome;
        EW.RFlashNByte = 'R';
        SPI_FLASH_ReadNByte(T_Adr, page, 0, 3); //;读出起始位置的时间
        EW.RFlashNByte = 0;
/*        
        if ((T_Adr[0] == 0xFF) || (T_Adr[1] == 0xFF) || (T_Adr[2] == 0xFF))  
        {
            Ind_Low = Ind_Mid;  //;--@6
        }
*/
        compare = TimeCompare_3Byte(Td_d, T_Adr);
        if(compare == same)  
        {
            return Ind_Mid;
        }  
        else if(compare == ahead)  
        {
            Ind_High = Ind_Mid - 1;
        }  
        else  
        {
            Ind_Low = Ind_Mid + 1;
        }
    }

    return 0xFF; //;冻结数据未找到
}


/* 
********************************************************************************
* 函 数 名：MDJ_FTP
* 功    能：根据给出的时间找出冻结数据对应该时间的序列号
* 参    数：Pn-测量点号；Td_m-需要读取冻结数据的时间
* 返    回：0xFF--未找到该时间点的数据 
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：2011-09-23 13:23:11 修改算法
********************************************************************************
*/
u8 MDJ_FTP(u8 Pn, u8 *Td_m)
{
    u8 PointPage, compare;
    s16 Ind_Low, Ind_High, Ind_Mid, index;
    u8 T_Adr[2];
    u16 temp, page, PnDJHome, DJPointNum;
    u32 *Addr32;
    struct DJ_Control *PnDJ;

    index = ZD_Pn_Find(Pn);
    if( index < 0)  //;--20141225
    	return 0xFF;  //;测量点不存在
    Addr32 =(u32*)(*(PnDTAddr + index));
    PnDJ = (struct DJ_Control*)(*(Addr32 + _PnMDJ));
    PnDJHome = (u16)(*(Addr32 + _PnMDJHome));
    DJPointNum = MDJPointNum;
    PointPage = MDJPointPage;
    if (PnDJ->Index == PnDJ->StInd) //;冻结数据还不存在  
    { 
        return 0xFF;
    }


    Ind_Low = PnDJ->StInd;
    Ind_High = PnDJ->Index - 1;
    while(Ind_High >= Ind_Low)  
    {
        Ind_Mid = (Ind_Low + Ind_High) / 2;
        temp = Ind_Mid % DJPointNum; //;整个冻结数据保存队列的结束位置
        page = temp * PointPage; //;所在的页码
        page += PnDJHome;
        EW.RFlashNByte = 'R';
        SPI_FLASH_ReadNByte(T_Adr, page, 0, 2); //;读出起始位置的时间
        EW.RFlashNByte = 0;
       
        compare = TimeCompare_2Byte(Td_m, T_Adr);
        if(compare == same)  
        {
            return Ind_Mid;
        } 
        else if(compare == ahead)  
        {
            Ind_High = Ind_Mid - 1;
        }  
        else  
        {
            Ind_Low = Ind_Mid + 1;
        }
    }

    return 0xFF; //;冻结数据未找到

}


/* 
********************************************************************************
* 函 数 名：QDJ_FTP
* 功    能：根据给出的时间找出冻结数据对应该时间的序列号
* 参    数：Pn-测量点号；Time-需要读取冻结数据的时间
* 返    回：0xFF--未找到该时间点的数据 
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：2011-09-23 13:23:11 修改算法
********************************************************************************
*/
u16 QDJ_FTP(u8 Pn, u8 *Time)
{
    u8 T_Adr[5];
    u8 area_ind,PagePoint, AreaNum, compare;
    s16 Ind_Low, Ind_High, Ind_Mid, index;
    u16 temp, page, PnDJHome, DJPointNum;
    u32 *Addr32;
    struct QX_Control *PnDJ;

    index = ZD_Pn_Find(Pn);
    if( index < 0)  //;--20141225
    	return 0xFF;  //;测量点不存在
    Addr32 =(u32*)(*(PnDTAddr + index));
    PnDJ = (struct QX_Control*)(*(Addr32 + _PnQDJ));
    PnDJHome = (u16)(*(Addr32 + _PnQDJHome));
    DJPointNum = QDJPointNum; //;曲线冻结总共的点
    PagePoint = QDJPagePoint; //;每页存的点
    AreaNum = QDJAreaNum;     //;每点存的字节
    
    if (PnDJ->Index == PnDJ->StInd)  
    {
        return 0xFFFF;
    }

    Ind_Low = PnDJ->StInd;
    Ind_High = PnDJ->Index - 1;
    while(Ind_High >= Ind_Low)  
    {
        Ind_Mid = (Ind_Low + Ind_High) / 2;
        temp = Ind_Mid % DJPointNum;
        page = temp / PagePoint;
        page += PnDJHome;
        area_ind = temp % PagePoint;
        EW.RFlashNByte = 'R';
        SPI_FLASH_ReadNByte(T_Adr, page, area_ind * AreaNum, 5);
        EW.RFlashNByte = 0;
        
        compare = TimeCompare_5Byte(Time, T_Adr);
        if(compare == same)  
        {
            return Ind_Mid;
        }  
        else if(compare == ahead)  
        {
            Ind_High = Ind_Mid - 1;
        }  
        else  
        {
            Ind_Low = Ind_Mid + 1;
        }
    }

    return 0xFFFF;
}


/* 
********************************************************************************
* 函 数 名：DDJ_Read
* 功    能：根据输入日期等; 找出出日冻结数据
* 参    数：Buf_Out-冻结数据存放数组；Buf_Ind-存放索引
*           Pn-测量点号；Td_d-时标
* 返    回：0xFF-数据不存在，否则为读取的数据长度
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：2011-09-22 20:27:39
********************************************************************************
*/
u8 DDJ_Read(char *Buf_Out, u16 Buf_Ind, u8 Pn, u8 Fn, u8 *Td_d)
{
    u8  j,DJIndex;
    u16 Buf_Add = 0;
    u8  len, expage, readtime, ratenum, temp;
    u16 page, PnDJHome, stindex;
    u32 *adr32;
    u8  i;
    s16 index;

    DJIndex = DDJ_FTP(Pn, Td_d);
    if(DJIndex == 0xFF)   //;不存在
    {
        return 0xFF;
    }  

    adr32 = (u32*)(PN0DZU + Fn);
    temp = *((u8*)adr32 + 3); 
    readtime = temp & 0x08;
    ratenum  = temp & 0x10;
    len  = *((u8*)adr32 + 2);
    expage = temp >> 5;
    stindex = *(u16*)(adr32);
    
    index = ZD_Pn_Find(Pn);
    if( index < 0)  //;--20141225
    	return 0xFF;  //;测量点不存在
    adr32 = (u32*)(*(PnDTAddr + index));
    PnDJHome = (u16)(*(adr32 + _PnDDJHome));
    page = (DJIndex %DDJPointNum)* DDJPointPage;
    page += PnDJHome + expage;
    
    
    SPI_FLASH_PageRead(AllBuf.Spi1, page);

    *(Td_d + 0) = *(AllBuf.Spi1 + 0); //;写入找到数据的日期
    *(Td_d + 1) = *(AllBuf.Spi1 + 1);
    *(Td_d + 2) = *(AllBuf.Spi1 + 2);

    if(readtime)   //;包含抄表日
    {  
        *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + 3);
        *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + 4);
        *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + 5);
        *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + 6);
        *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + 7);
        Buf_Add += 5;
    }
    if(ratenum)   //;包含费率数
    {  
        *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + 8);
        Buf_Add += 1;
    }

	Buf_Add += 1; //;保留一个字节空间  //;20130926
    if(*(AllBuf.Spi1 + 9) == 'T') //补冻结标识
    {
        if(Fn >=5 && Fn <=8)
	      {
	        return 0xFF;
        }
    }
//////////////////////////////////////////////////////// by zrt 20130808 F185-F188格式需要调整
    if(Fn>=185&&Fn<=188)
    {
      for(i=0; i<5; i++)
      {
        for(j=0; j<3; j++)
        {
            *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + stindex + j + 3*i);
        }
        for(j=0; j<4; j++)
        {
            *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + stindex + j + 15 + 4*i);
        }
      }
    }
    else 
    {
      for(j=0; j<len; j++)  
      {
          *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + stindex + j);
      }
    }
////////////////////////////////////////////////////////////
/*    
    for(j=0; j<len; j++)  
    {
        *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + stindex + j);
    }
*/    
    Buf_Add += len;
    
    return Buf_Add;
}


/* 
********************************************************************************
* 函 数 名：BDJ_Read
* 功    能：根据输入日期 找出抄表日冻结数据
* 参    数：Buf_Out-冻结数据存放数组；Buf_Ind-存放索引
*           Pn-测量点号；Td_d-时标
* 返    回：0xFF-数据不存在，否则为读取的数据长度
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
u8 BDJ_Read(char *Buf_Out, u16 Buf_Ind, u8 Pn, u8 Fn, u8 *Td_d)
{
    u8 j, DJIndex;
    u16 Buf_Add = 0;
    u8  len, temp, readtime, ratenum, expage;
    u16 page, PnDJHome, stindex;
    u32 *adr32;
    u8 i;
    s16 index;

    DJIndex = BDJ_FTP(Pn, Td_d);
    if (DJIndex == 0xFF)
    {
        return 0xFF; //;不存在
    }

    adr32 = (u32*)(PN0DZU + Fn);
    temp = *((u8*)adr32 + 3); 
    readtime = temp & 0x08;
    ratenum  = temp & 0x10;
    len  = *((u8*)adr32 + 2);
    expage = temp >> 5;
    stindex = *(u16*)(adr32);

    index = ZD_Pn_Find(Pn);
    if( index < 0)  //;--20141225
    	return 0xFF;  //;测量点不存在
    
    adr32 = (u32*)(*(PnDTAddr + index));
    PnDJHome = (u16)(*(adr32 + _PnBDJHome));
    page = (DJIndex % BDJPointNum) * BDJPointPage;
    page += PnDJHome + expage;
    SPI_FLASH_PageRead(AllBuf.Spi1, page);

    *(Td_d + 0) = *(AllBuf.Spi1 + 0);
    *(Td_d + 1) = *(AllBuf.Spi1 + 1);
    *(Td_d + 2) = *(AllBuf.Spi1 + 2);

    if(readtime)  //;包含抄表日
    {
        *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + 3);
        *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + 4);
        *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + 5);
        *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + 6);
        *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + 7);
        Buf_Add += 5;
    }
    if(ratenum)
    {
        *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + 8);   //;包含费率数
        Buf_Add += 1;
    }
//////////////////////////////////////////////////////// by zrt 20130808 F189-F192格式需要调整
    if(Fn>=189&&Fn<=192)
    {
      for(i=0; i<5; i++)
      {
        for(j=0; j<3; j++)
        {
            *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + stindex + j + 3*i);
        }
        for(j=0; j<4; j++)
        {
            *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + stindex + j + 15 + 4*i);
        }
      }
    }
    else 
    {
      for(j=0; j<len; j++)  
      {
          *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + stindex + j);
      }
    }
////////////////////////////////////////////////////////////
/*    
    for(j=0; j<len; j++)  
    {
        *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + stindex + j);
    }
*/
    Buf_Add += len;
    
    return Buf_Add;
}


/* 
********************************************************************************
* 函 数 名：MDJ_Read
* 功    能：根据输入时期, Pn, Fn; 找出月冻结数据
* 参    数：Buf_Out-冻结数据存放数组；Buf_Ind-存放索引
*           Pn-测量点号；Td_d-时标
* 返    回：0xFF-数据不存在，否则为读取的数据长度
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：
********************************************************************************
*/
u8 MDJ_Read(char *Buf_Out, u16 Buf_Ind, u8 Pn, u8 Fn, u8 *Td_m)
{
    u8 j, DJIndex;
    u16 Buf_Add = 0;
    u8 len, expage, temp, readtime, ratenum;
    u16 page, PnDJHome, stindex;
    u32 *adr32;
    u8 i;
    s16 index;

    DJIndex = MDJ_FTP(Pn, Td_m);
    if (DJIndex == 0xFF)  
    {
        return 0xFF; //;不存在
    }

    adr32 = (u32*)(PN0DZU + Fn);
    temp = *((u8*)adr32 + 3); 
    readtime = temp & 0x08;
    ratenum  = temp & 0x10;
    len  = *((u8*)adr32 + 2);
    expage = temp >> 5;
    stindex = *(u16*)(adr32);

    index = ZD_Pn_Find(Pn);
    if( index < 0)  //;--20141225
    	return 0xFF;  //;测量点不存在
    adr32 = (u32*)(*(PnDTAddr + index));
    PnDJHome = (u16)(*(adr32 + _PnMDJHome));
    page = (DJIndex % MDJPointNum) * MDJPointPage;
    page += PnDJHome + expage;
    
    SPI_FLASH_PageRead(AllBuf.Spi1, page);

    *(Td_m + 0) = *(AllBuf.Spi1 + 0);
    *(Td_m + 1) = *(AllBuf.Spi1 + 1);
    
    if(readtime)   //;包含抄表日
    {  
        *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + 2);
        *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + 3);
        *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + 4);
        *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + 5);
        *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + 6);
        Buf_Add += 5;
    }
    if(ratenum) //;包含费率数 
    { 
        *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + 7);
        Buf_Add += 1;
    }

    if(*(AllBuf.Spi1 + 8) == 'T') //补冻结标识
    {
        if(Fn >=21 && Fn <=24)
	 {
	     return 0xFF;
        }
    }

    Buf_Add += 1; //;保留一个字节空间  //;20130926	
	
//////////////////////////////////////////////////////// by zrt 20130808 F193-F196格式需要调整
    if(Fn>=193&&Fn<=196)
    {
      for(i=0; i<5; i++)
      {
        for(j=0; j<3; j++)
        {
            *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + stindex + j + 3*i);
        }
        for(j=0; j<4; j++)
        {
            *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + stindex + j + 15 + 4*i);
        }
      }
    }
    else 
    {
      for(j=0; j<len; j++)  
      {
          *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + stindex + j);
      }
    }
////////////////////////////////////////////////////////////
/*    
    for(j=0; j<len; j++)  
    {
        *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + stindex + j);
    }
*/
    
    Buf_Add += len;
    
    return Buf_Add;
}

/* 
********************************************************************************
* 函 数 名：QDJ_Read
* 功    能：根据输入日期， 找出曲线冻结数据
* 参    数：Buf_Out-冻结数据存放数组；Buf_Ind-存放索引
*           Pn-测量点号；Td_d-时标
* 返    回：0xFF-数据不存在，否则为读取的数据长度
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
/*
u8 QDJ_Read(char *Buf_Out, u16 Buf_Ind, u8 Pn, u8 Fn, u8 *Td_c)
{
    u8 j, index, Buf_Add = 0;
    u8 area_ind,  len;
    u16 page, PnDJHome, DJIndex, stindex;
    u32 *adr32;

    DJIndex = QDJ_FTP(Pn, Td_c);
    if(DJIndex == 0xFFFF)   //;不存在 
    { 
        return 0xFF;
    }

    adr32 = (u32*)(PN0DZU + Fn); 
    len  = *((u8*)adr32 + 2);
    stindex = *(u16*)(adr32);
    
    index = ZD_Pn_Find(Pn);
    adr32 = (u32*)(*(PnDTAddr + index));
    PnDJHome = (u16)(*(adr32 + _PnQDJHome));
    area_ind = (DJIndex % QDJPointNum) % QDJPagePoint;
    page = (DJIndex % QDJPointNum) / QDJPagePoint;
    page += PnDJHome;    
    EW.RFlashNByte = 'R';
    SPI_FLASH_ReadNByte(AllBuf.Spi1, page, area_ind * QDJAreaNum, QDJAreaNum); //;读出曲线数据
    EW.RFlashNByte = 0;
    
    *(Td_c + 0) = *(AllBuf.Spi1 + 0);
    *(Td_c + 1) = *(AllBuf.Spi1 + 1);
    *(Td_c + 2) = *(AllBuf.Spi1 + 2);
    *(Td_c + 3) = *(AllBuf.Spi1 + 3);
    *(Td_c + 4) = *(AllBuf.Spi1 + 4);
    
    for(j=0; j<len; j++) //;写入曲线数据 
    {  
        *(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + stindex + j);
    }
    Buf_Add += len;
    
    return Buf_Add;
}
*/

u8 QDJ_Read(char *Buf_Out, u16 Buf_Ind, u8 Pn, u8 Fn, u8 *Td_c)
{
	u8 j;
	u16 Buf_Add = 0;
	u8 area_ind,  len;
	u16 page, PnDJHome, DJIndex, stindex;
	u32 *adr32;
	s16 index;
	
	DJIndex = QDJ_FTP(Pn, Td_c);
//	if(DJIndex == 0xFFFF)   //;不存在 
//	{ 
//		return 0xFF;
//	}
		
	adr32 = (u32*)(PN0DZU + Fn); 
	len  = *((u8*)adr32 + 2);
	stindex = *(u16*)(adr32);

//	*(Td_c + 0) = *(AllBuf.Spi1 + 0);
//	*(Td_c + 1) = *(AllBuf.Spi1 + 1);
//	*(Td_c + 2) = *(AllBuf.Spi1 + 2);
//	*(Td_c + 3) = *(AllBuf.Spi1 + 3);
//	*(Td_c + 4) = *(AllBuf.Spi1 + 4);
	if(DJIndex == 0xFFFF)   //;qiu_20130516
	{
		for(j=0; j<len; j++) //;写入曲线数据 
		{  
			*(Buf_Out + Buf_Ind++) = 0xee;
		}
	}
	else
	{
		index = ZD_Pn_Find(Pn);
	  if( index < 0)  //;--20141225
    	return 0xFF;  //;测量点不存在	
		adr32 = (u32*)(*(PnDTAddr + index));
		PnDJHome = (u16)(*(adr32 + _PnQDJHome));
		area_ind = (DJIndex % QDJPointNum) % QDJPagePoint;
		page = (DJIndex % QDJPointNum) / QDJPagePoint;
		page += PnDJHome;    
		EW.RFlashNByte = 'R';
		SPI_FLASH_ReadNByte(AllBuf.Spi1, page, area_ind * QDJAreaNum, QDJAreaNum); //;读出曲线数据
		EW.RFlashNByte = 0;
		for(j=0; j<len; j++) //;写入曲线数据 
		{  
			*(Buf_Out + Buf_Ind++) = *(AllBuf.Spi1 + stindex + j);
		}
	}
	Buf_Add += len;
	
	return Buf_Add;
}


/* 
********************************************************************************
* 函 数 名：HDJ_Read
* 功    能：小时冻结数据从Flash中读出。
* 参    数：无
* 返    回：无
* 说    明：上电时调用
* 抛出异常：
* 作    者：023
* 修改记录：2011-09-29 16:37:06 增加前一日日期计算,修改读出数据处理
********************************************************************************
*/
void HDJ_Read(void)
{
    u8 ch = false;
    u8 i, year=0, month=0, day=0, hour=0;
    u16 j, PnNHDJHome,PnOHDJHome;
    u32 *Addr32;
    struct PointDataStr *PnD;

    for(i=0; i<PNMAXNUM; i++)
    {
        Addr32 = (u32*)(*(PnDTAddr + i));
        PnD = (struct PointDataStr*)(*(Addr32 + _PnD));
        PnNHDJHome = (u16)(*(Addr32 + _PnNHDJHome));
        PnOHDJHome = (u16)(*(Addr32 + _PnOHDJHome));
        
        SPI_FLASH_PageRead(AllBuf.Spi1, PnNHDJHome);      //;读出当前小时冻结数据
        if((*AllBuf.Spi1=='H') && (*(AllBuf.Spi1+1)=='D') && 
           (*(AllBuf.Spi1+2)=='J'))
        {
            j = 7;
            EW.MoveByte = 'M';
            MovNByte(56, PnD->NDJ_B630.Td_h,  AllBuf.Spi1 + j);    //;总,A,B,C有功功率
            j += 56;
            MovNByte(56, PnD->NDJ_B640.Td_h,  AllBuf.Spi1 + j);    //;总,A,B,C无功功率
            j += 56;
            MovNByte(30, PnD->NDJ_B611.Td_h,  AllBuf.Spi1 + j);    //;A,B,C相电压
            j += 30;
/*            MovNByte(40, PnD->NDJ_B621.Td_h,  AllBuf.Spi1 + j);    //;A,B,C相电流以及中线电流  
            j += 40;  */ 
//   by zrt 20130719    电流为3个字节，所以总共改为56个字节
            MovNByte(56, PnD->NDJ_B621.Td_h,  AllBuf.Spi1 + j);    //;A,B,C相电流以及中线电流
            j += 56;
            MovNByte(72, PnD->NDJ_9010.Td_h,  AllBuf.Spi1 + j);    //;小时冻结日电能量
            j += 72;
            MovNByte(72, PnD->NDJ_9010_1.Td_h,  AllBuf.Spi1 + j);  //;小时冻结总电能示值
            j += 72;
            MovNByte(40, PnD->NDJ_B650.Td_h,  AllBuf.Spi1 + j);    //;总,A,B,C相功率因数

            EW.MoveByte = 0;
        }
        
        SPI_FLASH_PageRead(AllBuf.Spi1, PnOHDJHome);     //;读出保存的上一小时冻结数据  
        if((*AllBuf.Spi1=='H') && (*(AllBuf.Spi1+1)=='D') && 
           (*(AllBuf.Spi1+2)=='J'))
        {
            j = 3;
            year = *(AllBuf.Spi1 + j++);
            month = *(AllBuf.Spi1 + j++);
            day = *(AllBuf.Spi1 + j++);
            hour = *(AllBuf.Spi1 + j++);
            EW.MoveByte = 'M';
            MovNByte(56, PnD->ODJ_B630.Td_h,  AllBuf.Spi1 + j);    //;总,A,B,C有功功率
            j += 56;
            MovNByte(56, PnD->ODJ_B640.Td_h,  AllBuf.Spi1 + j);    //;总,A,B,C无功功率
            j += 56;
            MovNByte(30, PnD->ODJ_B611.Td_h,  AllBuf.Spi1 + j);    //;A,B,C相电压
            j += 30;
/*            MovNByte(40, PnD->ODJ_B621.Td_h,  AllBuf.Spi1 + j);    //;A,B,C相电流以及中线电流
            j += 40;			
*/         
//by zrt 20130719 
            MovNByte(56, PnD->ODJ_B621.Td_h,  AllBuf.Spi1 + j);    //;A,B,C相电流以及中线电流
            j += 56;
            MovNByte(72, PnD->ODJ_9010.Td_h,  AllBuf.Spi1 + j);    //;小时冻结日电能量
            j += 72;
            MovNByte(72, PnD->ODJ_9010_1.Td_h,  AllBuf.Spi1 + j);  //;小时冻结总电能示值
            j += 72;
            MovNByte(40, PnD->ODJ_B650.Td_h,  AllBuf.Spi1 + j);    //;总,A,B,C相功率因数
            EW.MoveByte = 0;
        }
       
        if((year != THEX.Year) || (month != THEX.Month) || 
           (day != THEX.Day) || (hour != THEX.Hour)) //;如果保存的不是当前小时的数据
        {
            //;计算下一日的年月日
            if(hour >= 23)  //;天翻转
	  	  	  {
	  	  	  	  hour = 0;	  	  	 	  
	  	  	  	  if(day >= DaysOfMonth[month])  //;判断是否发生月翻转
	  	  	  	  {
	  	  	  	  	  if(month == 2)  //;二月份的特殊情况
	  	  	  	  	  {
	  	  	  	  	  	  if((year % 4) == 0x00)  //;闰年 --@2
	  	  	  	  	  	  {
	  	  	  	  	  	  	  if(day >= 29 )
	  	  	  	  	  	  	  {
	  	  	  	  	  	  	  	  day = 1;
	  	  	  	  	  	  	  }
	  	  	  	  	  	  	  else   
	  	  	  	  	  	  	  {
	  	  	  	  	  	  	  	  day++;
	  	  	  	  	  	  	  }
	  	  	  	  	  	  }
	  	  	  	  	  	  else
	  	  	  	  	  	  {
	  	  	  	  	  	  	  day = 1;
	  	  	  	  	  	  }
	  	  	  	  	  }
	  	  	  	  	  else
	  	  	  	  	  {
	  	  	  	  	  	  day = 1;
	  	  	  	  	  }
	  	  	  	  	  
	  	  	  	  	  if(day == 1) //;发生月翻转
	  	  	  	  	  {	  	  	  	  	  	      	  	  	  	  	  	      
	  	  	  	  	      if(month >= 12) //;发生年翻转
	  	  	  	  	      {	  	  	  	  	  	          
	  	  	  	  	          month = 1;
	  	  	  	  	          year++;
	  	  	  	  	      }
	  	  	  	  	      else
	  	  	  	  	      {
	  	  	  	  	          month++;
	  	  	  	  	      }
	  	  	  	  	  }
	  	  	  	  }
	  	  	  	  else
	  	  	  	  {
	  	  	  	  	  day++;
	  	  	  	  }
	  	  	  }
            else
            {
                hour++;
            }
            
            if((hour == THEX.Hour) && (year == THEX.Year) && 
                (month == THEX.Month) && (day == THEX.Day)) //;如果为上一个小时的数据
            {
                ch = true;
            }           
            
            if(ch == true)  //;如果保存的数据为上一个小时的数据
            {
                EW.MoveByte = 'M';
                MovNByte(56, PnD->ODJ_B630.Td_h,   PnD->NDJ_B630.Td_h);    //;总,A,B,C有功功率
                MovNByte(56, PnD->ODJ_B640.Td_h,   PnD->NDJ_B640.Td_h);    //;总,A,B,C无功功率
                MovNByte(30, PnD->ODJ_B611.Td_h,   PnD->NDJ_B611.Td_h);    //;A,B,C相电压
//              MovNByte(40, PnD->ODJ_B621.Td_h,   PnD->NDJ_B621.Td_h);    //;A,B,C相电流以及中线电流  by zrt 20130719
                MovNByte(56, PnD->ODJ_B621.Td_h,   PnD->NDJ_B621.Td_h);    //;A,B,C相电流以及中线电流                
                MovNByte(72, PnD->ODJ_9010.Td_h,   PnD->NDJ_9010.Td_h);    //;小时冻结日电能量
                MovNByte(72, PnD->ODJ_9010_1.Td_h, PnD->NDJ_9010_1.Td_h);  //;小时冻结总电能示值
                MovNByte(40, PnD->ODJ_B650.Td_h,   PnD->NDJ_B650.Td_h);    //;总,A,B,C相功率因数
                EW.MoveByte = 0;
                HDJ_Reset(i);  //;重新开始一个小时的小时冻结前将小时冻结数据复位为没有该数据
            }
            else //;不是上一小时数据，也不是当前小时数据
            {
                ByteN_Fill(366, PnD->ODJ_B630.Td_h, 0x00);               
            }
        }
        HDJ_Pre(i); //;根据小时冻结的参数把当前冻结数据中的Td_h准备好
    }
}


/* 
********************************************************************************
* 函 数 名：HDJ_Save
* 功    能：将小时冻结数据保存到FLASH
* 参    数：Pn-测量点号
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void HDJ_Save(u8 Pn)
{
    u16 j, PnNHDJHome, PnOHDJHome;
    u32 *Addr32;
    struct PointDataStr *PnD;

    Addr32 = (u32*)(*(PnDTAddr + Pn));
    PnD = (struct PointDataStr*)(*(Addr32 + _PnD));
    PnNHDJHome = (u16)(*(Addr32 + _PnNHDJHome));
    PnOHDJHome = (u16)(*(Addr32 + _PnOHDJHome));  
    
    EW.MoveByte = 'M'; //;保存当前小时数据
    j = 0;
    *(AllBuf.Spi1 + j++) = 'H';
    *(AllBuf.Spi1 + j++) = 'D';
    *(AllBuf.Spi1 + j++) = 'J';
    *(AllBuf.Spi1 + j++) = THEX.Year;
    *(AllBuf.Spi1 + j++) = THEX.Month;
    *(AllBuf.Spi1 + j++) = THEX.Day;
    *(AllBuf.Spi1 + j++) = THEX.Hour;

    MovNByte(56,  AllBuf.Spi1 + j, PnD->NDJ_B630.Td_h);    //;总,A,B,C有功功率
    j += 56;
    MovNByte(56,  AllBuf.Spi1 + j, PnD->NDJ_B640.Td_h);    //;总,A,B,C无功功率
    j += 56;
    MovNByte(30,  AllBuf.Spi1 + j, PnD->NDJ_B611.Td_h);    //;A,B,C相电压
    j += 30;
/*	
    MovNByte(40,  AllBuf.Spi1 + j, PnD->NDJ_B621.Td_h);    //;A,B,C相电流以及中线电流
    j += 40;  
*/
//   by zrt 20130719    电流为3个字节，所以总共40字节应改为56个字节
    MovNByte(56,  AllBuf.Spi1 + j, PnD->NDJ_B621.Td_h);    //;A,B,C相电流以及中线电流
    j += 56;  
    MovNByte(72,  AllBuf.Spi1 + j, PnD->NDJ_9010.Td_h);    //;小时冻结日电能量
    j += 72;
    MovNByte(72,  AllBuf.Spi1 + j, PnD->NDJ_9010_1.Td_h);  //;小时冻结总电能示值
    j += 72;
    MovNByte(40,  AllBuf.Spi1 + j, PnD->NDJ_B650.Td_h);    //;总,A,B,C相功率因数
    j += 40;

    for(; j < 527; j++)
    {
        *(AllBuf.Spi1 + j) = 0x00; //;不用的地址填充为0
    }
    
    SPI_FLASH_PageWrite_E(AllBuf.Spi1,PnNHDJHome);

    j = 0; //;保存上一小时数据
    *(AllBuf.Spi1 + j++) = 'H';
    *(AllBuf.Spi1 + j++) = 'D';
    *(AllBuf.Spi1 + j++) = 'J';
    *(AllBuf.Spi1 + j++) = THEX.Year;
    *(AllBuf.Spi1 + j++) = THEX.Month;
    *(AllBuf.Spi1 + j++) = THEX.Day;
    *(AllBuf.Spi1 + j++) = THEX.Hour;

    MovNByte(56,  AllBuf.Spi1 + j, PnD->ODJ_B630.Td_h);    //;上一小时冻结总,A,B,C有功功率
    j += 56;
    MovNByte(56,  AllBuf.Spi1 + j, PnD->ODJ_B640.Td_h);    //;上一小时冻结总,A,B,C无功功率
    j += 56;
    MovNByte(30,  AllBuf.Spi1 + j, PnD->ODJ_B611.Td_h);    //;上一小时冻结A,B,C相电压
    j += 30;
/*	
    MovNByte(40,  AllBuf.Spi1 + j, PnD->ODJ_B621.Td_h);    //;上一小时冻结A,B,C相电流以及中线电流
    j += 40;
*/
//20130719 by zrt 
    MovNByte(56,  AllBuf.Spi1 + j, PnD->ODJ_B621.Td_h);    //;上一小时冻结A,B,C相电流以及中线电流
    j += 56;
    MovNByte(72,  AllBuf.Spi1 + j, PnD->ODJ_9010.Td_h);    //;上一小时冻结日电能量
    j += 72;
    MovNByte(72,  AllBuf.Spi1 + j, PnD->ODJ_9010_1.Td_h);  //;上一小时冻结总电能示值
    j += 72;
    MovNByte(40,  AllBuf.Spi1 + j, PnD->ODJ_B650.Td_h);    //;上一小时冻结总,A,B,C相功率因数
    j += 40;

    EW.MoveByte = 0;
    
    for (; j < 527; j++)
    {
        *(AllBuf.Spi1 + j) = 0x00;
    }
    
    SPI_FLASH_PageWrite_E(AllBuf.Spi1, PnOHDJHome);
}

/* 
********************************************************************************
* 函 数 名：StatData_Read
* 功    能：读出当日当月的统计数据
* 参    数：无
* 返    回：无
* 说    明：443B+3B+3B
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void MVT_Read(void)
{
    u8 i, Year, Month, Day;
    u16 j, PnMVTHome;
    u32 *Addr32;
    struct PointDataStr *PnD;

    for(i=0; i<PNMAXNUM; i++)
    {
        Addr32 = (u32*)(*(PnDTAddr + i));
        PnD = (struct PointDataStr*)(*(Addr32 + _PnD));
        PnMVTHome = (u16)(*(Addr32 + _PnMVTHome));

        SPI_FLASH_PageRead(AllBuf.Spi1, PnMVTHome);
        if ((*AllBuf.Spi1=='M') && (*(AllBuf.Spi1+1)=='T') && 
            (*(AllBuf.Spi1+2)=='V'))
        {
            j = 3;
            Day  = *(AllBuf.Spi1 + j++);
            Month = *(AllBuf.Spi1 + j++);
            Year  = *(AllBuf.Spi1 + j++);
            if((Year == THEX.Year) && (Month == THEX.Month))
            {
                /*如果是当月统计数据*/
                EW.MoveByte = 'M';
                MovNByte(32, PnD->DF33.ZPMax,  AllBuf.Spi1 + j);   //;当月总及分相最大功率及发生时间  F33
                j += 32;
                MovNByte(24, PnD->DF34.ZPMN, AllBuf.Spi1 + j);    //;当月总用分相最大需量及发生时间  F34
                j += 24;
                MovNByte(66, PnD->DF35.AVUUMin, AllBuf.Spi1 + j);  //;当月电压统计数据  F35
                j += 66;
                MovNByte(16, PnD->DF36.IImbMin, AllBuf.Spi1 + j);//;当月不平衡度超限累计时间 F36
                j += 16;
                MovNByte(38, PnD->DF37.AIUUMin, AllBuf.Spi1 + j);  //;当月电流统计数据 F37
                j += 38;
                MovNByte(4,  PnD->DF38.SUUMin, AllBuf.Spi1 + j);   //;当月视在功率越限累计时间 F38
                j += 4;
                MovNByte(12, PnD->DF39.Vmax, AllBuf.Spi1 + j);   //;月负载率统计 
                j += 12;
                MovNByte(4,  PnD->DF51.PowerT, AllBuf.Spi1 + j);   //;月冻结供电时间月复位累计
                j += 4;
                MovNByte(4,  PnD->DF52.MonthCutNum, AllBuf.Spi1 + j);   //;月冻结控制统计
                j += 4;
                MovNByte(4,  (u8*)&ZDS.F10.Flux_Month, AllBuf.Spi1 + j);   //;流量 F54
                j += 4;
                MovNByte(14,  PnD->DF213.ProgramNum, AllBuf.Spi1 + j);   //;F213
                j += 14;
                MovNByte(14,  PnD->DF214.CLKChangeNum, AllBuf.Spi1 + j);   //;F214
                j += 14;
                if(Day == THEX.Day)
                {
                    MovNByte(32, PnD->DF25.ZPMax,  AllBuf.Spi1 + j);  
                    j += 32;
                    MovNByte(24, PnD->DF26.ZPMN, AllBuf.Spi1 + j);    
                    j += 24;
                    MovNByte(66, PnD->DF27.AVUUMin, AllBuf.Spi1 + j); 
                    j += 66;
                    MovNByte(14, PnD->DF28.IImbMin, AllBuf.Spi1 + j);
                    j += 14;
                    MovNByte(38, PnD->DF29.AIUUMin, AllBuf.Spi1 + j); 
                    j += 38;
                    MovNByte(4,  PnD->DF30.SUUMin, AllBuf.Spi1 + j); 
                    j += 4;
                    MovNByte(10, PnD->DF31.Vmax, AllBuf.Spi1 + j);   
                    j += 10;
                    MovNByte(4,  PnD->DF49.PowerT, AllBuf.Spi1 + j);   
                    j += 4;
                    MovNByte(4,  PnD->DF50.MonthCutNum, AllBuf.Spi1 + j);   
                    j += 4;
                    MovNByte(4,  (u8*)&ZDS.F10.Flux_Day, AllBuf.Spi1 + j);  
                    j += 4;                    
                    MovNByte(11,  PnD->DF209.MeterPowerStat, AllBuf.Spi1 + j);                
                }
                EW.MoveByte = 0;
            }
        }
    }
}


/* 
********************************************************************************
* 函 数 名：MVT_Save
* 功    能：掉电保存,每小时保存一次当日当月的, Most Value and Time
* 参    数：Pn-信息点
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void MVT_Save(u8 Pn)
{
    u16 j, PnMVTHome;
    u32 *Addr32;
    struct PointDataStr *PnD;
  
    Addr32 = (u32*)(*(PnDTAddr + Pn));
    PnD = (struct PointDataStr*)(*(Addr32 + _PnD));
    PnMVTHome = (u16)(*(Addr32 + _PnMVTHome));

    j = 0;
    *(AllBuf.Spi1 + j++) = 'M';
    *(AllBuf.Spi1 + j++) = 'T';
    *(AllBuf.Spi1 + j++) = 'V';
    *(AllBuf.Spi1 + j++) = THEX.Day;
    *(AllBuf.Spi1 + j++) = THEX.Month;
    *(AllBuf.Spi1 + j++) = THEX.Year;

    EW.MoveByte = 'M';
    MovNByte(32, AllBuf.Spi1 + j, PnD->DF33.ZPMax);//;当月总及分相最大功率及发生时间  F33
    j += 32;
    MovNByte(24, AllBuf.Spi1 + j, PnD->DF34.ZPMN);//;当月总及分相最大需量及发生时间  F34
    j += 24;
    MovNByte(66, AllBuf.Spi1 + j, PnD->DF35.AVUUMin); //;当月电压统计数据  F35
    j += 66;
    MovNByte(16, AllBuf.Spi1 + j, PnD->DF36.IImbMin);//;当月不平衡度超限累计时间 F36
    j += 16;
    MovNByte(38, AllBuf.Spi1 + j, PnD->DF37.AIUUMin); //;当月电流统计数据 F37
    j += 38;
    MovNByte(4, AllBuf.Spi1 + j, PnD->DF38.SUUMin);//;当月视在功率越限累计时间 F38
    j += 4;
    MovNByte(12,AllBuf.Spi1 + j, PnD->DF39.Vmax);   //;月负载率统计 
    j += 12;
    MovNByte(4, AllBuf.Spi1 + j,  PnD->DF51.PowerT);   //;月冻结供电时间月复位累计
    j += 4;
    MovNByte(4, AllBuf.Spi1 + j,  PnD->DF52.MonthCutNum);   //;月冻结控制统计
    j += 4;
    MovNByte(4, AllBuf.Spi1 + j,  (u8*)&ZDS.F10.Flux_Month);   //;流量 F54
    j += 4;
    MovNByte(14, AllBuf.Spi1 + j,  PnD->DF213.ProgramNum);   //;F213
    j += 14;
    MovNByte(14, AllBuf.Spi1 + j,  PnD->DF214.CLKChangeNum);   //;F214
    j += 14;
    
    //;日统计数据
    MovNByte(32,  AllBuf.Spi1 + j, PnD->DF25.ZPMax);  
    j += 32;
    MovNByte(24, AllBuf.Spi1 + j, PnD->DF26.ZPMN);    
    j += 24;
    MovNByte(66, AllBuf.Spi1 + j, PnD->DF27.AVUUMin); 
    j += 66;
    MovNByte(14, AllBuf.Spi1 + j, PnD->DF28.IImbMin);
    j += 14;
    MovNByte(38, AllBuf.Spi1 + j, PnD->DF29.AIUUMin); 
    j += 38;
    MovNByte(4, AllBuf.Spi1 + j,  PnD->DF30.SUUMin); 
    j += 4;
    MovNByte(10, AllBuf.Spi1 + j, PnD->DF31.Vmax);   
    j += 10;
    MovNByte(4, AllBuf.Spi1 + j,  PnD->DF49.PowerT);   
    j += 4;
    MovNByte(4, AllBuf.Spi1 + j,  PnD->DF50.MonthCutNum);   
    j += 4;
    MovNByte(4, AllBuf.Spi1 + j,  (u8*)&ZDS.F10.Flux_Day);  
    j += 4;                    
    MovNByte(11, AllBuf.Spi1 + j,  PnD->DF209.MeterPowerStat); 
    j += 11;
    EW.MoveByte = 0;

    for(; j < 527; j++) //;不用的地址填充为0
    {
        *(AllBuf.Spi1 + j) = 0x00;
    }
    
    SPI_FLASH_PageWrite_E(AllBuf.Spi1, PnMVTHome);
}



/* 
********************************************************************************
* 函 数 名：PEZD_Save
* 功    能：保存当日的零点电量，用于计算日电量
* 参    数：Pn-信息点；Date-电量数据
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void PEZD_Save(u8 Pn, u8 *Date)
{
    u16 j, PnPEZHome;
    u32 *Addr32;
    struct PointDataStr *PnD;
    
    Addr32 = (u32*)(*(PnDTAddr + Pn));
    PnD = (struct PointDataStr*)(*(Addr32 + _PnD));
    PnPEZHome = (u16)(*(Addr32 + _PnPEZHome));
    
    SPI_FLASH_PageRead(AllBuf.Spi1, PnPEZHome);  
    
    EW.MoveByte = 'M';
    j = 0;    
    *(AllBuf.Spi1 + j++) = 'P';  //;保存上一日日末电能量示值
    *(AllBuf.Spi1 + j++) = 'E';
    *(AllBuf.Spi1 + j++) = 'Z';
    *(AllBuf.Spi1 + j++) = 'D';
    *(AllBuf.Spi1 + j++) = *(Date + 0);
    *(AllBuf.Spi1 + j++) = *(Date + 1);
    *(AllBuf.Spi1 + j++) = *(Date + 2);
    MovNByte(90, AllBuf.Spi1 + j, PnD->Z9010_D);
    j += 90; 
    SPI_FLASH_PageWrite_E(AllBuf.Spi1, PnPEZHome);
}


/* 
********************************************************************************
* 函 数 名：PEZM_Save
* 功    能：保存当月的零点电量，用于计算月电量
* 参    数：Pn-信息点；Date-电量数据
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*
void PEZM_Save(u8 Pn, u8 *Date)
{
    u16 j, PnPEZHome;
    u32 *Addr32;
    struct PointDataStr *PnD;

    Addr32 = (u32*)(*(PnDTAddr + Pn));
    PnD = (struct PointDataStr*)(*(Addr32 + _PnD));
    PnPEZHome = (u16)(*(Addr32 + _PnPEZHome));

    SPI_FLASH_PageRead(AllBuf.Spi1, PnPEZHome);
    
    EW.MoveByte = 'M';    
    j = 97;
    *(AllBuf.Spi1 + j++) = 'P';
    *(AllBuf.Spi1 + j++) = 'E';
    *(AllBuf.Spi1 + j++) = 'Z';
    *(AllBuf.Spi1 + j++) = 'M';
    *(AllBuf.Spi1 + j++) = *(Date + 0);
    *(AllBuf.Spi1 + j++) = *(Date + 1);   
    MovNByte(90, AllBuf.Spi1 + j, PnD->Z9010_M);
    j += 90;
    EW.MoveByte = 0;
    for (; j < 527; j++) //;不用的地址填充为0
    {
        *(AllBuf.Spi1 + j) = 0x00;
    }  
    SPI_FLASH_PageWrite_E(AllBuf.Spi1, PnPEZHome);
}
*/
/* 
********************************************************************************
* 函 数 名：PEZM_Save
* 功    能：保存当月的零点电量，用于计算月电量，电表操作记录(zrt20130815)
* 参    数：Pn-信息点；Date-电量数据
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void PEZM_Save(u8 Pn, u8 *Date)
{
    u16 j, PnPEZHome;
    u32 *Addr32;
    struct PointDataStr *PnD;

    Addr32 = (u32*)(*(PnDTAddr + Pn));
    PnD = (struct PointDataStr*)(*(Addr32 + _PnD));
    PnPEZHome = (u16)(*(Addr32 + _PnPEZHome));

    SPI_FLASH_PageRead(AllBuf.Spi1, PnPEZHome);
    
    EW.MoveByte = 'M';    
    j = 97;
    *(AllBuf.Spi1 + j++) = 'P';
    *(AllBuf.Spi1 + j++) = 'E';
    *(AllBuf.Spi1 + j++) = 'Z';
    *(AllBuf.Spi1 + j++) = 'M';
    *(AllBuf.Spi1 + j++) = *(Date + 0);
    *(AllBuf.Spi1 + j++) = *(Date + 1);   
    MovNByte(90+28, AllBuf.Spi1 + j, PnD->Z9010_M);
    j += (90+28);
    EW.MoveByte = 0;
    for (; j < 527; j++) //;不用的地址填充为0
    {
        *(AllBuf.Spi1 + j) = 0x00;
    }  
    SPI_FLASH_PageWrite_E(AllBuf.Spi1, PnPEZHome);
}


/* 
********************************************************************************
* 函 数 名：PEZ_Read
* 功    能：读出零点电能,如果没有读到，就将当前值作为零点电能，电表操作记录(zrt20130815)
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*
void PEZ_Read(void)
{
    u8 i, PEZBZ[4], Year, Month, Day;
    u16 j, PnPEZHome;
    u32 *Addr32;
    struct PointDataStr *PnD;

    for(i = 0; i < PNMAXNUM; i++)
    {
        Addr32 = (u32*)(*(PnDTAddr + i));
        PnD = (struct PointDataStr*)(*(Addr32 + _PnD));
        PnPEZHome = (u16)(*(Addr32 + _PnPEZHome));
        SPI_FLASH_PageRead(AllBuf.Spi1, PnPEZHome); 
        j = 0;
        PEZBZ[0] = *(AllBuf.Spi1  + j++);     
        PEZBZ[1] = *(AllBuf.Spi1  + j++);
        PEZBZ[2] = *(AllBuf.Spi1  + j++);
        PEZBZ[3] = *(AllBuf.Spi1  + j++);        
        if((PEZBZ[0] == 'P') && (PEZBZ[1] == 'E') && (PEZBZ[2] == 'Z') && (PEZBZ[3] == 'D'))
        {
            Day   = *(AllBuf.Spi1 + j++);
            Month = *(AllBuf.Spi1 + j++);
            Year  = *(AllBuf.Spi1 + j++);               
            if ((Day == TBCD.Day) && (Month == TBCD.Month) && (Year == TBCD.Year))
            {
                EW.MoveByte = 'M';
                MovNByte(90, PnD->Z9010_D,  AllBuf.Spi1 + j);
                EW.MoveByte = 0;
            }
        }
        
        j = 97;
        PEZBZ[0] = *(AllBuf.Spi1  + j++);     
        PEZBZ[1] = *(AllBuf.Spi1  + j++);
        PEZBZ[2] = *(AllBuf.Spi1  + j++);
        PEZBZ[3] = *(AllBuf.Spi1  + j++);        
        if((PEZBZ[0] == 'P') && (PEZBZ[1] == 'E') && (PEZBZ[2] == 'Z') && (PEZBZ[3] == 'M'))
        {
            Month = *(AllBuf.Spi1 + j++);
            Year  = *(AllBuf.Spi1 + j++);                   
            if((Month == TBCD.Month) && (Year == TBCD.Year))
            {
                EW.MoveByte = 'M';
                MovNByte(90, PnD->Z9010_M,  AllBuf.Spi1 + j);
                EW.MoveByte = 0;
            }
        }  
    }
}
*/

void PEZ_Read(void)
{
    u8 i, PEZBZ[4], Year, Month, Day;
    u16 j, PnPEZHome;
    u32 *Addr32;
    struct PointDataStr *PnD;

    for(i = 0; i < PNMAXNUM; i++)
    {
        Addr32 = (u32*)(*(PnDTAddr + i));
        PnD = (struct PointDataStr*)(*(Addr32 + _PnD));
        PnPEZHome = (u16)(*(Addr32 + _PnPEZHome));
        SPI_FLASH_PageRead(AllBuf.Spi1, PnPEZHome); 
        j = 0;
        PEZBZ[0] = *(AllBuf.Spi1  + j++);     
        PEZBZ[1] = *(AllBuf.Spi1  + j++);
        PEZBZ[2] = *(AllBuf.Spi1  + j++);
        PEZBZ[3] = *(AllBuf.Spi1  + j++);        
        if((PEZBZ[0] == 'P') && (PEZBZ[1] == 'E') && (PEZBZ[2] == 'Z') && (PEZBZ[3] == 'D'))
        {
            Day   = *(AllBuf.Spi1 + j++);
            Month = *(AllBuf.Spi1 + j++);
            Year  = *(AllBuf.Spi1 + j++);               
            if ((Day == TBCD.Day) && (Month == TBCD.Month) && (Year == TBCD.Year))
            {
                EW.MoveByte = 'M';
                MovNByte(90, PnD->Z9010_D,  AllBuf.Spi1 + j);
                EW.MoveByte = 0;
            }
        }
        
        j = 97;
        PEZBZ[0] = *(AllBuf.Spi1  + j++);     
        PEZBZ[1] = *(AllBuf.Spi1  + j++);
        PEZBZ[2] = *(AllBuf.Spi1  + j++);
        PEZBZ[3] = *(AllBuf.Spi1  + j++);        
        if((PEZBZ[0] == 'P') && (PEZBZ[1] == 'E') && (PEZBZ[2] == 'Z') && (PEZBZ[3] == 'M'))
        {
            Month = *(AllBuf.Spi1 + j++);
            Year  = *(AllBuf.Spi1 + j++);                   
            if((Month == TBCD.Month) && (Year == TBCD.Year))
            {
                EW.MoveByte = 'M';
                MovNByte(90+28, PnD->Z9010_M,  AllBuf.Spi1 + j);
                EW.MoveByte = 0;
            }
        }  
    }
}

/* 
********************************************************************************
* 函 数 名：FindIndex_DJ
* 功    能：找出当前冻结数据要存放的顺序号
* 参    数：Type:==0,日冻结；==1,抄表日冻结
* 返    回：冻结数据存放的序号
* 说    明：时间向前突变的时候把顺序号也向前改变
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
u8 FindIndex_DJ(u8 Pn, u8 Type, u8 *Date)
{
    u8 Ind_Low, Ind_High, Ind_Mid, compare;
    u8 T_Adr[3], DJPointNum, PointPage ;
    u16 temp, page, PnDJHome;
    u32 *Addr32;
    struct DJ_Control *PnDJ;
    
    Addr32 =(u32*)(*(PnDTAddr + Pn));
    if(Type == 0) //;日冻结
    {
        PnDJ = (struct DJ_Control*)(*(Addr32 + _PnDDJ));        
        PnDJHome = (u16)(*(Addr32 + _PnDDJHome));
        DJPointNum = DDJPointNum; //;总共存的点数
        PointPage = DDJPointPage; //;每点存页数
    }
    else if(Type == 1) //;抄表日冻结
    {
        PnDJ = (struct DJ_Control*)(*(Addr32 + _PnBDJ));        
        PnDJHome = (u16)(*(Addr32 + _PnBDJHome));
        DJPointNum = BDJPointNum;
        PointPage = BDJPointPage; 
    }
   
    if(PnDJ->Index == PnDJ->StInd)
    {
        return PnDJ->Index;//;最后点与开始点相同只有在都为0的情况下成立
    }
    
    temp = (PnDJ->Index - 1) % DJPointNum;//;在PointNum中的第几点
    page = temp * PointPage; //;在测量点保存区内的页码
    page += PnDJHome; //;整个Flash中的页码
    EW.RFlashNByte = 'R'; 
    SPI_FLASH_ReadNByte(T_Adr, page, 0, 3);//;读出任务序列的最后一个时间
    EW.RFlashNByte = 0;
    
    compare = TimeCompare_3Byte(Date, T_Adr);
    if(compare == back)
    {
        return PnDJ->Index;
    }
    else if(compare == same)
    {
        return PnDJ->Index - 1;
    }
    
    temp = PnDJ->StInd % DJPointNum;
    page = temp * PointPage; 
    page += PnDJHome;
    EW.RFlashNByte = 'R';
    SPI_FLASH_ReadNByte(T_Adr, page, 0, 3);
    EW.RFlashNByte = 0;
    
    compare = TimeCompare_3Byte(Date, T_Adr);
    if(compare != back)
    {
        return PnDJ->StInd;
    }
    
    Ind_Low = PnDJ->StInd;
    Ind_High = PnDJ->Index - 1;
    while(Ind_High >= Ind_Low + 2)
    {
        Ind_Mid = (Ind_Low + Ind_High) / 2;
        temp = Ind_Mid % DJPointNum; 
        page = temp * PointPage; 
        page += PnDJHome; 
        EW.RFlashNByte = 'R';
        SPI_FLASH_ReadNByte(T_Adr, page, 0, 3);
        EW.RFlashNByte = 0;
        
        compare = TimeCompare_3Byte(Date, T_Adr);
        if(compare == ahead)
        {
            Ind_High = Ind_Mid;
        } 
        else if(compare == back)
        {
            Ind_Low = Ind_Mid;
        } 
        else if(compare == same)
        {
            return Ind_Mid; 
        }
    }
    return Ind_High;
}



/* 
********************************************************************************
* 函 数 名：FindIndex_MDJ
* 功    能：找出当前冻结数据要存放的顺序号
* 参    数：Pn-信息点；Date-冻结日期
* 返    回：冻结数据存放的序号 
* 说    明：时间向前突变的时候把顺序号也向前改变
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
u8 FindIndex_MDJ(u8 Pn, u8 * Date)
{
    u8 Ind_Low, Ind_High, Ind_Mid, compare;
    u8 T_Adr[2], DJPointNum, PointPage ;
    u16 temp, page, PnDJHome;
    u32 *Addr32;
    struct DJ_Control *PnDJ;
    
    Addr32 =(u32*)(*(PnDTAddr + Pn));
    
    PnDJ = (struct DJ_Control*)(*(Addr32 + _PnMDJ));        
    PnDJHome = (u16)(*(Addr32 + _PnMDJHome));
    DJPointNum = MDJPointNum;
    PointPage = MDJPointPage; 

    if(PnDJ->Index == PnDJ->StInd)
    {
        return PnDJ->Index;//;最后点与开始点相同只有在都为0的情况下成立
    }
    
    temp = (PnDJ->Index - 1) % DJPointNum;//;在PointNum中的第几点
    page = temp * PointPage; //;在测量点保存区内的页码
    page += PnDJHome; //;整个Flash中的页码
    EW.RFlashNByte = 'R'; 
    SPI_FLASH_ReadNByte(T_Adr, page, 0, 2);//;读出任务序列的最后一个时间
    EW.RFlashNByte = 0;
    compare = TimeCompare_2Byte(Date, T_Adr);
    if(compare == back)
    {
        return PnDJ->Index;
    }
    else if(compare == same)
    {
        return (PnDJ->Index - 1);
    }
    
    temp = PnDJ->StInd % DJPointNum;
    page = temp * PointPage; 
    page += PnDJHome;
    EW.RFlashNByte = 'R';
    SPI_FLASH_ReadNByte(T_Adr, page, 0, 2);
    EW.RFlashNByte = 0;
    compare = TimeCompare_2Byte(Date, T_Adr);
    if(compare != back)
    {
        return PnDJ->StInd;
    }
    
    Ind_Low = PnDJ->StInd;
    Ind_High = PnDJ->Index - 1;
    while (Ind_High >= Ind_Low + 2)
    {
        Ind_Mid = (Ind_Low + Ind_High) / 2;
        temp = Ind_Mid % DJPointNum; 
        page = temp * PointPage; 
        page += PnDJHome; 
        
        EW.RFlashNByte = 'R';
        SPI_FLASH_ReadNByte(T_Adr, page, 0, 2);
        EW.RFlashNByte = 0;
        compare = TimeCompare_2Byte(Date, T_Adr);
        if(compare == ahead)
        {
            Ind_High = Ind_Mid;
        } 
        else if(compare == back)
        {
            Ind_Low = Ind_Mid;
        } 
        else if(compare == same)
        {
            return Ind_Mid;
        }
    }
    return Ind_High;
}



/* 
********************************************************************************
* 函 数 名：FindIndex_CDJ
* 功    能：找出当前冻结数据要存放的顺序号
* 参    数：Pn-信息点
* 返    回：数据存放序号
* 说    明：时间向前突变的时候把顺序号也向前改变
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
u16 FindIndex_CDJ(u8 Pn)
{
    u8 area_ind, PagePoint, AreaNum, compare;
    u8 T_Adr[5] ;
    u16 Ind_Low, Ind_High, Ind_Mid;
    u16 temp, page, PnDJHome, DJPointNum;
    u32 *Addr32;
    struct QX_Control *PnDJ;
    
    Addr32 =(u32*)(*(PnDTAddr + Pn));
    PnDJ = (struct QX_Control*)(*(Addr32 + _PnQDJ));  
    PnDJHome = (u16)(*(Addr32 + _PnQDJHome));
    DJPointNum = QDJPointNum;   //;曲线冻结总共的点
    PagePoint = QDJPagePoint; //;每页存的点
    AreaNum = QDJAreaNum;     //;每点存的字节
    
    if(PnDJ->Index == PnDJ->StInd)
    {
        return PnDJ->Index;
    }
    
    temp = (PnDJ->Index - 1) % DJPointNum; 
    page = temp / PagePoint;
    page += PnDJHome;
    area_ind = temp % PagePoint; //;在该页内的第几个区
    EW.RFlashNByte = 'R'; //;用于保护
    SPI_FLASH_ReadNByte(T_Adr, page, area_ind * AreaNum, 5); //;读出任务序列的最后一个时间
    EW.RFlashNByte = 0;
    compare = TimeCompare_5Byte(&TBCD.Min, T_Adr);
    if(compare == back)
    {
        return PnDJ->Index;
    }
    else if(compare == same)
    {
        return (PnDJ->Index - 1);
    }
    
    temp = PnDJ->StInd % DJPointNum;
    page = temp / PagePoint;
    page += PnDJHome;
    area_ind = temp % PagePoint;
    EW.RFlashNByte = 'R'; 
    SPI_FLASH_ReadNByte(T_Adr, page, area_ind * AreaNum, 5);
    EW.RFlashNByte = 0;
    
    compare = TimeCompare_5Byte(&TBCD.Min, T_Adr);
    if(compare != back)
    {
        return PnDJ->StInd;
    }
    
    Ind_Low = PnDJ->StInd;
    Ind_High = PnDJ->Index - 1;
    while(Ind_High >= Ind_Low + 2)
    {
        
        Ind_Mid = (Ind_Low + Ind_High) / 2;
        temp = Ind_Mid % DJPointNum; 
        page = temp / PagePoint; 
        page += PnDJHome;
        area_ind = temp % PagePoint; 
        EW.RFlashNByte = 'R'; 
        SPI_FLASH_ReadNByte(T_Adr, page, area_ind * AreaNum, 5);
        EW.RFlashNByte = 0;
        compare = TimeCompare_5Byte(&TBCD.Min, T_Adr);
        if(compare == ahead)
        {
            Ind_High = Ind_Mid;
        }
        else if(compare == back)
        {
            Ind_Low = Ind_Mid;
        }
        else if(compare == same)
        {
            return Ind_Mid;
        }
    }
    return Ind_High;
}


/* 
********************************************************************************
* 函 数 名：DDJ_Save
* 功    能：把日冻结数据保存到Flash中
* 参    数：Pn-信息点；Date-冻结日期
* 返    回：无
* 说    明：日冻结数据一共保存100天,每天1个点，保存2页，共用200页
*           Flash中的地址由 P1DDJ.Index 决定，数据保存完以后将影响这个值
*           并且把这个值保存到FM24C04中去
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void DDJ_Save(u8 Pn, u8 *Date)
{
    u8  Index, StInd, MaxPo, Points, flag ;
    u16 page, j, PnDJHome, E2R_PnDJ;
    u32 *Addr32;
    struct PointDataStr *PnD;
    struct DJ_Control   *PnDJ;

    Addr32 = (u32*)(*(PnDTAddr + Pn));
    PnD = (struct PointDataStr*)(*(Addr32 + _PnD));
    PnDJ = (struct DJ_Control*)(*(Addr32 + _PnDDJ));
    PnDJHome = (u16)(*(Addr32 + _PnDDJHome));
    E2R_PnDJ =(u16)(*(Addr32 + _E2R_PnDDJ));
    
    PnDJ->Index = FindIndex_DJ(Pn, 0, Date);//;找日冻结

    j = 0;
    *(AllBuf.Spi1 + j++) = *(Date + 0);
    *(AllBuf.Spi1 + j++) = *(Date + 1);
    *(AllBuf.Spi1 + j++) = *(Date + 2);
    EW.MoveByte = 'M';
    MovNByte(5, AllBuf.Spi1 + j, PnD->CB_Time); //;保存抄表时间
    j += 5;
    *(AllBuf.Spi1 + j++) = 4;

    *(AllBuf.Spi1 + j++) = DDLDJPn.Status;	
//	j++;	//;保留一个字节的空间 //;20130926
    //;F1
    MovNByte(25, AllBuf.Spi1 + j, PnD->R9010);
    j += 25;
    MovNByte(20, AllBuf.Spi1 + j, PnD->R9110);
    j += 20;
    MovNByte(20, AllBuf.Spi1 + j, PnD->R9130);
    j += 20;
    MovNByte(20, AllBuf.Spi1 + j, PnD->R9140);
    j += 20;
    //;F2
    MovNByte(25, AllBuf.Spi1 + j, PnD->R9020);
    j += 25;
    MovNByte(20, AllBuf.Spi1 + j, PnD->R9120);
    j += 20;
    MovNByte(20, AllBuf.Spi1 + j, PnD->R9150);
    j += 20;
    MovNByte(20, AllBuf.Spi1 + j, PnD->R9160);
    j += 20;
    //;F3
    MovNByte(15, AllBuf.Spi1 + j, PnD->RA010);
    j += 15;
    MovNByte(20, AllBuf.Spi1 + j, PnD->RB010);
    j += 20;
    MovNByte(15, AllBuf.Spi1 + j, PnD->RA110);
    j += 15;
    MovNByte(20, AllBuf.Spi1 + j, PnD->RB110);
    j += 20;
    //;F4
    MovNByte(15, AllBuf.Spi1 + j, PnD->RA020);
    j += 15;
    MovNByte(20, AllBuf.Spi1 + j, PnD->RB020);
    j += 20;
    MovNByte(15, AllBuf.Spi1 + j, PnD->RA120);
    j += 15;
    MovNByte(20, AllBuf.Spi1 + j, PnD->RB120);
    j += 20;
    //;F5
    MovNByte(20, AllBuf.Spi1 + j, PnD->R9010_D);
    j += 20;
    //;F6
    MovNByte(20, AllBuf.Spi1 + j, PnD->R9110_D);
    j += 20;
    //;F7
    MovNByte(20, AllBuf.Spi1 + j, PnD->R9020_D);
    j += 20;
    //;F8
    MovNByte(20, AllBuf.Spi1 + j, PnD->R9120_D);
    j += 20;
    //;F25
    MovNByte(32, AllBuf.Spi1 + j, PnD->DF25.ZPMax);
    j += 32;
    //;F26
    MovNByte(24, AllBuf.Spi1 + j, PnD->DF26.ZPMN);
    j += 24;
    //;F27
    MovNByte(66, AllBuf.Spi1 + j, PnD->DF27.AVUUMin);
    j += 66;    
    
    for (; j < 528; j++)  //;不用的地址填充为0
    {
        *(AllBuf.Spi1 + j) = 0x00;
    }

    page = PnDJ->Index % DDJPointNum; //;page为实际的序号
    page *= DDJPointPage;
    page += PnDJHome; //;page为实际对应的页码中的第一页
    SPI_FLASH_PageWrite_E(AllBuf.Spi1, page);

    j = 0;
    *(AllBuf.Spi1 + j++) = *(Date + 0);
    *(AllBuf.Spi1 + j++) = *(Date + 1);
    *(AllBuf.Spi1 + j++) = *(Date + 2);
    MovNByte(5, AllBuf.Spi1 + j, PnD->CB_Time); //;保存抄表时间
    j += 5;

    *(AllBuf.Spi1 + j++) = 4; //;费率数
    
    MovNByte(14, AllBuf.Spi1 + j, PnD->DF28.IImbMin);//;F28
    j += 14; 
    MovNByte(38, AllBuf.Spi1 + j, PnD->DF29.AIUUMin);//;F29
    j += 38;  
    MovNByte(4, AllBuf.Spi1 + j,  PnD->DF30.SUUMin);//;F30
    j += 4;
    MovNByte(10, AllBuf.Spi1 + j, PnD->DF31.Vmax); //;F31
    j += 10;   
    MovNByte(52, AllBuf.Spi1 + j, PnD->RB310); //;F32
    j += 52;
    MovNByte(4, AllBuf.Spi1 + j, PnD->DF49.PowerT); //;F49
    j += 4;   
    MovNByte(4, AllBuf.Spi1 + j, PnD->DF50.MonthCutNum); //;F50
    j += 4;   
    MovNByte(4, AllBuf.Spi1 + j, (u8*)&ZDS.F10.Flux_Day); //;F53
    j += 4;  
    //;F153
    MovNByte(5, AllBuf.Spi1 + j, PnD->R30A1); 
    j += 5; 
    MovNByte(5, AllBuf.Spi1 + j, PnD->R30B1); 
    j += 5;  
    MovNByte(5, AllBuf.Spi1 + j, PnD->R30C1); 
    j += 5;
     //;F154
    MovNByte(4, AllBuf.Spi1 + j, PnD->R30A3); 
    j += 4; 
    MovNByte(4, AllBuf.Spi1 + j, PnD->R30B3); 
    j += 4;  
    MovNByte(4, AllBuf.Spi1 + j, PnD->R30C3); 
    j += 4;
     //;F155
    MovNByte(5, AllBuf.Spi1 + j, PnD->R30A2); 
    j += 5; 
    MovNByte(5, AllBuf.Spi1 + j, PnD->R30B2); 
    j += 5;  
    MovNByte(5, AllBuf.Spi1 + j, PnD->R30C2); 
    j += 5;
     //;F156
    MovNByte(4, AllBuf.Spi1 + j, PnD->R30A4); 
    j += 4; 
    MovNByte(4, AllBuf.Spi1 + j, PnD->R30B4); 
    j += 4;  
    MovNByte(4, AllBuf.Spi1 + j, PnD->R30C4); 
    j += 4;
    
    MovNByte(11, AllBuf.Spi1 + j, PnD->DF209.MeterPowerStat); //;F209
    j += 11;   
    for (; j < 528; j++)
    //;不用的地址填充为0
    {
        *(AllBuf.Spi1 + j) = 0x00;
    }

    page = PnDJ->Index % DDJPointNum; //;page为实际的序号 --@6
    page *= DDJPointPage;
    page += PnDJHome;

    page++; //;page为实际对应的页码中的第二页
    
    SPI_FLASH_PageWrite_E(AllBuf.Spi1, page);

    /*新算法保存序列*/
    Index = PnDJ->Index;//;针尾
    StInd = PnDJ->StInd;//;针头
    MaxPo = DDJPointNum;//;最大点数
    Points = 1;         //;步长
    Index++;
    if((Index - StInd) >= MaxPo)
    {
        StInd = Index - MaxPo + Points;
        flag = (u8)(StInd % Points);
        StInd -= flag;
    }
    if(StInd >= MaxPo)
    {
        StInd -= MaxPo;
        Index -= MaxPo;
    }
    PnDJ->Index = Index;//;针尾
    PnDJ->StInd = StInd;//;针头
    SaveData_FM24(2, E2R_PnDJ, E2R_PnDJ + E2R_ByteNum, &PnDJ->Index);
    EW.MoveByte = 0;
}


/* 
********************************************************************************
* 函 数 名：BDJ_Save
* 功    能：把抄表日冻结数据保存到Flash中
* 参    数：Pn-信息点；Date-冻结日期
* 返    回：无
* 说    明：日冻结数据一共保存100天,每天1个点，保存2页，共用200页
*           Flash中的地址由 P1DDJ.Index 决定，数据保存完以后将影响这个值
*           并且把这个值保存到FM24C04中去
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void BDJ_Save(u8 Pn, u8 *Date)
{
    u8  Index, StInd, MaxPo, Points, flag;
    u16 page, j, PnDJHome, E2R_PnDJ;
    u32 *Addr32;
    struct PointDataStr *PnD;
    struct DJ_Control   *PnDJ;
    
    Addr32 = (u32*)(*(PnDTAddr + Pn));
    PnD = (struct PointDataStr*)(*(Addr32 + _PnD));
    PnDJ = (struct DJ_Control*)(*(Addr32 + _PnBDJ));
    PnDJHome = (u16)(*(Addr32 + _PnBDJHome));
    E2R_PnDJ =(u16)(*(Addr32 + _E2R_PnBDJ));
    
    PnDJ->Index = FindIndex_DJ(Pn, 1, Date);//;找抄表日冻结

    j = 0;
    *(AllBuf.Spi1 + j++) = *(Date + 0);
    *(AllBuf.Spi1 + j++) = *(Date + 1);
    *(AllBuf.Spi1 + j++) = *(Date + 2);
    EW.MoveByte = 'M';
    MovNByte(5, AllBuf.Spi1 + j, PnD->CB_Time); //;保存抄表时间
    j += 5;
    *(AllBuf.Spi1 + j++) = 4;
    //;*(AllBuf.Spi1 + j++) = PnD->RC313[0];       //;费率数
    //;F9
    MovNByte(25, AllBuf.Spi1 + j, PnD->R9010);
    j += 25;
    MovNByte(20, AllBuf.Spi1 + j, PnD->R9110);
    j += 20;
    MovNByte(20, AllBuf.Spi1 + j, PnD->R9130);
    j += 20;
    MovNByte(20, AllBuf.Spi1 + j, PnD->R9140);
    j += 20;
    //;F10
    MovNByte(25, AllBuf.Spi1 + j, PnD->R9020);
    j += 25;
    MovNByte(20, AllBuf.Spi1 + j, PnD->R9120);
    j += 20;
    MovNByte(20, AllBuf.Spi1 + j, PnD->R9150);
    j += 20;
    MovNByte(20, AllBuf.Spi1 + j, PnD->R9160);
    j += 20;
    //;F11
    MovNByte(15, AllBuf.Spi1 + j, PnD->RA010);
    j += 15;
    MovNByte(20, AllBuf.Spi1 + j, PnD->RB010);
    j += 20;
    MovNByte(15, AllBuf.Spi1 + j, PnD->RA110);
    j += 15;
    MovNByte(20, AllBuf.Spi1 + j, PnD->RB110);
    j += 20;
    //;F12
    MovNByte(15, AllBuf.Spi1 + j, PnD->RA020);
    j += 15;
    MovNByte(20, AllBuf.Spi1 + j, PnD->RB020);
    j += 20;
    MovNByte(15, AllBuf.Spi1 + j, PnD->RA120);
    j += 15;
    MovNByte(20, AllBuf.Spi1 + j, PnD->RB120);
    j += 20;
    for (; j < 528; j++)
    {
        *(AllBuf.Spi1 + j) = 0x00;
    }
    page = PnDJ->Index % BDJPointNum;
    page *= BDJPointPage;
    page += PnDJHome;               //;page为实际对应的页码中的第一页   
    SPI_FLASH_PageWrite_E(AllBuf.Spi1, page);
    /*---------------新算法保存序列------------*/
    Index  =  PnDJ->Index; //;针尾
    StInd  =  PnDJ->StInd; //;针头
    MaxPo  =  DDJPointNum; //;最大点数
    Points =  1;           //;步长

    Index++;
    if((Index - StInd) >= MaxPo)
    {
        StInd = Index - MaxPo + Points;
        flag = (u8)(StInd % Points);
        StInd -= flag;
    }
    if(StInd >= MaxPo)
    {
        StInd -= MaxPo;
        Index -= MaxPo;
    }
    PnDJ->Index = Index;   //;针尾
    PnDJ->StInd = StInd;   //;针头
    /*-----------------------------------------*/
    SaveData_FM24(2, E2R_PnDJ, E2R_PnDJ + E2R_ByteNum, &PnDJ->Index);
    EW.MoveByte = 0;
}


/* 
********************************************************************************
* 函 数 名：BDJ_Save
* 功    能：把月冻结数据保存到Flash中
* 参    数：Pn-信息点；Date-冻结日期
* 返    回：无 
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void MDJ_Save(u8 Pn, u8 *Date)
{
    u8  Index, StInd, MaxPo, Points, flag ;
    u16 page, j, PnDJHome, E2R_PnDJ;
    u32 *Addr32;
    struct PointDataStr *PnD;
    struct DJ_Control   *PnDJ;
    
    Addr32 = (u32*)(*(PnDTAddr + Pn));
    PnD = (struct PointDataStr*)(*(Addr32 + _PnD));
    PnDJ = (struct DJ_Control*)(*(Addr32 + _PnMDJ));
    PnDJHome = (u16)(*(Addr32 + _PnMDJHome));
    E2R_PnDJ =(u16)(*(Addr32 + _E2R_PnMDJ));
    
    PnDJ->Index = FindIndex_MDJ(Pn, Date);

    j = 0;
    *(AllBuf.Spi1 + j++) = *(Date + 0);
    *(AllBuf.Spi1 + j++) = *(Date + 1);
    EW.MoveByte = 'M';
    MovNByte(5, AllBuf.Spi1 + j, PnD->CB_Time); //;保存抄表时间
    j += 5;
    *(AllBuf.Spi1 + j++) = 4;

    *(AllBuf.Spi1 + j++) = MDLDJPn.Status;	
//	j++;	//;保留一个字节的空间 //;20130926
	
    //;*(AllBuf.Spi1 + j++) = PnD->RC313[0]; //;费率数
    //;F17
    MovNByte(25, AllBuf.Spi1 + j, PnD->R9010);
    j += 25;
    MovNByte(20, AllBuf.Spi1 + j, PnD->R9110);
    j += 20;
    MovNByte(20, AllBuf.Spi1 + j, PnD->R9130);
    j += 20;
    MovNByte(20, AllBuf.Spi1 + j, PnD->R9140);
    j += 20;
    //;F18
    MovNByte(25, AllBuf.Spi1 + j, PnD->R9020);
    j += 25;
    MovNByte(20, AllBuf.Spi1 + j, PnD->R9120);
    j += 20;
    MovNByte(20, AllBuf.Spi1 + j, PnD->R9150);
    j += 20;
    MovNByte(20, AllBuf.Spi1 + j, PnD->R9160);
    j += 20;
    //;F19
    MovNByte(15, AllBuf.Spi1 + j, PnD->RA010);
    j += 15;
    MovNByte(20, AllBuf.Spi1 + j, PnD->RB010);
    j += 20;
    MovNByte(15, AllBuf.Spi1 + j, PnD->RA110);
    j += 15;
    MovNByte(20, AllBuf.Spi1 + j, PnD->RB110);
    j += 20;
    //;F20
    MovNByte(15, AllBuf.Spi1 + j, PnD->RA020);
    j += 15;
    MovNByte(20, AllBuf.Spi1 + j, PnD->RB020);
    j += 20;
    MovNByte(15, AllBuf.Spi1 + j, PnD->RA120);
    j += 15;
    MovNByte(20, AllBuf.Spi1 + j, PnD->RB120);
    j += 20;
    //;F21
    MovNByte(20, AllBuf.Spi1 + j, PnD->R9010_M);
    j += 20;
    //;F22
    MovNByte(20, AllBuf.Spi1 + j, PnD->R9110_M);
    j += 20;
    //;F23
    MovNByte(20, AllBuf.Spi1 + j, PnD->R9020_M);
    j += 20;
    //;F24
    MovNByte(20, AllBuf.Spi1 + j, PnD->R9120_M);
    j += 20;
    //;F33
    MovNByte(32, AllBuf.Spi1 + j, PnD->DF33.ZPMax);
    j += 32;
    //;F34
    MovNByte(24, AllBuf.Spi1 + j, PnD->DF34.ZPMN);
    j += 24;
    //;F35
    MovNByte(66, AllBuf.Spi1 + j, PnD->DF35.AVUUMin);
    j += 66;
    
    for (; j < 528; j++)//;不用的地址填充为0
    {
        *(AllBuf.Spi1 + j) = 0x00;
    }

    page = PnDJ->Index % MDJPointNum; //;page为实际的序号
    page *= MDJPointPage;
    page += PnDJHome; //;page为实际对应的页码中的第一页
    SPI_FLASH_PageWrite_E(AllBuf.Spi1, page);

    j = 0;
    *(AllBuf.Spi1 + j++) = *(Date + 0);
    *(AllBuf.Spi1 + j++) = *(Date + 1);
    MovNByte(5, AllBuf.Spi1 + j, PnD->CB_Time); //;保存抄表时间
    j += 5;

    *(AllBuf.Spi1 + j++) = 4; //;费率数

    MovNByte(16, AllBuf.Spi1 + j, PnD->DF36.IImbMin);//;F36
    j += 16; 
    MovNByte(38, AllBuf.Spi1 + j, PnD->DF37.AIUUMin);//;F37
    j += 38;  
    MovNByte(4, AllBuf.Spi1 + j,  PnD->DF38.SUUMin);//;F38
    j += 4;
    MovNByte(12, AllBuf.Spi1 + j, PnD->DF39.Vmax); //;F39
    j += 12;     
    MovNByte(4, AllBuf.Spi1 + j, PnD->DF51.PowerT); //;F51
    j += 4;   
    MovNByte(4, AllBuf.Spi1 + j, PnD->DF52.MonthCutNum); //;F52
    j += 4;   
    MovNByte(4, AllBuf.Spi1 + j, (u8*)&ZDS.F10.Flux_Month); //;F54
    j += 4;  
    //;F157
    MovNByte(5, AllBuf.Spi1 + j, PnD->R30A1); 
    j += 5; 
    MovNByte(5, AllBuf.Spi1 + j, PnD->R30B1); 
    j += 5;  
    MovNByte(5, AllBuf.Spi1 + j, PnD->R30C1); 
    j += 5;
     //;F158
    MovNByte(4, AllBuf.Spi1 + j, PnD->R30A3); 
    j += 4; 
    MovNByte(4, AllBuf.Spi1 + j, PnD->R30B3); 
    j += 4;  
    MovNByte(4, AllBuf.Spi1 + j, PnD->R30C3); 
    j += 4;
     //;F159
    MovNByte(5, AllBuf.Spi1 + j, PnD->R30A2); 
    j += 5; 
    MovNByte(5, AllBuf.Spi1 + j, PnD->R30B2); 
    j += 5;  
    MovNByte(5, AllBuf.Spi1 + j, PnD->R30C2); 
    j += 5;
     //;F160
    MovNByte(4, AllBuf.Spi1 + j, PnD->R30A4); 
    j += 4; 
    MovNByte(4, AllBuf.Spi1 + j, PnD->R30B4); 
    j += 4;  
    MovNByte(4, AllBuf.Spi1 + j, PnD->R30C4); 
    j += 4;
    //;F201~F208
    MovNByte(25, AllBuf.Spi1 + j, PnD->R5010); 
    j += 25;
    MovNByte(25, AllBuf.Spi1 + j, PnD->R5020); 
    j += 25;
    MovNByte(25, AllBuf.Spi1 + j, PnD->R5030); 
    j += 25;
    MovNByte(25, AllBuf.Spi1 + j, PnD->R5040); 
    j += 25;
    MovNByte(25, AllBuf.Spi1 + j, PnD->R5050); 
    j += 25;
    MovNByte(25, AllBuf.Spi1 + j, PnD->R5060); 
    j += 25;
    MovNByte(25, AllBuf.Spi1 + j, PnD->R5070); 
    j += 25;
    MovNByte(25, AllBuf.Spi1 + j, PnD->R5080); 
    j += 25;
   
    MovNByte(14, AllBuf.Spi1 + j, PnD->DF213.ProgramNum); //;F213
    j += 14;  
    MovNByte(14, AllBuf.Spi1 + j, PnD->DF214.CLKChangeNum); //;F214
    j += 14; 
    for (; j < 528; j++)
    //;不用的地址填充为0
    {
        *(AllBuf.Spi1 + j) = 0x00;
    }

    page = PnDJ->Index % MDJPointNum; //;page为实际的序号 --@6
    page *= MDJPointPage;
    page += PnDJHome; //;page为实际对应的页码中的第一页
    page++;   
    SPI_FLASH_PageWrite_E(AllBuf.Spi1, page);


    /*---------------------------新算法保存序列-----*/
    Index  =  PnDJ->Index; //;针尾
    StInd  =  PnDJ->StInd; //;针头
    MaxPo  =  MDJPointNum; //;最大点数
    Points =  1;           //;步长

    Index++;
    if((Index - StInd) >= MaxPo)
    {
      StInd = Index - MaxPo + Points;
      flag = (u8)(StInd % Points);
      StInd -= flag;
    }
    if(StInd >= MaxPo)
    {
      StInd -= MaxPo;
      Index -= MaxPo;
    }

    PnDJ->Index = Index;     //;针尾
    PnDJ->StInd = StInd;     //;针头
    /*----------------------------------------------*/
    SaveData_FM24(2, E2R_PnDJ, E2R_PnDJ + E2R_ByteNum, &PnDJ->Index);
    EW.MoveByte = 0;
}



/* 
********************************************************************************
* 函 数 名：CDJ_Save_Pre
* 功    能：测量点的曲线数据写到缓冲区(终端为第1个测量点)
* 参    数：Pn-信息点；Date-冻结日期；Buf-曲线数据
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void CDJ_Save_Pre(u8 *Buf, u8 Pn, u8 *Date)
{
    u8 j = 0;
    u32 *Addr32;
    struct PointDataStr *PnD;

    Addr32 =(u32*)(*(PnDTAddr + Pn));
    PnD = (struct PointDataStr*)(*(Addr32 + _PnD));

    EW.MoveByte = 'M';
    *(Buf + j++) = *(Date + 0);
    *(Buf + j++) = *(Date + 1);
    *(Buf + j++) = *(Date + 2);
    *(Buf + j++) = *(Date + 3);
    *(Buf + j++) = *(Date + 4);
    MovNByte(12, Buf + j, PnD->RB630);   //;F81-F84  有功功率
    j += 12;
    MovNByte(12, Buf + j, PnD->RB640);   //;F85-F88  无功功率
    j += 12;
    MovNByte(6,  Buf + j, PnD->RB611); //;F89-F91  电压
    j += 6;
    MovNByte(12,  Buf + j, PnD->RB621);   //;F92-F95  电流零序电流
    j += 12;
    MovNByte(4, Buf + j, PnD->R9010_D);  //;F97
    j += 4;
    MovNByte(4, Buf + j, PnD->R9110_D);  //;F98
    j += 4;
    MovNByte(4, Buf + j, PnD->R9020_D);  //;F99
    j += 4;
    MovNByte(4, Buf + j, PnD->R9120_D);  //;F100
    j += 4;
    MovNByte(4, Buf + j, PnD->R9010 + 1);    //;F101
    j += 4;
    MovNByte(4, Buf + j, PnD->R9110);    //;102
    j += 4;
    MovNByte(4, Buf+  j, PnD->R9020 + 1);    //;103
    j += 4;
    MovNByte(4, Buf + j, PnD->R9120);    //;104
    j += 4;
    MovNByte(8, Buf + j, PnD->RB650);    //;F105-F108
    j += 8;
    MovNByte(12, Buf + j, PnD->RB671);    //;F109-F110
    j += 12;
    MovNByte(4, Buf + j, PnD->R9130);    //;F145
    j += 4;
    MovNByte(4, Buf + j, PnD->R9140);    //;F146
    j += 4;
    MovNByte(4, Buf + j, PnD->R9150);    //;F147
    j += 4;
    MovNByte(4, Buf + j, PnD->R9160);    //;F148
    j += 4;
    
    *(Buf + j++) = 0;
    *(Buf + j++) = 0;
    *(Buf + j++) = 0;
    *(Buf + j++) = 0;
    *(Buf + j++) = 0;
    EW.MoveByte = 0;
}

/* 
********************************************************************************
* 函 数 名：CDJ_Save
* 功    能：把曲线冻结数据保存到Flash中
* 参    数：Pn-信息点；Date-冻结日期
* 返    回：无
* 说    明：Flash中的地址由 P1QX.Index 决定 ，数据保存完以后将影响这个值
*           并且把这个值保存到FM24C04中去
*           88字节一个点，每页保存6个点，共保存9600个点，要1600页
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void CDJ_Save(u8 Pn, u8 *Date)
{
    u8 Points, flag, PagePoint, AreaNum ;
    u16 page, j, temp, ZIndex, DJPointNum;
    u16 Index, StInd, MaxPo, PnDJHome, E2R_PnDJ;
    u32 *Addr32;
//;    struct PointDataStr *PnD;
    struct QX_Control *PnDJ;
    
    Addr32 =(u32*)(*(PnDTAddr + Pn));
//;    PnD = (struct PointDataStr*)(*(Addr32 + _PnD));

//;    /*当前数据存在时才能冻结*/
//;    if ((PnD->CB_Time[0] != 0) || (PnD->CB_Time[1] != 0) || (PnD->CB_Time[2] != 0) ||
//;        (PnD->CB_Time[3] != 0) || (PnD->CB_Time[4] != 0))
//;    {
        PnDJ = (struct QX_Control*)(*(Addr32 + _PnQDJ));
        PnDJHome = (u16)(*(Addr32 + _PnQDJHome));
        E2R_PnDJ =(u16)(*(Addr32 + _E2R_PnQDJ));
        DJPointNum = QDJPointNum; //;曲线冻结总共的点
        PagePoint = QDJPagePoint; //;每页存的点
        AreaNum = QDJAreaNum;     //;每点存的字节

        PnDJ->Index = FindIndex_CDJ(Pn);

        temp = PnDJ->Index % DJPointNum;
        page = temp / PagePoint;
        page += PnDJHome;
        
        ZIndex = temp % PagePoint;
        SPI_FLASH_PageRead(AllBuf.Spi1, page);
        for(j = ZIndex * AreaNum; j < (ZIndex + 1) * AreaNum; j++)
        {
            if(*(AllBuf.Spi1 + j) != 0xff)
            {
                break;
            }
        }
        CDJ_Save_Pre(AllBuf.Spi1 + ZIndex * AreaNum, Pn, Date);
        if (j >= (ZIndex + 1) * AreaNum)  //;已经格式化
        {
            SPI_FLASH_PageWrite_NE(AllBuf.Spi1, page); //;将数据保存到flash中
        }
        else //;还没有格式化
        {
            for (j = (ZIndex + 1) * AreaNum; j < PAGESIZE; j++)
            {
                *(AllBuf.Spi1 + j) = 0xff;
            }
            SPI_FLASH_PageWrite_E(AllBuf.Spi1, page); //;将数据保存到flash中
        }

        /*---------------------------新算法保存序列-----*/
        Index  =  PnDJ->Index; //;针尾
        StInd  =  PnDJ->StInd; //;针头
        MaxPo  =  DJPointNum;  //;最大点数
        Points =  PagePoint;   //;步长

        Index++;
        if((Index - StInd) >= MaxPo)
        {
            StInd = Index - MaxPo + Points;
            flag = (u8)(StInd % Points);
            StInd -= flag;
        }
        if(StInd >= MaxPo)
        {
            StInd -= MaxPo;
            Index -= MaxPo;
        }

        PnDJ->Index = Index;     //;针尾
        PnDJ->StInd = StInd;     //;针头
        /*----------------------------------------------*/
        SaveData_FM24(4, E2R_PnDJ, E2R_PnDJ + E2R_ByteNum, (u8*)(&PnDJ->Index));
//;    }
}


/* 
********************************************************************************
* 函 数 名：ERC_Save
* 功    能：保存的事件点数 264点 每页8点 共用33页
* 参    数：Len-事件数据长度；Data_Add-数据的起始地址
*             ERC-记录事件类型  P1ERC1 为重要事件   P1ERC2 为一般事件
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ERC_Save(u8 Len, char *Data_Add, u8 Type)
{
    u8  Points, flag, AreaNum,PagePoint;
    u16 i, j, Ind_S, ERCHome, ERCIndex;
    u16 page, temp, area_ind;
    u16 Index, StInd, MaxPo;

    PagePoint = ERCPagePoint;
    AreaNum = ERCAreaNum;

    if(Type == 1)
    {
        ERCHome = ERC1Home;
        ERCIndex = ERC1.Index;
    }
    else if(Type == 0)
    {
        ERCHome = ERC2Home;
        ERCIndex = ERC2.Index;
    }
    temp = ERCIndex % ERCPointNum;
    page = ERCHome + temp / PagePoint;
    
    area_ind = temp % PagePoint;
    SPI_FLASH_PageRead( AllBuf.Spi1, page);
    for(i = area_ind * AreaNum; i < (area_ind + 1) *AreaNum; i++)
    {
        if (*(AllBuf.Spi1 + i) != 0xff)
        {
            break;
        }
    }
    Ind_S = area_ind * AreaNum;
    for(j = 0; j < Len; j++)
    {
        *(AllBuf.Spi1 + Ind_S + j) = *(Data_Add + j);
    }
    if(i >= (area_ind + 1) * AreaNum)
    //;已经格式化
    {
        SPI_FLASH_PageWrite_NE(AllBuf.Spi1, page);
       
    }
    else
    //;还没有格式化
    {
        for(i = (area_ind + 1) * AreaNum; i < PAGESIZE; i++)
        {
            *(AllBuf.Spi1 + i) = 0xff;
        }        
        SPI_FLASH_PageWrite_E(AllBuf.Spi1, page);
    }

    if(Type == 1)
    {
        Index  =  ERC1.Index;   //;针尾
        StInd  =  ERC1.StInd;   //;针头
    }
    else if(Type == 0)
    {
        Index  =  ERC2.Index;   //;针尾
        StInd  =  ERC2.StInd;   //;针头
    }

    MaxPo  =  ERCPointNum;   //;最大点数
    Points =  ERCPagePoint;  //;步长

    /*---------------------------新算法保存序列-----*/
    Index++;
    if((Index - StInd) >= MaxPo)
    {
        StInd = Index - MaxPo + Points;
        flag = (u8)(StInd % Points);
        StInd -= flag;
    }
    if(StInd >= MaxPo)
    {
        StInd -= MaxPo;
        Index -= MaxPo;
    }
    /*----------------------------------------------*/
    if(Type == 1)
    {
        ERC1.Index = Index;     //;针尾
        ERC1.StInd = StInd;     //;针头
        SaveData_FM24(4, E2R_ERC1, E2R_ERC1 + E2R_ByteNum, (u8*)(&ERC1.Index));
    }
    else if(Type == 0)
    {
        ERC2.Index = Index;     //;针尾
        ERC2.StInd = StInd;     //;针头
        SaveData_FM24(4, E2R_ERC2, E2R_ERC2 + E2R_ByteNum, (u8*)(&ERC2.Index));
    }
}

