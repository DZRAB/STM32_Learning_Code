#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "LED.h"
#include "Key.h"
#include "Timer.h"

uint8_t KeyNum;
uint8_t Flag;
uint16_t i;

int main (void){

	OLED_Init();
	LED_Init();
	Key_Init();
	Timer_Init();
	while(1){
		KeyNum = Key_GetNum();
		
		if(KeyNum == 1)
		{
			Flag = !Flag;
		}
		
		if(Flag)
		{	
			LED1_ON();
			Delay_ms(500);
			LED1_OFF();
			Delay_ms(500);
		}
		else
		{
			LED1_OFF();
		}
		OLED_ShowNum(1,1,i,5);	
	}
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) == SET)
	{
		i++;
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
}

