#include "USART.h"

/*USART1，默认复用GPIO初始化*/
void USART1_DefaultInit(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE); //打开USART1时钟
	/*USART1结构体初始化*/
	USART_InitTypeDef USART_InitStructure; //串口初始化结构体
	USART_InitStructure.USART_BaudRate = 115200;//串口波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流量控制关
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //初始化发送和接收功能
	USART_InitStructure.USART_Parity = USART_Parity_No;//校验位初始化
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//停止位初始化
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//数据位初始化
	USART_Init(USART1,&USART_InitStructure);//串口初始化函数
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); //打开USART1复用GPIO时钟
	/*USART1初始化默认复用引脚PA9-TX PA10-RX*/
	GPIO_InitTypeDef GPIO_IintStructure;//GPIO初始化结构体
	GPIO_IintStructure.GPIO_Mode = GPIO_Mode_AF_PP;//TX复用推挽输出
	GPIO_IintStructure.GPIO_Pin = GPIO_Pin_9;//初始化GPIO引脚
	GPIO_IintStructure.GPIO_Speed = GPIO_Speed_10MHz;//GPIO最大输出速度
	GPIO_Init(GPIOA,&GPIO_IintStructure);//GPIO初始化函数
	
	GPIO_IintStructure.GPIO_Mode = GPIO_Mode_IPU;//RX上拉输入
	GPIO_IintStructure.GPIO_Pin = GPIO_Pin_10;//初始化GPIO引脚
	GPIO_IintStructure.GPIO_Speed = GPIO_Speed_10MHz;//输入模式最大输出速度无效
	GPIO_Init(GPIOA,&GPIO_IintStructure);//GPIO初始化函数
	
	USART_Cmd(USART1, ENABLE);//使能USART1
}

	
/*USART1，重映射复用GPIO初始化*/
void USART1_RemapInit(void)
{	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE); //打开USART1时钟	
	/*USART1结构体初始化*/
	USART_InitTypeDef USART_InitStructure; //串口初始化结构体
	USART_InitStructure.USART_BaudRate = 115200;//串口波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流量控制关
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //初始化发送和接收功能
	USART_InitStructure.USART_Parity = USART_Parity_No;//校验位初始化
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//停止位初始化
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//数据位初始化
	USART_Init(USART1,&USART_InitStructure);//串口初始化函数

	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); //打开重映射时钟
	GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);//重映射USART1(USART1_REMAP = 1)
	/*USART1初始化重映射后的复用引脚PB6-TX PB7-RX*/
	GPIO_InitTypeDef GPIO_IintStructure;//GPIO初始化结构体
	GPIO_IintStructure.GPIO_Mode = GPIO_Mode_AF_PP;//TX复用推挽输出
	GPIO_IintStructure.GPIO_Pin = GPIO_Pin_6;//初始化GPIO引脚
	GPIO_IintStructure.GPIO_Speed = GPIO_Speed_10MHz;//GPIO最大输出速度
	GPIO_Init(GPIOB,&GPIO_IintStructure);//GPIO初始化函数
	
	GPIO_IintStructure.GPIO_Mode = GPIO_Mode_IPU;//RX上拉输入
	GPIO_IintStructure.GPIO_Pin = GPIO_Pin_7;//初始化GPIO引脚
	GPIO_IintStructure.GPIO_Speed = GPIO_Speed_10MHz;//输入模式最大输出速度无效
	GPIO_Init(GPIOB,&GPIO_IintStructure);//GPIO初始化函数
	
	USART_Cmd(USART1, ENABLE);//使能USART1
	
}

/*串口1发送一个字节*/
void USART1_SendByte(uint16_t Byte)
{
	//等待USART_FLAG_TXE标志位为1，即发送数据寄存器为空，数据已转移移位寄存器，可以发送新数据
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	
	//发送一个字节
	USART_SendData(USART1,Byte);
	
	//等待USART_FLAG_TC标志位为1，即数据发送完成
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}

/*串口1发送多个字节*/
void USART1_SendBytes(uint8_t *pData, uint16_t Size)
{
	//循环发送Size个字节
	for(uint16_t i=0;i<Size;i++)
	{
		//每次发送字节前，等待USART_FLAG_TXE标志位为1，即发送数据寄存器为空，数据已转移移位寄存器，可以发送新数据
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		USART_SendData(USART1,pData[i]);
	}
	
	//发送完所有字节后，等待USART_FLAG_TC标志位为1，即数据发送完成
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}

/*重写fputc，printf可以通过串口1发送格式化字符串数据*/
int fputc(int ch, FILE *f)
{
	//等待USART_FLAG_TXE标志位为1，即发送数据寄存器为空，数据已转移移位寄存器，可以发送新数据
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	//发送一个字节
	USART_SendData(USART1, (uint8_t)ch);
	return ch;
}



