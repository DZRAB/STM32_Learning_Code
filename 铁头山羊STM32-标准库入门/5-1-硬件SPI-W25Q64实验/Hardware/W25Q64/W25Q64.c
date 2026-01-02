#include "W25Q64.h"

void W25Q64_SaveByte(uint8_t Byte)
{
	uint8_t buff[10];
	
	buff[0] = W25Q64_WRITE_ENABLE;
	//写使能
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET);
	SPI1_SwapDate(buff,buff,1);
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET);
	
	buff[0] = W25Q64_SECTOR_ERASE_4KB;
	buff[1] = 0x00;
	buff[2] = 0x00;
	buff[3] = 0x00;
	//扇区擦除
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET);
	SPI1_SwapDate(buff,buff,4);
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET);
	
	while(1)
	{
		buff[0] = W25Q64_READ_STATUS_REGISTER_1;
		//等待空闲
		GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET);
		SPI1_SwapDate(buff,buff,1);
		buff[0] = 0xff;
		SPI1_SwapDate(buff,buff,1);
		GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET);
		if((buff[0] & 0x01) == 0) break;
	}


	buff[0] = W25Q64_WRITE_ENABLE;
	//写使能
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET);
	SPI1_SwapDate(buff,buff,1);
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET);
	
	buff[0] = W25Q64_PAGE_PROGRAM;
	buff[1] = 0x00;
	buff[2] = 0x00;
	buff[3] = 0x00;	
	buff[4] = Byte;	
	//页编程
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET);
	SPI1_SwapDate(buff,buff,5);
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET);
	
	while(1)
	{
		buff[0] = W25Q64_READ_STATUS_REGISTER_1;
		//等待空闲
		GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET);
		SPI1_SwapDate(buff,buff,1);
		buff[0] = 0xff;
		SPI1_SwapDate(buff,buff,1);
		GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET);
		if((buff[0] & 0x01) == 0) break;
	}
}
	
uint8_t W25Q64_ReadByte(void)
{
	uint8_t buff[10];

	//读数据
	buff[0] = W25Q64_READ_DATA;
	buff[1] = 0x00;
	buff[2] = 0x00;
	buff[3] = 0x00;
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET);
	SPI1_SwapDate(buff,buff,4);
	buff[0] = 0xff;
	SPI1_SwapDate(buff,buff,1);
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET);
	return buff[0];
}
	

















