#include "InputCapture.h"


void InputCapture_Init(void)
{
	//配置RCC-APB1-TIM3输入捕获
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	//配置RCC-GPIOA
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	//配置GPIOA
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//选择时钟源-内部时钟
	TIM_InternalClockConfig(TIM2);
	
	//配置时基单元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 65536-1; //ARR - 最大计数值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72-1; //PSC - 1Mhz
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
	//初始化输入捕获单元
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICFilter = 0xF;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	
	//配置TRGI的触发源为TI1FP1
	TIM_SelectInputTrigger(TIM3,TIM_TS_TI1FP1);
	//配置从模式为RESET
	TIM_SelectSlaveMode(TIM3,TIM_SlaveMode_Reset);
	
	//运行控制
	TIM_Cmd(TIM3, ENABLE);
}

uint32_t InputCapture_GetFreq(void)
{
	return 1000000 / (TIM_GetCapture1(TIM3) + 1);
}
