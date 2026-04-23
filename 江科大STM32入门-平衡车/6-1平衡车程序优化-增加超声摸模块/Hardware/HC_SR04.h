#ifndef __HC_SR04_H
#define __HC_SR04_H

#include "stm32f10x.h"   // 包含必要的类型定义

// 对外函数声明
void SR04_Init(void);
void SR04_Trigger(void);
uint16_t Get_Num(void);

// 对外变量声明（extern 表示定义在其他 .c 文件中）
extern volatile uint32_t sr04_width_us;   // 距离脉宽，单位 us
extern volatile uint8_t  sr04_new_data;   // 新数据标志
extern volatile uint32_t sr04_overflow_cnt;
extern uint16_t Num;
#endif
