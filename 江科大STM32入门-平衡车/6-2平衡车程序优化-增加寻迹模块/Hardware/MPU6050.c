#include "MPU6050.h"
#include "MPU6050_Reg.h"

//指定地址写寄存器数据
void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data)
{
	SoftwareI2C_Start();
	SoftwareI2C_SendByte(MPU6050_ADDRESS);
	SoftwareI2C_ReceiveACK();
	SoftwareI2C_SendByte(RegAddress);
	SoftwareI2C_ReceiveACK();
	SoftwareI2C_SendByte(Data);
	SoftwareI2C_ReceiveACK();
	SoftwareI2C_Stop();
}

//指定地址读寄存器数据
uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	
	SoftwareI2C_Start();
	SoftwareI2C_SendByte(MPU6050_ADDRESS);
	SoftwareI2C_ReceiveACK();
	SoftwareI2C_SendByte(RegAddress);
	SoftwareI2C_ReceiveACK();
	
	SoftwareI2C_Start();
	SoftwareI2C_SendByte(MPU6050_ADDRESS | 0x01);
	SoftwareI2C_ReceiveACK();
	Data = SoftwareI2C_ReceiveByte();
	SoftwareI2C_SendACK(1);
	SoftwareI2C_Stop();
	
	return Data;
}

//指定地址读一组寄存器数据
void MPU6050_ReadRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count)
{
	uint8_t i;
	
	SoftwareI2C_Start();
	SoftwareI2C_SendByte(MPU6050_ADDRESS);
	SoftwareI2C_ReceiveACK();
	SoftwareI2C_SendByte(RegAddress);
	SoftwareI2C_ReceiveACK();
	
	SoftwareI2C_Start();
	SoftwareI2C_SendByte(MPU6050_ADDRESS | 0x01);
	SoftwareI2C_ReceiveACK();
	for(i=0;i<Count;i++)
	{
		DataArray[i] = SoftwareI2C_ReceiveByte();
		if(i<Count-1)
		{
			SoftwareI2C_SendACK(0);
		}
		else
		{
			SoftwareI2C_SendACK(1);
		}
	}
	SoftwareI2C_Stop();
	
}


void MPU6050_Init(void)
{
	SoftwareI2C_Init();
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1,0x01);
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2,0x00);
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV,0x07);
	MPU6050_WriteReg(MPU6050_CONFIG,0x00);
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG,0x18);
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG,0x18);

}
uint8_t  MPU6050_GetID(void)
{
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}

void MPU6050_GetData(int16_t* AccX, int16_t* AccY, int16_t* AccZ,
					 int16_t* GyroX,int16_t* GyroY,int16_t* GyroZ)
{
//	uint8_t DataH, DataL;
	
//	DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
//	DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
//	*AccX = (DataH << 8) | DataL;
//	
//	DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
//	DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
//	*AccY = (DataH << 8) | DataL;
//	
//	DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
//	DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
//	*AccZ = (DataH << 8) | DataL;
//	
//	DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
//	DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
//	*GyroX = (DataH << 8) | DataL;
//	
//	DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
//	DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
//	*GyroY = (DataH << 8) | DataL;
//	
//	DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
//	DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
//	*GyroZ = (DataH << 8) | DataL;
	uint8_t Data[14];
	MPU6050_ReadRegs(MPU6050_ACCEL_XOUT_H,Data,14);
	*AccX = (Data[0] << 8) | Data[1];
	*AccY = (Data[2] << 8) | Data[3];
	*AccZ = (Data[4] << 8) | Data[5];
	*GyroX = (Data[8] << 8) | Data[9];
	*GyroY = (Data[10] << 8) | Data[11];
	*GyroZ = (Data[12] << 8) | Data[13];
	
}


