/*
 * tell_tales.c
 *
 *  Created on: 10 sie 2026
 *      Author: rataj
 */
#include "tell_tales.h"

// LEDs on driver
const GPIO_Pin_t LED_OK = { .port = GPIOC, .pin = GPIO_PIN_6};
const GPIO_Pin_t LED_WARNING = { .port = GPIOC, .pin = GPIO_PIN_7};

// Tell-Tales
const GPIO_Pin_t TCR = { .port = GPIOC, .pin = GPIO_PIN_4};
const GPIO_Pin_t BRAKE_PADS = { .port = GPIOC, .pin = GPIO_PIN_5};
const GPIO_Pin_t ABS = { .port = GPIOB, .pin = GPIO_PIN_0};
const GPIO_Pin_t SENSOR = { .port = GPIOB, .pin = GPIO_PIN_10};
const GPIO_Pin_t COOL_LVL = { .port = GPIOB, .pin = GPIO_PIN_12};
const GPIO_Pin_t AIRBAG = { .port = GPIOB, .pin = GPIO_PIN_13};
const GPIO_Pin_t WIPER_FLUID = { .port = GPIOB, .pin = GPIO_PIN_14};
const GPIO_Pin_t BRAKES = { .port = GPIOB, .pin = GPIO_PIN_15};

// Speed
const GPIO_Pin_t SPEED = { .port = GPIOA, .pin = GPIO_PIN_7};

static const TellTale_Config_t TELL_TALES[8] ={
	{&ABS,			ABS_BIT, 				true},
	{&TCR, 			TCR_BIT, 				false},
	{&BRAKE_PADS, 	BRAKE_PADS_BIT, 		true},
	{&BRAKES, 		BRAKES_BIT, 			false},
	{&SENSOR,		SENSOR_BIT,				true},
	{&COOL_LVL,		COOL_LVL_BIT,			true},
	{&AIRBAG,		AIRBAG_BIT,				true},
	{&WIPER_FLUID,	WIPER_FLUID_BIT, 		true}
};

// Function used to set indicators state
void Pin_Write(const GPIO_Pin_t *gpio, GPIO_PinState state){
	HAL_GPIO_WritePin(gpio->port, gpio->pin, state);
}

void Send_TellTales(const uint8_t *indicators){
	uint8_t indi = *indicators;

	for(uint8_t i=0; i<8; i++){
		bool is_set = (indi & TELL_TALES[i].mask) != 0;

		if(TELL_TALES[i].inverted){
			is_set = !is_set;
		}

		GPIO_PinState state;
		state = is_set ? GPIO_PIN_SET : GPIO_PIN_RESET;
		Pin_Write(TELL_TALES[i].pin, state);
	}
}

void TellTales_Init(){
	  Pin_Write(&BRAKES, GPIO_PIN_RESET); // RESET - OFF
	  Pin_Write(&AIRBAG, GPIO_PIN_SET); // SET - OFF
	  Pin_Write(&ABS, GPIO_PIN_SET); // SET - OFF
	  Pin_Write(&TCR, GPIO_PIN_RESET); // RESET - OFF
	  Pin_Write(&COOL_LVL, GPIO_PIN_RESET); // SET - OFF
	  Pin_Write(&WIPER_FLUID, GPIO_PIN_SET); // SET - OFF
	  Pin_Write(&BRAKE_PADS, GPIO_PIN_SET); //SET - OFF
	  Pin_Write(&SENSOR, GPIO_PIN_RESET); // SET - OFF
}
