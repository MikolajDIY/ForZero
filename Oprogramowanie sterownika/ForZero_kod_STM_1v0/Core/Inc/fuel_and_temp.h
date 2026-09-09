/*
 * fuel_and_temp.h
 *
 *  Created on: 14 sie 2026
 *      Author: rataj
 */

#ifndef INC_FUEL_AND_TEMP_H_
#define INC_FUEL_AND_TEMP_H_

#include "mcp4661.h"

void Send_Fuel_lvl(uint8_t fuel_lvl);
void Send_Engine_Temperature(uint8_t temp_lvl);

#endif /* INC_FUEL_AND_TEMP_H_ */
