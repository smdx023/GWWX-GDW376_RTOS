/*
********************************************************************************
*  文 件 名：Setup.C
*
*  文件描述：硬件初始化相关函数  
*
*  所用芯片：
*
*  创 建 人：
*
*  版 本 号：
*
*  修改记录：1.修改延时函数以适应不同时钟。 
********************************************************************************
*/
#include "BSP_CFG.h"

#define I2C1_SLAVE_ADDRESS7    0xa0   //;FM24C16
#define I2C2_SLAVE_ADDRESS7    0x65   //;EPSON8025
#define I2C_Clock              100000

u16 DelayNms;


/* 
********************************************************************************
* 函 数 名：RCC_Configuration
* 功    能：统外围器件时钟配置
* 参    数：无
* 返    回：无
* 说    明：无
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void RCC_Configuration(void)
{
    /* ADC时钟设为APB2的时钟2分频 */
    RCC_ADCCLKConfig(RCC_PCLK2_Div2);

    /* 开启GPIO引脚时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                           RCC_APB2Periph_GPIOC , ENABLE);

    /* 开启I2C1,I2C2时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    /* 开启串口2,串口4,串口5时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 
                           | RCC_APB1Periph_USART3
                           | RCC_APB1Periph_UART5 
                           | RCC_APB1Periph_UART4, ENABLE);

    /* TIM3 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /* 开启串口1,SPI1时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_SPI1, ENABLE);
    
    /* 开启SPI2时钟 */
//;    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    /* Enable DMA1 clock */
//;    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* Enable ADC1 */
//;    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
}


/* 
********************************************************************************
* 函 数 名：NVIC_Configuration
* 功    能：系统中断向量优先级配置
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    /* Enable the RTC Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


/* 
********************************************************************************
* 函 数 名：GPIO_Configuration
* 功    能：初始化IO口
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    //;启动和关闭GPRS模块 PA8,控制基表485通信PA4
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //;推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    //;设置 PA0 为M_EVENT 输入 ,用于检测电表是否有事件发生 @20
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    //;设置 PA4 为M_SET输入 ,用于检测电表是否有设置输入 @20 --20141210
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    
    //;设置 PB14 为 输入 ,用于检测GPRS模块是否启动 @20
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //; PB0, PB1, PB8,PB9 LED指示灯
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //;推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);


    //; 设置 PC6为输入，
    //;P6用于检测模块是否来电，暂无用到
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);


    //;设置 PC9为输出
    //;PC9用于连接M590的EMERGOFF脚，用于紧急关机，暂无用到
    //;PC7 控制网络灯
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_7 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //;推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    //;设置 PC14 为M_STA开漏输出 , @20 --20141210
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; //;开漏输出
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    //;设置 PC15 为M_RST 输入 ,用于检测电表是否有复位输入 @20 --20141210
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    //;设置 PB12输出
    //;PB12设为高电平时给GPRS模块上电
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //;推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

}


/* 
********************************************************************************
* 函 数 名：SysTickStart
* 功    能：配置系统滴答时钟
* 参    数：无
* 返    回：无
* 说    明：1ms
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void SysTickStart(void)
{
    if (SysTick_Config(SystemCoreClock / 1000)) 
    {
      /* Capture error */
      while (1);
    }
}


/* 
********************************************************************************
* 函 数 名：RTC_Configuration
* 功    能：实时时钟250ms中断一次
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void RTC_Configuration(void)
{
      /* Enable PWR and BKP clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);

    /* Reset Backup Domain */
    BKP_DeInit();
    
    //;设置RTC的时钟为HSE的128分频
    RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div128);  

    RCC_RTCCLKCmd(ENABLE);  //;允许RTC的时钟工作

    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Enable the RTC Second */
    RTC_ITConfig(RTC_IT_SEC, ENABLE);

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    //;8M/128=62500HZ
    RTC_SetPrescaler(15624); //; 0.25秒钟产生一次中断

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
}



/* 
********************************************************************************
* 函 数 名：TIM_Configuration
* 功    能：
* 参    数：
* 返    回： 
* 说    明：TIM2 Channel2(PA1) 产生 38KHZ 方波配置
*           系统时钟：56MHZ
*           TIM3_ARR = (56000 / 38) - 1 = 1472;
*           TIM3_CCR1 = 736;
*           占空比 = (TIM3_CCR1/ TIM3_ARR)* 100 = 50%
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void TIM_Configuration(void)
{
    u16 period;
    
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);

    /* Time base configuration  */
    period = (SystemCoreClock / 38000) - 1;
     TIM_TimeBaseStructure.TIM_Period = period;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    /* PWM1 Mode configuration: Channel3 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = period / 2;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);

    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}


/* 
********************************************************************************
* 函 数 名：I2C_Configuration
* 功    能：初始化I2C总线
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
//void I2C_Configuration (void)
//{
//    GPIO_InitTypeDef GPIO_InitStructure;
//    I2C_InitTypeDef  I2C_InitStructure;
//
//    /*I2C-GPIO*/
//    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; //;开漏输出
//    GPIO_Init(GPIOB, &GPIO_InitStructure);
//    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//;推挽输出
//    GPIO_Init(GPIOB, &GPIO_InitStructure);
//    
//    /*I2C1 //取消使用芯片自带I2C --20141210*/
//    /*
//    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C; //;I2C模式
//    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2; //;快速模式Tlow/Thigh=2
//    I2C_InitStructure.I2C_OwnAddress1 = I2C1_SLAVE_ADDRESS7; //;设置第一个设备地址
//    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable; //;使能应答
//    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; //;应答7位地址
//    I2C_InitStructure.I2C_ClockSpeed = I2C_Clock; //;设置时钟频率
//    I2C_Cmd(I2C1, ENABLE);
//    I2C_Init(I2C1, &I2C_InitStructure);
//    */
//}


/* 
********************************************************************************
* 函 数 名：USART_Configuration
* 功    能：初始化USART
* 参    数：无
* 返    回：无
* 说    明：485A :  USART5 -- PC12(TX), PD2(RX)
*           485B :  USART4 -- PC10(TX), PC11(RX)
*           485C :  USART1 -- PA9(TX),  PA10(RX)
*           GPRS :  USART2 -- PA2(TX),  PA3(RX)
*           红外口：USART3 -- PD8(TX),  PD9(RX),
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void USART_Configuration(void)
{
    GPIO_InitTypeDef        GPIO_InitStructure;
    USART_InitTypeDef       USART_InitStructure;
    USART_ClockInitTypeDef  USART_ClockInitStructure;

    //;时钟设置
    USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
    USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
    USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
    USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;

#ifdef Local_USART1_ON 
    /*USART1*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //;浮空输入模式
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_ClockInit(USART1, &USART_ClockInitStructure); //;与USART1相同
   USART_InitStructure.USART_BaudRate = 9600;
//    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);
#else
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif   
    /*USART2*/
//;    GPIO_PinRemapConfig(GPIO_FullRemap_USART2, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_ClockInit(USART2, &USART_ClockInitStructure); //;与USART1相同
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART2, ENABLE);
    
     /*USART3*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    USART_ClockInit(USART3, &USART_ClockInitStructure); //;与USART1相同
    USART_InitStructure.USART_BaudRate = 1200;
    USART_InitStructure.USART_WordLength = USART_WordLength_9b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
//;    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Parity = USART_Parity_Even;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART3, ENABLE);

#ifdef PORT2USART    
    /*USART4*/
//;    GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    USART_ClockInit(UART4, &USART_ClockInitStructure); //;与USART1相同
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_9b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_Even;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(UART4, &USART_InitStructure);
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
    USART_Cmd(UART4, ENABLE);
#else
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
#endif    
    
    /*USART5*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //;浮空输入模式
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //;复用功能推挽输出模式
    GPIO_Init(GPIOC, &GPIO_InitStructure);

   
    USART_ClockInit(UART5, &USART_ClockInitStructure);

    //USART_InitStructure.USART_BaudRate = 9600; //;波特率
    USART_InitStructure.USART_BaudRate = 2400; //;波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_9b; //;连校验位为9个数据位
    USART_InitStructure.USART_StopBits = USART_StopBits_1; //;停止位为1位
    USART_InitStructure.USART_Parity = USART_Parity_Even; //;偶校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //;无硬件控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //;同时允许TX和RX
    USART_Init(UART5, &USART_InitStructure);
    /*允许USART5中断接收*/
    RS485CTRLRCV();
    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
    
    /*允许USART5工作*/
    USART_Cmd(UART5, ENABLE);

}


/* 
********************************************************************************
* 函 数 名：SPI_FLASH_Init
* 功    能：初始化Flash所用的SPI1总线的物理硬件
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void SPI_FLASH_Init(void)
{
    u16 i;

    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    //;初始化 SPI1引脚 SCK, MISO and MOSI
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;;//;复用功能推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //;PC4用于spi1-Flash2片选,PC3用于复位,PC5用于保护
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4|GPIO_Pin_3|GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //;推挽输出
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    //;输出复位信号到flash2芯片
    GPIO_ResetBits(GPIOC, GPIO_Pin_3);
    for (i = 0;i < 100;i++);//;延时
    {
        GPIO_SetBits(GPIOC, GPIO_Pin_4);
    }
    GPIO_SetBits(GPIOC, GPIO_Pin_3);

//;    //;输出复位信号到flash1芯片
//;    GPIO_ResetBits(GPIOC, GPIO_Pin_0);
//;    for (i = 0;i < 100; i++); //;延时
//;    {
//;        GPIO_SetBits(GPIOC, GPIO_Pin_1);
//;    }
//;    GPIO_SetBits(GPIOC, GPIO_Pin_0);

    /* SPI1 configuration */
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //;全双工通讯
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;  //;主控制
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;  //;数据格式为8位
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;   //;时钟为上升沿
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;  //;第2个时钟才开始有效
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;  //;通讯波特率为72M/4=18M
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  //;先发送最高位
    SPI_InitStructure.SPI_CRCPolynomial = 7;   //;循环冗余码校验采用公式 7
    SPI_Init(SPI1, &SPI_InitStructure);
    /* Enable SPI1  */
    SPI_Cmd(SPI1, ENABLE);
}


/* 
********************************************************************************
* 函 数 名：InitHardware
* 功    能：初始化系统硬件
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void InitHardware(void)
{ 
   s8 i;
#ifdef IAP
   // NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x3000);
#endif
    RCC_Configuration();
#ifdef IWDGON
    IWDG_Configuration(); //;打开独立看门狗
 #endif
    NVIC_Configuration();
    GPIO_Configuration();
    
    SPI_FLASH_Init();
    SPI_FLASH_EraseProtReg(0); //;把所有的扇区的保护寄存器全部写为0xff
    I2C_InitFRAM();
    //I2C_Configuration();
    SysTickStart();    //;系统时钟1ms中断一次
    RTC_Configuration();
    USART_Configuration();
    TIM_Configuration();
    LOGONLIGHTOFF;
    COMLIGHTOFF();
    TCPDATALIGHTOFF();
    METERDATALIGHTOFF();
    Modem_PowOFF();
    //;基表供电不稳定，延时2S
    for(i=5; i>0; i--)
    {
        Delay_nms(200);
        Watchdog(); 
        Delay_nms(200); 
        Watchdog();
    }
//    Delay_nms(200);
//    Watchdog(); 
//    Delay_nms(200); 
//    Watchdog(); 
//    Delay_nms(200);
}


/* 
********************************************************************************
* 函 数 名：IWDG_Configuration
* 功    能：配置独立看门狗
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void IWDG_Configuration(void)
{
    /* Enable write access to IWDG_PR and IWDG_RLR registers */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    /* IWDG counter clock: 40KHz(LSI) / 64 = 0.625 KHz =1.6ms*/
    IWDG_SetPrescaler(IWDG_Prescaler_64);

    /* Set counter reload value to 2000 * 1.6ms = 3.2s */
    IWDG_SetReload(2000);

    /* Reload IWDG counter */
    IWDG_ReloadCounter();

    /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
    IWDG_Enable();
}


/* 
********************************************************************************
* 函 数 名：Watchdog
* 功    能：看门狗复位
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void Watchdog(void)
{
#ifdef IWDGON
    IWDG_ReloadCounter();
#endif
}

/* 
********************************************************************************
* 函 数 名：Delay_nms
* 功    能：延时函数，若超过100ms则先喂狗。
* 参    数：延时毫秒数
* 返    回： 
* 说    明：延时不能超过1秒。
* 抛出异常：
* 作    者：023
* 修改记录：2012-05-18 15:16:12
********************************************************************************
*/
void Delay_nms(u16 n)
{
    DelayNms = n;
    if(n > 35)
    {
        Watchdog();
    }
    while(DelayNms)
    {
        ;
    }
}


/* 
********************************************************************************
* 函 数 名：USART1_BaudRateSet
* 功    能：USART2 重设波特率
* 参    数：无
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void USART2_BaudRateSet(u32 BaudRate)
{
  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = BaudRate;  //;波特率
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //;连校验位为8个数据位
  USART_InitStructure.USART_StopBits = USART_StopBits_1;  //;停止位为1位
  USART_InitStructure.USART_Parity = USART_Parity_No;  //;无校验
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //;无硬件控制
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  //;同时允许TX和RX
  USART_Init(USART2, &USART_InitStructure);  //;初始化 USART2
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);  //;允许USART2中断
  USART_Cmd(USART2, ENABLE);  //;允许USART2工作
  
}

/* 
********************************************************************************
* 函 数 名：UARTx_BaudRate_Set
* 功    能：设置串口4,5的波特率
* 参    数：BPS-波特率；UAERx-串口号
* 返    回：无
* 说    明：
* 抛出异常：
* 作    者：023
* 修改记录：
********************************************************************************
*/
void UARTx_BaudRate_Set(u8 BPS, u8 UARTx)
{
    u32 BaudRate;
    USART_InitTypeDef USART_InitStructure;
    USART_ClockInitTypeDef USART_ClockInitStructure;

    switch(BPS)
    {
        case 0x01:
        {
            BaudRate = 600;
            break;
        }
        case 0x02:
        {
            BaudRate = 1200;
            break;
        }
        case 0x03:
        {
            BaudRate = 2400;
            break;
        }
        case 0x04:
        {
            BaudRate = 4800;
            break;
        }
        case 0x05:
        {
            BaudRate = 7200;
            break;
        }
        case 0x06:
        {
            BaudRate = 9600;
            break;
        }
        case 0x07:
        {
            BaudRate = 19200;
            break;
        }
        case 0x08:
        {
            BaudRate = 38400;
            break;
        }
        default:
        {
            BaudRate = 2400;
            break;
        }
    }

    USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
    USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
    USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
    USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;
    USART_InitStructure.USART_BaudRate = BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_9b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_Even;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    
    if(UARTx == 5)
    {
        USART_ClockInit(UART5, &USART_ClockInitStructure);
        USART_Init(UART5, &USART_InitStructure);
        USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
        USART_Cmd(UART5, ENABLE);
    }
#ifdef PORT2USART    
    else if(UARTx == 4)
    {
        USART_ClockInit(UART4, &USART_ClockInitStructure);
        USART_Init(UART4, &USART_InitStructure);
        USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
        USART_Cmd(UART4, ENABLE);
    }
#endif    
}
