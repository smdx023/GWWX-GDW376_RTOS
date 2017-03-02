/*
********************************************************************************
*  文 件 名：SPI_Flash.C
*
*  文件描述：
*
*  所用芯片：
*
*  创 建 人：
*
*  版 本 号：
*
*  修改记录：2011-07-07 14:40:06
********************************************************************************
*/

/* Includes ------------------------------------------------------------------*/

#include "BSP_CFG.h"

/* Private typedef -----------------------------------------------------------*/
#ifndef PAGECOUNT
    #define PAGECOUNT 4096
#endif


#define WIP_Flag   0x01  /* Write In Progress (WIP) flag */

#define Dummy_Byte 0xA5


//;======================AT45DB161D的命令字===========================
#define BUFFER_1_WRITE 0x84					 //; 写入第一缓冲区
#define BUFFER_2_WRITE 0x87					 //; 写入第二缓冲区
#define BUFFER_1_READ 0xD4					 //; 读取第一缓冲区
#define BUFFER_2_READ 0xD6					 //; 读取第二缓冲区
#define B1_TO_MM_PAGE_PROG_WITH_ERASE 0x83	 //; 将第一缓冲区的数据写入主存储器（擦除模式）
#define B2_TO_MM_PAGE_PROG_WITH_ERASE 0x86	 //; 将第二缓冲区的数据写入主存储器（擦除模式）
#define MM_PAGE_TO_B1_XFER 0x53				 //; 将主存储器的指定页数据加载到第一缓冲区
#define MM_PAGE_TO_B2_XFER 0x55				 //; 将主存储器的指定页数据加载到第二缓冲区
#define PAGE_ERASE 0x81						 //; 页删除（每页512/528字节）
#define SECTOR_ERASE 0x7C					 //; 扇区擦除（每扇区128K字节）
#define READ_STATE_REGISTER 0xD7			 //; 读取状态寄存器

//;-----------Flash2控制
#define SPI1_CS1_Low    GPIO_ResetBits(GPIOC, GPIO_Pin_4)
#define SPI1_CS1_High   GPIO_SetBits(GPIOC, GPIO_Pin_4)
#define SPI1_WP1_Low    GPIO_ResetBits(GPIOC, GPIO_Pin_5)
#define SPI1_WP1_High   GPIO_SetBits(GPIOC, GPIO_Pin_5)

//;//;-----------Flash2控制
//;#define SPI1_CS2_Low    GPIO_ResetBits(GPIOC, GPIO_Pin_4)
//;#define SPI1_CS2_High   GPIO_SetBits(GPIOC, GPIO_Pin_4)
//;#define SPI1_WP2_Low    GPIO_ResetBits(GPIOC, GPIO_Pin_5)
//;#define SPI1_WP2_High   GPIO_SetBits(GPIOC, GPIO_Pin_5)
//;
//;//;-----------Flash3控制
//;#define SPI2_CS1_Low    GPIO_ResetBits(GPIOD, GPIO_Pin_10)
//;#define SPI2_CS1_High   GPIO_SetBits(GPIOD, GPIO_Pin_10)
//;#define SPI2_WP1_Low    GPIO_ResetBits(GPIOD, GPIO_Pin_13)
//;#define SPI2_WP1_High   GPIO_SetBits(GPIOD, GPIO_Pin_13)
//;
//;//;-----------Flash4控制
//;#define SPI2_CS2_Low    GPIO_ResetBits(GPIOE, GPIO_Pin_4)
//;#define SPI2_CS2_High   GPIO_SetBits(GPIOE, GPIO_Pin_4)
//;#define SPI2_WP2_Low    GPIO_ResetBits(GPIOE, GPIO_Pin_3)
//;#define SPI2_WP2_High   GPIO_SetBits(GPIOE, GPIO_Pin_3)


 /*根据输入的页数来选择第几个Flash来保存数据*/
void SPI_CS_Low(u8 M)
{
    if(M == 0)
    {
        SPI1_CS1_Low;
    }
//;    else if(M == 1)
//;    {
//;        SPI1_CS2_Low;
//;    }
//;    else if(M == 2)
//;    {
//;        SPI2_CS1_Low;
//;    }
//;    else if(M == 3)
//;    {
//;        SPI2_CS2_Low;
//;    }
}
 /*根据输入的页数来选择第几个Flash来保存数据*/
void SPI_CS_High(u8 M)
{
    if(M == 0)
    {
        SPI1_CS1_High;
    }
//;    else if(M == 1)
//;    {
//;        SPI1_CS2_High;
//;    }
//;    else if(M == 2)
//;    {
//;        SPI2_CS1_High;
//;    }
//;    else if(M == 3)
//;    {
//;        SPI2_CS2_High;
//;    }
}

/*根据输入的页数来选择第几个Flash来保存数据*/
void SPI_WP_Low(u8 M)
{
    if(M == 0)
    {
        SPI1_WP1_Low;
    }
//;    else if(M == 1)
//;    {
//;        SPI1_WP2_Low;
//;    }
//;    else if(M == 2)
//;    {
//;        SPI2_WP1_Low;
//;    }
//;    else if(M == 3)
//;    {
//;        SPI2_WP2_Low;
//;    }
}

/*根据输入的页数来选择第几个Flash来保存数据*/
void SPI_WP_High(u8 M)
{
    if(M == 0)
    {
        SPI1_WP1_High;
    }
//;    else if(M == 1)
//;    {
//;        SPI1_WP2_High;
//;    }
//;    else if(M == 2)
//;    {
//;        SPI2_WP1_High;
//;    }
//;    else if(M == 3)
//;    {
//;        SPI2_WP2_High;
//;    }
}

/* 
********************************************************************************
* 函 数 名：SPI_FLASH_SendByte
* 功    能：发送一个字节并且返回接收到的一个字节
* 参    数：n==0:SPI1,n==1:SPI2 ;byte : 需要发送的字节
* 返    回：接收到的字节 
* 说    明：
* 抛出异常：
* 作    者：
* 修改记录：2011-07-07 15:09:29
********************************************************************************
*/
u8 SPI_FLASH_SendByte(u8 n, u8 byte)
{
    if(n == 0)
    {
        while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
        SPI_I2S_SendData(SPI1, byte);
        while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
        return SPI_I2S_ReceiveData(SPI1);
    }
//;    else if(n == 1)
//;    {
//;        while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
//;        SPI_I2S_SendData(SPI2, byte);
//;        while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
//;        return SPI_I2S_ReceiveData(SPI2);
//;    }  
    return 0;
}


/*******************************************************************************
* 程序名称   : SPI_FLASH_ReadByte
* 功    能   : 从 SPI Flash 中读出数据
*              这个程序需要先执行 Start_Read_Sequence 函数
* 输    入   ：None
* 输    出   : None
* 返    回   : Byte Read from the SPI Flash.
*******************************************************************************/
u8 SPI_FLASH_ReadByte(u8 n)
{
  return (SPI_FLASH_SendByte(n, Dummy_Byte));
}


/*******************************************************************************
* 程序名称  : SPI_FLASH_WriteEnable
* 功    能  : 允许向Flash芯片写数据(取消软件保护)
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void SPI_FLASH_WriteEnable(u8 n, u8 M)
{
  SPI_CS_Low(M);                     //;选择器件有效
  SPI_FLASH_SendByte(n, 0x3d);        //;写数据－写允许的指令
  SPI_FLASH_SendByte(n, 0x2a);        //;写数据－写允许的指令
  SPI_FLASH_SendByte(n, 0x7f);        //;写数据－写允许的指令
  SPI_FLASH_SendByte(n, 0x9a);        //;写数据－写允许的指令
  SPI_CS_High(M);                    //;选择器件有效
}

/*******************************************************************************
* 程序名称  : SPI_FLASH_WriteEnable
* 功    能  : 禁止向Flash芯片写数据 (软件保护)
* 输    入  : None
* 输    出  : None
* 返    回  : None
*******************************************************************************/
void SPI_FLASH_WriteDisable(u8 n, u8 M)
{
  SPI_CS_Low(M);                     //;选择器件有效
  SPI_FLASH_SendByte(n, 0x3d);        //;写数据－写允许的指令
  SPI_FLASH_SendByte(n, 0x2a);        //;写数据－写允许的指令
  SPI_FLASH_SendByte(n, 0x7f);        //;写数据－写允许的指令
  SPI_FLASH_SendByte(n, 0xA9);        //;写数据－写允许的指令
  SPI_CS_High(M);                    //;选择器件有效
}
/*******************************************************************************
*程序名称  :SPI_FLASH_ReadStatus
*功    能  :读取Flash的状态寄存器  
*输    入  :不断读取FLash的状态一直到 (RDY/BUSY)位为1
*输    出  :PROTECT 位的状态 
*返    回  :None
*******************************************************************************/
u8 SPI_FLASH_ReadStatus(u8 n, u8 M)
{
  u8 FLASH_Status = 0;
  SPI_CS_Low(M);     //;选择器件有效
  SPI_FLASH_SendByte(n, READ_STATE_REGISTER);  //;发送读状态寄存器的命令
  //;循环读取写寄存器的状态，一直到Flash不忙才推出
  do
  {
    FLASH_Status = SPI_FLASH_SendByte(n, 0x00);
  } while(!(FLASH_Status & 0x80));
  SPI_CS_High(M);    //;CS2置高
  return(FLASH_Status&0x02);
}


/*******************************************************************************
* 程序名称  : SPI_FLASH_SectorErase
* 功    能  : 擦除 FLASH指定页的数据
* 输    入  : SectorAddr: 指定扇区的地址
* 输    出  : 无
* 返    回  : 无
*******************************************************************************/
void SPI_FLASH_SectorErase(u32 page)
{
  u8 FlashX, spiN;
  
  FlashX = page / PAGECOUNT; //;第几片Flash
  spiN   = FlashX / 2;
  page = page % PAGECOUNT; //;在该Flash中的第几页
  //;SPI_FLASH_WriteEnable();   //;向FLASH芯片发送写允许的指令
  SPI_CS_Low(FlashX);               //;选择芯片 cs 置低
  SPI_FLASH_SendByte(spiN, SECTOR_ERASE);    //;发送数据 － 擦除扇区的命令
  SPI_FLASH_SendByte(spiN, (unsigned char)(page >> 6)); //;发送数据 －地址2字节
  SPI_FLASH_SendByte(spiN, (unsigned char)(page << 2));  //;发送数据 －地址1字节
  SPI_FLASH_SendByte(spiN, 0x00);   //;发送数据 － 地址最低字节
  SPI_CS_High(FlashX);               //; cs 置高
  SPI_FLASH_ReadStatus(spiN, FlashX);  //;等待擦写完毕
}


/*******************************************************************************
* 程序名称  : SPI_FLASH_PageErase
* 功    能  : 擦除 FLASH指定页的数据
* 输    入  : SectorAddr: 指定扇区的地址
* 输    出  : 无
* 返    回  : 无
*******************************************************************************/
void SPI_FLASH_PageErase(u32 page)
{
  u8 FlashX, spiN;
  
  FlashX = page / PAGECOUNT; //;第几片Flash
  spiN   = FlashX / 2;
  page = page % PAGECOUNT; //;在该Flash中的第几页
  //;SPI_FLASH_WriteEnable();   //;向FLASH芯片发送写允许的指令
  SPI_CS_Low(FlashX);               //;选择芯片 cs 置低
  SPI_FLASH_SendByte(spiN, PAGE_ERASE);    //;发送数据 － 擦除扇区的命令
  SPI_FLASH_SendByte(spiN, (unsigned char)(page >> 6)); //;发送数据 －地址2字节
  SPI_FLASH_SendByte(spiN, (unsigned char)(page << 2));  //;发送数据 －地址1字节
  SPI_FLASH_SendByte(spiN, 0x00);   //;发送数据 － 地址最低字节
  SPI_CS_High(FlashX);               //; cs 置高
  SPI_FLASH_ReadStatus(spiN, FlashX);  //;等待擦写完毕
}

/*******************************************************************************
*程序名称  : SPI_FLASH_PageWrite_E
*功    能  : flash先刷后写
*输    入  : - pBuffer : 写入Flash的数据的起始地址   page ：写入到的页码
*******************************************************************************/
void SPI_FLASH_PageWrite_E(u8* pBuffer, u16 page)
{ 
  u8 FlashX, spiN;
  u16 NumByteToWrite=528;
  
  FlashX = page / PAGECOUNT; //;第几片Flash
  spiN   = FlashX / 2;
  page = page % PAGECOUNT; //;在该Flash中的第几页
  
  SPI_FLASH_Init();
  SPI_FLASH_WriteEnable(spiN, FlashX);
  
  SPI_WP_High(FlashX);
  //;SPI_FLASH_WriteEnable(); //;允许写Flash芯片
  SPI_CS_Low(FlashX);               //;选择芯片 cs 置低
  SPI_FLASH_SendByte(spiN, BUFFER_2_WRITE);   //;写buffer2命令
  SPI_FLASH_SendByte(spiN, 0x00);
  SPI_FLASH_SendByte(spiN, 0x00);  
  SPI_FLASH_SendByte(spiN, 0x00);
  while(NumByteToWrite--) 
  {
    SPI_FLASH_SendByte(spiN, *pBuffer);
    pBuffer++; 
  }
  SPI_CS_High(FlashX);               //;选择芯片 cs 置高
  
  if(page<4096)
  { 
  	SPI_CS_Low(FlashX);                //;选择芯片 cs 置低
    SPI_FLASH_SendByte(spiN, 0x86);   //;通过buffer2先刷后写
    SPI_FLASH_SendByte(spiN, (u8)(page>>6));
    SPI_FLASH_SendByte(spiN, (u8)(page<<2));  
    SPI_FLASH_SendByte(spiN, 0x00);
  	
    SPI_CS_High(FlashX);               //;选择芯片 cs 置高
    SPI_FLASH_ReadStatus(spiN, FlashX);
  }
  SPI_WP_Low(FlashX);
  
  Delay_nms(40);
  SPI_FLASH_WriteDisable(spiN, FlashX);
}


/*******************************************************************************
*程序名称  : SPI_FLASH_PageWrite_NE
*功    能  : flash不用刷，直接写
*输    入  : - pBuffer : 写入Flash的数据的起始地址   page ：写入到的页码
*******************************************************************************/
void SPI_FLASH_PageWrite_NE(u8* pBuffer, u16 page )
{ 
  u8  FlashX, spiN;
  u16 NumByteToWrite=528;
  
  FlashX = page / PAGECOUNT; //;第几片Flash
  spiN   = FlashX / 2;
  page = page % PAGECOUNT; //;在该Flash中的第几页
  
  SPI_FLASH_Init();
  SPI_FLASH_WriteEnable(spiN, FlashX);
  
  SPI_WP_High(FlashX);
  //;SPI_FLASH_WriteEnable(); //;允许写Flash芯片
  SPI_CS_Low(FlashX);               //;选择芯片 cs 置低
  SPI_FLASH_SendByte(spiN, BUFFER_2_WRITE);   //;写buffer2命令
  SPI_FLASH_SendByte(spiN, 0x00);
  SPI_FLASH_SendByte(spiN, 0x00);  
  SPI_FLASH_SendByte(spiN, 0x00);
  while(NumByteToWrite--) 
  {
    SPI_FLASH_SendByte(spiN, *pBuffer);
    pBuffer++; 
  }
  SPI_CS_High(FlashX);               //;选择芯片 cs 置高
  
  if(page<4096)
  { 
  	SPI_CS_Low(FlashX);               //;选择芯片 cs 置低
    SPI_FLASH_SendByte(spiN, 0x89);   //;通过buffer2直接写
    SPI_FLASH_SendByte(spiN, (u8)(page>>6));
    SPI_FLASH_SendByte(spiN, (u8)(page<<2));  
    SPI_FLASH_SendByte(spiN, 0x00);
  	
    SPI_CS_High(FlashX);               //;选择芯片 cs 置高
    SPI_FLASH_ReadStatus(spiN, FlashX);
  }
  SPI_WP_Low(FlashX);
  
  Delay_nms(40);
  SPI_FLASH_WriteDisable(spiN, FlashX);
}

/*******************************************************************************
*程序名称  : SPI_FLASH_PageRead
*功    能  : flash页写
*输    入  : - pBuffer : 写入Flash的数据的起始地址   page ：写入到的页码
*输    出  :
*返    回  :
*******************************************************************************/
void SPI_FLASH_PageRead(u8* pBuffer, u16 page )
{ 
  u8  FlashX, spiN;
  u16 NumByteToWrite=528;
  
  FlashX = page / PAGECOUNT; //;第几片Flash
  spiN   = FlashX / 2;
  page = page % PAGECOUNT; //;在该Flash中的第几页
  SPI_CS_Low(FlashX);               //;选择芯片 cs 置低
  SPI_FLASH_SendByte(spiN, MM_PAGE_TO_B1_XFER);   //;将主存储器的数据写入到 buffer1
  SPI_FLASH_SendByte(spiN, (u8)(page>>6));
  SPI_FLASH_SendByte(spiN, (u8)(page<<2));  
  SPI_FLASH_SendByte(spiN, 0x00);
  SPI_CS_High(FlashX);              //;选择芯片 cs 置高
  SPI_FLASH_ReadStatus(spiN, FlashX);
  
  SPI_CS_Low(FlashX);               //;选择芯片 cs 置低
  SPI_FLASH_SendByte(spiN, BUFFER_1_READ );   //;读buffer1命令
  SPI_FLASH_SendByte(spiN, 0x00);
  SPI_FLASH_SendByte(spiN, 0x00);  
  SPI_FLASH_SendByte(spiN, 0x00);
  SPI_FLASH_SendByte(spiN, 0x00);
  while(NumByteToWrite--) 
  {
//  	if((pBuffer< AllBuf.St_Spi) || (pBuffer> AllBuf.En_Spi))  //;保护，不能超出缓冲区
//  	{ 
//  	    SPI_CS_High(FlashX); return; 
//  	}
    *pBuffer=SPI_FLASH_ReadByte(spiN);
    pBuffer++; 
  }        
  SPI_CS_High(FlashX);             //;选择芯片 cs 置高  
}



//;/*******************************************************************************
//;*程序名称  : SPI_FLASH_ReadNByte
//;*功    能  : 从FLASH主存里面直接读出n个字节到缓冲区
//;*输    入  : - pBuffer : 把从Flash读出的数据保存起来的起始地址
//;               page ：页   adr_p ：页内的地址
//;*输    出  :
//;*返    回  :
//;*******************************************************************************/
void SPI_FLASH_ReadNByte(u8* pBuffer, u16 page,u16 adr_p,u16 n )
{ //;u16 NumByteToWrite=528;
	u8  FlashX, spiN;
	if(n>=528) n=528;
  FlashX = page / PAGECOUNT; //;第几片Flash
  spiN   = FlashX / 2;
  page = page % PAGECOUNT; //;在该Flash中的第几页
  SPI_CS_Low(FlashX);               //;选择芯片 cs 置低
  SPI_FLASH_SendByte(spiN, MM_PAGE_TO_B1_XFER);   //;将主存储器的数据写入到 buffer1
  SPI_FLASH_SendByte(spiN, (u8)(page>>6));
  SPI_FLASH_SendByte(spiN, (u8)(page<<2));  
  SPI_FLASH_SendByte(spiN, 0x00);
  SPI_CS_High(FlashX);              //;选择芯片 cs 置高
  SPI_FLASH_ReadStatus(spiN, FlashX);
  
  SPI_CS_Low(FlashX);               //;选择芯片 cs 置低
  SPI_FLASH_SendByte(spiN,  BUFFER_1_READ );   //;读buffer1命令
  SPI_FLASH_SendByte(spiN, 0x00);
  SPI_FLASH_SendByte(spiN, (u8)(adr_p>>8));
  SPI_FLASH_SendByte(spiN, (u8)(adr_p));
  SPI_FLASH_SendByte(spiN, 0x00);
  while(n--) 
  {
  	if(EW.RFlashNByte!='R') return;
    *pBuffer=SPI_FLASH_ReadByte(spiN);
    pBuffer++; 
  }        
  SPI_CS_High(FlashX);             //;选择芯片 cs 置高  
  
}

/*******************************************************************************
*程序名称  :SPI_FLASH_CheckProtReg
*功    能  :检测Protection寄存器内容是否全为0xff
*输    入  :无
*输    出  :无
*返    回  :None
*******************************************************************************/
u8 SPI_FLASH_CheckProtReg(u8 n, u8 M)
{   
    u8 NumByteToWrite=16;
	u8 ch1,ch2=0xff;
	
	SPI_CS_Low(M);
	SPI_FLASH_SendByte(n, 0x32); //;
	SPI_FLASH_SendByte(n, 0x00);                        
	SPI_FLASH_SendByte(n, 0x00);   
	SPI_FLASH_SendByte(n, 0x00);  
    while(NumByteToWrite--) 
    {
      ch1=SPI_FLASH_ReadByte(n);
      if(ch1!=0XFF) ch2=0x00; 
    }        
    SPI_CS_High(M);      
    SPI_FLASH_ReadStatus(n, M);   
    return(ch2);       	
} 


/*******************************************************************************
*程序名称  :SPI_FLASH_EraseProtReg
*功    能  :格式化FLash芯片的保护寄存器－－就是将Flash的保护寄存器全部写为 0xff
*输    入  :无
*输    出  :无
*返    回  :无
*******************************************************************************/
void SPI_FLASH_EraseProtReg(u8 M)
{ 
    u8 spiN;
    
    spiN = M / 2;
	  if(SPI_FLASH_CheckProtReg(spiN, M) == 0x00)
	  {
	      SPI_CS_Low(M);
	      SPI_FLASH_SendByte(spiN, 0x3D); //;
	      SPI_FLASH_SendByte(spiN, 0x2A);                        
	      SPI_FLASH_SendByte(spiN, 0x7F);   
	      SPI_FLASH_SendByte(spiN, 0xCF);  
        SPI_CS_High(M);       
        SPI_FLASH_ReadStatus(spiN, M);      	
	  }
} 


