/*
 * hal_can.c
 *
 *  Created on: 13 мая 2024 г.
 *      Author: i.dymov
 */
#include "hal_can.h"
#include "string.h"




static HAL_CAN_t CAN;


/* Time out for INAK bit */
#define INAK_TIMEOUT                 ((uint32_t)0x0000FFFF)*2
#define CAN_MODE_MASK                ((uint32_t) 0x00000003)
/* CAN Mailbox Transmit Request */
#define TMIDxR_TXRQ                  ((uint32_t)0x00000001)
/* CAN FCTLR Register bits */
#define FCTLR_FINIT                  ((uint32_t)0x00000001)
void   USB_HP_CAN1_TX_IRQHandler(void)
 #ifdef CAN_FAST
  __attribute__((interrupt("WCH-Interrupt-fast")));
  #else 
 __attribute__((interrupt())); 
 #endif/* USB HP and CAN1 TX */
void   USB_LP_CAN1_RX0_IRQHandler(void) 
 #ifdef CAN_FAST
  __attribute__((interrupt("WCH-Interrupt-fast")));
  #else 
 __attribute__((interrupt())); 
 #endif
void   CAN1_RX1_IRQHandler(void)     
  #ifdef CAN_FAST
  __attribute__((interrupt("WCH-Interrupt-fast")));
  #else 
 __attribute__((interrupt())); 
 #endif
void   CAN1_SCE_IRQHandler(void)      
#ifdef CAN_FAST
  __attribute__((interrupt("WCH-Interrupt-fast")));
  #else 
 __attribute__((interrupt())); 
 #endif

void HAL_CANSetTXCallback(void (* f) ( void ))
{
	CAN.txcallback = f;
}
void HAL_CANSetRXCallback(void (* f) ( HAL_CAN_RX_FIFO_NUMBER_t))
{

	CAN.rxcallback = f;
}

void HAL_CANSetERRCallback(void (* f) ( void ))
{
	CAN.errorcallback = f;
}



INIT_FUNC_LOC void HAL_CANInt(  uint8_t   CANbitRate)
{
     u16 CAN_Prescaler;
     /* Configure CAN timing */
      switch (CANbitRate)
      {
          case 0: CAN_Prescaler  = 2;
                  break;
          case 1:  CAN_Prescaler = 4;
                  break;
          case 2:  CAN_Prescaler = 8;
                  break;
         default:
         case 3:  CAN_Prescaler = 16;
                 break;
         case 4:  CAN_Prescaler = 20;
                break;
         case 5:  CAN_Prescaler = 120;
                break;
         case 6:  CAN_Prescaler = 300;
               break;
         case 7:  CAN_Prescaler = 600;
              break;
      }

      uint32_t wait_ack = 0x00000000;
      CAN1->CTLR &= (~(uint32_t)CAN_CTLR_SLEEP);
      CAN1->CTLR |= CAN_CTLR_INRQ ;
      while (((CAN1->STATR & CAN_STATR_INAK) != CAN_STATR_INAK) && (wait_ack != INAK_TIMEOUT))
      {
         wait_ack++;
      }
      if ((CAN1->STATR & CAN_STATR_INAK) == CAN_STATR_INAK)
      {
          CAN1->CTLR &= ~(uint32_t)CAN_CTLR_TTCM;
          CAN1->CTLR |= CAN_CTLR_ABOM;
          CAN1->CTLR &= ~(uint32_t)CAN_CTLR_AWUM;
          CAN1->CTLR |= CAN_CTLR_NART;
          CAN1->CTLR &= ~(uint32_t)CAN_CTLR_RFLM;
          CAN1->CTLR &= ~(uint32_t)CAN_CTLR_TXFP;

          CAN1->BTIMR = (uint32_t)((uint32_t)CAN_Mode_Normal << 30) | \
                                      ((uint32_t)CAN_SJW_4tq << 24) | \
                                      ((uint32_t)CAN_BS1_12tq << 16) | \
                                      ((uint32_t)CAN_BS2_5tq << 20) | \
                                      ((uint32_t)CAN_Prescaler - 1);
          CAN1->CTLR &= ~(uint32_t)CAN_CTLR_INRQ;
          wait_ack = 0;
          while (((CAN1->STATR & CAN_STATR_INAK) == CAN_STATR_INAK) && (wait_ack != INAK_TIMEOUT))
          {
                  wait_ack++;
          }
      }
}






INIT_FUNC_LOC void HAL_CANIntIT( CAN_BOUNDRATE  CANbitRate, uint8_t prior, uint8_t subprior)
{
     u16 CAN_Prescaler = 8;
      uint8_t bs1 = CAN_BS1_12tq;
     uint8_t bs2 = CAN_BS2_5tq;
     HAL_InitAPB1(RCC_APB1Periph_CAN1);
     uint8_t mul = SystemCoreClock == (144000000)? 2 : 1;
     /* Configure CAN timing */
       switch (CANbitRate)
      {
          case CAN_1MBS:    CAN_Prescaler  = 2;
                            break;
         /* case CAN_800KBS:
                            CAN_Prescaler  = 3;
                            bs1 = CAN_BS1_10tq;
                            bs2 = CAN_BS2_4tq;
                            break;*/
          case CAN_500KBS:  CAN_Prescaler = 4;
                  break;
          case CAN_250KBS:
          default:
                  break;
          case CAN_125KBS:  CAN_Prescaler = 16;
                 break;
         case CAN_100KBS:  CAN_Prescaler = 20;
                break;
         case CAN_50KBS :  CAN_Prescaler = 40;
                break;
         case CAN_20KBS:  CAN_Prescaler = 100;
               break;
          case CAN_10KBS:  CAN_Prescaler = 200;
              break;
      }
      uint32_t wait_ack = 0x00000000;

      CAN1->CTLR &= (~(uint32_t)CAN_CTLR_SLEEP);
      CAN1->CTLR |= CAN_CTLR_INRQ ;
      while (((CAN1->STATR & CAN_STATR_INAK) != CAN_STATR_INAK) && (wait_ack != INAK_TIMEOUT))
      {
         wait_ack++;
      }
      if ((CAN1->STATR & CAN_STATR_INAK) == CAN_STATR_INAK)
      {
          CAN1->CTLR &= ~(uint32_t)CAN_CTLR_TTCM;
          CAN1->CTLR |= CAN_CTLR_ABOM;
          CAN1->CTLR &= ~(uint32_t)CAN_CTLR_AWUM;
          CAN1->CTLR |= CAN_CTLR_NART;
          CAN1->CTLR &= ~(uint32_t)CAN_CTLR_RFLM;
          CAN1->CTLR &= ~(uint32_t)CAN_CTLR_TXFP;

   
          CAN1->BTIMR = (uint32_t)((uint32_t)CAN_Mode_Normal << 30) | \
                                      ((uint32_t)CAN_SJW_4tq << 24) | \
                                      ((uint32_t)bs1 << 16) | \
                                      ((uint32_t)bs2<< 20) | \
                                      ((uint32_t)(CAN_Prescaler*mul - 1));
          CAN1->CTLR &= ~(uint32_t)CAN_CTLR_INRQ;
          wait_ack = 0;
          while (((CAN1->STATR & CAN_STATR_INAK) == CAN_STATR_INAK) && (wait_ack != INAK_TIMEOUT*mul))
          {
                  wait_ack++;
          }
         if ((CAN1->STATR & CAN_STATR_INAK) != CAN_STATR_INAK)
         {

             CAN1->INTENR |= CAN_IT_TME | CAN_IT_BOF | CAN_IT_FMP0 | CAN_IT_FMP1;
#if CORE == WCH32V2
             PFIC_IRQ_ENABLE_PG1(USB_LP_CAN1_RX0_IRQn,prior,subprior);
             PFIC_IRQ_ENABLE_PG1(CAN1_RX1_IRQn,prior,subprior);
             PFIC_IRQ_ENABLE_PG1(USB_HP_CAN1_TX_IRQn,prior,subprior);
             PFIC_IRQ_ENABLE_PG1(CAN1_SCE_IRQn,prior,subprior);
#else
             PFIC_IRQ_ENABLE_PG2(USB_LP_CAN1_RX0_IRQn,prior,subprior);
             PFIC_IRQ_ENABLE_PG2(CAN1_RX1_IRQn,prior,subprior);
             PFIC_IRQ_ENABLE_PG2(USB_HP_CAN1_TX_IRQn,prior,subprior);
             PFIC_IRQ_ENABLE_PG2(CAN1_SCE_IRQn,prior,subprior);
#endif
         }
      }

}

INIT_FUNC_LOC uint8_t HAL_CANToInitMode()
{
	uint32_t timeout = INAK_TIMEOUT;
	CAN1->CTLR = (uint32_t)((CAN1->CTLR & (uint32_t)(~(uint32_t)CAN_CTLR_SLEEP)) | CAN_CTLR_INRQ);
	while (((CAN1->STATR & CAN_MODE_MASK) != CAN_STATR_INAK) && (timeout != 0))
	{
	      timeout--;
	}
	return ( ((CAN1->STATR & CAN_MODE_MASK) != CAN_STATR_INAK)?  CAN_ModeStatus_Failed : CAN_ModeStatus_Success);

}
INIT_FUNC_LOC uint8_t HAL_CANToOperatingMode()
{
	uint32_t timeout = INAK_TIMEOUT;
	CAN1->CTLR &= (uint32_t)(~(CAN_CTLR_SLEEP|CAN_CTLR_INRQ));
	while (((CAN1->STATR & CAN_MODE_MASK) != 0) && (timeout!=0))
	{
	    timeout--;
	}
	return ( ((CAN1->STATR & CAN_MODE_MASK) != 0)?  CAN_ModeStatus_Failed : CAN_ModeStatus_Success);
}


/*
Функция отправки пакета.
*/
uint8_t HAL_CANSend(CAN_TX_FRAME_TYPE *buffer)
{
   u8 RTR;
   u32 StdId;
   u8 Data[8];
   u8 DLC                = (uint32_t)buffer->DLC;
   memcpy(Data,buffer->data,buffer->DLC);
   uint8_t transmit_mailbox;
   if ((CAN1->TSTATR & CAN_TSTATR_TME0) == CAN_TSTATR_TME0)
   {
       transmit_mailbox = 0;
   }
   else if ((CAN1->TSTATR & CAN_TSTATR_TME1) == CAN_TSTATR_TME1)
   {
       transmit_mailbox = 1;
   }
   else if ((CAN1->TSTATR & CAN_TSTATR_TME2) == CAN_TSTATR_TME2)
   {
       transmit_mailbox = 2;
   }
   else
   {
      return ( CAN_TxStatus_NoMailBox);
   }
   CAN1->sTxMailBox[transmit_mailbox].TXMIR &= TMIDxR_TXRQ;
   RTR   =  buffer->rtr;
   StdId =  buffer->ident;
   if ( buffer->extd )
   {
       CAN1->sTxMailBox[transmit_mailbox].TXMIR |= ((StdId << 3) | (uint32_t)0x00000004 |  RTR);
    }  
    else
    {
        CAN1->sTxMailBox[transmit_mailbox].TXMIR |= ((StdId << 21) | RTR);
    }
   DLC &= (uint8_t)0x0000000F;
   CAN1->sTxMailBox[transmit_mailbox].TXMDTR &= (uint32_t)0xFFFFFFF0;
   CAN1->sTxMailBox[transmit_mailbox].TXMDTR |= DLC;

   CAN1->sTxMailBox[transmit_mailbox].TXMDLR =          (((uint32_t)Data[3] << 24)|
                                                        ((uint32_t)Data[2] << 16) |
                                                        ((uint32_t)Data[1] << 8)  |
                                                        ((uint32_t)Data[0]));
   CAN1->sTxMailBox[transmit_mailbox].TXMDHR =         (((uint32_t)Data[7] << 24)|
                                                       ((uint32_t)Data[6] << 16) |
                                                       ((uint32_t)Data[5] << 8)  |
                                                       ((uint32_t)Data[4]));
   CAN1->sTxMailBox[transmit_mailbox].TXMIR |= TMIDxR_TXRQ;
   return (transmit_mailbox);

}

INIT_FUNC_LOC void HAL_CANSetFiters(uint8_t filter_index, uint32_t f1,uint32_t f2,uint32_t f3,uint32_t f4, HAL_CAN_FILTER_FIFO_t FIFO)
{
    if  ( (f1!=0) || (f2!=0) || (f3!=0) || (f4!=0))
    {
        u16 CAN_FilterIdLow       = f1 <<5;
        u16 CAN_FilterIdHigh      = f3 <<5;
        u16 CAN_FilterMaskIdLow   = f2 <<5;
        u16 CAN_FilterMaskIdHigh  = f4 <<5;
        uint16_t CAN_FilterFIFOAssignment =  (FIFO  == FILTER_FIFO_0) ?  CAN_Filter_FIFO0 :  CAN_Filter_FIFO1 ;
        uint32_t filter_number_bit_pos = 0;
        filter_number_bit_pos = ((uint32_t)1) << filter_index;
        CAN1->FCTLR |= FCTLR_FINIT;
        CAN1->FWR &= ~(uint32_t)filter_number_bit_pos;
        CAN1->FSCFGR &= ~(uint32_t)filter_number_bit_pos;
        CAN1->sFilterRegister[filter_index].FR1 =
            ((0x0000FFFF & (uint32_t)CAN_FilterMaskIdLow) << 16) |
                    (0x0000FFFF & (uint32_t)CAN_FilterIdLow);
            CAN1->sFilterRegister[filter_index].FR2 =
            ((0x0000FFFF & (uint32_t)CAN_FilterMaskIdHigh) << 16) |
                    (0x0000FFFF & (uint32_t)CAN_FilterIdHigh);
        #if defined (CH32V20x_D6)||defined (CH32V20x_D8)
        if(((*(uint32_t *) 0x40022030) & 0x0F000000) == 0)
        {
            uint32_t i;
            for(i = 0; i < 64; i++)
            {
                *(__IO uint16_t *)(0x40006000 + 512 + 4 * i) = *(__IO uint16_t *)(0x40006000 + 768 + 4 * i);
            }
        }
        #endif
        CAN1->FMCFGR |= (uint32_t)filter_number_bit_pos;
        if (CAN_FilterFIFOAssignment == CAN_Filter_FIFO0)
        {
            CAN1->FAFIFOR &= ~(uint32_t)filter_number_bit_pos;
        }
        else
        {
            CAN1->FAFIFOR |= (uint32_t)filter_number_bit_pos;
        }
        CAN1->FWR |= filter_number_bit_pos;
        CAN1->FCTLR &= ~FCTLR_FINIT;
    }
    else 
    {
      HAL_CANResetFiltesr(filter_index);
    }
}


INIT_FUNC_LOC void HAL_CANInitIDInactive(uint8_t filter_index, HAL_CAN_FILTER_FIFO_t FIFO)
{
    
    {
        u16 CAN_FilterIdLow       = 0;
        u16 CAN_FilterIdHigh      = 0;
        u16 CAN_FilterMaskIdLow   = 0;
        u16 CAN_FilterMaskIdHigh  = 0;
        uint16_t CAN_FilterFIFOAssignment =  (FIFO  == FILTER_FIFO_0) ?  CAN_Filter_FIFO0 :  CAN_Filter_FIFO1 ;
        uint32_t filter_number_bit_pos = 0;
        filter_number_bit_pos = ((uint32_t)1) << filter_index;
        CAN1->FCTLR |= FCTLR_FINIT;
        CAN1->FWR &= ~(uint32_t)filter_number_bit_pos;
        CAN1->FSCFGR &= ~(uint32_t)filter_number_bit_pos;
        CAN1->sFilterRegister[filter_index].FR1 =
            ((0x0000FFFF & (uint32_t)CAN_FilterMaskIdLow) << 16) |
                    (0x0000FFFF & (uint32_t)CAN_FilterIdLow);
            CAN1->sFilterRegister[filter_index].FR2 =
            ((0x0000FFFF & (uint32_t)CAN_FilterMaskIdHigh) << 16) |
                    (0x0000FFFF & (uint32_t)CAN_FilterIdHigh);
        #if defined (CH32V20x_D6)||defined (CH32V20x_D8)
        if(((*(uint32_t *) 0x40022030) & 0x0F000000) == 0)
        {
            uint32_t i;
            for(i = 0; i < 64; i++)
            {
                *(__IO uint16_t *)(0x40006000 + 512 + 4 * i) = *(__IO uint16_t *)(0x40006000 + 768 + 4 * i);
            }
        }
        #endif
        CAN1->FMCFGR |= (uint32_t)filter_number_bit_pos;
        if (CAN_FilterFIFOAssignment == CAN_Filter_FIFO0)
        {
            CAN1->FAFIFOR &= ~(uint32_t)filter_number_bit_pos;
        }
        else
        {
            CAN1->FAFIFOR |= (uint32_t)filter_number_bit_pos;
        }
        CAN1->FWR  &= ~filter_number_bit_pos;
        CAN1->FCTLR &= ~FCTLR_FINIT;
    }
   
}


/*
Функция выключения фильтра
*/
INIT_FUNC_LOC void HAL_CANResetFiltesr( uint8_t filter_index)
{
    uint32_t filter_number_bit_pos = ((uint32_t)1) << filter_index;;
    CAN1->FCTLR |= FCTLR_FINIT;
    CAN1->FWR &= ~(uint32_t)filter_number_bit_pos;
    CAN1->FCTLR &= ~FCTLR_FINIT;
}


INIT_FUNC_LOC void HAL_CANSetFitersEX(uint8_t filter_index, uint32_t f1,uint32_t f2, HAL_CAN_FILTER_FIFO_t FIFO)
{
    if ((f1 !=0) || (f2!=0))
    {
        u16 CAN_FilterId1Low       = ((f1 <<3) | 0x04) & 0xFFFF;
        u16 CAN_FilterId1High  = (f1 >>13) & 0xFFFF;
        u16 CAN_FilterId2Low   = ((f2 <<3) | 0x04) & 0xFFFF;
        u16 CAN_FilterId2High      = (f2 >>13) & 0xFFFF;
        uint16_t CAN_FilterFIFOAssignment =  (FIFO  == FILTER_FIFO_0) ?  CAN_Filter_FIFO0 :  CAN_Filter_FIFO1 ;
        uint32_t filter_number_bit_pos = 0;
        filter_number_bit_pos = ((uint32_t)1) << filter_index;
        CAN1->FCTLR |= FCTLR_FINIT;
        CAN1->FWR &= ~(uint32_t)filter_number_bit_pos;
        CAN1->FSCFGR |= filter_number_bit_pos;
        CAN1->sFilterRegister[filter_index].FR1 =
            ((0x0000FFFF & (uint32_t)CAN_FilterId1High) << 16) |
                    (0x0000FFFF & (uint32_t)CAN_FilterId1Low ) ;
            CAN1->sFilterRegister[filter_index].FR2 =
            ((0x0000FFFF & (uint32_t)CAN_FilterId2High) << 16) |
                    (0x0000FFFF & (uint32_t)CAN_FilterId2Low);
        #if defined (CH32V20x_D6)||defined (CH32V20x_D8)
        if(((*(uint32_t *) 0x40022030) & 0x0F000000) == 0)
        {
            uint32_t i;
            for(i = 0; i < 64; i++)
            {
                *(__IO uint16_t *)(0x40006000 + 512 + 4 * i) = *(__IO uint16_t *)(0x40006000 + 768 + 4 * i);
            }
        }
        #endif
        CAN1->FMCFGR |= (uint32_t)filter_number_bit_pos;
        if (CAN_FilterFIFOAssignment == CAN_Filter_FIFO0)
        {
            CAN1->FAFIFOR &= ~(uint32_t)filter_number_bit_pos;
        }
        else
        {
            CAN1->FAFIFOR |= (uint32_t)filter_number_bit_pos;
        }
        CAN1->FWR |= filter_number_bit_pos;
        CAN1->FCTLR &= ~FCTLR_FINIT;
    }
    else
     {
        HAL_CANResetFiltesr(filter_index);
     }
}


HAL_CAN_ERROR_t HAL_CAN_MSG_GET( HAL_CAN_RX_FIFO_NUMBER_t fifo,  CAN_FRAME_TYPE * rx_message )
{
    if (((uint8_t)0x04 & CAN1->sFIFOMailBox[fifo ].RXMIR )== 0x04)
    {
        rx_message->ident = (uint32_t)0x1FFFFFFF & (CAN1->sFIFOMailBox[fifo].RXMIR >> 3);
        rx_message->extd = 1;
    }
    else
    {
        rx_message->ident = (uint32_t)0x000007FF & (CAN1->sFIFOMailBox[fifo ].RXMIR >> 21);
        rx_message->extd = 0;
    }
    if  (((uint8_t)0x02 & CAN1->sFIFOMailBox[fifo ].RXMIR )== 0x00)
    {
        rx_message->rtr = 0;
        rx_message->DLC = (uint8_t)0x0F & CAN1->sFIFOMailBox[fifo ].RXMDTR;
        rx_message->data[0] = (uint8_t)0xFF &  CAN1->sFIFOMailBox[fifo ].RXMDLR;
        rx_message->data[1] = (uint8_t)0xFF & (CAN1->sFIFOMailBox[fifo ].RXMDLR >> 8);
        rx_message->data[2] = (uint8_t)0xFF & (CAN1->sFIFOMailBox[fifo ].RXMDLR >> 16);
        rx_message->data[3] = (uint8_t)0xFF & (CAN1->sFIFOMailBox[fifo ].RXMDLR >> 24);
        rx_message->data[4] = (uint8_t)0xFF &  CAN1->sFIFOMailBox[fifo ].RXMDHR;
        rx_message->data[5] = (uint8_t)0xFF & (CAN1->sFIFOMailBox[fifo ].RXMDHR >> 8);
        rx_message->data[6] = (uint8_t)0xFF & (CAN1->sFIFOMailBox[fifo ].RXMDHR >> 16);
        rx_message->data[7] = (uint8_t)0xFF & (CAN1->sFIFOMailBox[fifo ].RXMDHR >> 24);
    }
    else
    {
        rx_message->rtr = 1 ;
        rx_message->DLC = 0;
    }
    rx_message->filter_id = (uint8_t)0xFF & (CAN1->sFIFOMailBox[fifo].RXMDTR >> 8);
    if (fifo == CAN_FIFO0)
        CAN1->RFIFO0 |= CAN_RFIFO0_RFOM0;
    else
        CAN1->RFIFO1 |= CAN_RFIFO1_RFOM1;

    return (HAL_CAN_OK);
}


/*
 * Обработчик прерывания по отправки данных
 */
void   USB_HP_CAN1_TX_IRQHandler(void)
{
    //Сбрасываем флаг прерывания по всем mainbox. В данном драйвере нам нет необходимости отслеживать
    // что мы передали, а что нет.
    CAN1->TSTATR = CAN_TSTATR_RQCP0|CAN_TSTATR_RQCP1|CAN_TSTATR_RQCP2;
    CAN.txcallback();

}
/*
 * Обработчик прерывания по приему в данных в Fifo 0
 */
void   USB_LP_CAN1_RX0_IRQHandler(void)
{


	CAN.rxcallback( HAL_RX_FIFO0);
	CAN1->RFIFO0 = CAN_RFIFO0_FULL0;  //Сбрасываем флаг прерывания

}
/*
 * Обработчик прерывания по приему в данных в Fifo 1
 */
void   CAN1_RX1_IRQHandler(void)
{

	CAN.rxcallback( HAL_RX_FIFO1);
	CAN1->RFIFO1 = CAN_RFIFO1_FULL1;  //Сбрасываем флаг прерывания

}
/*
 * Обработчик прерывния общики CAN
 */
void   CAN1_SCE_IRQHandler(void)
{

    CAN1->STATR = CAN_STATR_ERRI;   //Сбрасываем флаг прерывания
    CAN.errorcallback();
}



