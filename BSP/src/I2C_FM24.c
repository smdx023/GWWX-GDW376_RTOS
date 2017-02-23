/*
********************************************************************************
*  文 件 名：I2C_FM24.c
*
*  文件描述：FM24CL04驱动接口
*
*  所用芯片：FM24CL04
*
*  创 建 人：023
*
*  版 本 号：
*
*  修改记录：2013-04-09 10:57:03
********************************************************************************
*/

#include "BSP_CFG.h"

#define I2C_WP_LOW()   GPIO_ResetBits(GPIOB, GPIO_Pin_5)
#define I2C_WP_HIGH()  GPIO_SetBits(GPIOB, GPIO_Pin_5)

/* 
********************************************************************************
* 函 数 名：Chk_StAns
* 功    能：
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
u8 Chk_StAns(void)
{
    u16 err = 0;
    
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        err++;
        if(err >= 2000)
        {
            return false;
        }
    }
    return true;
}

/* 
********************************************************************************
* 函 数 名：Chk_DevAns
* 功    能：
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
u8 Chk_DevAns(void)
{
    u16 err = 0;
    
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        err++;
        if(err >= 2000)
        {
            return false;
        }
    }
    return true;
}

/* 
********************************************************************************
* 函 数 名：Chk_DataAns
* 功    能：
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
u8 Chk_DataAns(void)
{
    u16 err = 0;
    
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        err++;
        if(err >= 2000)
        {
            return false;
        }
    }
    return true;
}

/* 
********************************************************************************
* 函 数 名：I2C_EE_WaitEepromStandbyState
* 功    能：
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void I2C_EE_WaitEepromStandbyState(void) 
{
    vu16 SR1_Tmp = 0;
    u16 err = 0; 
    
    do
    {
        //;产生 I2Cx传输 START条件
        I2C_GenerateSTART(I2C1, ENABLE); 

        //;读取指定的 I2C寄存器 I2C_SR1 并返回其值
        SR1_Tmp = I2C_ReadRegister(I2C1, I2C_Register_SR1); 
        
        //;向指定的从 I2C设备传送地址字 ,选择发送方向
        I2C_Send7bitAddress(I2C1, 0xA0, I2C_Direction_Transmitter); 
        SR1_Tmp = I2C_ReadRegister(I2C1, I2C_Register_SR1);
        err++;
        if(err >= 2000)
        {
            return ;
        }
    }
    while(!(I2C_ReadRegister(I2C1, I2C_Register_SR1) &0x0002)); //;地址发送结束

    //;清除 I2Cx的应答错误标志位
    I2C_ClearFlag(I2C1, I2C_FLAG_AF); 
}


/* 
********************************************************************************
* 函 数 名：WriteM16_I2C1
* 功    能：通过I2C1向AT24C04写入最多16个字节
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
u8 WriteM16_I2C1(u8 n, u16 adr, u8 *adr_ram)
{
    u16 devies;
    u8 i, num;

    if(n > 16)
    {
        n = 16;
    }
    
//    if(adr &0x100)
//    {
//        devies = 0xa2;
//    }
//    else
//    {
//        devies = 0xa0;
//    }
    
    devies = (adr & 0x100)?(0xa2):(0xa0);
    for(num = 0; num < 5; num++)
    {
        I2C_WP_LOW();
        I2C_EE_WaitEepromStandbyState();
        I2C_GenerateSTART(I2C1, ENABLE);
        if(EW.W16 != 'W')
        {
            return false; //;如果不允许写数据，则退出
        }
        if(!Chk_StAns())
        {
            continue;
        }
        I2C_Send7bitAddress(I2C1, devies, I2C_Direction_Transmitter);
        if(!Chk_DevAns())
        {
            continue;
        }
        I2C_SendData(I2C1, (u8)(adr));
//;        I2C_SendData(I2C1, (u8)((adr >> 8) << 1));
        if(Chk_DataAns() == false)
        {
            continue;
        }
        for(i = 0; i < n; i++)
        {
            I2C_SendData(I2C1, *(adr_ram + i));
            if(Chk_DataAns() == false)
            {
                break;
            }
        }
        if(i != n)
        {
            continue;
        }
        I2C_GenerateSTOP(I2C1, ENABLE);
        I2C_WP_HIGH();;
        EW.W16 = 0; //;不允许写存储器

        return true;
    }
    I2C_GenerateSTOP(I2C1, ENABLE);
    I2C_WP_HIGH();;
    EW.W16 = 0; //;不允许写存储器
    return false;
}


/* 
********************************************************************************
* 函 数 名：
* 功    能：通过I2C1向FM24CL04写入N个字节
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
u8 WriteNByte_I2C1(u8 n, u16 adr, u8 *adr_ram)
{
    u8 i, j = 0, k, l;

    i = n;
    
    if(i == 0)
    {
        return true;
    }
    while(i > 16)
    {
        k = 5;
        while(--k)
        {
            for(l = 0; l < 16; l++)
            {
                *(AllBuf.I2C_1 + l) = *(adr_ram + j + l);
            }
            
            if(EW.WFM != 'W')
            {
                return false;
            }
            
            EW.W16 = 'W'; //;允许EEPROM写入最多16个字节
            if(WriteM16_I2C1(16, adr + j, AllBuf.I2C_1) == false)
            {
                EW.WFM = 0;
                return false;
            }
            
            EW.R16 = 'R'; //;允许EEPROM读出最多16个字节
            if(ReadM16_I2C1(16, adr + j, AllBuf.I2C_2) == false)
            {
                EW.WFM = 0;
                return false;
            }
            
            if(Compare_NByte(16, AllBuf.I2C_1, AllBuf.I2C_2) == true)
            {
                i -= 16;
                j += 16;
                break;
            }
        }
        if(k == 0)
        {
            EW.WFM = 0;
            return false;
        }
    }

    k = 5;
    while(--k)
    {
        for(l = 0; l < i; l++)
        {
            *(AllBuf.I2C_1 + l) = *(adr_ram + j + l);
        }
        
        if (EW.WFM != 'W')
        {
            return false;
        }
        EW.W16 = 'W'; //;允许EEPROM写入最多16个字节
        if(WriteM16_I2C1(i, adr + j, AllBuf.I2C_1) == false)
        {
            EW.WFM = 0;
            return false;
        }
        EW.R16 = 'R';

        if(ReadM16_I2C1(i, adr + j, AllBuf.I2C_2) == false)
        {
            EW.WFM = 0;
            return false;
        }

        if(Compare_NByte(i, AllBuf.I2C_1, AllBuf.I2C_2) == true)
        {
            break;
        }
    }
    if(k == 0)
    {
        EW.WFM = 0;
        return false;
    }
    else
    {
        return true;
    }
}


/* 
********************************************************************************
* 函 数 名：ReadM16_I2C1
* 功    能：通过I2C1从AT24C04读出N个字节
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
u8 ReadM16_I2C1(u8 n, u16 adr, u8 *adr_ram)
{
    u16 devies;
    u8 i = 0;
    u16 err;
    u8 num;
    
    if(n < 1)
    {
        return true;
    }
    //;如果读的字节数为0，则跳出
    if (n > 16)
    {
        n = 16;
    }

    devies = (adr & 0x100)?(0xa2):(0xa0);
    for(num = 0; num < 5; num++)
    {
        I2C_EE_WaitEepromStandbyState();

        I2C_GenerateSTART(I2C1, ENABLE); //;开始位
        if(Chk_StAns() == false)
        {
            continue;
        }
        if(n == 1)
        {
            I2C_AcknowledgeConfig(I2C1, DISABLE);
        } //;如果只读一个字节
        I2C_Send7bitAddress(I2C1, devies, I2C_Direction_Transmitter);
        if(Chk_DevAns() == false)
        {
            continue;
        }
        I2C_Cmd(I2C1, ENABLE);
        I2C_SendData(I2C1, (u8)(adr));
        if(Chk_DataAns() == false)
        {
            continue;
        }
        I2C_GenerateSTART(I2C1, ENABLE);
        if(Chk_StAns() == false)
        {
            continue;
        }
        I2C_Send7bitAddress(I2C1, devies, I2C_Direction_Receiver);
        err = 0;
        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) //;写入器件的地址
        {
            err++;
            if (err > 2000)
            {
                break;
            }
        }
        err = 0;
        while(n)
        {
            if(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))
            {
                *(adr_ram + i) = I2C_ReceiveData(I2C1);
                if (n == 2)
                {
                    I2C_AcknowledgeConfig(I2C1, DISABLE);
                }
                if(n == 1)
                {
                    I2C_GenerateSTOP(I2C1, ENABLE);
                }
                i++;
                n--;
                err = 0;
            }
            else
            {
                err++;
                if(err > 2000)
                {
                    break;
                }
            }
        }
        if(n != 0)
        {
            continue;
        }
        I2C_AcknowledgeConfig(I2C1, ENABLE);
        return true;
    }
    return false;
}


/* 
********************************************************************************
* 函 数 名：ReadNByte_I2C1
* 功    能：通过I2C1向FM24C04写入N个字节
* 参    数：
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
u8 ReadNByte_I2C1(u8 n, u16 adr, u8 *adr_ram)
{
    u8 i, j = 0, k, l;

    i = n;
    
    if(i == 0)
    {
        return true;
    }
    
    while(i > 16)
    {
        k = 5;
        while(--k)
        {
            if(EW.RFM != 'R')
            {
                return false;
            }
            EW.R16 = 'R'; //;允许EEPROM读出最多16个字节

            if(ReadM16_I2C1(16, adr + j, AllBuf.I2C_1) == false)
            {
                EW.RFM = 0;
                return false;
            }

            EW.R16 = 'R'; //;允许EEPROM读出最多16个字节
            if(ReadM16_I2C1(16, adr + j, AllBuf.I2C_2) == false)
            {
                EW.RFM = 0;
                return false;
            }
            if(Compare_NByte(16, AllBuf.I2C_1, AllBuf.I2C_2) == true)
            {
                for(l = 0; l < 16; l++)
                {
                    *(adr_ram + j + l) = AllBuf.I2C_1[l];
                }
                i -= 16;
                j += 16;
                break;
            }
        }
        if(k == 0)
        {
            EW.RFM = 0;
            return (false);
        }
    }
    k = 5;
    while(--k)
    {
        if (EW.RFM != 'R')
        {
            return false;
        }
        EW.R16 = 'R'; //;允许EEPROM读出最多16个字节
        if (ReadM16_I2C1(i, adr + j, AllBuf.I2C_1) == false)
        {
            EW.RFM = 0;
            return (false);
        }

        EW.R16 = 'R'; //;允许EEPROM读出最多16个字节
        if (ReadM16_I2C1(i, adr + j, AllBuf.I2C_2) == false)
        {
            EW.RFM = 0;
            return (false);
        }
        if (Compare_NByte(i, AllBuf.I2C_1, AllBuf.I2C_2) == true)
        {
            for (l = 0; l < i; l++)
            {
                *(adr_ram + j + l) = AllBuf.I2C_1[l];
            }
            break;
        }
    }
    if(k == 0)
    {
        EW.RFM = 0;
        return false;
    }
    else
    {
        return false;
    }

}

/* 
********************************************************************************
* 函 数 名：SaveData_FM24
* 功    能：数据和它的校验码保存到存储器的两个位置
* 参    数：n--      数据长度，不包括校验码
*           EEAdr1 EEAdr2  存储器的两个位置
*           adr_ram        要保存数据的起始地址
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void SaveData_FM24(u8 n, u16 EEAdr1, u16 EEAdr2, u8 *adr_ram)
{
    u8 i;

    AllBuf.Spi1[n] = 0x55;
    for(i = 0; i < n; i++)
    {
        AllBuf.Spi1[i] = *(adr_ram + i);
        AllBuf.Spi1[n] += *(adr_ram + i);
    }

    EW.WFM = 'W';
    WriteNByte_I2C1(n + 1, EEAdr1, AllBuf.Spi1);
    WriteNByte_I2C1(n + 1, EEAdr2, AllBuf.Spi1);
    EW.WFM = 0;

}

/* 
********************************************************************************
* 函 数 名：ReadData_FM24
* 功    能：将数据从存储器的两个位置读出，找出正确的数据
* 参    数：n    --        数据长度，不包括校验码
*           EEAdr1 EEAdr2  存储器的两个位置
*           adr_ram        要保存数据的起始地址
* 返    回： 
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void ReadData_FM24(u8 n, u16 EEAdr1, u16 EEAdr2, u8 *adr_ram)
{
    u8 i, ch;

    EW.RFM = 'R';
    ReadNByte_I2C1(n + 1, EEAdr1, AllBuf.Spi1);
    ch = 0x55;
    for(i = 0; i < n; i++)
    {
        ch += AllBuf.Spi1[i];
    }
    if(ch == AllBuf.Spi1[n])
    {
        for(i = 0; i < n; i++)
        {
            *(adr_ram + i) = AllBuf.Spi1[i];
        }
        return ;
    }
    else
    {
        ReadNByte_I2C1(n + 1, EEAdr2, AllBuf.Spi1);
        ch = 0x55;
        for(i = 0; i < n; i++)
        {
            ch += AllBuf.Spi1[i];
        }
        if(ch == AllBuf.Spi1[n])
        {
            for(i = 0; i < n; i++)
            {
                *(adr_ram + i) = AllBuf.Spi1[i];
            }
            return ;
        }
        else
        {
            for(i = 0; i < n; i++)
            {
                *(adr_ram + i) = 0;
            }
        }
    }
    EW.RFM = 0;
}
