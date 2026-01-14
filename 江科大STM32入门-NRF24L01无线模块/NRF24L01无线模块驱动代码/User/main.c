#include "stm32f10x.h"
#include "NRF24L01.h"  
#include "OLED.h"  
#include "Delay.h" 
#include "Key.h" 

uint8_t KeyNum;

int main(void)
{
	OLED_Init();
	NRF24L01_Init();
	Key_Init();
	
	while(1){
		KeyNum = Key_GetNum();
		
		if(KeyNum == 1)
		{
			NRF24L01_TXPacket[0]++;
			NRF24L01_TXPacket[1]+=2;
			NRF24L01_TXPacket[2]+=3;
			NRF24L01_TXPacket[3]+=4;
			NRF24L01_Send();
		}
		
		if(NRF24L01_Receive() == 1)
		{
			OLED_ShowHexNum(1,1,NRF24L01_RXPacket[0],2);
			OLED_ShowHexNum(1,4,NRF24L01_RXPacket[1],2);
			OLED_ShowHexNum(1,7,NRF24L01_RXPacket[2],2);
			OLED_ShowHexNum(1,10,NRF24L01_RXPacket[3],2);
		}
	
	}
}
