/*
********************************************************************************
*  文 件 名： I2C_FRAM.c
*
*  文件描述： MB85RC04V的驱动程序
*
*  所用芯片： MB85RC04V
*
*  创 建 人： simon
*
*  版 本 号： 1.00
*
*  修改记录： 1. 创建于2014-12-10 10:52:17
********************************************************************************
*/

/* 需要的头文件 */
#include "stm32f10x.h"
#include "I2C_FRAM.h"

/* 芯片相关宏定义 */
#define		FRAM_ADDR		0xA0
#define		FRAM_READ		0x01
#define		FRAM_WRITE		0x00
#define		FRAM_UPPER_ADDR		0x02
#define		FRAM_LOWER_ADDR		0x00

//;SCL引脚
#define		I2C_SCL_H		GPIO_SetBits(GPIOB, GPIO_Pin_6)
#define		I2C_SCL_L		GPIO_ResetBits(GPIOB, GPIO_Pin_6)
//;SDA引脚
#define		I2C_SDA_H		GPIO_SetBits(GPIOB, GPIO_Pin_7)
#define		I2C_SDA_L		GPIO_ResetBits(GPIOB, GPIO_Pin_7)
#define		I2C_GET_SDA		GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)
//;WP引脚
#ifdef I2C_USE_WP
	#define		I2C_WP_H		GPIO_SetBits(GPIOB, GPIO_Pin_5)
	#define		I2C_WP_L		GPIO_ResetBits(GPIOB, GPIO_Pin_5)
#else
	#define		I2C_WP_H
	#define		I2C_WP_L
#endif


/* 变量 */
#ifdef I2C_STM32_SYSCLK
	static u32 SYSCLK_Frequency = 0;	//;系统时钟
#endif

/* 
********************************************************************************
* 函 数 名： I2C_SDA_DIR_OUT
* 功    能： 配置SDA引脚为输出
* 参    数： 无
* 返    回： 无 
* 说    明： 
* 抛出异常：
* 作    者： simon
* 修改记录： 
********************************************************************************
*/
static void I2C_SDA_DIR_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //;SDA
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //;推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/* 
********************************************************************************
* 函 数 名： I2C_SDA_DIR_IN
* 功    能： 配置SDA引脚为输入
* 参    数： 无
* 返    回： 无
* 说    明：
* 抛出异常：
* 作    者：simon
* 修改记录：2014-12-10
********************************************************************************
*/
static void I2C_SDA_DIR_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //;SDA
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //;浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}


/* 
********************************************************************************
* 函 数 名： I2C_TimingDelay
* 功    能： IIC工作时序延时函数
* 参    数： time: 需要延时的微秒(us)数 减去 1 
* 返    回： 无
* 说    明： 可根据不同工作频率调节延时时间(不是STM32需重新编写该函数)
* 抛出异常：
* 作    者： simon
* 修改记录： 创建于2014-12-10 13:37:37
********************************************************************************
*/
void I2C_TimingDelay(u32 time)
{
	#ifdef I2C_STM32_SYSCLK		//;stm32单片机延时操作
		u32 cnt;
		cnt = SYSCLK_Frequency * time;
		while (cnt--);
	#else				//;其他单片机延时控制
		time *= 10;
		while (time--);
	#endif
}

/* 
********************************************************************************
* 函 数 名： I2C_StartTimingSeq
* 功    能： IIC启动时序
* 参    数： 无
* 返    回： 无
* 说    明： 
* 抛出异常：
* 作    者： simon
* 修改记录： 创建于2014-12-10
********************************************************************************
*/
static void I2C_StartTimingSeq(void)
{
	I2C_SDA_DIR_OUT();
	I2C_SCL_H;
	I2C_SDA_H;
	I2C_TimingDelay(5);
	I2C_SDA_L;
	I2C_TimingDelay(5);
	I2C_SCL_L;
}

/* 
********************************************************************************
* 函 数 名： I2C_StopTimingSeq
* 功    能： IIC结束时序
* 参    数： 无
* 返    回： 无
* 说    明： 
* 抛出异常：
* 作    者： simon
* 修改记录： 2014-12-10
********************************************************************************
*/
static void I2C_StopTimingSeq(void)
{
	I2C_SDA_DIR_OUT();
	I2C_SCL_L;
	I2C_SDA_L;
	I2C_TimingDelay(5);
	I2C_SCL_H;
	I2C_SDA_H;
	I2C_TimingDelay(10);
}

/* 
********************************************************************************
* 函 数 名： I2C_InitFRAM
* 功    能： 配置IIC相关引脚初始状态
* 参    数： 无
* 返    回： 无
* 说    明：
* 抛出异常：
* 作    者：simon
* 修改记录：2014-12-10
********************************************************************************
*/
void I2C_InitFRAM(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE); //;开启GPIO时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7; //;SCL , SDA
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //;推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	I2C_SCL_H;
	I2C_SDA_H;
	I2C_WP_L;
	
	#ifdef I2C_STM32_SYSCLK		//;使用STM32芯片额外操作
	{
		RCC_ClocksTypeDef RCC_Clocks;
		RCC_GetClocksFreq(&RCC_Clocks); //;获取工作时钟
		SYSCLK_Frequency = RCC_Clocks.SYSCLK_Frequency;
		if (0 == SYSCLK_Frequency)
		{
			SYSCLK_Frequency = 36000000;	//;默认工作频率36M
		}
		SYSCLK_Frequency /= 6000000;		//;除以6M
	}
	#endif
        I2C_StopTimingSeq();
        I2C_TimingDelay(100);
}


/* 
********************************************************************************
* 函 数 名： I2C_SendByte
* 功    能： 向IIC总线写入一个字节
* 参    数： data：需要写入的数据
* 返    回： 无
* 说    明： 
* 抛出异常：
* 作    者： simon
* 修改记录： 2014-12-10
********************************************************************************
*/
static void I2C_SendByte(u8 data)
{
	u8 i;
	
	I2C_SDA_DIR_OUT();
	I2C_SCL_L;
	for (i = 0; i < 8; i++)
	{
		if (data & 0x80) //;从最高位开始发送
		{
			I2C_SDA_H;
		}
		else
		{
			I2C_SDA_L;
		}
		data <<= 1;
		I2C_TimingDelay(2);
		I2C_SCL_H; 
		I2C_TimingDelay(3);
		I2C_SCL_L;
		I2C_TimingDelay(2);
	}
}

/* 
********************************************************************************
* 函 数 名： I2C_GetAck
* 功    能： 获取应答信号
* 参    数： 无
* 返    回： 应答电平
* 说    明： 
* 抛出异常：
* 作    者： simon
* 修改记录： 2014-12-10
********************************************************************************
*/
static u8 I2C_GetAck(void)
{
	u8 ack = I2C_SLAVE_NOACK, trynum = 0;
	
	I2C_SDA_DIR_OUT();
	I2C_SCL_L;
	I2C_SDA_H;
	I2C_SDA_DIR_IN();
	I2C_TimingDelay(0);
	while ((I2C_SLAVE_NOACK == ack) && (trynum < 200))
	{
		ack = I2C_GET_SDA;
		trynum++;
	}
	if (trynum < 200)
	{
		ack = I2C_SLAVE_ACK;
	}
	I2C_SCL_H; 
	I2C_TimingDelay(2);
	I2C_SCL_L;
	I2C_TimingDelay(2);
	return ack;
}

/* 
********************************************************************************
* 函 数 名： I2C_ReceiveByte
* 功    能： 向IIC总线读取一个字节
* 参    数： 无
* 返    回： 读取到的数据
* 说    明： 
* 抛出异常：
* 作    者： simon
* 修改记录： 2014-12-10
********************************************************************************
*/
static u8 I2C_ReceiveByte(void)
{
	u8 data = 0;
	u8 i;
	
	I2C_SCL_L;
	I2C_TimingDelay(0);
	I2C_SDA_DIR_IN();
	for (i=0; i<8; i++)
	{
		I2C_SCL_H;
		data <<= 1;
		if (I2C_GET_SDA)
		{
			data++;
		}
		I2C_TimingDelay(4);
		I2C_SCL_L;
		I2C_TimingDelay(2);
	}
	I2C_SDA_DIR_OUT();
	return data;
}
/* 
********************************************************************************
* 函 数 名： I2C_GiveAck
* 功    能： 给出应答信号
* 参    数： 无
* 返    回： 无
* 说    明： 
* 抛出异常：
* 作    者： simon
* 修改记录： 2014-12-10
********************************************************************************
*/
static void I2C_GiveAck(void)
{
	I2C_SDA_DIR_OUT();
	I2C_SCL_L;
	I2C_TimingDelay(1);
	I2C_SDA_L;
	I2C_TimingDelay(1);
	I2C_SCL_H;
	I2C_TimingDelay(3);
	I2C_SCL_L;
	I2C_TimingDelay(1);
}

/* 
********************************************************************************
* 函 数 名： I2C_GiveNoAck
* 功    能： 给出不应答信号
* 参    数： 无
* 返    回： 无
* 说    明： 
* 抛出异常：
* 作    者： simon
* 修改记录： 2014-12-10
********************************************************************************
*/
static void I2C_GiveNoAck(void)
{
	I2C_SDA_DIR_OUT();
	I2C_SCL_L;
	I2C_TimingDelay(1);
	I2C_SDA_H;
	I2C_TimingDelay(1);
	I2C_SCL_H;
	I2C_TimingDelay(3);
	I2C_SCL_L;
	I2C_TimingDelay(1);
}

/* 
********************************************************************************
* 函 数 名： I2C_SaveDataToFRAM
* 功    能： 保存指定长度数据到FRAM的指定地址
* 参    数： 
*	dataNum：保存的数据字节数
*	saveAddr：保存位置
*	dataToSave：需要保存的数据
*	checkWord: 必须为宏定义“I2C_WRITE_PROTECT_WORD”，防止程序跑飞，进行误保存
* 返    回： result:操作结果
* 说    明： 
* 抛出异常：
* 作    者： simon
* 修改记录： 2014-12-10
********************************************************************************
*/
u8 I2C_SaveDataToFRAM(u16 dataNum, u16 saveAddr, u8* dataToSave,u32 checkWord)
{
	u8 result = I2C_OPERATION_FAILED; //;操作结果返回值
	
	if((dataNum > 0) && (I2C_WRITE_PROTECT_WORD == checkWord))
	{
		u8 upperFlag = saveAddr / 256;	//;高位地址标记address[8]
		u8 offset = saveAddr % 256;	//;偏移地址
		u8 ack;		//;应答信号
		u8 trynum;	//;重试次数
		u16 i;		//;计数器
		
		for (trynum = 0; trynum < 5; trynum++)
		{
			I2C_WP_L;		//;允许写操作
			I2C_StartTimingSeq();	//;启动IIC总线操作
			if (upperFlag)
			{
				I2C_SendByte(FRAM_ADDR | FRAM_UPPER_ADDR | FRAM_WRITE); //;高地址写数据操作
			}
			else
			{
				I2C_SendByte(FRAM_ADDR | FRAM_LOWER_ADDR | FRAM_WRITE); //;低地址写数据操作
			}
			ack = I2C_GetAck();
			if (I2C_SLAVE_NOACK == ack)
			{
				continue;
			}
			I2C_SendByte(offset); //;address[7:0]
			ack = I2C_GetAck();
			if (I2C_SLAVE_NOACK == ack)
			{
				continue;
			}
			for (i = 0; i < dataNum; i++)
			{
				I2C_SendByte(dataToSave[i]);
				ack = I2C_GetAck();
				if (I2C_SLAVE_NOACK == ack)
				{
					break;
				}
			}
			if (i != dataNum)
			{
				continue;
			}
			else
			{
				break;
			}
		}
		I2C_StopTimingSeq();
		I2C_TimingDelay(100);
		I2C_WP_H;
		if (trynum <5)
		{
			result = I2C_OPERATION_SUCCEED;
		}
	}
	return result;
}

/* 
********************************************************************************
* 函 数 名： I2C_GetDataFromFRAM
* 功    能： 从FRAM中指定地址读取指定长度的数据
* 参    数： 
*	dataNum：需要读取的数据字节数
*	getAddr：读取位置
*	dataBuf：读取到的数据保存的位置
* 返    回： result:操作结果
* 说    明： 
* 抛出异常：
* 作    者： simon
* 修改记录： 2014-12-10
********************************************************************************
*/
u8 I2C_GetDataFromFRAM(u16 dataNum, u16 getAddr, u8* dataBuf)
{
	u8 result = I2C_OPERATION_FAILED; //;操作结果返回值
	
	if(dataNum > 0)
	{
		u8 upperFlag = getAddr / 256;	//;高位地址标记address[8]
		u8 offset = getAddr % 256;	//;偏移地址
		u8 ack;		//;应答信号
		u8 trynum;	//;重试次数
		u16 i;		//;计数器
		
		for (trynum = 0; trynum < 5; trynum++)
		{
			I2C_WP_L;		//;允许写操作
			I2C_StartTimingSeq();	//;启动IIC总线操作
			if (upperFlag)
			{
				I2C_SendByte(FRAM_ADDR | FRAM_UPPER_ADDR | FRAM_WRITE); //;高地址写数据操作
			}
			else
			{
				I2C_SendByte(FRAM_ADDR | FRAM_LOWER_ADDR | FRAM_WRITE); //;低地址写数据操作
			}
			ack = I2C_GetAck();
			if (I2C_SLAVE_NOACK == ack)
			{
				continue;
			}
			I2C_SendByte(offset); //;address[7:0]
			ack = I2C_GetAck();
			if (I2C_SLAVE_NOACK == ack)
			{
				continue;
			}
			I2C_StartTimingSeq();	//;启动IIC总线操作
			if (upperFlag)
			{
				I2C_SendByte(FRAM_ADDR | FRAM_UPPER_ADDR | FRAM_READ); //;高地址写数据操作
			}
			else
			{
				I2C_SendByte(FRAM_ADDR | FRAM_LOWER_ADDR | FRAM_READ); //;低地址写数据操作
			}
			ack = I2C_GetAck();
			if (I2C_SLAVE_NOACK == ack)
			{
				continue;
			}
			for(i = 0; i < (dataNum - 1); i++)
			{
				dataBuf[i] = I2C_ReceiveByte();
				I2C_GiveAck();
			}
			dataBuf[dataNum - 1] = I2C_ReceiveByte();
			I2C_GiveNoAck();
			I2C_StopTimingSeq();
			I2C_TimingDelay(100);
			result = I2C_OPERATION_SUCCEED;
			break;
		}
	}
	return result;
}

/* 
********************************************************************************
* 函 数 名： I2C_FormatFRAM
* 功    能： 格式化FRAM
* 参    数： 无
* 返    回： 无
* 说    明： 
* 抛出异常：
* 作    者： simon
* 修改记录： 2014-12-10
********************************************************************************
*/
void I2C_FormatFRAM(void)
{
	u8 trynum, ack;
	u16 i;
	
	for (trynum = 0; trynum < 5; trynum++)
	{
		I2C_WP_L;		//;允许写操作
		I2C_StartTimingSeq();	//;启动IIC总线操作
		I2C_SendByte(FRAM_ADDR | FRAM_LOWER_ADDR | FRAM_WRITE); //;低地址写数据操作
		ack = I2C_GetAck();
		if (I2C_SLAVE_NOACK == ack)
		{
			continue;
		}
		I2C_SendByte(0); 	//;address[7:0],从地址0开始
		ack = I2C_GetAck();
		if (I2C_SLAVE_NOACK == ack)
		{
			continue;
		}
		for (i = 0; i < 512; i++)
		{
			I2C_SendByte(0);
			ack = I2C_GetAck();
			if (I2C_SLAVE_NOACK == ack)
			{
				break;
			}
		}
		if (i != 512)
		{
			continue;
		}
		else
		{
			break;
		}
	}
	I2C_StopTimingSeq();
	I2C_TimingDelay(100);
	I2C_WP_H;
        I2C_TimingDelay(5000);
}

/*----------------------------------以下为灌溉表专用函数，移植可删除--------------------------------------*/

//;==================================================
//;将数据和它的校验码保存到存储器的两个位置
//; 输入 n--      数据长度，不包括校验码
//;      EEAdr1 EEAdr2  存储器的两个位置
//;      adr_ram        要保存数据的起始地址
//;==================================================
void SaveData_FM24(u8 n, u16 EEAdr1, u16 EEAdr2, u8 *adr_ram)
{
	u8 i;
	u8 buf[256];
	
	buf[n] = 0x55;
	for (i = 0; i < n; i++)
	{
		buf[i] = *(adr_ram + i);
		buf[n] += *(adr_ram + i);
	}
	I2C_SaveDataToFRAM(n+1, EEAdr1, buf, I2C_WRITE_PROTECT_WORD);
	I2C_SaveDataToFRAM(n+1, EEAdr2, buf, I2C_WRITE_PROTECT_WORD);
}

//;==================================================
//;将数据从存储器的两个位置读出，找出正确的数据
//; 输入 n    --        数据长度，不包括校验码
//;      EEAdr1 EEAdr2  存储器的两个位置
//;      adr_ram        要保存数据的起始地址
//;==================================================
void ReadData_FM24(u8 n, u16 EEAdr1, u16 EEAdr2, u8 *adr_ram)
{
	u8 i, ch;
	u8 buf[256];
	
	I2C_GetDataFromFRAM(n + 1, EEAdr1, buf);
	ch = 0x55;
	for (i = 0; i < n; i++)
	{
		ch += buf[i];
	}
	if (ch == buf[n])
	{
		for (i = 0; i < n; i++)
		{
			*(adr_ram + i) = buf[i];
		}
		return ;
	}
	else
	{
		I2C_GetDataFromFRAM(n + 1, EEAdr2, buf);
		ch = 0x55;
		for (i = 0; i < n; i++)
		{
			ch += buf[i];
		}
		if (ch == buf[n])
		{
			for (i = 0; i < n; i++)
			{
				*(adr_ram + i) = buf[i];
			}
			return ;
		}
		else
		{
			for (i = 0; i < n; i++)
			{
				*(adr_ram + i) = 0;
			}
		}
	}
}
/*------------------------------------------------END---------------------------------------------------*/

