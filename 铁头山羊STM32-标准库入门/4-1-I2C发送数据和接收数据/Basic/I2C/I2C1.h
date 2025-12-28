#ifndef __I2C1_H
#define __I2C1_H
#include "stm32f10x.h"     

void I2C1_DefaultInit(void);
void I2C1_RemapInit(void);
int8_t I2C1_SendByte(uint8_t Addr, uint8_t *pData, uint16_t Size);

#endif
