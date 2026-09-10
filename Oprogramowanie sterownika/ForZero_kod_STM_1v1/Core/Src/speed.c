/*
 * speed.c
 *
 *  Created on: 14 sie 2026
 *      Author: rataj
 */

#include "speed.h"
#include "stdbool.h"

uint32_t last_tick = 0;

void Speed_Init(){
	__HAL_TIM_ENABLE_OCxPRELOAD(&htim3, TIM_CHANNEL_2);
	htim3.Instance->CR1 |= TIM_CR1_ARPE; // Włącza buforowanie ARR

  HAL_Delay(500);
  Send_Speed(2);
  Send_Speed(260);
  Send_Speed(2);
}

static bool is_speed_pwm_running = false;

void Send_Speed(float speed)
{
    if (speed <= 1.0f) {
        if (is_speed_pwm_running) {
            HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
            is_speed_pwm_running = false;
        }
        return;
    }

    float frequency_hz = 1.115f * speed;
    uint32_t arr_value = (uint32_t)(100000.0f / frequency_hz) - 1;

    if (arr_value > 65535) {
        arr_value = 65535;
    }

    __HAL_TIM_SET_AUTORELOAD(&htim3, arr_value);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, (arr_value + 1) / 2);

    if (!is_speed_pwm_running) {
        HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
        is_speed_pwm_running = true;
    }
}



