#include "Key.h"

/*按键-EXIT初始化*/
void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //打开时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); //打开时钟
	
	GPIO_InitTypeDef GPIO_InitStructure; //初始化结构体
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_11;//Key端口号
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//输入模式速度无效
	GPIO_Init(GPIOB,&GPIO_InitStructure);//GPIO初始化端口B
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
	EXTI_InitTypeDef EXTI_Structure;
	EXTI_Structure.EXTI_Line = EXTI_Line11;
	EXTI_Structure.EXTI_LineCmd = ENABLE;
	EXTI_Structure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_Structure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_Structure);
	
	EXTI_Structure.EXTI_Line = EXTI_Line1;
	EXTI_Structure.EXTI_LineCmd = ENABLE;
	EXTI_Structure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_Structure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_Structure);	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_Sturcture;
	NVIC_Sturcture.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_Sturcture.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Sturcture.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Sturcture.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_Sturcture);
	
	NVIC_Sturcture.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_Sturcture.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Sturcture.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Sturcture.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_Sturcture);	
	
	}
