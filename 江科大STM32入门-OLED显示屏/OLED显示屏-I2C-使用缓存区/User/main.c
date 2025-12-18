#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "LED.h"
#include <math.h>

int main (void)
{
	LED_Init();
	OLED_Init();
	
	while(1)
	{
//		for(uint8_t x=0;x<128;x++)
//		{
//			OLED_DrawPoint(x,sin(x/128.0 * 2 * 3.14)*32+32);
//			OLED_Updata();
//		}
//		for(uint8_t x=0;x<128;x++)
//		{
//			OLED_DrawPoint(x,64/2);
//			OLED_Updata();
//		}
//		for(uint8_t x=0;x<64;x++)
//		{
//			OLED_DrawPoint(128/2,x);
//			OLED_Updata();
//		}
//		
//		if(OLED_GetPoint(0,32))
//		{
//			OLED_ShowString(0,0,"YES",8);
//		}
//		else
//		{
//			OLED_ShowString(0,0,"NO ",8);
//		}

		//OLED_Clear();
		for(uint16_t i=0;i<360;i+=20)
		{
			OLED_DrawLine(64,32,64+30*cos(i/360.*2*3.14),32+30*sin(i/360.*2*3.14));
			OLED_Updata();
		}
		
	}
}

