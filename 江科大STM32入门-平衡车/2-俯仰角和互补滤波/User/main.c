#include "stm32f10x.h"
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "Timer.h"
#include "Key.h"
#include "MPU6050.h"
#include "Motor.h"
#include "Encode.h"
#include "Serial.h"
#include "BlueSerial.h"
#include <math.h>

int16_t AX,AY,AZ,GX,GY,GZ;
float MotorSpeed1;
float MotorSpeed2;
float AngleACC;
float AngleGyro;
float Angle;
int main (void){
	
	LED_Init();
	OLED_Init();
	Timer_Init();
	Key_Init();
	MPU6050_Init();
	Motor_Init();
	Encode_Init();
	Serial_Init();
	BlueSerial_Init();	
	while(1)
	{
		OLED_Printf(0,0,OLED_8X16,"X:%+05d",AX);
		OLED_Printf(0,16,OLED_8X16,"Y:%+05d",AY);
		OLED_Printf(0,32,OLED_8X16,"Z:%+05d",AZ);
		OLED_Printf(64,0,OLED_8X16,"X:%+05d",GX);
		OLED_Printf(64,16,OLED_8X16,"Y:%+05d",GY);
		OLED_Printf(64,32,OLED_8X16,"Z:%+05d",GZ);
		OLED_Update();
		
//		BlueSerial_Printf("[plot,%d,%d,%d]",GX,GY,GZ);  
		BlueSerial_Printf("[plot,%f,%f,%f]",AngleACC,AngleGyro,Angle);  
	}
}

void TIM1_UP_IRQHandler(void)
{
	static uint8_t Count;
	float Alpha = 0;
	if(TIM_GetITStatus(TIM1,TIM_IT_Update) == SET)
	{
		Count++;
		if(Count >= 50)
		{
			Count = 0;
			MotorSpeed1 = Encode_Get(1)/44.0/0.05/9.675;
			MotorSpeed2 = Encode_Get(2)/44.0/0.05/9.675;
		}
		Key_Tick();
		MPU6050_GetData(&AX,&AY,&AZ,&GX,&GY,&GZ);
		AngleACC = -atan2(AX,AZ) / 3.1415926 * 180;
		GY += 14;
		AngleGyro = Angle + GY / 32768.0 * 2000 * 0.001;
		Alpha = 0.001;
		Angle = Alpha * AngleACC + (1-Alpha) * AngleGyro;
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}
}
