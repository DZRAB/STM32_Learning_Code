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
#include <string.h>
#include <stdlib.h>
#include "PID.h"                  

PID_t AnglePID = {
	.Kp = 3,
	.Ki = 0.1,
	.Kd = 3,
	
	.OutMAX = 100,
	.OutMIN = -100,
	
};

int16_t AX,AY,AZ,GX,GY,GZ;
float MotorSpeed1;
float MotorSpeed2;
float AngleACC;
float AngleGyro;
float Angle;

uint8_t KeyNum, RunFlag;
int16_t LeftPWM=0, RightPWM=0;
int16_t AvePWM=0, DifPWM=0;//平均PWM和差分PWM
int main (void)
{
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
		KeyNum = Key_GetNum();
		if(KeyNum == 1)
		{
			if(RunFlag == 0)
			{
				PID_Init(&AnglePID);
				RunFlag = 1;
			}
			else
			{
				RunFlag = 0;
			}
			
		}
		if(RunFlag != 0)
		{
			LED1_ON();
		}
		else
		{
			LED1_OFF();
		}
		
//		OLED_Printf(0,0,OLED_8X16,"X:%+05d",AX);
//		OLED_Printf(0,16,OLED_8X16,"Y:%+05d",AY);
//		OLED_Printf(0,32,OLED_8X16,"Z:%+05d",AZ);
//		OLED_Printf(64,0,OLED_8X16,"X:%+05d",GX);
//		OLED_Printf(64,16,OLED_8X16,"Y:%+05d",GY);
//		OLED_Printf(64,32,OLED_8X16,"Z:%+05d",GZ);
//		OLED_Update();
		
		OLED_Clear();
		OLED_Printf(0,0,OLED_6X8,"  Angle",Angle);
		OLED_Printf(0,8,OLED_6X8,"P:%05.2f",AnglePID.Kp);
		OLED_Printf(0,16,OLED_6X8,"I:%05.2f",AnglePID.Ki);
		OLED_Printf(0,24,OLED_6X8,"D:%05.2f",AnglePID.Kd);
		OLED_Printf(0,32,OLED_6X8,"T:%+05.1f",AnglePID.Target);
		OLED_Printf(0,40,OLED_6X8,"A:%+05.1f",Angle);
		OLED_Printf(0,48,OLED_6X8,"O:%+05.1f",AnglePID.Out);		
		OLED_Update();
		
		if (BlueSerial_RxFlag == 1)
		{	
			char *Tag = strtok(BlueSerial_RxPacket, ",");
			if (strcmp(Tag, "key") == 0)
			{
				char *Name = strtok(NULL, ",");
				char *Action = strtok(NULL, ",");
				
			}
			else if (strcmp(Tag, "slider") == 0)
			{
				char *Name = strtok(NULL, ",");
				char *Value = strtok(NULL, ",");
				if(strcmp(Name, "AngleKp") == 0)
				{
					AnglePID.Kp = atof(Value);
				}
				else if(strcmp(Name, "AngleKi") == 0)
				{
					AnglePID.Ki = atof(Value);
				}
				else if(strcmp(Name, "AngleKd") == 0)
				{
					AnglePID.Kd = atof(Value);
				}

			}
			else if (strcmp(Tag, "joystick") == 0)
			{
				int8_t LH = atoi(strtok(NULL, ","));
				int8_t LV = atoi(strtok(NULL, ","));
				int8_t RH = atoi(strtok(NULL, ","));
				int8_t RV = atoi(strtok(NULL, ","));
				
				AnglePID.Target = LV / 10;
				DifPWM = RH / 2;
			}
			BlueSerial_RxFlag = 0;
		}
		BlueSerial_Printf("[plot,%f,%f]",AnglePID.Target,Angle);  

	}
}


void TIM1_UP_IRQHandler(void)
{
	static uint8_t Count,AngleCount;
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
		AngleCount++;
		if(AngleCount >= 10)
		{
			AngleCount = 0;
			MPU6050_GetData(&AX,&AY,&AZ,&GX,&GY,&GZ);
			AngleACC = -atan2(AX,AZ) / 3.1415926 * 180;
			GY += 16;
			AngleACC += 5;
			AngleGyro = Angle + GY / 32768.0 * 2000 * 0.01;
			Alpha = 0.01;
			Angle = Alpha * AngleACC + (1-Alpha) * AngleGyro;
			
			if(Angle > 50 || Angle < -50)
			{
				RunFlag = 0;
			}	
			if(RunFlag != 0)
			{
				AnglePID.Actual = Angle;
				PID_Update(&AnglePID);
				AvePWM = -AnglePID.Out;
				
				LeftPWM = AvePWM + DifPWM / 2;
				RightPWM = AvePWM - DifPWM / 2;
				
				if(LeftPWM > 100)
				{
					LeftPWM = 100;
				}
				else if(LeftPWM < -100)
				{
					LeftPWM = -100;
				}
				
				if(RightPWM > 100)
				{
					RightPWM = 100;
				}
				else if(RightPWM < -100)
				{
					RightPWM = -100;
				}
				
				Motor_SetPWM(1,LeftPWM);
				Motor_SetPWM(2,RightPWM);
			}
			else
			{
				Motor_SetPWM(1,0);
				Motor_SetPWM(2,0);
			}
		}
		Key_Tick();
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}
}
