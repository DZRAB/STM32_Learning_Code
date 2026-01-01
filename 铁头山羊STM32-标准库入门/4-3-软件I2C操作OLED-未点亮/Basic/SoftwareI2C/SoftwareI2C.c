#include "SoftwareI2C.h"


void SoftwareI2C_W_SCL(uint8_t BitValue)
{
	GPIO_WriteBit(SoftwareI2C_PORT, SoftwareI2C_SCL, (BitAction)BitValue);
	Delay_us(1);
}

void SoftwareI2C_W_SDA(uint8_t BitValue)
{
	GPIO_WriteBit(SoftwareI2C_PORT, SoftwareI2C_SDA, (BitAction)BitValue);
	Delay_us(1);
}

uint8_t SoftwareI2C_R_SDA(void)
{
	uint8_t BitValue = GPIO_ReadInputDataBit(SoftwareI2C_PORT, SoftwareI2C_SDA);
	Delay_us(1);
	return BitValue;
}

void SoftwareI2C_Init(void)
{
	
	RCC_APB2PeriphClockCmd(SoftwareI2C_RCCPERIPH,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Pin = SoftwareI2C_SCL | SoftwareI2C_SDA;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(SoftwareI2C_PORT, &GPIO_InitStruct);
	
	SoftwareI2C_W_SCL(0);
	SoftwareI2C_W_SDA(1);
	SoftwareI2C_W_SCL(1);
}

void SoftwareI2C_Start(void)
{
	SoftwareI2C_W_SDA(0);
}

void SoftwareI2C_Stop(void)
{
	SoftwareI2C_W_SCL(0);
	SoftwareI2C_W_SDA(0);
	SoftwareI2C_W_SCL(1);
	SoftwareI2C_W_SDA(1);
}

void SoftwareI2C_SendByte(uint8_t Byte)
{
	for(uint8_t i=0;i<8;i++)
	{
		SoftwareI2C_W_SCL(0);
		SoftwareI2C_W_SDA(!!(Byte & (0x80>>i)));
		SoftwareI2C_W_SCL(1);
	}
}

uint8_t SoftwareI2C_ReceiveByte(void)
{
	uint8_t Byte = 0x00;
	SoftwareI2C_W_SCL(0);
	SoftwareI2C_W_SDA(1);//主机释放SDA
	
	for(uint8_t i=0;i<8;i++)
	{
		SoftwareI2C_W_SCL(0);
		SoftwareI2C_W_SCL(1);
		if(SoftwareI2C_R_SDA() != 0)
		{
			Byte = Byte | (0x80>>i);
		}
	}
	
	return Byte;
}

void SoftwareI2C_SendACK(uint8_t ACKValue)
{
	SoftwareI2C_W_SCL(0);
	SoftwareI2C_W_SDA(ACKValue);
	SoftwareI2C_W_SCL(1);
}

uint8_t SoftwareI2C_ReceiveACK(void)
{
	uint8_t ACKValue;
	SoftwareI2C_W_SCL(0);
	SoftwareI2C_W_SDA(1);//主机释放SDA
	SoftwareI2C_W_SCL(1);	
	ACKValue = SoftwareI2C_R_SDA();
	
	return ACKValue;
}

int8_t SoftwareI2C_SendBytes(uint8_t Addr,const uint8_t *pByte, uint16_t Size)
{
	SoftwareI2C_Start();
	SoftwareI2C_SendByte(Addr & 0xfe);//这里的地址需要向左移动一位后的地址
	if(SoftwareI2C_ReceiveACK() != 0) //没有接收到ACK
	{
		SoftwareI2C_Stop();
		return -1;
	}
	for(uint16_t i=0;i<Size;i++)
	{
		SoftwareI2C_SendByte(pByte[i]);
		if(SoftwareI2C_ReceiveACK() != 0) //没有接收到ACK
		{
			SoftwareI2C_Stop();
			return -2;
		}
	}
	SoftwareI2C_Stop();
	return 0;
}

int8_t SoftwareI2C_ReceiveBytes(uint8_t Addr, uint8_t *pByte, uint16_t Size)
{
	SoftwareI2C_Start();
	SoftwareI2C_SendByte(Addr | 0x01);//这里的地址需要向左移动一位后的地址
	if(SoftwareI2C_ReceiveACK() != 0) //没有接收到ACK
	{
		SoftwareI2C_Stop();
		return -1;
	}
	for(uint16_t i=0;i<Size-1;i++)
	{
		pByte[i] = SoftwareI2C_ReceiveByte();
		SoftwareI2C_SendACK(0);//发送应答0
	}
	pByte[Size-1] = SoftwareI2C_ReceiveByte();
	SoftwareI2C_SendACK(1);//不发送应答，非应答1
	
	SoftwareI2C_Stop();

	return 0;
	
}


