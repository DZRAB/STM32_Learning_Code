#include "CountSensor.h"
#include "Delay.h"

uint16_t CountSensor_Count = 0;

void CountSensor_Init(void)
{
	//使用外部终端，需要开启GPIO, AFIO, EXIT, NVIC的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//启用APB2外设GPIOx的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//启用APB2外设AFIO的时钟
	// EXIT独立外设不需要开启时钟，一直开启的
	// NVIC是内核内的外设不需要开启时钟
	
	//配置GPIO
	GPIO_InitTypeDef GPIO_InitStructure; //GPIO 初始化结构体
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //上拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure); //根据 GPIO_InitStruct 中指定的参数初始化 GPIOx 外围设备
	GPIO_SetBits(GPIOB, GPIO_Pin_12);

	//配置AFIO
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);//配置AFIO外部终端引脚选择
	
	
	//配置EXTI
	EXTI_InitTypeDef EXTI_InitStructure;//EXTI 初始化结构体
	EXTI_InitStructure.EXTI_Line = EXTI_Line14;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; //中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿触发
	EXTI_Init(&EXTI_InitStructure); //EXTI初始化
	
	//配置NVIC
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//配置优先级分组，整个工程只需要执行一次，可以放到main开始函数里。
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//启用中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //配置抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //配置响应优先级
	NVIC_Init(&NVIC_InitStructure);
	
}

uint16_t CountSensor_Get(void)
{
	return CountSensor_Count;
}

void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line14) == SET)
	{
		//判断引脚电平，以避免抖动
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0)
		{
			CountSensor_Count++;
		}
		
		EXTI_ClearITPendingBit(EXTI_Line14);
	}
	
}


