#include "stm32f10x.h"
#include "Track.h"

/**************************************************************************************
 * 四路巡线寻迹模块 (TCRT5000)
 * 
 * 【功能说明】
 * 本模块实现基于TCRT5000四路红外反射传感器的循迹功能。
 * TCRT5000的工作原理：红外发射管发射红外光，红外接收管接收反射回来的光。
 * - 黑色表面吸收红外光 → 接收管截止 → 输出高电平(1)（经LM393比较器整形后）
 * - 白色表面反射红外光 → 接收管导通 → 输出低电平(0)（经LM393比较器整形后）
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
 * 其中传感器检测值：1=检测到黑线(高电平), 0=未检测到(低电平)
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
static uint8_t Track_FirstRun = 1;          // 首次运行标志（1=首次，用于初始无线时直接进入搜索）

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
 * 函    数：复位循迹内部状态
 * 参    数：无
 * 返 回 值：无
 * 说    明：每次K2开启循迹模式时调用，清除之前的脱轨计数、搜索阶段、
 *           首次运行标志等内部状态，确保重新开始循迹时状态干净。
 */
void Track_ResetState(void)
{
    Track_Error = 0.0f;            // 误差清零
    Track_LastError = 0.0f;        // 上次误差清零（强迫首次进入搜索模式）
    Track_LostCount = 0;           // 脱轨计数清零
    Track_SearchPhase = 0;         // 搜索阶段清零
    Track_FirstRun = 1;            // 设为首次运行（无线历史时直接进入搜索）
}

/**
 * 函    数：读取传感器状态并计算循迹误差
 * 参    数：无
 * 返 回 值：无
 * 说    明：此函数需在定时中断中周期性调用（推荐50ms周期）
 * 
 * 【算法说明 - 平衡车专用循迹策略】
 * 
 * 针对平衡车的特点（惯性大、精度差、PID调整易振荡）设计了如下策略，
 * 由用户根据TCRT5000四路传感器和18mm黑线的实际情况提出：
 * 
 * ┌──────────────────────────────────────────────────────────┐
 * │             传感器布局（间距）                              │
 * │   S1(PC14) ──26mm── S2(PA9) ──8mm── S3(PA10) ──26mm── S4(PC15) │
 * │              ←18mm黑线→（小于S1-S2或S3-S4间距）               │
 * └──────────────────────────────────────────────────────────┘
 * 
 * 【状态机】
 *   状态1 - 中心在线（S2或S3检测到黑线）：
 *     说明偏差在±4mm范围内，误差≈0，直接走直，不产生任何转向调整。
 *     平衡车稍微调整就会摆动很大，小偏差不调整反而更稳。
 *     同时记录漂移方向（S2→偏右，S3→偏左），供脱轨后使用。
 *   
 *   状态2 - 外圈压线（S1或S4检测到黑线）：
 *     偏差超过26mm，必须转向纠正。
 *     S1压线 → 线在左前方 → 强制向左转找回
 *     S4压线 → 线在右前方 → 强制向右转找回
 *   
 *   状态3 - 滑行过渡（无线，但丢线<500ms）：
 *     18mm黑线宽度介于8mm(S2-S3)和26mm(S1-S2/S3-S4)之间，
 *     中心传感器刚脱离黑线但外圈尚未压到时，线其实还在传感器覆盖范围内。
 *     此时不调整、继续直行，让车的惯性自然过渡到外圈传感器压线或回到中心。
 *   
 *   状态4 - 主动搜索（丢线>500ms且<10s）：
 *     真正脱离了传感器覆盖范围。先沿最后漂移方向放大搜索，然后左右周期摆动。
 *   
 *   状态5 - 超时停车（丢线>10s）：
 *     误差归零直行，等待用户干预。
 */
void Track_Update(void)
{
    uint8_t s1, s2, s3, s4;
    
    /* ======== 读取传感器状态 ======== */
    /* TCRT5000模块经LM393比较器输出：高电平=黑线，低电平=白面 */
    s1 = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14);   // S1: PC14
    s2 = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9);    // S2: PA9
    s3 = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10);   // S3: PA10
    s4 = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15);   // S4: PC15
    
    /* 打包传感器状态到8位变量（方便OLED显示调试） */
    Track_SensorValues = (s1 << TRACK_S1_POS) | (s2 << TRACK_S2_POS) | 
                         (s3 << TRACK_S3_POS) | (s4 << TRACK_S4_POS);
    Track_SensorSum = s1 + s2 + s3 + s4;
    
    /* ================================================================
     * 状态1：中心传感器（S2/S3）在线 → 误差为0，直接走直
     * ================================================================ */
    if (s2 || s3)
    {
        Track_Error = 0.0f;          // 不调整方向，直行
        Track_LostCount = 0;         // 脱轨计数清零
        Track_SearchPhase = 0;
        Track_FirstRun = 0;          // 已上轨
        
        /* 记录漂移方向：仅S2在线→偏右漂移，仅S3在线→偏左漂移 */
        if (s2 && !s3)
            Track_LastError = -1.0f;  // 偏向右侧，脱轨时向左搜
        else if (s3 && !s2)
            Track_LastError = 1.0f;   // 偏向左侧，脱轨时向右搜
        else
            Track_LastError = 0.0f;   // 正中心
    }
    /* ================================================================
     * 状态2：外圈传感器（S1/S4）压线 → 偏差大了，强制转向纠正
     * ================================================================ */
    else if (s1 || s4)
    {
        if (s1)
            Track_Error = -30.0f;     // 线在左前方 → 向左转向找回
        if (s4)
            Track_Error = 30.0f;      // 线在右前方 → 向右转向找回
        /* S1和S4同时压线（过路口等特殊情况）→ 直行 */
        if (s1 && s4)
            Track_Error = 0.0f;
        
        Track_LastError = Track_Error;
        Track_LostCount = 0;
        Track_SearchPhase = 0;
        Track_FirstRun = 0;
    }
    /* ================================================================
     * 状态3/4/5：所有传感器均未检测到黑线
     * ================================================================ */
    else
    {
        Track_LostCount++;
        
        /***** 状态3：滑行过渡（<500ms，共10次×50ms） *****/
        /* 
         * 刚脱离中心传感器但外圈尚未压线，说明线还在传感器范围内，
         * 不调整方向，让车直行自然过渡。若外圈压上则进入状态2，
         * 若回到中心则回到状态1。
         */
        if (Track_LostCount <= 10)
        {
            Track_Error = 0.0f;       // 不调整，继续直行
        }
        /***** 状态4：主动搜索（500ms~10s） *****/
        /*
         * 第一阶段（500ms~1s）：方向放大搜索
         *   沿最后记录的漂移方向搜索找回
         */
        else if (Track_LostCount <= 30)
        {
            if (Track_FirstRun || Track_LastError == 0.0f)
            {
                /* 无历史方向 → 先左后右扫一遍 */
                if (Track_LostCount <= 20)
                    Track_Error = -20.0f;   // 向左搜
                else
                    Track_Error = 20.0f;    // 向右搜
            }
            else
            {
                /* 有历史方向 → 沿该方向放大 */
                float factor = 1.0f + (Track_LostCount - 10) * 0.1f;
                if (factor > 3.0f) factor = 3.0f;
                Track_Error = Track_LastError * factor;
            }
        }
        /*
         * 第二阶段（>1s~10s）：周期左右摆动搜索
         */
        else if (Track_LostCount < TRACK_LOST_TIMEOUT)
        {
            Track_SearchPhase++;
            if (Track_SearchPhase < 20)       // 向左搜1秒
                Track_Error = -40.0f;
            else if (Track_SearchPhase < 40)  // 向右搜1秒
                Track_Error = 40.0f;
            else
                Track_SearchPhase = 0;
        }
        /***** 状态5：超时（>10s） → 直行等待 *****/
        else
        {
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
