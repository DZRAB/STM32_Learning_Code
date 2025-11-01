/*********************************************************************************************
模板制作：  杜洋工作室/洋桃电子
程序名：	睡眠模式测试程序
编写人：	杜洋	
编写时间：	2018年7月14日
硬件支持：	洋桃1号开发板 STM32F103C8 外部晶振8MHz RCC函数设置主频72MHz　  

修改日志：　　
1-	
	
							
说明：
 # 本程序是在洋桃1号开发板的硬件基础上编写的，移植需了解硬件接口差异。
 # 本模板加载了STM32F103内部的RCC时钟设置，并加入了利用滴答定时器的延时函数。
 # 可根据自己的需要增加或删减。

*********************************************************************************************/
#include "stm32f10x.h" //STM32头文件
#include "sys.h"
#include "delay.h"
#include "relay.h"
#include "oled0561.h"
#include "led.h"
#include "key.h"

#include "NVIC.h"


int main (void){//主程序
	delay_ms(500); //上电时等待其他器件就绪
	RCC_Configuration(); //系统时钟初始化 
	RELAY_Init();//继电器初始化
	LED_Init();//LED 
	KEY_Init();//KEY

	I2C_Configuration();//I2C初始化

	OLED0561_Init(); //OLED初始化
	OLED_DISPLAY_8x16_BUFFER(0,"   SLEEP TEST   "); //显示字符串

	INT_MARK=0;//标志位清0
	NVIC_Configuration();//设置中断优先级
	KEY_INT_INIT();//按键中断初始化（PA0是按键中断输入）

	NVIC_SystemLPConfig(NVIC_LP_SEVONPEND,DISABLE);	//SEVONPEND: 0：只有使能的中断或事件才能唤醒内核。1：任何中断和事件都可以唤醒内核。（0=DISABLE，1=ENABLE) 
	NVIC_SystemLPConfig(NVIC_LP_SLEEPDEEP,DISABLE);	//SLEEPDEEP: 0：低功耗模式为睡眠模式。1：进入低功耗时为深度睡眠模式。
    NVIC_SystemLPConfig(NVIC_LP_SLEEPONEXIT,DISABLE); //SLEEPONEXIT: 0: 被唤醒进入线程模式后不再进入睡眠模式。1：被唤醒后执行完相应的中断处理函数后进入睡眠模式。

	while(1){

		GPIO_WriteBit(LEDPORT,LED1,(BitAction)(1)); //LED控制
		OLED_DISPLAY_8x16_BUFFER(4,"  CPU SLEEP!    "); //显示字符串
		delay_ms(500); //

		__WFI(); //进入睡眠模式，等待中断唤醒
//		__WFE(); //进入睡眠模式，等待事件唤醒

		GPIO_WriteBit(LEDPORT,LED1,(BitAction)(0)); //LED控制
		OLED_DISPLAY_8x16_BUFFER(4,"  CPU WAKE UP!  "); //显示字符串
		delay_ms(500); //
	}

}

/*********************************************************************************************
 * 杜洋工作室 www.DoYoung.net
 * 洋桃电子 www.DoYoung.net/YT 
*********************************************************************************************/
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



