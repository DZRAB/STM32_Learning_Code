#ifndef __MY_DMA_H
#define __MY_DMA_H

#include "stm32f10x.h"
#include "Delay.h"

void MyDMA_Init(uint32_t AddrA, uint32_t AddrB, uint16_t Size); //≥ı ºªØDMA
void MyDMA_Transfer(void);

#endif
