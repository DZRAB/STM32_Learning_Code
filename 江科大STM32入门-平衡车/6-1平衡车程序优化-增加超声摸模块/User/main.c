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
#include "HC_SR04.h"

uint32_t distance_mm;

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
	SR04_Init();
	
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
			}
			
		}
		
		// 检查是否有新的测距结果
		if(sr04_new_data)
		{
			sr04_new_data = 0;  // 清除标志，避免重复处理
			// 将脉宽（微秒）转换为距离（毫米）
			// 公式：距离 = (高电平时间 us) * 340m/s / 2
			// 340m/s = 0.34mm/us，除以 2 得 0.17mm/us
			// 为避免浮点运算，用整数： width * 17 / 100
			if(sr04_width_us == 0) {
				distance_mm = 0;
			}
			else{
				distance_mm = (sr04_width_us * 17) / 100;
			// 现在可以使用 distance_mm 做后续处理，例如串口输出、避障判断等
			}
		}
		
		if(RunFlag != 0)
		{
			LED1_ON();
			if(KeyNum == 2)
			{
				if(distance_mm == 0)
				{
					SpeedPID.Target = 100.0;
				}else if(distance_mm > 800 && distance_mm < 1300)
				{
					SpeedPID.Target = 60.0;
				}else if(distance_mm > 300 && distance_mm < 800)
				{
					SpeedPID.Target = 20.0;
				}else if(distance_mm > 150 && distance_mm < 300)
				{
					SpeedPID.Target = 20.0;
					TurnPID.Target = 25.0;
				}else if(distance_mm > 0 && distance_mm < 150)
				{
					SpeedPID.Target = 0;
				}
			}
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
		//OLED_Printf(56,56,OLED_6X8,"Offset:%02.0f",AnglePID.OutOffset);	
		OLED_Printf(56,56,OLED_6X8,"dis:%05dmm",distance_mm);		
		
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
			}
			else if (strcmp(Tag, "joystick") == 0)
			{
				int8_t LH = atoi(strtok(NULL, ","));
				int8_t LV = atoi(strtok(NULL, ","));
				int8_t RH = atoi(strtok(NULL, ","));
				int8_t RV = atoi(strtok(NULL, ","));
				
				SpeedPID.Target = LV / 25.0;
				TurnPID.Target = RH / 25.0;
			}
			BlueSerial_RxFlag = 0;
		}
		//BlueSerial_Printf("[plot,%f,%f]",TurnPID.Target,DifSpeed);  

	}
}


void TIM1_UP_IRQHandler(void)
{
	static uint8_t Count,AngleCount;
	float Alpha = 0;
	if(TIM_GetITStatus(TIM1,TIM_IT_Update) == SET)
	{
		Count++;
		if(Count >= 5)
		{
			Count = 0;
			LeftSpeed = Encode_Get(1)/44.0/0.05/9.6;
			RightSpeed = Encode_Get(2)/44.0/0.05/9.6;
			
			AveSpeed = (LeftSpeed + RightSpeed)/2;
			DifSpeed = LeftSpeed - RightSpeed;
			
			if(RunFlag != 0)
			{
				SpeedPID.Actual = AveSpeed;
				PID_Update(&SpeedPID);
				AnglePID.Target = SpeedPID.Out;
				
				TurnPID.Actual = DifSpeed;
				PID_Update(&TurnPID);
				DifPWM = TurnPID.Out;
			}

		}
		AngleCount++;
		if(AngleCount >= 1)
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
		
		Num++;
		sr04_overflow_cnt++;        
		// 每隔 50ms 触发一次测距
		if(Num >= 5)
		{
			SR04_Trigger();
			Num -= 5;
		}		
	}
}


/* ============= TIM1 捕获中断服务函数（核心测距逻辑） ============= */
// 注意：这个函数名必须与中断向量表一致（TIM1_CC_IRQn 对应的函数名）
void TIM1_CC_IRQHandler(void)
{
    // 使用静态局部变量，它们只在本函数内可见，且值在多次调用间保持
    static uint8_t step = 0;           // 状态机：0=等待上升沿，1=等待下降沿
    static uint32_t rise_tick = 0;     // 上升沿时定时器的计数值
    static uint32_t rise_overflow = 0; // 上升沿时全局溢出计数器 sr04_overflow_cnt 的快照

    // 检查是否是通道4的捕获中断，如果不是则退出（避免误触发）
    if(TIM_GetITStatus(TIM1, TIM_IT_CC4) == RESET) return;
    TIM_ClearITPendingBit(TIM1, TIM_IT_CC4); // 清除中断标志

    if(step == 0)   // ---------- 当前状态：等待上升沿 ----------
    {
        // 记录上升沿发生时的定时器计数值（从 TIM1_CCR4 寄存器读取）
        rise_tick = TIM_GetCapture4(TIM1);
        // 记录此时全局溢出计数器的值。这个计数器在 1ms 更新中断里累加，
        // 用于计算从上升沿到下降沿之间定时器溢出了多少次（因为脉宽可能超过 1ms）
        rise_overflow = sr04_overflow_cnt;
        // 将通道4的捕获极性改为下降沿，这样下次捕获会在 ECHO 变低时触发
        TIM_OC4PolarityConfig(TIM1, TIM_ICPolarity_Falling);
        step = 1;   // 状态切换到“等待下降沿”
    }
    else            // ---------- 当前状态：等待下降沿 ----------
    {
        // 记录下降沿时的定时器计数值
        uint32_t fall_tick = TIM_GetCapture4(TIM1);
        // 记录下降沿时的全局溢出计数器（注意：此时 sr04_overflow_cnt 可能已经增加了多次）
        uint32_t fall_overflow = sr04_overflow_cnt;
        // 计算从上升沿到下降沿期间，定时器溢出的次数
        // 因为定时器每 1ms 溢出一次，溢出次数差值就是经过的完整毫秒数
        uint32_t width;  // 将存储最终的高电平宽度（单位：定时器计数次数，即微秒）
			
        // 关键：如果发生了溢出（脉宽 >= 10ms），则丢弃本次测量		
				// 检查是否溢出（超出量程）
        if(fall_overflow != rise_overflow) {
            // 超出量程，宽度设为 0
            width = 0;
        } else {
            // 未溢出，正常计算
						// 计算宽度：宽度 = 下降沿计数 - 上升沿计数
						if(fall_tick >= rise_tick)
						{
								// 正常情况：下降沿计数大于上升沿计数，没有发生计数器回绕
								width = fall_tick - rise_tick;
						}
						else
						{
								// 发生了回绕：下降沿计数小于上升沿计数，说明在两次捕获之间计数器溢出了至少一次
								// 此时宽度 = ARR+1 - rise_tick + fall_tick
								// 因为 ARR=9999，所以 ARR+1 = 10000
								width = 10000 - rise_tick + fall_tick;
						}
        }
			
        

        // 将计算出的宽度（微秒）存入全局变量，供主循环使用
        sr04_width_us = width;
        // 置位“新数据”标志，通知主循环已经测完一次
        sr04_new_data = 1;

        // 将捕获极性改回上升沿，准备下一次测距
        TIM_OC4PolarityConfig(TIM1, TIM_ICPolarity_Rising);
        step = 0;   // 状态回到“等待上升沿”
    }
}

