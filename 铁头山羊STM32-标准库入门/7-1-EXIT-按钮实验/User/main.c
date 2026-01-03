#include "stm32f10x.h"
#include "Delay.h"
#include "LED.h"
#include "Key.h"

int main (void){

	LED_Init();
	Key_Init();
	while(1)
	{


	}
}


void EXTI1_IRQHandler(void)
{
		if(EXTI_GetFlagStatus(EXTI_Line1) == SET)
		{
			GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);
		}
		EXTI_ClearFlag(EXTI_Line1);
}

void EXTI15_10_IRQHandler(void)
{
		if(EXTI_GetFlagStatus(EXTI_Line11) == SET)
		{
			GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);
		}
		EXTI_ClearFlag(EXTI_Line11);	
}	

