#include "OLED.h"

void OLED_Init(void)
{
	SoftwareI2C_Init();
	Delay_ms(100);
	OLED_WriteCommand(0xAE);
	
	OLED_WriteCommand(0xD5);
	OLED_WriteCommand(0x80);
	
	OLED_WriteCommand(0xD3);
	OLED_WriteCommand(0x00);

	OLED_WriteCommand(0x40);
	
	OLED_WriteCommand(0xA1);
	
	OLED_WriteCommand(0xC8);

	OLED_WriteCommand(0xDA);
	OLED_WriteCommand(0x12);
	
	OLED_WriteCommand(0x81);
	OLED_WriteCommand(0xCF);

	OLED_WriteCommand(0xD9);
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDB);
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0xA4);
	
	OLED_WriteCommand(0xA6);

	OLED_WriteCommand(0x8D);
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF);

	Delay_ms(100);
	OLED_Clear();
}

void OLED_WriteCommand(uint8_t Command)
{
	SoftwareI2C_Start();
	SoftwareI2C_SendByte(0x78); //寻址
	SoftwareI2C_ReceiveACK();
	SoftwareI2C_SendByte(0x00); //发送Control Byte
	SoftwareI2C_ReceiveACK();
	SoftwareI2C_SendByte(Command); //发送Date Byte
	SoftwareI2C_ReceiveACK();
	SoftwareI2C_Stop();
}

void OLED_WriteData(uint8_t Data)
{
	SoftwareI2C_Start();
	SoftwareI2C_SendByte(0x78); //寻址
	SoftwareI2C_ReceiveACK();
	SoftwareI2C_SendByte(0x40); //发送Control Byte
	SoftwareI2C_ReceiveACK();
	SoftwareI2C_SendByte(Data); //发送Date Byte
	SoftwareI2C_ReceiveACK();
	SoftwareI2C_Stop();
}

//显示起始坐标
void OLED_SetCursor(uint8_t X, uint8_t Page)
{
	OLED_WriteCommand(0x00 | (X & 0x0F));
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));
	OLED_WriteCommand(0xB0 | Page);

}

void OLED_Clear(void)
{
	uint8_t i,j;
	for(i=0;i<8;i++)
	{
		OLED_SetCursor(0,i);
		for(j=0;j<128;j++)
		{
			OLED_WriteData(0x00);
		}
	}
}

void OLED_ShowChar(uint8_t X, uint8_t Page, char Char, uint8_t FontSize)
{
	uint8_t i;
	if(FontSize == 8)
	{
		OLED_SetCursor(X, Page);
		for(i=0;i<8;i++)
		{
			OLED_WriteData(OLED_F8x16[Char-' '][i]);
		}
		OLED_SetCursor(X, Page+1);
		for(i=8;i<16;i++)
		{
			OLED_WriteData(OLED_F8x16[Char-' '][i]);
		}
	}
	else if(FontSize == 6)
	{
		OLED_SetCursor(X, Page);
		for(i=0;i<6;i++)
		{
			OLED_WriteData(OLED_F6x8[Char-' '][i]);
		}
	}	
}

void OLED_ShowString(uint8_t X, uint8_t Page, char *String, uint8_t FontSize)
{
	uint8_t i;
	for(i=0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(X+i*FontSize,Page,String[i],FontSize);
	}
	
}

void OLED_ShowNum(uint8_t X, uint32_t Number, uint8_t Length, uint8_t FontSize);
void OLED_ShowSignedNum(uint8_t X, int32_t Number, uint8_t Length, uint8_t FontSize);
void OLED_ShowHexNum(uint8_t X, uint32_t Number, uint8_t Length, uint8_t FontSize);
void OLED_ShowBinNum(uint8_t X, uint32_t Number, uint8_t Length, uint8_t FontSize);
void OLED_ShowFloatNum(uint8_t X, double Number, uint8_t IntLength, uint8_t FraLength, uint8_t FontSize);

void OLED_ShowChinese(uint8_t X, uint8_t Page, char *Chinese)
{
	char SigleChinese[OLED_CHN_CHAR_WIDTH + 1]={0};
	uint8_t pChinese = 0;
	uint8_t pIndex;
	uint8_t i;
	for(i=0; Chinese[i] != '\0'; i++)
	{
		SigleChinese[pChinese] = Chinese[i];
		pChinese++;
		if(pChinese>=OLED_CHN_CHAR_WIDTH)
		{
			pChinese = 0;
			for(pIndex=0;strcmp(OLED_CF16x16[pIndex].Index,"") != 0;pIndex++)
			{
				if(strcmp(OLED_CF16x16[pIndex].Index, SigleChinese) == 0)
				{
					break;
				}
			}
			OLED_ShowImage(X+((i+1)/OLED_CHN_CHAR_WIDTH-1)*16,Page,16,2,OLED_CF16x16[pIndex].Data);
		}
	}
}

void OLED_ShowImage(uint8_t X, uint8_t Page, uint8_t Width, uint8_t Height, const uint8_t *Image)
{
	uint8_t i,j;
	for(j = 0; j<Height; j++)
	{
		OLED_SetCursor(X, Page + j);
		for(i=0;i<Width;i++)
		{
			OLED_WriteData(Image[Width*j+i]);
		}
	}
}

void OLED_Printf(uint8_t X, uint8_t FontSize, char *format, ...);




