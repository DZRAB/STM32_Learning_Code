#ifndef __BUZZER_H
#define __BUZZER_H	 
#include "sys.h"

#define BUZZERPORT	GPIOB	//定义IO接口
#define BUZZER	GPIO_Pin_5	//定义IO接口


void BUZZER_Init(void);//初始化
void BUZZER_BEEP1(void);//响一声
void BUZZER_BOOT_BEEP(void); //开机音
void BUZZER_KEY_BEEP(void);//按键音
void BUZZER_SET_BEEP(void); //SET音
void BUZZER_QUIT_SET_BEEP(void);//退出SET音
void MIDI_PLAY(void);//MIDI音乐

		 				    
#endif
