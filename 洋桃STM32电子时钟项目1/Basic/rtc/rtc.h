#ifndef __RTC_H
#define __RTC_H	 
#include "sys.h" 


u8 RTC_Get(u16 *ryear,u8 *rmon,u8 *rday,u8 *rhour,u8 *rmin,u8 *rsec,u8 *rweek);//读出当前时间值 //返回值:0,成功;其他:错误代码.	
void RTC_First_Config(void);//首次启用RTC的设置
void RTC_Config(void);//实时时钟初始化
u8 Is_Leap_Year(u16 year);//判断是否是闰年函数                    
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);//写入当前时间
u8 RTC_Get_Week(u16 year,u8 month,u8 day);//按年月日计算星期




#endif


















