#ifndef __SOFTWARE_SPI_H
#define __SOFTWARE_SPI_H

#include "stm32f10x.h"                  // Device header
#include "Delay.h"
/*
定义软件模拟SPI的GPIO，更改GPIO只需要修改下面5行宏定义，如果更换GPIO端口，如要在RCC初始化的时候修改GPIO端口
*/
#define SOFTWARE_SPI_PORT 		GPIOA
#define SOFTWARE_SCK_PIN 		GPIO_Pin_5
#define SOFTWARE_MOSI_PIN 		GPIO_Pin_7
#define SOFTWARE_MISO_PIN 		GPIO_Pin_6
#define SOFTWARE_SS_PIN 		GPIO_Pin_4
/*
带参数的宏定义，这里用作示例参考，主要使用函数封装
*/
#define SOFTWARE_SCK(x) 		GPIO_WriteBit(SOFTWARE_SPI_PORT,SOFTWARE_SCK_PIN,(BitAction)(x))
#define SOFTWARE_SS(x)			GPIO_WriteBit(SOFTWARE_SPI_PORT,SOFTWARE_SS_PIN,(BitAction)(x))
#define SOFTWARE_MOSI(x) 		GPIO_WriteBit(SOFTWARE_SPI_PORT,SOFTWARE_MOSI_PIN,(BitAction)(x))
#define SOFTWARE_MISO()			GPIO_ReadInputDataBit(SOFTWARE_SPI_PORT,SOFTWARE_MISO_PIN)

void SoftwareSPI_Init(void);
void SoftwareSPI_Start(void);
void SoftwareSPI_Stop(void);
//掩码的方式交换字节-模式0
uint8_t SoftwareSPI_Mask_SwapByte(uint8_t ByteSend);
//移位方式交换字节-模式0
uint8_t SoftwareSPI_SwapByte(uint8_t ByteSend);
//移位方式交换字节-模式1
uint8_t SoftwareSPI_SwapByte1(uint8_t ByteSend);
//移位方式交换字节-模式2
uint8_t SoftwareSPI_SwapByte2(uint8_t ByteSend);
//移位方式交换字节-模式3
uint8_t SoftwareSPI_SwapByte3(uint8_t ByteSend);

#endif
