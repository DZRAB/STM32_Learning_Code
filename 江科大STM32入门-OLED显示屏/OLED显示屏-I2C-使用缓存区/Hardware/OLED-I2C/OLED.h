#ifndef __OLED_H
#define __OLED_H

#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "SoftwareI2C.h"
#include "OLED_Data.h"
#include <string.h>

extern uint8_t OLED_DisplayBuf[8][128];

void OLED_Init(void);
void OLED_WriteCommand(uint8_t Command);
void OLED_WriteData(uint8_t *Data, uint8_t Count);
void OLED_SetCursor(uint8_t X, uint8_t Page);
void OLED_Updata(void);
void OLED_Clear(void);

void OLED_ShowChar(uint8_t X, uint8_t Y, char Char, uint8_t FontSize);
void OLED_ShowString(uint8_t X, uint8_t Y, char *String, uint8_t FontSize);

void OLED_ShowNum(uint8_t X, uint8_t Y,uint32_t Number, uint8_t Length, uint8_t FontSize);
void OLED_ShowSignedNum(uint8_t X, uint8_t Y, int32_t Number, uint8_t Length, uint8_t FontSize);
void OLED_ShowHexNum(uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize);
void OLED_ShowBinNum(uint8_t X, uint8_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize);

void OLED_ShowFloatNum(uint8_t X, uint8_t Y, double Number, uint8_t IntLength, uint8_t FraLength, uint8_t FontSize);

void OLED_ShowChinese(uint8_t X, uint8_t Y, char *Chinese);
void OLED_ShowImage(uint8_t X, uint8_t Y, uint8_t Width, uint8_t Height, const uint8_t *Image);

void OLED_Printf(uint8_t X, uint8_t FontSize, char *format, ...);



#endif

