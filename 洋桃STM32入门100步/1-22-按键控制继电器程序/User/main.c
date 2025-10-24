/*********************************************************************************************
模板制作：  杜洋工作室/洋桃电子
程序名：	按键控制继电器程序
编写人：	杜洋	
编写时间：	2018年3月9日
硬件支持：	STM32F103C8   外部晶振8MHz RCC函数设置主频72MHz　  

修改日志：　　
1-	
	
							
说明：
 # 本模板加载了STM32F103内部的RCC时钟设置，并加入了利用滴答定时器的延时函数。
 # 可根据自己的需要增加或删减。

*********************************************************************************************/
#include "stm32f10x.h" //STM32头文件
#include "sys.h"
#include "delay.h"
#include "touch_key.h"

#include "relay.h"

int main (void){//主程序
	RCC_Configuration(); //系统时钟初始化 
	TOUCH_KEY_Init();//触摸按键初始化

	RELAY_Init();//继电器初始化

	while(1){
		if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A))RELAY_1(1); //当按键A按下时继电器1标志置位		
		if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B))RELAY_1(0); //当按键B按下时继电器1标志置位		
		if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C))RELAY_2(1); //当按键C按下时继电器2标志置位
		if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D))RELAY_2(0); //当按键D按下时继电器2标志置位
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



