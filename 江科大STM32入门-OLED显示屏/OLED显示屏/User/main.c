#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"

int main (void)
{
	OLED_Init();
	OLED_ShowChar(0,0,'A',OLED_8X16);
	OLED_ShowChar(0,20,'B',OLED_6X8);
	OLED_ShowString(20,0,"Hello World!",OLED_8X16);
	
	OLED_ShowString(20,20,"Hello World!",OLED_6X8);
	OLED_Update();
	while(1)
	{
	}
}

