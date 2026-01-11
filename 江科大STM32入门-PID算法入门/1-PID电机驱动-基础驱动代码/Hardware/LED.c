#include "LED.h"

/*LED-GPIO初始化*/
void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); //打开时钟
	
	GPIO_InitTypeDef GPIO_InitStructure; //初始化结构体
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;//开漏输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//LED端口号
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//GPIO输出最大速度
	GPIO_Init(GPIOC,&GPIO_InitStructure);//GPIO初始化端口C
	
	GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);//默认关灯
}

void LED_ON(void)
{
	GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);
}

void LED_OFF(void)
{
	GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);
}

