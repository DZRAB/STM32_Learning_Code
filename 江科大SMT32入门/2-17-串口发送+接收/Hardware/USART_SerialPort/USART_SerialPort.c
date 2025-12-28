#include "USART_SerialPort.h"

uint8_t Serial_RxData;
uint8_t Serial_RxFlag;

void SerialPort_Init(void)
{
	//????USART1?????
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	//????GPIOA?????
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure; //GPIO ?????????
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //TX???????????
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure); //???? GPIO_InitStruct ??????????????? GPIOx ??¦Ά?υτ
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //RX????????
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure); //???? GPIO_InitStruct ??????????????? GPIOx ??¦Ά?υτ

	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);
	
	USART_ITConfig(USART1,USART_IT_RXNE, ENABLE);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1, ENABLE);

}

//??????????
void SerialPort_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

//???????????
void SerialPort_SendArray(uint8_t* Array, uint16_t Length)
{
	uint16_t i;
	for(i=0; i<Length; i++)
	{
		SerialPort_SendByte(Array[i]);
	}
}
//????????????
void SerialPort_SendString(char* String)
{
	uint16_t i;
	for(i=0; String[i]!='\0'; i++)
	{
		SerialPort_SendByte(String[i]);
	}
}

//????x??y?¦Η?
uint32_t SerialPort_Pow(uint32_t x, uint32_t y)
{
	uint32_t Result = 1;
	while(y--)
	{
		Result *= x;
	}
	
	return Result;
}

//???????????
void SerialPort_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for(i=0;i<Length;i++)
	{
		SerialPort_SendByte(Number / SerialPort_Pow(10,Length-i-1) % 10 + '0');
	}
}

//?????printf???USART1
int fputc(int ch, FILE *f)
{
	SerialPort_SendByte(ch);
	return ch;
}

//???‘€??sprintf
void SerialPort_Printf(char* format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	SerialPort_SendString(String);
}

uint8_t Serial_GetRxFalg(void)
{
	//??????????????	
	if(Serial_RxFlag == 1)
	{
		Serial_RxFlag = 0;
		return 1;
	}
	else
	{
		return 0;
	}
	
}

uint8_t Serial_GerRxData(void)
{
	return Serial_RxData;
}

void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
	{
		Serial_RxData = USART_ReceiveData(USART1);
		Serial_RxFlag = 1;
		
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}
}

