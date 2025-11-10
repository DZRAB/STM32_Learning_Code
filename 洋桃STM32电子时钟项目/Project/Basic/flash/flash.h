#ifndef __FLASH_H
#define __FLASH_H 			   
#include "sys.h"
#define FLASH_START_ADDR 0x0801f000 // 写入的起始地址
 
extern	u8 alhour[8];													 // 设置每个闹钟的小时，一共8个
extern	u8 almin[8];													 // 设置每个闹钟的分钟，一共8个

void FLASH_W(u32 add,u16 dat);
u16 FLASH_R(u32 add);
void ALFlash_W(void); // 写闹钟数据到flash

#endif





























