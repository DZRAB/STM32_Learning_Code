#include "Key.h"

uint8_t Key_Flag[KEY_COUNT]; 

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//启用APB2外设GPIOx的时钟
	GPIO_InitTypeDef GPIO_InitStructure; //GPIO 初始化结构体
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //上拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_11; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure); //根据 GPIO_InitStruct 中指定的参数初始化 GPIOx 外围设备

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //下拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_13; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure); //根据 GPIO_InitStruct 中指定的参数初始化 GPIOx 外围设备

}

uint8_t Key_GetState(uint8_t N)
{
	if(N == KEY_1)
	{
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
		{
			return KEY_PRESSED;
		}
	}
	if(N == KEY_2)
	{
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0)
		{
			return KEY_PRESSED;
		}
	}
	if(N == KEY_3)
	{
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 1)
		{
			return KEY_PRESSED;
		}
	}
	if(N == KEY_4)
	{
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 1)
		{
			return KEY_PRESSED;
		}
	}
	return KEY_UNPRESSED;
}

uint8_t Key_Check(uint8_t N, uint8_t Flag)
{
	if(Key_Flag[N] & Flag)
	{
		if(Key_Flag[N] != KEY_HOLD)
		{
			Key_Flag[N] &= ~Flag;
		}
		return 1;
	}
	return 0;
}

void Key_Tick(void)
{
	static uint8_t Count, i;
	static uint8_t CurrState[KEY_COUNT], PrevState[KEY_COUNT];
	static uint8_t S[KEY_COUNT];
	static uint16_t Time[KEY_COUNT];

	Count++;
	
	for(i=0;i<KEY_COUNT;i++)
	{
		if(Time[i] > 0)
		{
			Time[i]--;
		}
	}
	
	if(Count >= 15)
	{
		Count = 0;
		
		for(i=0;i<KEY_COUNT;i++)
		{
			PrevState[i] = CurrState[i];
			CurrState[i] = Key_GetState(i);
			if(CurrState[i] == KEY_PRESSED)
			{
				//HOLD = 1;表示按键按下
				Key_Flag[i] |= KEY_HOLD;
			}
			else
			{
				//HOLD = 0;表示按键未按下
				Key_Flag[i] &= ~KEY_HOLD;
			}
			if(CurrState[i] == KEY_PRESSED && PrevState[i] == KEY_UNPRESSED)
			{
				Key_Flag[i] |= KEY_DOWN;
				//DOWN = 1;表示按键按下的瞬间
			}
			if(CurrState[i] == KEY_UNPRESSED && PrevState[i] == KEY_PRESSED)
			{
				Key_Flag[i] |= KEY_UP;
				//UP = 1;表示按键松开的瞬间
			}

			if(S[i] == 0)
			{
				if(CurrState[i] == KEY_PRESSED)
				{
					//设置长按时间2S
					Time[i] = KEY_TIME_LONG;
					//跳到下个状态
					S[i] = 1;
				}
				
			}
			else if(S[i] == 1)
			{
				if(CurrState[i] == KEY_UNPRESSED)
				{	
					//设定双击时间
					Time[i] = KEY_TIME_DOUBLE;
					S[i] = 2;
				}
				else if(Time[i] == 0) //长按时间到
				{
					//设定重复时间
					Time[i] = KEY_TIME_REPEAT;
					//LONG = 1;
					Key_Flag[i] |= KEY_LONG;
					S[i] = 4;
				}
			}
			else if(S[i] == 2)
			{
				if(CurrState[i] == KEY_PRESSED)
				{
					//DOUBLE = 1;
					Key_Flag[i] |= KEY_DOUBLE;
					//跳到下个状态
					S[i] = 3;
				}
				else if(Time[i] == 0) //双击时间到
				{
					//SINGLE = 1;
					Key_Flag[i] |= KEY_SINGLE;
					S[i] = 0;
				}
			}
			else if(S[i] == 3)
			{
				if(CurrState[i] == KEY_UNPRESSED)
				{	
					S[i] = 0;
				}
			}
			else if(S[i] == 4)
			{
				if(CurrState[i] == KEY_UNPRESSED)
				{	
					S[i] = 0;
				}
				else if(Time[i] == 0)//重复时间到
				{
					//重置重复时间
					Time[i] = KEY_TIME_REPEAT;
					//REPEAT = 1;
					Key_Flag[i] |= KEY_REPEAT;
				}
			}
		}
	}
}
