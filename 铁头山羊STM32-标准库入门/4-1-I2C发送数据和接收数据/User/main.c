#include "stm32f10x.h"
#include "Delay.h"
#include "LED.h"
#include "Key.h"
#include "I2C1.h"

uint8_t comd[]={0x00,0x8d,0x14,0xaf,0xa5};
int main (void){

	LED_Init();
	Key_Init();
	I2C1_RemapInit();
	I2C1_SendByte(0x78,comd,5);
	while(1)
	{
		
	}
}
