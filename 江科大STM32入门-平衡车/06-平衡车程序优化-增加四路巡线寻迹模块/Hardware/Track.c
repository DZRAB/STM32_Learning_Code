#include "stm32f10x.h"
#include "Track.h"

/**************************************************************************************
 * 四路巡线寻迹模块 (TCRT5000)
 * 
 * 【功能说明】
 * 本模块实现基于TCRT5000四路红外反射传感器的循迹功能。
 * TCRT5000的工作原理：红外发射管发射红外光，红外接收管接收反射回来的光。
 * - 黑色表面吸收红外光 → 接收管截止 → 输出低电平(0)
 * - 白色表面反射红外光 → 接收管导通 → 输出高电平(1)
 * 
 * 【接线定义】
 * S2 -> PA9  (原USART1_TX，已放弃串口调试，改为GPIO输入)
 * S3 -> PA10 (原USART1_RX，已放弃串口调试，改为GPIO输入)
 * S1 -> PC14 (原32.768kHz晶振引脚，未使用LSE，改为GPIO输入)
 * S4 -> PC15 (原32.768kHz晶振引脚，未使用LSE，改为GPIO输入)
 * 
 * 【传感器间距布局】
 *      S1(PC14) ---- S2(PA9) -- S3(PA10) ---- S4(PC15)
 *        26mm          8mm          26mm
 * 位置坐标系（以S2-S3中点为原点，向左为负，向右为正）：
 *   S1: -30mm, S2: -4mm, S3: +4mm, S4: +30mm
 * 
 * 【循迹算法说明 - 加权质心法】
 * 采用加权质心(Weighted Centroid)算法计算线的位置误差：
 *   Error = Σ(传感器权重 × 传感器检测值) / Σ(传感器检测值)
 * 其中传感器检测值：1=检测到黑线(低电平), 0=未检测到(高电平)
 * 
 * 例：S2和S3同时检测到黑线（线在中心）：
 *   Error = (0×(-30) + 1×(-4) + 1×4 + 0×30) / (0+1+1+0) = 0/2 = 0
 * 
 * 【脱轨找回策略】
 * 当所有传感器都检测不到黑线时（sum==0），判定为脱轨：
 * 1. 短时脱轨(<10次采样)：放大上次误差方向，使小车朝线消失的方向加大转向
 * 2. 长时脱轨(>=10次采样)：交替搜索模式，左-右周期性摆动寻找黑线
 * 3. 恢复上轨后：立即恢复正常循迹
 * 
 * 【PC14/PC15用作GPIO的说明】
 * STM32F103的PC14和PC15与LSE(32.768kHz低速外部晶振)复用。
 * 默认上电后LSE处于关闭状态，此时PC14/PC15可作为普通GPIO使用。
 * 为确保安全，初始化时显式关闭LSE（RCC_LSEConfig(RCC_LSE_OFF)）。
 * 
 * 注意事项：由于板子上物理焊接了32.768kHz晶振，晶振会作为容性负载连接在
 * PC14/PC15引脚上。这会导致：
 * - GPIO输入仍正常工作（读到的电平仍然正确）
 * - 可能会略微增加功耗（晶振和电容的漏电流）
 * - 实际使用中不影响功能，可以正常工作
 **************************************************************************************/

/* 全局变量定义 */
uint8_t Track_SensorValues = 0;     // 传感器状态值（bit3=S1, bit2=S2, bit1=S3, bit0=S4）
uint8_t Track_SensorSum = 0;        // 检测到黑线的传感器总数
float Track_Error = 0.0f;           // 当前位置误差
float Track_LastError = 0.0f;       // 上次位置误差（用于脱轨时方向判断）
uint8_t Track_Enabled = 0;          // 循迹模式使能标志（0=关闭，1=开启）
float Track_Speed = TRACK_NORMAL_SPEED;  // 循迹目标速度（单位：转/秒）

/* 脱轨控制变量 */
static uint8_t Track_LostCount = 0;         // 脱轨计数
static uint8_t Track_SearchPhase = 0;       // 搜索阶段计数（用于周期搜索模式）

/**
 * 函    数：四路巡线模块初始化
 * 参    数：无
 * 返 回 值：无
 * 说    明：将PA9、PA10、PC14、PC15配置为GPIO输入模式
 *           PA9和PA10原为USART1功能，放弃使用
 *           PC14和PC15原为LSE晶振功能，显式关闭LSE后作为GPIO使用
 */
void Track_Init(void)
{
    /* 开启GPIO时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // PA9, PA10
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);  // PC14, PC15
    
    /* ======== LSE晶振配置：PC14和PC15用作GPIO ======== */
    /* 显式关闭LSE（低速外部晶振），确保PC14和PC15可作GPIO使用 */
    /* 注意：LSE默认上电后即为关闭状态，此处显式配置更安全可靠 */
    RCC_LSEConfig(RCC_LSE_OFF);
    /* 等待LSE关闭完成 */
    /* LSE关闭不需要等待，但为确保后续配置正确，加入短暂延时 */
    volatile uint32_t delay = 1000;
    while (delay--);
    
    /* ======== PA9(S2)和PA10(S3)引脚配置 ======== */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;     // 上拉输入（TCRT5000输出开漏，上拉确保高电平）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);             // PA9(原USART1_TX), PA10(原USART1_RX)
    
    /* ======== PC14(S1)和PC15(S4)引脚配置 ======== */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;     // 上拉输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOC, &GPIO_InitStructure);             // PC14(S1), PC15(S4)
}

/**
 * 函    数：读取传感器状态并计算位置误差
 * 参    数：无
 * 返 回 值：无
 * 说    明：此函数需在定时中断中周期性调用（推荐50ms周期）
 * 
 * 【算法详解 - 加权质心法】
 * 使用加权质心法计算黑线的精确位置，相比于简单的"查表法"，
 * 加权质心法具有以下优势：
 * 1. 连续输出：误差值是连续的浮点数，而非离散值
 * 2. 亚传感器精度：当黑线位于两个传感器之间时，能计算出中间值
 * 3. 平滑输出：不会出现误差跳变，循迹更丝滑
 * 
 * 计算步骤：
 * 1. 读取4个传感器状态并取反（低电平=黑线→置1）
 * 2. 计算检测到黑线的传感器总数sum
 * 3. 若sum>0（在线）：加权平均计算误差
 *    若sum=0（脱轨）：进入脱轨找回逻辑
 * 4. 将计算结果存入Track_Error供PID控制器使用
 */
void Track_Update(void)
{
    uint8_t s1, s2, s3, s4, sum;
    float weightedSum;
    
    /* ======== 读取传感器状态 ======== */
    /* TCRT5000输出低电平表示检测到黑线，取反后1表示黑线 */
    s1 = !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14);   // S1: PC14
    s2 = !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9);    // S2: PA9
    s3 = !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10);   // S3: PA10
    s4 = !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15);   // S4: PC15
    
    /* 打包传感器状态到8位变量（方便OLED显示调试） */
    Track_SensorValues = (s1 << TRACK_S1_POS) | (s2 << TRACK_S2_POS) | 
                         (s3 << TRACK_S3_POS) | (s4 << TRACK_S4_POS);
    
    sum = s1 + s2 + s3 + s4;
    Track_SensorSum = sum;
    
    /* ======== 在线状态：加权质心法计算位置误差 ======== */
    if (sum > 0)
    {
        /* 
         * 加权质心法公式：
         *   Error = Σ(weight_i × value_i) / Σ(value_i)
         * 
         * 例1：只有S2(-4mm)检测到黑线
         *   Error = (0×(-30)+1×(-4)+0×4+0×30) / (0+1+0+0) = -4.0
         *   负值表示线偏左
         * 
         * 例2：S2(-4mm)和S3(4mm)同时检测到黑线
         *   Error = (0×(-30)+1×(-4)+1×4+0×30) / (0+1+1+0) = 0.0
         *   表示线在正中心
         * 
         * 例3：S1(-30mm)和S2(-4mm)检测到黑线
         *   Error = (1×(-30)+1×(-4)+0×4+0×30) / (1+1+0+0) = -17.0
         *   表示线在左偏17mm位置
         */
        weightedSum = (float)(s1 * TRACK_S1_WEIGHT + s2 * TRACK_S2_WEIGHT +
                              s3 * TRACK_S3_WEIGHT + s4 * TRACK_S4_WEIGHT);
        Track_Error = weightedSum / (float)sum;
        
        /* 更新上次误差（用于脱轨找回） */
        Track_LastError = Track_Error;
        
        /* 脱轨计数清零（已成功上轨） */
        Track_LostCount = 0;
        Track_SearchPhase = 0;                                      // 重置搜索阶段
    }
    /* ======== 脱轨状态：所有传感器均未检测到黑线 ======== */
    else
    {
        Track_LostCount++;
        
        if (Track_LostCount < 50)    // 前2.5秒(50次×50ms)：根据上次方向尝试找回
        {
            /* 
             * 【脱轨找回策略 - 方向放大法】
             * 将上次的有效误差方向放大，使小车朝缺线的方向转向。
             * 例如：上次误差为-10mm（偏左），脱轨后误差变为-15mm
             * （放大1.5倍），小车会向左转向，尝试重新找到黑线。
             * 放大系数随脱轨时间递增，加强纠偏力度。
             */
            float factor = 1.0f + Track_LostCount * 0.02f;      // 放大系数逐渐增大
            if (factor > 3.0f) factor = 3.0f;                   // 限制最大放大3倍
            Track_Error = Track_LastError * factor;
        }
        else if (Track_LostCount < TRACK_LOST_TIMEOUT)  // 2.5秒~10秒：交替搜索模式
        {
            /* 
             * 【脱轨找回策略 - 周期搜索法】
             * 交替产生左偏和右偏的误差，使小车左右摆动寻找黑线。
             * 搜索周期约2秒（40次×50ms），左右各占一半。
             * 搜索幅度逐渐增大，扩大搜索范围。
             */
            Track_SearchPhase++;
            if (Track_SearchPhase < 20)         // 前1秒(20×50ms)向左搜索
            {
                Track_Error = -40.0f;           // 强制左转
            }
            else if (Track_SearchPhase < 40)    // 后1秒(20×50ms)向右搜索
            {
                Track_Error = 40.0f;            // 强制右转
            }
            else
            {
                Track_SearchPhase = 0;          // 循环周期
            }
        }
        else    // 超过10秒仍未找回：停车等待
        {
            /* 超时仍未找回，误差归零（直行），等待重新上轨 */
            Track_Error = 0.0f;
        }
    }
}

/**
 * 函    数：获取原始传感器数据（用于调试显示）
 * 参    数：无
 * 返 回 值：传感器状态（bit3=S1, bit2=S2, bit1=S3, bit0=S4）
 */
uint8_t Track_GetSensorRaw(void)
{
    return Track_SensorValues;
}

/**
 * 函    数：获取当前位置误差
 * 参    数：无
 * 返 回 值：位置误差，范围约-30~+30（单位：mm偏移量）
 */
float Track_GetError(void)
{
    return Track_Error;
}

/**
 * 函    数：设置循迹速度
 * 参    数：Speed 目标速度（单位：转/秒）
 * 返 回 值：无
 * 说    明：正值前进，负值后退。建议范围0.5~3.0转/秒
 */
void Track_SetSpeed(float Speed)
{
    Track_Speed = Speed;
}

/**
 * 函    数：获取当前循迹速度
 * 参    数：无
 * 返 回 值：当前目标速度（单位：转/秒）
 */
float Track_GetSpeed(void)
{
    return Track_Speed;
}

/**
 * 函    数：检测是否在黑线上（在线状态）
 * 参    数：无
 * 返 回 值：1=在线（至少一个传感器检测到黑线），0=脱轨
 */
uint8_t Track_IsOnLine(void)
{
    return (Track_SensorSum > 0) ? 1 : 0;
}

/**
 * 函    数：检测是否脱轨（完全离线）
 * 参    数：无
 * 返 回 值：1=脱轨，0=在线
 */
uint8_t Track_IsLost(void)
{
    return (Track_LostCount > 0) ? 1 : 0;
}
