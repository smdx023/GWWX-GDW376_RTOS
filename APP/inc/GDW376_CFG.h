/*
********************************************************************************
*  文 件 名：GDW376_CFG.h
*
*  文件描述：规约相关宏定义，全局变量和函数
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
#ifndef __GDW376_CFG_h
#define __GDW376_CFG_h

#include "stm32f10x.h"
#include "BSP_CFG.h"
#include "GDW376_DataType.h"
#include "Function_CFG.h"
//;#include "GDW376_MMU.h"


#define GYType2007   30 //;485表
#define GYType1997   1 //;网络表
#define GYType6F     4 //;6F表

#define  _TaskON     0x55

//;#define  ERC_SMS         //;打开事件发生的短信发送功能
//;#define  LocalUpERC      //;允许通过本地调试口上送重要事件
//;#define  LocalUpTask     //;允许通过本地调试口上送任务主动

//;#define FRAMESIZE    500   //;每一帧数据的最大长度
#define FRAMESIZE    1010   //;每一帧数据的最大长度
#define NFRAME     5      //;最大分帧数目

#define PNDSIZE    sizeof(struct PointDataStr) - 2

#define DITab_GCNM_Num   34 
#define DITab_Num  8



//;#define DITab_2007_Num  72      //;07规约数据标识个数by zrt 20130808
#define DITab_2007_Num  101      //;07规约数据标识个数
#define DITab_1997_Num  69


/*各测量点类型数据在PnDTAddr中的排列结构偏移,如果要添加，删除，更改
  PnDTAddr中的内容，相应的结构偏移也要更改，无需更改程序*/
#define  _PnP         (0)
#define  _PnD         (_PnP        +  1)
#define  _ERCPn       (_PnD        +  1)
#define  _PnDDJ       (_ERCPn      +  1)
#define  _PnBDJ       (_PnDDJ      +  1)
#define  _PnMDJ       (_PnBDJ      +  1)
#define  _PnQDJ       (_PnMDJ      +  1)
#define  _PnDDJHome   (_PnQDJ      +  1)
#define  _PnBDJHome   (_PnDDJHome  +  1)
#define  _PnMDJHome   (_PnBDJHome  +  1)
#define  _PnQDJHome   (_PnMDJHome  +  1)
#define  _PnNHDJHome  (_PnQDJHome  +  1)
#define  _PnOHDJHome  (_PnNHDJHome +  1)
#define  _PnMVTHome   (_PnOHDJHome +  1)
#define  _PnPEZHome   (_PnMVTHome  +  1)
#define  _E2R_PnDDJ   (_PnPEZHome  +  1)
#define  _E2R_PnBDJ   (_E2R_PnDDJ  +  1)
#define  _E2R_PnMDJ   (_E2R_PnBDJ  +  1)
#define  _E2R_PnQDJ   (_E2R_PnMDJ  +  1)

extern u8 TimeChgFlag;
extern u8 F49[1];
extern struct ZDPStr ZDP;         //;终端参数
extern struct ZDStatStr ZDS;      //;终端状态数据
extern struct ZDPStrKZ ZDPKZ;     //;终端扩展参数 
extern struct GroupStr GRPP;      //;总加组参数
extern struct DCPortStr DCPP;     //;直流模拟量参数
extern struct TaskPStr TaskP;     //;任务配置参数
extern struct MeterData PMD1, PMD2;  //;基表数据
extern struct PoPStr P1P;//;P2P,P3P,P4P,P5P,P6P,P7P,P8P,P9P,P10P;//;测量点参数                 
extern struct PointDataStr P1D;//;P2D,P3D,P4D,P5D,P6D,P7D,P8D,P9D,P10D; //;测量点数据                                                                                               

extern struct DJ_Control P1DDJ;//;P2DDJ,P3DDJ,P4DDJ,P5DDJ,P6DDJ,P7DDJ,P8DDJ,P9DDJ,P10DDJ;    
extern struct DJ_Control P1BDJ;//;P2BDJ,P3BDJ,P4BDJ,P5BDJ,P6BDJ,P7BDJ,P8BDJ,P9BDJ,P10BDJ;    
extern struct DJ_Control P1MDJ;//;P2MDJ,P3MDJ,P4MDJ,P5MDJ,P6MDJ,P7MDJ,P8MDJ,P9MDJ,P10MDJ;    
extern struct QX_Control P1QDJ;//;P2QDJ,P3QDJ,P4QDJ,P5QDJ,P6QDJ,P7QDJ,P8QDJ,P9QDJ,P10QDJ;   
extern struct DJ_SAVE_CTRL DDJPn,  BDJPn, MDJPn, QDJPn,DDLDJPn,MDLDJPn;                                                                                   

extern u8 OldZDP[54];
extern struct ZDCBCtrl     ZDCB;
extern struct PortCtrl     Port1,  Port2;
extern struct ControlOrder CtrlOrder;   //;跳闸合闸控制参数
extern struct GuiYueCtrl   LocGyC, GprsGyC, InfGyC;
extern struct Relay_Control RelayC;       //;中继控制参数
extern struct Task_Control  TaskC;         //;任务控制参数
extern u8 SofewareOldVer[4]; //;变更前的软件的版本号
extern u32 FluxLimValue;

extern u8 ERCIMP_Pm[32][4];
extern struct ERC_Control  ERC1,ERC2;
extern struct ERCUpCtrl   ERCUp;
extern struct ERCSMSCtrl  SMS;                                                             
extern struct ERCSaveCtrl ERCSave;                                                         
extern struct ERCP0Data   ERCP0;
extern struct ERCPnData   ERCP1;//;ERCP2,ERCP3,ERCP4,ERCP5,ERCP6,ERCP7,ERCP8,ERCP9,ERCP10;

extern B4 P1DTAddr[];//;P2DTAddr[],P3DTAddr[],P4DTAddr[],P5DTAddr[];
//;extern B4 P6DTAddr[],P7DTAddr[],P8DTAddr[],P9DTAddr[],P10DTAddr[];
extern B4 PnDTAddr[];

//;extern B4 CSZU_TaskT1[],CSZU_TaskT2[],CSZU_TaskOn1[],CSZU_TaskOn2[];
extern B4 ZDCSZU[];
extern B1 ZDCFG_F1[];
extern B4 ZDCFGZU[];
extern B4 PN0CZU[];
extern B4 PN0DZU[];
extern B2 PnHDJ_Adr[];

extern B4 DITab_2007[];
extern B4 DITab_1997[];

extern struct AlarmHVM_Control AlarmHVM;
extern struct GprsToMeter  G2M;

extern void USART1_Send(void); //;20141215
/*----------------------------------------------------------------------------*
 |  GDW130_AFN.C
 *----------------------------------------------------------------------------*/
extern 	u16 Find_Fn( u8 DT1,u8 DT2 );
extern 	u8 Find_FnI( u8 DT1 );
extern 	u16 Find_Pn( u8 DA1,u8 DA2 );
extern  u8 Find_ZDPnEnable(u8 Pn);
extern  u16 GW130_AFN01H(char *Order_address, char *out_buf);
extern  u8  GW130_AFN02H(char *Order_address, struct GuiYueCtrl *GYCtrl, u8 Fn);
extern 	u16 GW130_AFN04H(char *Order_address, char *out_buf);
extern  u16 GW130_AFN05H(char *Order_address, char *out_buf);
extern 	u16 GW130_AFN0AH(char *Order_address, char *out_buf , struct GuiYueCtrl  *GYCtrl);
extern 	u16 GW130_AFN0CH(char *Order_address, char *out_buf , struct GuiYueCtrl  *GYCtrl);
extern 	u16 GW130_AFN0DH(char *Order_address, char *out_buf , struct GuiYueCtrl  *GYCtrl);
extern  u16 GW130_AFN0EH(char *Order_address, char *out_buf , struct GuiYueCtrl  *GYCtrl);
extern 	u32 PreData_AFN04H( u16 Ind_S, u16 Ind_R, char *SData, char *RData);
extern  u32 PreData_AFN05H( u16 Ind_S, u16 Ind_R, char *SData, char *RData);
extern 	u32 PreData_AFN0CH( u16 Ind_S,u16 Ind_R, char * SData, char * RData );
extern  u32 PreData_AFN0EH ( u16 Ind_S, u16 Ind_R, char *SData, char *RData, struct GuiYueCtrl  *GYCtrl );
extern  u8 AFN0DH_TCode(u8 Fn);
extern  u32 AFN0DH_SCode(u8 Fn);
extern  void AFNComDeal(void);

/*----------------------------------------------------------------------------*
 |  GDW130_DJPN.C
 *----------------------------------------------------------------------------*/
extern void HDJ_Pre(u8 Pn);
extern void HDJ_Reset(u8 Pn);
extern void HDJ_Program(u8 Pn);
extern void ZD_DJPN_CHK(void);
extern void ZD_DJPN(void);
extern void ZD_CBD_CHK(void);

/*----------------------------------------------------------------------------*
 |  GDW130_FUN.C
 *----------------------------------------------------------------------------*/
extern void ZD_RAM_MAK(void);
extern void ZD_HWareRes_CHK(void); 
extern void Reset_Flux();
extern u8 CalcCRC(char *InData, char *InPW);
extern s16 ZD_Pn_Find(u16 Pn);
extern void ZD_Stat_Init(void);
extern void ZD_DataInit(void);
extern void ZD_CS1_Init(u8 type);
extern void ZD_CS2_Init(void);
extern void ZD_CS3_Init(void);
extern void ZD_CS4_Init(u16 Pn);
extern void ZD_CS5_Init(void);
extern void ZD_CS6_Init(void);
extern void ZD_CS7_Init(void);
extern void ZD_CS8_Init(u8 type);
extern void ZD_CS9_Init(void);
extern void ZD_CS10_Init(void);
extern void ZD_CSDataInit(void);
extern void ZD_NCCSDataInit(void);
extern void ZD_HardWareReset(void);
extern void ZD_Time_Set(u8 *TimeAddr);
extern void ZD_Time_Read(void);
extern void ClockTimer(void);
extern void CheckTime(void);
extern void ZD_Relay(void);
extern void ZD_POW_CHK(void);
extern void ZD_Mast_Link(void);
extern void Master_Link(void);
extern void LinkStat_CHK(void);
extern void TimeChangeDeal(void);
extern void ZD_ACConver(void);
extern void ZD_ACDefault(void);

/*----------------------------------------------------------------------------*
 |  GDW130_ERC.C
 *----------------------------------------------------------------------------*/
extern  void ERC1_Judge(u8 Type);
extern  void ERC14_Judge(u8 *Byte1, u8 Type);
extern  void ERCPn_EnergyJudge(u16 index);
extern  void ERCPn_ParaJudge(u16 index);
extern  void ERC10_Judge(struct PoPStr *PnP, 
                         struct PointDataStr *PnD, 
                         struct ERCPnData *ERCPn);
extern  void ERC17_Judge(struct PointDataStr *PnD, struct ERCPnData *ERCPn);
extern  void ERC24_Judge(struct PointDataStr *PnD, struct ERCPnData *ERCPn);                        
extern  void ERC25_Judge(struct PointDataStr *PnD, struct ERCPnData *ERCPn);
extern  void ERC26_Judge(struct PoPStr  *PnP, 
                         struct PointDataStr *PnD, 
                         struct ERCPnData *ERCPn);
extern  void ERC21_4Judge(u8 Occur);
extern  u16 ERC_SMS_Up(char *Out_Buf);
extern  u8 ERC_DataPre(u16 Pn, u8 ERCNo, char *OutData);
extern  void ERC_Deal_SMS(void);
extern  u8 ZD_ERC(void);
extern  void Save_AlarmHVM(u8 *alarm_buf, u16 alarm_page);

/*----------------------------------------------------------------------------*
 |  GDW130_Task.C
 *----------------------------------------------------------------------------*/
extern  u8   T2Task_CBF(u8 *DateIn, u8 *DateOut);
extern  void T2Task_CTF(u8 *DateIn, u8 *DateOut);
extern  u8   T2Task_FnPn(char *Out_Buf, u16 Buf_Ind, u8 Pn, u8 Fn);
extern  void Task_Init(u16 Pn, u8 Fn);
extern  u8   TaskUpFind(void);
//;extern  void Task_Deal(void);
extern  void ZD_Task(void);
extern  void ZD_Task_CHK(void);
/*----------------------------------------------------------------------------*
 | GDW130_MMU.C
 *----------------------------------------------------------------------------*/
extern  void ReadFlash_Verify(u16 Page1, u16 Page2);
extern  void MMU_Flash_Save(u16 Page);
extern  void MMU_Flash_Read(u8 *Buffer, u16 Page);
extern  void MMU_FM24_Read(void);
extern  void Save_Flux(void);
extern 	void CS_Save(void);
extern 	void CS_Read(void);
extern void ZD_CSSAVE_CHK(void);
extern  u8 DDJ_Read(char *Buf_Out, u16 Buf_Ind, u8 Pn, u8 Fn, u8 *Td_d);
extern  u8 BDJ_Read(char *Buf_Out, u16 Buf_Ind, u8 Pn, u8 Fn, u8 *Td_d);
extern  u8 MDJ_Read(char *Buf_Out, u16 Buf_Ind, u8 Pn, u8 Fn, u8 *Td_d);
extern  u8 QDJ_Read(char *Buf_Out, u16 Buf_Ind, u8 Pn, u8 Fn, u8 *Td_c);
extern  void HDJ_Read(void);
extern  void HDJ_Save(u8 Pn);
extern  u8 DDJ_FTP(u8 Pn, u8 *Td_d);
extern  u8 MDJ_FTP(u8 Pn, u8 *Td_m);
extern  void DDJ_Save(u8 Pn, u8 *Date);
extern  void BDJ_Save(u8 Pn, u8 *Date);
extern  void MDJ_Save(u8 Pn, u8 *Date);
extern  void CDJ_Save(u8 Pn, u8 *Date);
extern  void MVT_Read(void);
extern  void MVT_Save(u8 Pn);
extern void PEZD_Save(u8 Pn, u8 *Date);
extern void PEZM_Save(u8 Pn, u8 *Date);
extern void PEZ_Read(void);
extern void ERC_Save(u8 Len, char *Data_Add, u8 Type);


/*----------------------------------------------------------------------------*
 |  GDW130_ZDCB.c
 *----------------------------------------------------------------------------*/
extern void Format6Current(u8 *AddrOut, u8 *AddrIn);
extern void Format7Voltage(u8 *AddrOut, u8 *AddrIn);
extern void Format7Voltage(u8 *AddrOut, u8 *AddrIn);
extern void Format9Power(u8 *AddrOut, u8 *AddrIn, u8 Type);
extern void Format13Energy(u8 *AddrOut, u8 *AddrIn);
extern void Format14Energy(u8 *AddrOut, u8 *AddrIn);
extern void Format14Energy5(u8 *AddrOut, u8 *AddrIn);
extern void ZDPN_YXT(void);
extern void ZDPN_MXT(void);
extern void ZDPN_DME(u8 index);
extern void MVTD_Res(u8 Pn);
extern void MVTM_Res(u8 Pn);
extern void ZD_Port_Com(void);  
extern void ZD_CB_CHK(void);
extern void ZD_CB(void);
extern void ZD_SB(void);
extern void Send_Modem_Status(void);
extern void ZD_EventRead(void);
extern void Send_TimetoMeter(void);

#endif /*__GDW130_CFG_h */
