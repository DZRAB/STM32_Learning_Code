#include "stm32f10x.h"
#include "OLED.h"
#include "LED.h"
#include "Timer.h"
#include "Key.h"
#include "RP-ADC.h"
#include "Motor.h"
#include "Encode.h"
#include "Serial.h"

//串口测试程序
int main(void)
{
	uint16_t RP1_Num, RP2_Num, RP3_Num, RP4_Num;

	OLED_Init();
	RP_Init();
	Serial_Init();
	
	OLED_ShowString(0,0*8,"RP1_Num:",OLED_8X16);
	OLED_ShowString(0,2*8,"RP2_Num:",OLED_8X16);
	OLED_ShowString(0,4*8,"RP3_Num:",OLED_8X16);
	OLED_ShowString(0,6*8,"RP4_Num:",OLED_8X16);
	
	OLED_Update();
	
	while(1){
		
		RP1_Num = RP_GetValue(1);
		RP2_Num = RP_GetValue(2);
		RP3_Num = RP_GetValue(3);
		RP4_Num = RP_GetValue(4);
		OLED_ShowNum(9*8,0*8,RP1_Num,5,OLED_8X16);
		OLED_ShowNum(9*8,2*8,RP2_Num,5,OLED_8X16);
		OLED_ShowNum(9*8,4*8,RP3_Num,5,OLED_8X16);
		OLED_ShowNum(9*8,6*8,RP4_Num,5,OLED_8X16);
		OLED_Update();
		Serial_Printf("%d, %d, %d, %d\r\n", RP1_Num, RP2_Num, RP3_Num, RP4_Num);
	}
}

/*
//PWM 电机 电机编码器测速 测试程序
int16_t speed, location;

int main(void)
{
	OLED_Init();
	Motor_Init();
	Key_Init();
	Timer_Init();
	Encode_Init();
	
	uint8_t PWMNum;
	uint8_t KeyNum;
	
	OLED_ShowString(0,0*8,"KeyNum:",OLED_8X16);
	OLED_ShowString(0,2*8,"PWMNum:",OLED_8X16);
	OLED_ShowString(0,4*8,"speed:",OLED_8X16);
	OLED_ShowString(0,6*8,"location:",OLED_8X16);
	
	while(1)
	{
		KeyNum = Key_GetNum();
				
		if(KeyNum == 1)
		{
			OLED_ShowNum(8*8,0*8,KeyNum,5,OLED_8X16);
			PWMNum += 10;
		}
		if(KeyNum == 2)
		{
			OLED_ShowNum(8*8,0*8,KeyNum,5,OLED_8X16);
			PWMNum -= 10;
		}

		if(PWMNum > 100)
		{
			PWMNum = 0;
		}
			
		OLED_ShowNum(8*8,0*8,KeyNum,5,OLED_8X16);
		OLED_ShowNum(8*8,2*8,PWMNum,5,OLED_8X16);
		OLED_ShowSignedNum(9*8,4*8,speed,5,OLED_8X16);
		OLED_ShowSignedNum(10*8,6*8,location,5,OLED_8X16);
		Motor_SetPWM(PWMNum);
		OLED_Update();		
	}
}
//TIM1 1ms进一次中断
void TIM1_UP_IRQHandler(void)
{
	static uint16_t Count;
	if(TIM_GetITStatus(TIM1,TIM_IT_Update) == SET)
	{		
		Count++;
		Key_Tick();
		if(Count>=40)
		{
			Count = 0;
			speed = Encode_Get();
			location += speed;
		}
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}
}
*/

/*
//RP电位器旋钮测试程序
int main(void)
{
	uint16_t RP1_Num, RP2_Num, RP3_Num, RP4_Num;

	OLED_Init();
	RP_Init();
	
	OLED_ShowString(0,0*8,"RP1_Num:",OLED_8X16);
	OLED_ShowString(0,2*8,"RP2_Num:",OLED_8X16);
	OLED_ShowString(0,4*8,"RP3_Num:",OLED_8X16);
	OLED_ShowString(0,6*8,"RP4_Num:",OLED_8X16);
	
	OLED_Update();
	
	while(1){
		
		RP1_Num = RP_GetValue(1);
		RP2_Num = RP_GetValue(2);
		RP3_Num = RP_GetValue(3);
		RP4_Num = RP_GetValue(4);
		OLED_ShowNum(9*8,0*8,RP1_Num,5,OLED_8X16);
		OLED_ShowNum(9*8,2*8,RP2_Num,5,OLED_8X16);
		OLED_ShowNum(9*8,4*8,RP3_Num,5,OLED_8X16);
		OLED_ShowNum(9*8,6*8,RP4_Num,5,OLED_8X16);
		OLED_Update();
	}
}
*/

/* 
//OLED LED TIM KEY测试程序
int main(void)
{
	uint32_t  Num;
	uint8_t KeyNum;

	OLED_Init();
	LED_Init();
	Timer_Init();
	Key_Init();
	
	OLED_ShowString(0,0*8,"HelloWorld",OLED_8X16);
	OLED_ShowString(0,2*8,"HelloWorld",OLED_8X16);
	OLED_ShowString(0,4*8,"Num:",OLED_8X16);
	OLED_ShowString(0,6*8,"KeyNum:",OLED_8X16);
	
	OLED_Update();
	LED_ON();
	
	while(1){
		KeyNum = Key_GetNum();
		if(KeyNum != 0)
			OLED_ShowNum(8*8,6*8,KeyNum,2,OLED_8X16);
		OLED_ShowNum(5*8,4*8,Num,8,OLED_8X16);
		OLED_Update();
	}
}
//TIM1 1ms进一次中断
void TIM1_UP_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM1,TIM_IT_Update) == SET)
	{
		
		Num++;
		Key_Tick();
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}
}
*/


