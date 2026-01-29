#include "HC-SR04.h"                  // Device header


void HC_SR04_Init(void)
{
	TIM1_Init();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); 
	GPIO_InitTypeDef GPIO_IintStructure;
	GPIO_IintStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_IintStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_IintStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA,&GPIO_IintStructure);

}
