/*
 * hw_timout.c
 *
 *  Created on: Dec 24, 2024
 *      Author: i.dymov
 */


#include "hw_timeout.h"


HW_TIMEOUT_TYPE timeouts[ TIMOUTS_COUNT];


void vHW_INIT_TIMEOUT( uint8_t timeouts_id, u32 timout)
{
    if (timeouts_id < TIMOUTS_COUNT)
    {
        timeouts[timeouts_id].counter = 0;
        timeouts[timeouts_id].timeout  = timout;
    }

}

uint8_t vHW_PROCESS_TIMEOUT( uint8_t timeouts_id, u32 tick)
{
    timeouts[timeouts_id].counter = timeouts[timeouts_id].counter + tick;
    if (timeouts[timeouts_id].counter > timeouts[timeouts_id].timeout )
    {
        timeouts[timeouts_id].counter = 0;
        return 1;
    }
    else
    {
      return 0;
    }

}
