#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "LED.h"

int main (void)
{
	LED_Init();
	OLED_Init();
	OLED_ShowChinese(0,0,"ƒ„∫√£¨’≈Ê√Ê√£¨∞°");
	while(1)
	{
	}
}

