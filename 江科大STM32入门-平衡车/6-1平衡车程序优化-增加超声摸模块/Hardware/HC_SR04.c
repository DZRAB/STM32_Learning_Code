#include "stm32f10x.h"                  // Device header
#include "HC_SR04.h"
#include "Delay.h"

/* ============= 全局变量定义 ============= */
// 注意：这些变量需要在多处访问，所以用 volatile 防止编译器优化
volatile uint32_t sr04_overflow_cnt = 0;   // 在 TIM1 更新中断（1ms）里累加，记录计数器溢出的次数
volatile uint32_t sr04_width_us = 0;       // 存储测得的 ECHO 高电平脉宽，单位微秒（定时器时钟 1us/计数）
volatile uint8_t  sr04_new_data = 0;       // 标志位，1 表示一次完整的测距已完成，主循环可以读取新数据
uint16_t Num;

/* ============= SR04 硬件初始化函数 ============= */
void SR04_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* 1. 使能 GPIOA 时钟（PA11、PA12 都属于 GPIOA） */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* 2. 配置 PA11 为输入模式（ECHO 信号输入） */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   // 上拉输入，因为 ECHO 空闲时为低电平，上拉可避免浮空
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 3. 配置 PA12 为推挽输出（TRIG 触发信号输出） */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出，驱动能力强
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA, GPIO_Pin_12);              // 初始拉低 TRIG 引脚

    /* 4. 配置 TIM1 的通道4 为输入捕获模式（用于测量 ECHO 高电平宽度） */
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;         // 使用 TIM1 的通道4
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; // 先捕获上升沿（ECHO 变高）
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; // 直连到 CH4 引脚
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;    // 不分频，每个定时器时钟都捕获
    TIM_ICInitStructure.TIM_ICFilter = 0X5;                    // 不滤波（若信号有毛刺可设 0x3~0x5）
    TIM_ICInit(TIM1, &TIM_ICInitStructure);                  // 将配置写入硬件寄存器

    /* 5. 使能 TIM1 的捕获中断（当 CH4 发生捕获事件时触发中断） */
    TIM_ITConfig(TIM1, TIM_IT_CC4, ENABLE);

    /* 6. 配置捕获中断的优先级（必须高于 TIM1 的更新中断，确保及时响应） */
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;       // 捕获/比较中断向量
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 高抢占优先级（数值越小优先级越高）
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       // 子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/* ============= 触发一次 SR04 测距 ============= */
void SR04_Trigger(void)
{
    // 产生一个大于 10us 的高电平脉冲，启动超声波发射
    GPIO_SetBits(GPIOA, GPIO_Pin_12);   // TRIG 拉高
    Delay_us(15);                       // 保持 10 微秒（需要你自己实现延时函数）
    GPIO_ResetBits(GPIOA, GPIO_Pin_12); // TRIG 拉低，超声波模块开始发射 8 个 40kHz 脉冲
    sr04_new_data = 0;                  // 清除“新数据”标志，等待本次测量完成
}


uint16_t Get_Num(void)
{
	 return Num;
}
