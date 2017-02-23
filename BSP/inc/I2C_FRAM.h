/*
********************************************************************************
*  文 件 名： I2C_FRAM.h
*
*  文件描述： MB85RC04V的驱动程序
*
*  所用芯片： MB85RC04V
*
*  创 建 人： simon--20141210
*
*  版 本 号： 1.00
*
*  修改记录： 1. 创建于2013-10-24 10:52:17
********************************************************************************
*/

#ifndef __I2C_FRAM
#define __I2C_FRAM

/* 功能宏定义 */
#define		I2C_USE_WP			//;使用WP引脚
#define		I2C_STM32_SYSCLK		//;使用stm32芯片的内核时钟进行延时
						//;不是stm32(取消该宏定义)自行修改I2C_TimingDelay函数

/* 返回值宏定义 */
#define		I2C_OPERATION_SUCCEED		0x00//;操作成功
#define		I2C_OPERATION_FAILED		0x01//;操作失败
#define		I2C_SLAVE_ACK			0x00//;从设备有应答
#define		I2C_SLAVE_NOACK			0x01//;从设备无应答

/* 检查宏定义 */
#define		I2C_WRITE_PROTECT_WORD		0x12345678

/* 函数声明 */
extern void I2C_InitFRAM(void);
extern u8 I2C_GetDataFromFRAM(u16 dataNum, u16 getAddr, u8* dataBuf);
extern u8 I2C_SaveDataToFRAM(u16 dataNum, u16 saveAddr, u8* dataToSave,u32 checkWord);
extern void I2C_FormatFRAM(void);


/*------------------****---下面为与灌溉表模块相关应用，移植科删除下面所有东西---------****---------*/
static inline u8 ReadNByte_I2C1(u8 n, u16 adr, u8* adr_ram )
{
	I2C_GetDataFromFRAM(n, adr, adr_ram);
	return 1;
}

static inline u8 WriteNByte_I2C1(u8 n, u16 adr, u8 *adr_ram)
{
	I2C_SaveDataToFRAM(n, adr, adr_ram,I2C_WRITE_PROTECT_WORD);
	return 1;
}

extern void SaveData_FM24(u8 n, u16 EEAdr1, u16 EEAdr2, u8 *adr_ram);
extern void ReadData_FM24(u8 n, u16 EEAdr1, u16 EEAdr2, u8 *adr_ram);
/*---------------------------------------------END-------------------------------------------------*/

#endif
