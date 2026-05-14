#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "LED.h"
#include "Timer.h"
#include "Key.h"
#include "MPU6050.h"
#include "Motor.h"
#include "Encoder.h"
//#include "Serial.h"				// [注] PA9/PA10已改为循迹模块的GPIO输入，放弃USART1串口调试功能
#include "BlueSerial.h"
#include "PID.h"
#include "Track.h"					// 四路巡线寻迹模块
#include <math.h>
#include <string.h>
#include <stdlib.h>

/*定义全局变量*/
int16_t AX, AY, AZ, GX, GY, GZ;		//读取MPU6050的原始数据
uint8_t TimerErrorFlag;	//定时器错误标志位，如果定时中断函数执行时间超过了定时时间，则此标志位置1
uint16_t TimerCount;	//定时器计数值，此值可用于计算定时中断函数具体的执行时间

float AngleAcc;			//由加速度计得到的角度值
float AngleGyro;		//由陀螺仪得到的角度值，执行互补滤波后，此值基本与Angle相等
float Angle;			//互补滤波后的角度值，准确且无漂移

uint8_t KeyNum, RunFlag;			//按键键码，运行标志位
uint8_t TrackModeFlag = 0;			//循迹模式标志位（0=遥控模式，1=循迹模式）

int16_t LeftPWM, RightPWM;			//左PWM，右PWM
int16_t AvePWM, DifPWM;				//平均PWM，差分PWM

float LeftSpeed, RightSpeed;		//左速度，右速度
float AveSpeed, DifSpeed;			//平均速度，差分速度

/*定义PID结构体变量*/
PID_t AnglePID = {					//角度环PID结构体变量，定义的时候同时给部分成员赋初值
	.Kp = 5,						//比例项权重
	.Ki = 0.1,						//积分项权重
	.Kd = 5,						//微分项权重
	
	.OutMax = 100,					//输出限幅的最大值
	.OutMin = -100,					//输出限幅的最小值
	
	.OutOffset = 3,					//输出偏移
	
	.ErrorIntMax = 600,				//误差积分的最大值
	.ErrorIntMin = -600,			//误差积分的最小值
};

PID_t SpeedPID = {					//速度环PID结构体变量，定义的时候同时给部分成员赋初值
	.Kp = 2,						//比例项权重
	.Ki = 0.05,						//积分项权重
	.Kd = 0,						//微分项权重
	
	.OutMax = 20,					//输出限幅的最大值
	.OutMin = -20,					//输出限幅的最小值
	
	.ErrorIntMax = 150,				//误差积分的最大值
	.ErrorIntMin = -150,			//误差积分的最小值
};

PID_t TurnPID = {					//转向环PID结构体变量，定义的时候同时给部分成员赋初值
	.Kp = 4,						//比例项权重
	.Ki = 3,						//积分项权重
	.Kd = 0,						//微分项权重
	
	.OutMax = 50,					//输出限幅的最大值
	.OutMin = -50,					//输出限幅的最小值
	
	.ErrorIntMax = 20,				//误差积分的最大值
	.ErrorIntMin = -20,				//误差积分的最小值
};

/**************************************************************************************
 * 循迹环PID（TrackPID）- 四路巡线寻迹专用PID控制器
 * 
 * 【循迹环在PID控制体系中的位置】
 * 循迹环PID是整个平衡车控制系统的"最外层"控制器。控制层级如下：
 * 
 *   循迹环(TrackPID) --[Target]-> 转向环(TurnPID) --[DifPWM]-> 差分PWM(左右转向)
 *   循迹速度设定值  --[Target]-> 速度环(SpeedPID) --[Target]-> 角度环(AnglePID) -> AvePWM(前后行进)
 *   角度环(AnglePID) --[AvePWM + DifPWM]-> 左右电机PWM -> 平衡车姿态
 * 
 * 【循迹环PID的工作原理】
 * - Actual（实际值）：Track_Error，即循迹传感器计算出的黑线位置误差
 *   （范围约-30~+30，负值表示线偏左，正值表示线偏右，0表示在正中心）
 * - Target（目标值）：0，目标是使黑线始终保持在传感器阵列正中心
 * - Out（输出值）：输出给转向环的转向目标值（范围受OutMax/OutMin限制）
 * 
 * 【循迹环PID参数调节建议】
 *   Kp（比例项）：决定转向的响应强度。值越大，转向越灵敏，但过大会导致左右摆动
 *   Ki（积分项）：消除静态误差。当小车持续偏左或偏右时，积分项逐渐累加纠偏
 *   Kd（微分项）：抑制摆动。当小车左右摆动时，微分项提供阻尼效果
 *   
 *   典型初始值建议：Kp=0.3~0.8, Ki=0.01~0.05, Kd=0.1~0.3
 *   实际值需要根据路面状况和小车机械特性进行现场调试
 **************************************************************************************/
PID_t TrackPID = {					//循迹环PID结构体变量
	.Kp = 0.5,						//比例项权重：决定循迹响应速度，越大转向越灵敏
	.Ki = 0.02,						//积分项权重：消除循迹稳态误差
	.Kd = 0.2,						//微分项权重：抑制循迹过程中的摆动，增加循迹丝滑度
	
	.OutMax = 30,					//输出限幅最大值，限制最大转向目标值
	.OutMin = -30,					//输出限幅最小值
	
	.ErrorIntMax = 50,				//误差积分最大值，防止积分饱和
	.ErrorIntMin = -50,				//误差积分最小值
};

int main(void)
{
	/*模块初始化*/
	OLED_Init();		//OLED初始化
	MPU6050_Init();		//MPU6050初始化
	BlueSerial_Init();	//蓝牙串口初始化
	LED_Init();			//LED初始化
	Key_Init();			//按键初始化
	Motor_Init();		//电机初始化
	Encoder_Init();		//编码器初始化
	Track_Init();		//四路巡线寻迹模块初始化（PA9/PA10/PC14/PC15）
	//Serial_Init();	//USART1调试串口初始化已取消，PA9/PA10已用于循迹模块
	
	Timer_Init();		//定时器初始化，定时中断时间1ms
	
	while (1)
	{
		/*LED指示程序运行状态*/
		if (RunFlag) {LED_ON();} else {LED_OFF();}		//RunFlag非0时LED点亮
		
		/*按键控制*/
		KeyNum = Key_GetNum();		//获取键码
		if (KeyNum == 1)			//如果K1按下（PB1）：切换平衡启停
		{
			/*RunFlag变量取非*/
			if (RunFlag == 0)		//如果RunFlag为0
			{
				PID_Init(&AnglePID);//启动程序时给PID初始化，清除之前可能遗留的参数
				PID_Init(&SpeedPID);
				PID_Init(&TurnPID);
				PID_Init(&TrackPID);	//循迹PID也一并初始化
				RunFlag = 1;		//则RunFlag置1
			}
			else					//否则，RunFlag非0
			{
				RunFlag = 0;		//则RunFlag置0
				TrackModeFlag = 0;	//同时退出循迹模式
				Track_Enabled = 0;	//关闭循迹使能
			}
		}
		/* ========== K2按键：循迹模式控制 ========== */
		if (KeyNum == 2)			//如果K2按下（PB0）：切换循迹模式
		{
			TrackModeFlag = !TrackModeFlag;		//切换循迹模式标志
			if (TrackModeFlag)					//如果开启循迹模式
			{
				if (RunFlag == 0)				//如果当前未运行，先启动平衡
				{
					PID_Init(&AnglePID);
					PID_Init(&SpeedPID);
					PID_Init(&TurnPID);
					PID_Init(&TrackPID);
					RunFlag = 1;
				}
				Track_Enabled = 1;				//开启循迹使能
				PID_Init(&TrackPID);			//初始化循迹PID，清除历史数据
				Track_Speed = TRACK_NORMAL_SPEED;	//设置为正常循迹速度
			}
			else
			{
				Track_Enabled = 0;				//关闭循迹使能
				/* 退出循迹模式后，重置速度/转向目标，让小车回归平衡 */
				SpeedPID.Target = 0;
				TurnPID.Target = 0;
			}
		}
		
		/*OLED显示*/
		OLED_Clear();
		OLED_Printf(0, 0, OLED_6X8, "  Angle");						//OLED左侧显示角度环参数
		OLED_Printf(0, 8, OLED_6X8, "P:%05.2f", AnglePID.Kp);		//角度环Kp
		OLED_Printf(0, 16, OLED_6X8, "I:%05.2f", AnglePID.Ki);		//角度环Ki
		OLED_Printf(0, 24, OLED_6X8, "D:%05.2f", AnglePID.Kd);		//角度环Kd
		OLED_Printf(0, 32, OLED_6X8, "T:%+05.1f", AnglePID.Target);	//角度环目标值
		OLED_Printf(0, 40, OLED_6X8, "A:%+05.1f", Angle);			//角度环实际值
		OLED_Printf(0, 48, OLED_6X8, "O:%+05.0f", AnglePID.Out);	//角度环输出值
		OLED_Printf(0, 56, OLED_6X8, "GY:%+05d", GY);				//显示GY，便于校准陀螺仪零漂
		OLED_Printf(56, 56, OLED_6X8, "Offset:%02.0f", AnglePID.OutOffset);		//显示输出偏移值
		
		/* OLED中间显示区域：根据模式显示不同内容 */
		if (TrackModeFlag)		//循迹模式下显示循迹相关信息
		{
			/* OLED中间显示循迹环参数（取代速度环显示） */
			OLED_Printf(44, 0, OLED_6X8, "Track");					//OLED中间显示"Track"
			OLED_Printf(38, 8, OLED_6X8, "P:%05.2f", TrackPID.Kp);	//循迹环Kp
			OLED_Printf(38, 16, OLED_6X8, "I:%05.2f", TrackPID.Ki);//循迹环Ki
			OLED_Printf(38, 24, OLED_6X8, "D:%05.2f", TrackPID.Kd);//循迹环Kd
			OLED_Printf(38, 32, OLED_6X8, "E:%+05.1f", Track_Error);//循迹误差
			OLED_Printf(38, 40, OLED_6X8, "Sp:%05.2f", Track_Speed);//循迹速度
			/* 显示传感器状态（用字符图形表示4个传感器在线状态） */
			/* 格式：|S1|S2|S3|S4|，*表示检测到黑线，-表示未检测到 */
			OLED_Printf(38, 48, OLED_6X8, "|%c|%c|%c|%c|",
				(Track_SensorValues & 0x08) ? '*' : '-',
				(Track_SensorValues & 0x04) ? '*' : '-',
				(Track_SensorValues & 0x02) ? '*' : '-',
				(Track_SensorValues & 0x01) ? '*' : '-');
			/* 显示循迹模式状态 */
			OLED_Printf(38, 56, OLED_6X8, "Mode:TRACK");			//显示当前模式为循迹模式
		}
		else					//遥控模式下显示速度环和转向环参数（原有显示）
		{
			OLED_Printf(50, 0, OLED_6X8, "Speed");						//OLED中间显示速度环参数
			OLED_Printf(50, 8, OLED_6X8, "%05.2f", SpeedPID.Kp);		//速度环Kp
			OLED_Printf(50, 16, OLED_6X8, "%05.2f", SpeedPID.Ki);		//速度环Ki
			OLED_Printf(50, 24, OLED_6X8, "%05.2f", SpeedPID.Kd);		//速度环Kd
			OLED_Printf(50, 32, OLED_6X8, "%+05.1f", SpeedPID.Target);	//速度环目标值
			OLED_Printf(50, 40, OLED_6X8, "%+05.1f", AveSpeed);			//速度环实际值
			OLED_Printf(50, 48, OLED_6X8, "%+05.0f", SpeedPID.Out);		//速度环输出值
			OLED_Printf(50, 56, OLED_6X8, "Mode:REMOTE");				//显示当前模式为遥控模式
		}
		
		OLED_Printf(88, 0, OLED_6X8, "Turn");						//OLED右侧显示转向环参数
		OLED_Printf(88, 8, OLED_6X8, "%05.2f", TurnPID.Kp);			//转向环Kp
		OLED_Printf(88, 16, OLED_6X8, "%05.2f", TurnPID.Ki);		//转向环Ki
		OLED_Printf(88, 24, OLED_6X8, "%05.2f", TurnPID.Kd);		//转向环Kd
		OLED_Printf(88, 32, OLED_6X8, "%+05.1f", TurnPID.Target);	//转向环目标值
		OLED_Printf(88, 40, OLED_6X8, "%+05.1f", DifSpeed);			//转向环实际值
		OLED_Printf(88, 48, OLED_6X8, "%+05.0f", TurnPID.Out);		//转向环输出值
		OLED_Update();				//OLED更新
		
		/*蓝牙串口接收数据包处理*/
		/*规定的数据包格式为：[数据1,数据2,数据3,...]*/
		if (BlueSerial_RxFlag == 1)		//如果收到数据包
		{
			char *Tag = strtok(BlueSerial_RxPacket, ",");	//提取数据1，定义为标签Tag
			if (strcmp(Tag, "key") == 0)					//Tag为key，收到按键数据包
			{
				char *Name = strtok(NULL, ",");				//提取数据2，定义为按键名称
				char *Action = strtok(NULL, ",");			//提取数据3，定义为按键动作
				
				/*此处可执行按键操作，目前程序暂时没用到按键*/
			}
			else if (strcmp(Tag, "slider") == 0)			//Tag为slider，收到滑杆数据包
			{
				char *Name = strtok(NULL, ",");				//提取数据2，定义为滑杆名称
				char *Value = strtok(NULL, ",");			//提取数据3，定义为滑杆值
				
				/*执行滑杆操作*/
				if (strcmp(Name, "AngleKp") == 0)			//如果滑杆名称是AngleKp
				{
					AnglePID.Kp = atof(Value);				//则把滑杆值赋值给角度环Kp
				}
				else if (strcmp(Name, "AngleKi") == 0)		//如果滑杆名称是AngleKi
				{
					AnglePID.Ki = atof(Value);				//则把滑杆值赋值给角度环Ki
				}
				else if (strcmp(Name, "AngleKd") == 0)		//如果滑杆名称是AngleKd
				{
					AnglePID.Kd = atof(Value);				//则把滑杆值赋值给角度环Kd
				}
				else if (strcmp(Name, "SpeedKp") == 0)		//如果滑杆名称是SpeedKp
				{
					SpeedPID.Kp = atof(Value);				//则把滑杆值赋值给速度环Kp
				}
				else if (strcmp(Name, "SpeedKi") == 0)		//如果滑杆名称是SpeedKi
				{
					SpeedPID.Ki = atof(Value);				//则把滑杆值赋值给速度环Ki
				}
				else if (strcmp(Name, "SpeedKd") == 0)		//如果滑杆名称是SpeedKd
				{
					SpeedPID.Kd = atof(Value);				//则把滑杆值赋值给速度环Kd
				}
				else if (strcmp(Name, "TurnKp") == 0)		//如果滑杆名称是TurnKp
				{
					TurnPID.Kp = atof(Value);				//则把滑杆值赋值给转向环Kp
				}
				else if (strcmp(Name, "TurnKi") == 0)		//如果滑杆名称是TurnKi
				{
					TurnPID.Ki = atof(Value);				//则把滑杆值赋值给转向环Ki
				}
				else if (strcmp(Name, "TurnKd") == 0)		//如果滑杆名称是TurnKd
				{
					TurnPID.Kd = atof(Value);				//则把滑杆值赋值给转向环Kd
				}
				else if (strcmp(Name, "Offset") == 0)		//如果滑杆名称是Offset
				{
					AnglePID.OutOffset = atof(Value);		//则把滑杆值赋值给OutOffset
				}
				else if (strcmp(Name, "TrackKp") == 0)		//如果滑杆名称是TrackKp（循迹PID Kp）
				{
					TrackPID.Kp = atof(Value);				//则把滑杆值赋值给循迹环Kp
				}
				else if (strcmp(Name, "TrackKi") == 0)		//如果滑杆名称是TrackKi（循迹PID Ki）
				{
					TrackPID.Ki = atof(Value);				//则把滑杆值赋值给循迹环Ki
				}
				else if (strcmp(Name, "TrackKd") == 0)		//如果滑杆名称是TrackKd（循迹PID Kd）
				{
					TrackPID.Kd = atof(Value);				//则把滑杆值赋值给循迹环Kd
				}
				else if (strcmp(Name, "TrackSpd") == 0)		//如果滑杆名称是TrackSpd（循迹速度）
				{
					Track_SetSpeed(atof(Value));			//则设置新的循迹速度
				}
			}
			else if (strcmp(Tag, "joystick") == 0)			//Tag为joystick，收到摇杆数据包
			{
				int8_t LH = atoi(strtok(NULL, ","));		//提取数据2，定义为摇杆值LH
				int8_t LV = atoi(strtok(NULL, ","));		//提取数据3，定义为摇杆值LV
				int8_t RH = atoi(strtok(NULL, ","));		//提取数据4，定义为摇杆值RH
				int8_t RV = atoi(strtok(NULL, ","));		//提取数据5，定义为摇杆值RV
				
				/*执行摇杆操作*/
				/* 注意：循迹模式下，摇杆控制被禁用，由循迹模块自动控制速度/转向 */
				if (!TrackModeFlag)		//仅在遥控模式下响应摇杆控制
				{
					SpeedPID.Target = LV / 25.0;	//摇杆值LV缩放后，控制速度环目标值，前后行进控制
					TurnPID.Target = RH / 25.0;	//摇杆值RH缩放后，控制转向环目标值，左右转弯控制
				}
			}
			
			BlueSerial_RxFlag = 0;				//处理完成后，标志位置0，允许接收下一个数据包
		}
		
		/*蓝牙串口打印波形，需配合蓝牙串口小程序实现波形绘制*/
//		BlueSerial_Printf("[plot,%f,%f,%f]", AnglePID.ErrorInt, SpeedPID.ErrorInt, TurnPID.ErrorInt);
	}
}

void TIM1_UP_IRQHandler(void)
{
	static uint16_t Count0, Count1;
	
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
	{
		/*定时中断函数1ms自动执行一次*/
		
		/*进入中断函数后，立刻清标志位*/
		/*如果中断函数退出前，标志位又置1了，说明中断函数执行时间超过了定时时间（1ms）*/
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
		
		Key_Tick();				//调用按键的Tick函数
		
		/*计次分频*/
		Count0 ++;				//计次自增
		if (Count0 >= 10)		//如果计次10次，则if成立，即if每隔10ms进一次
		{
			Count0 = 0;			//计次清零，便于下次计次
			
			/*在中断里读取MPU6050，可以保证读取间隔严格为1ms*/
			/*但要保证MPU6050_GetData执行时间不超过1ms*/
			MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);
			
			/*校准陀螺仪Y轴零漂*/
			/*此值需实测确定，不同的设备零漂一般不同*/
			/*实测方法是，在完全静止时，观察OLED显示的GY值，即为零漂值*/
			/*然后在此处将零漂值减去，使得完全静止时，GY值为0*/
			GY -= 16;
			
			/*由加速度计计算得到角度值*/
			/*atan2计算反正切，得到角度（弧度制）， / 3.14159 * 180可将弧度制转为角度值*/
			AngleAcc = -atan2(AX, AZ) / 3.14159 * 180;
			
			/*校准中心角度*/
			/*此值需实测确定，不同的设备中心角度一般不同*/
			/*实测方法是，使平衡车绝对竖直，观察OLED显示的角度环实际值，即为中心角度偏移值*/
			/*然后在此处将偏移值减去，使得绝对竖直时，角度环实际值为0*/
			AngleAcc += 0.5;
			
			/*由陀螺仪积分得到角度值*/
			/*互补滤波下，角度积分要在上次滤波后的Angle上进行*/
			/*公式中32768是int16_t变量的最大值，2000是陀螺仪配置的满量程2000度每秒，0.01是间隔时间10ms*/
			AngleGyro = Angle + GY / 32768.0 * 2000 * 0.01;
			
			/*执行互补滤波*/
			float Alpha = 0.01;		//互补滤波系数，值越大，越偏向于加速度计，值越小，越偏向于陀螺仪
			Angle = Alpha * AngleAcc + (1 - Alpha) * AngleGyro;		//互补滤波计算，得到稳定的角度值
			
			/*平衡车倒地后自动停止PID程序*/
			if (Angle > 50 || Angle < -50)	//角度超过-50度~50度的范围，认为平衡车倒地了
			{
				RunFlag = 0;				//RunFlag置0，停止PID程序
				TrackModeFlag = 0;			//退出循迹模式
				Track_Enabled = 0;			//关闭循迹使能
			}
			
			/*执行PID调控程序*/
			if (RunFlag)					//RunFlag非0时，启动PID程序
			{
				/*角度环PID控制*/
				AnglePID.Actual = Angle;	//角度环实际值为Angle
				PID_Update(&AnglePID);		//调用封装好的函数，一步完成PID计算和更新
				AvePWM = -AnglePID.Out;		//角度环的输出值给到电机平均PWM，用于控制前后行进
				
				/*控制量转换*/
				LeftPWM = AvePWM + DifPWM / 2;		//由平均PWM和差分PWM计算得到左轮PWM
				RightPWM = AvePWM - DifPWM / 2;		//由平均PWM和差分PWM计算得到右轮PWM
				
				/*PWM限幅*/
				/*上式计算后，LeftPWM和RightPWM可能会超出电机允许的PWM范围，此处将PWM值范围限制在-100~100之内*/
				if (LeftPWM > 100) {LeftPWM = 100;} else if (LeftPWM < -100) {LeftPWM = -100;}
				if (RightPWM > 100) {RightPWM = 100;} else if (RightPWM < -100) {RightPWM = -100;}
				
				/*PWM输出给电机*/
				Motor_SetPWM(1, LeftPWM);		//LeftPWM输出给左轮电机
				Motor_SetPWM(2, RightPWM);		//RightPWM输出给右轮电机
			}
			else		//RunFlag为0时，停止PID程序
			{
				/*左右电机PWM均设置为0*/
				Motor_SetPWM(1, 0);
				Motor_SetPWM(2, 0);
			}
		}
		
		/*计次分频*/
		Count1 ++;				//计次自增
		if (Count1 >= 50)		//如果计次50次，则if成立，即if每隔50ms进一次
		{
			Count1 = 0;			//计次清零，便于下次计次
			
			/*获取编码器的计次值增量，并计算电机旋转速度*/
			/*Encoder_Get函数，可以获取两次读取编码器的计次值增量*/
			/*编码器磁铁旋转一圈，计次增量为44，编码器读取间隔是50ms（0.05s）*/
			/*因此磁铁旋转速度 = 计次增量 / 44 / 0.05，单位是转每秒*/
			/*平衡车使用的电机带有减速箱，减速比为9.27666*/
			/*因此电机输出轴旋转速度 = 磁铁旋转速度 / 9.27666，单位是转每秒*/
			LeftSpeed = Encoder_Get(1) / 44.0 / 0.05 / 9.27666;
			RightSpeed = Encoder_Get(2) / 44.0 / 0.05 / 9.27666;
			
			/*信号量转换*/
			AveSpeed = (LeftSpeed + RightSpeed) / 2.0;	//由左轮速度和右轮速度计算得到平均速度
			DifSpeed = LeftSpeed - RightSpeed;			//由左轮速度和右轮速度计算得到差分速度
			
			/* ========== 循迹模块：读取传感器并计算位置误差 ========== */
			/* 
			 * 循迹传感器读取放在50ms周期中，与编码器读取同一周期。
			 * 读取频率20Hz足够满足循迹控制需求，同时避免过多占用中断时间。
			 * Track_Update()内部使用加权质心法计算黑线位置误差Track_Error。
			 */
			if (Track_Enabled)
			{
				Track_Update();
			}
			
			/*执行PID调控程序*/
			if (RunFlag)					//RunFlag非0时，启动PID程序
			{
				/* ========== 循迹模式下的PID控制 ========== */
				/*
				 * 循迹模式下，循迹环(TrackPID)替代摇杆控制速度环和转向环的目标值。
				 * 控制流程：
				 *   1. 循迹传感器读取位置误差Track_Error
				 *   2. TrackPID以Track_Error为实际值，0为目标值进行PID运算
				 *   3. TrackPID.Out输出给转向环目标值(TurnPID.Target)，控制左右转向
				 *   4. Track_Speed作为目标速度值，输出给速度环目标值(SpeedPID.Target)
				 *   5. 速度环和转向环继续按现有逻辑运行
				 * 
				 * 脱轨状态处理：
				 *   脱轨时，Track_Update内部会设置搜索误差，TrackPID会响应此搜索误差，
				 *   使小车左右摆动寻找黑线。找到后Track_Error回到正常值，循迹恢复。
				 *   脱轨期间适当降低速度(Track_Speed = TRACK_SEARCH_SPEED)，
				 *   方便更好地找回黑线。
				 */
				if (Track_Enabled)		//循迹模式
				{
					/* 循迹环PID控制（TrackPID） */
					/*
					 * TrackPID是循迹控制的核心：
					 * - Target=0：目标是使黑线位于传感器正中心（Track_Error=0）
					 * - Actual=Track_Error：当前黑线位置误差
					 * - Out：输出给转向环的目标值，决定转向方向和大小
					 */
					TrackPID.Target = 0;				//循迹目标：误差为0（线在正中心）
					TrackPID.Actual = Track_Error;		//循迹实际值：当前黑线位置误差
					PID_Update(&TrackPID);				//执行循迹PID计算
					
					/* 
					 * 循迹速度控制：脱轨时减速便于找回，在线时恢复正常速度。
					 * Track_GetSpeed()获取用户通过蓝牙设置的目标速度，
					 * 脱轨搜索时强制使用TRACK_SEARCH_SPEED低速搜索。
					 */
					float TrackEffectiveSpeed;			//当前有效的循迹速度
					if (Track_IsLost())
					{
						TrackEffectiveSpeed = TRACK_SEARCH_SPEED;	//脱轨搜索速度（低速）
					}
					else
					{
						TrackEffectiveSpeed = Track_GetSpeed();		//正常循迹速度（用户设置值）
					}
					
					SpeedPID.Target = TrackEffectiveSpeed;	//循迹有效速度给到速度环
					TurnPID.Target = TrackPID.Out;			//循迹转向输出给到转向环目标值
				}
				else						//遥控模式（原有逻辑）
				{
					/* 遥控模式下速度/转向目标已由蓝牙摇杆在main loop中设置 */
				}
				
				/*速度环PID控制*/
				SpeedPID.Actual = AveSpeed;			//速度环实际值为AveSpeed
				PID_Update(&SpeedPID);				//调用封装好的函数，一步完成PID计算和更新
				AnglePID.Target = SpeedPID.Out;		//速度环的输出值给到角度环的目标值，构成串级PID
				
				/*转向环PID控制*/
				TurnPID.Actual = DifSpeed;			//转向环实际值为DifSpeed
				PID_Update(&TurnPID);				//调用封装好的函数，一步完成PID计算和更新
				DifPWM = TurnPID.Out;				//转向环的输出值给到电机差分PWM，用于控制左右转弯
			}
		}
		
		/*中断函数退出前，再次检查标志位*/
		if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
		{
			/*标志位又置1了，说明中断函数执行时间超过了定时时间（1ms）*/
			/*置TimerErrorFlag为1，表示定时中断错误*/
			TimerErrorFlag = 1;
			
			/*清标志位，避免中断连续触发，导致主函数完全无法执行*/
			TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
		}
		
		/*中断函数退出前，读取计数器的值，此值可用于测量中断函数的具体执行时间*/
		TimerCount = TIM_GetCounter(TIM1);
	}
}
