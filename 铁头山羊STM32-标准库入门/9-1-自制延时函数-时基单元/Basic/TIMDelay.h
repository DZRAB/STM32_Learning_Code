#ifndef __TIM_DELAY_H 
#define __TIM_DELAY_H
#include "stm32f10x.h"                  // Device header

void TIM1_Init(void);
uint32_t TIM1_GetCurrentTick(void);
void TIM1_tick(void);
void TIM1_Delay_ms(uint32_t ms);


#endif
