/*********************************************************************************************
模板制作：  杜洋工作室/洋桃电子
程序名：	窗口看门狗测试程序
编写人：	杜洋	
编写时间：	2018年7月17日
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

#include "wwdg.h"


int main (void){//主程序
	delay_ms(500); //上电时等待其他器件就绪
	RCC_Configuration(); //系统时钟初始化 
	RELAY_Init();//继电器初始化
	LED_Init();//LED 
	KEY_Init();//KEY
	BUZZER_Init();

	I2C_Configuration();//I2C初始化

	OLED0561_Init(); //OLED初始化---------------"
	OLED_DISPLAY_8x16_BUFFER(0,"   WWDG TEST    "); //显示字符串
	OLED_DISPLAY_8x16_BUFFER(4,"    RESET!      "); //显示字符串
	delay_ms(800); //
	OLED_DISPLAY_8x16_BUFFER(4,"                "); //显示字符串

	WWDG_Init(); //初始化并启动独立看门狗

	while(1){
		delay_ms(54); //用延时找到喂狗的窗口时间
		WWDG_Feed(); //喂狗

		if(!GPIO_ReadInputDataBit(KEYPORT,KEY1)){
			delay_s(2);	//延时2秒，使程序不能喂狗而导致复制
		}
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



