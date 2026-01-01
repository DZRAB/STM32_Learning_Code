#include "stm32f10x.h"
#include "Delay.h"
#include "LED.h"
#include "Key.h"
#include "si2c.h"
#include "oled.h"

uint8_t comd[]={0x00,0x8d,0x14,0xaf,0xa5};
uint8_t buffer = 0;
OLED_TypeDef oled;
SI2C_TypeDef si2c;


void MT_OLEDScreen_Init(void);
int i2c_write_bytes(uint8_t addr, const uint8_t *pdata, uint16_t size);

int main (void)
{
	LED_Init();
	Key_Init();
	
	si2c.SCL_GPIOx = GPIOB;
	si2c.SCL_GPIO_Pin = GPIO_Pin_8;
	si2c.SDA_GPIOx  = GPIOB;
	si2c.SDA_GPIO_Pin = GPIO_Pin_9;
	My_SI2C_Init(&si2c);
	
	MT_OLEDScreen_Init();
	OLED_SetPen(&oled,PEN_COLOR_WHITE,1);
	OLED_SetBrush(&oled,BRUSH_TRANSPARENT);
	Delay_ms(100);
	OLED_SetCursor(&oled,24,50);
	OLED_DrawString(&oled, "hello world");
	
	OLED_SendBuffer(&oled);
	while(1)
	{
	}
}

int i2c_write_bytes(uint8_t addr, const uint8_t *pdata, uint16_t size)
	// i2c写数据回调函数
{
	return My_SI2C_SendBytes(&si2c,addr,pdata,size);
}


void MT_OLEDScreen_Init(void)
{
	OLED_InitTypeDef OLED_InitStruct;
	OLED_InitStruct.i2c_write_cb = i2c_write_bytes;
	OLED_Init(&oled, &OLED_InitStruct);
}

