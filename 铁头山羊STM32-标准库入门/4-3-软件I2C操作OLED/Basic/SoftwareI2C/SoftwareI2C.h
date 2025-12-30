#ifndef __SOFTWARE_I2C_H
#define __SOFTWARE_I2C_H
#include "stm32f10x.h"   
#include "Delay.h"

#define SoftwareI2C_RCCPERIPH		RCC_APB2Periph_GPIOB
#define SoftwareI2C_PORT				GPIOB
#define SoftwareI2C_SCL 				GPIO_Pin_8
#define SoftwareI2C_SDA 				GPIO_Pin_9

void SoftwareI2C_W_SCL(uint8_t BitValue);
void SoftwareI2C_W_SDA(uint8_t BitValue);
uint8_t SoftwareI2C_R_SDA(void);

void SoftwareI2C_Init(void);
void SoftwareI2C_Start(void);
void SoftwareI2C_Stop(void);
void SoftwareI2C_SendByte(uint8_t Byte);
uint8_t SoftwareI2C_ReceiveByte(void);
void SoftwareI2C_SendACK(uint8_t ACKValue);
uint8_t SoftwareI2C_ReceiveACK(void);
int8_t SoftwareI2C_SendBytes(uint8_t Addr, uint8_t *pByte, uint16_t Size);
int8_t SoftwareI2C_ReceiveBytes(uint8_t Addr, uint8_t *pByte, uint16_t Size);
#endif
