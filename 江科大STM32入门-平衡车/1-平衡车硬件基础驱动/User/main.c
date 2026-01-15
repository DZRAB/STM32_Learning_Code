#include "stm32f10x.h"
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "Timer.h"
#include "Key.h"
#include "MPU6050.h"

int16_t AX,AY,AZ,GX,GY,GZ;

int main (void){
	LED_Init();
	OLED_Init();
	Timer_Init();
	Key_Init();
	MPU6050_Init();
	
	
	while(1)
	{
		OLED_Printf(0,0,OLED_8X16,"A:%05d",AX);
		OLED_Printf(0,16,OLED_8X16,"A:%05d",AY);
		OLED_Printf(0,32,OLED_8X16,"A:%05d",AZ);
		OLED_Printf(64,0,OLED_8X16,"G:%05d",GX);
		OLED_Printf(64,16,OLED_8X16,"G:%05d",GY);
		OLED_Printf(64,32,OLED_8X16,"G:%05d",GZ);
		OLED_Update();
		
	}
}

void TIM1_UP_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM1,TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
		Key_Tick();
		MPU6050_GetData(&AX,&AY,&AZ,&GX,&GY,&GZ);
	}
}
