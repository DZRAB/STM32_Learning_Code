#include "PID.h"                  
#include "stm32f10x.h"                  
#include "math.h"                  

void PID_Update(PID_t *p)
{
	p->Error1 = p->Error0;
	p->Error0 = p->Target - p->Actual;
	
	if(fabs(p->Ki) > 0.0001)
	{
		p->ErrorInt += p->Error0;
		if(p->ErrorInt > p->ErrorIntMAX){p->ErrorInt = p->ErrorIntMAX;}
		if(p->ErrorInt < p->ErrorIntMIN){p->ErrorInt = p->ErrorIntMIN;}
	}
	else
		p->ErrorInt = 0;
	
	p->Out = p->Kp * p->Error0
					+ p->Ki * p->ErrorInt
//				+ p->Kd * (p->Error0 - p->Error1);
					- p->Kd * (p->Actual - p->Actual1);		
	
	if(p->Out > 0){p->Out += p->OutOffset;}
	if(p->Out < 0){p->Out -= p->OutOffset;}
	
	if(p->Out > p->OutMAX){p->Out = p->OutMAX;}
	if(p->Out < p->OutMIN){p->Out = p->OutMIN;}
	
	p->Actual1 = p->Actual;
}

void PID_Init(PID_t *p)
{
	p->Actual = p->Target = p->Error0 = p->Error1 = 
	p->ErrorInt = p->Out = p->Actual1 = 0;
}
