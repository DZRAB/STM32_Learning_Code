#ifndef __RP_ADC_H
#define __RP_ADC_H

#include "stm32f10x.h" 

void RP_Init(void);
uint16_t RP_GetValue(uint8_t n);

#endif
