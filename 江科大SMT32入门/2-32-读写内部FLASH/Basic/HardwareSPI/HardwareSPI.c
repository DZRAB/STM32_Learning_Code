#include "HardwareSPI.h"

void HardwareSPI_W_SS(uint8_t BitValue)
{
	GPIO_WriteBit(HARDWARE_SPI_PORT,HARDWARE_SS_PIN,(BitAction)BitValue);
}

void HardwareSPI_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);//启用APB2外设GPIOx的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//启用APB2外设GPIOx的时钟

	//SS仍然使用GPIO模拟
	GPIO_InitTypeDef GPIO_InitStructure; //GPIO 初始化结构体
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //SCK MOSI输出配置复用推挽
	GPIO_InitStructure.GPIO_Pin = HARDWARE_SCK_PIN | HARDWARE_MOSI_PIN; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(HARDWARE_SPI_PORT,&GPIO_InitStructure); //根据 GPIO_InitStruct 中指定的参数初始化 GPIOx 外围设备

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //SS输出配置推挽
	GPIO_InitStructure.GPIO_Pin = HARDWARE_SS_PIN; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(HARDWARE_SPI_PORT,&GPIO_InitStructure); //根据 GPIO_InitStruct 中指定的参数初始化 GPIOx 外围设备
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //MISO输入配置上拉
	GPIO_InitStructure.GPIO_Pin = HARDWARE_MISO_PIN; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(HARDWARE_SPI_PORT,&GPIO_InitStructure); //根据 GPIO_InitStruct 中指定的参数初始化 GPIOx 外围设备

	SPI_InitTypeDef SPI_InitSturcture;
	SPI_InitSturcture.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;//72M / 128
	SPI_InitSturcture.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitSturcture.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitSturcture.SPI_CRCPolynomial = 7;//默认值
	SPI_InitSturcture.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitSturcture.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitSturcture.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitSturcture.SPI_Mode = SPI_Mode_Master;
	SPI_InitSturcture.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(SPI1,&SPI_InitSturcture);
	
	SPI_Cmd(SPI1,ENABLE);
	HardwareSPI_W_SS(1);
}

void HardwareSPI_Start(void)
{
	HardwareSPI_W_SS(0);
}

void HardwareSPI_Stop(void)
{
	HardwareSPI_W_SS(1);
}

//模式0
uint8_t HardwareSPI_SwapByte(uint8_t ByteSend)
{
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) != SET);
	SPI_I2S_SendData(SPI1,ByteSend);
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE) != SET);
	return (uint8_t)SPI_I2S_ReceiveData(SPI1);
}

