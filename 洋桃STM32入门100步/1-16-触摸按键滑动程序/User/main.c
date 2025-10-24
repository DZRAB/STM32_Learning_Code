/*********************************************************************************************
程序名：	触摸按键滑动,单击,双击,长按程序
硬件支持：	STM32F103C8   外部晶振8MHz RCC函数设置主频72MHz　  
							
说明：
 # 本模板加载了STM32F103内部的RCC时钟设置，并加入了利用滴答定时器的延时函数。
 # 可根据自己的需要增加或删减。

*********************************************************************************************/
#include "stm32f10x.h" //STM32头文件
#include "sys.h"
#include "delay.h"
#include "led.h"
#include "touch_key.h"
#include "usart.h"

#define KEYA_SPEED1	100	  //长按的时间长度（单位10mS）
#define KEYA_SPEED2	10	  //双击的时间长度（单位20mS）



int main (void){//主程序
	u16 k=1000;	//用于滑动加减计数
	u8 a=0,b,c=0;
	u8 s=0; //刚刚结束滑动标志
	RCC_Configuration(); //系统时钟初始化 
	USART1_Init(115200); //串口初始化，参数中写波特率
	LED_Init();//LED初始化
	TOUCH_KEY_Init();//按键初始化
	while(1){
//A
		if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){ //检测按键是否按下
			delay_ms(20); //延时去抖动
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){//判断长短键
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A))&&c<KEYA_SPEED1){ //循环判断长按，到时跳转
					c++;delay_ms(10); //长按判断的计时
				}
				if(c>=KEYA_SPEED1){ //长键处理
					//长按后执行的程序放到此处
					GPIO_WriteBit(LEDPORT,LED1,(BitAction)(1));//LED控制
					printf("A键长按 \r\n");
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A));
				}else{ //单击处理
					if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){
						k++; //用于显示的计数值
						printf("A键右滑 %d \r\n",k); 
						a=1;s=1; //a是单双击判断标志，s是刚刚结束滑动标志
					}
					if(a==0){
						for(b=0;b<KEYA_SPEED2;b++){//检测双击
							delay_ms(20);
							if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){
								a=1;
								//双击后执行的程序放到此处
								GPIO_WriteBit(LEDPORT,LED2,(BitAction)(1));//LED控制
								printf("A键双击 \r\n");
								while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A));
							}
						}
						if(a==0){ //判断单击
							if(s==1){ //判断是不是刚执行完滑动操作
								s=0; //如果是则本次不执行单击处理（因为是滑动的放开操作）
							}else{	 //如果不是，则正常执行单击处理
								//单击后执行的程序放到此处
								GPIO_WriteBit(LEDPORT,LED1|LED2,(BitAction)(0));//LED控制
								printf("A键单击 \r\n");
							}
						}
					}
				}
				a=0;c=0; //参数清0
			}
		} //按键判断在此结束
//B
		if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){ //检测按键是否按下
			delay_ms(20); //延时去抖动
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){//判断长短键
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B))&&c<KEYA_SPEED1){ //循环判断长按，到时跳转
					c++;delay_ms(10); //长按判断的计时
				}
				if(c>=KEYA_SPEED1){ //长键处理
					//长按后执行的程序放到此处
					GPIO_WriteBit(LEDPORT,LED1,(BitAction)(1));//LED控制
					printf("B键长按 \r\n");
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B));
				}else{ //单击处理
					if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)){
						k++;
						printf("B键右滑 %d \r\n",k); 
						a=1;s=1; //a是单双击判断标志，s是刚刚结束滑动标志
					}
					if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_A)){
						k--;
						printf("B键左滑 %d \r\n",k); 
						a=1;s=1; //a是单双击判断标志，s是刚刚结束滑动标志
					}
					if(a==0){
						for(b=0;b<KEYA_SPEED2;b++){//检测双击
							delay_ms(20);
							if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){
								a=1;
								//双击后执行的程序放到此处
								GPIO_WriteBit(LEDPORT,LED2,(BitAction)(1));//LED控制
								printf("B键双击 \r\n");
								while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B));
							}
						}
						if(a==0){ //判断单击
							if(s==1){ //判断是不是刚执行完滑动操作
								s=0; //如果是则本次不执行单击处理（因为是滑动的放开操作）
							}else{	 //如果不是，则正常执行单击处理
								//单击后执行的程序放到此处
								GPIO_WriteBit(LEDPORT,LED1|LED2,(BitAction)(0));//LED控制
								printf("B键单击 \r\n");
							}
						}
					}
				}
				a=0;c=0; //参数清0
			}
		} //按键判断在此结束
//C
		if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)){ //检测按键是否按下
			delay_ms(20); //延时去抖动
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)){//判断长短键
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C))&&c<KEYA_SPEED1){ //循环判断长按，到时跳转
					c++;delay_ms(10); //长按判断的计时
				}
				if(c>=KEYA_SPEED1){ //长键处理
					//长按后执行的程序放到此处
					GPIO_WriteBit(LEDPORT,LED1,(BitAction)(1));//LED控制
					printf("C键长按 \r\n");
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
				}else{ //单击处理
					if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D)){
						k++;
						printf("C键右滑 %d \r\n",k); 
						a=1;s=1; //a是单双击判断标志，s是刚刚结束滑动标志
					}
					if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_B)){
						k--;
						printf("C键左滑 %d \r\n",k); 
						a=1;s=1; //a是单双击判断标志，s是刚刚结束滑动标志
					}
					if(a==0){
						for(b=0;b<KEYA_SPEED2;b++){//检测双击
							delay_ms(20);
							if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)){
								a=1;
								//双击后执行的程序放到此处
								GPIO_WriteBit(LEDPORT,LED2,(BitAction)(1));//LED控制
								printf("C键双击 \r\n");
								while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C));
							}
						}
						if(a==0){ //判断单击
							if(s==1){ //判断是不是刚执行完滑动操作
								s=0; //如果是则本次不执行单击处理（因为是滑动的放开操作）
							}else{	 //如果不是，则正常执行单击处理
								//单击后执行的程序放到此处
								GPIO_WriteBit(LEDPORT,LED1|LED2,(BitAction)(0));//LED控制
								printf("C键单击 \r\n");
							}
						}
					}
				}
				a=0;c=0; //参数清0
			}
		} //按键判断在此结束
//D
		if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D)){ //检测按键是否按下
			delay_ms(20); //延时去抖动
			if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D)){//判断长短键
				while((!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D))&&c<KEYA_SPEED1){ //循环判断长按，到时跳转
					c++;delay_ms(10); //长按判断的计时
				}
				if(c>=KEYA_SPEED1){ //长键处理
					//长按后执行的程序放到此处
					GPIO_WriteBit(LEDPORT,LED1,(BitAction)(1));//LED控制
					printf("D键长按 \r\n");
					while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
				}else{ //单击处理
					if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_C)){
						k--;
						printf("D键左滑 %d \r\n",k); 
						a=1;s=1; //a是单双击判断标志，s是刚刚结束滑动标志
					}
					if(a==0){
						for(b=0;b<KEYA_SPEED2;b++){//检测双击
							delay_ms(20);
							if(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D)){
								a=1;
								//双击后执行的程序放到此处
								GPIO_WriteBit(LEDPORT,LED2,(BitAction)(1));//LED控制
								printf("D键双击 \r\n");
								while(!GPIO_ReadInputDataBit(TOUCH_KEYPORT,TOUCH_KEY_D));
							}
						}
						if(a==0){ //判断单击
							if(s==1){ //判断是不是刚执行完滑动操作
								s=0; //如果是则本次不执行单击处理（因为是滑动的放开操作）
							}else{	 //如果不是，则正常执行单击处理
								//单击后执行的程序放到此处
								GPIO_WriteBit(LEDPORT,LED1|LED2,(BitAction)(0));//LED控制
								printf("D键单击 \r\n");
							}
						}
					}
				}
				a=0;c=0; //参数清0
			}
		} //按键判断在此结束

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



