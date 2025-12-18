#include "OLED.h"

uint8_t OLED_DisplayBuf[8][128]={0};

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

void OLED_WriteData(uint8_t *Data, uint8_t Count)
{
	uint8_t i;
	SoftwareI2C_Start();
	SoftwareI2C_SendByte(0x78); //寻址
	SoftwareI2C_ReceiveACK();
	SoftwareI2C_SendByte(0x40); //发送Control Byte
	SoftwareI2C_ReceiveACK();
	for(i=0;i<Count;i++)
	{
		SoftwareI2C_SendByte(Data[i]); //发送Date Byte
		SoftwareI2C_ReceiveACK();
	}
	
	SoftwareI2C_Stop();
}

//显示起始坐标
void OLED_SetCursor(uint8_t X, uint8_t Page)
{
	OLED_WriteCommand(0x00 | (X & 0x0F));
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));
	OLED_WriteCommand(0xB0 | Page);

}

void OLED_Updata(void)
{
	uint8_t y;
	for(y=0;y<8;y++)
	{	
		OLED_SetCursor(0,y);
		OLED_WriteData(OLED_DisplayBuf[y],128);
	}
}

void OLED_Clear(void)
{
	for(uint8_t y=0; y<8; y++)
	{
		for(uint8_t x=0; x<128; x++)
		{
			OLED_DisplayBuf[y][x] = 0X00;
		}
	}
}

void OLED_ShowChar(uint8_t X, uint8_t Y, char Char, uint8_t FontSize)
{
	if(FontSize == 8)
	{
		OLED_ShowImage(X,Y,8,16,OLED_F8x16[Char-' ']);
////		OLED_SetCursor(X, Y);
//		for(i=0;i<8;i++)
//		{
////			OLED_WriteData(OLED_F8x16[Char-' '][i]);
//			OLED_DisplayBuf[Y][X+i] = OLED_F8x16[Char-' '][i];
//		}
////		OLED_SetCursor(X, Y+1);
//		for(i=8;i<16;i++)
//		{
////			OLED_WriteData(OLED_F8x16[Char-' '][i]);
//			OLED_DisplayBuf[Y+1][X+i-8] = OLED_F8x16[Char-' '][i];
//		}
	}
	else if(FontSize == 6)
	{
		OLED_ShowImage(X,Y,6,8,OLED_F6x8[Char-' ']);
////		OLED_SetCursor(X, Y);
//		for(i=0;i<6;i++)
//		{
////			OLED_WriteData(OLED_F6x8[Char-' '][i]);
//			OLED_DisplayBuf[Y][X+i] = OLED_F6x8[Char-' '][i];
//		}
	}	
}

void OLED_ShowString(uint8_t X, uint8_t Y, char *String, uint8_t FontSize)
{
	uint8_t i;
	for(i=0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(X+i*FontSize,Y,String[i],FontSize);
	}	
}

uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while(Y--)
	{
		Result = Result * X;
	}
	return Result;
}

void OLED_ShowNum(uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize)
{
	uint8_t i;
	for(i=0;i<Length;i++)
	{
		OLED_ShowChar(X + i*FontSize, Y, Number/OLED_Pow(10,Length-i-1) % 10 + '0',FontSize);
	}
}

void OLED_ShowSignedNum(uint8_t X, uint8_t Y, int32_t Number, uint8_t Length, uint8_t FontSize)
{
	uint32_t uNumber;
	if(Number >= 0)
	{
		uNumber = Number;
		OLED_ShowChar(X,Y,'+',FontSize);
	}else{
		uNumber = -Number;
		OLED_ShowChar(X,Y,'-',FontSize);
	}
	uint8_t i;
	for(i=0;i<Length;i++)
	{
		OLED_ShowChar(X+ + (i+1)*FontSize, Y, uNumber/OLED_Pow(10,Length-i-1) % 10 + '0',FontSize);
	}
}


void OLED_ShowHexNum(uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize)
{
	uint8_t i, HexNumber;
	for(i=0;i<Length;i++)
	{
		HexNumber = Number/OLED_Pow(16,Length-i-1) % 16;
		if(HexNumber < 10)
		{
			OLED_ShowChar(X + i*FontSize, Y, HexNumber + '0',FontSize);
		}else{
			OLED_ShowChar(X + i*FontSize, Y, HexNumber - 10 + 'A',FontSize);
		}
	}
}

void OLED_ShowBinNum(uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize)
{
	uint8_t i, BinNumber;
	for(i=0;i<Length;i++)
	{
		BinNumber = Number/OLED_Pow(2,Length-i-1) % 2;
		OLED_ShowChar(X + i*FontSize, Y, BinNumber + '0',FontSize);
		
	}
}

void OLED_ShowFloatNum(uint8_t X, uint8_t Y, double Number, uint8_t IntLength, uint8_t FraLength, uint8_t FontSize)
{
	uint32_t IntNumber, FraNumber;
	IntNumber = (uint32_t)Number;
	FraNumber = (Number-IntNumber)*OLED_Pow(10,FraLength);//直接转换小数，如果截取显示，不进行四舍五入操作
	
	OLED_ShowNum(X, Y, IntNumber, IntLength, FontSize);
	OLED_ShowChar(X + IntLength*FontSize, Y, '.',FontSize);
	OLED_ShowNum(X+ (IntLength+1)*FontSize, Y, FraNumber, FraLength, FontSize);

}

void OLED_ShowChinese(uint8_t X, uint8_t Y, char *Chinese)
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
			OLED_ShowImage(X+((i+1)/OLED_CHN_CHAR_WIDTH-1)*16,Y,16,16,OLED_CF16x16[pIndex].Data);
		}
	}
}

void OLED_ClearArea(uint8_t X, uint8_t Y, uint8_t Width, uint8_t Height)
{
	for(uint8_t j=Y;j<Y+Height;j++)
	{
		for(uint8_t i=X;i<X+Width;i++)
		{
			OLED_DisplayBuf[j/8][i] &= ~(0x01 << (j%8));
		}
	}
}

void OLED_ShowImage(uint8_t X, uint8_t Y, uint8_t Width, uint8_t Height, const uint8_t *Image)
{
	OLED_ClearArea(X,Y,Width,Height);
	uint8_t i,j;
	//复杂，涉及很多算法
	for(j=0;j<(Height-1)/8+1;j++)
	{
		for(i=0;i<Width;i++)
		{
			OLED_DisplayBuf[Y/8+j][X+i] |= Image[(j*Width)+i] << (Y%8);
			OLED_DisplayBuf[Y/8+j+1][X+i] |= Image[(j*Width)+i] >> (8-Y%8);
		}
	}
}

void OLED_Printf(uint8_t X, uint8_t FontSize, char *format, ...);


void OLED_DrawPoint(uint8_t X, uint8_t Y)
{
	OLED_DisplayBuf[Y/8][X] |= 0x01  << (Y%8);
}

uint8_t OLED_GetPoint(uint8_t X, uint8_t Y)
{
	if(OLED_DisplayBuf[Y/8][X] & 0x01 << (Y%8))
	{
		return 1;
	}
	return 0;
}
void OLED_DrawLine(uint8_t X0, uint8_t Y0, uint8_t X1, uint8_t Y1)
{
	float k = (float)(Y1-Y0)/(X1-X0);
	uint8_t Temp;
	if(fabs(k)<1)
	{
		if(X0>X1)
		{
			Temp =X0;
			X0=X1;
			X1=Temp;
			
			Temp = Y0;
			Y0 = Y1;
			Y1 = Temp;
		}
		for(uint8_t x=X0;x<=X1;x++)
		{
			OLED_DrawPoint(x,round(Y0+(x-X0)*k));
		}
	}
	else
	{
		if(Y0>Y1)
		{
			Temp =X0;
			X0=X1;
			X1=Temp;
			
			Temp = Y0;
			Y0 = Y1;
			Y1 = Temp;
		}
		for(uint8_t y=Y0;y<=Y1;y++)
		{
			OLED_DrawPoint(round(X0+(y-Y0)/k),y);
		}
	}
}



