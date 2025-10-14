#include "hw_lib_can.h"
#include "hal_flash.h"
#include "string.h"

static CANRX MailBoxBuffer[MAILBOXSIZE];
static uint16_t CANbitRate;
static TaskHandle_t  CanRXTaskHandle;
static MessageBufferHandle_t pCanRXMessageBuffer;
static MessageBufferHandle_t pCanTXMessageBuffer;
 SemaphoreHandle_t xSemaphore;

SemaphoreHandle_t * pGetCanMutex()
{
	return &xSemaphore ;
}

MessageBufferHandle_t * xGetCanTXMessageBufffer()
{
	return &pCanTXMessageBuffer;
}

MessageBufferHandle_t * xGetCanRXMessageBufffer()
{
	return &pCanRXMessageBuffer;
}

TaskHandle_t * xGetCanRXTaskHandle ()
{
    return  &CanRXTaskHandle ;
}

void CAN_SendMessage()
{
    CAN_TX_FRAME_TYPE buffer;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if (xMessageBufferReceiveFromISR(pCanTXMessageBuffer,&buffer,sizeof( CAN_TX_FRAME_TYPE),&xHigherPriorityTaskWoken) !=0 )
    {
        HAL_CANSend(&buffer);
    }
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

void vRestartNode( void )
{
    return;
}
/*
*/
void  prv_read_can_received_msg( HAL_CAN_RX_FIFO_NUMBER_t fifo) 
{
   CAN_FRAME_TYPE rxMsg;
   HAL_CAN_MSG_GET(fifo, &rxMsg);
   static portBASE_TYPE xHigherPriorityTaskWoken;
   xHigherPriorityTaskWoken = pdFALSE;
   xMessageBufferSendFromISR(pCanRXMessageBuffer,&rxMsg,sizeof(CAN_FRAME_TYPE), &xHigherPriorityTaskWoken );
   portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
   return;
}

uint8_t uFindMessageToMailbox( uint8_t * index_id, uint32_t can_id, uint8_t ext, uint8_t rtr)
{
	uint16_t first_index, last_index;
	if (ext)
	{
		first_index = NORMAL_CAN_ID_FILTER_COUNT ;
		last_index  = MAILBOXSIZE;
	}
	else 
	{
		first_index = 0;
        last_index  = NORMAL_CAN_ID_FILTER_COUNT ;
	}
	for (uint16_t k =  first_index ; k < last_index; k++)
	{
		if  ( (MailBoxBuffer[k].ident == can_id ) && (MailBoxBuffer[k].rtr == rtr) )
		{
			*index_id = k;
			return 1;
		}
	}
    return 0;
}

uint8_t GetMailBoxData( uint8_t mail_box_index,CAN_FRAME_TYPE * RXPacket )
{
	uint8_t res = 0U;
	if (mail_box_index < MAILBOXSIZE)
	{
		if ((MailBoxBuffer[mail_box_index].enable == 1) && (MailBoxBuffer[mail_box_index].new_data == 1))
		{
			  
			MailBoxBuffer[mail_box_index].new_data = 0;
			RXPacket->ident	 = MailBoxBuffer[mail_box_index].ident;
			 xSemaphoreTake( xSemaphore, portMAX_DELAY);
			RXPacket->DLC = MailBoxBuffer[mail_box_index].DLC;
			memcpy(RXPacket->data,MailBoxBuffer[mail_box_index].data,RXPacket->DLC);
			xSemaphoreGive( xSemaphore );
			res = 1U;
		}
	}
	return (res);
}


/*
*/
uint8_t vGetNodeId( void )
{
  return ( OB->Data1 & 0xFF );
}
/*

*/
void vSetNodeID( uint8_t data)
{
    ProgramOptionByteData(1,data) ;
}
/*
*
*/
void ConfigNodeID( uint8_t node_id)
{
	if (vGetNodeId()!=node_id)
	{
		vSetNodeID(node_id);
	}
}
/*
*
*/
void vSetBitrate( uint8_t data)
{
    uint8_t temp = OB->Data0 & 0x80;
    ProgramOptionByteData(0,temp | (data & 0x7F )) ;
}

/*
 * ����٧ӧ�ѧ�ѧ֧� �٧ߧѧ�֧ߧڧ� ��ܧ������ CAN �ڧ� EEPROM
 */
uint16_t vGetBitrate()
{
   return (OB->Data0 & 0x7F);
}

void vCANBoudInit( uint16_t boudrate )
{
    switch (boudrate )
    {
        case 1000:
            CANbitRate = CAN_1MBS;
            break;
        case 500:
            CANbitRate = CAN_500KBS;
            break;
        case 250:
            CANbitRate = CAN_250KBS;
            break;
        default:
            CANbitRate = CAN_125KBS;
            break;    
		case 800:
			CANbitRate = CAN_800KBS;
			break;
		case 100:
			CANbitRate = CAN_100KBS;
			break;
		case 50:
			CANbitRate = CAN_50KBS;
			break;
		case 20:
			CANbitRate = CAN_20KBS;
			break;
		case 10:
			CANbitRate = CAN_10KBS;
			break;
    }
	if (CANbitRate!=vGetBitrate() ) vSetBitrate(CANbitRate);
    HAL_CANSetTXCallback(&CAN_SendMessage);
    HAL_CANSetERRCallback(&vRestartNode);
    HAL_CANSetRXCallback(&prv_read_can_received_msg);
	xTaskNotify(CanRXTaskHandle,0xFFFFFFFF,eSetValueWithOverwrite);
    HAL_CANIntIT(CANbitRate,CAN1_PRIOR,CAN1_SUBPRIOR);
	for (int i=0;i<MAILBOXSIZE;i++)
	{
		MailBoxBuffer[i].enable = 0U;
 
		if ((i%4) == 0 ) HAL_CANInitIDInactive(i/4, FILTER_FIFO_0);
       // HAL_CANResetFiltesr(i);
	}	
    return;
}
/*
 *
 */
void vCanInsertRXData(CAN_FRAME_TYPE * RXPacket)
{
	uint16_t id = RXPacket->filter_id;
	if ((MailBoxBuffer[id].ident == RXPacket->ident) &&  (MailBoxBuffer[id].extd_id == RXPacket->extd) && (MailBoxBuffer[id].rtr == RXPacket->rtr)  )
	{
		xSemaphoreTake( xSemaphore, portMAX_DELAY);
		MailBoxBuffer[id].DLC = RXPacket->DLC;
		memcpy(MailBoxBuffer[id].data,RXPacket->data,RXPacket->DLC);
		if (MailBoxBuffer[id].enable == 1)
		{
			
			MailBoxBuffer[id].new_data = 1;
		}	
		xSemaphoreGive( xSemaphore );
	}
	else
	{
		uint8_t MailboxId;
        if ( uFindMessageToMailbox( &MailboxId,RXPacket->ident, RXPacket->extd, RXPacket->rtr) == 1 ) 
		{	
			xSemaphoreTake( xSemaphore, portMAX_DELAY);						 
			MailBoxBuffer[MailboxId].DLC = RXPacket->DLC;
			memcpy(MailBoxBuffer[MailboxId].data,RXPacket->data,RXPacket->DLC);
			if (MailBoxBuffer[MailboxId].enable == 1)
			{
				MailBoxBuffer[MailboxId].new_data = 1;
			}
			xSemaphoreGive( xSemaphore );
		}
	}
	
	return;
}
/*
 *
 */
void vCanRXTask(void *argument)
{
	CAN_FRAME_TYPE RXPacket;
	while(1)
	{  
		xMessageBufferReceive(pCanRXMessageBuffer,&RXPacket,sizeof(CAN_FRAME_TYPE),portMAX_DELAY);
		vCanInsertRXData(&RXPacket);
		
	}
}
/*
*
*/
static void vFilterSet(uint16_t mailboxindex)
{
	 uint16_t index = mailboxindex / 4;
     HAL_CANSetFiters(	index,
	 					MailBoxBuffer[index*4U  	].ident,
	 					MailBoxBuffer[index*4U  +1 	].ident,
						MailBoxBuffer[index*4U  +2	].ident,
	 					MailBoxBuffer[index*4U  +3	].ident   , 
						FILTER_FIFO_0); 
	 return;
}
/*
*/
void vFilterSetExtd(uint16_t mailboxindex)
{
	uint16_t index = HALF_CAN_FILTER_COUNT +  ( mailboxindex - NORMAL_CAN_ID_FILTER_COUNT )/ 2;  //���ߧէ֧ܧ� �ҧѧߧܧ� ��ڧݧ�����, �� �ܧ������ �ߧѧէ� ����ѧߧ�ӧڧ�� ��ڧݧ���
	uint16_t offset = (mailboxindex % 2) ? mailboxindex - 1 : mailboxindex;   //���ߧէ֧ܧ� ��֧�ӧ�ԧ� ��ݧ֧ާ֧ߧ�� �� �ާѧ��ڧӧ�  MailBoxBuffer, �ܧ������ �ߧѧէ� �٧ѧ�ڧ�ѧ�� �� ��ڧݧ���
	HAL_CANSetFitersEX(	index,
		 				MailBoxBuffer[offset     ].ident,
						MailBoxBuffer[offset  +1 ].ident,
						FILTER_FIFO_1 );
	return;

}

uint8_t  uCheckMailBoxData(uint8_t MailboxId) 
{
  return 	((MailBoxBuffer[MailboxId].enable == 1) &&	(MailBoxBuffer[MailboxId].new_data == 1)?1:0);
}
/*
*/
void eMailboxFilterReset(uint8_t MailboxId) 
{
     if (MailboxId < MAILBOXSIZE) 
	 {
		if	(MailBoxBuffer[MailboxId].enable == 1)
		{
			MailBoxBuffer[MailboxId].enable = 0;
			if (MailBoxBuffer[MailboxId].extd_id )
			{
				vFilterSetExtd(MailboxId);
			}
			else 
			{
				vFilterSet(MailboxId);
			}
		}
	}
	return;
}
/*
*/
ERROR_TYPE_t eMailboxFilterSet(uint32_t id, uint8_t extd, uint8_t rtr) 
{
	ERROR_TYPE_t eRes = BUFFER_FULL;
	uint8_t findMBIndex,first_index,last_index;
	
	first_index = (extd)? NORMAL_CAN_ID_FILTER_COUNT : 0;
	last_index  = (extd) ? MAILBOXSIZE : NORMAL_CAN_ID_FILTER_COUNT;

	for (findMBIndex = first_index ; findMBIndex <  last_index ; findMBIndex++)  //���� �ڧ�֧� ��ӧ�ҧ�էߧ�� �ҧ���֧� �� ���ѧ��֧� ���ݧ�ӧڧߧ� �ާѧ��ڧӧ�
	{
		if ( MailBoxBuffer[findMBIndex].enable == 0U )
		{
			eRes = ERROR_NO;
			break;
		}
	}
	if (eRes ==ERROR_NO )
	{
		MailBoxBuffer[findMBIndex].ident = id;
		MailBoxBuffer[findMBIndex].rtr = rtr;
		MailBoxBuffer[findMBIndex].enable = 1;
		if (extd ) 
		{
			vFilterSetExtd(findMBIndex);
		}
		else 
		{
			
			vFilterSet(findMBIndex);
		}
		printf("id  =%x index=%x\r\n",id,findMBIndex);
		
	}
	return ( findMBIndex );
}

/*
������ѧӧܧ� ��ѧܧ֧�� �� ��֧�� CAN. 
����ݧ� ��֧ܧ��ڧ� �ާѧۧݧҧ�ܧ�� ���ߧ���ݧݧ֧�� CAN �٧ѧߧ���, ��� ��ѧܧ֧� ��ѧ٧ާ֧�ѧ֧��� �� ���֧�֧է� ���֧�ѧ�ڧ�ߧߧ�� ��ڧ��֧ާ�
*/
void APPCANSEND(CAN_TX_FRAME_TYPE *buffer)
{
    if ( HAL_CANSend(buffer) == CAN_TxStatus_NoMailBox )
    {
		xMessageBufferSend(pCanTXMessageBuffer,&buffer,sizeof(CAN_TX_FRAME_TYPE), portMAX_DELAY);
    }
}


