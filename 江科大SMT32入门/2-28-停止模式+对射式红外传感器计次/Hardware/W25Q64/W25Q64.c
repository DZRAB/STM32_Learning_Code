#include "W25Q64.h"
#include "W25Q64_Reg.h"


void W25Q64_Init(void)
{
	HardwareSPI_Init();
}

void W25Q64_ReadID(uint8_t *MID,uint16_t *DID)
{
	HardwareSPI_Start();
	HardwareSPI_SwapByte(W25Q64_JEDEC_ID);
	*MID = HardwareSPI_SwapByte(W25Q64_DUMMY_BYTE);
	*DID = HardwareSPI_SwapByte(W25Q64_DUMMY_BYTE);
	*DID = *DID << 8;
	*DID = *DID | HardwareSPI_SwapByte(W25Q64_DUMMY_BYTE);
	HardwareSPI_Stop();
}

void W25Q64_WriteEnable(void)
{
	HardwareSPI_Start();
	HardwareSPI_SwapByte(W25Q64_WRITE_ENABLE);
	HardwareSPI_Stop();
}

void W25Q64_WaitBusy(void)
{
	uint32_t Timeout = 100000;
	HardwareSPI_Start();
	HardwareSPI_SwapByte(W25Q64_READ_STATUS_REGISTER_1);
	while((HardwareSPI_SwapByte(W25Q64_DUMMY_BYTE) & 0x01) == 1) // 判断BUSY状态
	{
		Timeout--;
		if(Timeout == 0)
		{
			break;
		}
	}
	HardwareSPI_Stop();
}
//Count最大值256，一次只能写256个数据
void W25Q64_PageProgram(uint32_t Address, uint8_t *DataArray, uint16_t Count)
{
	W25Q64_WaitBusy();
	W25Q64_WriteEnable();
	uint16_t i;
	HardwareSPI_Start();
	HardwareSPI_SwapByte(W25Q64_PAGE_PROGRAM);
	HardwareSPI_SwapByte(Address>>16);
	HardwareSPI_SwapByte(Address>>8);
	HardwareSPI_SwapByte(Address);
	for(i=0;i<Count;i++)
	{
		HardwareSPI_SwapByte(DataArray[i]);
	}
	
	HardwareSPI_Stop();	
}

void W25Q64_SectorErase4KB(uint32_t Address)
{
	W25Q64_WaitBusy();
	W25Q64_WriteEnable();
	HardwareSPI_Start();
	HardwareSPI_SwapByte(W25Q64_SECTOR_ERASE_4KB);
	HardwareSPI_SwapByte(Address>>16);
	HardwareSPI_SwapByte(Address>>8);
	HardwareSPI_SwapByte(Address);
	HardwareSPI_Stop();

}
//读数据Count无大小限制
void W25Q64_ReadData(uint32_t Address, uint8_t *DataArray, uint32_t Count)
{
	W25Q64_WaitBusy();
	uint32_t i;
	HardwareSPI_Start();
	HardwareSPI_SwapByte(W25Q64_READ_DATA);
	HardwareSPI_SwapByte(Address>>16);
	HardwareSPI_SwapByte(Address>>8);
	HardwareSPI_SwapByte(Address);
	for(i=0;i<Count;i++)
	{
		DataArray[i] = HardwareSPI_SwapByte(W25Q64_DUMMY_BYTE);
	}
	HardwareSPI_Stop();
}

