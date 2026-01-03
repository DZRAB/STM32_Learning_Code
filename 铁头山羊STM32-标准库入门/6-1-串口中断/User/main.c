#include "stm32f10x.h"
#include "Delay.h"
#include "LED.h"
#include "Key.h"
#include "USART1.h"

uint16_t Sec=1000;

int main (void){

	LED_Init();
	Key_Init();
	USART1_DefaultInit();
	while(1)
	{
		GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);
		Delay_ms(Sec);
		GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);
		Delay_ms(Sec);
	}
}

void USART1_IRQHandler(void)
{
	uint8_t UART_ReceiveByte;
	
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
	{
		UART_ReceiveByte = USART_ReceiveData(USART1);
		if(UART_ReceiveByte == '0')
		{
			Sec = 1000;
		}
		else if(UART_ReceiveByte == '1')
		{
			Sec = 500; 
		}
		else if(UART_ReceiveByte == '2') 	
		{ 	
			Sec = 100;  		
		} 	
		else if(UART_ReceiveByte == '3') 	
		{ 	
			Sec = 1;  		
		} 
	}		
}
