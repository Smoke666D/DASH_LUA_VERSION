/*
 * hal_wdt.h
 *
 *  Created on: 26 апр. 2024 г.
 *      Author: i.dymov
 */

#ifndef HAL_HAL_WDT_H_
#define HAL_HAL_WDT_H_

#include "system_config.h"

//indeprendet wachdog clock soure
#define WDT_CLOCK  40000
#define CTLR_KEY_Reload    ((uint16_t)0xAAAA)
#define CTLR_KEY_Enable    ((uint16_t)0xCCCC)
#define CTLR_KEY_Modify    ((uint16_t)0x5555)

void HAL_WDTInit();
void HAL_WDTReset();
void HAL_WDT_Init1s();

#endif /* HAL_HAL_WDT_H_ */
