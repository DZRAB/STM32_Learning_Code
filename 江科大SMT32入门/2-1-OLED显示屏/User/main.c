#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"

/*

*使用库函数-OLED屏幕做调试工具
*使用I2C通讯的0.96寸OLED屏幕
*使用软件模拟I2C，使用的端口PB8, PB9

*/


int main (void)
{
	OLED_Init();
	OLED_Clear();
	
	while(1)
	{
		OLED_ShowChar(1, 1, 'A');
		OLED_ShowString(1, 3, "HelloWorld!");
		OLED_ShowNum(2, 1, 123, 3);
		OLED_ShowSignedNum(2, 5, -123, 3);
		OLED_ShowString(3, 1, "0x");
		OLED_ShowHexNum(3, 3, 0xabcd, 4);
		OLED_ShowString(3, 8, "= BIN:");
		OLED_ShowBinNum(4, 1, 0xabcd, 16);
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
















