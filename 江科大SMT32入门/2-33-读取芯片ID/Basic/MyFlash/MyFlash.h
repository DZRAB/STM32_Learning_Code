#ifndef __MY_FLASH_H
#define __MY_FLASH_H

#include "stm32f10x.h"
#include "Delay.h"

uint32_t MyFlash_ReadWord(uint32_t Address);
uint16_t MyFlash_ReadHalfWord(uint32_t Address);
uint8_t MyFlash_ReadByte(uint32_t Address);
void MyFlash_EraseAllPages(void);
void MyFlash_ErasePages(uint32_t Address);
void MyFlash_ProgramWord(uint32_t Address, uint32_t Data);
void MyFlash_ProgramHalfWord(uint32_t Address, uint16_t Data);

#endif
