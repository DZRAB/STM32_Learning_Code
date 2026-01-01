#include "stm32f10x.h"
#include "Delay.h"
#include "LED.h"
#include "Key.h"
#include "SoftwareI2C.h"

uint8_t comd[]={0x00,0x8d,0x14,0xaf,0xa5};
uint8_t buffer = 0;

int main (void){

	LED_Init();
	Key_Init();
	SoftwareI2C_Init();
	Delay_ms(100);
	SoftwareI2C_SendBytes(0x78, comd, 5);
//	SoftwareI2C_ReceiveBytes(0x78, &buffer, 1);
	while(1)
	{
//		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) == 0)
//		{
//			Delay_ms(20);
//			SoftwareI2C_SendBytes(0x78, comd, 5);
//			Delay_ms(100);
//		}
//		SoftwareI2C_ReceiveBytes(0x78, &buffer, 1);

//		
//		if(((buffer>>6) & 0x01) == 0)
//		{
//			GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);
//			Delay_ms(1000);
//			GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);	
//			Delay_ms(1000);

//		}
//		else
//		{
//			GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);
//			Delay_ms(100);
//			GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);		
//			Delay_ms(200);

//		}
	}
}
