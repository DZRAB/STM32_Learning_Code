#include "stm32f10x.h"
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "Key.h"
#include "BlueSerial.h"
#include "Timer.h"
#include <string.h>

int main (void){
	
	LED_Init();
	OLED_Init();
	Key_Init();
	BlueSerial_Init();	
	Timer_Init();
	
	OLED_Printf(0,0,OLED_8X16,"%s","                ");
	OLED_Printf(0,0,OLED_8X16,"%s","JDY-29 Config");
	OLED_Update();
	
	uint8_t KeyNum;
	uint8_t PrevFlag=0;
	uint8_t CurrFlag = 0;
	uint8_t AutoConfigFlag = 0;

	while(1)
	{
		BlueSerial_Printf("AT\r\n");
		Delay_ms(100);
		if(BlueSerial_RxFlag == 1) //判断是否有蓝牙模块回应
		{
				PrevFlag = CurrFlag; 
				CurrFlag = 1;
				BlueSerial_RxFlag = 0;
		}
		else
		{
			CurrFlag = 0;
		}
		
		if(PrevFlag == 0 && CurrFlag == 1) //判断是否是第一次插入模块
		{
			AutoConfigFlag = 1;
		}
		else if(PrevFlag == 0 && CurrFlag == 0) //没有模块
		{
			
		}
		else 
		{
			AutoConfigFlag = 0;
		}
		
		KeyNum = Key_GetNum();
		if(KeyNum == 1 || AutoConfigFlag == 1) //按键触发或者自动触发
		{
			OLED_Clear();
			OLED_Printf(0,0,OLED_8X16,"%s","                ");
			OLED_Printf(0,0,OLED_8X16,"%s","JDY-29 Config");
			
			BlueSerial_Printf("AT+DEFAULT\r\n");
			OLED_Printf(0,32,OLED_8X16,"TX:%s","                ");
			OLED_Printf(0,32,OLED_8X16,"TX:%s","AT+DEFAULT");
			OLED_Update();
			Delay_ms(100);
			if(BlueSerial_RxFlag == 1)
			{
				OLED_Printf(0,48,OLED_8X16,"RX:%s",BlueSerial_RxPacket);
				BlueSerial_RxFlag = 0;
				OLED_Update();
			}	
			else
			{
				OLED_Printf(0,48,OLED_8X16,"RX:ERROR");
				BlueSerial_RxFlag = 0;
				OLED_Update();
			}	
			
			BlueSerial_Printf("AT+NAMEKDPHC\r\n");
			OLED_Printf(0,32,OLED_8X16,"TX:%s","                ");
			OLED_Printf(0,32,OLED_8X16,"TX:%s","AT+NAMEKDPHC");
			OLED_Update();
			Delay_ms(100);
			if(BlueSerial_RxFlag == 1)
			{
				OLED_Printf(0,48,OLED_8X16,"RX:%s",BlueSerial_RxPacket);
				BlueSerial_RxFlag = 0;
				OLED_Update();
			}		
			else
			{
				OLED_Printf(0,48,OLED_8X16,"RX:ERROR");
				BlueSerial_RxFlag = 0;
				OLED_Update();
			}	
			
			OLED_Clear();
			BlueSerial_Printf("AT+NAME\r\n");
			OLED_Printf(0,0,OLED_8X16,"TX:%s","                ");
			OLED_Printf(0,0,OLED_8X16,"TX:%s","AT+NAME");
			OLED_Update();
			Delay_ms(100);			
			if(BlueSerial_RxFlag == 1)
			{
				OLED_Printf(0,16,OLED_8X16,"RX:%s",BlueSerial_RxPacket);
				BlueSerial_RxFlag = 0;
				OLED_Update();
			}	
			else
			{
				OLED_Printf(0,16,OLED_8X16,"RX:ERROR");
				BlueSerial_RxFlag = 0;
				OLED_Update();
			}	
			
			BlueSerial_Printf("AT+BAUD\r\n");
			OLED_Printf(0,32,OLED_8X16,"TX:%s","                ");
			OLED_Printf(0,32,OLED_8X16,"TX:%s","AT+BAUD");
			OLED_Update();
			Delay_ms(100);			
			if(BlueSerial_RxFlag == 1)
			{
				OLED_Printf(0,48,OLED_8X16,"RX:%s",BlueSerial_RxPacket);
				BlueSerial_RxFlag = 0;
				OLED_Update();
			}			
			else
			{
				OLED_Printf(0,48,OLED_8X16,"RX:ERROR");
				BlueSerial_RxFlag = 0;
				OLED_Update();
			}	
			
		}

	}
}

void TIM1_UP_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM1,TIM_IT_Update) == SET)
	{
		Key_Tick();
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}
}
