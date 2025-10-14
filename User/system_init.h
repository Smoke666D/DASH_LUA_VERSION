/*
 * system_init.h
 *
 *  Created on: Dec 11, 2024
 *      Author: i.dymov
 */

#ifndef USER_SYSTEM_INIT_H_
#define USER_SYSTEM_INIT_H_


#include "main.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "list.h"
#include "task.h"
#include "semphr.h"
#include "message_buffer.h"
#include "portmacro.h"
#include "stdbool.h"
#include "lua_task.h"

#define APP_TASK_PRIO               2  //ДОЛЖЕН БЫТЬ САМЫ ВЫСОКИЙ
#define APP_STK_SIZE                128
#define LUA_TASK_PRIO               3  //ДОЛЖЕН БЫТЬ САМЫ ВЫСОКИЙ
#define LUA_STK_SIZE                700
#define CANRX_TASK_PRIO             9 //ДОЛЖЕН БЫТЬ САМЫ ВЫСОКИЙ
#define CANRX_STK_SIZE             256
#define IO_TASK_PRIO                5 //ДОЛЖЕН БЫТЬ САМЫ ВЫСОКИЙ
#define IO_STK_SIZE                 128

#define  CANRX_QUEUE_SIZE 				16U
#define  CANTX_QUEUE_SIZE 				16U

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize );

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize );

void vSYStaskInit ( void );
void vSYSqueueInit ( void );
void vSYSeventInit ( void );

#endif /* USER_SYSTEM_INIT_H_ */
