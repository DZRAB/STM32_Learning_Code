#ifndef __SERVO_H
#define __SERVO_H

#include "stm32f10x.h" 
#include "PWM.h"

void Serveo_Init(void);
void Servo_SetAngle(float Angle);

#endif
