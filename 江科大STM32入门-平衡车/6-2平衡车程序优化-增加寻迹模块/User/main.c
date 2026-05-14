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

/*══════════════════════════════════════════════════════════════════════
 *  平衡小车PID参数定义
 *══════════════════════════════════════════════════════════════════════
 *  控制架构（串级+P）：
 *    循迹/摇杆 → SpeedPID/TurnPID → AnglePID → 电机PWM
 *══════════════════════════════════════════════════════════════════════*/

/* ── 角度环 PID（平衡控制，10ms更新一次）──
 *   Kp：主要恢复力，角度误差直接对应电机出力
 *   Ki：消除静差（重心偏移、陀螺零偏等）
 *   Kd：微分阻尼，抑制前后摆动
 *   OutOffset：电机死区补偿（PWM小信号跳过电机不转区域）
 */
PID_t AnglePID = {
	.Kp = 5,
	.Ki = 0.05,
	.Kd = 3,
	.OutMAX = 100,
	.OutMIN = -100,
	.OutOffset = 1,
	.ErrorIntMAX = 600,
	.ErrorIntMIN = -600,
};

/* ── 速度环 PID（50ms更新一次）──
 *   Target：循迹时设为 TrackSpeed，摇杆时来自摇杆
 *   Actual：左右轮平均速度（编码器测得）
 *   Out 作为角度环的 Target（车体前倾=前进）
 */
PID_t SpeedPID = {
	.Kp = 2,
	.Ki = 0.05,
	.Kd = 0,
	.OutMAX = 10,
	.OutMIN = -10,
	.ErrorIntMAX = 150,
	.ErrorIntMIN = -150,
};

/* ── 转向环 PID（PD控制，Ki=0 避免积分饱和，50ms更新一次）──
 *   Target：循迹时 = pos × kp（黑线偏移量×转向系数）
 *   Actual：左右轮差速（左-右，正=右转倾向）
 *   Out 作为 DifPWM，叠加到左右轮PWM上产生转向
 *   Kd 提供阻尼，防止出弯过冲
 */
PID_t TurnPID = {
	.Kp = 5,
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

/* ── 循迹相关变量 ─────────────────────────────────────────────*/

uint8_t TrackFlag = 0;              /* 循迹模式：1=开启, 0=关闭  */
float TrackSpeed = 1.0f;            /* 循迹前进速度目标值          */
float TrackKp = 0.15f;              /* 转向比例系数（位置mm→目标） */

#define CONF_TRACK_KP_CORNER_BOOST 2.0f   /* 弯道增强倍数          */
#define CONF_TRACK_DEAD_ZONE_MM 3.0f      /* 死区宽度(mm)          */

float TrackLastPos = 0.0f;          /* 最近一次有效位置（脱线找回用） */
uint16_t TrackLostCount = 0;        /* 脱线持续计数                  */

/*  控制关系：
 *   pos = Track_GetPosition();       // 黑线偏移量(mm)
 *   pos = 死区处理, 弯道增强          // 修正后位置
 *   TurnPID.Target = pos * TrackKp;  // 转PID目标值
 */
/*──────────────────────────────────────────────────────────────────*/

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
		/* ── K1：启停平衡 ── */
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
		/* ── K2：开启/关闭循迹模式 ── */
		if(KeyNum == 2)
		{
			if(RunFlag != 0)		// 只有平衡启动后才能开启寻迹
			{
				if(TrackFlag == 0)	// 开启循迹
				{
					PID_Init(&AnglePID);
					PID_Init(&SpeedPID);
					PID_Init(&TurnPID);
					TrackFlag = 1;
				}
				else				// 关闭循迹
				{
					TrackFlag = 0;
				}
			}
		}
		/* K3 未使用，保留备用 */
		if(RunFlag != 0)
		{
			LED1_ON();
		}
		else
		{
			LED1_OFF();
		}
		
		/* ── OLED 显示 ── */
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
		/* ── 循迹状态与传感器显示 ── */
		if (TrackFlag)
		{
			OLED_Printf(72,56,OLED_6X8,"TRK:ON ");
			uint16_t trackBits = Track_Get();
			OLED_Printf(82,56,OLED_6X8,"%c%c%c%c%c",
				(trackBits & 0x01) ? '1' : '0',
				(trackBits & 0x02) ? '1' : '0',
				(trackBits & 0x04) ? '1' : '0',
				(trackBits & 0x08) ? '1' : '0',
				(trackBits & 0x10) ? '1' : '0');
		}
		else
		{
			OLED_Printf(72,56,OLED_6X8,"TRK:OFF");
		}
		OLED_Update();
		
		/* ── 蓝牙串口指令处理 ── */
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
				if(strcmp(Name, "AngleKp") == 0)  { AnglePID.Kp = atof(Value); }
				else if(strcmp(Name, "AngleKi") == 0)  { AnglePID.Ki = atof(Value); }
				else if(strcmp(Name, "AngleKd") == 0)  { AnglePID.Kd = atof(Value); }
				else if(strcmp(Name, "SpeedKp") == 0)  { SpeedPID.Kp = atof(Value); }
				else if(strcmp(Name, "SpeedKi") == 0)  { SpeedPID.Ki = atof(Value); }
				else if(strcmp(Name, "SpeedKd") == 0)  { SpeedPID.Kd = atof(Value); }
				else if(strcmp(Name, "TurnKp") == 0)   { TurnPID.Kp = atof(Value); }
				else if(strcmp(Name, "TurnKi") == 0)   { TurnPID.Ki = atof(Value); }
				else if(strcmp(Name, "TurnKd") == 0)   { TurnPID.Kd = atof(Value); }
				else if(strcmp(Name, "Offset") == 0)   { AnglePID.OutOffset = atof(Value); }
				else if(strcmp(Name, "TrackKp") == 0)  { TrackKp = atof(Value); }
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


/*══════════════════════════════════════════════════════════════════════
 *  TIM1 中断（1ms）：平衡控制 + 速度控制 + 循迹控制
 *══════════════════════════════════════════════════════════════════════
 *  每个中断处理三个通道：
 *    50ms通道：编码器读数 + SpeedPID/TurnPID 更新
 *    10ms通道：MPU6050读数 + 角度计算 + AnglePID 更新 + 电机输出
 *  循迹传感器在50ms通道中随PID一起同步刷新，保证Target新鲜。
 *══════════════════════════════════════════════════════════════════════*/
void TIM1_UP_IRQHandler(void)
{
	static uint8_t Count,AngleCount;
	float Alpha = 0;
	if(TIM_GetITStatus(TIM1,TIM_IT_Update) == SET)
	{
		/* ── 50ms通道：循迹 + 速度/转向PID ── */
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
				 * ── 循迹控制逻辑（与PID同步执行） ──
				 * 读5路TCRT5000传感器 → 加权重心法算偏移 →
				 * 死区处理 → 弯道增强 → 设置TurnPID.Target
				 *
				 * 循迹带宽度仅18mm，传感器间距22~24.5mm，
				 * 死区±3mm避免传感器噪声导致频繁微调。
				 * 弯道|pos|>12mm时kp线性增强至2倍(30mm达峰值)，
				 * 使直角转弯有足够转向力，直行又不会太敏感。
				 */
				if (TrackFlag)
				{
					float pos = Track_GetPosition();
					if (pos < 100.0f)           // 检测到黑线
					{
						/* 死区：中心附近小偏移不修正 */
						if (pos > -CONF_TRACK_DEAD_ZONE_MM && pos < CONF_TRACK_DEAD_ZONE_MM)
							pos = 0.0f;

						/* 弯道平滑增强（12~30mm线性过渡） */
						float kp = TrackKp;
						float absPos = (pos > 0) ? pos : -pos;
						if (absPos > 12.0f)
						{
							float t = (absPos - 12.0f) / 18.0f;
							if (t > 1.0f) t = 1.0f;
							kp = TrackKp * (1.0f + t * (CONF_TRACK_KP_CORNER_BOOST - 1.0f));
						}

						SpeedPID.Target = TrackSpeed;           // 恒速前进
						TurnPID.Target  = pos * kp;             // 转向目标
						TrackLastPos = pos;                     // 记录位置
						TrackLostCount = 0;                     // 清零脱线计数
					}
					else                            // 脱线（所有传感器均未检测到黑线）
					{
						TrackLostCount++;
						SpeedPID.Target = TrackSpeed * 0.5f;   // 减速

						/*
						 * ── 三阶段脱线找回策略 ──
						 * Phase1(~400ms)：向最后消失方向慢转
						 * Phase2(~1.5s)：摆动扫描覆盖两侧
						 * Phase3(1.5s+)：停止前进，原地自旋
						 */
						if (TrackLostCount < 8)                  // Phase 1
						{
							if (!(TrackLastPos > -30.0f && TrackLastPos < 30.0f))
								TurnPID.Target = (TrackLastPos > 0) ? 3.0f : -3.0f;
						}
						else if (TrackLostCount < 30)            // Phase 2
						{
							SpeedPID.Target = TrackSpeed * 0.3f;
							TurnPID.Target = ((TrackLostCount / 6) % 2 == 0) ? 4.0f : -4.0f;
						}
						else                                     // Phase 3
						{
							SpeedPID.Target = 0;
							TurnPID.Target = ((TrackLostCount / 12) % 2 == 0) ? 5.0f : -5.0f;
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

		/* ── 10ms通道：角度计算 + AnglePID + 电机输出 ── */
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
				
				if(LeftPWM > 100)   LeftPWM = 100;
				else if(LeftPWM < -100) LeftPWM = -100;
				if(RightPWM > 100)  RightPWM = 100;
				else if(RightPWM < -100) RightPWM = -100;
				
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