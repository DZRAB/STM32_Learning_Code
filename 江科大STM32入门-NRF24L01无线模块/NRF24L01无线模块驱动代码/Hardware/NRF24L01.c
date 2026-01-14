#include "NRF24L01.h"  

uint8_t NRF24L01_RXAddress[5] = {0x01,0x02,0x03,0x04,0x05};
uint8_t NRF24L01_TXAddress[5] = {0x01,0x02,0x03,0x04,0x05};
uint8_t NRF24L01_TXPacket[NRF24L01_TX_PACKET_WIDTH];
uint8_t NRF24L01_RXPacket[NRF24L01_RX_PACKET_WIDTH];


void NRF24L01_W_CE(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_8, (BitAction)BitValue);
}

void NRF24L01_W_CSN(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, (BitAction)BitValue);
}

void NRF24L01_W_SCK(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_3, (BitAction)BitValue);
}

void NRF24L01_W_MOSI(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_5, (BitAction)BitValue);
}

uint8_t NRF24L01_R_MISO(void)
{
	return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4);
}

void NRF24L01_GPIO_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	/*开启GPIO时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	/*将CE、CSN、SCK、MOSI引脚初始化为推挽输出模式*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/*将MISO引脚初始化为上拉输入模式*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/*置引脚初始化后的默认电平*/
	NRF24L01_W_CE(0);		//CE默认为0，退出收发模式
	NRF24L01_W_CSN(1);		//CSN默认为1，不选中从机
	NRF24L01_W_SCK(0);		//SCK默认为0，对应SPI模式0
	NRF24L01_W_MOSI(0);		//MOSI默认电平随意，1和0均可
}

uint8_t NRF24L01_SPI_SwapByte(uint8_t Byte)
{
	uint8_t i;
	for(i = 0; i<8; i++)
	{
		if(Byte & 0x80)
		{
			NRF24L01_W_MOSI(1);
		}
		else
		{
			NRF24L01_W_MOSI(0);
		}
		Byte <<= 1;
		NRF24L01_W_SCK(1);
		
		if(NRF24L01_R_MISO() != 0)
		{
			Byte |= 0x01;
		}
		else
		{
			Byte &= ~0x01;		
		}
		NRF24L01_W_SCK(0);
	}
	
	return Byte;
}

void NRF24L01_WriteReg(uint8_t RegAddress, uint8_t Data)
{
	NRF24L01_W_CSN(0);
	NRF24L01_SPI_SwapByte(NRF24L01_W_REGISTER | RegAddress);
	NRF24L01_SPI_SwapByte(Data);
	NRF24L01_W_CSN(1);
}

uint8_t NRF24L01_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	
	NRF24L01_W_CSN(0);
	NRF24L01_SPI_SwapByte(NRF24L01_R_REGISTER | RegAddress);
	Data = NRF24L01_SPI_SwapByte(NRF24L01_NOP);
	NRF24L01_W_CSN(1);
	return Data;
}

void NRF24L01_WriteRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count)
{
	NRF24L01_W_CSN(0);
	NRF24L01_SPI_SwapByte(NRF24L01_W_REGISTER | RegAddress);
	for(uint8_t i=0; i<Count; i++)
	{
		NRF24L01_SPI_SwapByte(DataArray[i]);
	}
	NRF24L01_W_CSN(1);
}

void NRF24L01_ReadRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count)
{
	NRF24L01_W_CSN(0);
	NRF24L01_SPI_SwapByte(NRF24L01_R_REGISTER | RegAddress);
	for(uint8_t i=0; i<Count; i++)
	{
		DataArray[i] = NRF24L01_SPI_SwapByte(NRF24L01_NOP);
	}
	
	NRF24L01_W_CSN(1);
}

void NRF24L01_WriteTXPatload(uint8_t *DataArray, uint8_t Count)
{
	NRF24L01_W_CSN(0);
	NRF24L01_SPI_SwapByte(NRF24L01_W_TX_PAYLOAD);
	for(uint8_t i=0; i<Count; i++)
	{
		NRF24L01_SPI_SwapByte(DataArray[i]);
	}
	NRF24L01_W_CSN(1);
}

void NRF24L01_ReadRXPatload(uint8_t *DataArray, uint8_t Count)
{
	NRF24L01_W_CSN(0);
	NRF24L01_SPI_SwapByte(NRF24L01_R_RX_PAYLOAD);
	for(uint8_t i=0; i<Count; i++)
	{
		DataArray[i] = NRF24L01_SPI_SwapByte(NRF24L01_NOP);
	}
	
	NRF24L01_W_CSN(1);
}

void NRF24L01_FlushTX(void)
{
	NRF24L01_W_CSN(0);
	NRF24L01_SPI_SwapByte(NRF24L01_FLUSH_TX);
	NRF24L01_W_CSN(1);
}

void NRF24L01_FlushRX(void)
{
	NRF24L01_W_CSN(0);
	NRF24L01_SPI_SwapByte(NRF24L01_FLUSH_RX);
	NRF24L01_W_CSN(1);
}
	
uint8_t NRF24L01_ReadStatus(void)
{
	uint8_t StatusData;
	NRF24L01_W_CSN(0);
	StatusData = NRF24L01_SPI_SwapByte(NRF24L01_NOP);
	NRF24L01_W_CSN(1);
	return StatusData;
}


//掉电模式
void NRF24L01_PowerDown(void)
{
	uint8_t Config;
	NRF24L01_W_CE(0);
	Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
	Config &= ~0x02;
	NRF24L01_WriteReg(NRF24L01_CONFIG,Config);
}
//待机模式1
void NRF24L01_Standby1(void)
{
	uint8_t Config;
	NRF24L01_W_CE(0);
	Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
	Config |= 0x02;
	NRF24L01_WriteReg(NRF24L01_CONFIG,Config);
}
//接收模式
void NRF24L01_RXMode(void)
{
	uint8_t Config;
	NRF24L01_W_CE(0);
	Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
	Config |= 0x03;
	NRF24L01_WriteReg(NRF24L01_CONFIG,Config);
	NRF24L01_W_CE(1);	
}
//发收模式
void NRF24L01_TXMode(void)
{
	uint8_t Config;
	NRF24L01_W_CE(0);
	Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
	Config |= 0x02;
	Config &= ~0x01;
	NRF24L01_WriteReg(NRF24L01_CONFIG,Config);
	NRF24L01_W_CE(1);	
}
void NRF24L01_Init(void)
{
	NRF24L01_GPIO_Init();
	NRF24L01_WriteReg(NRF24L01_CONFIG,0x08);
	NRF24L01_WriteReg(NRF24L01_EN_AA,0x3F);
	NRF24L01_WriteReg(NRF24L01_EN_RXADDR,0x01);
	NRF24L01_WriteReg(NRF24L01_SETUP_AW,0x03);
	NRF24L01_WriteReg(NRF24L01_SETUP_RETR,0x03);
	NRF24L01_WriteReg(NRF24L01_RF_CH,0x02);
	NRF24L01_WriteReg(NRF24L01_RF_SETUP,0x0E);
	
	NRF24L01_WriteRegs(NRF24L01_RX_ADDR_P0,NRF24L01_RXAddress,5);
	
	NRF24L01_WriteReg(NRF24L01_RX_PW_P0,NRF24L01_RX_PACKET_WIDTH);
	
	NRF24L01_RXMode();
}

void NRF24L01_Send(void)
{
	uint8_t Status;
	NRF24L01_WriteRegs(NRF24L01_TX_ADDR,NRF24L01_TXAddress,5);
	NRF24L01_WriteTXPatload(NRF24L01_TXPacket,NRF24L01_TX_PACKET_WIDTH);
	NRF24L01_WriteRegs(NRF24L01_RX_ADDR_P0,NRF24L01_TXAddress,5);
	
	NRF24L01_TXMode();
	while(1)
	{
		Status = NRF24L01_ReadStatus();
		if(Status & 0x20)
		{
			//发送成功
			break;
		}
		else if(Status & 0x10)
		{
			//发送失败
			break;
		}
	}
	
	NRF24L01_WriteReg(NRF24L01_STATUS,0x30);
	NRF24L01_FlushTX();
	NRF24L01_WriteRegs(NRF24L01_RX_ADDR_P0,NRF24L01_RXAddress,5);

	NRF24L01_RXMode();	
}

uint8_t  NRF24L01_Receive(void)
{
	uint8_t Status;
	Status = NRF24L01_ReadStatus();
	if(Status & 0x40)
	{
		NRF24L01_ReadRXPatload(NRF24L01_RXPacket,NRF24L01_RX_PACKET_WIDTH);
		NRF24L01_WriteReg(NRF24L01_STATUS,0x40);
		NRF24L01_FlushRX();
		return 1;	
	}
	return 0;
}

