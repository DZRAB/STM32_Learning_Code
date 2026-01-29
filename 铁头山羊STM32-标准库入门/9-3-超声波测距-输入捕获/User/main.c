#include "stm32f10x.h"
#include "Delay.h"
#include "HC-SR04.h"
#include "USART1.h"

int main (void){
	HC_SR04_Init();
	USART1_DefaultInit();
			
	while(1){
		USART1_SendString((uint8_t*)"HC-SR04 TEST\r\n");
		
		TIM_SetCounter(TIM1, 0);
		TIM_ClearFlag(TIM1, TIM_FLAG_CC1);
		TIM_ClearFlag(TIM1, TIM_FLAG_CC2);
		
		TIM_Cmd(TIM1, ENABLE);
		
		GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_SET);
		Delay_us(10);
		GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_RESET);

		while(TIM_GetFlagStatus(TIM1,TIM_FLAG_CC1) == RESET);
		while(TIM_GetFlagStatus(TIM1,TIM_FLAG_CC2) == RESET);
		
		uint16_t ccr1 = TIM_GetCapture1(TIM1);
		uint16_t ccr2 = TIM_GetCapture2(TIM1);
		
		TIM_Cmd(TIM1, DISABLE);	
		 
		float distance = (ccr2 - ccr1) * 1.0e-6f * 340.0f / 2;
		printf("distance: %.3fM \n", distance);
		
		Delay_s(1);
	}
}


//void TIM1_UP_IRQHandler(void)
//{
//	if(TIM_GetFlagStatus(TIM1, TIM_FLAG_Update) == SET)
//	{
//		TIM_ClearFlag(TIM1, TIM_FLAG_Update);
//	}
//}
