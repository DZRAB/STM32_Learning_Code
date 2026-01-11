#include "PID.h"                  
#include "stm32f10x.h"                  
#include "math.h"                  

void PID_Update(PID_t *p)
{
	p->Error1 = p->Error0;
	p->Error0 = p->Target - p->Actual;
	
	if(fabs(p->Ki) > 0.0001)
		p->ErrorInt += p->Error0;
	else
		p->ErrorInt = 0;
	
	p->Out = p->Kp * p->Error0
					+ p->Ki * p->ErrorInt
					+ p->Kp * (p->Error0 - p->Error1);
	
	if(p->Out > p->OutMAX){p->Out = p->OutMAX;}
	if(p->Out < p->OutMIN){p->Out = p->OutMIN;}
}
