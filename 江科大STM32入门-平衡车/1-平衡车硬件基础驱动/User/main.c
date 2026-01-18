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

int16_t AX,AY,AZ,GX,GY,GZ;
float MotorSpeed1;
float MotorSpeed2;
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
	
	BlueSerial_Printf("Init ok!\r\n");
	
	while(1)
	{

		OLED_Printf(0,0,OLED_8X16,"L:%+06f",MotorSpeed1);
		OLED_Printf(0,16,OLED_8X16,"R:%+06f",MotorSpeed2);

		if(BlueSerial_RxFlag == 1)
		{
			OLED_Printf(0,48,OLED_8X16,"%s",BlueSerial_RxPacket);
			BlueSerial_RxFlag = 0;
		}

//		OLED_Printf(0,32,OLED_8X16,"A:%05d",AZ);
//		OLED_Printf(64,0,OLED_8X16,"G:%05d",GX);
//		OLED_Printf(64,16,OLED_8X16,"G:%05d",GY);
//		OLED_Printf(64,32,OLED_8X16,"G:%05d",GZ);
		OLED_Update();
		
	}
}

void TIM1_UP_IRQHandler(void)
{
	static uint8_t Count;
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

		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}
}
