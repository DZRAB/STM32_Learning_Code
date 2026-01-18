#include "SoftwareI2C.h"

//I2C读写IO，封装成函数，有更好的兼容性，在高主频的MCU上，可以在这里更好的加延时
void SoftwareI2C_W_SCL(uint8_t BitValue)
{
	GPIO_WriteBit(SOFTWARE_I2C_PORT,SOFTWARE_SCL_PIN ,(BitAction)BitValue);
	//Delay_us(10);
}

//I2C读写IO，封装成函数，有更好的兼容性，在高主频的MCU上，可以在这里更好的加延时
void SoftwareI2C_W_SDA(uint8_t BitValue)
{
	GPIO_WriteBit(SOFTWARE_I2C_PORT,SOFTWARE_SDA_PIN ,(BitAction)BitValue);
	//Delay_us(10);
}

//I2C读写IO，封装成函数，有更好的兼容性，在高主频的MCU上，可以在这里更好的加延时
uint8_t SoftwareI2C_R_SDA(void)
{
	uint8_t BitValue;
	BitValue = GPIO_ReadInputDataBit(SOFTWARE_I2C_PORT,SOFTWARE_SDA_PIN);
	//Delay_us(10);
	return BitValue;
}

void SoftwareI2C_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//启用APB2外设GPIOx的时钟,软件I2C使用的GPIO端口要修改为实际使用的端口
	
	GPIO_InitTypeDef GPIO_InitStructure; //GPIO 初始化结构体
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; //开漏输出
	GPIO_InitStructure.GPIO_Pin = SOFTWARE_SCL_PIN  | SOFTWARE_SDA_PIN ; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SOFTWARE_I2C_PORT,&GPIO_InitStructure); //根据 GPIO_InitStruct 中指定的参数初始化 GPIOx 外围设备
	
	SoftwareI2C_W_SDA(1);
	SoftwareI2C_W_SCL(1);
}	

//开始
void SoftwareI2C_Start(void)
{
	SoftwareI2C_W_SDA(1);//先释放SDA
	SoftwareI2C_W_SCL(1);
	SoftwareI2C_W_SDA(0);
	SoftwareI2C_W_SCL(0);
}

//终止
void SoftwareI2C_Stop(void)
{
	SoftwareI2C_W_SDA(0);//先拉低SDA
	SoftwareI2C_W_SCL(1);
	SoftwareI2C_W_SDA(1);
}

//发送一个字节
void SoftwareI2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for(i=0;i<8;i++)
	{
		SoftwareI2C_W_SDA(!!((Byte << i) & 0x80));
		SoftwareI2C_W_SCL(1);
		SoftwareI2C_W_SCL(0);
	}
}

//接收一个字节
uint8_t SoftwareI2C_ReceiveByte(void)
{
	uint8_t i, Byte=0x00;
	SoftwareI2C_W_SDA(1);
	for(i=0;i<8;i++)
	{
		SoftwareI2C_W_SCL(1);
		
		if (SoftwareI2C_R_SDA())
		{
			Byte = Byte | (0x80 >> i);
		}	
		//读取SDA数据，并存储到Byte变量
		//当SDA为1时，置变量指定位为1，当SDA为0时，不做处理，指定位为默认的初值0

		SoftwareI2C_W_SCL(0);
	}
	return Byte;
}

//发送应答
void SoftwareI2C_SendACK(uint8_t ACKBit)
{
	SoftwareI2C_W_SDA(ACKBit);
	SoftwareI2C_W_SCL(1);
	SoftwareI2C_W_SCL(0);
}

//接收应答
uint8_t SoftwareI2C_ReceiveACK(void)
{
	uint8_t ACKBit;
	SoftwareI2C_W_SDA(1);
	SoftwareI2C_W_SCL(1);
	ACKBit = SoftwareI2C_R_SDA();
	SoftwareI2C_W_SCL(0);
	return ACKBit;
}



