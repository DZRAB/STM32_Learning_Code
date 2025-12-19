#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h" 
#include "Delay.h"

#define KEY_PRESSED		1
#define KEY_UNPRESSED	0

#define KEY_HOLD		0x01
#define KEY_DOWN		0x02
#define KEY_UP			0x04
#define KEY_SINGLE		0x08
#define KEY_DOUBLE		0x10
#define KEY_LONG		0x20
#define KEY_REPEAT		0x40

#define KEY_TIME_DOUBLE		80
#define KEY_TIME_LONG		2000
#define KEY_TIME_REPEAT 	100

#define KEY_COUNT		4

#define KEY_1			0
#define KEY_2			1
#define KEY_3			2
#define KEY_4			3


/*
Key_Flag标志位定义：
NULL/不定义			7
REPEAT/重复按下		6
LONG/长按			5
DOUBLE/双击			4
SINGLE/单击			3
UP/松开时刻			2
DOWN/按下时刻		1
HOLD/按住不放		0
*/

void Key_Init(void);
uint8_t Key_GetState(uint8_t N);
uint8_t Key_Check(uint8_t N, uint8_t Flag);
void Key_Tick(void);

	
#endif
