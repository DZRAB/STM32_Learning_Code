#include "SPI1.h"

void SPI1_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	SPI_InitTypeDef SPI_Structure;
	SPI_Structure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;//72M / 64
	SPI_Structure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_Structure.SPI_CPOL = SPI_CPOL_Low;
	SPI_Structure.SPI_CRCPolynomial = 7;//默认值
	SPI_Structure.SPI_DataSize = SPI_DataSize_8b;
	SPI_Structure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_Structure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_Structure.SPI_Mode = SPI_Mode_Master;
	SPI_Structure.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(SPI1, &SPI_Structure);
	
	/*
	*初始化SPI1默认复用引脚
	*SCK-PA5
	*MOSI-PA7
	*MISO-PA6
	*NSS-软件NSS-PA4
	*/
	GPIO_InitTypeDef GPIO_Structure;
	GPIO_Structure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Structure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_Structure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_Structure);
	
	GPIO_Structure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Structure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Structure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_Structure);

		
	GPIO_Structure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Structure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Structure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_Structure);
	
	SPI_NSSInternalSoftwareConfig(SPI1,SPI_NSSInternalSoft_Set);

	SPI_Cmd(SPI1,ENABLE);
}

void SPI1_SwapDate(uint8_t *pDataTx, uint8_t *pDataRx, uint16_t Size)
{
	
}

