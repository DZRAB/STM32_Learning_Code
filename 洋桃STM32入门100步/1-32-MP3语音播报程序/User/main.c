/*********************************************************************************************
模板制作：  杜洋工作室/洋桃电子
程序名：	MP3语音播报程序
编写人：	杜洋	
编写时间：	2018年4月20日
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
#include "oled0561.h"
#include "rtc.h"

#include "encoder.h"
#include "usart.h"
#include "my1690.h"

int main (void){//主程序
	delay_ms(500); //上电时等待其他器件就绪
	RCC_Configuration(); //系统时钟初始化 
	RTC_Config();//实时时钟初始化
	TOUCH_KEY_Init();//触摸按键初始化
	RELAY_Init();//继电器初始化

	ENCODER_Init(); //旋转编码器初始化
	MY1690_Init(); //MP3芯片初始化

	I2C_Configuration();//I2C初始化
	OLED0561_Init(); //OLED初始化
	OLED_DISPLAY_8x16_BUFFER(0,"   YoungTalk    "); //显示字符串
	OLED_DISPLAY_8x16_BUFFER(2," MP3 TIME READ  "); //显示字符串

	while(1){

		if(RTC_Get()==0){
			OLED_DISPLAY_8x16(4,8*3,rhour/10+0x30);//显示时间
			OLED_DISPLAY_8x16(4,8*4,rhour%10+0x30);//显示时间
			OLED_DISPLAY_8x16(4,8*5,':');//
			OLED_DISPLAY_8x16(4,8*6,rmin/10+0x30);//显示时间
			OLED_DISPLAY_8x16(4,8*7,rmin%10+0x30);//显示时间
			OLED_DISPLAY_8x16(4,8*8,':');//
			OLED_DISPLAY_8x16(4,8*9,rsec/10+0x30);//显示时间
			OLED_DISPLAY_8x16(4,8*10,rsec%10+0x30);//显示时间
			delay_ms(200); //延时
		}
		if(GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)==0    //判断4个按键是否按下
			||GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)==0 
			||GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)==0 
			||GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D)==0){
			delay_ms(20); //延时
			if(GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)==0){	//4个按键：A上一曲，B下一曲，
				OLED_DISPLAY_8x16_BUFFER(6,"  -- PLAY --    "); //显示字符串
				//无语法
//				MY1690_CMD3(0x41,12); //直接播放
//				MY1690_CMD3(0x41,rhour/10+1); //直接播放
//				MY1690_CMD3(0x41,11); //直接播放
//				MY1690_CMD3(0x41,rhour%10+1); //直接播放
//				MY1690_CMD3(0x41,13); //直接播放
//				MY1690_CMD3(0x41,rmin/10+1); //直接播放
//				MY1690_CMD3(0x41,11); //直接播放
//				MY1690_CMD3(0x41,rmin%10+1); //直接播放
//				MY1690_CMD3(0x41,14); //直接播放
//				MY1690_CMD3(0x41,rsec/10+1); //直接播放
//				MY1690_CMD3(0x41,11); //直接播放
//				MY1690_CMD3(0x41,rsec%10+1); //直接播放
//				MY1690_CMD3(0x41,15); //直接播放

				//有语法
				MY1690_CMD3(0x41,12); //直接播放
				if(rhour/10==0){
					//不发音
				}else if(rhour/10==1){
					MY1690_CMD3(0x41,11); //直接播放
				}else{
					MY1690_CMD3(0x41,rhour/10+1); //直接播放
					MY1690_CMD3(0x41,11); //直接播放
				}
				if(rhour%10!=0 || rhour==0){
					MY1690_CMD3(0x41,rhour%10+1); //直接播放
				}
				MY1690_CMD3(0x41,13); //直接播放
				if(rmin/10==0){
					MY1690_CMD3(0x41,rmin/10+1); //直接播放
				}else if(rmin/10==1){
					MY1690_CMD3(0x41,11); //直接播放
				}else{
					MY1690_CMD3(0x41,rmin/10+1); //直接播放
					MY1690_CMD3(0x41,11); //直接播放
				}
				if(rmin%10!=0 || rmin==0){
					MY1690_CMD3(0x41,rmin%10+1); //直接播放
				}
				MY1690_CMD3(0x41,14); //直接播放
				if(rsec/10==0){
					MY1690_CMD3(0x41,rsec/10+1); //直接播放
				}else if(rsec/10==1){
					MY1690_CMD3(0x41,11); //直接播放
				}else{
					MY1690_CMD3(0x41,rsec/10+1); //直接播放
					MY1690_CMD3(0x41,11); //直接播放
				}
				if(rsec%10!=0 || rsec==0){
					MY1690_CMD3(0x41,rsec%10+1); //直接播放
				}
				MY1690_CMD3(0x41,15); //直接播放
			}
			while(GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)==0	//等待按键放开
			||GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)==0 
			||GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)==0 
			||GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D)==0);
		}
		//串口接收处理
        if(USART3_RX_STA==1){ //如果标志位是1 表示收到STOP
            OLED_DISPLAY_8x16_BUFFER(6,"  -- STOP --    "); //显示字符串
            USART3_RX_STA=0; //将串口数据标志位清0
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



