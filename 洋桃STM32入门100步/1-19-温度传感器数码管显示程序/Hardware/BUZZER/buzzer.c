#include "buzzer.h"
#include "delay.h"
// ��С���ǡ����׶�ӦƵ��(Hz)
// 1(Do)=523, 2(Re)=587, 3(Mi)=659, 4(Fa)=698, 5(Sol)=784, 6(La)=880, 7(Si)=988

// �������ݸ�ʽ��ÿ2��Ԫ��һ�� [Ƶ��, ʱ��(ms)]
u16 little_star[] = {
    // ��һ�䣺1155665
    523, 400,   // 1
    523, 400,   // 1
    784, 400,   // 5
    784, 400,   // 5
    880, 400,   // 6
    880, 400,   // 6
    784, 800,   // 5 (����)
    
    // �ڶ��䣺4433221
    698, 400,   // 4
    698, 400,   // 4
    659, 400,   // 3
    659, 400,   // 3
    587, 400,   // 2
    587, 400,   // 2
    523, 800,   // 1 (����)
    
    // �����䣺5544332
    784, 400,   // 5
    784, 400,   // 5
    698, 400,   // 4
    698, 400,   // 4
    659, 400,   // 3
    659, 400,   // 3
    587, 800,   // 2 (����)
    
    // ���ľ䣺5544332
    784, 400,   // 5
    784, 400,   // 5
    698, 400,   // 4
    698, 400,   // 4
    659, 400,   // 3
    659, 400,   // 3
    587, 800,   // 2 (����)
    
    // ����䣺1155665
    523, 400,   // 1
    523, 400,   // 1
    784, 400,   // 5
    784, 400,   // 5
    880, 400,   // 6
    880, 400,   // 6
    784, 800,   // 5 (����)
    
    // �����䣺4433221
    698, 400,   // 4
    698, 400,   // 4
    659, 400,   // 3
    659, 400,   // 3
    587, 400,   // 2
    587, 400,   // 2
    523, 800,   // 1 (����)
};

uc16 music1[78]={ //����1�����ݱ�������������ż���ǳ��ȣ�
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

#define MUSIC_LIST happy_birthday  //�����������ݰ������������Բ��Ŷ�Ӧ������
#define MUSIC_LIST_NOTES  (sizeof(MUSIC_LIST) / sizeof(MUSIC_LIST[0]) / 2)

void BUZZER_Init(void){ //�������Ľӿڳ�ʼ��
		GPIO_InitTypeDef  GPIO_InitStructure; 	
    GPIO_InitStructure.GPIO_Pin = BUZZER; //ѡ��˿ں�                        
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //ѡ��IO�ӿڹ�����ʽ       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //����IO�ӿ��ٶȣ�2/10/50MHz��    
		GPIO_Init(BUZZERPORT, &GPIO_InitStructure);	
	
		GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(1)); //�������ӿ�����ߵ�ƽ1		
}

void BUZZER_BEEP1(void){ //��������һ��
	u16 i;
	for(i=0;i<200;i++){
		GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(0)); //�������ӿ����0
		delay_us(500); //��ʱ		
		GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(1)); //�������ӿ�����ߵ�ƽ1
		delay_us(500); //��ʱ		
	}
}
void BUZZER_BEEP2(void){ //��������һ��
	u16 i;
	for(i=0;i<200;i++){
		GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(0)); //�������ӿ����0
		delay_us(700); //��ʱ		
		GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(1)); //�������ӿ�����ߵ�ƽ1
		delay_us(700); //��ʱ		
	}
}


void MIDI_PLAY(void){ //MIDI����
	  u16 i=0;
		u16 e=0;
    u32 cycle_count=0;
    u16 freq=0;
		u16 duration=0;
    
    for(i = 0; i < MUSIC_LIST_NOTES; i++) {
        freq = MUSIC_LIST[i * 2];         // ��ȡƵ��
        duration = MUSIC_LIST[i * 2 + 1]; // ��ȡʱ��
        
        // ����ѭ����������������
        cycle_count = ((u32)freq * duration) / 1000;
        
        // ���ŵ�������
        for(e = 0; e < cycle_count; e++) {
            GPIO_WriteBit(BUZZERPORT, BUZZER, (BitAction)(0));  // ��������
            delay_us(500000 / freq);               // ��������ʱ
            GPIO_WriteBit(BUZZERPORT, BUZZER, (BitAction)(1));  // ��������
            delay_us(500000 / freq);               // ��������ʱ
        }
        
        // ������Ķ��ݼ��������ճ����
        delay_us(20000);  // 20ms���
    }  
}

//void MIDI_PLAY(void){ //MIDI����
//	u16 i,e;
//	for(i=0;i<39;i++){
//		for(e=0;e<music1[i*2]*music1[i*2+1]/1000;e++){
//			GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(0)); //�������ӿ����0
//			delay_us(500000/music1[i*2]); //��ʱ		
//			GPIO_WriteBit(BUZZERPORT,BUZZER,(BitAction)(1)); //�������ӿ�����ߵ�ƽ1
//			delay_us(500000/music1[i*2]); //��ʱ	
//		}	
//	}
//}

 

/*
ѡ��IO�ӿڹ�����ʽ��
GPIO_Mode_AIN ģ������
GPIO_Mode_IN_FLOATING ��������
GPIO_Mode_IPD ��������
GPIO_Mode_IPU ��������
GPIO_Mode_Out_PP �������
GPIO_Mode_Out_OD ��©���
GPIO_Mode_AF_PP �����������
GPIO_Mode_AF_OD ���ÿ�©���
*/
	
