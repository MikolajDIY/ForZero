/*
 * tell_tales.h
 *
 *  Created on: 10 sie 2026
 *      Author: rataj
 */

#ifndef INC_TELL_TALES_H_
#define INC_TELL_TALES_H_

#include "main.h"
#include <stdbool.h>
typedef struct {
	GPIO_TypeDef *port;
	uint16_t pin;
} GPIO_Pin_t;

// LEDs built-in driver
extern const GPIO_Pin_t LED_OK;
extern const GPIO_Pin_t LED_WARNING;

// Cluster Tell-Tales
extern const GPIO_Pin_t TCR;
extern const GPIO_Pin_t BRAKE_PADS;
extern const GPIO_Pin_t ABS;
extern const GPIO_Pin_t SENSOR;
extern const GPIO_Pin_t COOL_LVL;
extern const GPIO_Pin_t AIRBAG;
extern const GPIO_Pin_t WIPER_FLUID;
extern const GPIO_Pin_t BRAKES;

typedef enum{
	ABS_BIT = (1 << 0), 		// 0-bit
	TCR_BIT = (1 << 1), 		// 1-bit
	BRAKES_BIT = (1 << 2),		// 2-bit
	BRAKE_PADS_BIT = (1 << 3), 	// 3-bit
	SENSOR_BIT = (1 << 4),		// 4-bit
	COOL_LVL_BIT = (1 << 5), 	// 5-bit
	WIPER_FLUID_BIT = (1 << 6), // 6-bit
	AIRBAG_BIT = (1 << 7)		// 7-bit
}IndicatorsBit_t;

typedef struct{
	const GPIO_Pin_t *pin;
	IndicatorsBit_t mask;
	bool inverted;
} TellTale_Config_t;

void Pin_Write(const GPIO_Pin_t *gpio, GPIO_PinState state);

void Send_TellTales(const uint8_t *indicators);

void TellTales_Init();

#endif /* INC_TELL_TALES_H_ */
