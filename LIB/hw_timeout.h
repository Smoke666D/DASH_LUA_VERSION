/*
 * hw_timeout.h
 *
 *  Created on: Dec 24, 2024
 *      Author: i.dymov
 */

#ifndef HW_TIMEOUT_H_
#define HW_TIMEOUT_H_

#include "main.h"


#define TIMOUTS_COUNT  3

typedef struct
{
   u32 counter;
   u32 timeout;

} HW_TIMEOUT_TYPE;

void vHW_INIT_TIMEOUT( uint8_t timeouts_id, u32 timout);
uint8_t vHW_PROCESS_TIMEOUT( uint8_t timeouts_id, u32 tick);

#endif /* HW_TIMEOUT_H_ */
