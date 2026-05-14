#ifndef __TRACK_H
#define __TRACK_H

#include <stdint.h>

/* 四路巡线模块引脚定义
 * TCRT5000四路寻迹模块接线（低电平检测到黑线）：
 *   S1 -> PC14 (原低频晶振引脚，因未使用32.768kHz晶振，已配置为GPIO)
 *   S2 -> PA9  (原USART1_TX，放弃串口调试功能，配置为GPIO)
 *   S3 -> PA10 (原USART1_RX，放弃串口调试功能，配置为GPIO)
 *   S4 -> PC15 (原低频晶振引脚，因未使用32.768kHz晶振，已配置为GPIO)
 * 
 * 传感器间距：S1-S2:26mm, S2-S3:8mm, S3-S4:26mm
 * 对应传感器位置坐标（以S2-S3中点为原点，单位mm）：
 *   S1:-30, S2:-4, S3:4, S4:30
 */

/* 传感器状态位定义 */
#define TRACK_S1_POS   3   // S1(PC14) 对应位3
#define TRACK_S2_POS   2   // S2(PA9)  对应位2
#define TRACK_S3_POS   1   // S3(PA10) 对应位1
#define TRACK_S4_POS   0   // S4(PC15) 对应位0

/* 传感器位置权重（用于加权平均计算误差，单位：mm偏移量） */
#define TRACK_S1_WEIGHT   (-30)
#define TRACK_S2_WEIGHT   (-4)
#define TRACK_S3_WEIGHT   4
#define TRACK_S4_WEIGHT   30

/* 脱轨找回相关参数 */
#define TRACK_LOST_TIMEOUT   200    // 脱轨超时计数值（50ms为周期，200*50ms=10s后进入强制搜索）
#define TRACK_SEARCH_SPEED   0.5f  // 脱轨搜索时的速度（较慢，便于重新上轨）
#define TRACK_NORMAL_SPEED   2.0f  // 正常循迹速度（可通过蓝牙调整）

/* 函数声明 */
void Track_Init(void);                 // 四路巡线模块初始化
void Track_Update(void);               // 读取传感器并计算位置误差
uint8_t Track_GetSensorRaw(void);      // 获取原始传感器数据
float Track_GetError(void);            // 获取计算后的位置误差
void Track_SetSpeed(float Speed);      // 设置循迹速度
float Track_GetSpeed(void);            // 获取当前循迹速度
uint8_t Track_IsOnLine(void);          // 是否在线（至少一个传感器检测到黑线）
uint8_t Track_IsLost(void);            // 是否脱轨

/* 全局变量声明 */
extern uint8_t Track_SensorValues;     // 传感器状态值（bit3=S1, bit2=S2, bit1=S3, bit0=S4）
extern float Track_Error;              // 当前位置误差
extern float Track_LastError;          // 上次位置误差
extern uint8_t Track_Enabled;          // 循迹模式使能标志
extern float Track_Speed;              // 循迹目标速度

#endif
