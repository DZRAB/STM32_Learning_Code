/**
 * @file    main.c
 * @brief   STM32F103C8T6 - LSE 32.768KHz 晶振频率测试仪
 *
 * 测试原理:
 *   HSE 8MHz(已知准) -> PLL x9 -> 72MHz系统时钟 -> TIM2(1MHz计数器)
 *   LSE 32.768KHz     -> RTC预分频/32768 -> 1Hz秒中断
 *
 *   每次RTC秒中断时读取TIM2计数值, 计算两次中断间的tick差值:
 *   f_LSE = 32768000000 / tick_diff_us   (精度: ±1ppm)
 *
 * 硬件: STM32F103C8T6 + OLED(SSD1306, I2C-PB8/PB9) + LED(PC13)
 */

#include "stm32f10x.h"
#include "OLED.h"

/* ====== 常量定义 ====== */
#define LSE_FREQ          32768UL      /* LSE标称频率 */
#define TIM2_PSC          71           /* 72MHz/(PSC+1)=1MHz, 即1us/tick */
#define TIM2_OVF_PERIOD   65536        /* 16位计数器溢出周期(tick数) */
#define TICKS_PER_SEC     1000000UL    /* 理想1秒对应的TIM2 tick数(1MHz下) */

/* ====== 全局变量 (测量结果) ====== */
volatile uint32_t g_ovf_cnt;           /* TIM2溢出次数累计 */
volatile uint8_t  g_first_irq = 1;     /* 首次RTC中断标记 */
volatile uint8_t  g_new_data = 0;      /* 新测量数据就绪标志 */

uint32_t g_snap_last = 0;              /* 上次快照: [高16位=ovf, 低16位=CNT] */
uint32_t g_tim_diff;                   /* 两次RTC中断间的总tick差值(us) */
uint32_t g_freq;                       /* LSE实测频率(Hz) */
int32_t  g_ppm;                        /* 频率误差(ppm) */
uint16_t g_cnt;                        /* 已完成测量次数 */
uint8_t  g_lse_ok;

/* ======================== 基础驱动 ======================== */

/** @brief 空循环延时 (@72MHz, 约54000循环/ms) */
static void Delay_ms(uint32_t ms)
{
    volatile uint32_t n;
    for (; ms > 0; ms--)
        for (n = 0; n < 54000; n++);
}

/** @brief LED初始化 (PC13推挽输出) */
static void LED_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitTypeDef g;
    g.GPIO_Pin   = GPIO_Pin_13;
    g.GPIO_Mode  = GPIO_Mode_Out_PP;
    g.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &g);
    GPIO_SetBits(GPIOC, GPIO_Pin_13);  /* LED灭(低电平亮) */
}

/** @brief 错误停机: 显示信息 + LED快闪 */
static void Error_Halt(const char *msg)
{
    OLED_ShowString(1, 1, (char *)msg);
    while (1) { Delay_ms(200); GPIOC->ODR ^= GPIO_Pin_13; }
}

/* ======================== 外设初始化 ======================== */

/**
 * @brief  系统时钟配置: HSE 8MHz -> PLL x9 -> SYSCLK 72MHz
 *         AHB=72M, APB1=36M(TIMx2=72M), APB2=72M
 * @return 1=成功, 0=HSE未起振
 */
static uint8_t Clk_Init(void)
{
    RCC_HSEConfig(RCC_HSE_ON);
    volatile uint32_t t = 500000;
    while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET && --t);
    if (!t) return 0;

    FLASH_SetLatency(FLASH_Latency_2);
    FLASH_PrefetchBufferCmd(ENABLE);

    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    RCC_PLLCmd(ENABLE);
    while (!(RCC->CR & RCC_CR_PLLRDY));

    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while ((RCC->CFGR & RCC_CFGR_SWS) != 0x08);

    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    RCC_PCLK1Config(RCC_HCLK_Div2);       /* APB1=36M -> TIM时钟x2=72M */
    RCC_PCLK2Config(RCC_SYSCLK_Div1);

    SystemCoreClockUpdate();
    return 1;
}

/**
 * @brief  TIM2配置为1MHz自由计数器 + 溢出中断
 *         PSC=71 => 72M/(71+1)=1MHz, ARR=0xFFFF, 每65.5ms溢出一次
 *         溢出IRQ记录次数, 配合RTC中断实现32位等效计数
 */
static void Tim2_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseInitTypeDef tim = {
        .TIM_Period            = 0xFFFF,
        .TIM_Prescaler         = TIM2_PSC,
        .TIM_ClockDivision     = 0,
        .TIM_CounterMode       = TIM_CounterMode_Up,
        .TIM_RepetitionCounter = 0
    };
    TIM_TimeBaseInit(TIM2, &tim);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    NVIC_InitTypeDef nvic = {
        .NVIC_IRQChannel                   = TIM2_IRQn,
        .NVIC_IRQChannelPreemptionPriority = 0,  /* 最高优先级, 防丢失溢出 */
        .NVIC_IRQChannelSubPriority        = 0,
        .NVIC_IRQChannelCmd                = ENABLE
    };
    NVIC_Init(&nvic);
    TIM_Cmd(TIM2, ENABLE);
}

/**
 * @brief  RTC配置: 使用LSE作为时钟源, 预分频32768->1Hz秒中断
 *         BKP_DR1用作首次配置标志, 避免重复初始化
 * @return 1=LSE起振成功, 0=LSE未起振
 */
static uint8_t Rtc_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);

    if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
    {
        RCC_LSEConfig(RCC_LSE_ON);
        volatile uint32_t to = 45000000;
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET && --to);
        if (!to) return 0;

        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
        RCC_RTCCLKCmd(ENABLE);
        RTC_WaitForSynchro();
        RTC_SetPrescaler(LSE_FREQ - 1);     /* 32768分频 -> 1Hz */
        RTC_WaitForLastTask();
        RTC_ITConfig(RTC_IT_SEC, ENABLE);
        RTC_WaitForLastTask();
        RTC_SetCounter(0);
        BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
    }
    else
    {
        RCC_RTCCLKCmd(ENABLE);
        RTC_WaitForSynchro();
        RTC_ITConfig(RTC_IT_SEC, ENABLE);
    }
    return 1;
}

/* ======================== 中断服务 ======================== */

/** @brief  TIM2更新中断: 每65.5ms触发, 累加溢出计数 */
void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_FLAG_Update)
    {
        g_ovf_cnt++;
        TIM2->SR = ~TIM_FLAG_Update;      /* 清除溢出标志 */
    }
}

/**
 * @brief  RTC秒中断回调 (核心测量逻辑)
 *
 *         每次RTC秒中断时读取"当前时间戳":
 *           时间戳 = [溢出次数(高16bit) | TIM2_CNT(低16bit)]
 *         与上次时间戳相减得到精确的us级间隔, 进而计算LSE频率.
 */
void RTC_IRQHandler_Callback(void)
{
    if (RTC_GetITStatus(RTC_IT_SEC))       /* if (RTC->CRL & RTC_CRL_SECF) */
    {
        __disable_irq();                   /* 关中断, 保证读数原子性 */

        uint16_t cnt_now  = (uint16_t)(TIM2->CNT);
        uint32_t snap_now = (g_ovf_cnt << 16) | cnt_now;

        if (!g_first_irq && snap_now != g_snap_last)
        {
            /* 计算两次中断间的总tick差 */
            g_tim_diff = snap_now - g_snap_last;  /* 无符号减自动处理溢出 */

            /* 频率计算: f_LSE = 32768Hz × (1000000us / 实测us) */
            g_freq = (uint32_t)(32768000000ULL / g_tim_diff);

            /* ppm误差: (实测 - 标称) × 10^6 / 标称 */
            int64_t err = (int64_t)g_freq - (int64_t)LSE_FREQ;
            g_ppm = (int32_t)(err * 1000LL / (LSE_FREQ / 1000));

            g_cnt++;
            g_new_data = 1;
        }

        g_snap_last = snap_now;
        __enable_irq();

        g_first_irq = 0;
        RTC_ClearITPendingBit(RTC_IT_SEC); /* RTC->CRL &= ~RTC_CRL_SECF */
    }
}

/* ======================== OLED显示 ======================== */

/** @brief  显示测量结果 (首次画框架, 后续只刷新数字) */
static void Display_Result(void)
{
    static uint8_t inited = 0;

    if (!inited)
    {
        OLED_Clear();
        OLED_ShowString(1, 1, "LSE Crystal Test");   /* 行1: 标题 */
        OLED_ShowString(2, 1, "F:");
        OLED_ShowChar(2, 5, '.');
        OLED_ShowString(2, 10, "kHz");               /* 行2: F:xx.xxx kHz */
        OLED_ShowString(3, 4, "Err:");
        OLED_ShowString(3, 14, "ppm");               /* 行3: Err:±xxxx ppm */
        OLED_ShowChar(4, 1, 'T');
        OLED_ShowChar(4, 2, ':');
        OLED_ShowString(4, 11, "#");                  /* 行4: T:ddddddd #NN */
        inited = 1;
    }

    /* 刷新变化的数字 (覆盖写, 不影响其他位置) */

    /* 频率: 整数部分(2位).小数部分(3位) */
    OLED_ShowNum(2, 3, g_freq / 1000, 2);
    OLED_ShowNum(2, 6, g_freq % 1000, 3);

    /* ppm误差: 有符号5位数 */
    if (g_ppm >= 0)
        OLED_ShowSignedNum(3, 8, g_ppm, 5);
    else
    {
        OLED_ShowChar(3, 8, '-');
        OLED_ShowNum(3, 9, (uint32_t)(-g_ppm), 4);
    }

    /* TIM2 tick差值 + 测量序号 */
    OLED_ShowNum(4, 3, g_tim_diff, 7);
    OLED_ShowNum(4, 12, g_cnt, 2);
}

/* ======================== 主程序 ======================== */

int main(void)
{
    /* ---- 硬件初始化 ---- */
    LED_Init();                            /* PC13 LED */
    OLED_Init();                           /* SSD1306 I2C(PB8/PB9) */
    OLED_Clear();

    /* ---- 系统时钟: HSE 8MHz --> PLL 72MHz ---- */
    if (!Clk_Init())
        Error_Halt("ERROR:HSE fail!");

    /* ---- 高精度计时: TIM2 1MHz + RTC LSE ---- */
    Tim2_Init();

    NVIC_InitTypeDef nvic_rtc = {
        .NVIC_IRQChannel                   = RTC_IRQn,
        .NVIC_IRQChannelPreemptionPriority = 1,
        .NVIC_IRQChannelSubPriority        = 0,
        .NVIC_IRQChannelCmd                = ENABLE
    };
    NVIC_Init(&nvic_rtc);

    if (!Rtc_Init())
        Error_Halt("ERROR:LSE fail!");

    /* ---- 进入测量模式 ---- */
    OLED_Clear();
    OLED_ShowString(1, 1, "LSE Crystal Test");
    OLED_ShowString(3, 4, "Measuring...");

    for (;;)
    {
        if (g_new_data)
        {
            g_new_data = 0;
            Display_Result();               /* ~2秒后首次出结果, 之后每秒更新 */
        }

        Delay_ms(100);
        GPIOC->ODR ^= GPIO_Pin_13;         /* LED闪烁指示运行中 */
    }
}
