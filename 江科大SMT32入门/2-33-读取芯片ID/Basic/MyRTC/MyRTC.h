#ifndef __MY_RTC_H
#define __MY_RTC_H

#include "stm32f10x.h"
#include "Delay.h"

extern uint16_t MyRTC_Time[];

void MyRTC_Init(void);
void MyRTC_SetTime(void);
void MyRTC_ReadTime(void);

#endif
