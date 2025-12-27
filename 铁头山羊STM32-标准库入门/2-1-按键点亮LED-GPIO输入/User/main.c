#include "stm32f10x.h"
#include "Delay.h"
#include "LED.h"
#include "Key.h"

int main (void){

	LED_Init();
	Key_Init();
	while(1){

		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) == 0)
		{
			Delay_ms(20);
			GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);
		}
		else
		{
			GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);
			Delay_ms(20);	
		}

	}
}
