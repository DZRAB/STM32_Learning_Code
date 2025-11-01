/*********************************************************************************************
模板制作：  杜洋工作室/洋桃电子
程序名：	待机模式测试程序
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


int main (void){//主程序
	delay_ms(500); //上电时等待其他器件就绪
	RCC_Configuration(); //系统时钟初始化 
	RELAY_Init();//继电器初始化
	LED_Init();//LED 

	I2C_Configuration();//I2C初始化

	OLED0561_Init(); //OLED初始化
	OLED_DISPLAY_8x16_BUFFER(0," STANDBY TEST   "); //显示字符串

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);  //使能电源PWR时钟
	PWR_WakeUpPinCmd(ENABLE);//WKUP唤醒功能开启（待机时WKUP脚PA0为模拟输入）

	GPIO_WriteBit(LEDPORT,LED1,(BitAction)(0)); //LED控制
	OLED_DISPLAY_8x16_BUFFER(4,"  CPU RESET!    "); //显示字符串
	delay_ms(2000); //

	GPIO_WriteBit(LEDPORT,LED1,(BitAction)(1)); //LED控制
	OLED_DISPLAY_8x16_BUFFER(4,"   STANDBY!     "); //显示字符串
	delay_ms(2000); //

	PWR_EnterSTANDBYMode();//进入待机模式

	//因为待机唤醒后程序从头运行，所以不需要加while(1)的主循环体。
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



