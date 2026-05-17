#ifndef __TRACK_H
#define __TRACK_H

#include "stm32f10x.h"

/*
 * TCRT5000 四路寻迹模块 引脚映射（2025新模块）
 *
 * 接线方式（从左到右）：
 *   S1(最左) → PC14   (备份域引脚，LSE未使用)
 *   S2(左中) → PA9    (原USART1_TX)
 *   S3(右中) → PA10   (原USART1_RX)
 *   S4(最右) → PC15   (备份域引脚，LSE未使用)
 *
 * 传感器物理间距（实测值，总跨度60mm）：
 *   S1 <--26mm--> S2 <--8mm--> S3 <--26mm--> S4
 *
 * 以 S2-S3 中心为原点（偏移±4mm）：
 *   S1=-30mm, S2=-4mm, S3=+4mm, S4=+30mm
 *
 * 传感器电气特性（新模块）：
 *   检测到黑线         → 输出高电平 (HIGH) → 函数返回1
 *   未检测到黑线(白色) → 输出低电平 (LOW)  → 函数返回0
 */

#define TRACK_S1_PIN   GPIO_Pin_14     /* PC14 — 最左传感器 */
#define TRACK_S2_PIN   GPIO_Pin_9      /* PA9  —  左  */
#define TRACK_S3_PIN   GPIO_Pin_10     /* PA10 —  右  */
#define TRACK_S4_PIN   GPIO_Pin_15     /* PC15 — 最右传感器 */

#define TRACK_S1_PORT  GPIOC
#define TRACK_S2_PORT  GPIOA
#define TRACK_S3_PORT  GPIOA
#define TRACK_S4_PORT  GPIOC

/* 传感器位掩码定义 */
#define TRACK_MASK_S1  0x01   /* bit0 = S1 */
#define TRACK_MASK_S2  0x02   /* bit1 = S2 */
#define TRACK_MASK_S3  0x04   /* bit2 = S3 */
#define TRACK_MASK_S4  0x08   /* bit3 = S4 */

/* ── 对外接口 ── */

void Track_Init(void);

uint8_t Track_GetS1(void);
uint8_t Track_GetS2(void);
uint8_t Track_GetS3(void);
uint8_t Track_GetS4(void);

/**
 * @brief  读取全部4路传感器，返回4位掩码
 *         bit0=S1(最左) … bit3=S4(最右)
 *         对应位=1 → 检测到黑线
 */
uint16_t Track_Get(void);

/**
 * @brief  加权重心法计算黑线位置
 *
 *         算法：position = Σ(s[i] * w[i]) / Σ(s[i])
 *         权重 w[i]（单位mm）：
 *             S1=-30,  S2=-4,  S3=+4,  S4=+30
 *
 * @retval 位置坐标（单位：mm）
 *         负值=线偏左，正值=线偏右
 *         所有传感器均未检测到黑线时返回 999.0f
 *         所有传感器均检测到黑线时返回 0.0f
 */
float Track_GetPosition(void);

#endif
