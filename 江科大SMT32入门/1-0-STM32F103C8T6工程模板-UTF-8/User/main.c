#include "stm32f10x.h"
#include "Delay.h"
#include "LED.h"

int main (void){
	LED_Init();
	while(1){
		LED1_ON();
		Delay_ms(300);
		LED1_OFF();
		Delay_ms(300);
	}
}
