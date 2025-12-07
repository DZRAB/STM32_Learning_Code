#include "W25Q64.h"
#include "W25Q64_Reg.h"

void W25Q64_Init(void)
{
	SoftwareSPI_Init();
}

void W25Q64_ReadID(uint8_t *MID,uint16_t *DID)
{
	SoftwareSPI_Start();
	SoftwareSPI_SwapByte(W25Q64_JEDEC_ID);
	*MID = SoftwareSPI_SwapByte(W25Q64_DUMMY_BYTE);
	*DID = SoftwareSPI_SwapByte(W25Q64_DUMMY_BYTE);
	*DID = *DID << 8;
	*DID = *DID | SoftwareSPI_SwapByte(W25Q64_DUMMY_BYTE);
	SoftwareSPI_Stop();
}

void W25Q64_WriteEnable(void)
{
	SoftwareSPI_Start();
	SoftwareSPI_SwapByte(W25Q64_WRITE_ENABLE);
	SoftwareSPI_Stop();
}

void W25Q64_WaitBusy(void)
{
	uint32_t Timeout = 100000;
	SoftwareSPI_Start();
	SoftwareSPI_SwapByte(W25Q64_READ_STATUS_REGISTER_1);
	while((SoftwareSPI_SwapByte(W25Q64_DUMMY_BYTE) & 0x01) == 1) // 判断BUSY状态
	{
		Timeout--;
		if(Timeout == 0)
		{
			break;
		}
	}
	SoftwareSPI_Stop();
}
//Count最大值256，一次只能写256个数据
void W25Q64_PageProgram(uint32_t Address, uint8_t *DataArray, uint16_t Count)
{
	W25Q64_WaitBusy();
	W25Q64_WriteEnable();
	uint16_t i;
	SoftwareSPI_Start();
	SoftwareSPI_SwapByte(W25Q64_PAGE_PROGRAM);
	SoftwareSPI_SwapByte(Address>>16);
	SoftwareSPI_SwapByte(Address>>8);
	SoftwareSPI_SwapByte(Address);
	for(i=0;i<Count;i++)
	{
		SoftwareSPI_SwapByte(DataArray[i]);
	}
	
	SoftwareSPI_Stop();	
}

void W25Q64_SectorErase4KB(uint32_t Address)
{
	W25Q64_WaitBusy();
	W25Q64_WriteEnable();
	SoftwareSPI_Start();
	SoftwareSPI_SwapByte(W25Q64_SECTOR_ERASE_4KB);
	SoftwareSPI_SwapByte(Address>>16);
	SoftwareSPI_SwapByte(Address>>8);
	SoftwareSPI_SwapByte(Address);
	SoftwareSPI_Stop();

}
//读数据Count无大小限制
void W25Q64_ReadData(uint32_t Address, uint8_t *DataArray, uint32_t Count)
{
	W25Q64_WaitBusy();
	uint32_t i;
	SoftwareSPI_Start();
	SoftwareSPI_SwapByte(W25Q64_READ_DATA);
	SoftwareSPI_SwapByte(Address>>16);
	SoftwareSPI_SwapByte(Address>>8);
	SoftwareSPI_SwapByte(Address);
	for(i=0;i<Count;i++)
	{
		DataArray[i] = SoftwareSPI_SwapByte(W25Q64_DUMMY_BYTE);
	}
	SoftwareSPI_Stop();
}

