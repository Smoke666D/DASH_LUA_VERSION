/*
 * hal_flash.h
 *
 *  Created on: 15 мая 2024 г.
 *      Author: i.dymov
 */

#ifndef HAL_HAL_FLASH_H_
#define HAL_HAL_FLASH_H_

#include "system_config.h"



typedef enum
{
  FLASH_OK,
  FLASH_ERROR_ADR,
  FLASH_ERROR_LENGTH,
  FLASH_ERROR_UNLOCK,
  FLASH_ERROR_LOCK,
  FLASH_ERROR_ACCESS,
  FLASH_ERROR_WRITING,
  FLASH_ERROR_WRITING_TIMEOUT,
  FLASH_ERROR_VERIFICATION
} FLASH_STATE;


void HAL_FLASH_Lock();
void HAL_FLASH_Unlock();
FLASH_Status ProgramOptionByteData(uint8_t Address, uint8_t Data);
FLASH_STATE HAL_FLASH_WriteByWord( uint8_t * src, uint8_t * dest, uint32_t len);
FLASH_STATE HAL_FLASH_ErasePage(uint32_t Page_Address);



#endif /* HAL_HAL_FLASH_H_ */
