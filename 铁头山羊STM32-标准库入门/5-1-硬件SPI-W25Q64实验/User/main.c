#include "stm32f10x.h"
#include "Delay.h"
#include "LED.h"
#include "Key.h"
#include "W25Q64.h"
uint8_t redaByte = 0;
int main (void){

	LED_Init();
	Key_Init();
	SPI1_Init();
	while(1)
	{
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)  == 0)
		{
			Delay_ms(10);
			W25Q64_SaveByte(0x79);
			
		}
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)  == 0)
		{
			Delay_ms(10);
			W25Q64_SaveByte(0x78);
		
		}
		
		redaByte = W25Q64_ReadByte();
		if(redaByte == 0x78)
		{
			GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);

		}
		else if(redaByte == 0x79)
		{
			GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);
		
		}
		
	}
}
