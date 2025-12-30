#ifndef __USART1_H
#define __USART1_H
#include "stm32f10x.h"     
#include <stdio.h>

void USART1_DefaultInit(void);
void USART1_RemapInit(void);
void USART1_SendByte(uint16_t Byte);
void USART1_SendBytes(uint8_t *pData, uint16_t Size);
void USART1_SendString(uint8_t *String);
uint8_t USART1_ReceiveByte(void);

#endif
