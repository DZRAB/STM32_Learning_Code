#ifndef __USART_H
#define __USART_H
#include "stm32f10x.h"     
#include <stdio.h>

void USART1_DefaultInit(void);
void USART1_RemapInit(void);
void USART1_SendByte(uint16_t Byte);
void USART1_SendBytes(uint8_t *pData, uint16_t Size);


#endif
