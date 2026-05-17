#include "stm32f10x.h"
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "Timer.h"
#include "Key.h"
#include "MPU6050.h"
#include "Motor.h"
#include "Encode.h"
#include "Track.h"
#include "BlueSerial.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "PID.h"                  

PID_t AnglePID = {
	.Kp = 5,
	.Ki = 0.1,
	.Kd = 5,
	
	.OutMAX = 100,
	.OutMIN = -100,
	
	.OutOffset = 3,
	.ErrorIntMAX = 600,
	.ErrorIntMIN = -600,
};
PID_t SpeedPID = {
	.Kp = 2,
	.Ki = 0.05,
	.Kd = 0,
	
	.OutMAX = 10,
	.OutMIN = -10,
	.ErrorIntMAX = 150,
	.ErrorIntMIN = -150,
};
PID_t TurnPID = {
	.Kp = 6,
	.Ki = 0,
	.Kd = 1.5,
	
	.OutMAX = 50,
	.OutMIN = -50,
	
	.ErrorIntMAX = 20,
	.ErrorIntMIN = -20,
};

int16_t AX,AY,AZ,GX,GY,GZ;
float LeftSpeed, RightSpeed;
float AveSpeed,DifSpeed;

float AngleACC;
float AngleGyro;
float Angle;

uint8_t KeyNum, RunFlag;
uint8_t TrackFlag = 0;              /* 寻迹模式标志：1=开启，0=关闭                    */
float TrackSpeed = 0.8f;            /* 循迹前进速度目标值                                   */
float TrackKp = 0.3f;               /* 寻迹转向比例系数                                   */
float TrackLastPos = 0.0f;          /* 上次有效位置，盲区找回用                          */
uint8_t TrackLostCount = 0;         /* 失线持续次数                                      */
                                    /*                                                      */
                                    /* 控制关系：                                        */
                                    /*   pos = Track_GetPosition();   // 单位：mm        */
                                    /*   TurnPID.Target = pos * TrackKp;                 */
                                    /*                                                      */
                                    /* TrackKp初始值估算：                               */
                                    /*   假设最大偏移 ±46.5mm，希望转向PID目标限幅在     */
                                    /*   ±5左右，则 TrackKp ≈ 5/46.5 ≈ 0.108            */
                                    /*   但实际中需要地面实测，建议从0.5开始调           */
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
	Track_Init();
	BlueSerial_Init();	
	
	while(1)
	{
		KeyNum = Key_GetNum();
		if(KeyNum == 1)
		{
			if(RunFlag == 0)
			{
				PID_Init(&AnglePID);
				PID_Init(&SpeedPID);
				PID_Init(&TurnPID);
				RunFlag = 1;
			}
			else
			{
				RunFlag = 0;
				TrackFlag = 0;		// 停止平衡时同时关闭寻迹
			}
			
		}
		if(KeyNum == 2)
		{
			if(RunFlag != 0)		// 只有平衡启动后才能开启寻迹
			{
				if(TrackFlag == 0)	// 开启寻迹
				{
					PID_Init(&AnglePID);
					PID_Init(&SpeedPID);
					PID_Init(&TurnPID);
					TrackFlag = 1;
				}
				else				// 关闭寻迹
				{
					TrackFlag = 0;
				}
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
		OLED_Printf(0,56,OLED_6X8,"GY:%+05d",GY);		
		OLED_Printf(56,56,OLED_6X8,"Offset:%02.0f",AnglePID.OutOffset);		
		OLED_Printf(50,0,OLED_6X8,"Speed",Angle);
		OLED_Printf(50,8,OLED_6X8,"%05.2f",SpeedPID.Kp);
		OLED_Printf(50,16,OLED_6X8,"%05.2f",SpeedPID.Ki);
		OLED_Printf(50,24,OLED_6X8,"%05.2f",SpeedPID.Kd);
		OLED_Printf(50,32,OLED_6X8,"%+05.1f",SpeedPID.Target);
		OLED_Printf(50,40,OLED_6X8,"%+05.1f",Angle);
		OLED_Printf(50,48,OLED_6X8,"%+05.1f",SpeedPID.Out);		
		OLED_Printf(88,0,OLED_6X8,"Turn",Angle);
		OLED_Printf(88,8,OLED_6X8,"%05.2f",TurnPID.Kp);
		OLED_Printf(88,16,OLED_6X8,"%05.2f",TurnPID.Ki);
		OLED_Printf(88,24,OLED_6X8,"%05.2f",TurnPID.Kd);
		OLED_Printf(88,32,OLED_6X8,"%+05.1f",TurnPID.Target);
		OLED_Printf(88,40,OLED_6X8,"%+05.1f",Angle);
		OLED_Printf(88,48,OLED_6X8,"%+05.1f",TurnPID.Out);
		/* 寻迹状态 + 传感器显示 */
		if (TrackFlag)
		{
			OLED_Printf(72,56,OLED_6X8,"TRK:ON");
			uint16_t tb = Track_Get();
			OLED_Printf(90,56,OLED_6X8,"%c%c%c%c",
				(tb & 0x08) ? '1' : '0',  /* S4 */
				(tb & 0x04) ? '1' : '0',  /* S3 */
				(tb & 0x02) ? '1' : '0',  /* S2 */
				(tb & 0x01) ? '1' : '0'); /* S1 */
		}
		else
		{
			OLED_Printf(72,56,OLED_6X8,"TRK:OFF  ");
		}
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
				if(strcmp(Name, "AngleKp") == 0)   { AnglePID.Kp = atof(Value); }
				else if(strcmp(Name, "AngleKi") == 0)   { AnglePID.Ki = atof(Value); }
				else if(strcmp(Name, "AngleKd") == 0)   { AnglePID.Kd = atof(Value); }
				else if(strcmp(Name, "SpeedKp") == 0)   { SpeedPID.Kp = atof(Value); }
				else if(strcmp(Name, "SpeedKi") == 0)   { SpeedPID.Ki = atof(Value); }
				else if(strcmp(Name, "SpeedKd") == 0)   { SpeedPID.Kd = atof(Value); }
				else if(strcmp(Name, "TurnKp") == 0)    { TurnPID.Kp = atof(Value); }
				else if(strcmp(Name, "TurnKi") == 0)    { TurnPID.Ki = atof(Value); }
				else if(strcmp(Name, "TurnKd") == 0)    { TurnPID.Kd = atof(Value); }
				else if(strcmp(Name, "Offset") == 0)    { AnglePID.OutOffset = atof(Value); }
				else if(strcmp(Name, "TrackKp") == 0)   { TrackKp = atof(Value); }
				else if(strcmp(Name, "TrackSpeed") == 0){ TrackSpeed = atof(Value); }
			}
			else if (strcmp(Tag, "track") == 0)
			{
				uint8_t cmd = atoi(strtok(NULL, ","));
				if (cmd == 1)
				{
					TrackFlag = 1;
					PID_Init(&AnglePID);
					PID_Init(&SpeedPID);
					PID_Init(&TurnPID);
					RunFlag = 1;
				}
				else { TrackFlag = 0; }
			}
			else if (strcmp(Tag, "joystick") == 0)
			{
				int8_t LH = atoi(strtok(NULL, ","));
				int8_t LV = atoi(strtok(NULL, ","));
				int8_t RH = atoi(strtok(NULL, ","));
				int8_t RV = atoi(strtok(NULL, ","));
				if (!TrackFlag)
				{
					SpeedPID.Target = LV / 25.0;
					TurnPID.Target = RH / 25.0;
				}
			}
			BlueSerial_RxFlag = 0;
		}
		//BlueSerial_Printf("[plot,%f,%f]",TurnPID.Target,DifSpeed);  
	}
}


void TIM1_UP_IRQHandler(void)
{
	static uint8_t Count, SnsCount, AngleCount;
	float Alpha = 0;
	if(TIM_GetITStatus(TIM1,TIM_IT_Update) == SET)
	{
		/* ── 50ms低速通道：编码器读数 + PID更新 ── */
		Count++;
		if(Count >= 50)
		{
			Count = 0;
			LeftSpeed = Encode_Get(1)/44.0/0.05/9.6;
			RightSpeed = Encode_Get(2)/44.0/0.05/9.6;
			
			AveSpeed = (LeftSpeed + RightSpeed)/2;
			DifSpeed = LeftSpeed - RightSpeed;
			
			if(RunFlag != 0)
			{
				/* PID更新前读传感器，保证Target新鲜 */
				if (TrackFlag)
				{
					float pos = Track_GetPosition();
					if (pos < 100.0f) {
						SpeedPID.Target = TrackSpeed;
						float target = pos * TrackKp;
						if (target > -2.4f && target < 2.4f) target *= 0.5f;
						TurnPID.Target = target;
						uint16_t bits = Track_Get();
						if ((bits & 0x01) && !(bits & 0x02)) TurnPID.Target -= 2.0f;
						if ((bits & 0x08) && !(bits & 0x04)) TurnPID.Target += 2.0f;
						TrackLastPos = pos;
						TrackLostCount = 0;
					} else {
						TrackLostCount++;
						/* 盲区找线：根据最后位置向对应方向轻转找回 */
						if (TrackLostCount < 10) {
							SpeedPID.Target = TrackSpeed;
							if (TrackLastPos < 0) TurnPID.Target = -2.0f;
							else                  TurnPID.Target =  2.0f;
						} else {
							SpeedPID.Target = TrackSpeed * 0.3f;
						}
					}
				}
				
				SpeedPID.Actual = AveSpeed;
				PID_Update(&SpeedPID);
				AnglePID.Target = SpeedPID.Out;
				
				TurnPID.Actual = DifSpeed;
				PID_Update(&TurnPID);
				DifPWM = TurnPID.Out;
			}
		}

		/* ── 25ms中速通道：传感器提前刷新Target ── */
		SnsCount++;
		if(SnsCount >= 25 && RunFlag != 0 && TrackFlag)
		{
			SnsCount = 0;
			float pos = Track_GetPosition();
			if (pos < 100.0f) {
				SpeedPID.Target = TrackSpeed;
				float target = pos * TrackKp;
				if (target > -2.4f && target < 2.4f) target *= 0.5f;
				TurnPID.Target = target;
				uint16_t bits = Track_Get();
				if ((bits & 0x01) && !(bits & 0x02)) TurnPID.Target -= 2.0f;
				if ((bits & 0x08) && !(bits & 0x04)) TurnPID.Target += 2.0f;
				TrackLastPos = pos;
				TrackLostCount = 0;
			} else {
				TrackLostCount++;
				if (TrackLostCount < 10) {
					SpeedPID.Target = TrackSpeed;
					if (TrackLastPos < 0) TurnPID.Target = -2.0f;
					else                  TurnPID.Target =  2.0f;
				}
			}
		}

		/* ── 10ms高速通道：MPU6050 + 角度PID ── */
		AngleCount++;
		if(AngleCount >= 10)
		{
			AngleCount = 0;
			MPU6050_GetData(&AX,&AY,&AZ,&GX,&GY,&GZ);
			AngleACC = -atan2(AX,AZ) / 3.1415926 * 180;
			GY += 14;
			AngleACC += 2.5;
			AngleGyro = Angle + GY / 32768.0 * 2000 * 0.01;
			Alpha = 0.01;
			Angle = Alpha * AngleACC + (1-Alpha) * AngleGyro;
			
			if(Angle > 50 || Angle < -50) { RunFlag = 0; }	
			if(RunFlag != 0)
			{
				AnglePID.Actual = Angle;
				PID_Update(&AnglePID);
				AvePWM = -AnglePID.Out;
				
				LeftPWM = AvePWM + DifPWM / 2;
				RightPWM = AvePWM - DifPWM / 2;
				
				if(LeftPWM > 100) LeftPWM = 100;
				else if(LeftPWM < -100) LeftPWM = -100;
				if(RightPWM > 100) RightPWM = 100;
				else if(RightPWM < -100) RightPWM = -100;
				
				Motor_SetPWM(1,LeftPWM);
				Motor_SetPWM(2,RightPWM);
			}
			else { Motor_SetPWM(1,0); Motor_SetPWM(2,0); }
		}
		Key_Tick();
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}
}
