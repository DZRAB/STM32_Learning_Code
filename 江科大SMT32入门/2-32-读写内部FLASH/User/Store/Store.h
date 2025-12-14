#ifndef __MY_STORE_H
#define __MY_STORE_H

#include "stm32f10x.h"
#include "Delay.h"
#include "MyFlash.h"

#define STORE_START_ADDRESS 0x0800FC00
#define STORE_COUNT 		512


extern uint16_t Store_Data[STORE_COUNT];


void Stroe_Init(void);
void Store_Save(void);
void Store_Clear(void);

#endif
