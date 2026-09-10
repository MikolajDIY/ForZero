/*
 * mcp4661.h
 *
 *  Created on: 9 sie 2026
 *      Author: rataj
 */

#ifndef INC_MCP4661_H_
#define INC_MCP4661_H_

#include "stm32f1xx_hal.h"

#define MCP4661_I2C_ADDR (0x28 << 1)
#define MCP4661_VOLATILE_WIPER_0 0x00
#define MCP4661_VOLATILE_WIPER_1 0x01
#define MCP4661_NV_WIPER_0       0x02
#define MCP4661_NV_WIPER_1       0x03

HAL_StatusTypeDef MCP4661_SetWiper(I2C_HandleTypeDef *hi2c, uint8_t wiper_addr, uint8_t value);

#endif /* INC_MCP4661_H_ */
