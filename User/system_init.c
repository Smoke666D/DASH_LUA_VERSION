/*
 * system_init.c
 *
 *  Created on: Dec 11, 2024
 *      Author: i.dymov
 */
#include "system_init.h"
#include "hw_lib_can.h"
#include "hal_can.h"
#include "io_task.h"
#include "hw_lib_keyboard.h"

static StaticTask_t xIdleTaskTCB;
static StaticTask_t xTimerTaskTCB;
static StaticTask_t xLuaTaskTCB;
static StaticTask_t xIOTaskTCB;
static StaticTask_t xCANRXTaskTCB;
static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];
static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];
static StackType_t CANRXTaskBuffer[ CANRX_STK_SIZE ];
static StackType_t LuaTaskBuffer[ LUA_STK_SIZE ];
static StackType_t IOTaskBuffer[ IO_STK_SIZE ];
static StaticMessageBuffer_t xMessageBufferStruct;
static StaticMessageBuffer_t xKeyboardMessageBufferStruct;
static StaticMessageBuffer_t xTXCanMessageBufferStruct;
static uint8_t ucMessageBufferStorage[ CANRX_QUEUE_SIZE * sizeof( CAN_FRAME_TYPE ) ];
static uint8_t ucTXCanMessageBufferStorage[ CANRX_QUEUE_SIZE * sizeof( CAN_TX_FRAME_TYPE) ];
static uint8_t ucKeyboardMessageBufferStorage[ 16 * sizeof( KeyEvent ) ];
//static uint8_t ucQueueStorageArea[  16U * sizeof( CAN_FRAME_TYPE ) ];
 StaticSemaphore_t xMutexBuffer;



/*
 * 妤快把快技快扶扶抑快
 */

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize )
{
   *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
   *ppxIdleTaskStackBuffer = uxIdleTaskStack;
   *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}


void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize )
{
   *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
   *ppxTimerTaskStackBuffer = uxTimerTaskStack;
   *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}


INIT_FUNC_LOC  void vSYStaskInit ( void )
{
 (* xGetCanRXTaskHandle ())= xTaskCreateStatic( vCanRXTask, "CanrxTask", CANRX_STK_SIZE , ( void * ) 1, CANRX_TASK_PRIO  ,
      (StackType_t * const )CANRXTaskBuffer, &xCANRXTaskTCB );
 (* xGetIOTaskHandle ())= xTaskCreateStatic( vIOTask, "IOTask", IO_STK_SIZE , ( void * ) 1, IO_TASK_PRIO  ,
    (StackType_t * const )IOTaskBuffer, &xIOTaskTCB );
 (* xGetLuaTaskHandle()) = xTaskCreateStatic( vLuaTask, "LuaTask", LUA_STK_SIZE , ( void * ) 1, LUA_TASK_PRIO  ,
      (StackType_t * const )LuaTaskBuffer, &xLuaTaskTCB );
  return;
}

INIT_FUNC_LOC void vSYSqueueInit ( void )
{
   *(xGetCanRXMessageBufffer()) = xMessageBufferCreateStatic( sizeof( ucMessageBufferStorage ),
                                                 ucMessageBufferStorage,
                                                 &xMessageBufferStruct );

 * (xGetCanTXMessageBufffer())= xMessageBufferCreateStatic( sizeof( ucTXCanMessageBufferStorage ),
                                                 ucTXCanMessageBufferStorage,
                                                 &xTXCanMessageBufferStruct );

 * (xKeyboardMessageBuffer()) =  xMessageBufferCreateStatic( sizeof( ucKeyboardMessageBufferStorage ),
                                                 ucKeyboardMessageBufferStorage,
                                                 &xKeyboardMessageBufferStruct );
   (* pGetCanMutex()) = xSemaphoreCreateMutexStatic( &xMutexBuffer );
  
}
/*----------------------------------------------------------------------------*/
void vSYSeventInit ( void )
{
 // *(osLUAetPDMstatusHandle () ) = xEventGroupCreateStatic(&xLuaStateEventGroup );
}
