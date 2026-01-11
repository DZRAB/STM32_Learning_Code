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
#include "math.h"


uint8_t KeyNum = 0;

int16_t Speed, Location;

float InnerTarget, InnerActual, InnerOut; //目标值，实际值，输出值
float InnerKp = 0.3, InnerKi = 0.3, InnerKd = 0;//比例项，积分项，微分项的权重表
float InnerError0, InnerError1, InnerErrorInt;//本次误差，上次误差，误差积分

float OuterTarget, OuterActual, OuterOut; //目标值，实际值，输出值
float OuterKp = 0.3, OuterKi = 0, OuterKd = 0.4;//比例项，积分项，微分项的权重表
float OuterError0, OuterError1, OuterErrorInt;//本次误差，上次误差，误差积分

int main(void)
{
	OLED_Init();
	Motor_Init();
	Key_Init();
	Timer_Init();
	Encode_Init();
	RP_Init();
	Serial_Init();
	
	OLED_ShowString(0,0*8,"2*PID Control:",OLED_8X16);
	OLED_Update();		
	
	while(1)
	{
//		KeyNum = Key_GetNum();
				
//		if(KeyNum == 1)
//		{
//			InnerTarget += 10;
//		}
//		if(KeyNum == 2)
//		{
//			InnerTarget -= 10;
//		}
//		if(KeyNum == 3)
//		{
//			InnerTarget = 0;
//		}
		/*内环单独调参*/
//		InnerKp = RP_GetValue(1)/4095.0*2;
//		InnerKi = RP_GetValue(2)/4095.0*2;
//		InnerKd = RP_GetValue(3)/4095.0*2;
//		InnerTarget = RP_GetValue(4)/4095.0 * 300 - 150;
//			
//		OLED_Printf(0,2*8,OLED_8X16,"Kp:%4.2f",InnerKp);
//		OLED_Printf(0,4*8,OLED_8X16,"Ki:%4.2f",InnerKi);
//		OLED_Printf(0,6*8,OLED_8X16,"Kd:%4.2f",InnerKd);
//		
//		OLED_Printf(64,2*8,OLED_8X16,"Tar:%+04.0f",InnerTarget);
//		OLED_Printf(64,4*8,OLED_8X16,"Act:%+04.0f",InnerActual);
//		OLED_Printf(64,6*8,OLED_8X16,"Out:%+04.0f",InnerOut);
//		
//		OLED_Update();		
//		Serial_Printf("%f,%f,%f,%f\r\n",InnerTarget,InnerActual,InnerOut);

		/*双环-外环调参*/
//		OuterKp = RP_GetValue(1)/4095.0*2;
//		OuterKi = RP_GetValue(2)/4095.0*2;
//		OuterKd = RP_GetValue(3)/4095.0*2;
		OuterTarget = RP_GetValue(4)/4095.0 * 816 - 408;
			
		OLED_Printf(0,2*8,OLED_8X16,"Kp:%4.2f",OuterKp);
		OLED_Printf(0,4*8,OLED_8X16,"Ki:%4.2f",OuterKi);
		OLED_Printf(0,6*8,OLED_8X16,"Kd:%4.2f",OuterKd);
		
		OLED_Printf(64,2*8,OLED_8X16,"Tar:%+04.0f",OuterTarget);
		OLED_Printf(64,4*8,OLED_8X16,"Act:%+04.0f",OuterActual);
		OLED_Printf(64,6*8,OLED_8X16,"Out:%+04.0f",OuterOut);
		
		OLED_Update();		
		Serial_Printf("%f,%f,%f,%f\r\n",OuterTarget,OuterActual,OuterOut);

	}
}

//TIM1 1ms进一次中断
void TIM1_UP_IRQHandler(void)
{
	static uint16_t Count;
	static uint16_t Count2;

	if(TIM_GetITStatus(TIM1,TIM_IT_Update) == SET)
	{	
		Key_Tick();		
		/*内环*/
		Count++;
		if(Count>40)
		{
			Count = 0;
			Speed = Encode_Get();
			Location += Speed;
			InnerActual = Speed;
			InnerError1 = InnerError0;
			InnerError0 = InnerTarget - InnerActual;
			
			if(fabs(InnerKi) > 0.0001)
				InnerErrorInt += InnerError0;
			else
				InnerErrorInt = 0;
			
			InnerOut = InnerKp*InnerError0 + InnerKi*InnerErrorInt + InnerKd*(InnerError0 - InnerError1);
			
			if(InnerOut > 100)
				InnerOut = 100;
			if(InnerOut < -100)
				InnerOut = -100;
				
			Motor_SetPWM(InnerOut);
		}
		/*外环*/
		Count2++;
		if(Count2>40)
		{
			Count2 = 0;
			OuterActual = Location;
			OuterError1 = OuterError0;
			OuterError0 = OuterTarget - OuterActual;
			
			if(fabs(InnerKi) > 0.0001)
				OuterErrorInt += OuterError0;
			else
				OuterErrorInt = 0;
			
			OuterOut = OuterKp*OuterError0 + OuterKi*OuterErrorInt + OuterKd*(OuterError0 - OuterError1);
			
			if(OuterOut > 20)
				OuterOut = 20;
			if(OuterOut < -20)
				OuterOut = -20;
				
			InnerTarget = OuterOut;
		}
		
		
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}
}
