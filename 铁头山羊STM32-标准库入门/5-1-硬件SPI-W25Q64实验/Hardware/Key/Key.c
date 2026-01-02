#include "Key.h"

/*按键-GPIO初始化*/
void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //打开时钟
	
	GPIO_InitTypeDef GPIO_InitStructure; //初始化结构体
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_11;//Key端口号
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//输入模式速度无效
	GPIO_Init(GPIOB,&GPIO_InitStructure);//GPIO初始化端口B
	}
