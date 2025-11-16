#include "stm32f10x.h"
#include "Delay.h"

/*

*使用库函数控制有源蜂鸣器，端口是PB12，低电平有效

*/
int main (void){
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//启用APB2外设GPIOB的时钟
	
	
	GPIO_InitTypeDef GPIO_InitStructure; //GPIO 初始化结构体
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure); //根据 GPIO_InitStruct 中指定的参数初始化 GPIOx 外围设备
	
	
	while(1){
		
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);
		Delay_ms(100);
		GPIO_SetBits(GPIOB,GPIO_Pin_12);
		Delay_ms(100);
		
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);
		Delay_ms(100);
		GPIO_SetBits(GPIOB,GPIO_Pin_12);
		Delay_ms(700);
		
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);
		Delay_ms(100);
		GPIO_SetBits(GPIOB,GPIO_Pin_12);
		Delay_ms(100);
	}
}


/*
**********************************************************************************
RCC常用库函数:

//启用或禁用 AHB 外设时钟
void RCC_AHBPeriphClockCmd(uint32_t RCC_AHBPeriph, FunctionalState NewState);

//启用或禁用 高速AHB2 外设时钟
void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);

启用或禁用 低速AHB1 外设时钟
void RCC_APB1PeriphClockCmd(uint32_t RCC_APB1Periph, FunctionalState NewState);
**********************************************************************************
GPIO常用库函数:

//根据 GPIO_InitStruct 中指定的参数初始化 GPIOx 外围设备
void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);

//读取指定的输入端口引脚
uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
//读取指定的 GPIO 输入数据端口
uint16_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx);
//读取指定的输出数据端口的位值
uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
//读取指定的 GPIO 输出数据端口
uint16_t GPIO_ReadOutputData(GPIO_TypeDef* GPIOx);

//设置所选的数据端口位
void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
//清除所选的数据端口位
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
//设置或清除选定的数据端口位
void GPIO_WriteBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BitAction BitVal);
//将数据写入指定的 GPIO 数据端口
void GPIO_Write(GPIO_TypeDef* GPIOx, uint16_t PortVal);
**********************************************************************************
*/
















