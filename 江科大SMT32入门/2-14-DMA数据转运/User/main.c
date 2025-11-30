#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "MyDMA.h"


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

uint8_t DataA[] = {0x01, 0x02, 0x03, 0x04}; // 外设站点
uint8_t DataB[] = {0, 0, 0, 0};				// 存储器站点
	
int main (void)
{
	uint8_t i;
	OLED_Init();
	MyDMA_Init((uint32_t)DataA,(uint32_t)DataB,4);
	OLED_ShowString(1, 1, "DateA:");
	OLED_ShowHexNum(1, 7, (uint32_t)DataA, 8);
	OLED_ShowString(3, 1, "Date8:");
	OLED_ShowHexNum(3, 7, (uint32_t)DataB, 8);
	
	
	while(1)
	{
		for(i = 0; i<4; i++)
		{
			DataA[i]++;
		}
		for(i = 0; i<4; i++)
		{
			OLED_ShowHexNum(2, 3*i+1, DataA[i], 2);
		}
		for(i = 0; i<4; i++)
		{
			OLED_ShowHexNum(4, 3*i+1, DataB[i], 2);
		}		
		Delay_ms(1000);
		MyDMA_Transfer();
		for(i = 0; i<4; i++)
		{
			OLED_ShowHexNum(2, 3*i+1, DataA[i], 2);
		}
		for(i = 0; i<4; i++)
		{
			OLED_ShowHexNum(4, 3*i+1, DataB[i], 2);
		}
		Delay_ms(1000);
		
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
Timer编码器接口常用库函数:
void TIM_EncoderInterfaceConfig(TIM_TypeDef* TIMx, uint16_t TIM_EncoderMode,
                                uint16_t TIM_IC1Polarity, uint16_t TIM_IC2Polarity);
**********************************************************************************
ADC常用库函数:

void RCC_ADCCLKConfig(uint32_t RCC_PCLK2);
void ADC_DeInit(ADC_TypeDef* ADCx);
void ADC_Init(ADC_TypeDef* ADCx, ADC_InitTypeDef* ADC_InitStruct);
void ADC_StructInit(ADC_InitTypeDef* ADC_InitStruct);
void ADC_Cmd(ADC_TypeDef* ADCx, FunctionalState NewState);
void ADC_DMACmd(ADC_TypeDef* ADCx, FunctionalState NewState);
void ADC_ITConfig(ADC_TypeDef* ADCx, uint16_t ADC_IT, FunctionalState NewState);
void ADC_ResetCalibration(ADC_TypeDef* ADCx);
FlagStatus ADC_GetResetCalibrationStatus(ADC_TypeDef* ADCx);
void ADC_StartCalibration(ADC_TypeDef* ADCx);
FlagStatus ADC_GetCalibrationStatus(ADC_TypeDef* ADCx);
void ADC_SoftwareStartConvCmd(ADC_TypeDef* ADCx, FunctionalState NewState);
void ADC_RegularChannelConfig(ADC_TypeDef* ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime);
void ADC_ExternalTrigConvCmd(ADC_TypeDef* ADCx, FunctionalState NewState);
uint16_t ADC_GetConversionValue(ADC_TypeDef* ADCx);
FlagStatus ADC_GetFlagStatus(ADC_TypeDef* ADCx, uint8_t ADC_FLAG);
**********************************************************************************
DMA常用库函数:
void DMA_Init(DMA_Channel_TypeDef* DMAy_Channelx, DMA_InitTypeDef* DMA_InitStruct);
void DMA_StructInit(DMA_InitTypeDef* DMA_InitStruct);
void DMA_Cmd(DMA_Channel_TypeDef* DMAy_Channelx, FunctionalState NewState);
void DMA_ITConfig(DMA_Channel_TypeDef* DMAy_Channelx, uint32_t DMA_IT, FunctionalState NewState);
void DMA_SetCurrDataCounter(DMA_Channel_TypeDef* DMAy_Channelx, uint16_t DataNumber); 
uint16_t DMA_GetCurrDataCounter(DMA_Channel_TypeDef* DMAy_Channelx);
FlagStatus DMA_GetFlagStatus(uint32_t DMAy_FLAG);
void DMA_ClearFlag(uint32_t DMAy_FLAG);
ITStatus DMA_GetITStatus(uint32_t DMAy_IT);
void DMA_ClearITPendingBit(uint32_t DMAy_IT);

**********************************************************************************
*/
















