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
#include "user_data.h"

static TaskHandle_t DefautTask_Handler;
static StaticTask_t xIdleTaskTCB;
static StaticTask_t xTimerTaskTCB;
static StaticTask_t xLuaTaskTCB;
static StaticTask_t xIOTaskTCB;
static StaticTask_t xCANRXTaskTCB;
static StaticTask_t defaultTaskControlBlock;
static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];
static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];
static StackType_t defaultTaskBuffer[DEFAULT_TASK_STACK_SIZE];
static StackType_t CANRXTaskBuffer[ CANRX_STK_SIZE ];
static StackType_t LuaTaskBuffer[ LUA_STK_SIZE ];
static StackType_t IOTaskBuffer[ IO_STK_SIZE ];
static StaticMessageBuffer_t xMessageBufferStruct;
static StaticMessageBuffer_t xTXCanMessageBufferStruct;
static uint8_t ucMessageBufferStorage[ CANRX_QUEUE_SIZE * sizeof( CAN_FRAME_TYPE ) ];
static uint8_t ucTXCanMessageBufferStorage[ CANRX_QUEUE_SIZE * sizeof( CAN_TX_FRAME_TYPE) ];
//static uint8_t ucQueueStorageArea[  16U * sizeof( CAN_FRAME_TYPE ) ];
 StaticSemaphore_t xMutexBuffer;
 StaticSemaphore_t xDinMutexBuffer;

static void StartDefaultTask(void *argument);

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



void vSystemStop()
{
    vTaskSuspend(* xGetCanRXTaskHandle ());
    //vTaskSuspend( * xGetIOTaskHandle ());
    vTaskSuspend( * xGetLuaTaskHandle());
}

INIT_FUNC_LOC  void vSYStaskInit ( void )
{
 (* xGetCanRXTaskHandle ())= xTaskCreateStatic( vCanRXTask, "CanrxTask", CANRX_STK_SIZE , ( void * ) 1, CANRX_TASK_PRIO  ,
      (StackType_t * const )CANRXTaskBuffer, &xCANRXTaskTCB );
 (* xGetIOTaskHandle ())= xTaskCreateStatic( vIOTask, "IOTask", IO_STK_SIZE , ( void * ) 1, IO_TASK_PRIO  ,
    (StackType_t * const )IOTaskBuffer, &xIOTaskTCB );
 (* xGetLuaTaskHandle()) = xTaskCreateStatic( vLuaTask, "LuaTask", LUA_STK_SIZE , ( void * ) 1, LUA_TASK_PRIO  ,
      (StackType_t * const )LuaTaskBuffer, &xLuaTaskTCB );
   DefautTask_Handler = xTaskCreateStatic( StartDefaultTask, "DefTask", DEFAULT_TASK_STACK_SIZE , ( void * ) 1, DEFAULT_TASK_PRIOR, (StackType_t * const )defaultTaskBuffer, &defaultTaskControlBlock );
  vSystemStop();
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
   (* pGetCanMutex()) = xSemaphoreCreateMutexStatic( &xMutexBuffer );

   ( * pGetDinSemaphoreHandle()) = xSemaphoreCreateMutexStatic( &xDinMutexBuffer );
  
}
/*----------------------------------------------------------------------------*/
void vSYSeventInit ( void )
{
 // *(osLUAetPDMstatusHandle () ) = xEventGroupCreateStatic(&xLuaStateEventGroup );
}


static void StartDefaultTask(void *argument)
{
   uint8_t counter = 0;
   DataModel_Init();
   /*uint8_t test_mode =((getReg8(BITRATE_ADR ) & 0x80) == 0 )?0:1;
   vProceesInit();
   vTaskResume( *xCanOpenProcessTaskHandle());
   vTaskResume( *xCanOpenPeriodicTaskHandle ());
   vTaskResume(* xProcessTaskHandle ());
   vTaskDelay(100);
   vTaskResume(* getInputsTaskHandle());*/
   for(;;)
   {
      vTaskDelay(500);
    /*  HAL_WDTReset();
      if (test_mode )
      {
          if (counter++ ==2)
          {
             printf("\r\n Sec =%d \r\n", DataModelGetSecCounter());
             for (uint8_t i=AIN1;i<=AIN3;i++)
             {
                  printf("ain%d R  %d \r\n",i,(uint16_t)GetAIN(i));
             }
             for (uint8_t i=AIN4;i<=AIN5;i++)
             {
                 printf("ain%d B*100 %d \r\n",i,(uint16_t)(GetAIN(i)*100));
             }
             for (uint8_t i = 0 ; i< 4;i++)
             printf("DIN%d =%d \r\n", i,InputsGetTest( i));
             counter = 0;
          }
      }*/
   }
  /* USER CODE END 5 */
}