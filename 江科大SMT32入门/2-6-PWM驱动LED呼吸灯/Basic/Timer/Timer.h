#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"
#include "Delay.h"

void Timer_Init(void); //初始化TIM2
uint16_t Timer_GetCounter(void);



#endif
