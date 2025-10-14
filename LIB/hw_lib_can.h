#ifndef HW_LIB_CAN_H_
#define HW_LIB_CAN_H_


#include "hal_can.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "list.h"
#include "task.h"
#include "semphr.h"
#include "message_buffer.h"
#include "portmacro.h"

#define CAN_EXT_FLAG   0x80000000
#define CAN_RTR_FLAG   0x40000000

#define CAN_FILTER_COUNT              16
#define HALF_CAN_FILTER_COUNT		  (CAN_FILTER_COUNT/2)
#define NORMAL_CAN_ID_FILTER_COUNT   ((HALF_CAN_FILTER_COUNT)*4)
#define EXTEND_CAN_ID_FILTER_COUNT   ((HALF_CAN_FILTER_COUNT)*2)
#define MAILBOXSIZE  (NORMAL_CAN_ID_FILTER_COUNT  + EXTEND_CAN_ID_FILTER_COUNT )


typedef struct {
    uint32_t ident;
    uint8_t filter_id;
    uint8_t DLC;
    uint8_t data[8];
    uint8_t new_data:1;
    uint8_t extd_id:1;
    uint8_t rtr:1;
    uint8_t enable:1;
} CANRX;

typedef struct {
  uint8_t enable:1;
  uint8_t index;
} CanFilterHeandler;

#define  ANSWER_FILTER 0x01
#define  INPUT_FILTER  0x00


typedef enum
{
  ERROR_NO  = 0,
  BUFFER_FULL = 1
} ERROR_TYPE_t;

uint8_t GetMailBoxData( uint8_t mail_box_index,CAN_FRAME_TYPE * RXPacket );
uint8_t  uCheckMailBoxData(uint8_t MailboxId) ;
void vResetFilter( int id);
void ConfigNodeID( uint8_t node_id);
uint8_t vCanGetAnsewerMessage(CAN_FRAME_TYPE * RXPacket);
void eMailboxFilterReset(uint8_t MailboxId) ;
ERROR_TYPE_t eMailboxFilterSet(uint32_t id, uint8_t extd, uint8_t rtr);
void vCANBoudInit( uint16_t boudrate );
void vCanRXTask(void *argument);
void APPCANSEND(CAN_TX_FRAME_TYPE *buffer);
uint8_t vCanChekMessage(uint32_t id);
TaskHandle_t * xGetCanRXTaskHandle ();
MessageBufferHandle_t * xGetCanRXMessageBufffer();
MessageBufferHandle_t * xGetCanTXMessageBufffer();
uint8_t uFindMessageToMailbox( uint8_t * index_id, uint32_t can_id, uint8_t ext, uint8_t rtr);
SemaphoreHandle_t * pGetCanMutex();
#endif