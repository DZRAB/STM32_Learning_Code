#include "USART_SerialPort.h"

void SerialPort_Init(void)
{
	//配置USART1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	//配置GPIOA的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure; //GPIO 初始化结构体
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //TX复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure); //根据 GPIO_InitStruct 中指定的参数初始化 GPIOx 外围设备
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	
	USART_Init(USART1, &USART_InitStructure);
	
	USART_Cmd(USART1, ENABLE);

}

//发送一个字节
void SerialPort_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

//发送一个数组
void SerialPort_SendArray(uint8_t* Array, uint16_t Length)
{
	uint16_t i;
	for(i=0; i<Length; i++)
	{
		SerialPort_SendByte(Array[i]);
	}
}
//发送一个字符串
void SerialPort_SendString(char* String)
{
	uint16_t i;
	for(i=0; String[i]!='\0'; i++)
	{
		SerialPort_SendByte(String[i]);
	}
}

//计算x的y次方
uint32_t SerialPort_Pow(uint32_t x, uint32_t y)
{
	uint32_t Result = 1;
	while(y--)
	{
		Result *= x;
	}
	
	return Result;
}

//发送一串数字
void SerialPort_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for(i=0;i<Length;i++)
	{
		SerialPort_SendByte(Number / SerialPort_Pow(10,Length-i-1) % 10 + '0');
	}
}

//重定义printf输出USART1
int fputc(int ch, FILE *f)
{
	SerialPort_SendByte(ch);
	return ch;
}

//重新封装sprintf
void SerialPort_Printf(char* format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	SerialPort_SendString(String);
}