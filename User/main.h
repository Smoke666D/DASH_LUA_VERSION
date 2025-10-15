/*
 * main.h
 *
 *  Created on: Dec 11, 2024
 *      Author: i.dymov
 */

#ifndef USER_MAIN_H_
#define USER_MAIN_H_

#include "system_init.h"
#include "system_config.h"

#define __SECTION(x)

#define V1  1
#define V2  2
#define V3  4

#define DEBUG_PRINT
#define LUA_FUNC_LOC  INIT_FUNC_LOC


#define ADC_Port            PORT_A
#define ADC0_Pin            GPIO_0
#define ADC1_Pin            GPIO_1
#define ADC2_Pin            GPIO_2
#define ADC3_Pin            GPIO_3
#define SPI1_Port           PORT_A
#define SPI1_NSS_Pin        GPIO_4
#define SPI1_SCK_Pin        GPIO_5
#define ADC6_Pin            GPIO_6
#define SPI1_MOSI_Pin       GPIO_7
#define nOE_Port           PORT_B
#define nOE1_Pin            GPIO_0
#define nOE2_Pin            GPIO_1
#define Din3_4_5_Port       PORT_B
#define Din3_Pin            GPIO_10
#define Din4_Pin            GPIO_11
#define SPI2_NSS_Pin        GPIO_12
#define SPI2_Port           PORT_B
#define SPI2_SCK_Pin        GPIO_13
#define SPI2_MOSI_Pin       GPIO_15
#define UARTTX_Port         PORT_A
#define UARTTX_Pin          GPIO_9
#define Din1_Port           PORT_A
#define Din1_Pin            GPIO_11
#define Din2_Port           PORT_B
#define Din2_Pin            GPIO_3
#define PowerOn_Port        PORT_B
#define PowerOn_Pin         GPIO_4
#define Din5_Pin            GPIO_5
#define I2C1_Port          PORT_B
#define I2C1_SCL_Pin       GPIO_6
#define I2C1_SDA_Pin       GPIO_7
#define CAN_Port            PORT_B
#define CAN_RX_Pin          GPIO_8
#define CAN_TX_Pin          GPIO_9





//#define TIM1_UP_ENABLE
//#define TIM2_UP_ENABLE
//#define TIM3_UP_ENABLE
#define TIM4_UP_ENABLE

#define ADC_1_2_ENABLE
/***************************watchdog***************************/
//Активировать ватчдог
#define WDT_ENABLE
/****************************RTC**************************/
#define RTC_IRQ_ENABLE
/************************I2C************************************/
#define I2C1_ENABLE
#define I2C1_IT_ENABLE
//#define I2C2_IT_ENABLE


//#define OD_ATTR_OD const


#define ADC_1_ENABLE  1
#define ADC_2_ENABLE  0
#define ADC_CHANNEL_NUMBER_0_9   1
#define ADC_CHANNEL_NUMBER_10_16 0
#define ADC_CHANNELRANK_0_6  1
#define ADC_CHANNELRANK_7_12  0
#define ADC_CHANNELRANK_13_16 0

#define DMA1_CH1_ENABLE 1
#define DMA1_CH2_ENABLE 0
#define DMA1_CH3_ENABLE 1
#define DMA1_CH4_ENABLE 1
#define DMA1_CH5_ENABLE 1
#define DMA1_CH6_ENABLE 0
#define DMA1_CH7_ENABLE 1


#define RESTART_DISABLE  0x0001
#define DATA_MODEL_READY 0x02
#define IGNITON_FLAG    0x08
#define SPI1_READY      0x10
#define SPI2_READY      0x20
#define KEYBOARD_START  0x40
#endif /* USER_MAIN_H_ */
