/*
 * init.h
 *
 *  Created on: Dec 11, 2024
 *      Author: i.dymov
 */

#ifndef USER_INIT_H_
#define USER_INIT_H_

#include "main.h"
#include "hal_typedef.h"

#define USART_PRIOR     1
#define USART_SUB_PRIOR 2

#define CAN_RX_Pin                  GPIO_8
#define CAN_TX_Pin                  GPIO_9
#define CAN_Port                    PORT_B
#define RS_TX_Pin                   GPIO_9
#define RS_RX_Pin                   GPIO_10
#define RS_Port                     PORT_A
#define KL1Pin   GPIO_5
#define KL1_Port PORT_A
#define KL2_8_Port PORT_B
#define KL2Pin GPIO_1
#define KL3Pin GPIO_2
#define KL4Pin GPIO_3
#define KL5Pin GPIO_4
#define KL6Pin GPIO_5
#define KL7Pin GPIO_6
#define KL8Pin GPIO_7
#define CSPin GPIO_12
#define CS_Port PORT_B
#define SPI_Port PORT_B
#define SPI_MISO_Pin GPIO_14
#define SPI_MOSI_Pin GPIO_15
#define SPI_SCK_Pin GPIO_13
#define TIM2_CH1_2_Port PORT_A
#define TIM2_CH1_Pin GPIO_0
#define TIM2_CH2_Pin GPIO_1
#define TIM2_CH3_Port PORT_B
#define TIM2_CH3_Pin GPIO_10

void InitDevice();

#endif /* USER_INIT_H_ */
