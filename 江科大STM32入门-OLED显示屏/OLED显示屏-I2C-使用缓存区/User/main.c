#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "LED.h"

int main (void)
{
	LED_Init();
	OLED_Init();
//	
//	OLED_ShowChar(7,7,'A',6);
//	OLED_ShowString(10,0,"CD",8);
//	OLED_ShowNum(40, 0,123, 3, 8);
//	OLED_ShowSignedNum(70, 0,-456, 3, 8);
//	OLED_ShowSignedNum(0, 2, 789, 3, 8);
//	OLED_ShowHexNum(40, 2, 0xAA, 2, 8);
//	OLED_ShowBinNum(60, 2, 0xAA, 8, 8);

//	OLED_ShowFloatNum(0, 4, 3.1415, 1, 4, 8);
//	OLED_ShowChinese(60, 4, "你好世界");
//	OLED_Updata();
	
	OLED_ShowChar(7,7,'A',8);
	OLED_ShowChar(7,7,'B',8);
	OLED_Updata();
	
	
	while(1)
	{
//		for(uint8_t i=0;i<63;i++)
//		{
//			OLED_Clear();
//			OLED_ShowImage(i*2,i,15,15,Img);
//			OLED_ShowString(i*2+16,i,"Hello",8);
//			OLED_ShowChinese(i*2,i+16, "你好世界");
//			OLED_Updata();
//			Delay_ms(10);
//		}
	}
}

