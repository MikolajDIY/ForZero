/*
 * CAN_motor.c
 *
 *  Created on: 12 sie 2026
 *      Author: rataj
 */

#include "CAN_motor.h"

extern CAN_HandleTypeDef hcan1;

static CAN_TxHeaderTypeDef TxHeader;
static uint32_t TxMailbox;
volatile uint8_t abs_alive_counter = 0;

static uint8_t msg_rpm[8] = {0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00};
static uint8_t rpm_alive_counter = 0;

static uint8_t msg_motor2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


void Send_RPM(uint16_t rpm)
{

  // 1. Aktualizacja danych dla ramki 0x280 (Obroty)
  uint16_t raw_value = rpm * 4;
  msg_rpm[2] = (uint8_t)(raw_value & 0xFF);         // LSB
  msg_rpm[3] = (uint8_t)((raw_value >> 8) & 0xFF);  // MSB

  msg_rpm[6] = rpm_alive_counter;

  /* Gdyby alive counter był wymagany
  rpm_alive_counter++;
  if(rpm_alive_counter > 0x0F){rpm_alive_counter = 0;}

  uint8_t xor = 0x02 ^ 0x80;
  for(uint8_t i =0; i<7; i++){
	xor ^= msg_rpm[i];
  }
  msg_rpm[7] = xor;
*/
  // 2. Konfiguracja nagłówka
  TxHeader.IDE = CAN_ID_STD;
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.DLC = 8;
  TxHeader.TransmitGlobalTime = DISABLE;

  // 3. Wysłanie ramki 1 (0x280 - Motor 1)
  TxHeader.StdId = 0x280;

  if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {
          HAL_CAN_AbortTxRequest(&hcan1, CAN_TX_MAILBOX0 | CAN_TX_MAILBOX1 | CAN_TX_MAILBOX2);
  }

  HAL_CAN_AddTxMessage(&hcan1, &TxHeader, msg_rpm, &TxMailbox);

  TxHeader.StdId = 0x288;
  HAL_CAN_AddTxMessage(&hcan1, &TxHeader, msg_motor2, &TxMailbox);

}

