#include "stm32f10x.h"
#include "Delay.h"

/*

*使用库函数点亮LED，端口是PA0，低电平点亮
*四种GPIO写函数的用法

*/
int main (void){
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//启用APB2外设GPIOA的时钟
	
	
	GPIO_InitTypeDef GPIO_InitStructure; //GPIO 初始化结构体
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure); //根据 GPIO_InitStruct 中指定的参数初始化 GPIOx 外围设备
	
	
	while(1){
		
		//指定端口设置高电平-三种函数用法
		//GPIO_SetBits(GPIOA,GPIO_Pin_0); //直接设置指定端口为1高电平
		//GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_SET); //通过第三个参数来设定端口的状态，Bit_SET为1高电平
		GPIO_Write(GPIOA,0x0001); //可以同时设置整组端口的电平，0x0001二进制:0000 0000 0000 0001，最低位也就是PA0为1高电平
		Delay_ms(100);
		
		//指定端口设置低电平-三种函数用法
		//GPIO_ResetBits(GPIOA,GPIO_Pin_0); //直接设置指定端口为0低电平
		//GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_RESET);//通过第三个参数来设定端口的状态，Bit_RESET1低电平
		GPIO_Write(GPIOA,0x00);//可以同时设置整组端口的电平，0x0001二进制:0000 0000 0000 0000，最低位也就是PA0为0低电平
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
















