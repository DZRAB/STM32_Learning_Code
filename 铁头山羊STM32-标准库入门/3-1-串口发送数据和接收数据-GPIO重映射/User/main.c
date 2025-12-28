#include "stm32f10x.h"
#include "Delay.h"
#include "LED.h"
#include "Key.h"
#include "USART.h"

uint8_t PN532_WakeUpCmd[] = {0x55 ,0x55 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0xFF ,0x03 ,0xFD ,0xD4 ,0x14 ,0x01 ,0x17 ,0x00};
uint8_t PN532_SearchCardCmd[] = {0x00 ,0x00 ,0xFF ,0x04 ,0xFC ,0xD4 ,0x4A ,0x01 ,0x00 ,0xE1 ,0x00};

int main (void){

	LED_Init();
	Key_Init();
	USART1_DefaultInit();
	
	USART1_SendBytes(PN532_WakeUpCmd,sizeof(PN532_WakeUpCmd)/sizeof(PN532_WakeUpCmd[0]));
	Delay_ms(200);
	USART1_SendBytes(PN532_SearchCardCmd,sizeof(PN532_SearchCardCmd)/sizeof(PN532_SearchCardCmd[0]));
		printf("HelloWorld!\r\n");
		printf("1+1=%d", 1+1);

	while(1){

		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) == 0)
		{
			Delay_ms(20);
			GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);
		}
		else
		{
			GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);
			Delay_ms(20);	
		}

	}
}
