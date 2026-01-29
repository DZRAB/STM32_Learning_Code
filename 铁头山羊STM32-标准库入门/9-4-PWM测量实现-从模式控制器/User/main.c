#include "stm32f10x.h"
#include "Delay.h"
#include "USART1.h"
#include "Timer1.h"
#include "Timer3.h"

int main (void)
{
	
	USART1_DefaultInit();
	TIM3_Init();
	TIM1_Init();
	TIM_SetCompare1(TIM3,300);
	while(1)
	{
		TIM_ClearFlag(TIM1,TIM_FLAG_Trigger);
		while(TIM_GetFlagStatus(TIM1, TIM_FLAG_Trigger) != SET);
			
		uint16_t cc1 = TIM_GetCapture1(TIM1);
		uint16_t cc2 = TIM_GetCapture2(TIM1);
		
		float period = cc1 * 1.0e-6f * 1.0e3f;
		float duty = (float)cc2 / cc1 * 100.0;		
		
		printf("CCR1:%d CCR2:%d period:%.3fms duty:%.2f%%\r\n",cc1,cc2,period,duty);
		Delay_s(1);
		
	}
	
}


//void TIM1_UP_IRQHandler(void)
//{
//	if(TIM_GetFlagStatus(TIM1, TIM_FLAG_Update) == SET)
//	{
//		TIM_ClearFlag(TIM1, TIM_FLAG_Update);
//	}
//}
