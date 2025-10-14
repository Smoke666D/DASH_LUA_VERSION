/*
 * hal_can.h
 *
 *  Created on: 13 灞戦偑瑜� 2024 璋�.
 *      Author: i.dymov
 */

#ifndef HAL_HAL_CAN_H_
#define HAL_HAL_CAN_H_

#include "system_config.h"



/* CAN masks for identifiers */
#define CAN_SFID_MASK                      ((uint32_t)0x000007FFU)      /*!< mask of standard identifier */
#define CANID_MASK                              0x07FF  /*!< CAN standard ID mask */
#define FLAG_RTR                                0x8000  /*!< RTR flag, part of identifier */
#define CAN_STD_ID_TYPE  0x00
#define CAN_EXTD_ID_TYPE 0x01
#define CAN_DATA_TYPE    0x00
#define CAN_RTR_TYPE     0x02

typedef enum 
{
  CAN_1MBS =   0,
  CAN_800KBS = 0,
  CAN_500KBS = 1,
  CAN_250KBS = 2,
  CAN_125KBS = 3,
  CAN_100KBS = 4,
  CAN_50KBS =  5,
  CAN_20KBS =  6,
  CAN_10KBS =  7,
} CAN_BOUNDRATE;

typedef enum
{
	FILTER_FIFO_0,
	FILTER_FIFO_1,
} HAL_CAN_FILTER_FIFO_t;


#if CORE == WCH32V2 || CORE == WCH32V3
typedef enum
{
	HAL_RX_FIFO0 =  CAN_FIFO0,
	HAL_RX_FIFO1 =  CAN_FIFO1,
} HAL_CAN_RX_FIFO_NUMBER_t;
#endif


#define   HAL_CAN_STD_ID        ( CAN_STD_ID_TYPE  | CAN_DATA_TYPE )
#define   HAL_CAN_EXTD_ID       ( CAN_EXTD_ID_TYPE | CAN_DATA_TYPE )
#define   HAL_CAN_STR_ID_RTR    ( CAN_STD_ID_TYPE  | CAN_RTR_TYPE  )
#define   HAL_CAN_EXTD_ID_RTR   ( CAN_EXTD_ID_TYPE | CAN_RTR_TYPE )


/* Transmit message object */
typedef struct {
    uint8_t DLC;
    uint8_t data[8];
    uint32_t ident;
    uint8_t rtr:1;
    uint8_t extd:1;
} CAN_TX_FRAME_TYPE;


typedef enum
{
  HAL_CAN_RTR = 1,
  HAL_CAN_DATA =0,

} HAL_CAN_MSG_TYPE;


typedef struct {
  uint8_t filter_id;
  uint8_t  DLC;
  uint8_t  data[8];
	uint32_t ident;
  uint8_t rtr:1;
  uint8_t extd:1;
} CAN_FRAME_TYPE;


typedef enum
{
  HAL_CAN_OK,
  HAL_CAN_ERROR,
} HAL_CAN_ERROR_t;


typedef struct
{
  void (* rxcallback) ( HAL_CAN_RX_FIFO_NUMBER_t);
  void (* txcallback) ( void );
  void (* errorcallback)(void );
} HAL_CAN_t;


void   USB_HP_CAN1_TX_IRQHandler(void);
void   USB_LP_CAN1_RX0_IRQHandler(void);
void   CAN1_RX1_IRQHandler(void);
void   CAN1_SCE_IRQHandler(void);

INIT_FUNC_LOC void HAL_CANInitIDInactive(uint8_t filter_index, HAL_CAN_FILTER_FIFO_t FIFO);
void HAL_CANInt(  uint8_t   CANbitRate);
void HAL_CANResetFiltesr( uint8_t filter_index);
HAL_CAN_ERROR_t HAL_CAN_MSG_GET( HAL_CAN_RX_FIFO_NUMBER_t fifo,  CAN_FRAME_TYPE * rx_message );
void HAL_CANSetTXCallback(void (* f) ( void ));
void HAL_CANSetRXCallback(void (* f) ( HAL_CAN_RX_FIFO_NUMBER_t));
void HAL_CANSetERRCallback(void (* f) ( void ));
void HAL_CANIntIT(   CAN_BOUNDRATE    CANbitRate, uint8_t prior, uint8_t subprior);
uint8_t HAL_CANToInitMode();
uint8_t HAL_CANToOperatingMode();
uint8_t HAL_CANSend(CAN_TX_FRAME_TYPE *buffer);
void HAL_CANSetFiters(uint8_t filter_index, uint32_t f1,uint32_t f2,uint32_t f3,uint32_t f4, HAL_CAN_FILTER_FIFO_t FIFO);
void HAL_CANSetFitersEX(uint8_t filter_index, uint32_t f1,uint32_t f2, HAL_CAN_FILTER_FIFO_t FIFO);
#endif /* HAL_HAL_CAN_H_ */
