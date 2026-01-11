/***************************************************************************************
*PID算法入门，电机控速和电机角度

*外设使用资源说明：
*OLED模块：
	使用模拟I2C，GPIO使用：PB8-SCL PB9-SDA
*LED：
	使用板载LED，GPIO使用：PC13
*Key：
	使用TIM1扫描按键，GPIO使用：KEY1:PB10 KEY2:PB11 KEY3:PA11 KEY4:PA12 
*电位器RP：
	使用ADC2通道2/3/4/5，GPIO使用：RP1:PA2 RP2:PA3 RP3:PA4 RP4:PA5
*PWM：
	使用TIM2的PWM功能，GPIO使用：PA0
*Motor电机驱动：使用TB6612模块
	PWMA使用TIM2, GPIO使用：AIN1:PB12 AIN2:PB13
*Encode电机测速编码器：
	使用TIM3输入捕获，GPIO使用：E1A:PA6 E1B:PA7
*Serial串口输出：
	使用USART1，GPIO使用：PA9:TXD PA10:RXD

****************************************************************************************/

#include "stm32f10x.h"
#include "OLED.h"
#include "LED.h"
#include "Timer.h"
#include "Key.h"
#include "RP-ADC.h"
#include "Motor.h"
#include "Encode.h"
#include "Serial.h"
#include "Delay.h"
#include "ADC.h"
#include "PID.h" 

#define CENTER_ANGLE 2025 //角度传感器的中心角度值
#define CENTER_RANGE 500 //角度传感器的中心角度值可调区间范围

#define START_PWM 35 //启动力度
#define START_TIME 100 //起摆时间

uint8_t KeyNum=0;//按键值
uint8_t RunStart=0; //运行标志位0为停止 非0为运行
uint16_t Angle=0;//角度传感器值
int16_t Speed, Location = 0;//电机速度值,位置值

//内环PID 角速度传感器
PID_t AnglePID = 
{
	.Target = CENTER_ANGLE,
	.Kp = 0.3,
	.Ki = 0.01,
	.Kd = 0.4,
	
	.OutMAX = 100,
	.OutMIN = -100,
	
};

//外环PID 电机位置
PID_t LocationPID = 
{
	.Target = 0,
	.Kp = 0.4,
	.Ki = 0,
	.Kd = 4,
	
	.OutMAX = 100,
	.OutMIN = -100,
	
};

int main(void)
{
	OLED_Init();
	LED_Init();
	Key_Init();
	RP_Init();
	Motor_Init();
	Encode_Init();
	Serial_Init();
	AD_Init();
	Timer_Init();
	
	while(1)
	{
		KeyNum = Key_GetNum();
//		Angle = AD_GetValue();	
//		Speed = Encode_Get();
//		Location += Speed;
		if(KeyNum == 1)
		{
			if(RunStart == 0)
			{
					RunStart = 21;
			}
			else
			{
					RunStart = 0;
			}
		}
		else if(KeyNum == 2)
		{
			LocationPID.Target += 408;
			if(LocationPID.Target > 4080)
			{
				LocationPID.Target = 4080;
			}
		}
		else if(KeyNum == 3)
		{
			LocationPID.Target -= 408;
			if(LocationPID.Target < -4080)
			{
				LocationPID.Target = -4080;
			}
		}
		
		if(RunStart)
		{
			LED_ON();
		}
		else
		{
			LED_OFF();
		}
//		AnglePID.Kp = RP_GetValue(1)/4095.0*1;
//		AnglePID.Ki = RP_GetValue(2)/4095.0*1;
//		AnglePID.Kd = RP_GetValue(3)/4095.0*1;
			
		OLED_Printf(0,0*8,OLED_6X8,"Angle");
		OLED_Printf(0,12,OLED_6X8,"Kp:%05.3f", AnglePID.Kp);
		OLED_Printf(0,20,OLED_6X8,"Ki:%05.3f", AnglePID.Ki);
		OLED_Printf(0,28,OLED_6X8,"Kd:%05.3f", AnglePID.Kd);
		OLED_Printf(0,40,OLED_6X8,"Tar:%04.0f", AnglePID.Target);
		OLED_Printf(0,48,OLED_6X8,"Act:%04d", Angle);
		OLED_Printf(0,56,OLED_6X8,"Out:%04.0f", AnglePID.Out);
		
//		LocationPID.Kp = RP_GetValue(1)/4095.0*1;
//		LocationPID.Ki = RP_GetValue(2)/4095.0*1;
//		LocationPID.Kd = RP_GetValue(3)/4095.0*9;
			
		OLED_Printf(64,0*8,OLED_6X8,"Location");
		OLED_Printf(64,12,OLED_6X8,"Kp:%05.3f", LocationPID.Kp);
		OLED_Printf(64,20,OLED_6X8,"Ki:%05.3f", LocationPID.Ki);
		OLED_Printf(64,28,OLED_6X8,"Kd:%05.3f", LocationPID.Kd);
		OLED_Printf(64,40,OLED_6X8,"Tar:%+05.0f", LocationPID.Target);
		OLED_Printf(64,48,OLED_6X8,"Act:%+05d", Location);
		OLED_Printf(64,56,OLED_6X8,"Out:%+04.0f", LocationPID.Out);
		OLED_Update();

	}
}
//TIM1 1ms进一次中断
void TIM1_UP_IRQHandler(void)
{
	static uint16_t Count0= 0, Count1=0, Count2=0, CountTime;
	static uint16_t Angle0, Angle1, Angle2; //本次采样，上次采样，上上次采样
	if(TIM_GetITStatus(TIM1,TIM_IT_Update) == SET)
	{
		Key_Tick();
		Angle = AD_GetValue();	
		Speed = Encode_Get();
		Location += Speed;
		
		if(RunStart == 0)
		{
			Motor_SetPWM(0);
		}
		else if(RunStart == 1)
		{
			Count0++;
			if(Count0 >= 40)
			{
				Count0 = 0;
				Angle2 = Angle1;
				Angle1 = Angle0;
				Angle0 = Angle;
				
				if(Angle0>CENTER_ANGLE+CENTER_RANGE
				&& Angle1>CENTER_ANGLE+CENTER_RANGE
				&& Angle2>CENTER_ANGLE+CENTER_RANGE
				&& Angle1<Angle0
				&& Angle1<Angle2)
				{
					RunStart = 21;
				}
				if(Angle0<CENTER_ANGLE-CENTER_RANGE
				&& Angle1<CENTER_ANGLE-CENTER_RANGE
				&& Angle2<CENTER_ANGLE-CENTER_RANGE
				&& Angle1>Angle0
				&& Angle1>Angle2)
				{
					RunStart = 31;
				}
				
				if(Angle0>CENTER_ANGLE-CENTER_RANGE
				&& Angle0<CENTER_ANGLE+CENTER_RANGE
				&& Angle1>CENTER_ANGLE-CENTER_RANGE
				&& Angle1<CENTER_ANGLE+CENTER_RANGE)				
				{
					Location = 0;
					AnglePID.ErrorInt = 0;
					LocationPID.ErrorInt = 0;
					RunStart = 4;
				}
			}

		}
		else if(RunStart == 21)
		{
			Motor_SetPWM(START_PWM);
			CountTime = START_TIME;
			RunStart = 22;
		}
		else if(RunStart == 22)
		{
			CountTime--;
			if(CountTime == 0)
			{
				RunStart = 23;
			}
		}
		else if(RunStart == 23)
		{
			Motor_SetPWM(-START_PWM);
			CountTime = START_TIME;
			RunStart = 24;
		}
		else if(RunStart == 24)
		{
			CountTime--;
			if(CountTime == 0)
			{
				Motor_SetPWM(0);
				RunStart = 1;
			}
		}
		else if(RunStart == 31)
		{
			Motor_SetPWM(-START_PWM);
			CountTime = START_TIME;
			RunStart = 32;
		}
		else if(RunStart == 32)
		{
			CountTime--;
			if(CountTime == 0)
			{
				RunStart = 33;
			}
		}
		else if(RunStart == 33)
		{
			Motor_SetPWM(START_PWM);
			CountTime = START_TIME;
			RunStart = 34;
		}
		else if(RunStart == 34)
		{
			CountTime--;
			if(CountTime == 0)
			{
				Motor_SetPWM(0);
				RunStart = 1;
			}
		}
		else if(RunStart == 4)
		{
			if(!((Angle < CENTER_ANGLE + CENTER_RANGE) 
				&& (Angle > CENTER_ANGLE - CENTER_RANGE)))
			{
				RunStart = 0;
			}
				
			Count1++;
			if(Count1 >= 5)
			{
				Count1 = 0;
				AnglePID.Actual = Angle;
				PID_Update(&AnglePID);
				Motor_SetPWM(AnglePID.Out);
			}
			Count2++;
			if(Count2 >= 50)
			{
				Count2 = 0;
				LocationPID.Actual = Location;
				PID_Update(&LocationPID);
				AnglePID.Target = CENTER_ANGLE - LocationPID.Out;
			}
			
		}
		
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}
}

