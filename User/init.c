/*
 * init.c
 *
 *  Created on: Dec 11, 2024
 *      Author: i.dymov
 */

#include "init.h"
#include "hal_gpio.h"
#include "hal_timers.h"
#include "led_task.h"
#include "hal_spi.h"
#include "hal_wdt.h"

INIT_FUNC_LOC void InitDevice()
{
    HAL_InitGPO();
    HAL_InitGpioAF(CAN_Port,CAN_TX_Pin,GPIO_Remap1_CAN1,GPIO_Mode_AF_PP);
    HAL_InitGpioAF(CAN_Port,CAN_RX_Pin,GPIO_Remap1_CAN1,GPIO_Mode_IPU);
    HAL_InitGpioAF(RS_Port,RS_TX_Pin,0,GPIO_Mode_AF_PP);
    HAL_InitGpioIn(RS_Port,RS_RX_Pin);
    HAL_InitGpioOut( CS_Port, CSPin);
    HAL_InitGpioAIN(SPI_Port, SPI_MISO_Pin) ;
    HAL_InitGpioAF(SPI_Port,SPI_MOSI_Pin |  SPI_SCK_Pin ,0,GPIO_Mode_AF_PP);
    HAL_InitGpioOut(TIM2_CH1_2_Port,TIM2_CH1_Pin |  TIM2_CH2_Pin); 
    HAL_InitGpioOut(TIM2_CH3_Port  ,TIM2_CH3_Pin);
    HAL_SPI_InitDMA(HAL_SPI2, SPI_8bit);
    HW_TIMER_TimerInit(TIMER1,100000,10000);
    vLedDriverStart();
    HAL_WDTInit();
}

