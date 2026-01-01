#include "stm32f10x.h"
#include "Delay.h"
#include "LED.h"
#include "Key.h"
#include "I2C1.h"

uint8_t comd[]={0x00,0x8d,0x14,0xaf,0xa5};
uint8_t buffer = 0;

int main (void){

	LED_Init();
	Key_Init();
	I2C1_RemapInit();
	Delay_ms(100);
	I2C1_SendByte(0x78,comd,5);
	Delay_ms(100);
	I2C1_ReceiveByte(0x78,&buffer,1);
	if(((buffer>>6) & 0x01) == 0)
	{
		GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);
	}
	else
	{
		GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);
	}
	while(1)
	{
		
	}
}
