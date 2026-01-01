#include "I2C1.h"

/*I2C1，默认复用GPIO初始化*/
void I2C1_DefaultInit(void)
{
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);//打开I2C1的时钟
	/*I2C1结构体初始化*/
	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;//使能应答位,
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;//应答地址，选择7位，从机模式下才有效
	I2C_InitStructure.I2C_ClockSpeed = 50*1000;//I2C时钟频率50KHZ，最大400KHZ
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;//快速模式的时钟占空比，选择Tlow/Thigh = 2
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;//模式选择
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;//从模式的自身地址，从机模式下才有效
	I2C_Init(I2C1,&I2C_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //打开I2C1复用GPIO时钟
	/*I2C1初始化默认复用引脚PB6-SCL PB7-SDA*/
	GPIO_InitTypeDef GPIO_IintStructure;//GPIO初始化结构体
	GPIO_IintStructure.GPIO_Mode = GPIO_Mode_AF_OD;//复用开漏输出
	GPIO_IintStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;//初始化GPIO引脚
	GPIO_IintStructure.GPIO_Speed = GPIO_Speed_10MHz;//GPIO最大输出速度
	GPIO_Init(GPIOB,&GPIO_IintStructure);//GPIO初始化函数
		
	I2C_Cmd(I2C1, ENABLE);//使能I2C1
}

/*I2C1，重映射复用GPIO初始化*/
void I2C1_RemapInit(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);//打开I2C1的时钟
	/*I2C1结构体初始化*/
	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;//使能应答位
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;//应答地址，选择7位，从机模式下才有效
	I2C_InitStructure.I2C_ClockSpeed = 50*1000;//I2C时钟频率50KHZ，最大400KHZ
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;//快速模式的时钟占空比，选择Tlow/Thigh = 2
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;//模式选择
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;//从模式的自身地址，从机模式下才有效
	I2C_Init(I2C1,&I2C_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); //打开重映射AFIO时钟
	GPIO_PinRemapConfig(GPIO_Remap_I2C1,ENABLE);//重映射I2C1(I2C1_REMAP = 1)
	/*I2C1初始化重映射后的复用引脚PB8-SCL PB9-SDA*/
	GPIO_InitTypeDef GPIO_IintStructure;//GPIO初始化结构体
	GPIO_IintStructure.GPIO_Mode = GPIO_Mode_AF_OD;//复用开漏输出
	GPIO_IintStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;//初始化GPIO引脚
	GPIO_IintStructure.GPIO_Speed = GPIO_Speed_10MHz;//GPIO最大输出速度
	GPIO_Init(GPIOB,&GPIO_IintStructure);//GPIO初始化函数
		
	I2C_Cmd(I2C1, ENABLE);//使能I2C1
}

/*I2C1，向从机地址Addr，发送多个字节*/
int8_t I2C1_SendByte(uint8_t Addr, uint8_t *pData, uint16_t Size)
{
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET);//等待总线空闲
	I2C_GenerateSTART(I2C1,ENABLE);//产生起始位
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_SB) == RESET);//等待起始位发送成功
	
	I2C_ClearFlag(I2C1,I2C_FLAG_AF);//清除应答标志位
	I2C_SendData(I2C1, Addr & 0xFE);//发送从机地址
	while(1)
	{
		if(I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) == SET)//接收应答失败，发送地址失败
		{
			I2C_GenerateSTOP(I2C1,ENABLE);//产生停止位
			return -1;
		}
		if(I2C_GetFlagStatus(I2C1, I2C_FLAG_ADDR) == SET) break;//发送地址成功
		
	}
	/*清除I2C_FLAG_ADDR状态*/
	I2C_ReadRegister(I2C1,I2C_Register_SR1);
	I2C_ReadRegister(I2C1,I2C_Register_SR2);
	/*循环发送数据*/
	for(uint16_t i=0;i<Size;i++)
	{
		while(1)
		{
			if(I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) == SET)//接收应答失败，发送数据失败
			{
				I2C_GenerateSTOP(I2C1,ENABLE);//产生停止位
				return -2;
			}
			if(I2C_GetFlagStatus(I2C1, I2C_FLAG_TXE) == SET)///数据寄存器为空
			{
				break;
			}	
		}
		I2C_SendData(I2C1, pData[i]);//发送数据
	}
	while(1)
	{
		if(I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) == SET)//接收应答失败，发送数据失败
		{
			I2C_GenerateSTOP(I2C1,ENABLE);//产生停止位
			return -2;
		}
		if(I2C_GetFlagStatus(I2C1, I2C_FLAG_TXE) == SET)///数据寄存器为空
		{
			break;
		}	
	}
			
	I2C_GenerateSTOP(I2C1,ENABLE);//产生停止位
	return 0;	
}

/*I2C1，向从机地址Addr，接收多个字节*/
int8_t I2C1_ReceiveByte(uint8_t Addr, uint8_t *pData, uint16_t Size)
{
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET);//等待总线空闲
	I2C_GenerateSTART(I2C1,ENABLE);//产生起始位
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_SB) == RESET);//等待起始位发送成功
	
	I2C_ClearFlag(I2C1,I2C_FLAG_AF);//清除应答标志位
	I2C_SendData(I2C1, Addr | 0x01);//发送从机地址
	while(1)
	{
		if(I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) == SET)//接收应答失败，发送地址失败
		{
			I2C_GenerateSTOP(I2C1,ENABLE);//产生停止位
			return -1;
		}
		if(I2C_GetFlagStatus(I2C1, I2C_FLAG_ADDR) == SET) break;//发送地址成功		
	}
	/*清除I2C_FLAG_ADDR状态*/
	I2C_ReadRegister(I2C1,I2C_Register_SR1);
	I2C_ReadRegister(I2C1,I2C_Register_SR2);
	
	if(Size == 1)
	{
		I2C_AcknowledgeConfig(I2C1,DISABLE);//接收最后一个字节需要提前发送NACK，失能ACK
		I2C_GenerateSTOP(I2C1,ENABLE);//接收最后一个字节需要提前发送停止位
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET);//等待接收寄存器为非空
		pData[0] = I2C_ReceiveData(I2C1);
	}
	else if(Size == 2)
	{
		I2C_AcknowledgeConfig(I2C1,ENABLE);//接收数据前，使能ACK
		
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET);//等待接收寄存器为非空
		pData[0] = I2C_ReceiveData(I2C1);
		
		I2C_AcknowledgeConfig(I2C1,DISABLE);//接收最后一个字节需要提前发送NACK，失能ACK
		I2C_GenerateSTOP(I2C1,ENABLE);//接收最后一个字节需要提前发送停止位
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET);//等待接收寄存器为非空
		pData[1] = I2C_ReceiveData(I2C1);
		
	}
	else if(Size > 2)
	{
		I2C_AcknowledgeConfig(I2C1,ENABLE);//接收数据前，使能ACK	
		
		for(uint16_t i=0;i<Size-1;i++)//Size-1表示只读到倒数第二位
		{
				while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET);//等待接收寄存器为非空
				pData[i] = I2C_ReceiveData(I2C1);
		}
		
		/*读最后一位*/
		I2C_AcknowledgeConfig(I2C1,DISABLE);//接收最后一个字节需要提前发送NACK，失能ACK
		I2C_GenerateSTOP(I2C1,ENABLE);//接收最后一个字节需要提前发送停止位
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET);//等待接收寄存器为非空
		pData[Size-1] = I2C_ReceiveData(I2C1);
	}
	
	return 0;
}













