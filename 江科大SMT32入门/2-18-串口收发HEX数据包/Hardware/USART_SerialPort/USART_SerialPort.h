#ifndef __SERIAL_PORT_H
#define __SERIAL_PORT_H

#include "stm32f10x.h" 
#include <stdio.h>
#include <stdarg.h>

extern uint8_t Serial_TxPacket[];
extern uint8_t Serial_RxPacket[];


void SerialPort_Init(void);
void SerialPort_SendByte(uint8_t Byte);
void SerialPort_SendArray(uint8_t* Array, uint16_t Length);
void SerialPort_SendString(char* String);
void SerialPort_SendNumber(uint32_t Number, uint8_t Length);
void SerialPort_Printf(char* format, ...);
uint8_t Serial_GetRxFalg(void);
void Serial_SendPacket(void);

#endif
