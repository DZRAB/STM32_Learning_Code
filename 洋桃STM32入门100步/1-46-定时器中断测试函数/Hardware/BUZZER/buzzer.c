#include "buzzer.h"
#include "delay.h"
// 《小星星》简谱对应频率(Hz)
// 1(Do)=523, 2(Re)=587, 3(Mi)=659, 4(Fa)=698, 5(Sol)=784, 6(La)=880, 7(Si)=988

// 音乐数据格式：每2个元素一组 [频率, 时长(ms)]
u16 little_star[] = {
    // 第一句：1155665
    523, 400,   // 1
    523, 400,   // 1
    784, 400,   // 5
    784, 400,   // 5
    880, 400,   // 6
    880, 400,   // 6
    784, 800,   // 5 (长音)
    
    // 第二句：4433221
    698, 400,   // 4
    698, 400,   // 4
    659, 400,   // 3
    659, 400,   // 3
    587, 400,   // 2
    587, 400,   // 2
    523, 800,   // 1 (长音)
    
    // 第三句：5544332
    784, 400,   // 5
    784, 400,   // 5
    698, 400,   // 4
    698, 400,   // 4
    659, 400,   // 3
    659, 400,   // 3
    587, 800,   // 2 (长音)
    
    // 第四句：5544332
    784, 400,   // 5
    784, 400,   // 5
    698, 400,   // 4
    698, 400,   // 4
    659, 400,   // 3
    659, 400,   // 3
    587, 800,   // 2 (长音)
    
    // 第五句：1155665
    523, 400,   // 1
    523, 400,   // 1
    784, 400,   // 5
    784, 400,   // 5
    880, 400,   // 6
    880, 400,   // 6
    784, 800,   // 5 (长音)
    
    // 第六句：4433221
    698, 400,   // 4
    698, 400,   // 4
    659, 400,   // 3
    659, 400,   // 3
    587, 400,   // 2
    587, 400,   // 2
    523, 800,   // 1 (长音)
};

uc16 music1[78]={ //音乐1的数据表（奇数是音调，偶数是长度）
330,750,
440,375,
494,375,
523,750,
587,375,
659,375,
587,750,
494,375,
392,375,
440,1500,
330,750,
440,375,
494,375,
523,750,
587,375,
659,375,
587,750,
494,375,
392,375,
784,1500,
659,750,
698,375,
784,375,
880,750,
784,375,
698,375,
659,750,
587,750,
659,750,
523,375,
494,375,
440,750,
440,375,
494,375,
523,750,
523,750,
494,750,
392,750,
440,3000
};
u16 two_tigers[] = {
    523, 400, 587, 400, 659, 400, 523, 400,  // 1 2 3 1
    523, 400, 587, 400, 659, 400, 523, 400,  // 1 2 3 1
    659, 400, 698, 400, 784, 800,            // 3 4 5
    659, 400, 698, 400, 784, 800,            // 3 4 5
    784, 200, 880, 200, 784, 200, 698, 200, 659, 400, 523, 400,  // 5 6 5 4 3 1
    784, 200, 880, 200, 784, 200, 698, 200, 659, 400, 523, 400,  // 5 6 5 4 3 1
    587, 400, 392, 400, 523, 800,            // 2 7 1
    587, 400, 392, 400, 523, 800             // 2 7 1
};
u16 happy_birthday[] = {
    523, 500, 523, 500, 587, 1000, 523, 1000, 659, 1000, 587, 2000,
    523, 500, 523, 500, 587, 1000, 523, 1000, 698, 1000, 659, 2000,
    523, 500, 523, 500, 1047, 1000, 880, 1000, 659, 1000, 587, 1000, 698, 1000,
    988, 500, 988, 500, 880, 1000, 659, 1000, 698, 1000, 659, 2000
};

#define MUSIC_LIST happy_birthday  //更改音乐数据包数组名，可以播放对应的音乐
#define MUSIC_LIST_NOTES  (sizeof(MUSIC_LIST) / sizeof(MUSIC_LIST[0]) / 2)

void BUZZER_Init(void){ //蜂鸣器的接口初始化
		GPIO_InitTypeDef  GPIO_InitStructure; 	
    GPIO_InitStructure.GPIO_Pin = BUZZER; //选择端口号                        
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //选择IO接口工作方式       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //设置IO接口速度（2/10/50MHz）    
		GPIO_Init(BUZZERPORT, &GPIO_InitStructure);	
	
		GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(1)); //蜂鸣器接口输出高电平1		
}

void BUZZER_BEEP1(void){ //蜂鸣器响一声
	u16 i;
	for(i=0;i<200;i++){
		GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(0)); //蜂鸣器接口输出0
		delay_us(500); //延时		
		GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(1)); //蜂鸣器接口输出高电平1
		delay_us(500); //延时		
	}
}
void BUZZER_BEEP2(void){ //蜂鸣器响一声
	u16 i;
	for(i=0;i<200;i++){
		GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(0)); //蜂鸣器接口输出0
		delay_us(700); //延时		
		GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(1)); //蜂鸣器接口输出高电平1
		delay_us(700); //延时		
	}
}


void MIDI_PLAY(void){ //MIDI音乐
	  u16 i=0;
		u16 e=0;
    u32 cycle_count=0;
    u16 freq=0;
		u16 duration=0;
    
    for(i = 0; i < MUSIC_LIST_NOTES; i++) {
        freq = MUSIC_LIST[i * 2];         // 获取频率
        duration = MUSIC_LIST[i * 2 + 1]; // 获取时长
        
        // 计算循环次数（周期数）
        cycle_count = ((u32)freq * duration) / 1000;
        
        // 播放单个音符
        for(e = 0; e < cycle_count; e++) {
            GPIO_WriteBit(BUZZERPORT, BUZZER, (BitAction)(0));  // 蜂鸣器关
            delay_us(500000 / freq);               // 半周期延时
            GPIO_WriteBit(BUZZERPORT, BUZZER, (BitAction)(1));  // 蜂鸣器开
            delay_us(500000 / freq);               // 半周期延时
        }
        
        // 音符间的短暂间隔（避免粘连）
        delay_us(20000);  // 20ms间隔
    }  
}

//void MIDI_PLAY(void){ //MIDI音乐
//	u16 i,e;
//	for(i=0;i<39;i++){
//		for(e=0;e<music1[i*2]*music1[i*2+1]/1000;e++){
//			GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(0)); //蜂鸣器接口输出0
//			delay_us(500000/music1[i*2]); //延时		
//			GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(1)); //蜂鸣器接口输出高电平1
//			delay_us(500000/music1[i*2]); //延时	
//		}	
//	}
//}

 

/*
选择IO接口工作方式：
GPIO_Mode_AIN 模拟输入
GPIO_Mode_IN_FLOATING 浮空输入
GPIO_Mode_IPD 下拉输入
GPIO_Mode_IPU 上拉输入
GPIO_Mode_Out_PP 推挽输出
GPIO_Mode_Out_OD 开漏输出
GPIO_Mode_AF_PP 复用推挽输出
GPIO_Mode_AF_OD 复用开漏输出
*/
	
