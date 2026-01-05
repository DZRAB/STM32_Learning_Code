#include "stm32f10x.h"
#include "Delay.h"
#include "LED.h"
#include "TIMDelay.h"
int main (void){
	
	LED_Init();
	TIM1_Init();
	
	while(1){

		GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);
		TIM1_Delay_ms(50);
		GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);
		TIM1_Delay_ms(300);
		GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);
		TIM1_Delay_ms(50);
		GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);
		TIM1_Delay_ms(300);
		GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);
		TIM1_Delay_ms(300);
		GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);
		TIM1_Delay_ms(300);
		
	}
}


void TIM1_UP_IRQHandler(void)
{
	if(TIM_GetFlagStatus(TIM1, TIM_FLAG_Update) == SET)
	{
		TIM1_tick();
		TIM_ClearFlag(TIM1, TIM_FLAG_Update);
	}
}
