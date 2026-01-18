#include "BlueSerial.h"

uint8_t BlueSerial_RxFlag;
char BlueSerial_RxPacket[100];

void BlueSerial_Init(void)
{
	//配置USART2的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	//配置GPIOA的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure; //GPIO 初始化结构体
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //TX复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure); //根据 GPIO_InitStruct 中指定的参数初始化 GPIOx 外围设备
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //RX上拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure); //根据 GPIO_InitStruct 中指定的参数初始化 GPIOx 外围设备

	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &USART_InitStructure);
	
	USART_ITConfig(USART2,USART_IT_RXNE, ENABLE);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART2, ENABLE);
}

//发送一个字节
void BlueSerial_SendByte(uint8_t Byte)
{
	USART_SendData(USART2, Byte);
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

//发送一个数组
void BlueSerial_SendArray(uint8_t* Array, uint16_t Length)
{
	uint16_t i;
	for(i=0; i<Length; i++)
	{
		BlueSerial_SendByte(Array[i]);
	}
}
//发送一个字符串
void BlueSerial_SendString(char* String)
{
	uint16_t i;
	for(i=0; String[i]!='\0'; i++)
	{
		BlueSerial_SendByte(String[i]);
	}
}

//计算x的y次方
uint32_t BlueSerial_Pow(uint32_t x, uint32_t y)
{
	uint32_t Result = 1;
	while(y--)
	{
		Result *= x;
	}
	
	return Result;
}

//发送一串数字
void BlueSerial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for(i=0;i<Length;i++)
	{
		BlueSerial_SendByte(Number / BlueSerial_Pow(10,Length-i-1) % 10 + '0');
	}
}

////重定义printf输出USART2
//int fputc(int ch, FILE *f)
//{
//	BlueSerial_SendByte(ch);
//	return ch;
//}

//重新封装sprintf
void BlueSerial_Printf(char* format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	BlueSerial_SendString(String);
}


void USART2_IRQHandler(void)
{
	static uint8_t RxState = 0;
	static uint8_t pRxPacket = 0;

	if(USART_GetITStatus(USART2,USART_IT_RXNE) == SET)
	{
		uint8_t RxDate = USART_ReceiveData(USART2);
		
		if(RxState == 0) //等待包头
		{
			if(RxDate == '+' && BlueSerial_RxFlag == 0) 
			{
				RxState = 1;
				pRxPacket = 0;
			}

		}
		else if(RxState == 1) //等待数据
		{
			if(RxDate == '\r')
			{
				RxState = 2;
			}
			else
			{
				BlueSerial_RxPacket[pRxPacket] = RxDate;
				pRxPacket++;
			}
		}	
		else if(RxState == 2) //等待包尾
		{
			if(RxDate == '\n')
			{
				RxState = 0;
				BlueSerial_RxPacket[pRxPacket++] = '\0'; //结束后，给接收到的字符串增加字符串标志为
				BlueSerial_RxFlag = 1;//接收完成 置标志位
			}
		}

		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
	}
}
