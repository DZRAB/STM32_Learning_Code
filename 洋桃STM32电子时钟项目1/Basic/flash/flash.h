#ifndef __FLASH_H
#define __FLASH_H
#include "sys.h"
#define FLASH_START_ADDR 0x0801f000 // 写入的起始地址

extern u8 alhour[10]; // 设置闹钟的小时，一共8个，其中8、9两位是温度报警的整数
extern u8 almin[10];  // 设置闹钟的分钟，一共8个，其中8、9两位是温度报警的小数

void FLASH_W(u32 add, u16 dat);
u16 FLASH_R(u32 add);
void ALFlash_W(void); // 写闹钟数据到flash

#endif
