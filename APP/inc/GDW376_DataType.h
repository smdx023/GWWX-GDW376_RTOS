/*
********************************************************************************
*  文 件 名：
*
*  文件描述： GDW130规约所用数据结构 
*
*  所用芯片：
*
*  创 建 人：
*
*  版 本 号：
*
*  修改记录：1.
              2.
              3.ZDPStr_F13规约支持脉冲配置路数为64，此为4
              4.ZDPStr_F14_1规约支持64个测量点，此Point_Op中为8个。
              5.ZDPStr_F22规约支持48个费率，此为14个。
              6.ZDPStr_F33规约支持32个参数块，此为4个
********************************************************************************
*/

#ifndef __GDW130_DataType_h
#define __GDW130_DataType_h

#include "stm32f10x.h"

#define SBNUM 30

#define  METERNUM  1   //;ZDPStr_F10规约支持2040块电能表，在此为1个 //;模块只支持1个测量点--20141226
#define  PULSENUM  4    //;ZDPStr_F11规约支持脉冲配置路数为64，此为4
#define  SIMULNUM  4    //;ZDPStr_F13规约支持脉冲配置路数为64，此为4
#define  GRPPTNUM  8    //;ZDPStr_F14_1规约支持64个测量点，此Point_Op中为8个。
#define  RATENUM   4    //;ZDPStr_F22规约支持48个费率，此为4个。
#define  NODENUM   4    //;ZDPStr_F31规约支持20个节点，此为4个。
#define  RMTNUM    4    //;ZDPStr_F33_1_1规约支持24个抄表时段，此为4个。
#define  PORTNUM   2    //;ZDPStr_F33_1规约支持31个端口号，此为2个。F34亦用。
#define  KEYMETNUM 5    //;ZDPStr_F35规约支持20个重点户，此为5个。
#define  USRSNUM   4    //;ZDPStr_F38、F39规约支持16个用户小类号，此为4个。
#define  TASK1NUM   8    //;ZDPStr_F65任务个数。
#define  TASK2NUM   8    //;ZDPStr_F66任务个数。

//;6(F1)+33(F2)+28(F3)+16(F4)+3(F5)+16(F6)+64(F7)+8(F8)+16(F9)+272(F10) = 462Byte
#define  CS1NUM    462  
//;21(F11)+2(F12)+13(F13)+81(F14)+73(F15)+64(F16)+2(F17)+12(F18)+1(F19)+1(F20)
//;  +25(F21)+17(F22)+3(F23) = 315Byte
#define  CS2NUM    315  
//;61(F33)+13(F34)+11(F35)+4(F36)+19(F37)+138(F38)+138(F39)+3(F57)+1(F58)+4(F59)
//; +44(F60)+1(F61) = 413Byte
#define  CS3NUM    437  
//;11(F25)+57(F26)+24(F27)+4(F28)+12(F29)+1(F30)+25(F31)+48(F73)+10(F74)+16(F76)
//;  +1(F76) = 209Byte
#define  CS4NUM    209  
//;365(F65)1~4 = 356Byte
#define  CS5NUM    356  
//;356(F66)+8(F67)+8(F68) = 372Byte
#define  CS6NUM    372  
//;52(F41)+6(F42)+1(F43)+8(F44)+1(F45)+4(F46)+17(F47)+1(F48) = 90Byte
#define  CS7NUM_GRP    90  
//;4(F81)+4(F82)+1(F83) = 9Byte
#define  CS7NUM_DCP    9  
//;1(F149) = 1Byte
#define  CS7NUM_CTR    1  
//;365(F65)5~8 = 365Byte
#define  CS9NUM    356  
//;365(F66)5~8 = 365Byte
#define  CS10NUM    356  

#define  ZD      1
#define  POINT   2
#define  TASK    3
#define  GRP     4
#define  CTR     5
#define  DCP     6


struct ZDPStr_F1  //;终端通讯参数 6B
{
    u8  RTS;       //;数传机延时时间
    u8  DELT;      //;作为启动站延时时间
    u8  OVT_RNum[2];  //;超时时间和重发次数
    u8  CON;       //;需要主站确认的标志
    u8  HBeat;     //;心跳周期
};

struct ZDPStr_F2  //;中继转发设置 33B
{
    u8 En_Num;  //;允许和转发的地址数
    u8 Adr[16][2];  //;要转发的地址
};

struct ZDPStr_F3  //;主站IP地址和端口 28B
{
    u8  IP1[4];
    u8 Port1[2];
    u8  IP2[4];
    u8 Port2[2];   
    char APN[16];
};

struct ZDPStr_F4  //;主站电话号码和短信中心号码 16B
{
    u8 Tel_Code[8];
    u8 SMS_Code[8];
};

struct ZDPStr_F5  //;终端消息认证参数设置 3B
{
    u8  Code;
    u8  Para[2];
};

struct ZDPStr_F6  //;终端组地址 16B
{
    u8 Addr1[2];
    u8 Addr2[2];
    u8 Addr3[2];
    u8 Addr4[2];
    u8 Addr5[2];
    u8 Addr6[2];
    u8 Addr7[2];
    u8 Addr8[2];
};

struct ZDPStr_F7  //;终端IP地址和端口 64B
{
    u8  ZDIP[4];
    u8  SubnetIP[4]; //;子网掩码
    u8  GateIP[4];   //;网关
    u8  DNSType;
    u8  DNSIP[4];    //;代理服务器
    u8  DNSPort[2];     //;代理服务器端口
    u8  DNSLinkType; //;代理服务器连接方式
    u8  UsrLen;      //;用户名长度
    u8  UsrName[20]; //;用户名
    u8  PasswordLen; //;密码长度
    u8  Password[20];
    u8  ZDPort[2];      //;终端侦听端口  
};

struct ZDPStr_F8  //;终端上行通信工作方式 8B
{
    u8  WorkMode;   //;工作模式
    u8  ActRedailSec[2]; //;永久在线、时段在线模式重拨间隔
    u8  RedailNum; //;被动激活模式重播次数
    u8  NoComBreakMin;  //;被动激活模式无通信自动断线时间
    u8  AllowTimeF[3];    //;时段在线模式允许在线时段标志
};

struct ZDPStr_F9  //;事件记录配置 16B
{
    u8  Enable[8];
    u8  Rating[8];
};

struct ZDPStr_F10_1 //;27B
{
    u8  Index[2];
    u8  Point[2];
    u8  BPS_Port;
    u8  Type;
    u8  Addr[6];
    u8  PW[6];
    u8  Rate_Digit;
    u8  WEnergyPN; //;有功电能示值整数位及小数位个数
    u8  CJQComAddr[6]; //;所属采集器通信地址
    u8  UsrN;          //;用户大类号及小类号分配。
};

struct  ZDPStr_F10   //;终端电能表，交流采样装置配置参数 272B
{
    u8     MeterNum[2];
    struct ZDPStr_F10_1 Meter[METERNUM]; 
};

struct ZDPStr_F11_1  //;5B
{
    u8  PuPort;
    u8  Point;
    u8  PuType;
    u8  Imp[2];
};
struct  ZDPStr_F11    //;终端脉冲配置 21B
{
    u8 PuNum;
    struct ZDPStr_F11_1 Pulse[PULSENUM];
};

struct ZDPStr_F12    //;终端状态量输入 2B
{
    u8 ZTL_Flag;
    u8 ZTL_Type;
};
struct ZDPStr_F13_1  //;3B
{
    u8 Port;
    u8 Point;
    u8 Type;
};

struct ZDPStr_F13 //;终端电压、电流模拟量配置参数 13B
{
    u8 SimulNum;
    struct ZDPStr_F13_1 Simul[SIMULNUM];
};

struct ZDPStr_F14_1 //;10B
{
    u8 Index;        //;总加组序号
    u8 Point_Num;    //;测量点数
    u8 Point_Op[GRPPTNUM];  //;运算法则
};

struct ZDPStr_F14  //;总加组配置参数 81B
{
    u8 GRPNum;
    struct ZDPStr_F14_1 ADDGRP[8];
};

struct ZDPStr_F15_1 //;9B
{
    u8 Index;
    u8 QIndex;
    u8 qIndex;
    u8 Type;
    u8 Qr;
    u8 Qa[4];
};
struct ZDPStr_F15 //;有功总电能量差动越限事件参数配置 73B
{
    u8 PEENum;
    struct ZDPStr_F15_1 PEEvent[8];
};

struct ZDPStr_F16  //;虚拟专网用户名、密码 64B
{
    u8 User[32];
    u8 PW[32];
};

struct ZDPStr_F22 //;终端电能量费率 17B
{
    u8 RateN;
    u8 Rate[RATENUM][4];
};

struct PoPStr_F25 //;测量点基本参数 11B  
{
    u8 UMul[2];
    u8 IMul[2];
    u8 Un[2];
    u8 In;
    u8 KVAMax[3];
    u8 Type;
};

struct PoPStr_F26 //;测量点限值参数 57B
{
    u8 UHigh[2];      //;电压合格率判别参数
    u8 ULow[2];
    u8 UBreak[2];
    u8 UHH[2];       //;过压参数
    u8 GY_DLY;
    u8 GY_Ratio[2];
    u8 ULL[2];       //;欠压参数
    u8 QY_DLY;
    u8 QY_Ratio[2];
    u8 IHH[3];      //;过流参数    
    u8 GL_DLY;
    u8 GL_Ratio[2]; 
    u8 IH[3];        //;超额定电流参数
    u8 GBL_DLY;
    u8 GBL_Ratio[2]; 
    u8 I0High[3];   //;零序电流超限判别参数
    u8 G0L_DLY;
    u8 G0L_Ratio[2];     
    u8 SHH[3];      //;视在功率超上上限判别参数
    u8 SHH_DLY;
    u8 SHH_Ratio[2];  
    u8 SH[3];       //;视在功率超上限判别参数
    u8 SH_DLY;
    u8 SH_Ratio[2];  
    u8 UImb[2];     //;三相电压不平衡超限判别参数
    u8 UImb_DLY;
    u8 UImb_Ratio[2];  
    u8 IImb[2];     //;三相电流不平衡超限判别参数
    u8 IImb_DLY;
    u8 IImb_Ratio[2];
    u8 ULostT;
};

struct PoPStr_F27 //;测量点铜损铁损参数 24B
{
    u8 RA[2];  //;A相电阻
    u8 XA[2];  //;A相电抗
    u8 GA[2];  //;A相电导
    u8 BA[2];  //;A相电纳
    u8 RB[2];  
    u8 XB[2];  
    u8 GB[2];  
    u8 BB[2];  
    u8 RC[2];  
    u8 XC[2];  
    u8 GC[2];  
    u8 BC[2];    
};

struct PoPStr_F28 //;测量点功率因数分段限值 4B
{
    u8 Cos1[2];
    u8 Cos2[2];
};

struct PoPStr_F31 //;载波从节点附属节点地址  25B
{
    u8 NodeN;
    u8 NodeAddr[NODENUM][6];
};

struct ZDPStr_F33_1_1 //;4B
{   
    u8 RTDStartT[2]; //;允许抄表时段开始时间
    u8 RTDEndT[2];   //;允许抄表时段结束时间
};

struct ZDPStr_F33_1     //;30B
{
    u8 ComPort;
    u8 RCtr[2];
    u8 RDay[4];
    u8 RTime[2];
    u8 RTimeDis;
    u8 BroadcastT[3];
    u8 RMTNum;
    struct ZDPStr_F33_1_1  RMTSet[RMTNUM];
};

struct ZDPStr_F33 //;终端抄表运行参数设置 61B
{
    u8 PortN;
    struct ZDPStr_F33_1 PortP[PORTNUM];  
};

struct ZDPStr_F34_1  //;6B
{
    u8 ComPort;
    u8 ComCtr;
    u8 BaudRate[4];  
};

struct ZDPStr_F34 //;集中器下行通信模块的参数设置  13B
{
    u8 PortN;
    struct ZDPStr_F34_1 PortP[PORTNUM];  
};

struct ZDPStr_F35 //;台区集中抄表重点户设置 11B
{
    u8 UsrN;
    u8 UsrIndex[KEYMETNUM][2];  
};

struct ZDPStr_F37_1 //;终端级联通信参数
{
    u8 A1[2];
    u8 A2[2];
};

struct ZDPStr_F37 //;终端级联通信参数 19B
{
    u8 Port;
    u8 Control; 
    u8 MesOut100ms;
    u8 ByteOut10ms;
    u8 ReSendNum;
    u8 CysMin;
    u8 ZDNum;
    struct ZDPStr_F37_1 JLZDAddr[3];
     
};

struct ZDPStr_F38_1 //;1类数据配置设置 //;34B
{   
    u8 UsrSIndex;
    u8 InfoGrpN;
    u8 GrpFlag[32]; //;信息类的标志
};

struct ZDPStr_F38 //;1类和2类数据配置设置 
{
    u8 UsrBIndex;
    u8 GroupN;
    struct ZDPStr_F38_1 InfoGrp[USRSNUM];  //;数值范围从1~16 138B
     
};

struct GroupStr_F41_1 //;时段功控定值
{
    u8 SDIndex;
    u8 Value[8][2];
};

struct GroupStr_F41   //;时段功控定值 52B
{
    u8 Flag; //;方案标志
    struct GroupStr_F41_1 Group[3];
};

struct GroupStr_F42 //;厂休功控参数 6B
{
    u8 PValue[2];
    u8 Time[2];
    u8 DLY05H;
    u8 CtrFlag;
};

struct GroupStr_F44 //;营业报停控参数 8B
{
    u8 TimeS[3];
    u8 TimeE[3];
    u8 PValue[2];
};

struct GroupStr_F47 //;购电量控参数 17B
{
    u8 BuyIndex[4];
    u8 Flag;
    u8 BuyValue[4];
    u8 ArmValue[4];
    u8 CutValue[4];
};

struct ZDPStr_F59 //;电能表异常判别阈值设定 4B 
{
    u8  EnergyOver;
    u8  MeterFly;
    u8  MeterStop;
    u8  MeterTime;
};

struct ZDPStr_F60 //;谐波限值 44B
{
    u8 XBU_High[2];
    u8 XBU_High_Odd[2];
    u8 XBU_High_Even[2];
    u8 XBI_High[2];
    u8 XBI_High_2[2];
    u8 XBI_High_4[2];
    u8 XBI_High_6[2];
    u8 XBI_High_8[2];
    u8 XBI_High_10[2];
    u8 XBI_High_12[2];
    u8 XBI_High_14[2];
    u8 XBI_High_16[2];
    u8 XBI_High_18[2];
    u8 XBI_High_3[2];
    u8 XBI_High_5[2];
    u8 XBI_High_7[2];
    u8 XBI_High_9[2];
    u8 XBI_High_11[2];
    u8 XBI_High_13[2];
    u8 XBI_High_15[2];
    u8 XBI_High_17[2];
    u8 XBI_High_19[2];
};

struct ZDPStr_F62
{
    u8   Type;
    u16  Interval;
    u8   Retry;
    u8   DisconT;
};

struct TaskPStr_F65 //;89B
{
    u8  Cyc;
    u8  BaseTime[6];
    u8  Rate;
    u8  PnFnNum;
    u8  PnFn[20][4];
};   
    
struct PoPStr_F73 //;电容器参数 3B
{
    u8 Sign;
    u8 Cap[2];    
};    
    
struct PoPStr_F74 //;电容器投切运行参数 10B
{
    u8 PowerFactor[2];
    u8 InWP[3];
    u8 OutWP[3];
    u8 DLYSec;
    u8 DisMin;
};

struct PoPStr_F75 //;电容器保护参数 16B
{
    u8 UOut[2];
    u8 UOutDis[2];
    u8 ULess[2];
    u8 ULessDis[2];
    u8 AberI[2];
    u8 AberIDis[2];
    u8 AberU[2];
    u8 AberUDis[2];  
};

struct DCPortStr_F81 //;直流模拟量输入变比
{
  u8 ValueS[2];
  u8 ValueE[2];  
};

struct DCPortStr_F82 //;直流模拟量限值
{
  u8 ValueU[2];
  u8 ValueD[2];  
};
    
struct ZDPStr //;终端参数结构体
{
    struct ZDPStr_F1  F1 ;
    struct ZDPStr_F2  F2 ;
    struct ZDPStr_F3  F3 ;
    struct ZDPStr_F4  F4 ;
    struct ZDPStr_F5  F5 ;
    struct ZDPStr_F6  F6 ;
    struct ZDPStr_F7  F7 ;
    struct ZDPStr_F8  F8 ;
    struct ZDPStr_F9  F9 ;
    struct ZDPStr_F10 F10;
    struct ZDPStr_F11 F11;
    struct ZDPStr_F12 F12;
    struct ZDPStr_F13 F13;
    struct ZDPStr_F14 F14;
    struct ZDPStr_F15 F15;
    struct ZDPStr_F16 F16;
    u8 F17[2];
    u8 F18[12];
    u8 F19[1];
    u8 F20[1];
    u8 F21[25];
    struct ZDPStr_F22 F22;
    u8 F23[3];
    struct ZDPStr_F33 F33;
    struct ZDPStr_F34 F34;
    struct ZDPStr_F35 F35;
    u8 F36[4];
    struct ZDPStr_F37 F37;
    struct ZDPStr_F38 F38;
    struct ZDPStr_F38 F39;
    u8 F57[3];
    u8 F58[1];
    struct ZDPStr_F59 F59;
    struct ZDPStr_F60 F60;
    u8 F61[1];  
};

struct TaskPStr //;任务参数结构体
{
    struct TaskPStr_F65 F65[TASK1NUM];
    struct TaskPStr_F65 F66[TASK2NUM];
    u8 F67[TASK1NUM];
    u8 F68[TASK2NUM];
    
};

struct PoPStr  //;测量点参数结构体
{
    struct PoPStr_F25 F25;
    struct PoPStr_F26 F26;
    struct PoPStr_F27 F27;
    struct PoPStr_F28 F28;
    u8 F29[12];  
    u8 F30[1]; 
    struct PoPStr_F31 F31; 
    struct PoPStr_F73 F73[16]; 
    struct PoPStr_F74 F74;
    struct PoPStr_F75 F75;
    u8 F76[1];
};

struct GroupStr //;总加组参数
{
    struct GroupStr_F41 F41;
    struct GroupStr_F42 F42;
    u8 F43[1];
    struct GroupStr_F44 F44;
    u8 F45[1];
    u8 F46[4];
    struct GroupStr_F47 F47;
    u8 F48[1];
};

struct DCPortStr //;直流模拟量参数
{
    struct DCPortStr_F81 F81;
    struct DCPortStr_F82 F82;
    u8 F83[1];
};

//;struct GprsComStr
//;{
//;    u8 QuXian[2];
//;    u16 DiZhi;
//;};

struct ZDSStr_F5_1
{
    u8 PCtrNum;
    u8 PCtrESign;
    u8 PCtrStat;
    u8 EleCtrStat;
    u8 PCtrTStat;
    u8 EleCtrTStat;
};

struct ZDSStr_F5  //;50B
{
    u8 InSign;
    u8 AddSign;
    struct ZDSStr_F5_1 AddGRP[8]; 
};

struct ZDSStr_F6_1
{
    u8 PValue[2];
    u8 PDown;
    u8 PCutStat;
    u8 MonthCutStat;
    u8 BuyCutStat;
    u8 PCtrAStat;
    u8 EleCtrAStat;
};

struct ZDSStr_F6  //;64B
{
    u8 TelCutStat;
    u8 AlarmStat;
    u8 AddSign;
    struct ZDSStr_F6_1 AddGRP[8]; 
};

struct ZDSStr_F10 //;终端与主站当日、当月通信流量
{
    u32 Flux_Day;
    u32 Flux_Month;
};

struct ZDSStr_F11_1  //;终端集中抄表状态信息 
{
    u8 PortInd;
    u8 RMeterN[2];
    u8 Sign;
    u8 SucceedN;
    u8 ImpSucN;
    u8 RStartT[6];
    u8 REndT[6];  
};

struct ZDSStr_F11  //;终端集中抄表状态信息 37B
{
    u8 PortN;
    struct ZDSStr_F11_1 PortS[PORTNUM];  
};

struct ZDStatStr  //;终端数据
{   
    u8 F2[6];           //;终端日历时钟            F2
    u8 F3[31];       //;参数映射表              F3
    u8 F4[1];       //;终端通信状态            F4
    struct ZDSStr_F5 F5;     //;终端控制设置状态        F5
    struct ZDSStr_F6 F6;     //;终端当前控制状态        F6
    u8 F7[2];               //;终端重要事件计数器当前值 F7
    u8 F8[8];         //;终端事件标志状态         F8
    u8 F9[2];            //;终端状态量及变位标志     F9
    struct ZDSStr_F10 F10;  //;终端与主站当日、月流量   F10
    struct ZDSStr_F11 F11;  //;终端集中抄表状态信息    F11
};

struct Alarm_CS //;35Bye
{
    u8 Start[1];
    u16 BACOM; //;A相与B相通信中断
    u16 BCCOM; //;C相与B相通信中断
    u16 ZBCOM; //;终端与B相通信中断
    u16 ZMCOM; //;终端与主站通信中断    
    u16 ZDOFF;
    u16 UA125;
    u16 UA75;
    u16 UA50;
    u16 UA0;
    u16 IA125;
    u16 IA0;
    u16 UC125;
    u16 UC75;
    u16 UC50;
    u16 UC0;
    u16 IC125;
    u16 IC0;
};

/*----------------山西省电力公司通讯规约补充@1-----------------------*/
struct ZDPStr_F89
{
    u8 QuXian[2];
    u8 DiZhi[2];
};

struct ZDPStr_F90 //;缺相、断相等事件判断延时、飞走、停走事件比较测量点
{
    u8 ERCOCT[1]; //;事件发生判断时间
    u8 ERCOVT[1]; //;事件恢复判断时间
    u8 FlyPOI[1]; //;飞走、停走事件比较测量点
};

struct ZDPStr_F91  //;当前功率下浮控的告警时间和控制时间
{
    u8 XFKT1[1]; //;当前功率下浮控第1轮告警时间
    u8 XFKT2[1]; //;当前功率下浮控第2轮告警时间
    u8 XFKT3[1]; //;当前功率下浮控第3轮告警时间
    u8 XFKT4[1]; //;当前功率下浮控第4轮告警时间
    u8 XFKT[1];  //;当前功率下浮控的控制时间
};

/*----------------本公司通讯规约补充-----------------------*/
struct ZDPStr_F97  //;终端事件短信配置设置
{
    u8  UserSend[8];
    u8  AdmiSend[8];
};

struct  ZDPStrKZ   //;对终端参数扩展组12
{
    //;---------------组12-----------
    struct ZDPStr_F89 F89;
    struct ZDPStr_F90 F90;
    struct ZDPStr_F91 F91;
    //;---------------组13-----------
    struct ZDPStr_F97 F97; //;终端事件短信配置设置
    u8 F98[20]; //;用户短信头
    u8 F99[20]; //;用户短信尾
    u8 F100[20];//;管理员短信头
    u8 F101[20];//;管理员短信尾
};


struct AlarmHVM_Control  //;高压电能表告警相关状态
{
    u8 Status[15];  
    u8 ComAC;       
    u8 BAE,BAER;
    u8 BCE,BCER;
    u16 BJE,BJER;
    u16 PWROFF,PWROFFR;
    u8 AUH,AUHR;
    u8 AUL,AULR;
    u8 CUH,CUHR;
    u8 CUL,CULR;
    u8 AIH,AIHR;
    u8 AIL,AILR;
    u8 CIH,CIHR;
    u8 CIL,CILR;
    u8 PhaseA,PhaseC;
    u8 EnUpL;       
    u8 ComRev;     
    u8 UpCnt;       
    u8 IdleCnt;     
    u8 SendIdle;     
    u8 State;       
    u8 SendFlag;    
    u16 WaitAns;    
};

//;void __Point_struct(void){}
struct DJ_B2
{
    u8 Td_h[2];      
    u8 Data[4][2];
};

struct DJ_B3
{
    u8 Td_h[2];
    u8 Data[4][3];
};

struct DJ_B4
{
    u8 Td_h[2];
    u8 Data[4][4];
};



struct DF25DF33Str  //; T2_F25   32bye
{
    u8 ZPMax[3],ZPMax_T[3];   //;最大总有功功率及发生时间
    u8 APMax[3],APMax_T[3];   //;A最大有功功率及发生时间
    u8 BPMax[3],BPMax_T[3];   //;B最大有功功率及发生时间
    u8 CPMax[3],CPMax_T[3];   //;C最大有功功率及发生时间
    u8 ZP0Min[2];
    u8 AP0Min[2];
    u8 BP0Min[2];
    u8 CP0Min[2];   //;总ABC有功功率为0的持续时间
};


struct DF26F34Str   //; T2_F26
{
    u8 ZPMN[3],ZPMN_T[3];     //;总有功最大需量及发生时间
    u8 APMN[3],APMN_T[3];     //;A有功最大需量及发生时间
    u8 BPMN[3],BPMN_T[3];     //;B有功最大需量及发生时间
    u8 CPMN[3],CPMN_T[3];     //;C有功最大需量及发生时间
};

struct DF27F35Str    //; T2_F27
{
    u8 AVUUMin[2], AVLLMin[2], AVUMin[2], AVLMin[2], AVHGMin[2];  //;A相电压统计
    u8 BVUUMin[2], BVLLMin[2], BVUMin[2], BVLMin[2], BVHGMin[2];  //;B相电压统计
    u8 CVUUMin[2], CVLLMin[2], CVUMin[2], CVLMin[2], CVHGMin[2];  //;C相电压统计
    u8 AVMax[2],AVMax_T[3],AVMini[2],AVMini_T[3]; //;A相电压最值记录
    u8 BVMax[2],BVMax_T[3],BVMini[2],BVMini_T[3]; //;B相电压最值记录
    u8 CVMax[2],CVMax_T[3],CVMini[2],CVMini_T[3]; //;C相电压最值记录
    u8 AAver[2],BAver[2],CAver[2];   //;各相的平均电压
};

struct DF28Str  //; T2_F28 14B
{
    u8 IImbMin[2],VImbMin[2];
    u8 IImbV[2], IImbT[3];
    u8 UImbV[2], UImbT[3];
};

struct DF36Str  //; T2_F28 16B
{
    u8 IImbMin[2],VImbMin[2];
    u8 IImbV[2], IImbT[4];
    u8 UImbV[2], UImbT[4];
};

struct DF29F37Str  //; T2_F29 38B
{
    u8 AIUUMin[2],AIUMin[2];  //;A相电流统计
    u8 BIUUMin[2],BIUMin[2];  //;B相电流统计
    u8 CIUUMin[2],CIUMin[2];  //;C相电流统计
    u8 I0UMin[2];  //;零序电流越上限累计时间
    u8 AIMax[3],AIMax_T[3]; //;A相电流最大值记录
    u8 BIMax[3],BIMax_T[3]; //;B相电流最大值记录
    u8 CIMax[3],CIMax_T[3]; //;C相电流最大值记录
    u8 I0Max[3],I0Max_T[3]; //;零序电流最大值记录
};

struct DF30F38Str //; T2_F30 4B
{
    u8 SUUMin[2], SUMin[2];    //;视在功率越限累计时间
};

struct DF31Str  //; T2_F31 日负载率统计 10B
{
    u8 Vmax[2];
    u8 VmaxT[3];
    u8 Vmin[2];
    u8 VminT[3];  
};

struct DF39Str  //; T2_F39 日负载率统计 12B
{
    u8 Vmax[2];
    u8 VmaxT[4];
    u8 Vmin[2];
    u8 VminT[4];  
};

struct DF49F51Str  //;供电时间，复位累计次数 4B
{
    u8 PowerT[2];
    u8 Reset[2];
};

struct DF50F52Str  //;供电时间，复位累计次数 4B
{
    u8 MonthCutNum[1];
    u8 BuyCutNum[1];
    u8 PCutNum[1];
    u8 TelCutNum[1];
};

struct DF209Str  //;电能表远程控制状态及信息
{
    u8 MeterPowerStat[1];
    u8 MPCloseT[5];
    u8 MPCutT[5];  
};

struct DF213Str  //;月冻结电能表开关操作次数及时间
{
    u8 ProgramNum[2];
    u8 LastProgramT[5];
    u8 LidOpenNum[2];
    u8 LastOpenT[5];  
};

struct DF214Str  //;月冻结电能表参数修改次数及时间
{
    u8 CLKChangeNum[2];
    u8 CLastChangeT[5];
    u8 PeriodChangeNum[2];
    u8 PLastChangeT[5];  
};

struct PointDataStr  //;测量点数据
{
    u8 Start[1];
    u8 CB_Time[5];//;抄表时间
    u8 RC313[1];//;费率数
    
                          /*GDW376.1规约数据*/
    /*------------- F25 -----------*/
    u8 RB630[3],RB631[3],RB632[3],RB633[3];//;总,A,B,C瞬时有功功率(带符号)
    u8 RB640[3],RB641[3],RB642[3],RB643[3];//;瞬时无功功率(带符号)
    u8 RB650[2],RB651[2],RB652[2],RB653[2];//;功率因素
    u8 RB611[2],RB612[2],RB613[2];         //;电压
    u8 RB621[3],RB622[3],RB623[3];         //;各相瞬时电流
    u8 RB6E5[3];                           //;零序电流
    u8 RB660[3],RB661[3],RB662[3],RB663[3];//;视在功率

    /*------------- F26 -----------*/
    u8 RB310[2],RB311[2],RB312[2],RB313[2];//;断相累计次数
    u8 RB320[3],RB321[3],RB322[3],RB323[3];//;断相累计时间
    u8 RB330[4],RB331[4],RB332[4],RB333[4];//;最近一次断相开始时间
    u8 RB340[4],RB341[4],RB342[4],RB343[4];//;最近一次断相结束时间

    /*------------- F27 -----------*/
    u8 RDayTime[6];       //;电能表日历时钟
    u8 RB216[4];          //;电池工作时间
    u8 RB212[3],RB21A[6]; //;当前编程次数及最后一次编程发生时间
    u8 RB214[3],RB21C[6]; //;电表清零总次数及最后一次发生时间
    u8 RB213[3],RB21B[6]; //;需量清零总次数及最后一次发生时间
    u8 RB215[3],RB21D[6]; //;事件清零总次数及最后一次发生时间
    u8 RB217[3],RB21F[6]; //;校时总次数及最后一次发生时间
    
    /*-------------- F28 ---------*/
    u8 RC020[2]; //;电表运行状态字变位标志1
    u8 RC021[2]; //;电表运行状态字变位标志2
    u8 RC022[2]; //;电表运行状态字变位标志3
    u8 RC023[2]; //;电表运行状态字变位标志4
    u8 RC024[2]; //;电表运行状态字变位标志5
    u8 RC025[2]; //;电表运行状态字变位标志6
    u8 RC026[2]; //;电表运行状态字变位标志7
    u8 RC027[2]; //;电表运行状态字1
    u8 RC028[2]; //;电表运行状态字2
    u8 RC029[2]; //;电表运行状态字3
    u8 RC02A[2]; //;电表运行状态字4
    u8 RC02B[2]; //;电表运行状态字5
    u8 RC02C[2]; //;电表运行状态字6
    u8 RC02D[2]; //;电表运行状态字7
    
    /*------------- F29 -----------*/
    u8 R2000[5],R2001[5]; //;当前铜损、铁损有功总电能示值
    
    /*------------- F30 -----------*/
    u8 R2010[5],R2011[5]; //;上一日铜损、铁损有功总电能示值
    
    /*------------- F31 -----------*/
    u8 R30A1[5],R30A2[5]; //;A相正向、反向有功总电能示值
    u8 R30A3[4],R30A4[4]; //;A相组合无功1/2电能示值
    u8 R30B1[5],R30B2[5]; //;B相正向、反向有功总电能示值
    u8 R30B3[4],R30B4[4]; //;B相组合无功1/2电能示值
    u8 R30C1[5],R30C2[5]; //;C相正向、反向有功总电能示值
    u8 R30C3[4],R30C4[4]; //;C相组合无功1/2电能示值
    
     /*------------- F32 -----------*/
    u8 R31A1[5],R31A2[5]; //;上一结算日A相正向、反向有功总电能示值
    u8 R31A3[4],R31A4[4]; //;上一结算日A相组合无功1/2电能示值
    u8 R31B1[5],R31B2[5]; //;上一结算日B相正向、反向有功总电能示值
    u8 R31B3[4],R31B4[4]; //;上一结算日B相组合无功1/2电能示值
    u8 R31C1[5],R31C2[5]; //;上一结算日C相正向、反向有功总电能示值
    u8 R31C3[4],R31C4[4]; //;上一结算日C相组合无功1/2电能示值
    
    /*------------- F33 -----------*/
    u8 R9010[5],R9011[5],R9012[5],R9013[5],R9014[5]; //; 当前正向有功各费率电能
    u8 R9110[4],R9111[4],R9112[4],R9113[4],R9114[4]; //;当前正向无功各费率电能
    u8 R9130[4],R9131[4],R9132[4],R9133[4],R9134[4]; //;一象限无功各费率电能
    u8 R9140[4],R9141[4],R9142[4],R9143[4],R9144[4]; //;四象限无功各费率电能
    
    /*------------- F34 -----------*/
    u8 R9020[5],R9021[5],R9022[5],R9023[5],R9024[5]; //;当前反向有功各费率电能
    u8 R9120[4],R9121[4],R9122[4],R9123[4],R9124[4]; //;当前反向无功各费率电能
    u8 R9150[4],R9151[4],R9152[4],R9153[4],R9154[4]; //;二象限无功各费率电能
    u8 R9160[4],R9161[4],R9162[4],R9163[4],R9164[4]; //;三象限无功各费率电能
    
    /*------------- F35 -----------*/
    u8 RA010[3],RA011[3],RA012[3],RA013[3],RA014[3];//;当前正向有功最大需量
    u8 RB010[4],RB011[4],RB012[4],RB013[4],RB014[4];//;当前正向有功最大需量发生时间
    u8 RA110[3],RA111[3],RA112[3],RA113[3],RA114[3];//;当前正向无功最大需量
    u8 RB110[4],RB111[4],RB112[4],RB113[4],RB114[4];//;当前正向无功最大需量发生时间
    
    /*------------- F36 -----------*/
    u8 RA020[3],RA021[3],RA022[3],RA023[3],RA024[3];//;当前反向有功各费率最大需量
    u8 RB020[4],RB021[4],RB022[4],RB023[4],RB024[4];//;当前反向有功各费率最大需量发生时间
    u8 RA120[3],RA121[3],RA122[3],RA123[3],RA124[3];//;当前反向无功各费率最大需量
    u8 RB120[4],RB121[4],RB122[4],RB123[4],RB124[4];//;当前反向无功各费率最大需量发生时间
    
    /*------------- F37 -----------*/
    u8 R9410[5],R9411[5],R9412[5],R9413[5],R9414[5];//;上月正向有功各费率电能
    u8 R9510[4],R9511[4],R9512[4],R9513[4],R9514[4];//;上月正向无功各费率电能
    u8 R9530[4],R9531[4],R9532[4],R9533[4],R9534[4];//;上月一象限无功各费率电能
    u8 R9540[4],R9541[4],R9542[4],R9543[4],R9544[4];//;上月四象限无功各费率电能
    
    /*------------- F38 -----------*/
    u8 R9420[5],R9421[5],R9422[5],R9423[5],R9424[5];//;上月反向有功各费率电能
    u8 R9520[4],R9521[4],R9522[4],R9523[4],R9524[4];//;上月反向无功各费率电能
    u8 R9550[4],R9551[4],R9552[4],R9553[4],R9554[4];//;上月二象限无功各费率电能
    u8 R9560[4],R9561[4],R9562[4],R9563[4],R9564[4];//;上月三象限无功各费率电能
    
    /*------------- F39 -----------*/
    u8 RA410[3],RA411[3],RA412[3],RA413[3],RA414[3];//;上月正向有功最大需量
    u8 RB410[4],RB411[4],RB412[4],RB413[4],RB414[4];//;上月正向有功各费率最大需量发生时间
    u8 RA510[3],RA511[3],RA512[3],RA513[3],RA514[3];//;上月正向无功最大需量
    u8 RB510[4],RB511[4],RB512[4],RB513[4],RB514[4];//;上月正向无功各费率最大需量发生时间
    
    /*------------- F40 -----------*/
    u8 RA420[3],RA421[3],RA422[3],RA423[3],RA424[3];//;上月反向有功各费率最大需量
    u8 RB420[4],RB421[4],RB422[4],RB423[4],RB424[4];//;上月反向有功各费率最大需量发生时间
    u8 RA520[3],RA521[3],RA522[3],RA523[3],RA524[3];//;上月反向无功各费率最大需量
    u8 RB520[4],RB521[4],RB522[4],RB523[4],RB524[4];//;上月反向无功各费率最大需量发生时间
    

    /*------------- F41~F48 -----------*/
    u8 R9010_D[4],R9011_D[4],R9012_D[4],R9013_D[4],R9014_D[4];//;当日正向有功各费率电能
    u8 R9110_D[4],R9111_D[4],R9112_D[4],R9113_D[4],R9114_D[4];//;当日正向无功各费率电能
    u8 R9020_D[4],R9021_D[4],R9022_D[4],R9023_D[4],R9024_D[4];//;当日反向有功各费率电能
    u8 R9120_D[4],R9121_D[4],R9122_D[4],R9123_D[4],R9124_D[4];//;当日反向无功各费率电能
    u8 R9010_M[4],R9011_M[4],R9012_M[4],R9013_M[4],R9014_M[4];//;当月正向有功各费率电能
    u8 R9110_M[4],R9111_M[4],R9112_M[4],R9113_M[4],R9114_M[4];//;当月正向无功各费率电能
    u8 R9020_M[4],R9021_M[4],R9022_M[4],R9023_M[4],R9024_M[4];//;当月反向有功各费率电能
    u8 R9120_M[4],R9121_M[4],R9122_M[4],R9123_M[4],R9124_M[4];//;当月反向无功各费率电能
    
    /*-------------- F49 ------------*/
    u8 RB671[2],RB672[2],RB673[2]; //;A、B、C电压相位角
    u8 RB681[2],RB682[2],RB683[2]; //;A、B、C电流相位角

    //;*------------- F89~F116 ----------- 366B
    struct DJ_B3  ODJ_B630,  ODJ_B631,  ODJ_B632,  ODJ_B633;  //;上一小时冻结总,A,B,C有功功率
    struct DJ_B3  ODJ_B640,  ODJ_B641,  ODJ_B642,  ODJ_B643;  //;上一小时冻结总,A,B,C无功功率
    struct DJ_B2  ODJ_B611,  ODJ_B612,  ODJ_B613;             //;上一小时冻结总,A,B,C相电压
    struct DJ_B3  ODJ_B621,  ODJ_B622,  ODJ_B623,  ODJ_B6E5;  //;上一小时冻结总,A,B,C相电流以及中线电流
    struct DJ_B4  ODJ_9010,  ODJ_9110,  ODJ_9020,  ODJ_9120;  //;上一小时冻结日电能量
    struct DJ_B4  ODJ_9010_1,ODJ_9110_1,ODJ_9020_1,ODJ_9120_1;//;上一小时冻结总电能量示值
    struct DJ_B2  ODJ_B650,  ODJ_B651,  ODJ_B652  ,ODJ_B653;  //;上一小时冻结总,A,B,C相功率因数

    struct DJ_B3  NDJ_B630  ,NDJ_B631  ,NDJ_B632  ,NDJ_B633;  //;当前小时冻结总,A,B,C有功功率
    struct DJ_B3  NDJ_B640  ,NDJ_B641  ,NDJ_B642  ,NDJ_B643;  //;当前小时冻结总,A,B,C无功功率
    struct DJ_B2  NDJ_B611  ,NDJ_B612  ,NDJ_B613  ;           //;当前小时冻结A,B,C相电压
    struct DJ_B3  NDJ_B621  ,NDJ_B622  ,NDJ_B623  ,NDJ_B6E5;  //;当前小时冻结A,B,C相电流以及中线电流
    struct DJ_B4  NDJ_9010  ,NDJ_9110  ,NDJ_9020  ,NDJ_9120;  //;当前小时冻结日电能量
    struct DJ_B4  NDJ_9010_1,NDJ_9110_1,NDJ_9020_1,NDJ_9120_1;//;当前小时冻结总电能示值
    struct DJ_B2  NDJ_B650  ,NDJ_B651  ,NDJ_B652  ,NDJ_B653;  //;当前小时冻结总,A,B,C相功率因数
    
    /*----------- F153~F160 ------------*/
    u8 R5010[5],R5011[5],R5012[5],R5013[5],R5014[5]; //;第一时区冻结正向有功电能示值
    u8 R5020[5],R5021[5],R5022[5],R5023[5],R5024[5]; //;第二时区冻结正向有功电能示值
    u8 R5030[5],R5031[5],R5032[5],R5033[5],R5034[5]; //;第三时区冻结正向有功电能示值
    u8 R5040[5],R5041[5],R5042[5],R5043[5],R5044[5]; //;第四时区冻结正向有功电能示值
    u8 R5050[5],R5051[5],R5052[5],R5053[5],R5054[5]; //;第五时区冻结正向有功电能示值
    u8 R5060[5],R5061[5],R5062[5],R5063[5],R5064[5]; //;第六时区冻结正向有功电能示值
    u8 R5070[5],R5071[5],R5072[5],R5073[5],R5074[5]; //;第七时区冻结正向有功电能示值
    u8 R5080[5],R5081[5],R5082[5],R5083[5],R5084[5]; //;第八时区冻结正向有功电能示值

    /*--------------F165~F166-----------------*/
    u8 DB212[2],DB21A[5];  //;编程总次数及最近一次时间 by zrt 20130809
    u8 RB218[2],RB219[5];  //;开尾盖次数及最后一次打开时间by zrt 20130808		
    u8 DB217[2],DB21F[5]; //;校时总次数及最近一次时间by zrt 20130814
    u8 RB220[2],RB221[5];	//;时段参数修改总次数及最近一次时间by zrt 20130814

    u8 RB222[3],RB223[5];  //;总掉电次数及最近一次时间by zrt 20310828
    u8 RB224[3];                 //;费率时段表编程次数by zrt 20130910
    
    //;T2_F31 日统计数据，共207B
    struct DF25DF33Str  DF25;  //;用于日冻结与月冻结
    struct DF26F34Str   DF26;
    struct DF27F35Str   DF27;
    struct DF28Str      DF28;
    struct DF29F37Str   DF29;
    struct DF30F38Str   DF30;
    struct DF31Str      DF31;
    struct DF49F51Str   DF49;
    struct DF50F52Str   DF50;
    struct DF209Str     DF209;
    //;月统计数据，共228B
    struct DF25DF33Str  DF33;  //;用于日冻结与月冻结
    struct DF26F34Str   DF34;
    struct DF27F35Str   DF35;
    struct DF36Str      DF36;
    struct DF29F37Str   DF37;
    struct DF30F38Str   DF38;
    struct DF39Str      DF39;
    struct DF49F51Str   DF51;
    struct DF50F52Str   DF52;
    struct DF213Str     DF213;
    struct DF214Str     DF214;

    u8 Z9010_D[5],Z9011_D[5],Z9012_D[5],Z9013_D[5],Z9014_D[5];//;当日零点正向有功各费率电能
    u8 Z9020_D[5],Z9021_D[5],Z9022_D[5],Z9023_D[5],Z9024_D[5];//;当日零点反向有功各费率电能
    u8 Z9110_D[4],Z9111_D[4],Z9112_D[4],Z9113_D[4],Z9114_D[4];//;当日零点正向无功各费率电能
    u8 Z9120_D[4],Z9121_D[4],Z9122_D[4],Z9123_D[4],Z9124_D[4];//;当日零点反向无功各费率电能

    u8 Z9010_M[5],Z9011_M[5],Z9012_M[5],Z9013_M[5],Z9014_M[5];//;当月零点正向有功各费率电能
    u8 Z9020_M[5],Z9021_M[5],Z9022_M[5],Z9023_M[5],Z9024_M[5];//;当月零点反向有功各费率电能
    u8 Z9110_M[4],Z9111_M[4],Z9112_M[4],Z9113_M[4],Z9114_M[4];//;当月零点正向无功各费率电能
    u8 Z9120_M[4],Z9121_M[4],Z9122_M[4],Z9123_M[4],Z9124_M[4];//;当月零点反向无功各费率电能

    u8 DB212_M[2],DB21A_M[5];  //;	当月编程总次数及最近一次时间 by zrt 20130815
    u8 RB218_M[2],RB219_M[5];	  //;当月开尾盖次数及最后一次打开时间
    u8 DB217_M[2],DB21F_M[5];  //;当月校时总次数及最近一次时间
    u8 RB220_M[2],RB221_M[5];	  //;当月时段参数修改总次数及最近一次时间

    /*others*/
//;    u8 RB660[3]; //;视在功率
//;    u8 RB670[1]; //;电源接线方式
//;    u8 RB710[2]; //;失压次数
    u8 RC040[3],RC041[3]; //;电压，电流变比,
    u8 RC030[3],RC031[3],RC032[6];  //;电表常数,表号
    u8 RC141[6]; //;标定电压
    u8 RC142[6]; //;标定电流 ASCII
    u8 RC117[2],RC118[2],RC119[2];//;自动抄表日期
    u8 RB710[3]; //;失压次数
//;    u8 RC141[3];//;标定电压
//;    u8 RC331[3],RC332[3],RC333[3],RC334[3],RC335[3],RC336[3],RC337[3],RC338[3];//;当前时段表起始时间及费率号

    /*前一分钟数据*/
    u8 O9010[5]; //;正向有功总电表示值，用于检测电能示度下降等
//;    u8 OC331[3],OC332[3],OC333[3],OC334[3],OC335[3],OC336[3],OC337[3],OC338[3];//;前一分钟时段表起始时间及费率号
    u8 OC040[3],OC041[3]; //;电压，电流变比,
    u8 OA010[3],OA020[3]; //;正向有功，无功最大需量
    u8 OB212[3]; //;编程总次数
    u8 OB21A[6]; //;最近第一次编程时间
    u8 OB213[3]; //;最大需量清零次数
    u8 OB310[2]; //;总断相次数
    u8 OB710[3]; //;失压次数
    u8 OC030[3],OC031[3]; //;脉冲常数
    u8 OC117[2],OC118[2],OC119[2];//;自动抄表日期

    u8 OB222[3],OB223[5];  // 掉电总次数及最近一次发生时间by zrt 20130828

    u8 OB224[3];                 //时段表编程次数

//    u8 OC027[2];  //;电表电池欠压标识by zrt 20130829
	
    u8 End[1];
};

struct MeterData /*来自电能表的数据适合485规约*/
{
    u8 Start[1];
    u8 GYType[1]; //;当前采用的规约类型
    u8 CB_Time[5];
    
    u8 R9010[4],R9011[4],R9012[4],R9013[4],R9014[4];//;当前正向有功各费率电能
    u8 R9020[4],R9021[4],R9022[4],R9023[4],R9024[4];//;当前反向有功各费率电能

    u8 R9110[4],R9111[4],R9112[4],R9113[4],R9114[4];//;当前正向无功各费率电能
    u8 R9120[4],R9121[4],R9122[4],R9123[4],R9124[4];//;当前反向无功各费率电能

    u8 R9130[4],R9131[4],R9132[4],R9133[4],R9134[4];//;当前一象限无功各费率电能
    u8 R9140[4],R9141[4],R9142[4],R9143[4],R9144[4];//;当前四象限无功各费率电能
    u8 R9150[4],R9151[4],R9152[4],R9153[4],R9154[4];//;当前二象限无功各费率电能
    u8 R9160[4],R9161[4],R9162[4],R9163[4],R9164[4];//;当前三象限无功各费率电能
    
    u8 R30A1[4]; //;A相正有功总电能示值  当前
    u8 R30B1[4]; //;B相正有功总电能示值
    u8 R30C1[4]; //;C相正有功总电能示值

    u8 R30A2[4]; //;A相反有功总电能示值  by zrt 20130909
    u8 R30B2[4]; //;B相反有功总电能示值
    u8 R30C2[4]; //;C相反有功总电能示值	

    u8 R30A3[4]; //;A相正无功总电能示值
    u8 R30B3[4]; //;B相正无功总电能示值
    u8 R30C3[4]; //;C相正无功总电能示值

    u8 R30A4[4]; //;A相反无功总电能示值  
    u8 R30B4[4]; //;B相反无功总电能示值
    u8 R30C4[4]; //;C相反无功总电能示值		

    u8 R9410[4],R9411[4],R9412[4],R9413[4],R9414[4];//;上月正向有功各费率电能
    u8 R9420[4],R9421[4],R9422[4],R9423[4],R9424[4];//;上月反向有功各费率电能

    u8 R9510[4],R9511[4],R9512[4],R9513[4],R9514[4];//;上月正向无功各费率电能
    u8 R9520[4],R9521[4],R9522[4],R9523[4],R9524[4];//;上月反向无功各费率电能

    u8 R9530[4],R9531[4],R9532[4],R9533[4],R9534[4];//;上月一象限无功各费率电能
    u8 R9540[4],R9541[4],R9542[4],R9543[4],R9544[4];//;上月四象限无功各费率电能
    u8 R9550[4],R9551[4],R9552[4],R9553[4],R9554[4];//;上月二象限无功各费率电能
    u8 R9560[4],R9561[4],R9562[4],R9563[4],R9564[4];//;上月三象限无功各费率电能

    u8 R31A1[4]; //;A相正有功总电能示值  上月
    u8 R31B1[4]; //;B相正有功总电能示值
    u8 R31C1[4]; //;C相正有功总电能示值

    u8 R31A2[4]; //;A相反有功总电能示值  by zrt 20130909
    u8 R31B2[4]; //;B相反有功总电能示值
    u8 R31C2[4]; //;C相反有功总电能示值		

    u8 R31A3[4]; //;A相正无功总电能示值 
    u8 R31B3[4]; //;B相正无功总电能示值
    u8 R31C3[4]; //;C相正无功总电能示值

    u8 R31A4[4]; //;A相反无功总电能示值 
    u8 R31B4[4]; //;B相反无功总电能示值
    u8 R31C4[4]; //;C相反无功总电能示值		
    
    u8 RA010[3],RB010[5],RA011[3],RB011[5],RA012[3],RB012[5];//;正有功最大需量及时间
    u8 RA013[3],RB013[5],RA014[3],RB014[5];
    u8 RA020[3],RB020[5],RA021[3],RB021[5],RA022[3],RB022[5];//;反有功最大需量及时间
    u8 RA023[3],RB023[5],RA024[3],RB024[5];
    
    u8 RA110[3],RB110[5],RA111[3],RB111[5],RA112[3],RB112[5];//;正无功最大需量及时间
    u8 RA113[3],RB113[5],RA114[3],RB114[5];
    u8 RA120[3],RB120[5],RA121[3],RB121[5],RA122[3],RB122[5];//;反无功最大需量及时间
    u8 RA123[3],RB123[5],RA124[3],RB124[5];

    u8 RA410[3],RB410[5],RA411[3],RB411[5],RA412[3],RB412[5];//;上月正有功最大需量及时间
    u8 RA413[3],RB413[5],RA414[3],RB414[5];
    u8 RA420[3],RB420[5],RA421[3],RB421[5],RA422[3],RB422[5];//;上月反有功最大需量及时间
    u8 RA423[3],RB423[5],RA424[3],RB424[5];
    
    u8 RA510[3],RB510[5],RA511[3],RB511[5],RA512[3],RB512[5];//;上月正无功最大需量及时间
    u8 RA513[3],RB513[5],RA514[3],RB514[5];
    u8 RA520[3],RB520[5],RA521[3],RB521[5],RA522[3],RB522[5];//;上月反无功最大需量及时间
    u8 RA523[3],RB523[5],RA524[3],RB524[5];

    u8 RC010[4],RC011[3]; //;日期及周次,时间
    u8 RB216[4];          //;电池工作时间
    u8 RB212[3],RB21A[6]; //;当前编程次数及最后一次编程发生时间
    u8 RB214[3],RB21C[6]; //;电表清零总次数及最后一次发生时间
    u8 RB213[3],RB21B[6]; //;需量清零总次数及最后一次发生时间
    u8 RB215[3],RB21D[6]; //;事件清零总次数及最后一次发生时间
    u8 RB217[3],RB21F[10]; //;校时总次数及最后一次发生时间

     u8 RB310[3],RB311[3],RB312[3],RB313[3];//;断相累计次数
    u8 RB320[3],RB321[3],RB322[3],RB323[3];//;断相累计时间
    u8 RB330[6],RB331[6],RB332[6],RB333[6];//;最近一次断相开始时间
    u8 RB340[6],RB341[6],RB342[6],RB343[6];//;最近一次断相结束时间

    u8 RB611[2],RB612[2],RB613[2];//;各相瞬时电压
    u8 RB621[3],RB622[3],RB623[3];//;各相瞬时电流
    u8 RB630[3],RB631[3],RB632[3],RB633[3];//;有功功率
    u8 RB640[3],RB641[3],RB642[3],RB643[3];//;无功功率
    u8 RB660[3],RB661[3],RB662[3],RB663[3];//;视在功率
    u8 RB650[2],RB651[2],RB652[2],RB653[2];//;功率因素
    u8 RB671[2],RB672[2],RB673[2]; //;A、B、C电压相位角
    
    u8 RC027[2]; //;电表运行状态字1
    u8 RC028[2]; //;电表运行状态字2
    u8 RC029[2]; //;电表运行状态字3
    u8 RC02A[2]; //;电表运行状态字4
    u8 RC02B[2]; //;电表运行状态字5
    u8 RC02C[2]; //;电表运行状态字6
    u8 RC02D[2]; //;电表运行状态字7

    u8 RC02E[1];//读事件状态
    
    u8 RC030[3],RC031[3],RC032[6];  //;电表常数,表号
    u8 RC117[2],RC118[2],RC119[2];//;自动抄表日期
    u8 RC313[1];//;费率数
    u8 RC141[6]; //;标定电压
    u8 RC142[6]; //;标定电流 ASCII
    u8 RC14A[10]; //;电表型号DTZ25 三相四线，DSZ25 三相三线
    u8 RC040[3], RC041[3]; //;电压互感器倍率，电流互感器倍率
    u8 RB710[3]; //;失压总次数
    
    u8 RB218[3],RB219[6];  //;开尾盖次数及最后一次打开时间by zrt 20130808    
    u8 RB220[3],RB221[6];  //;电能表时段参数修改次数及最后一次修改时间by zrt20130814

    u8 RB222[3],RB223[6];  //;总掉电次数及最后一次时间by zrt 20130828

    u8 RB224[3];                 //;费率时段表编程次数
    
    u8 End[1];
};

/*----------------------------------------------------------------------------*
 |  事件相关结构
 *----------------------------------------------------------------------------*/
//;void __ERC_struct(void) {}
struct ERCSMSCtrl     //;事件发送短信控制字
{
    u16 Pn;
    u8 ERCNo;        //;当前正在处理事件的代码
    u8 Status;       //;事件发送状态
    u8 Receiver;
    u8 Index;        //;发送到第几个号码
    u8 PhoneNo[8];   //;当前发送的手机号码
    u8 Len;
};

struct ERCUpCtrl     //;事件上送主站控制字
{
    u8 UpIndex;
    u8 SendERCNum;       //;上送ERC个数
    u8 Status;      //;事件发送状态
    u8 ReSedFlag;   //;重发标志
    u8 SendNum;      //;上送次数累计
    u8 WatiAns;      //;等待时间累计
    u8 PmStat;       //;上送状态
    u8 OVT;
};


struct ERCSaveCtrl   //;事件处理控制字
{
    u16  Pn;     //;事件处理状态
};

struct ERC1Data
{
    u8 Status[1];
    u8 OTime[5];
    u8 Mark[1];
    u8 Data[8];
};

struct ERC3Data
{
    u8 Status[1];
    u8 Num;           //;标识个数
    u8 OTime[5];      //;发生时间
    u8 HostAdr;       //;主站地址
    u8 Data[8][4];    //;一次最多记录8个单元标识
};

struct ERC8Data
{
    u8 Status[1];
    u8 Mark[1];      //;异常标志
};

struct ERC10Data    //;有四种分类事件
{
    u8 Status[4];    //;断相发生的处理状态  bit0:表示事件发生; bit1:表示事件保存完成; bit2表示事件上送完成; bit3表示事件发送短信完成
    u8 Status_O;    //;事件的旧状态
    u8 Status_N;    //;事件的新状态
    u8 DXOM;        //;断相发生处理状态
    u8 DXRM;        //;断相恢复处理状态
    u8 SYOM;        //;失压发生处理状态
    u8 SYRM;        //;失压恢复处理状态
    u8 Data[20];    //;事件的数据
};

struct ERC13Data
{
    u8 Status[1];
    u8 Mark[1];
};


struct ERC14Data
{
    u8 Status[2];
    u8 PTOTime[5];  //;停电发生时间
    u8 PLOTime[5];  //;来电发生时间
};


struct ERC17Data
{
    u8 Status[2];
    u8 Status_O;
    u8 Status_N;
    u8 BPH_OM;
    u8 BPH_RM;
    u8 Data[19];
    u16 Upn;
    u16 Ipn;
};

struct ERC20Data
{
    u8 Status[1];
    u8 OTime[5];
    u8 PW[16];
    u8 MSA[1];
};

struct ERC21Data
{
    u8 Status[1];
    u8 State; //;Bit0 :Port1故障，Bit1: Port2故障
    u8 OTime[5];
    u8 Data[1];
};

struct ERC24Data
{
    u8 Status[4];
    u8 Status_O;
    u8 Status_N;
    u8 YSSXOM;   //;(bit0:2)越上上限事件发生标志Occur Mark;  (bit3:7) 越上上限发生的处理标志
    u8 YSSXRM;   //;越上上限Recor Mark
    u8 YXXXOM;   //;越下下限Occur Mark
    u8 YXXXRM;   //;越下下限Recor Mark
    u8 Data[6];
};

struct ERC25Data
{
    u8 Status[4];
    u8 Status_O;
    u8 Status_N;
    u8 YSSXOM;   //;(bit0:2)越上上限Occur Mark;  (bit3:7) 越上上限处理标志
    u8 YSSXRM;   //;越上上限Recor Mark
    u8 YSXOM;    //;越上限Occur   Mark
    u8 YSXRM;    //;越上上限Recor Mark
    u8 Data[9];
};


struct ERC26Data
{
    u8 Status[4];
    u8 Status_O;
    u8 Status_N;
    u8 Data[6];
};

struct ERC27Data
{
    u8 Status[1];
    u8 Data[10];
};

struct ERC29Data
{
    u8 Status[1];
    u8 Data[11];
};

struct ERCPnData            //;测量点事件数据结构
{
    u16 Pn;                  //;测量点号
    u8 OTime[5];            //;发生时间
    struct ERC8Data  ERC8;
    struct ERC10Data ERC10;
    struct ERC13Data ERC13;
    struct ERC17Data ERC17;
    struct ERC24Data ERC24;
    struct ERC25Data ERC25;
    struct ERC26Data ERC26;
    struct ERC27Data ERC27;
    struct ERC29Data ERC29;
};

struct ERCP0Data           //;终端事件数据结构
{
    struct ERC1Data  ERC1;
    struct ERC3Data  ERC3;
    struct ERC14Data ERC14;
    struct ERC20Data ERC20;
    struct ERC21Data ERC21;
};


/*----------------------------------------------------------------------------*
 |  任务相关结构
 *----------------------------------------------------------------------------*/
//;void __Task_struct(void) {}
struct Task1_Time
{
    u8 Td1[5];      //;1号任务上送时间
    u8 Td2[5];      //;2号任务上送时间
    u8 Td3[5];      //;3号任务上送时间
    u8 Td4[5];      //;4号任务上送时间 
};

struct Task2_Td
{
    u8 Td_S[5];    //;上送时间
    u8 Td_SO[5];    //;上一次上送的日冻结的时间
    u8 Td_C[5];    //;上一次上送的曲线冻结的时间
};

struct Task2_Time
{
    struct Task2_Td Td1; //;1号任务上送时间
    struct Task2_Td Td2; //;2号任务上送时间
    struct Task2_Td Td3; //;3号任务上送时间
    struct Task2_Td Td4; //;4号任务上送时间 
};

struct Task_Control             //;任务控制参数
{
    u8 Switch;                  //;任务开关
    u8 Status;                  //;任务处理状态
    u8 TaskTn;                  //;当前处理任务的类型
    u8 TaskNO;                  //;当前处理的任务号
    u8 T1Over[1];               //;当前1类任务处理完成标志
    u8 T2Over[1];               //;当前2类任务处理完成标志
    struct Task1_Time T1;       //;1类任务上送时间
    struct Task2_Time T2;       //;2类任务上送时间
    u8 OVT;                     //;任务处理超时 
};


//;void __DJ_struct(void){}
struct DJ_Control //;冻结控制参数
{
    u8 Index;      //;用于保存冻结数据要保存到那一小块
    u8 StInd;      //;冻结数据的起始顺序号
};
struct QX_Control //;曲线控制参数
{
    u16 Index;      //;用于保存曲线数据要保存到那一小块
    u16 StInd;      //;曲线数据的起始顺序号
};

struct ERC_Control //;曲线控制参数
{
    u16 Index;      //;用于保存事件数据要保存到那一小块
    u16 StInd;      //;事件数据的起始顺序号
};

struct DJ_SAVE_CTRL
{
    u8  Status;
    u16 Pn;
};

/*----------------------------------------------------------------------------*
 |  规约处理的控制
 *----------------------------------------------------------------------------*/
//;void __Ctrl_struct(void){}
struct GuiYueCtrl    //;规约处理的控制
{
    u8  RecOrd;      //;表示当前接收到主站抄表命令
    u16 Index_Answer;//;用于指示当前应答到那一个数据
    u8  FSeq;        //;用于存储当前的帧序号
    u8  ISeq;        //;启动站帧计数器PFC
    u8  UpLNum;      //;用于记录同一个通道主动上送同一条命令的次数
    u8  WaitUpAnsT;  //;用于等待主动上送应答的时间计时
    u8  TaskUL;      //;用于表示当前一分钟以内是否已经通过执行过任务主动上送的程序
    u8  ERC_Index;   //;当前上送到第几个事件
    u8  ERC_Num;     //;事件个数
    u8  ERC_F;       //;第一帧分帧标志first
    u8  ERC_L;       //;最后一帧分帧标志Last
    u8  TaskNum;     //;当前那一个任务在分帧上传
    u8  Status;
    u8  ListNo;
    u8  AFNList[7];
    u8  AFN02_FN;    //;
    u8  AFN10;
    u8  AFNNum;      //;分帧次数不能超过10次
    u8  FileSign;    //;接收文件标识
    u16 FrameIndex;  //;已接收帧序号
    u32  FileSize;    //;文件大小
    u32  PageCur;  //;当前写入页偏移
    u32  SizeCur;  //;当前接收字节数
    
};

struct Relay_Control  //;中继控制参数
{
    u8   Status; //;收到中继命令的标志 idle/busy
    u8   Channel;   //;right / wrong
    u8   A3;     //;主站地址与组地址
    u8   SEQ;    //;帧序列域
    u8   Port;
    u8   OvTime;
    u8   BPS;
    u8   Len;
    char Data[256];
    u8   Tp[6];   //20131106
};

struct ZDCBCtrl
{
    u8   Status;        //;开关
    u8   Mark;          //;抄表类型
    u16  CouDown;       //;倒计时
};

struct PortCtrl
{
   u8  Status;        //;端口发送状态
   u8  Mark;          //;端口处理类型
   u16  Index;         //;当前处理到第几块表
   u16 PnOver;        //;
   u8  Over;
   u8  MeterNum[6];   //;当前正在采集的表号
   u8  GYType;        //;采用规约类型
   u8  BPS;            //;波特率
   u16 IndSend;
   u16 IndSave;
   u32 DI1DI0;   //;当前正在抄表的标识
   u8  WaitAns;
   u8  ReSNum;       //;重发次数
   u8  ErrNum;       //;
   u8  PnErrNum;
   char *Buffer;     //;端口发送与接收数据的缓存区间指针
   char Len;         //;要发送数据的长度
};

struct ControlOrder //;跳闸合闸控制数据结构
{
    u8 IndSave;
    u8 IndSend;
    u8 Status;
    u8 Port;
    u8 BPS;
    u8 ErrNum;
    u8 MeterNum[6];
    u32 DI[SBNUM];
    u8 Pn[SBNUM];
    u8 Order[SBNUM];
};

struct GprsToMeter  //;gprs模块与电表的通讯控制
{
	u16 Order[10];  
	u8  IndSave;    
	u8  IndSend;    
	u8  Status;     
	u8  WaitAns;	  
	u8  ErrorCount;    
	u8  IgnoreCount;   
	u8  RightCount;    
  	u8  SendFlag;  
  	u8  RMeter_Enable;
    u8  Bao1,Bao2, Bao3;
    u16 Bao4;
};






#endif /*__GDW130_DataType_h */
