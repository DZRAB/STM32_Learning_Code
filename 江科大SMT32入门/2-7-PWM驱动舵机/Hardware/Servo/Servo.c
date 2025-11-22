#include "Servo.h"

void Serveo_Init(void)
{
	PWM_Init();
}


//0°	对应 500
//180° 对应 2500
void Servo_SetAngle(float Angle)
{
	PWM_SetCompare2(Angle/180*2000+500);
}