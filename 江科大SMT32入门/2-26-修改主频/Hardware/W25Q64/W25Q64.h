#ifndef __M25Q64_H
#define __M25Q64_H

#include "stm32f10x.h" 
#include "HardwareSPI.h"

void W25Q64_Init(void);
void W25Q64_ReadID(uint8_t *MID,uint16_t *DID);
//Count最大值256，一次只能写256个数据
void W25Q64_PageProgram(uint32_t Address, uint8_t *DataArray, uint16_t Count);
void W25Q64_SectorErase4KB(uint32_t Address);
//读数据Count无大小限制
void W25Q64_ReadData(uint32_t Address, uint8_t *DataArray, uint32_t Count);
#endif
