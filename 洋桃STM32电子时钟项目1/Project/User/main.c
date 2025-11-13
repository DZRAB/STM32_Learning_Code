/*********************************************************************************************
程序名：	洋桃项目组1-电子时钟项目
硬件支持：	STM32F103C8   外部晶振8MHz RCC函数设置主频72MHz　
编写时间：2025年11月2日

修改日志	：
1-2025年11月2日12:07:32 创建工程，模板复制：1-17-数码管RTC显示程序
2-2025年11月2日21:07:56 增加按键选择功能
3-2025年11月8日 实现了闹钟选项功能
4-2025年11月9日 对程序进行了备注
5-2025年11月11日 实现8个闹钟的到点提醒，按键关断，自动关断，闹钟功能全部完成
6-2025年11月12日 实现了2个温度报警闹钟


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
#include "touch_key.h"
#include "buzzer.h"
#include "flash.h"
#include "relay.h"

int main(void) // 主程序
{
	u8 MENU = 0;				 // 菜单
	u8 temp[3];					 // 温度
	u16 i, a = 0;				 // 通用变量
	u8 c = 0;					 // 判断按键长按的计数
	u16 t = 0;					 // 循环时间计数变量
	u8 rup = 0;					 // 时间设置标志位
	u8 MENU2 = 0;				 // 菜单2,增加闹钟功能,0是设置时间,1-8是设置闹钟.9-10是温度报警
	u8 alFlag = 0;				 // 闹钟标志位
	u8 tc1Flag = 0, tc2Flag = 0; // 温度报警标志位
	// 以下2条局部变量--用于RTC时间的读取
	u16 ryear = 2025;												 // 4位年
	u8 rmon = 1, rday = 1, rhour = 1, rmin = 1, rsec = 1, rweek = 0; // 2位月日时分秒周													 // 闹钟标志为
	delay_ms(200);
	RCC_Configuration(); // 系统时钟初始化
	RTC_Config();		 // RTC初始化
	I2C_Configuration(); // I2C初始化
	TM1640_Init();		 // TM1640初始化
	TM1640_led(0x00);	 // 熄灭8个led
	TOUCH_KEY_Init();	 // 触摸按键初始化
	BUZZER_Init();
	BUZZER_BOOT_BEEP();
	RELAY_Init();
	RELAY_1(0);
	RELAY_2(0);

	for (i = 0; i < 10; i++) // 从flash读出闹钟数据
	{
		t = FLASH_R(FLASH_START_ADDR + i * 2); // 从指定页的地址读FLASH，因为是16位，每次读2个字节,读数据的偏移量每次是2
		alhour[i] = (t >> 8) & 0xff;		   // 高八位是小时
		almin[i] = t & 0xff;				   // 低八位是分钟
		if (i < 8 && alhour[i] > 24)
		{
			// 判断flash是否首次使用，是则清零
			alhour[i] = 24; // 初始化为24，默认闹钟关
			almin[i] = 0;
		}
		else if (i >= 8 && alhour[i] > 60)
		{
			alhour[i] = 0; // 温度报警初始化为0，默认关
			almin[i] = 0;
		}
	}
	ALFlash_W();

	while (1) // 主循环
	{
		if (rup == 1) // 设置时间标志位，时间设置好后，rup置1，把时间写入flash
		{
			BUZZER_QUIT_SET_BEEP();
			rup = 0;									   // 标志为清零
			RTC_Set(ryear, rmon, rday, rhour, rmin, rsec); // 写入当前时间
		}

		if (MENU < 2 || MENU > 8) // 不在时间设置时，才更新时间；并且显示温度时不更新温度
		{
			RTC_Get(&ryear, &rmon, &rday, &rhour, &rmin, &rsec, &rweek); // 读出RTC时间
																		 // if (MENU != 2)
			LM75A_GetTemp(temp);
		}

		if (MENU < 3) // 循环显示0-2菜单
		{
			t++;
			if (t == 1)
				MENU = 1;
			if (t == 7000)
				MENU = 0;
			if (t == 8000)
				MENU = 2;
			if (t > 8000)
				t = 0;
		}

		if (MENU < 3) // 年月期-时分秒-温度的显示
		{
			if (MENU == 0) // 显示年月日
			{
				TM1640_display(0, ryear % 100 / 10); // 年
				TM1640_display(1, ryear % 100 % 10);
				TM1640_display(2, 21);		  //-
				TM1640_display(3, rmon / 10); // 月
				TM1640_display(4, rmon % 10);
				TM1640_display(5, 21);		  //-
				TM1640_display(6, rday / 10); // 日
				TM1640_display(7, rday % 10);
			}
			else if (MENU == 1) // 显示时分秒
			{
				TM1640_display(0, 20); // 空
				TM1640_display(1, 20);
				TM1640_display(2, rhour / 10); // 时
				TM1640_display(3, rhour % 10 + 10);
				TM1640_display(4, rmin / 10); // 分
				TM1640_display(5, rmin % 10 + 10);
				TM1640_display(6, rsec / 10); // 秒
				TM1640_display(7, rsec % 10);
			}
			else if (MENU == 2) // 显示温度
			{
				TM1640_display(0, 20); // 空
				TM1640_display(1, 20);
				if (temp[0] == 0)		   // 判断温度正负
					TM1640_display(2, 20); // 不显示符号
				else
					TM1640_display(2, 21); // 显示-符号
				TM1640_display(3, temp[1] % 100 / 10);
				TM1640_display(4, temp[1] % 10 + 10); // 温度
				TM1640_display(5, temp[2] % 100 / 10);
				TM1640_display(6, temp[2] % 10);
				TM1640_display(7, 22);
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)) // 读触摸按键的电平
			{
				BUZZER_KEY_BEEP();
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A))
					;
				MENU = 0;
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) // 读触摸按键的电平
			{
				BUZZER_KEY_BEEP();
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B))
					;
				MENU = 1;
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)) // 读触摸按键的电平
			{
				BUZZER_KEY_BEEP();
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C))
					;
				MENU = 2;
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)) // 读触摸按键的电平
			{

				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D))
					;
				MENU = 3;
			}
		}

		if (MENU < 3) // 闹钟判断
		{
			for (i = 0; i < 8; i++) // 循环判断8个闹钟
			{
				if (alhour[i] == rhour && almin[i] == rmin && rsec == 0)
				{

					MENU2 = i + 1; // MENU2记录响铃闹钟，1-8表示闹钟的，i为0时表示是闹钟1的时间，这里所以要+1
					t = 0;
					if (MENU2 == 7 || MENU2 == 8) // 7和8闹钟设置成继电器的开和关
					{
						if (MENU2 == 7)
						{
							RELAY_1(1);
							RELAY_2(1);
							BUZZER_SET_BEEP();
						}
						if (MENU2 == 8)
						{
							RELAY_1(0);
							RELAY_2(0);
							BUZZER_SET_BEEP();
						}
						MENU = 1; // 继电器直接跳转到时间显示
					}
					else
					{
						MENU = 200; // 跳转到闹钟处理菜单
						alFlag = 1;
					}
				}
			}
		}

		if (alFlag == 1) // 闹钟响铃时间，继电器闹钟不管此处
		{
			t++;
			if (t > 350) // 10大概2秒
			{
				MENU = 1;
				alFlag = 0;
				t = 0;
			}
		}

		if (MENU < 3) // 温度判断
		{
			//LM75A_GetTemp(temp);
			for (a = 8; a < 10; a++) // 循环判断2个温度
			{
				// 高温报警
				if ((temp[1] % 100 * 10 + temp[2] % 100 / 10) >=
						(alhour[a] * 10 + almin[a]) &&
					alhour[a] != 0)
				{
					MENU2 = a + 1; // MENU2的9-10记录温度报警，，a为8时表示温度报警1，这里所以要+1
					if (MENU2 == 9 && tc1Flag != 1) //tc1
					{
						BUZZER_SET_BEEP();    
						tc1Flag = 1;
						RELAY_1(1);
						MENU = 1;
					}
					if (MENU2 == 10 && tc2Flag != 1) //tc2
					{
						BUZZER_SET_BEEP();
						tc2Flag = 1;
						RELAY_2(1);
						MENU = 1;
					}
				}

				// 低温报警
				if ((temp[1] % 100 * 10 + temp[2] % 100 / 10) <
						(alhour[a] * 10 + almin[a]) &&
					alhour[a] != 0)
				{
					MENU2 = a + 1;
					if (MENU2 == 9 && tc1Flag != 2) //tc1
					{
						BUZZER_SET_BEEP();
						tc1Flag = 2;
						RELAY_1(0);
						MENU = 1;
					}
					if (MENU2 == 10 && tc2Flag != 2) //tc2
					{
						BUZZER_SET_BEEP();
						tc2Flag = 2;
						RELAY_2(0);
						MENU = 1;
					}
				}
			}
		}

		if (MENU == 3) // 设置菜单，增加选择菜单，MENU2的值：0为设置时间，1-8为闹钟，9-10为温度报警
		{
			TM1640_display(0, 23); //'S'
			TM1640_display(1, 24); //'E'
			TM1640_display(2, 25); //'t'
			TM1640_display(3, 20);
			TM1640_display(4, 20);
			TM1640_display(5, 20);
			TM1640_display(6, 20);
			TM1640_display(7, 20);
			BUZZER_SET_BEEP();
			MENU = 100; // 跳转到设置选项
			MENU2 = 0;	// 默认为SET:时间设置
		}

		if (MENU == 100) // 设置选项，增加闹钟功能
		{
			if (MENU2 == 0) // SET
			{
				i++;
				if (i > 300) // 字符闪烁
				{
					TM1640_display(0, 23); //'S'
					TM1640_display(1, 24); //'E'
					TM1640_display(2, 25); //'t'
				}
				else
				{
					TM1640_display(0, 20);
					TM1640_display(1, 20);
					TM1640_display(2, 20);
				}
				if (i > 600)
					i = 0;
				TM1640_display(3, 20);
				TM1640_display(4, 20);
				TM1640_display(5, 20);
				TM1640_display(6, 20);
				TM1640_display(7, 20);
			}
			if (MENU2 >= 1 && MENU2 <= 8) // AL闹钟：1-8
			{
				i++;
				if (i > 300)
				{
					TM1640_display(0, 26);	  //'A'
					TM1640_display(1, 27);	  //'L'
					TM1640_display(2, MENU2); //'MENU2的值'
				}
				else
				{
					TM1640_display(0, 20);
					TM1640_display(1, 20);
					TM1640_display(2, 20);
				}
				if (i > 600)
					i = 0;

				TM1640_display(3, 20);

				// 显示对应闹钟的设置内容
				if (alhour[MENU2 - 1] == 24) // 小时永远到不了24，所以24表示闹钟关
				{
					TM1640_display(4, 28); // o
					TM1640_display(5, 29); // F
					TM1640_display(6, 29); // F
					TM1640_display(7, 20); // 空
				}
				else
				{
					TM1640_display(4, alhour[MENU2 - 1] / 10); // 时
					TM1640_display(5, alhour[MENU2 - 1] % 10 + 10);
					TM1640_display(6, almin[MENU2 - 1] / 10); // 分
					TM1640_display(7, almin[MENU2 - 1] % 10);
				}
			}
			if (MENU2 >= 9 && MENU2 <= 10) // 温度报警：9-10
			{
				i++;
				if (i > 300)
				{
					TM1640_display(0, 25);		  //'t'
					TM1640_display(1, 22);		  //'c'
					TM1640_display(2, MENU2 - 8); //'MENU2的值'
				}
				else
				{
					TM1640_display(0, 20);
					TM1640_display(1, 20);
					TM1640_display(2, 20);
				}
				if (i > 600)
					i = 0;

				TM1640_display(3, 20);

				// 显示对应温度报警的设置内容
				if (alhour[MENU2 - 1] == 0) // 温度报警0位关闭
				{
					TM1640_display(4, 28); // o
					TM1640_display(5, 29); // F
					TM1640_display(6, 29); // F
					TM1640_display(7, 20); // 空
				}
				else
				{
					TM1640_display(4, alhour[MENU2 - 1] / 10);
					TM1640_display(5, alhour[MENU2 - 1] % 10 + 10); // 温度
					TM1640_display(6, almin[MENU2 - 1] % 10);
					TM1640_display(7, 22);
				}
			}
			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)) // 读触摸按键的电平
			{
				i = 301; // 关闭数字闪烁
				BUZZER_KEY_BEEP();
				MENU2++;
				if (MENU2 > 10)
					MENU2 = 0;
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A))
					;
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) // 读触摸按键的电平
			{
				i = 301; // 关闭数字闪烁
				BUZZER_KEY_BEEP();

				if (MENU2 == 0)
					MENU2 = 10;
				else
					MENU2--;
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B))
					;
			}
			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)) // 读触摸按键的电平
			{
				BUZZER_KEY_BEEP();
				MENU = 101 + MENU2; // 根据菜单选项，跳到对应的设置，
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C))
					;
			}
			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)) // 读触摸按键的电平
			{
				BUZZER_QUIT_SET_BEEP();
				MENU = 0;
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D))
					;
			}
			if (MENU == 101) // 跳转到设置时间
				MENU = 4;
			if (MENU >= 102 && MENU <= 109) // 跳转到设置闹钟1-8
				MENU = 150;
			if (MENU >= 110 && MENU <= 111) // 跳转到温度报警9-10
				MENU = 210;
		}

		if (MENU == 4) // 设置年
		{
			i++;
			if (i > 300)
			{
				TM1640_display(0, ryear % 100 / 10); // 年
				TM1640_display(1, ryear % 100 % 10);
			}
			else
			{
				TM1640_display(0, 20); // 空
				TM1640_display(1, 20);
			}
			if (i > 600)
				i = 0;

			TM1640_display(2, 21);		  //-
			TM1640_display(3, rmon / 10); // 月
			TM1640_display(4, rmon % 10);
			TM1640_display(5, 21);		  //-
			TM1640_display(6, rday / 10); // 日
			TM1640_display(7, rday % 10);

			// 设置年中A键的长按操作，主要是学习理念，由于程序太臃肿，其他按键不做
			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A))
			{ // 判断长短键
				while ((!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)) && c < 100)
				{ // 循环判断长按，到时跳转
					c++;
					delay_ms(10); // 长按判断的计时
				}
				if (c >= 100)
				{ // 长键处理
					// 长按后执行的程序放到此处
					while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A))
					{
						i = 301; // 关闭数字闪烁
						ryear++;
						if (ryear > 2099)
						{
							ryear = 2000;
						}
						TM1640_display(0, ryear % 100 / 10); // 年
						TM1640_display(1, ryear % 100 % 10);
						delay_ms(100);
						BUZZER_KEY_BEEP();
					}
				}
				else
				{ // 单击处理
					// 单击后执行的程序放到此处
					BUZZER_KEY_BEEP();
					i = 301; // 关闭数字闪烁
					ryear++;
					if (ryear > 2099)
					{
						ryear = 2000;
					}
				}
				c = 0; // 参数清0
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) // 读触摸按键的电平
			{
				i = 301; // 关闭数字闪烁
				ryear--;
				if (ryear < 2000)
				{
					ryear = 2099;
				}
				BUZZER_KEY_BEEP();
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B))
					;
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)) // 读触摸按键的电平
			{
				BUZZER_KEY_BEEP();
				MENU = 5;

				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C))
					;
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)) // 读触摸按键的电平
			{
				MENU = 0;

				rup = 1;
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D))
					;
			}
		}
		if (MENU == 5) // 设置月
		{

			i++;
			TM1640_display(0, ryear % 100 / 10); // 年
			TM1640_display(1, ryear % 100 % 10);
			TM1640_display(2, 21); //-
			if (i > 300)
			{
				TM1640_display(3, rmon / 10); // 月
				TM1640_display(4, rmon % 10);
			}
			else
			{
				TM1640_display(3, 20); // 月
				TM1640_display(4, 20);
			}
			if (i > 600)
				i = 0;

			TM1640_display(5, 21);		  //-
			TM1640_display(6, rday / 10); // 日
			TM1640_display(7, rday % 10);

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)) // 读触摸按键的电平
			{
				BUZZER_KEY_BEEP();
				i = 301; // 关闭数字闪烁
				rmon++;
				if (rmon > 12)
				{
					rmon = 1;
				}

				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A))
					;
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) // 读触摸按键的电平
			{
				BUZZER_KEY_BEEP();
				i = 301; // 关闭数字闪烁
				rmon--;
				if (rmon < 1)
				{
					rmon = 12;
				}

				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B))
					;
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)) // 读触摸按键的电平
			{
				MENU = 6;

				BUZZER_KEY_BEEP();
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C))
					;
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)) // 读触摸按键的电平
			{
				MENU = 0;

				rup = 1;
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D))
					;
			}
		}
		if (MENU == 6) // 设置日
		{
			i++;
			TM1640_display(0, ryear % 100 / 10); // 年
			TM1640_display(1, ryear % 100 % 10);
			TM1640_display(2, 21);		  //-
			TM1640_display(3, rmon / 10); // 月
			TM1640_display(4, rmon % 10);
			TM1640_display(5, 21); //-
			if (i > 300)
			{
				TM1640_display(6, rday / 10); // 日
				TM1640_display(7, rday % 10);
			}
			else
			{
				TM1640_display(6, 20); // 日
				TM1640_display(7, 20);
			}
			if (i > 600)
				i = 0;

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)) // 读触摸按键的电平
			{
				i = 301; // 关闭数字闪烁
				rday++;
				if (rday > 31)
				{
					rday = 1;
				}
				BUZZER_KEY_BEEP();
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A))
					;
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) // 读触摸按键的电平
			{
				i = 301; // 关闭数字闪烁
				rday--;
				if (rday < 1)
				{
					rday = 31;
				}
				BUZZER_KEY_BEEP();
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B))
					;
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)) // 读触摸按键的电平
			{

				MENU = 7;

				BUZZER_KEY_BEEP();
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C))
					;
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)) // 读触摸按键的电平
			{
				MENU = 0;

				rup = 1;
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D))
					;
			}
		}

		if (MENU == 7) // 设置小时
		{
			i++;
			TM1640_display(0, 20); // 空
			TM1640_display(1, 20);
			if (i > 300)
			{
				TM1640_display(2, rhour / 10); // 时
				TM1640_display(3, rhour % 10 + 10);
			}
			else
			{
				TM1640_display(2, 20); // 时
				TM1640_display(3, 20);
			}
			if (i > 600)
				i = 0;

			TM1640_display(4, rmin / 10); // 分
			TM1640_display(5, rmin % 10 + 10);
			TM1640_display(6, rsec / 10); // 秒
			TM1640_display(7, rsec % 10);

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)) // 读触摸按键的电平
			{
				BUZZER_KEY_BEEP();
				i = 301; // 关闭数字闪烁
				rhour++;
				if (rhour > 23)
				{
					rhour = 0;
				}

				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A))
					;
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) // 读触摸按键的电平
			{
				i = 301; // 关闭数字闪烁
				if (rhour == 0)
				{
					rhour = 23;
				}
				else
				{
					rhour--;
				}
				BUZZER_KEY_BEEP();
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B))
					;
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)) // 读触摸按键的电平
			{
				MENU = 8;

				BUZZER_KEY_BEEP();
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C))
					;
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)) // 读触摸按键的电平
			{
				MENU = 1;

				rup = 1;
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D))
					;
			}
		}
		if (MENU == 8) // 设置分钟
		{
			i++;
			TM1640_display(0, 20); // 空
			TM1640_display(1, 20);
			TM1640_display(2, rhour / 10); // 时
			TM1640_display(3, rhour % 10 + 10);
			if (i > 300)
			{
				TM1640_display(4, rmin / 10); // 分
				TM1640_display(5, rmin % 10 + 10);
			}
			else
			{
				TM1640_display(4, 20); // 分
				TM1640_display(5, 20);
			}
			if (i > 600)
				i = 0;

			TM1640_display(6, rsec / 10); // 秒
			TM1640_display(7, rsec % 10);

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)) // 读触摸按键的电平
			{
				i = 301; // 关闭数字闪烁
				rmin++;
				if (rmin > 59)
				{
					rmin = 0;
				}
				rsec = 0;
				BUZZER_KEY_BEEP();
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A))
					;
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) // 读触摸按键的电平
			{
				BUZZER_KEY_BEEP();
				i = 301; // 关闭数字闪烁
				if (rmin == 0)
				{
					rmin = 59;
				}
				else
				{
					rmin--;
				}
				rsec = 0;

				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B))
					;
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)) // 读触摸按键的电平
			{
				MENU = 4;
				BUZZER_KEY_BEEP();
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C))
					;
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)) // 读触摸按键的电平
			{
				rsec = 0;
				MENU = 1;
				rup = 1;
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D))
					;
			}
		}

		if (MENU == 150) // 设置闹钟小时
		{
			i++;
			TM1640_display(0, 26);	  //'A'
			TM1640_display(1, 27);	  //'L'
			TM1640_display(2, MENU2); //'MENU2的值
			TM1640_display(3, 20);	  // 空
			if (i > 300)
			{
				if (alhour[MENU2 - 1] == 24)
				{
					TM1640_display(4, 28); // o
					TM1640_display(5, 29); // F
					TM1640_display(6, 29); // F
					TM1640_display(7, 20); // 空
				}
				else
				{
					TM1640_display(4, alhour[MENU2 - 1] / 10); // 时
					TM1640_display(5, alhour[MENU2 - 1] % 10 + 10);
				}
			}
			else
			{
				if (alhour[MENU2 - 1] == 24)
				{
					TM1640_display(4, 20); // 空
					TM1640_display(5, 20); // 空
					TM1640_display(6, 20); // 空
					TM1640_display(7, 20); // 空
				}
				else
				{
					TM1640_display(4, 20); // 空
					TM1640_display(5, 20);
				}
			}
			if (i > 600)
				i = 0;

			if (alhour[MENU2 - 1] != 24)
			{
				TM1640_display(6, almin[MENU2 - 1] / 10); // 分
				TM1640_display(7, almin[MENU2 - 1] % 10);
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)) // 读触摸按键的电平
			{
				alhour[MENU2 - 1]++;
				if (alhour[MENU2 - 1] > 24)
				{
					alhour[MENU2 - 1] = 0;
				}
				BUZZER_KEY_BEEP();
				i = 301; // 关闭数字闪烁
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A))
					;
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) // 读触摸按键的电平
			{
				if (alhour[MENU2 - 1] == 0)
				{
					alhour[MENU2 - 1] = 24;
				}
				else
				{
					alhour[MENU2 - 1]--;
				}
				i = 301; // 关闭数字闪烁
				BUZZER_KEY_BEEP();
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B))
					;
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)) // 读触摸按键的电平
			{
				ALFlash_W();
				if (alhour[MENU2 - 1] != 24)
				{
					MENU = 151;
				}
				else
				{
					MENU = 100;
				}
				BUZZER_KEY_BEEP();
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C))
					;
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)) // 读触摸按键的电平
			{
				ALFlash_W();
				MENU = 0;
				BUZZER_QUIT_SET_BEEP();
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D))
					;
			}
		}
		if (MENU == 151) // 设置闹钟分钟
		{
			i++;
			TM1640_display(0, 26);					   //'A'
			TM1640_display(1, 27);					   //'L'
			TM1640_display(2, MENU2);				   //'MENU2的值
			TM1640_display(3, 20);					   // 空
			TM1640_display(4, alhour[MENU2 - 1] / 10); // 时
			TM1640_display(5, alhour[MENU2 - 1] % 10 + 10);
			if (i > 300)
			{
				TM1640_display(6, almin[MENU2 - 1] / 10); // 分
				TM1640_display(7, almin[MENU2 - 1] % 10);
			}
			else
			{
				TM1640_display(6, 20); // 分
				TM1640_display(7, 20);
			}
			if (i > 600)
				i = 0;

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)) // 读触摸按键的电平
			{
				almin[MENU2 - 1]++;
				if (almin[MENU2 - 1] > 59)
				{
					almin[MENU2 - 1] = 0;
				}
				i = 301; // 关闭数字闪烁
				BUZZER_KEY_BEEP();
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A))
					;
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) // 读触摸按键的电平
			{
				if (almin[MENU2 - 1] == 0)
				{
					almin[MENU2 - 1] = 59;
				}
				else
				{
					almin[MENU2 - 1]--;
				}
				BUZZER_KEY_BEEP();
				i = 301; // 关闭数字闪烁
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B))
					;
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)) // 读触摸按键的电平
			{
				ALFlash_W();
				MENU = 100;
				BUZZER_KEY_BEEP();
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C))
					;
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)) // 读触摸按键的电平
			{
				ALFlash_W();
				MENU = 0;
				BUZZER_QUIT_SET_BEEP();
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D))
					;
			}
		}

		if (MENU == 200) // 闹钟处理
		{
			if (MENU2 < 7)
				BUZZER_SET_BEEP();
			i++;
			if (i > 1)
			{
				TM1640_display(0, 26);					   //'A'
				TM1640_display(1, 27);					   //'L'
				TM1640_display(2, MENU2);				   //'MENU2的值
				TM1640_display(3, 20);					   // 空
				TM1640_display(4, alhour[MENU2 - 1] / 10); // 时
				TM1640_display(5, alhour[MENU2 - 1] % 10 + 10);
				TM1640_display(6, almin[MENU2 - 1] / 10); // 分
				TM1640_display(7, almin[MENU2 - 1] % 10);
			}
			else
			{
				TM1640_display(0, 20); // 空
				TM1640_display(1, 20); // 空
				TM1640_display(2, 20); // 空
				TM1640_display(3, 20); // 空
				TM1640_display(4, 20); // 空
				TM1640_display(5, 20); // 空
				TM1640_display(6, 20); // 空
				TM1640_display(7, 20); // 空
			}
			if (i > 2)
				i = 0;

			// 闹钟提醒时，任意键退出
			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A) ||
				!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B) ||
				!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C) ||
				!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D))
			{

				MENU = 1;
				alFlag = 0; // 闹钟循环t次后，一般大于1分钟，如果不大于一分钟，置零后会再次判断闹钟响铃
				BUZZER_KEY_BEEP();
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A))
					;
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B))
					;
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C))
					;
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D))
					;
			}
		}

		if (MENU == 210) // 设置温度整数
		{
			i++;
			TM1640_display(0, 25);		  //'t'
			TM1640_display(1, 22);		  //'c'
			TM1640_display(2, MENU2 - 8); //'MENU2的值
			TM1640_display(3, 20);		  // 空

			if (i > 300)
			{
				if (alhour[MENU2 - 1] == 0)
				{
					TM1640_display(4, 28); // o
					TM1640_display(5, 29); // F
					TM1640_display(6, 29); // F
					TM1640_display(7, 20); // 空
				}
				else
				{
					// if (temp[0] == 0)		   // 判断温度正负
					// 	TM1640_display(3, 20); // 不显示符号
					// else
					// 	TM1640_display(3, 21); // 显示-符号
					TM1640_display(4, alhour[MENU2 - 1] / 10);
					TM1640_display(5, alhour[MENU2 - 1] % 10 + 10); // 温度
				}
			}
			else
			{
				if (alhour[MENU2 - 1] == 0)
				{
					TM1640_display(4, 20); // 空
					TM1640_display(5, 20); // 空
					TM1640_display(6, 20); // 空
					TM1640_display(7, 20); // 空
				}
				else
				{
					TM1640_display(4, 20); // 空
					TM1640_display(5, 20);
				}
			}
			if (i > 600)
				i = 0;
			if (alhour[MENU2 - 1] != 0)
			{
				TM1640_display(6, almin[MENU2 - 1] % 10);
				TM1640_display(7, 22);
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)) // 读触摸按键的电平
			{
				alhour[MENU2 - 1]++;
				if (alhour[MENU2 - 1] > 60)
				{
					alhour[MENU2 - 1] = 0;
				}
				BUZZER_KEY_BEEP();
				i = 301; // 关闭数字闪烁
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A))
					;
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) // 读触摸按键的电平
			{
				if (alhour[MENU2 - 1] == 0)
				{
					alhour[MENU2 - 1] = 60;
				}
				else
				{
					alhour[MENU2 - 1]--;
				}
				i = 301; // 关闭数字闪烁
				BUZZER_KEY_BEEP();
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B))
					;
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)) // 读触摸按键的电平
			{
				ALFlash_W();
				if (alhour[MENU2 - 1] != 0)
				{
					MENU = 211;
				}
				else
				{
					MENU = 100;
				}
				BUZZER_KEY_BEEP();
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C))
					;
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)) // 读触摸按键的电平
			{
				ALFlash_W();
				MENU = 0;
				BUZZER_QUIT_SET_BEEP();
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D))
					;
			}
		}
		if (MENU == 211) // 设置温度小数
		{
			i++;
			TM1640_display(0, 25);		  //'t'
			TM1640_display(1, 22);		  //'c'
			TM1640_display(2, MENU2 - 8); //'MENU2的值
			TM1640_display(3, 20);		  // 空

			if (i > 300)
			{
				if (alhour[MENU2 - 1] == 0)
				{
					TM1640_display(4, 28); // o
					TM1640_display(5, 29); // F
					TM1640_display(6, 29); // F
					TM1640_display(7, 20); // 空
				}
				else
				{
					// if (temp[0] == 0)		   // 判断温度正负
					// 	TM1640_display(3, 20); // 不显示符号
					// else
					// 	TM1640_display(3, 21); // 显示-符号
					TM1640_display(6, almin[MENU2 - 1] % 10);
				}
			}
			else
			{
				if (alhour[MENU2 - 1] == 0)
				{
					TM1640_display(4, 20); // 空
					TM1640_display(5, 20); // 空
					TM1640_display(6, 20); // 空
					TM1640_display(7, 20); // 空
				}
				else
					TM1640_display(6, 20);
			}
			if (i > 600)
				i = 0;
			if (alhour[MENU2 - 1] != 0)
			{
				TM1640_display(4, alhour[MENU2 - 1] / 10);
				TM1640_display(5, alhour[MENU2 - 1] % 10 + 10); // 温度
				TM1640_display(7, 22);
			}
			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A)) // 读触摸按键的电平
			{
				almin[MENU2 - 1]++;
				if (almin[MENU2 - 1] > 9)
				{
					almin[MENU2 - 1] = 0;
				}
				i = 301; // 关闭数字闪烁
				BUZZER_KEY_BEEP();
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_A))
					;
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B)) // 读触摸按键的电平
			{
				if (almin[MENU2 - 1] == 0)
				{
					almin[MENU2 - 1] = 9;
				}
				else
				{
					almin[MENU2 - 1]--;
				}
				BUZZER_KEY_BEEP();
				i = 301; // 关闭数字闪烁
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_B))
					;
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C)) // 读触摸按键的电平
			{
				ALFlash_W();
				MENU = 100;
				BUZZER_KEY_BEEP();
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_C))
					;
			}

			if (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D)) // 读触摸按键的电平
			{
				ALFlash_W();
				MENU = 0;
				BUZZER_QUIT_SET_BEEP();
				while (!GPIO_ReadInputDataBit(TOUCH_KEYPORT, TOUCH_KEY_D))
					;
			}
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
