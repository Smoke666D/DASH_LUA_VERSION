/*
 * lua_task.h
 *
 *  Created on: Feb 14, 2025
 *      Author: i.dymov
 */

#ifndef USER_LUA_TASK_H_
#define USER_LUA_TASK_H_

#include "main.h"
#include "system_init.h"

#define NO_RESULT  0U
#define ONE_RESULT 1U
#define TWO_RESULT 2U
#define SEVEN_RESULT   7U
#define SUCSESS    1U
#define ERROR	   0U

#define MAX_SCRIPT_SIZE  20000

#define CAN_FRAME_SIZE  				8U
#define CAN_ID_POS					    1U
#define FRAME_SIZE_POS					2U
#define CAN_TABLE_POS					3U

#define CANREQSEND_ARGUMENT_COUNT 3
#define SEND_REQUEST_ARGUMENT_COUNT 3
#define CAN_SEND_TABLE_ARGUMENT_COUNT 	3U

#define FIRST_ARGUMENT   1U
#define SECOND_ARGUMENT  2U
#define THIRD_ARGUMENT   3U
#define FOURTH_ARGUMENT  4U
#define FIVE_ARGUMENT	 5U
#define SIX_ARGUMENT	 6U
#define SEVEN_ARGUMENT   7U
#define LAST_ARGUMENT	-1

#define ONE_ARGUMENT	1U
#define TWO_ARGUMENTS	2U
#define THREE_ARGUMENTS  3U
#define FOUR_ARGUMENTS  4U

typedef enum
{
	RESULT_TRUE = 1,
	RESULT_FALSE =0
} RESULT_t;

typedef enum {
    LUA_INIT,
    LUA_RUN,
    LUA_ERROR,
    LUA_STOP,
    LUA_RESTART,
    LUA_STATE_SIZE,
    LUA_WAIT_READY
} LUA_STATE_t;




TaskHandle_t * xGetLuaTaskHandle ();
void vLuaTask( void * argument );

#endif /* USER_LUA_TASK_H_ */
