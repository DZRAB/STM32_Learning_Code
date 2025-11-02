/*********************************************************************************************
程序名：	洋桃项目组1-电子时钟项目
硬件支持：	STM32F103C8   外部晶振8MHz RCC函数设置主频72MHz　
编写时间：2025年11月2日

修改日志	：
1-2025年11月2日12:07:32 创建工程，模板复制：1-17-数码管RTC显示程序

说明：
 # 本模板加载了STM32F103内部的RCC时钟设置，并加入了利用滴答定时器的延时函数。
 # 可根据自己的需要增加或删减。

*********************************************************************************************/
#include "stm32f10x.h" //STM32头文件
#include "sys.h"
#include "delay.h"
#include "rtc.h"
#include "TM1640.h"
#include "lm75a.h"
#include "i2c.h"


int main(void)// 主程序
{ 
	u8 temp[3];
	RCC_Configuration(); // 系统时钟初始化
	RTC_Config();		 // RTC初始化
	I2C_Configuration();
	TM1640_Init();		 // TM1640初始化
	while (1)
	{
		
		if (RTC_Get() == 0)
		{				
			TM1640_display(0, ryear % 100 / 10); // 年
			TM1640_display(1, ryear % 100 % 10);
			TM1640_display(2, 21); //-
			TM1640_display(3, rmon / 10);  // 月
			TM1640_display(4, rmon % 10);
			TM1640_display(5, 21); //-
			TM1640_display(6, rday / 10);  // 日
			TM1640_display(7, rday % 10);

			delay_ms(1000); // 延时

			TM1640_display(0, 20); // 空
			TM1640_display(1, 20);
			TM1640_display(2, rhour / 10); // 时
			TM1640_display(3, rhour % 10 + 10);
			TM1640_display(4, rmin / 10); // 分
			TM1640_display(5, rmin % 10 + 10);
			TM1640_display(6, rsec / 10); // 秒
			TM1640_display(7, rsec % 10);

			delay_ms(1000); // 延时

			LM75A_GetTemp(temp);
			TM1640_display(0, 20); // 空
			TM1640_display(1, 20);
			if(temp[0] == 0) //判断温度正负
			{
				TM1640_display(2, 20);//不显示符号
			}else
			{
				TM1640_display(2, 21);//显示-符号
			}
			TM1640_display(3, temp[1] % 100 / 10); 
			TM1640_display(4, temp[1] % 10 + 10);  // 温度
			TM1640_display(5, temp[2] % 100 / 10);
			TM1640_display(6, temp[2] % 10);
			TM1640_display(7, 22); //'C'

			delay_ms(1000); // 延时
		}
	}
}

/*

【变量定义】
u32     a; //定义32位无符号变量a
u16     a; //定义16位无符号变量a
u8     a; //定义8位无符号变量a
vu32     a; //定义易变的32位无符号变量a
vu16     a; //定义易变的 16位无符号变量a
vu8     a; //定义易变的 8位无符号变量a
uc32     a; //定义只读的32位无符号变量a
uc16     a; //定义只读 的16位无符号变量a
uc8     a; //定义只读 的8位无符号变量a

#define ONE  1   //宏定义

delay_us(1); //延时1微秒
delay_ms(1); //延时1毫秒
delay_s(1); //延时1秒

GPIO_WriteBit(LEDPORT,LED1,(BitAction)(1)); //LED控制

*/
