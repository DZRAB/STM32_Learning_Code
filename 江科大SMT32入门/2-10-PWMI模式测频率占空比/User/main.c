#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "Key.h"
#include "PWM.h"
#include "InputCapture.h"

/*

*使用库函数-定时器定时中断

*/

/*	OLED显示函数使用示例：
	OLED_ShowChar(1, 1, 'A');
	OLED_ShowString(1, 3, "HelloWorld!");
	OLED_ShowNum(2, 1, 123, 3);
	OLED_ShowSignedNum(2, 5, -123, 3);
	OLED_ShowString(3, 1, "0x");
	OLED_ShowHexNum(3, 3, 0xabcd, 4);
	OLED_ShowString(3, 8, "= BIN:");
	OLED_ShowBinNum(4, 1, 0xabcd, 16);
*/


uint32_t Freq, Duty;

int main (void)
{
	OLED_Init();
	Key_Init();
	PWM_Init();
	InputCapture_Init();
	OLED_ShowString(1, 1, "Input Capture:");
	OLED_ShowString(2, 1, "Freq:");
	OLED_ShowString(2, 11, "Hz");
	OLED_ShowString(3, 1, "Duty:");
	OLED_ShowString(3, 8, "%");
	
	PWM_SetCompare1(60);// 占空比
	PWM_SetPrescaler(720-1); // PWM频率：	Freq = 720 000 / (PSC + 1)
	
	while(1)
	{	
		Freq = InputCapture_GetFreq();
		Duty = InputCapture_GetDuty();
		Delay_ms(20);
		OLED_ShowNum(2, 6, Freq, 5);
		OLED_ShowNum(3, 6, Duty, 2);
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

//更改指定引脚的映射（AFIO的操作函数）
void GPIO_PinRemapConfig(uint32_t GPIO_Remap, FunctionalState NewState);

//选择用作EXTI线的GPIO引脚（AFIO的操作函数）
void GPIO_EXTILineConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource);

**********************************************************************************
EXTI常用库函数:

//根据 EXTI_InitStruct 中指定的参数初始化 EXTI 外设
void EXTI_Init(EXTI_InitTypeDef* EXTI_InitStruct);

//检查是否设置了指定的EXTI行标志
FlagStatus EXTI_GetFlagStatus(uint32_t EXTI_Line);

//清除EXTI的行挂起标志
void EXTI_ClearFlag(uint32_t EXTI_Line);

//检查指定的EXTI行是否被断言
ITStatus EXTI_GetITStatus(uint32_t EXTI_Line);

//清除EXTI的行挂起位
void EXTI_ClearITPendingBit(uint32_t EXTI_Line);
**********************************************************************************
NVIC常用库函数:

//配置优先级分组：抢占优先级和子优先级（响应优先级）
void NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup);

//根据 NVIC_InitStruct 中指定的参数初始化 NVIC 外设
void NVIC_Init(NVIC_InitTypeDef* NVIC_InitStruct);

**********************************************************************************
Timer常用库函数:

void TIM_TimeBaseInit(TIM_TypeDef* TIMx, TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct);

void TIM_TimeBaseStructInit(TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct);

void TIM_Cmd(TIM_TypeDef* TIMx, FunctionalState NewState);

void TIM_ITConfig(TIM_TypeDef* TIMx, uint16_t TIM_IT, FunctionalState NewState);

void TIM_InternalClockConfig(TIM_TypeDef* TIMx);

void TIM_ITRxExternalClockConfig(TIM_TypeDef* TIMx, uint16_t TIM_InputTriggerSource);

void TIM_TIxExternalClockConfig(TIM_TypeDef* TIMx, uint16_t TIM_TIxExternalCLKSource,
                                uint16_t TIM_ICPolarity, uint16_t ICFilter);
								
void TIM_ETRClockMode1Config(TIM_TypeDef* TIMx, uint16_t TIM_ExtTRGPrescaler, uint16_t TIM_ExtTRGPolarity,
                             uint16_t ExtTRGFilter);
							 
void TIM_ETRClockMode2Config(TIM_TypeDef* TIMx, uint16_t TIM_ExtTRGPrescaler, 
                             uint16_t TIM_ExtTRGPolarity, uint16_t ExtTRGFilter);
							 
void TIM_ETRConfig(TIM_TypeDef* TIMx, uint16_t TIM_ExtTRGPrescaler, uint16_t TIM_ExtTRGPolarity,
                   uint16_t ExtTRGFilter);

void TIM_PrescalerConfig(TIM_TypeDef* TIMx, uint16_t Prescaler, uint16_t TIM_PSCReloadMode);

void TIM_CounterModeConfig(TIM_TypeDef* TIMx, uint16_t TIM_CounterMode);

void TIM_ARRPreloadConfig(TIM_TypeDef* TIMx, FunctionalState NewState);

void TIM_SetCounter(TIM_TypeDef* TIMx, uint16_t Counter);

void TIM_SetAutoreload(TIM_TypeDef* TIMx, uint16_t Autoreload);

uint16_t TIM_GetCounter(TIM_TypeDef* TIMx);

uint16_t TIM_GetPrescaler(TIM_TypeDef* TIMx);

FlagStatus TIM_GetFlagStatus(TIM_TypeDef* TIMx, uint16_t TIM_FLAG);

void TIM_ClearFlag(TIM_TypeDef* TIMx, uint16_t TIM_FLAG);

ITStatus TIM_GetITStatus(TIM_TypeDef* TIMx, uint16_t TIM_IT);

void TIM_ClearITPendingBit(TIM_TypeDef* TIMx, uint16_t TIM_IT);

**********************************************************************************
Timer输出比较-PWM常用库函数:
void TIM_OC1Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC2Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC3Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC4Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);

void TIM_OCStructInit(TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_CtrlPWMOutputs(TIM_TypeDef* TIMx, FunctionalState NewState);
void TIM_PrescalerConfig(TIM_TypeDef* TIMx, uint16_t Prescaler, uint16_t TIM_PSCReloadMode);


void TIM_SetCompare1(TIM_TypeDef* TIMx, uint16_t Compare1);
void TIM_SetCompare2(TIM_TypeDef* TIMx, uint16_t Compare2);
void TIM_SetCompare3(TIM_TypeDef* TIMx, uint16_t Compare3);
void TIM_SetCompare4(TIM_TypeDef* TIMx, uint16_t Compare4);

**********************************************************************************
Timer输入捕获常用库函数:
void TIM_ICInit(TIM_TypeDef* TIMx, TIM_ICInitTypeDef* TIM_ICInitStruct);
void TIM_PWMIConfig(TIM_TypeDef* TIMx, TIM_ICInitTypeDef* TIM_ICInitStruct);
void TIM_ICStructInit(TIM_ICInitTypeDef* TIM_ICInitStruct);
void TIM_SelectInputTrigger(TIM_TypeDef* TIMx, uint16_t TIM_InputTriggerSource);
void TIM_SelectOutputTrigger(TIM_TypeDef* TIMx, uint16_t TIM_TRGOSource);
void TIM_SelectSlaveMode(TIM_TypeDef* TIMx, uint16_t TIM_SlaveMode);
void TIM_SetIC1Prescaler(TIM_TypeDef* TIMx, uint16_t TIM_ICPSC);
void TIM_SetIC2Prescaler(TIM_TypeDef* TIMx, uint16_t TIM_ICPSC);
void TIM_SetIC3Prescaler(TIM_TypeDef* TIMx, uint16_t TIM_ICPSC);
void TIM_SetIC4Prescaler(TIM_TypeDef* TIMx, uint16_t TIM_ICPSC);
uint16_t TIM_GetCapture1(TIM_TypeDef* TIMx);
uint16_t TIM_GetCapture2(TIM_TypeDef* TIMx);
uint16_t TIM_GetCapture3(TIM_TypeDef* TIMx);
uint16_t TIM_GetCapture4(TIM_TypeDef* TIMx);

**********************************************************************************
*/
















