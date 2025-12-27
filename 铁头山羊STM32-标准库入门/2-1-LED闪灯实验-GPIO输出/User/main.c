#include "stm32f10x.h"
#include "Delay.h"
#include "LED.h"

int main (void){

	LED_Init();
	while(1){

		GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);
		Delay_ms(300);
		GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);
		Delay_ms(300);	
	}
}
