#ifndef __SPI1_H
#define __SPI1_H
#include "stm32f10x.h"     

void SPI1_Init(void);
void SPI1_SwapDate(uint8_t *pDataTx, uint8_t *pDataRx, uint16_t Size);

#endif
