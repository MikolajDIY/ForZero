/*
 * mcp4661.c
 *
 *  Created on: 9 sie 2026
 *      Author: rataj
 */

#include "mcp4661.h"

extern I2C_HandleTypeDef hi2c1;


HAL_StatusTypeDef MCP4661_SetWiper(I2C_HandleTypeDef *hi2c, uint8_t wiper_addr, uint8_t value) {

    // 1. Jeśli magistrala jest w stanie błędu lub zablokowana, zresetuj peryferium I2C
    if (HAL_I2C_GetState(hi2c) != HAL_I2C_STATE_READY) {

        // Czekaj max 5ms na zwolnienie
        uint32_t timeout = HAL_GetTick();
        while (HAL_I2C_GetState(hi2c) != HAL_I2C_STATE_READY) {
            if (HAL_GetTick() - timeout > 5) {
                // Odblokowanie zaciętego sprzętu I2C w STM32
                HAL_I2C_DeInit(hi2c);
                HAL_I2C_Init(hi2c);
                return HAL_TIMEOUT;
            }
        }
    }

    // 2. Adresowanie MCP4661
    uint8_t buffer[2];
    buffer[0] = (wiper_addr & 0x0F) << 4;
    buffer[1] = value;

    // 3. Transmisja z krótkim timeoutem
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(hi2c, MCP4661_I2C_ADDR, buffer, 2, 10);

    // Jeśli transmisja się nie powiodła (NACK / Timeout), zresetuj I2C na przyszłość
    if (status != HAL_OK) {
        HAL_I2C_DeInit(hi2c);
        HAL_I2C_Init(hi2c);
    }

    return status;
}
