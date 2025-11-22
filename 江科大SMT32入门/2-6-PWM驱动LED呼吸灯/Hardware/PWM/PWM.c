#include "PWM.h"


void PWM_Init(void)
{
	//配置RCC-APB1
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	//配置RCC-GPIOA
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	//配置RCC-AFIO重映射
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	//配置AFIO重映射
	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2,ENABLE);
	//解除JTAG映射
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	
	//配置GPIOA
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//PA0重映射为PA15
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//选择时钟源-内部时钟
	TIM_InternalClockConfig(TIM2);
	
	//配置时基单元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 100-1; //ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720-1; //PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	//配置输出比较单元
	/*频率1KHZ 占空比50% 分辨率1%
	● PWM频率：	Freq = CK_PSC / (PSC + 1) / (ARR + 1) 
					 = 72M / (719 + 1) / (99 + 1) 
	● PWM占空比：	Duty = CCR / (ARR + 1)  
						= 50 / (99 + 1)
	● PWM分辨率：	Reso = 1 / (ARR + 1) 
						= 1 / (99 + 1)
	*/
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 50; //CCR 
	TIM_OC1Init(TIM2,&TIM_OCInitStructure);
	
	//运行控制
	TIM_Cmd(TIM2, ENABLE);
}

void PWM_SetCompare1(uint16_t Compare)
{
	TIM_SetCompare1(TIM2,Compare);
}

