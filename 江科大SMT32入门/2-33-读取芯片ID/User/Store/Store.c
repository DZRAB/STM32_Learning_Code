#include "Store.h"
/*
使用SRAM缓存数组，管理FLASH的最后一页
*/
uint16_t Store_Data[STORE_COUNT];

void Stroe_Init(void)
{
	//FLASH最后一页的第一个半字当做标志位
	if(MyFlash_ReadHalfWord(STORE_START_ADDRESS) != 0xA5A5)
	{
		MyFlash_ErasePages(STORE_START_ADDRESS);
		MyFlash_ProgramHalfWord(STORE_START_ADDRESS, 0xA5A5);
		for(uint16_t i = 1; i<STORE_COUNT; i++)
		{
			MyFlash_ProgramHalfWord(STORE_START_ADDRESS + i * 2,0x0000);
		}
	}

	for(uint16_t i = 0; i<STORE_COUNT; i++)
	{
		Store_Data[i] = MyFlash_ReadHalfWord(STORE_START_ADDRESS + i * 2);
	}
}

void Store_Save(void)
{
	MyFlash_ErasePages(STORE_START_ADDRESS);
	for(uint16_t i = 0; i<STORE_COUNT; i++)
	{
		MyFlash_ProgramHalfWord(STORE_START_ADDRESS + i * 2,Store_Data[i]);
	}
}

void Store_Clear(void)
{
	for(uint16_t i = 1; i<STORE_COUNT; i++)
	{
		Store_Data[i] = 0x0000;
	}
	
	Store_Save();
}

