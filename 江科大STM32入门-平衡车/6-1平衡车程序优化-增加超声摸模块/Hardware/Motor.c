#include "Motor.h"

void Motor_Init(void)
{
	//配置RCC-GPIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	//配置GPIO
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	PWM_Init();
}

void Motor_SetPWM(uint8_t n, int8_t PWM)
{
	if(n == 1)
	{
		if(PWM >= 0) //正数是正转
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_12);
			GPIO_ResetBits(GPIOB,GPIO_Pin_13);
			PWM_SetCompare1(PWM);
		}
		else //负数反转
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_13);
			GPIO_ResetBits(GPIOB,GPIO_Pin_12);
			PWM_SetCompare1(-PWM);
		}
	}
	else if(n == 2)
	{
		if(PWM >= 0) //正数是正转
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_15);
			GPIO_ResetBits(GPIOB,GPIO_Pin_14);
			PWM_SetCompare2(PWM);
		}
		else //负数反转
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_14);
			GPIO_ResetBits(GPIOB,GPIO_Pin_15);
			PWM_SetCompare2(-PWM);
		}
	}	
	
	
}


