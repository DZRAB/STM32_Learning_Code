#ifndef __TRACK_H
#define __TRACK_H

#include "stm32f10x.h"

/*
 * TCRT5000 五路寻迹模块 引脚映射
 * 
 * 接线方式（从左到右）：
 *   S1(最左) → PA9     (原USART1_TX，现已释放)
 *   S2(左中) → PA10    (原USART1_RX，现已释放)
 *   S3(中间) → PA11
 *   S4(右中) → PA12
 *   S5(最右) → PC15    (备份域引脚，LSE晶振未使用)
 * 
 * 传感器电气特性：
 *   ┌─────────────────────────────────────────────┐
 *   │ 检测到黑线           → 输出低电平 (LOW)     │
 *   │ 未检测到黑线(白色)   → 输出高电平 (HIGH)    │
 *   │ 超出检测范围(悬空)   → 输出低电平 (LOW)     │
 *   └─────────────────────────────────────────────┘
 * 注意：当车体倾斜过大时传感器可能脱离地面，此时输出
 *       LOW(0) 会被误判为黑线，需保证传感器距地面
 *       高度合适（通常10~15mm，配合模块上的可调电位器校准）。
 * 
 * 传感器物理间距（实测值，从S1中心到S5中心共93mm）：
 *   S1 <--22mm--> S2 <--24.5mm--> S3 <--24.5mm--> S4 <--22mm--> S5
 */
#define TRACK_S1_PIN   GPIO_Pin_9      /* PA9  — 最左传感器 */
#define TRACK_S2_PIN   GPIO_Pin_10     /* PA10 —  左  */
#define TRACK_S3_PIN   GPIO_Pin_11     /* PA11 — 中   */
#define TRACK_S4_PIN   GPIO_Pin_12     /* PA12 —  右  */
#define TRACK_S5_PIN   GPIO_Pin_15     /* PC15 — 最右传感器 */

#define TRACK_S1_PORT  GPIOA
#define TRACK_S2_PORT  GPIOA
#define TRACK_S3_PORT  GPIOA
#define TRACK_S4_PORT  GPIOA
#define TRACK_S5_PORT  GPIOC

/* 传感器位掩码定义 */
#define TRACK_MASK_S1  0x01   /* bit0 = S1 */
#define TRACK_MASK_S2  0x02   /* bit1 = S2 */
#define TRACK_MASK_S3  0x04   /* bit2 = S3 */
#define TRACK_MASK_S4  0x08   /* bit3 = S4 */
#define TRACK_MASK_S5  0x10   /* bit4 = S5 */

/* ── 对外接口 ── */

/**
 * @brief  寻迹传感器硬件初始化
 *         - PA9~PA12 配置为上拉输入
 *         - PC15 属于备份域，需先关闭LSE+使能备份域访问
 */
void Track_Init(void);

/**
 * @brief  读取单个传感器状态
 * @retval 1 = 检测到黑线（引脚为低电平）
 *         0 = 未检测到黑线（引脚为高电平）
 */
uint8_t Track_GetS1(void);
uint8_t Track_GetS2(void);
uint8_t Track_GetS3(void);
uint8_t Track_GetS4(void);
uint8_t Track_GetS5(void);

/**
 * @brief  读取全部5路传感器，返回5位掩码
 *         bit0=S1(最左) … bit4=S5(最右)
 *         对应位=1 → 检测到黑线
 * @retval 掩码值，例：0x04 仅中间传感器检测到线
 */
uint16_t Track_Get(void);

/**
 * @brief  计算黑线相对于车体的横向偏移位置
 *         
 *         算法：加权重心法
 *         position = Σ(s[i] * w[i]) / Σ(s[i])
 *         
 *         其中 s[i] ∈ {0,1} 为第i个传感器的检测结果，
 *         w[i] 为以S3(中心)为原点的物理坐标(mm)：
 *             S1=-46.5,  S2=-24.5,  S3=0,  S4=24.5,  S5=46.5
 *         
 *         举例：
 *         只S3检测到 → pos = 0          → 线在正中间
 *         S2+S3检测  → pos = -12.25mm   → 线略微偏左
 *         只S5检测到 → pos = +46.5mm    → 线在最右侧
 *         
 * @retval 位置坐标（单位：mm）
 *         负值=线偏左，正值=线偏右
 *         所有传感器均未检测到黑线时返回 999.0f
 *         所有传感器均检测到黑线（例外情况）返回 0.0f
 */
float Track_GetPosition(void);

#endif
