#include "stm32f10x.h"
#include "Delay.h"
#include "LED.h"
#include "Timer1.h"
#include <math.h>
int main (void){
	
	TIM1_Init();
	uint32_t time=100;
	while(1){
		
		time++;
		Delay_ms(1);
		TIM_SetCompare1(TIM1,(uint16_t)(0.5*(sin(2*3.14*time)+1)*1000));
		
//		
//		for(uint16_t i = 0; i < 1000 ;i++)
//		{
//			TIM_SetCompare1(TIM1,i);
//			Delay_ms(1);
//		}
//		for(uint16_t i = 0; i < 1000 ;i++)
//		{
//			TIM_SetCompare1(TIM1,1000-i);
//			Delay_ms(1);
//		}

	}
}


void TIM1_UP_IRQHandler(void)
{
	if(TIM_GetFlagStatus(TIM1, TIM_FLAG_Update) == SET)
	{
		TIM_ClearFlag(TIM1, TIM_FLAG_Update);
	}
}
