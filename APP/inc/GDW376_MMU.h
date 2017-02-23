/*
********************************************************************************
*  文 件 名：GDW376_MMU.h
*
*  文件描述：数据保存分配Flash空间，FM24c04空间
*
*  所用芯片：FLASH为AT45DB161D（2M = 4096页 * 528字节）
*             EEROM为FM24C04（(4*1024)/8 = 512byte）
*
*  创 建 人：
*
*  版 本 号：
*
*  修改记录：
********************************************************************************
*/
#ifndef __GDW130_MMU_h
#define __GDW130_MMU_h

#include "stm32f10x.h"
#include "Function_CFG.h"

#ifndef PAGECOUNT
    #define PAGECOUNT 4096
#endif
#define PAGESIZE         528  //;每页保存的字节
//;#define PAGECOUNT        4095
#define PAGESOFSECTOR0A  8
#define PAGESOFSECTOR0B  248
#define PAGESOFSECTOR    264
/*------------------------------------------------------------------------------
|| Bin文件存放区，分为PROG0(存放需更新程序)，PROG1(存放原程序)。
|| 起始页为0，则扇区0A为4K，0B为124K。起始页改变需修改扇区配置
------------------------------------------------------------------------------*/
#if defined   (STM32F10XXB)
    #define   PROG_FLASH_SIZE     (0x20000)    /* 128 Kbyte */
#elif defined (STM32F10XXC)
    #define   PROG_FLASH_SIZE     (0x40000)    /* 256 Kbyte */
#elif defined (STM32F10XXD)
    #define   PROG_FLASH_SIZE     (0x60000)    /* 384 Kbyte */
#elif defined (STM32F10XXE)
    #define   PROG_FLASH_SIZE     (0x80000)    /* 512 Kbyte */
#else 
 #error "Not select the STM32 device type(Function_CFG.h)"    
#endif

#define PROG_PAGE_COUNT  (PROG_FLASH_SIZE / 512)

#define PROG_SECTOR_COUNT (PROG_PAGE_COUNT / 8 / 32)

#define FLASH_PAGE_START  0
#define PROG0_PAGE_START   FLASH_PAGE_START
#define PROG1_PAGE_START   PROG0_PAGE_START + PROG_PAGE_COUNT
#define PROG_PAGE_END      PROG1_PAGE_START + PROG_PAGE_COUNT

#define PAGE_SIGN_SIZE   4
#define PAGE_LENG_SIZE   4
#define PAGE_INDEX_SIZE  (2 + 2)
#define PAGE_HEAD_SIZE   PAGE_SIGN_SIZE + PAGE_LENG_SIZE + PAGE_INDEX_SIZE

#define PAGE_DATA_SIZE   512

//;
#define UPDATASIG0 'X'
#define UPDATASIG1 'Y'
#define UPDATASIG2 'G'
#define UPDATASIG3 'X'

#define OLDDATASIG0 'S'
#define OLDDATASIG1 'I'
#define OLDDATASIG2 'G'
#define OLDDATASIG3 'N'

//;定义两程序区间的扇区个数
#if PROG0_PAGE_START == 0
    #define PROG0_SECTOR_COUNT PROG_SECTOR_COUNT + 1
#else
    #define PROG0_SECTOR_COUNT PROG_SECTOR_COUNT
#endif

#define PROG1_SECTOR_COUNT PROG_SECTOR_COUNT

#if (PROG0_PAGE_START % 256) || (PROG0_PAGE_START % 256)
    #error "PROG isn't the sector's start!!"
#endif
 



/*------------------------------------------------------------------------------
|| 测量点冻结数据,包括日冻结,抄表日冻结,月冻结,曲线冻结在Flash中的空间分配
------------------------------------------------------------------------------*/
#define CSBiaoZhi1  'J'
#define CSBiaoZhi2  'S'
#define CSBiaoZhi3  'J'

//#define CSBiaoZhi1  'z'
//#define CSBiaoZhi2  'z'
//#define CSBiaoZhi3  'S'

#define TestBiaoZhi1  'T'           //;zrt 20130904   2014012217
#define TestBiaoZhi2  'E'
#define TestBiaoZhi3  'S' 
#define TestBiaoZhi4  'T'


/*事件相关定义*/

#define ERCReadNum   256  //;要从ERC_Num中读取的点数据
#define ERCPointNum  264  //;保存的事件点数， 每页8点 共用33页
#define ERCAreaNum   66   //;每点保存的字节
#define ERCPagePoint (PAGESIZE / ERCAreaNum)    //;每页保存的点
#define ERCPageNum   (ERCPointNum / ERCPagePoint)  //;共保存页数

/*日冻结相关定义*/
#define DDJPointNum  32   //;保存点数
#define DDJPointPage 2    //;每点保存2页
#define DDJPageNum   (DDJPointNum * DDJPointPage)  //;共保存页数

/*抄表日冻结相关定义*/
#define BDJPointNum  32
#define BDJPointPage 1
#define BDJPageNum   (BDJPointNum * BDJPointPage)

/*月冻结相关定义*/
#define MDJPointNum  13
#define MDJPointPage 2
#define MDJPageNum   (MDJPointNum * MDJPointPage)

/*曲线冻结相关定义*/
#define QDJPointNum  2976//;保存点数 = 天*24*4
#define QDJAreaNum   111  //;每点保存的字节
#define QDJPagePoint (PAGESIZE / QDJAreaNum)   //;每页保存的点 4
#define QDJPageNum   (QDJPointNum / QDJPagePoint) //;共保存页数 744 

/*小时冻结相关定义*/
//;掉电时保存。保存当前小时，上一小时数据并备份。
#define HDJPointPage 1   
#define HDJPageNum   2   //;每个测量点用4页

/*保存最值统计数据相关定义*/
#define  MVTPointPage  1
#define  MVTPageNum    1 //;每个测量点用1页

/*保存零点电能数据相关定义*/
#define  PEZPointPage 1
#define  PEZPageNum   1  //;每个测量点用1页

//;为每个测量点分配的页数.
#define PnZPage (MDJPageNum + DDJPageNum + BDJPageNum + QDJPageNum\
                 +HDJPageNum + MVTPageNum + PEZPageNum)


/* 参数区分配 */
#define CSBock1     PROG_PAGE_END 
#define CSBock2     (CSBock1       +  1)
#define CSBock3     (CSBock2       +  1)


#define P1CSBock4     (CSBock3   + 1)
#define CSBock5       (P1CSBock4 + 1)
#define CSBock6       (CSBock5       +  1)
#define CSBock7       (CSBock6       +  1)
#define CSBock8       (CSBock7       +  1)
#define CSBock9       (CSBock8       +  1)
#define CSBock10      (CSBock9       +  1)

#define CSBock1Bak    (CSBock10       +  1) //;参数备份区
#define CSBock2Bak    (CSBock1Bak    +  1)
#define CSBock3Bak    (CSBock2Bak    +  1)
#define P1CSBock4Bak  (CSBock3Bak   + 1)
#define CSBock5Bak    (P1CSBock4Bak  +  1)
#define CSBock6Bak    (CSBock5Bak    +  1)
#define CSBock7Bak    (CSBock6Bak    +  1)
#define CSBock8Bak    (CSBock7Bak    +  1)
#define CSBock9Bak    (CSBock8Bak    +  1)
#define CSBock10Bak   (CSBock9Bak    +  1)
#define CSEND         (CSBock10Bak    +  1)

/*事件区共66页*/
#define ERC1Home   (CSEND)
#define ERC2Home   (ERC1Home + ERCPageNum)
#define ERCEND     (ERC2Home + ERCPageNum)

/*测量点区共PnZPage * PnMaxNum = 517* 10 = 5170页*/
/*测量点1区*/
#define P1DDJHome  (ERCEND)
#define P1BDJHome  (P1DDJHome  + DDJPageNum)
#define P1MDJHome  (P1BDJHome  + BDJPageNum)
#define P1QDJHome  (P1MDJHome  + MDJPageNum)
#define P1NHDJHome (P1QDJHome  + QDJPageNum)
#define P1OHDJHome (P1NHDJHome + HDJPointPage)
#define P1MVTHome  (P1OHDJHome + HDJPointPage)
#define P1PEZHome  (P1MVTHome  + MVTPointPage)
#define P1DJEND    (P1PEZHome  + PEZPointPage)
#define PnDJEND    (P1DJEND)

#define FLASH_TEST PnDJEND+1            //;by zrt 20130904  增加flash测试

#define FLASHENDPAGE  FLASH_TEST
#if FLASHENDPAGE > PAGECOUNT
    #error "More than total FLASHPAGE "
#endif


/*------------------------------------------------------------------------------
|| FM24C04数据分配。
------------------------------------------------------------------------------*/
#define FM24BYTESIZE  512

//#define _FM24_BZ1    'C'
//#define _FM24_BZ2    'J'
//#define _FM24_BZ3    'Q'
//#define _FM24_BZ4    'K'

#define _FM24_BZ1    'J'
#define _FM24_BZ2    'S'
#define _FM24_BZ3    'J'
#define _FM24_BZ4    'M'

/*X字节+1个校验和*/
#define  BZByteNum     4        //;表字
#define  VerByteNum   (4  +  1)  //;软件版本号
#define  XDJByteNum   (2  +  1)  //;日，月，抄表日冻结保存的序号
#define  QDJByteNum   (4  +  1)  //;曲线保存的序号
#define  P0ERCByteNum (4  +  1)  //;终端事件保存的序号
#define  ERCByteNum   (1  +  1)  //;终端事件个数
#define  ERCUpByteNum (1  +  1)  //;上送序号
#define  FluxByteNum  (11 +  1)  //;流量

#define  E2R_BiaoZhi  0

/*前200个字节,用于保存终端配置参数*/
#define  Version      (E2R_BiaoZhi + BZByteNum)
#define  E2R_ERC1     (Version    + VerByteNum)
#define  E2R_ERC2     (E2R_ERC1   + P0ERCByteNum)
#define  E2R_EC1      (E2R_ERC2   + P0ERCByteNum)
#define  E2R_EC2      (E2R_EC1    + ERCByteNum)
#define  E2R_ERCUP    (E2R_EC2    + ERCByteNum)
//;#define  E2R_Flux     (E2R_ERCUP  + ERCUpByteNum)
#define  E2R_PowD     (E2R_ERCUP   + ERCUpByteNum)

/*用于保存测量点配置参数 14 * 10 = 140byte*/
/*测量点1*/
#define  E2R_P1DDJ      200
#define  E2R_P1BDJ      (E2R_P1DDJ + XDJByteNum)
#define  E2R_P1MDJ      (E2R_P1BDJ + XDJByteNum)
#define  E2R_P1QDJ      (E2R_P1MDJ + XDJByteNum)
#define  E2R_PNEND      E2R_P1QDJ

#define  E2R_ByteNum    (E2R_PNEND + QDJByteNum)  //; 将E2R_XXXX + E2R_ByteNum保存到相应的备件区

#define  _FM24_TEST     (E2R_ByteNum + 1)         //;FM24 测试by zrt 20130904

#if (E2R_ByteNum * 2 - 200) > FM24BYTESIZE
    #error "more than FM24SIZE"
#endif

#endif /*__GDW130_MMU_h */