#include "stm32f10x.h"
#include "Delay.h"
#include "LED.h"
#include "Key.h"
#include "USART1.h"

uint8_t PN532_WakeUpCmd[] = {0x55 ,0x55 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0xFF ,0x03 ,0xFD ,0xD4 ,0x14 ,0x01 ,0x17 ,0x00};
uint8_t PN532_SearchCardCmd[] = {0x00 ,0x00 ,0xFF ,0x04 ,0xFC ,0xD4 ,0x4A ,0x01 ,0x00 ,0xE1 ,0x00};

int main (void){

	LED_Init();
	Key_Init();
	USART1_DefaultInit();
	
	USART1_SendBytes(PN532_WakeUpCmd,sizeof(PN532_WakeUpCmd)/sizeof(PN532_WakeUpCmd[0]));
	Delay_ms(200);
	USART1_SendBytes(PN532_SearchCardCmd,sizeof(PN532_SearchCardCmd)/sizeof(PN532_SearchCardCmd[0]));
	Delay_ms(200);
	USART1_SendByte(0x41);
	Delay_ms(200);	
	printf("HelloWorld!\r\n");
	Delay_ms(200);	
	printf("1+1=%d", 1+1);
	Delay_ms(200);	
	USART1_SendString((uint8_t*)"TestSendString");
	Delay_ms(200);
	USART1_SendString((uint8_t*)"123456");
	Delay_ms(200);
	while(1){

		if((USART1_ReceiveByte() == '1'))
		{
			GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);
		}
		else if((USART1_ReceiveByte() == '0'))
		{
			GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);
		}

	}
}
