#include "stm32f10x.h"
#include "Delay.h"

/*

*使用库函数的流水灯程序，端口是PA0-PA7，低电平点亮

*/
int main (void){
	
	u8 i;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//启用APB2外设GPIOA的时钟
	
	
	GPIO_InitTypeDef GPIO_InitStructure; //GPIO 初始化结构体
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7; //这里可以通过|来启用多个端口
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure); //根据 GPIO_InitStruct 中指定的参数初始化 GPIOx 外围设备
	
	
	while(1){
		
		for(i = 0;i < 8; i++)
		{
			GPIO_Write(GPIOA,~(0x0001<<i));  //这里使用左移和取反
			Delay_ms(100);
		}
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
















