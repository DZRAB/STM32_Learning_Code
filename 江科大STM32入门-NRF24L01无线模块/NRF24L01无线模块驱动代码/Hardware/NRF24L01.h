#ifndef __NRF24L01_H__
#define __NRF24L01_H__

#include "stm32f10x.h" 
#include "NRF24L01_Define.h"   

#define NRF24L01_RX_PACKET_WIDTH 4
#define NRF24L01_TX_PACKET_WIDTH 4

extern uint8_t NRF24L01_RXAddress[5];
extern uint8_t NRF24L01_TXAddress[5];
extern uint8_t NRF24L01_TXPacket[NRF24L01_TX_PACKET_WIDTH];
extern uint8_t NRF24L01_RXPacket[NRF24L01_RX_PACKET_WIDTH];

void NRF24L01_W_CSN(uint8_t BitValue);
void NRF24L01_GPIO_Init(void);
uint8_t NRF24L01_SPI_SwapByte(uint8_t Byte);
void NRF24L01_WriteReg(uint8_t RegAddress, uint8_t Data);
uint8_t NRF24L01_ReadReg(uint8_t RegAddress);
void NRF24L01_WriteRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count);
void NRF24L01_ReadRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count);
void NRF24L01_WriteTXPatload(uint8_t *DataArray, uint8_t Count);
void NRF24L01_ReadRXPatload(uint8_t *DataArray, uint8_t Count);
void NRF24L01_FlushTX(void);
void NRF24L01_FlushRX(void);
uint8_t NRF24L01_ReadStatus(void);

void NRF24L01_PowerDown(void);
void NRF24L01_Standby1(void);
void NRF24L01_RXMode(void);
void NRF24L01_TXMode(void);
void NRF24L01_Init(void);
void NRF24L01_Send(void);
uint8_t  NRF24L01_Receive(void);

#endif
