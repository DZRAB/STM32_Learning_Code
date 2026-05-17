/*
 * Track.c — TCRT5000 四路寻迹模块驱动（新模块2025）
 *
 * 本模块负责：
 *   1. 初始化4路数字寻迹传感器的GPIO
 *   2. 提供读取传感器状态的接口
 *   3. 使用加权重心法计算黑线位置
 *
 * 硬件信息：
 *   - 模块：TCRT5000 四路红外寻迹模块（数字输出）
 *   - 输出逻辑：黑线→HIGH(3.3V)  白面→LOW(0V)（新模块）
 *   - 传感器间距：S1-S2=26mm, S2-S3=8mm, S3-S4=26mm
 *   - 总宽度：60mm
 *
 * 引脚分配：
 *   S1(最左)=PC14, S2=PA9, S3=PA10, S4(最右)=PC15
 *   PC14/PC15为备份域引脚，需关闭LSE后使用
 */

#include "Track.h"

/*──────────────────────────────────────────────────────────────────────
 *  硬件初始化
 *──────────────────────────────────────────────────────────────────────*/

void Track_Init(void)
{
    /* ── 第1步：使能 GPIO 时钟 ── */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    /* ── 第2步：PC14/PC15 备份域特殊处理 ── */
    /* PC14/PC15默认与LSE(32.768kHz)相关联，
     * 要作为GPIO使用必须关闭LSE并解锁备份域 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    RCC_LSEConfig(RCC_LSE_OFF);
    BKP_TamperPinCmd(DISABLE);
    BKP_RTCOutputConfig(BKP_RTCOutputSource_None);

    /* ── 第3步：配置 PA9(原USART1_TX)、PA10(原USART1_RX) 为上拉输入 ── */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* ── 第4步：配置 PC14(S1)、PC15(S4) 为上拉输入 ── */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/*──────────────────────────────────────────────────────────────────────
 *  单传感器读取
 *──────────────────────────────────────────────────────────────────────*/

uint8_t Track_GetS1(void)
{
    return (GPIO_ReadInputDataBit(TRACK_S1_PORT, TRACK_S1_PIN) == Bit_SET) ? 1 : 0;
}
uint8_t Track_GetS2(void)
{
    return (GPIO_ReadInputDataBit(TRACK_S2_PORT, TRACK_S2_PIN) == Bit_SET) ? 1 : 0;
}
uint8_t Track_GetS3(void)
{
    return (GPIO_ReadInputDataBit(TRACK_S3_PORT, TRACK_S3_PIN) == Bit_SET) ? 1 : 0;
}
uint8_t Track_GetS4(void)
{
    return (GPIO_ReadInputDataBit(TRACK_S4_PORT, TRACK_S4_PIN) == Bit_SET) ? 1 : 0;
}

/*──────────────────────────────────────────────────────────────────────
 *  全传感器读取 — 位掩码方式
 *──────────────────────────────────────────────────────────────────────
 * 返回值：低4位有效
 *   bit3   bit2   bit1   bit0
 *   (S4)   (S3)   (S2)   (S1)
 *   PC15   PA10   PA9    PC14
 */
uint16_t Track_Get(void)
{
    uint16_t val = 0;
    if (Track_GetS1()) val |= TRACK_MASK_S1;
    if (Track_GetS2()) val |= TRACK_MASK_S2;
    if (Track_GetS3()) val |= TRACK_MASK_S3;
    if (Track_GetS4()) val |= TRACK_MASK_S4;
    return val;
}

/*──────────────────────────────────────────────────────────────────────
 *  核心算法：加权重心法计算黑线位置
 *──────────────────────────────────────────────────────────────────────
 *
 *  新模块4路传感器坐标（以S2-S3中心为原点）：
 *     S1=-30mm, S2=-4mm, S3=+4mm, S4=+30mm
 *
 *  计算示例：
 *     S1 S2 S3 S4         位置(mm)     含义
 *     ○  ●  ○  ○          -4.0       S2检测到，略偏左
 *     ○  ○  ●  ○          +4.0       略偏右
 *     ○  ●  ●  ○           0.0       S2+S3中间，居中
 *     ●  ○  ○  ○         -30.0      最左侧
 *     ○  ○  ○  ●         +30.0      最右侧
 *     ●  ●  ●  ●           0.0      全黑
 *     ○  ○  ○  ○         999.0      未找到线
 *
 *  S2-S3间距仅8mm(<18mm轨迹宽)，盲区大幅减少。
 *  轨迹宽度足够覆盖S2和S3同时检测。
 */
float Track_GetPosition(void)
{
    /* 读取4路传感器状态 */
    uint8_t s[4];
    s[0] = Track_GetS1();
    s[1] = Track_GetS2();
    s[2] = Track_GetS3();
    s[3] = Track_GetS4();

    /*
     * 权重坐标（mm，以S2-S3中心为原点）：
     *   S1      S2      S3      S4
     *   │       │       │       │
     *  -30     -4      +4     +30
     */
    const float weights[4] = {-30.0f, -4.0f, 4.0f, 30.0f};

    float numerator   = 0.0f;   /* 分子：Σ(s[i] × w[i]) */
    float denominator = 0.0f;   /* 分母：Σ(s[i])        */
    uint8_t i;

    for (i = 0; i < 4; i++)
    {
        if (s[i])
        {
            numerator   += weights[i];
            denominator += 1.0f;
        }
    }

    /* 无传感器检测到黑线 */
    if (denominator < 0.5f)
    {
        return 999.0f;
    }

    /* 全部传感器检测到黑线（4路全触发） */
    if (denominator > 3.5f)
    {
        return 0.0f;
    }

    return numerator / denominator;
}
