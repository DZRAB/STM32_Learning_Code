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

float Target, Actual, Out, Actual1; //目标值，实际值，输出值，上次的实际值
float Kp = 0.3, Ki = 0.3   , Kd = 0;//比例项，积分项，微分项的权重表
float Error0, Error1, ErrorInt;//本次误差，上次误差，误差积分
float DifOut;//微分项的输出

int main(void)
{
	OLED_Init();
	Motor_Init();
	Key_Init();
	Timer_Init();
	Encode_Init();
	RP_Init();
	Serial_Init();
	
	OLED_ShowString(0,0*8,"Location Control:",OLED_8X16);
	OLED_Update();		
	
	while(1)
	{
		KeyNum = Key_GetNum();
				
		if(KeyNum == 1)
		{
			Target += 10;
		}
		if(KeyNum == 2)
		{
			Target -= 10;
		}
		if(KeyNum == 3)
		{
			Target = 0;
		}
		Kp = RP_GetValue(1)/4095.0*2;
		Ki = RP_GetValue(2)/4095.0*2;
		Kd = RP_GetValue(3)/4095.0*2;
		Target = RP_GetValue(4)/4095.0 * 816 - 408;
			
		OLED_Printf(0,2*8,OLED_8X16,"Kp:%4.2f",Kp);
		OLED_Printf(0,4*8,OLED_8X16,"Ki:%4.2f",Ki);
		OLED_Printf(0,6*8,OLED_8X16,"Kd:%4.2f",Kd);
		
		OLED_Printf(64,2*8,OLED_8X16,"Tar:%+04.0f",Target);
		OLED_Printf(64,4*8,OLED_8X16,"Act:%+04.0f",Actual);
		OLED_Printf(64,6*8,OLED_8X16,"Out:%+04.0f",Out);
		
		OLED_Update();		
		Serial_Printf("%f,%f,%f,%f\r\n",Target,Actual,Out,DifOut);
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
		if(Count>40)
		{
			Count = 0;
			Actual1 = Actual;
			Actual += Encode_Get();
			Error1 = Error0;
			Error0 = Target - Actual;
			
			if(fabs(Ki) > 0.0001)
				ErrorInt += Error0;
			else
				ErrorInt = 0;
			
			DifOut = -Kd*(Actual - Actual1);
			
			Out = Kp*Error0 + Ki*ErrorInt + DifOut;
			
			if(Out > 100)
				Out = 100;
			if(Out < -100)
				Out = -100;
				
			Motor_SetPWM(Out);
		}
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}
}
