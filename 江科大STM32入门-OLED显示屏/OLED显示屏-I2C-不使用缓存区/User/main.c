#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "LED.h"

int main (void)
{
	LED_Init();
	OLED_Init();
	OLED_ShowFloatNum(0,0,123.456,3,3,8);
	OLED_ShowBinNum(0,2,0xAA,8,8);
	OLED_ShowBinNum(0,4,0xAA,8,6);
	OLED_ShowBinNum(0,5,0xAA,8,6);
	OLED_ShowNum(0,6,456,4,8);
	while(1)
	{
	}
}

