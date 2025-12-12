#ifndef __HARDWARE_SPI_H
#define __HARDWARE_SPI_H

#include "stm32f10x.h"                  // Device header
#include "Delay.h"
/*
定义SPI1的GPIO
*/
#define HARDWARE_SPI_PORT 		GPIOA
#define HARDWARE_SCK_PIN 		GPIO_Pin_5
#define HARDWARE_MOSI_PIN 		GPIO_Pin_7
#define HARDWARE_MISO_PIN 		GPIO_Pin_6
#define HARDWARE_SS_PIN 		GPIO_Pin_4

void HardwareSPI_Init(void);
void HardwareSPI_Start(void);
void HardwareSPI_Stop(void);
//模式0
uint8_t HardwareSPI_SwapByte(uint8_t ByteSend);

#endif
