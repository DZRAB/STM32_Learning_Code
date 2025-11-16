#include "stm32f10x.h"

int main (void){
	//使用寄存器点亮核心板的LED，端口是PC13，低电平点亮
//	RCC->APB2ENR = 0x00000010;
//	GPIOC->CRH = 0x00300000;
//	GPIOC->ODR = 0x00002000;
	
	//使用库函数点亮核心板的LED，端口是PC13，低电平点亮
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//启用APB2外设GPIOC的时钟
	
	
	GPIO_InitTypeDef GPIO_InitStructure; //GPIO 初始化结构体
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure); //根据 GPIO_InitStruct 中指定的参数初始化 GPIOx 外围设备
	
	GPIO_SetBits(GPIOC,GPIO_Pin_13);
	GPIO_ResetBits(GPIOC,GPIO_Pin_13);
	while(1){
		
	
	}
}
