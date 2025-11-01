/*********************************************************************************************
模板制作：  杜洋工作室/洋桃电子
程序名：	CRC功能测试程序
编写人：	杜洋	
编写时间：	2018年7月21日
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


int main (void){//主程序
	u32 a,b;
	u8 c;
	u32 y[3]={0x87654321,0x98765432,0x09876543};
	delay_ms(500); //上电时等待其他器件就绪
	RCC_Configuration(); //系统时钟初始化 
	RELAY_Init();//继电器初始化
	LED_Init();//LED 
	KEY_Init();//KEY

	I2C_Configuration();//I2C初始化

	OLED0561_Init(); //OLED初始化---------------"
	OLED_DISPLAY_8x16_BUFFER(0,"   CRC TEST     "); //显示字符串

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);//开启CRC时钟

	while(1){
		CRC_ResetDR();//复位CRC，需要清0重新计算时先复位
		CRC_CalcCRC(0x12345678);//CRC计算一个32位数据。参数：32位数据。返回值：32位计算结果
		CRC_CalcCRC(0x23456789);//CRC计算一个32位数据。参数：32位数据。返回值：32位计算结果
		a = CRC_CalcCRC(0x34567890);//CRC计算一个32位数据。参数：32位数据。返回值：32位计算结果

		CRC_ResetDR();//复位CRC，需要清0重新计算时先复位
		b = CRC_CalcBlockCRC(y,3);//CRC计算一个32位数组。参数：32位数组名，数组长度。返回值：32位计算结果

		CRC_SetIDRegister(0x5a);//向独立寄存器CRC_IDR写数据。参数：8位数据。
		c = CRC_GetIDRegister();//从独立寄存器CRC_IDR读数据。返回值：8位数据。

		//此时，a存放的是3个独立数的CRC结果。（32位）
		//b存放的是数组y中3个数据CRC计算结果。（32位）
		//c存放的是我们写入的独立寄存器数据0x5a。（8位）
	}
}

//	以下是CRC固件库函数，可在主程序中直接调用  //

//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);//开启CRC时钟，主程序开始时调用
//	CRC_ResetDR();//复位CRC，需要清0重新计算时先复位
//	uint32_t CRC_CalcCRC(uint32_t Data);//CRC计算一个32位数据。参数：32位数据。返回值：32位计算结果
//	uint32_t CRC_CalcBlockCRC(uint32_t pBuffer[], uint32_t BufferLength);//CRC计算一个32位数组。参数：32位数组名，数组长度。返回值：32位计算结果
//	uint32_t CRC_GetCRC(void);//从CRC中读出计算结果。返回值：32位计算结果。

//	void CRC_SetIDRegister(uint8_t IDValue);//向独立寄存器CRC_IDR写数据。参数：8位数据。
//	uint8_t CRC_GetIDRegister(void);//从独立寄存器CRC_IDR读数据。返回值：8位数据。


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



