#include "SoftwareSPI.h"

void SoftwareSPI_W_SCK(uint8_t BitValue)
{
	GPIO_WriteBit(SOFTWARE_SPI_PORT,SOFTWARE_SCK_PIN,(BitAction)BitValue);
}
void SoftwareSPI_W_MOSI(uint8_t BitValue)
{
	GPIO_WriteBit(SOFTWARE_SPI_PORT,SOFTWARE_MOSI_PIN,(BitAction)BitValue);
}
void SoftwareSPI_W_SS(uint8_t BitValue)
{
	GPIO_WriteBit(SOFTWARE_SPI_PORT,SOFTWARE_SS_PIN,(BitAction)BitValue);
}
uint8_t SoftwareSPI_R_MISO(void)
{
	return GPIO_ReadInputDataBit(SOFTWARE_SPI_PORT,SOFTWARE_MISO_PIN);
}

void SoftwareSPI_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//启用APB2外设GPIOx的时钟
	GPIO_InitTypeDef GPIO_InitStructure; //GPIO 初始化结构体
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //输出配置推挽
	GPIO_InitStructure.GPIO_Pin = SOFTWARE_SCK_PIN | SOFTWARE_MOSI_PIN | SOFTWARE_SS_PIN; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SOFTWARE_SPI_PORT,&GPIO_InitStructure); //根据 GPIO_InitStruct 中指定的参数初始化 GPIOx 外围设备
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //输入配置上拉
	GPIO_InitStructure.GPIO_Pin = SOFTWARE_MISO_PIN; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SOFTWARE_SPI_PORT,&GPIO_InitStructure); //根据 GPIO_InitStruct 中指定的参数初始化 GPIOx 外围设备
	
	SoftwareSPI_W_SS(1);
	SoftwareSPI_W_SCK(0);
}

void SoftwareSPI_Start(void)
{
	SoftwareSPI_W_SS(0);
}

void SoftwareSPI_Stop(void)
{
	SoftwareSPI_W_SS(1);
}
//掩码的方式交换字节-模式0
uint8_t SoftwareSPI_Mask_SwapByte(uint8_t ByteSend)
{
	uint8_t ByteReceive=0, i=0;
	for(i=0; i<8; i++)
	{
		SoftwareSPI_W_MOSI(!!(ByteSend & (0x80>>i)));
		SoftwareSPI_W_SCK(1);
		if(SoftwareSPI_R_MISO())
		{
			ByteReceive = ByteReceive | (0x80>>i);
		}
		SoftwareSPI_W_SCK(0);
	}
	return ByteReceive;
}
//移位方式交换字节-模式0
uint8_t SoftwareSPI_SwapByte(uint8_t ByteSend)
{
	uint8_t i=0;
	for(i=0; i<8; i++)
	{
		SoftwareSPI_W_MOSI(ByteSend & 0x80);
		ByteSend = ByteSend<<1; //把发送的高位移出，低位自动补0
		SoftwareSPI_W_SCK(1);
		if(SoftwareSPI_R_MISO())
		{
			ByteSend = ByteSend | 0x01; //接收交换的高位放到ByteSend的最低位
		}
		SoftwareSPI_W_SCK(0);
	}
	return ByteSend;
}

//移位方式交换字节-模式1
uint8_t SoftwareSPI_SwapByte1(uint8_t ByteSend)
{
	uint8_t i=0;
	for(i=0; i<8; i++)
	{
		SoftwareSPI_W_SCK(1);
		SoftwareSPI_W_MOSI(ByteSend & 0x80);
		SoftwareSPI_W_SCK(0);
		ByteSend = ByteSend<<1; //把发送的高位移出，低位自动补0
		if(SoftwareSPI_R_MISO())
		{
			ByteSend = ByteSend | 0x01; //接收交换的高位放到ByteSend的最低位
		}
		
	}
	return ByteSend;
}

//移位方式交换字节-模式2
uint8_t SoftwareSPI_SwapByte2(uint8_t ByteSend)
{
	uint8_t i=0;
	SoftwareSPI_W_SCK(1);
	for(i=0; i<8; i++)
	{
		SoftwareSPI_W_MOSI(ByteSend & 0x80);
		SoftwareSPI_W_SCK(0);
		ByteSend = ByteSend<<1; //把发送的高位移出，低位自动补0
		if(SoftwareSPI_R_MISO())
		{
			ByteSend = ByteSend | 0x01; //接收交换的高位放到ByteSend的最低位
		}
		SoftwareSPI_W_SCK(1);
	}
	return ByteSend;
}

//移位方式交换字节-模式3
uint8_t SoftwareSPI_SwapByte3(uint8_t ByteSend)
{
	uint8_t i=0;
	for(i=0; i<8; i++)
	{
		SoftwareSPI_W_SCK(0);
		SoftwareSPI_W_MOSI(ByteSend & 0x80);
		SoftwareSPI_W_SCK(1);
		ByteSend = ByteSend<<1; //把发送的高位移出，低位自动补0
		if(SoftwareSPI_R_MISO())
		{
			ByteSend = ByteSend | 0x01; //接收交换的高位放到ByteSend的最低位
		}
		
	}
	return ByteSend;
}

