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
	.Kp = 3,
	.Ki = 3,
	.Kd = 0,
	
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
float TrackSpeed = 2.0f;            /* 寻迹前进速度目标值(m/s)，Track_GetPosition返回  */
                                    /* 的位置乘以 TrackKp 后作为 TurnPID 的转向目标     */
float TrackKp = 2.5f;               /* 寻迹转向比例系数                                */
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
		/* 寻迹状态显示 */
		if (TrackFlag)
		{
			OLED_Printf(72,56,OLED_6X8,"TRK:ON");
		}
		else
		{
			OLED_Printf(72,56,OLED_6X8,"TRK:OFF");
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
				else if(strcmp(Name, "SpeedKp") == 0)
				{
					SpeedPID.Kp = atof(Value);
				}
				else if(strcmp(Name, "SpeedKi") == 0)
				{
					SpeedPID.Ki = atof(Value);
				}
				else if(strcmp(Name, "SpeedKd") == 0)
				{
					SpeedPID.Kd = atof(Value);
				}
				else if(strcmp(Name, "TurnKp") == 0)
				{
					TurnPID.Kp = atof(Value);
				}
				else if(strcmp(Name, "TurnKi") == 0)
				{
					TurnPID.Ki = atof(Value);
				}
				else if(strcmp(Name, "TurnKd") == 0)
				{
					TurnPID.Kd = atof(Value);
				}
				else if(strcmp(Name, "Offset") == 0)
				{
					AnglePID.OutOffset = atof(Value);
				}
				else if(strcmp(Name, "TrackKp") == 0)
				{
					TrackKp = atof(Value);
				}
				else if(strcmp(Name, "TrackSpeed") == 0)
				{
					TrackSpeed = atof(Value);
				}
			}
			else if (strcmp(Tag, "track") == 0)
			{
				/*
				 * 寻迹模式开关：[track,1] 开启  [track,0] 关闭
				 *
				 * 开启时自动完成三件事：
				 *   ① 设置 TrackFlag = 1
				 *   ② 重置三个PID的积分项/误差项（避免旧状态污染）
				 *   ③ 设置 RunFlag = 1（启动平衡运行）
				 *
				 * 关闭时仅清标志，车继续平衡但不会再自动跟踪黑线，
				 * 摇杆控制恢复。
				 */
				uint8_t cmd = atoi(strtok(NULL, ","));
				if (cmd == 1)
				{
					TrackFlag = 1;
					PID_Init(&AnglePID);
					PID_Init(&SpeedPID);
					PID_Init(&TurnPID);
					RunFlag = 1;
				}
				else
				{
					TrackFlag = 0;
				}
			}
			else if (strcmp(Tag, "joystick") == 0)
			{
				int8_t LH = atoi(strtok(NULL, ","));
				int8_t LV = atoi(strtok(NULL, ","));
				int8_t RH = atoi(strtok(NULL, ","));
				int8_t RV = atoi(strtok(NULL, ","));
				
				/* 寻迹模式下，不响应摇杆控制 */
				if (!TrackFlag)
				{
					SpeedPID.Target = LV / 25.0;
					TurnPID.Target = RH / 25.0;
				}
			}
			BlueSerial_RxFlag = 0;
		}
		BlueSerial_Printf("[plot,%f,%f]",TurnPID.Target,DifSpeed);  

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
			LeftSpeed = Encode_Get(1)/44.0/0.05/9.6;
			RightSpeed = Encode_Get(2)/44.0/0.05/9.6;
			
			AveSpeed = (LeftSpeed + RightSpeed)/2;
			DifSpeed = LeftSpeed - RightSpeed;
			
			if(RunFlag != 0)
			{
				/*
				 * ── 寻迹模式控制逻辑 ──
				 *
				 * 控制架构（与摇杆共用同一套PID，只是目标值来源不同）：
				 *
				 *   外部输入               Speed/Turn PID           Angle PID
				 *   ┌───────┐            ┌──────────────────┐    ┌────────────┐
				 *   │ 摇杆  │──LV→       │ SpeedPID         │    │ AnglePID   │
				 *   │ 寻迹  │──pos×Kp→   │ Target → Out     │───→│ Target     │
				 *   └───────┘            │                  │    │  → P(平衡) │──→ AvePWM
				 *                        │ TurnPID          │    └────────────┘
				 *                        │ Target → Out     │──→ DifPWM
				 *                        └──────────────────┘
				 *
				 * 速度环(SpeedPID)每50ms运行一次，角度环(AnglePID)每10ms运行一次。
				 * 当 TrackFlag=1 时，寻迹模块覆盖摇杆输入，设置：
				 *   SpeedPID.Target = TrackSpeed           // 固定前进速度
				 *   TurnPID.Target  = pos * TrackKp        // 基于位置偏差的转向
				 *
				 * 这种"串级+P"控制架构的好处：
				 *   - 寻迹不修改底层平衡控制，安全
				 *   - 可以和摇杆控制共用同一PID环路
				 *   - 在线切换无冲击（因为PID_Init重置了积分项）
				 */
				if (TrackFlag)
				{
					/*
					 * Track_GetPosition() 返回传感器检测到的黑线位置：
					 *   - 正常范围：-46.5mm ~ +46.5mm（相对于车体中线）
					 *   - 负值=线在左侧，正值=线在右侧
					 *   - 脱线时返回 999.0f
					 *
					 * 控制策略：
					 *   有黑线：根据偏移量计算转向，保持固定前进速度
					 *   脱  线：降速至30%缓慢前进，尝试重新找回黑线
					 */
					float pos = Track_GetPosition();
					if (pos < 100.0f)           // 检测到黑线 (正常范围 ±46.5)
					{
						/*
						 * TurnPID.Target = pos × TrackKp
						 * 将黑线偏移量(mm)转换为转向PID的目标值。
						 *
						 * 符号约定：
						 *   pos > 0 (线偏右) → TurnPID.Target > 0
						 *   → DifPWM > 0 → LeftPWM增加, RightPWM减小
						 *   → 小车右转 → 纠正偏右 → 线回到中间
						 *
						 *   pos < 0 (线偏左) → 左转 → 同理纠正
						 */
						SpeedPID.Target = TrackSpeed;           // 恒速前进
						TurnPID.Target  = pos * TrackKp;        // 转向控制
					}
					else                        // 脱线：≈ 传感器离地或线消失
					{
						/*
						 * 脱线处理：
						 * 降速到30%并保持最后转向方向（PID积分保持），
						 * 让车子缓慢前进并可能自行找回黑线。
						 *
						 * 如果持续脱线超过一定时间（可后续增加超时计数器），
						 * 可以考虑停止运行或原地旋转搜索。
						 */
						SpeedPID.Target = TrackSpeed * 0.3f;
						/* TurnPID.Target保持上次值（由PID积分维持） */
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
