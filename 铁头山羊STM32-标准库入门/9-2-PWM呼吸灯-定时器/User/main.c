#include "stm32f10x.h"
#include "Delay.h"
#include "LED.h"
#include "Timer1.h"
int main (void){
	
	//LED_Init();
	TIM1_Init();
	
	while(1){
		
//		for(uint8_t i = 0; i < 100 ;i++)
//		{
//			TIM_SetCompare1(TIM1,i);
//			Delay_ms(10);
//		}
//		for(uint8_t i = 0; i < 100 ;i++)
//		{
//			TIM_SetCompare1(TIM1,100-i);
//			Delay_ms(10);
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
