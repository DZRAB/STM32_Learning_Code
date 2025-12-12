#ifndef __SOFTWARE_I2C_H
#define __SOFTWARE_I2C_H

#include "stm32f10x.h"                  // Device header
#include "Delay.h"
/*
定义软件模拟I2C的GPIO，更改GPIO只需要修改下面三行宏定义，如果更换GPIO端口，如要在RCC初始化的时候修改GPIO端口
*/
#define SOFTWARE_I2C_PORT 		GPIOB
#define SOFTWARE_SCL_PIN 		GPIO_Pin_10
#define SOFTWARE_SDA_PIN 		GPIO_Pin_11
/*
带参数的宏定义，这里用作示例参考，主要使用函数封装
*/
#define SOFTWARE_SCL(x) 		GPIO_WriteBit(SOFTWARE_I2C_PORT,SOFTWARE_SCL_PIN ,(BitAction)(x))
#define SOFTWARE_SDA(x)			GPIO_WriteBit(SOFTWARE_I2C_PORT,SOFTWARE_SDA_PIN ,(BitAction)(x))

void SoftwareI2C_Init(void);
//开始
void SoftwareI2C_Start(void);
//终止
void SoftwareI2C_Stop(void);
//发送一个字节
void SoftwareI2C_SendByte(uint8_t Byte);
//接收一个字节
uint8_t SoftwareI2C_ReceiveByte(void);
//发送应答
void SoftwareI2C_SendACK(uint8_t ACKBit);
//接收应答
uint8_t SoftwareI2C_ReceiveACK(void);


#endif
