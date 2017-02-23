
#ifndef __BSP_CFG_H
#define __BSP_CFG_H

#include "stm32f10x.h"
#include "USERMATH.h"
#include "string.h"
#include "Function_CFG.h"
#include "I2C_FRAM.h"  //20131025  

#define false   0x00
#define true    0x01
#define nosend  0x00
#define right   0x01
#define error   0x02
#define none    0x03
#define ignore  0x04
#define back    0x55
#define ahead   0xaa
#define same    0x00
#define idle    0x00    //;空闲
#define busy    0x10
#define OFF     0x00    //;开
#define ON      0x01    //;关

#define _220V   0x00
#define _48V    0x01


typedef const uint32_t B4;  /*!< Read Only */
typedef const uint16_t B2;  /*!< Read Only */
typedef const uint8_t  B1;   /*!< Read Only */

struct Time_Type  //;日期时间的数据结构
{
    u8 Sec;
    u8 Min;
    u8 Hour;
    u8 Day;
    u8 Month;
    u8 Year;
};

struct Buf_Type   //;所有的缓冲区的分配
{
    u8   Start[1];
    u8   I2C_1[16];
    u8   I2C_2[16];
    
    char Tx_SB[32];

    char Tx5_1[U5BUFSIZE];  //;串口5的缓冲区
    char Rx5_2[U5BUFSIZE];
    char Rx5_1[U5BUFSIZE];


    char Tx2_1[U2TXBUFSIZE];  //;串口2的缓冲区
    char Rx2_1[U2RXBUFSIZE];
    char Rx2_2[U2RXBUFSIZE];	
    
    char Tx1_1[U1BUFSIZE];  //;串口1的缓冲区
    char Rx1_2[U1BUFSIZE];
    char Rx1_1[U1BUFSIZE];
    
    char Tx3_1[U3BUFSIZE];  //;串口3的缓冲区
    char Rx3_2[U3BUFSIZE];
    char Rx3_1[U3BUFSIZE];

    char Tx4_1[U4BUFSIZE];  //;串口4的缓冲区
    char Rx4_1[U4BUFSIZE];
    char Rx4_2[U4BUFSIZE];

    u8   End[1];

    u8   St_Spi[1];
    u8   Spi1[528];
    u8   En_Spi[1];
};


struct USARTDataStream  //;串口数据流的控制
{
    u8  Mark;
    u16 RIndex;    //;接收序列号
    u16 TIndex;    //;发送序列号
    u16 TLong;     //;发送长度
    u8  TBusy;     //;发送忙碌
    u8  RecRun;    //;是否在接受
    u8  RBet2Byte; //;接收两个字节的时间间隔
    u8  RecOrd;    //;串口收到数据帧
    u16 RecLen;    //;接收长度
    u8  SedOrd;    //;串口在发送帧
    u8  DataLightMs;
    char  *BufAdr;   //;发送与接收缓存的指针
};


struct HardWareControl  //;用于硬件控制
{
    u8 GIn1;   //;GIn1的状态
    u8 GIn2;   //;GIn2的状态  //;用于表示当前模块是由电池供电还是电源供电
    u8 DCD;    //;DCD的状态
    u8 RI;     //;RI的状态
    u8 GIn2_Del;   //;GIn2引脚为高电平的时间计时	
    u8 DCD_Del;    //;DCD脚状态为高的时间
    u8 EventFlag;
    u8 EventRead;
    u8 DCD_Flag;
    u8 EnRes[2]; //;用于重启
    u8 FactoryInit; //;出厂标识by zrt 20130816
};

struct SafeWord //;安全认证字
{
    u8 Para[2];
    u16 SumT1;
    u16 SumT2;
    u16 SumT3;
    u16 SumT4;
    u16 SumT5;
    u16 SumT6;
    u16 SumT7;
    u16 SumT8;
};

struct EnableWordStr //;允许状态字
{
    u8 Start[1];
    u8 CSB1;
    u8 CSB2;
    u8 CSB3;
    u16 CSB4;
    u8 CSB5;
    u8 CSB6;
    u8 CSB7;
    u8 CSB8;
    u8 CSB9;
    u8 CSB10;
    u8 ZZBock;
    u8 W16;
    u8 R16;
    u8 WFM;
    u8 RFM;
    u8 TaskAUL_P;
    u8 TaskAUL_F;
    u8 TaskSamp;
    u8 MoveByte;
    u8 RFlashNByte;
    u8 TimeSet;
    u8 End[1];
};

struct FluxStat
{
    u32 Day;
    u32 Month;
};

struct  BattyCharge
{
    u16 BT1, BT2;
    u8 Station;
    u8 Switch;
    u16 Time[3];
};

extern struct SafeWord SW;
extern struct EnableWordStr EW;
extern u8 RTC_FlagMs, Num025s_WT, RTC_FlagSec, RTC_FlagMin, RTC_ms, RTC_sec, RTC_min;
extern u8 Enable_Save;//;为  I2C.c文件分配的变量
extern u8 OldStatus_GprsC,OldSign_GprsC;  //;GPRS的之前的状态
extern u8 Power_Modem;
extern struct HardWareControl  HWareC; //;用于硬件控制
extern struct USARTDataStream  U5DS, U1DS, U4DS, U2DS, U3DS;  //;Uart1和Uart2 的数据流的控制参数
extern struct ModemContrl  GprsC, GprsCErr;
extern struct ModemLinkCtrl GprsLC;
extern u8 Yester_Day,Yester_Month,Yester_Year, Old_Sec, Old_Min,Week;
extern struct Time_Type TBCD,THEX;  //;BCD码格式和HEX格式的日期时间
extern struct Buf_Type AllBuf;  //;缓冲区
extern u16 ReadFlash_Enable;
extern u16 ADCConvertedValue[2];
extern struct BattyCharge  BattyC; //;电池充电控制
extern u8 NUMPOWER;

/*----------------------------------------------------------------------------*
 | I2C_FM24_8025.c
 *----------------------------------------------------------------------------*/
//;extern void __I2C_FM24_8025_c(void){}
//extern u8 WriteM16_I2C1(u8 n, u16 adr, u8* adr_ram );
extern u8 WriteNByte_I2C1(u8 n, u16 adr, u8* adr_ram );
//extern u8 ReadM16_I2C1(u8 n, u16 adr, u8* adr_ram );
extern u8 ReadNByte_I2C1(u8 n, u16 adr, u8* adr_ram );
extern void SaveData_FM24(u8 n, u16 EEAdr1, u16 EEAdr2, u8* adr_ram);
extern void ReadData_FM24(u8 n, u16 EEAdr1, u16 EEAdr2, u8* adr_ram);
//extern void EPSON8025_SAVE(u8 Addr, u8 Data);
//extern void EPSON8025_READ(u8 *Addr);

/*----------------------------------------------------------------------------*
 |  MODEN_M590.c
 *----------------------------------------------------------------------------*/
//;extern void __MODEN_M590_c(void){}
#define  _Modem_ON            0x00 //;GPRS模块相关定义
#define  _Modem_OFF           0x01
#define  _ModemPowerOn        GPIO_SetBits(GPIOB, GPIO_Pin_12)       //;打开模块主用电源
#define  _ModemPowerOff       GPIO_ResetBits(GPIOB, GPIO_Pin_12)   //;关闭模块主用电源

#define  _ModemONOFF_High     GPIO_ResetBits(GPIOA, GPIO_Pin_8)      //;G-ON-OFF拉高
#define  _ModemONOFF_Low      GPIO_SetBits(GPIOA, GPIO_Pin_8)       //;G-ON-OFF拉低

#define  _GPRS_Modem_Run      GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)  //;检测GPRS模块启动
                    
#define  LOGONLIGHTON        GPIO_ResetBits(GPIOB, GPIO_Pin_8)  //;打开网络灯
#define  LOGONLIGHTOFF       GPIO_SetBits(GPIOB, GPIO_Pin_8)    //;关闭网络灯   

#define  COMLIGHTON()         GPIO_ResetBits(GPIOB, GPIO_Pin_0)  //;通信正常指示
#define  COMLIGHTOFF()        GPIO_SetBits(GPIOB, GPIO_Pin_0)

#define  TCPDATALIGHTON()    GPIO_ResetBits(GPIOB, GPIO_Pin_9)  //;网络数据灯开
#define  TCPDATALIGHTOFF()   GPIO_SetBits(GPIOB, GPIO_Pin_9) 

#define  METERDATALIGHTON()    GPIO_ResetBits(GPIOB, GPIO_Pin_1)  //;本地数据灯开
#define  METERDATALIGHTOFF()   GPIO_SetBits(GPIOB, GPIO_Pin_1)  

#define  RS485CTRLRCV()         GPIO_SetBits(GPIOA, GPIO_Pin_4)      
#define  RS485CTRLSED()         GPIO_ResetBits(GPIOA, GPIO_Pin_4)  

#define  EVENTCHECK()      GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)  //;检测事件

//;#define  RS485CTRLRCV()         GPIO_ResetBits(GPIOA, GPIO_Pin_4)      
//;#define  RS485CTRLSED()         GPIO_SetBits(GPIOA, GPIO_Pin_4)           

/*GprsC.Status用于下发到基表的模块状态*/
#define  _NoCard     0x01   //;无SIM卡 
#define  _FindHost   0x02   //;正在寻找主站
#define  _ContHost   0x04   //;与主站连接成功
#define  _FindHFalst 0x08   //;寻找主站失败

/*GprsC.State*/
#define  _SetUp      0x01   //; 模块已经启动
#define  _Link       0x02   //; 模块正在发起连接
#define  _Connect    0x04   //; 模块已经连接主站
#define  _HadLogin   0x08   //; 模块已经登陆主站
#define  _Redial     0x10   //; 模块正在等待重拨 

/*GprsC.ATStat*/         
#define  LINK0       0x00
#define  LINK1       0x01


struct ModemContrl       //;GPRS模块的控制
{
    u8   State;            //;主站的状态
    u8   Status;           //;模块的状态
    u8   State1;           //;链路1连接的状态
    u8   ReConnect;
    u8   Re_Connect_states;//;准备重新连接状态(只有远程更改IP才执行)
    u8   TCPFail;      
    u8   ATStatus;         //;当前正在发送AT命令的状态    
    u8   ATLen;            //;将要发送的AT指令的长度
    u8   ATSOVT;           //;AT状态超时防错
    u8   SendAT_Wait1s;
    u8   SendAT_Step;      //;等待向GPRS模块发送AT指令的步骤
    u8   ATAns_Wait1s;     //;等待AT指令应答的最大秒数
    u8   ReSendAT_Num;     //;AT指令重发的次数
    u8   ReCon_Num;        //;重拨次数
    u16  MinReCon;         //;主站重新拨号的时间间隔
    u8   CPINErrNum;
    u8   CSQErrNum;
    u8   CREGErrNum;
    u8   XIICErrNum;
    u8   CMGSNum;
    u8   Sign;             //;信号大小
    u8   *IP;
    u16  Port;
    char *APN;
    u8   LinkState;
    u8   LoginIn;          //;是否允许主站发送登陆报文
    u8   LoginTimeOut;     //;发送登陆报文最大延时时间
    u8   LoginNumSend;
    u8   LoginOut;
    u8   HBStatus;         //;发送心跳状态
    u8   HBNumSend;        //;心跳重发次数
    u16  HBTime;           //;心跳倒计时
    u16  HBOVT;            //;心跳超时
    u8   TCPRec;             
    u16  TCPLen;
    char *TCPBuf;
    u16  SMSLen;
    u8   SMSPer;
    u8   SignR;         //;新增    
    
    u8   NDataMin1;
    u16  NDataMin0;
    u16  NLogonMin;  
    u8   RailSign;
    u8   EnRes[2];         //;是否允许对模块进行复位
    char ATStr[50];        //;将要发送的AT指令
      
    u8 DataLightMs;
    
};

struct ModemLinkCtrl  //;GPRS连接相关参数
{   
    u8   PassLinkOrd;
    s8   PassLinkNum;
    u8   Link2Switch;  
    u8   CHKLink;    //;检测是否允许上线
    u8   LinkEnable; //;允许上线标志
    u8   IP1[21]; //;备用主站IP
    u8   IP1_long; //;备用IP端口号长度
};

#ifdef ZS
extern void RecordErr(u8 type);
#endif

extern void Modem_GDCDLightCtr(void);
extern  void Modem_PowON(void);
extern  void Modem_PowOFF(void);
extern  void Modem_PowBKON(void);
extern  void Modem_PowBKOFF(void);
extern  void Modem_GDCDON(void);
extern  void Modem_GDCDOFF(void);
extern  void Modem_ON(void);
extern  void Modem_ON_CHK(void);
extern  u8   Modem_OFF_CHK(void);
extern  void Modem_OFF(void);
extern  void Modem_OFFON(void);
extern  void ATComdSend(char *str1);
extern  u8   Modem_TCP_Send(void);
extern  void ATComdSetDeal(u8 *IP,  u8 Port, char *APN);
extern  void ATComdSetNoRec(void);
extern  u8 Modem_SMS_Send(void);
extern  u8   SMS_DataMove(u8 Len, char *OutAddr, u16 *InAddr);
extern  u8   SMS_CodeData(char *Out_Buf, u8 *SMS_Code, u8 *Tel_Code);
extern  char SMS_HexToASC(u8 HEX);
extern  void Modem_Com(void);
extern void Modem_LinkTime(void);
extern u8 NumIpToChar(char *Buf, u8 *IP, u16 Port);
extern void Modem_AT_Send(char *str1);  //by zrt 20130912
/*----------------------------------------------------------------------------*
 |  Serial.c
 *----------------------------------------------------------------------------*/
extern void SendByte_USART1(void);
extern void SendFirstByte_USART1(void);
extern void ReceiveByte_USART1(void);

extern void SendFirstByte_USART2(void);
extern void SendByte_USART2(void);
extern void ReceiveByte_USART2(void);

extern void SendByte_USART3(void);
extern void SendFirstByte_USART3(void);
extern void ReceiveByte_USART3(void);

extern void SendByte_UART4(void);
extern void SendFirstByte_UART4(void);
extern void ReceiveByte_UART4(void);

extern void SendByte_UART5(void);
extern void SendFirstByte_UART5(void);
extern void ReceiveByte_UART5(void);

extern void USART_Com(void);


/*----------------------------------------------------------------------------*
 |  Setup.c
 *----------------------------------------------------------------------------*/
//;extern void __Setup_c(void){}
extern void InitHardware (void);
extern void IWDG_Configuration(void);
extern void Batty_Charge(void);
extern void Watchdog( void );
extern void RTC_Time(void);
extern void Delay_nms(u16 n);
extern void USART2_BaudRateSet(u32 BaudRate);
extern void UARTx_BaudRate_Set(u8 BPS, u8 UARTx);

/*----------------------------------------------------------------------------*
 |  spi_flash.c
 *----------------------------------------------------------------------------*/
//;extern void __spi_flash_c(void){}
extern void SPI_FLASH_Init(void);
extern void SPI_FLASH_PageErase(u32 page);
extern void SPI_FLASH_PageRead(u8* pBuffer, u16 page );
extern void SPI_FLASH_PageWrite_E(u8* pBuffer, u16 page );
extern void SPI_FLASH_PageWrite_NE(u8* pBuffer, u16 page );
//;extern u8   SPI_FLASH_ReadByte(void);
//;extern u8   SPI_FLASH_SendByte(u8 byte);
//;extern void SPI_FLASH_WriteEnable(u8 M);
extern void SPI_FLASH_ReadProtReg(u8* pBuffer);
//;extern u8   SPI_FLASH_ReadStatus(u8 M);
//;extern u8   SPI_FLASH_CheckProtReg(u8 M);
extern void SPI_FLASH_EraseProtReg(u8 M);
//;extern void SPI_FLASH_WriteDisable(u8 M);
extern void SPI_FLASH_ReadNByte(u8* pBuffer, u16 page,u16 adr_p,u16 n );


#endif //;__BSP_CFG_H




