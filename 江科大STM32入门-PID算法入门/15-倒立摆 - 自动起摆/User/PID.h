#ifndef __PID_H_
#define __PID_H_

typedef struct 
{
	float Target, Actual, Out; //目标值，实际值，输出值
	float Kp, Ki, Kd;//比例项，积分项，微分项的权重表
	float Error0, Error1, ErrorInt;//本次误差，上次误差，误差积分
	
	float OutMAX, OutMIN;

} PID_t;

void PID_Update(PID_t *p);

#endif
