/*
 * speed.h
 *
 *  Created on: 14 sie 2026
 *      Author: rataj
 */

#ifndef INC_SPEED_H_
#define INC_SPEED_H_

#include "main.h"

extern uint32_t last_tick;
extern TIM_HandleTypeDef htim3;

void Speed_Init();

void Send_Speed(float speed);

#endif /* INC_SPEED_H_ */
