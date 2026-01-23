#include "TIMDelay.h"

volatile static uint32_t CurrentTick = 0; 

void TIM1_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 1000-1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72-1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);
	
	TIM_Cmd(TIM1,ENABLE);
	
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef  NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);
}

uint32_t TIM1_GetCurrentTick(void)
{
	return CurrentTick;
}

void TIM1_tick(void)
{
	CurrentTick++;
}	

void TIM1_Delay_ms(uint32_t ms)
{
		uint32_t GetDelay_ms = TIM1_GetCurrentTick() + ms;
		while(TIM1_GetCurrentTick() < GetDelay_ms);

}

