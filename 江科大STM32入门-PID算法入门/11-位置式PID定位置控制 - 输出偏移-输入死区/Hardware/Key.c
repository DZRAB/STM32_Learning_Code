#include "Key.h"
#include "Delay.h"

uint8_t Key_Num = 0;

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//启用APB2外设GPIOx的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//启用APB2外设GPIOx的时钟
	
	GPIO_InitTypeDef GPIO_InitStructure; //GPIO 初始化结构体
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //上拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure); //根据 GPIO_InitStruct 中指定的参数初始化 GPIOx 外围设备

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //上拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure); //根据 GPIO_InitStruct 中指定的参数初始化 GPIOx 外围设备

}

uint8_t Key_GetNum(void)
{
	uint8_t Temp = Key_Num;
	Key_Num = 0;
	return Temp;
}

uint8_t Key_GetState(void)
{
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 0)
	{
		return 1;
	}
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0)
	{
		return 2;
	}
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 0)
	{
		return 3;
	}
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12) == 0)
	{
		return 4;
	}
	return 0;
}

void Key_Tick(void)
{
	static uint8_t Count=0;
	static uint8_t CurrState=0, PrevState=0;
	Count++;
	if(Count >= 20)
	{
		Count = 0;
		PrevState = CurrState;
		CurrState = Key_GetState();
		if(CurrState == 0 && PrevState != 0)
		{
			Key_Num = PrevState;
		}
	}
}
