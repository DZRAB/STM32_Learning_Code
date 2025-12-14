#ifndef __INPUT_CAPTURE_H
#define __INPUT_CAPTURE_H

#include "stm32f10x.h" 
void InputCapture_Init(void);
uint32_t InputCapture_GetFreq(void);
uint32_t InputCapture_GetDuty(void);

#endif
